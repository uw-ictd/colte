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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "auc.h"
#include "hss_config.h"
#include "log.h"

extern hss_config_t                     hss_config;
extern uint8_t                          op[16];

uint8_t                                *
sqn_ms_derive (
  const uint8_t const opc[16],
  uint8_t * key,
  uint8_t * auts,
  uint8_t * rand_p)
{
  /*
   * AUTS = Conc(SQN MS ) || MAC-S
   * * * * Conc(SQN MS ) = SQN MS ^ f5* (RAND)
   * * * * MAC-S = f1* (SQN MS || RAND || AMF)
   */
  uint8_t                                 ak[6] = {0};
  uint8_t                                *conc_sqn_ms = NULL;
  uint8_t                                *mac_s       = NULL;
  uint8_t                                 mac_s_computed[MAC_S_LENGTH] = {0};
  uint8_t                                *sqn_ms = NULL;
  uint8_t                                 amf[2] = { 0, 0 };
  int                                     i = 0;

  conc_sqn_ms = auts;
  mac_s = &auts[6];
  sqn_ms = malloc (SQN_LENGTH_OCTEST);
  /*
   * if (hss_config.valid_opc == 0) {
   * SetOP(hss_config.operator_key);
   * }
   */
  /*
   * Derive AK from key and rand
   */
  f5star (opc, key, rand_p, ak);

  for (i = 0; i < 6; i++) {
    sqn_ms[i] = ak[i] ^ conc_sqn_ms[i];
  }

  print_buffer ("sqn_ms_derive() KEY    : ", key, 16);
  print_buffer ("sqn_ms_derive() RAND   : ", rand_p, 16);
  print_buffer ("sqn_ms_derive() AUTS   : ", auts, 14);
  print_buffer ("sqn_ms_derive() AK     : ", ak, 6);
  print_buffer ("sqn_ms_derive() SQN_MS : ", sqn_ms, 6);
  print_buffer ("sqn_ms_derive() MAC_S  : ", mac_s, 8);
  f1star (opc, key, rand_p, sqn_ms, amf, mac_s_computed);
  print_buffer ("MAC_S +: ", mac_s_computed, 8);

  if (memcmp (mac_s_computed, mac_s, 8) != 0) {
    FPRINTF_ERROR ( "Failed to verify computed SQN_MS\n");
    free (sqn_ms);
    return NULL;
  }

  return sqn_ms;
}
