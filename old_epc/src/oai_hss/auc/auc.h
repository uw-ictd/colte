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

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>

#ifndef AUC_H_
#define AUC_H_

#define SQN_LENGTH_BITS   (48)
#define SQN_LENGTH_OCTEST (SQN_LENGTH_BITS/8)
#define IK_LENGTH_BITS   (128)
#define IK_LENGTH_OCTETS (IK_LENGTH_BITS/8)
#define RAND_LENGTH_OCTETS  (16)
#define RAND_LENGTH_BITS    (RAND_LENGTH_OCTETS*8)
#define XRES_LENGTH_OCTETS  (8)
#define AUTN_LENGTH_OCTETS  (16)
#define KASME_LENGTH_OCTETS (32)
#define MAC_S_LENGTH        (8)

extern uint8_t opc[16];

typedef mpz_t random_t;
typedef mpz_t sqn_t;

typedef uint8_t uint8_t;

typedef struct {
  uint8_t rand[16];
  uint8_t rand_new;
  uint8_t xres[8];
  uint8_t autn[16];
  uint8_t kasme[32];
} auc_vector_t;

void RijndaelKeySchedule(const uint8_t const key[16]);
void RijndaelEncrypt(const uint8_t const in[16], uint8_t out[16]);

/* Sequence number functions */
struct sqn_ue_s;
struct sqn_ue_s *sqn_exists(uint64_t imsi);
void sqn_insert(struct sqn_ue_s *item);
void sqn_init(struct sqn_ue_s *item);
struct sqn_ue_s *sqn_new(uint64_t imsi);
void sqn_list_init(void);
void sqn_get(uint64_t imsi, uint8_t sqn[6]);

/* Random number functions */
struct random_state_s;
void random_init(void);
void generate_random(uint8_t *random, ssize_t length);

//void SetOP(char *opP);

void ComputeOPc( const uint8_t const kP[16], const uint8_t const opP[16], uint8_t opcP[16] );

void f1 ( const uint8_t const kP[16],const uint8_t const k[16], const uint8_t const rand[16], const uint8_t const sqn[6], const uint8_t const amf[2],
          uint8_t mac_a[8] );
void f1star( const uint8_t const kP[16],const uint8_t const k[16], const uint8_t const rand[16], const uint8_t const sqn[6], const uint8_t const amf[2],
             uint8_t mac_s[8] );
void f2345 ( const uint8_t const kP[16],const uint8_t const k[16], const uint8_t const rand[16],
             uint8_t res[8], uint8_t ck[16], uint8_t ik[16], uint8_t ak[6] );
void f5star( const uint8_t const kP[16],const uint8_t const k[16], const uint8_t const rand[16],
             uint8_t ak[6] );

void generate_autn(const uint8_t const sqn[6], const uint8_t const ak[6], const uint8_t const amf[2], const uint8_t const mac_a[8], uint8_t autn[16]);
int generate_vector(const uint8_t const opc[16], uint64_t imsi, uint8_t key[16], uint8_t plmn[3],
                    uint8_t sqn[6], auc_vector_t *vector);

void kdf(uint8_t *key, uint16_t key_len, uint8_t *s, uint16_t s_len, uint8_t *out,
         uint16_t out_len);

void derive_kasme(uint8_t ck[16], uint8_t ik[16], uint8_t plmn[3], uint8_t sqn[6],
                  uint8_t ak[6], uint8_t kasme[32]);

uint8_t *sqn_ms_derive(const uint8_t const opc[16], uint8_t *key, uint8_t *auts, uint8_t *rand);

static inline void print_buffer(const char *prefix, uint8_t *buffer, int length)
{
  int i;

  fprintf(stdout, "%s", prefix);

  for (i = 0; i < length; i++) {
    fprintf(stdout, "%02x.", buffer[i]);
  }

  fprintf(stdout, "\n");
}

#endif /* AUC_H_ */
