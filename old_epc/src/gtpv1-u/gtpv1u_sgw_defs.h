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
#ifndef FILE_GTPV1U_SGW_DEFS_SEEN
#define FILE_GTPV1U_SGW_DEFS_SEEN

#define GTPV1U_UDP_PORT (2152)

#define MAX_BEARERS_PER_UE (11)

typedef enum {
  BEARER_DOWN = 0,
  BEARER_IN_CONFIG,
  BEARER_UP,
  BEARER_DL_HANDOVER,
  BEARER_UL_HANDOVER,
  BEARER_MAX,
} bearer_state_t;

#define BUFFER_TO_uint32_t(buf, x) \
do {                            \
    x = ((uint32_t)((buf)[0]) ) |      \
        ((uint32_t)((buf)[1]) << 8) |      \
        ((uint32_t)((buf)[2]) << 16)  |      \
        ((uint32_t)((buf)[3]) << 24);             \
} while(0)



typedef struct gtpv1u_teid2enb_info_s {
  /* TEID used in dl and ul */
  uint32_t       teid_enb;         ///< Remote eNB TEID
  ip_address_t   enb_ip_addr;
  bearer_state_t state;
  uint16_t       port; /// LG ???
} gtpv1u_teid2enb_info_t;


typedef struct {
  /* RB tree of UEs */
  //RB_HEAD(gtpv1u_ue_map, gtpv1u_ue_data_s) gtpv1u_ue_map_head;
  /* Local IP address to use */
  ipv4_nbo_t            sgw_ip_address_for_S1u_S12_S4_up;
  char                 *ip_addr;

  uint16_t              seq_num;
  uint8_t               restart_counter;
  //gtpv1u_teid2enb_info_t* teid2enb_mapping[];
  hash_table_t         *S1U_mapping;

  // GTP-U kernel interface
  pthread_t      reader_thread;
  int fd0;    /* GTP0 file descriptor */
  int fd1u;   /* GTP1-U user plane file descriptor */
} gtpv1u_data_t;


int gtpv1u_init (spgw_config_t *spgw_config);
void gtpv1u_exit (gtpv1u_data_t * const gtpv1u_data);

#endif /* FILE_GTPV1U_SGW_DEFS_SEEN */
