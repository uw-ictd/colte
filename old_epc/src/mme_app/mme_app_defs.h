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



/* This file contains definitions related to mme applicative layer and should
 * not be included within other layers.
 * Use mme_app_extern.h to expose mme applicative layer procedures/data.
 */


#ifndef FILE_MME_APP_DEFS_SEEN
#define FILE_MME_APP_DEFS_SEEN
#include "intertask_interface.h"
#include "mme_app_ue_context.h"

typedef struct {
  /* UE contexts + some statistics variables */
  mme_ue_context_t mme_ue_contexts;

  long statistic_timer_id;
  uint32_t statistic_timer_period;
  
  /* Reader/writer lock */
  pthread_rwlock_t rw_lock;
  
  /* ***************Statistics*************
   * number of attached UE,number of connected UE,
   * number of idle UE,number of default bearers, 
   * number of S1_U bearers,number of PDN sessions
   */ 
  
  uint32_t               nb_enb_connected;
  uint32_t               nb_ue_attached;
  uint32_t               nb_ue_connected;
  uint32_t               nb_default_eps_bearers;
  uint32_t               nb_s1u_bearers;
  
  /* ***************Changes in Statistics**************/

  uint32_t               nb_ue_attached_since_last_stat;
  uint32_t               nb_ue_detached_since_last_stat;
  uint32_t               nb_ue_connected_since_last_stat;
  uint32_t               nb_ue_disconnected_since_last_stat;
  uint32_t               nb_eps_bearers_established_since_last_stat;
  uint32_t               nb_eps_bearers_released_since_last_stat;
  uint32_t               nb_enb_connected_since_last_stat;
  uint32_t               nb_enb_released_since_last_stat;
  uint32_t               nb_s1u_bearers_released_since_last_stat;
  uint32_t               nb_s1u_bearers_established_since_last_stat;
} mme_app_desc_t;

extern mme_app_desc_t mme_app_desc;

int mme_app_handle_s1ap_ue_capabilities_ind  (const itti_s1ap_ue_cap_ind_t const * s1ap_ue_cap_ind_pP);

void mme_app_handle_s1ap_ue_context_release_complete (const itti_s1ap_ue_context_release_complete_t const
                                                       *s1ap_ue_context_release_complete);


int mme_app_send_s11_release_access_bearers_req (struct ue_context_s *const ue_context_pP);

int mme_app_send_s11_create_session_req      (struct ue_context_s * const ue_context_pP);

int mme_app_send_s6a_update_location_req     (struct ue_context_s * const ue_context_pP);

int mme_app_handle_s6a_update_location_ans   (const s6a_update_location_ans_t * const ula_pP);

int mme_app_handle_nas_pdn_connectivity_req  ( itti_nas_pdn_connectivity_req_t * const nas_pdn_connectivity_req_p);

void mme_app_handle_detach_req (const itti_nas_detach_req_t * const detach_req_p);

void mme_app_handle_conn_est_cnf             (const itti_nas_conn_est_cnf_t * const nas_conn_est_cnf_pP);

void mme_app_handle_initial_ue_message       (itti_mme_app_initial_ue_message_t * const conn_est_ind_pP);

int mme_app_handle_create_sess_resp          (itti_s11_create_session_response_t * const create_sess_resp_pP); //not const because we need to free internal stucts

void mme_app_handle_delete_session_rsp	     (const itti_s11_delete_session_response_t * const delete_sess_respP);

int mme_app_handle_establish_ind             (const nas_establish_ind_t * const nas_establish_ind_pP);

int mme_app_handle_authentication_info_answer(const s6a_auth_info_ans_t * const s6a_auth_info_ans_pP);

void  mme_app_handle_release_access_bearers_resp (const itti_s11_release_access_bearers_response_t * const rel_access_bearers_rsp_pP);

nas_cause_t s6a_error_2_nas_cause            (const uint32_t s6a_errorP, const int experimentalP);

void mme_app_handle_nas_auth_param_req       (const itti_nas_auth_param_req_t * const nas_auth_param_req_pP);

void mme_app_handle_initial_context_setup_rsp(const itti_mme_app_initial_context_setup_rsp_t * const initial_ctxt_setup_rsp_pP);

void mme_app_handle_initial_context_setup_failure(const itti_mme_app_initial_context_setup_failure_t * const initial_ctxt_setup_failure_pP);

bool mme_app_dump_ue_context (const hash_key_t keyP, void *const ue_context_pP, void *unused_param_pP, void **unused_result_pP);

int mme_app_handle_nas_dl_req ( itti_nas_dl_data_req_t *const nas_dl_req_pP);

void mme_ue_context_update_ue_sig_connection_state (mme_ue_context_t * const mme_ue_context_p,
                                                                            struct ue_context_s *ue_context_p,ecm_state_t new_ecm_state);

void mme_app_handle_mobile_reachability_timer_expiry (struct ue_context_s *ue_context_p);

void mme_app_handle_implicit_detach_timer_expiry (struct ue_context_s *ue_context_p); 

void mme_app_handle_initial_context_setup_rsp_timer_expiry (struct ue_context_s *ue_context_p);

#define mme_stats_read_lock(mMEsTATS)  pthread_rwlock_rdlock(&(mMEsTATS)->rw_lock)
#define mme_stats_write_lock(mMEsTATS) pthread_rwlock_wrlock(&(mMEsTATS)->rw_lock)
#define mme_stats_unlock(mMEsTATS)     pthread_rwlock_unlock(&(mMEsTATS)->rw_lock)

#endif /* MME_APP_DEFS_H_ */
