/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under 
 * the Apache License, Version 2.0  (the "License"); you may not use this file
 * except in compliance with the License.  
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <gmp.h>
#include <nettle/hmac.h>

#include "auc.h"
#include "hss_config.h"

#define DEBUG_AUC_KDF 1
extern hss_config_t                     hss_config;

/*
   @param key the input key
   @param key_len length of the key
   @param s string for key derivation as defined in 3GPP TS.33401 Annex A
   @param s_len length of s
   @param out buffer to place the output of kdf
   @param ou_len expected length for the output key
*/
inline void
kdf (
  uint8_t * key,
  uint16_t key_len,
  uint8_t * s,
  uint16_t s_len,
  uint8_t * out,
  uint16_t out_len)
{
  struct hmac_sha256_ctx                  ctx;

  memset (&ctx, 0, sizeof (ctx));
  hmac_sha256_set_key (&ctx, key_len, key);
  hmac_sha256_update (&ctx, s_len, s);
  hmac_sha256_digest (&ctx, out_len, out);
}

/*
   Derive the Kasme using the KDF (key derive function).
   See 3GPP TS.33401 Annex A.2
   The input String S to the KDF is composed of 14 bytes:
   FC = 0x10
   P0 = SN id = PLMN
   L0 = length(SN id) = 0x00 0x03
   P1 = SQN xor AK
   L1 = length(P1) = 0x00 0x06
*/
inline void
derive_kasme (
  uint8_t ck[16],
  uint8_t ik[16],
  uint8_t plmn[3],
  uint8_t sqn[6],
  uint8_t ak[6],
  uint8_t * kasme)
{
  uint8_t                                 s[14];
  int                                     i;
  uint8_t                                 key[32];

  /*
   * The input key is equal to the concatenation of CK and IK
   */
  memcpy (&key[0], ck, 16);
  memcpy (&key[16], ik, 16);
  /*
   * if (hss_config.valid_opc == 0) {
   * SetOP(hss_config.operator_key);
   * }
   */
  /*
   * FC
   */
  s[0] = 0x10;
  /*
   * SN id is composed of MCC and MNC
   * * * * Octets:
   * * * *   1      MCC digit 2 | MCC digit 1
   * * * *   2      MNC digit 3 | MCC digit 3
   * * * *   3      MNC digit 2 | MNC digit 1
   */
  memcpy (&s[1], plmn, 3);
  /*
   * L0
   */
  s[4] = 0x00;
  s[5] = 0x03;

  /*
   * P1
   */
  for (i = 0; i < 6; i++) {
    s[6 + i] = sqn[i] ^ ak[i];
  }

  /*
   * L1
   */
  s[12] = 0x00;
  s[13] = 0x06;
#if DEBUG_AUC_KDF

  for (i = 0; i < 32; i++)
    printf ("0x%02x ", key[i]);

  printf ("\n");

  for (i = 0; i < 14; i++)
    printf ("0x%02x ", s[i]);

  printf ("\n");
#endif
  kdf (key, 32, s, 14, kasme, 32);
}

int
generate_vector (
  const uint8_t const opc[16],
  uint64_t imsi,
  uint8_t key[16],
  uint8_t plmn[3],
  uint8_t sqn[6],
  auc_vector_t * vector)
{
  /*
   * in E-UTRAN an authentication vector is composed of:
   * * * * - RAND
   * * * * - XRES
   * * * * - AUTN
   * * * * - KASME
   */
  uint8_t                                 amf[] = { 0x80, 0x00 };
  uint8_t                                 mac_a[8];
  uint8_t                                 ck[16];
  uint8_t                                 ik[16];
  uint8_t                                 ak[6];

  if (vector == NULL) {
    return EINVAL;
  }

  /*
   * Compute MAC
   */
  f1 (opc, key, vector->rand, sqn, amf, mac_a);
  print_buffer ("MAC_A   : ", mac_a, 8);
  print_buffer ("SQN     : ", sqn, 6);
  print_buffer ("RAND    : ", vector->rand, 16);
  /*
   * Compute XRES, CK, IK, AK
   */
  f2345 (opc, key, vector->rand, vector->xres, ck, ik, ak);
  print_buffer ("AK      : ", ak, 6);
  print_buffer ("CK      : ", ck, 16);
  print_buffer ("IK      : ", ik, 16);
  print_buffer ("XRES    : ", vector->xres, 8);
  /*
   * AUTN = SQN ^ AK || AMF || MAC
   */
  generate_autn (sqn, ak, amf, mac_a, vector->autn);
  print_buffer ("AUTN    : ", vector->autn, 16);
  derive_kasme (ck, ik, plmn, sqn, ak, vector->kasme);
  print_buffer ("KASME   : ", vector->kasme, 32);
  return 0;
}
