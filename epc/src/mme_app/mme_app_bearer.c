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


/*! \file mme_app_bearer.c
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#include "bstrlib.h"

#include "dynamic_memory_check.h"
#include "log.h"
#include "msc.h"
#include "assertions.h"
#include "conversions.h"
#include "common_types.h"
#include "intertask_interface.h"
#include "mme_config.h"
#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_apn_selection.h"
#include "mme_app_pdn_context.h"
#include "mme_app_sgw_selection.h"
#include "mme_app_bearer_context.h"
#include "sgw_ie_defs.h"
#include "common_defs.h"
#include "gcc_diag.h"
#include "mme_app_itti_messaging.h"
#include "mme_app_procedures.h"
#include "mme_app_statistics.h"
#include "timer.h"
#include "nas_proc.h"


//----------------------------------------------------------------------------
static bool mme_app_construct_guti(const plmn_t * const plmn_p, const s_tmsi_t * const s_tmsi_p,  guti_t * const guti_p);
static void notify_s1ap_new_ue_mme_s1ap_id_association (struct ue_mm_context_s *ue_context_p);



//------------------------------------------------------------------------------
int
mme_app_handle_nas_pdn_connectivity_req (
  itti_nas_pdn_connectivity_req_t * const nas_pdn_connectivity_req_pP)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                 *ue_context_p = NULL;
  imsi64_t                                imsi64 = INVALID_IMSI64;
  int                                     rc = RETURNok;

  DevAssert (nas_pdn_connectivity_req_pP );
  IMSI_STRING_TO_IMSI64 ((char *)nas_pdn_connectivity_req_pP->imsi, &imsi64);
  OAILOG_DEBUG (LOG_MME_APP, "Received NAS_PDN_CONNECTIVITY_REQ from NAS Handling imsi " IMSI_64_FMT "\n", imsi64);

  if ((ue_context_p = mme_ue_context_exists_imsi (&mme_app_desc.mme_ue_contexts, imsi64)) == NULL) {
    MSC_LOG_EVENT (MSC_MMEAPP_MME, " NAS_PDN_CONNECTIVITY_REQ Unknown imsi " IMSI_64_FMT, imsi64);
    OAILOG_ERROR (LOG_MME_APP, "That's embarrassing as we don't know this IMSI\n");
    mme_ue_context_dump_coll_keys();
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }

  /*
   * Consider the UE authenticated
   */
  ue_context_p->imsi_auth = IMSI_AUTHENTICATED;

  rc =  mme_app_send_s11_create_session_req (ue_context_p, nas_pdn_connectivity_req_pP->pdn_cid);

  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
}



// sent by NAS
//------------------------------------------------------------------------------
void
mme_app_handle_conn_est_cnf (
  itti_nas_conn_est_cnf_t * const nas_conn_est_cnf_pP)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                    *ue_context_p = NULL;
  MessageDef                             *message_p = NULL;
  itti_mme_app_connection_establishment_cnf_t *establishment_cnf_p = NULL;

  OAILOG_DEBUG (LOG_MME_APP, "Received NAS_CONNECTION_ESTABLISHMENT_CNF from NAS ue " MME_UE_S1AP_ID_FMT "\n", nas_conn_est_cnf_pP->ue_id);
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, nas_conn_est_cnf_pP->ue_id);

  if (!ue_context_p) {
    MSC_LOG_EVENT (MSC_MMEAPP_MME, " NAS_CONNECTION_ESTABLISHMENT_CNF Unknown ue " MME_UE_S1AP_ID_FMT " ", nas_conn_est_cnf_pP->ue_id);
    OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist for UE " MME_UE_S1AP_ID_FMT "\n", nas_conn_est_cnf_pP->ue_id);
    // memory leak
    bdestroy_wrapper(&nas_conn_est_cnf_pP->nas_msg);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  message_p = itti_alloc_new_message (TASK_MME_APP, MME_APP_CONNECTION_ESTABLISHMENT_CNF);
  establishment_cnf_p = &message_p->ittiMsg.mme_app_connection_establishment_cnf;

  establishment_cnf_p->ue_id = nas_conn_est_cnf_pP->ue_id;

  // Copy UE radio capabilities into message if it exists
  OAILOG_DEBUG (LOG_MME_APP, "UE radio context already cached: %s\n",
               ue_context_p->ue_radio_capability ? "yes" : "no");
  if (ue_context_p->ue_radio_capability) {
    establishment_cnf_p->ue_radio_capability = bstrcpy(ue_context_p->ue_radio_capability);
  }

    int j = 0;
    for (int i = 0; i < BEARERS_PER_UE; i++) {
      bearer_context_t *bc = ue_context_p->bearer_contexts[i];
      if (bc) {
        if (BEARER_STATE_SGW_CREATED & bc->bearer_state) {
          establishment_cnf_p->e_rab_id[j]                                 = bc->ebi ;//+ EPS_BEARER_IDENTITY_FIRST;
          establishment_cnf_p->e_rab_level_qos_qci[j]                      = bc->qci;
          establishment_cnf_p->e_rab_level_qos_priority_level[j]           = bc->priority_level;
          establishment_cnf_p->e_rab_level_qos_preemption_capability[j]    = bc->preemption_capability;
          establishment_cnf_p->e_rab_level_qos_preemption_vulnerability[j] = bc->preemption_vulnerability;
          establishment_cnf_p->transport_layer_address[j]                  = fteid_ip_address_to_bstring(&bc->s_gw_fteid_s1u);
          establishment_cnf_p->gtp_teid[j]                                 = bc->s_gw_fteid_s1u.teid;
          if (!j) {
            establishment_cnf_p->nas_pdu[j]                                = nas_conn_est_cnf_pP->nas_msg;
            nas_conn_est_cnf_pP->nas_msg = NULL;
#if DEBUG_IS_ON
            if (!establishment_cnf_p->nas_pdu[j]) {
              OAILOG_ERROR (LOG_MME_APP, "No NAS PDU found ue " MME_UE_S1AP_ID_FMT "\n", nas_conn_est_cnf_pP->ue_id);
            }
#endif
          }
          j=j+1;
        }
      }
    }
    establishment_cnf_p->no_of_e_rabs = j;

  //#pragma message  "Check ue_context_p ambr"
    establishment_cnf_p->ue_ambr.br_ul = ue_context_p->suscribed_ue_ambr.br_ul;
    establishment_cnf_p->ue_ambr.br_dl = ue_context_p->suscribed_ue_ambr.br_dl;
    establishment_cnf_p->ue_security_capabilities_encryption_algorithms = nas_conn_est_cnf_pP->encryption_algorithm_capabilities;
    establishment_cnf_p->ue_security_capabilities_integrity_algorithms = nas_conn_est_cnf_pP->integrity_algorithm_capabilities;
    memcpy(establishment_cnf_p->kenb, nas_conn_est_cnf_pP->kenb, AUTH_KENB_SIZE);


    OAILOG_DEBUG (LOG_MME_APP, "security_capabilities_encryption_algorithms 0x%04X\n", establishment_cnf_p->ue_security_capabilities_encryption_algorithms);
    OAILOG_DEBUG (LOG_MME_APP, "security_capabilities_integrity_algorithms  0x%04X\n", establishment_cnf_p->ue_security_capabilities_integrity_algorithms);

    MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S1AP_MME, NULL, 0,
                        "0 MME_APP_CONNECTION_ESTABLISHMENT_CNF ebi %u s1u_sgw teid " TEID_FMT " qci %u prio level %u sea 0x%x sia 0x%x",
                        establishment_cnf_p->e_rab_id[0],
                        establishment_cnf_p->gtp_teid[0],
                        establishment_cnf_p->e_rab_level_qos_qci[0],
                        establishment_cnf_p->e_rab_level_qos_priority_level[0],
                        establishment_cnf_p->ue_security_capabilities_encryption_algorithms,
                        establishment_cnf_p->ue_security_capabilities_integrity_algorithms);

    itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);

  /*
   * Move the UE to ECM Connected State.However if S1-U bearer establishment fails then we need to move the UE to idle.
   * S1 Signaling connection gets established via first DL NAS Trasnport message in some scenarios so check the state
   * first 
   */
  if (ue_context_p->ecm_state != ECM_CONNECTED)  
  {
    mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts,ue_context_p,ECM_CONNECTED);

  }

  /* Start timer to wait for Initial UE Context Response from eNB
   * If timer expires treat this as failure of ongoing procedure and abort corresponding NAS procedure such as ATTACH
   * or SERVICE REQUEST. Send UE context release command to eNB
   */
  if (timer_setup (ue_context_p->initial_context_setup_rsp_timer.sec, 0, 
                TASK_MME_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT, (void *) &(ue_context_p->mme_ue_s1ap_id), &(ue_context_p->initial_context_setup_rsp_timer.id)) < 0) { 
    OAILOG_ERROR (LOG_MME_APP, "Failed to start initial context setup response timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  } else {
    OAILOG_DEBUG (LOG_MME_APP, "MME APP : Sent Initial context Setup Request and Started guard timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  }
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

// sent by S1AP
//------------------------------------------------------------------------------
void
mme_app_handle_initial_ue_message (
  itti_s1ap_initial_ue_message_t * const initial_pP)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                 *ue_context_p = NULL;
  bool                                    is_guti_valid = false;
  bool                                    is_mm_ctx_new = false;
  emm_context_t                          *ue_nas_ctx = NULL;
  enb_s1ap_id_key_t                       enb_s1ap_id_key = INVALID_ENB_UE_S1AP_ID_KEY;

  OAILOG_DEBUG (LOG_MME_APP, "Received MME_APP_INITIAL_UE_MESSAGE from S1AP\n");

  // Check if there is any existing UE context using S-TMSI/GUTI
  if (initial_pP->is_s_tmsi_valid) 
  {
    OAILOG_DEBUG (LOG_MME_APP, "INITIAL UE Message: Valid mme_code %u and S-TMSI %u received from eNB.\n",
                  initial_pP->opt_s_tmsi.mme_code, initial_pP->opt_s_tmsi.m_tmsi);
    guti_t guti = {.gummei.plmn = {0}, .gummei.mme_gid = 0, .gummei.mme_code = 0, .m_tmsi = INVALID_M_TMSI};
    plmn_t plmn = {.mcc_digit1 = initial_pP->tai.mcc_digit1,
            .mcc_digit2 = initial_pP->tai.mcc_digit2,
            .mcc_digit3 = initial_pP->tai.mcc_digit3,
            .mnc_digit1 = initial_pP->tai.mnc_digit1,
            .mnc_digit2 = initial_pP->tai.mnc_digit2,
            .mnc_digit3 = initial_pP->tai.mnc_digit3};
    is_guti_valid = mme_app_construct_guti(&plmn,&(initial_pP->opt_s_tmsi),&guti);
    if (is_guti_valid) {
      ue_nas_ctx = emm_context_get_by_guti (&_emm_data, &guti);
      if (ue_nas_ctx) {
        // Get the UE context using mme_ue_s1ap_id 
        ue_context_p =  PARENT_STRUCT(ue_nas_ctx, struct ue_mm_context_s, emm_context);
        DevAssert(ue_context_p != NULL);
        if (ue_context_p != NULL) {
          initial_pP->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
          if (ue_context_p->enb_s1ap_id_key != INVALID_ENB_UE_S1AP_ID_KEY)
          {
            /*
             * Ideally this should never happen. When UE move to IDLE this key is set to INVALID.
             * Note - This can happen if eNB detects RLF late and by that time UE sends Initial NAS message via new RRC
             * connection 
             * However if this key is valid, remove the key from the hashtable.
             */

            OAILOG_ERROR (LOG_MME_APP, "MME_APP_INITAIL_UE_MESSAGE.ERROR***** enb_s1ap_id_key %ld has valid value.\n" ,ue_context_p->enb_s1ap_id_key);
            hashtable_uint64_ts_remove (mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->enb_s1ap_id_key);
            ue_context_p->enb_s1ap_id_key = INVALID_ENB_UE_S1AP_ID_KEY;
          }
          // Update MME UE context with new enb_ue_s1ap_id
          ue_context_p->enb_ue_s1ap_id = initial_pP->enb_ue_s1ap_id;
          // regenerate the enb_s1ap_id_key as enb_ue_s1ap_id is changed.
          MME_APP_ENB_S1AP_ID_KEY(enb_s1ap_id_key, initial_pP->enb_id, initial_pP->enb_ue_s1ap_id);
          // Update enb_s1ap_id_key in hashtable  
          mme_ue_context_update_coll_keys( &mme_app_desc.mme_ue_contexts,
                ue_context_p,
                enb_s1ap_id_key,
                ue_context_p->mme_ue_s1ap_id,
                ue_nas_ctx->_imsi64,
                ue_context_p->mme_teid_s11,
                &guti);
        }
      } else {
          OAILOG_DEBUG (LOG_MME_APP, "MME_APP_INITIAL_UE_MESSAGE with mme code %u and S-TMSI %u:"
            "no UE context found \n", initial_pP->opt_s_tmsi.mme_code, initial_pP->opt_s_tmsi.m_tmsi);
      }
    } else {
      OAILOG_DEBUG (LOG_MME_APP, "No MME is configured with MME code %u received in S-TMSI %u from UE.\n",
                    initial_pP->opt_s_tmsi.mme_code, initial_pP->opt_s_tmsi.m_tmsi);
    }
  } else {
    OAILOG_DEBUG (LOG_MME_APP, "MME_APP_INITIAL_UE_MESSAGE from S1AP,without S-TMSI. \n");
  }
  // create a new ue context if nothing is found
  if (!(ue_context_p)) {
    OAILOG_DEBUG (LOG_MME_APP, "UE context doesn't exist -> create one\n");
    if (!(ue_context_p = mme_create_new_ue_context ())) {
      /*
       * Error during ue context malloc
       */
      DevMessage ("mme_create_new_ue_context");
      OAILOG_FUNC_OUT (LOG_MME_APP);
    }
    is_mm_ctx_new = true;
    // Allocate new mme_ue_s1ap_id
    ue_context_p->mme_ue_s1ap_id    = mme_app_ctx_get_new_ue_id ();
    if (ue_context_p->mme_ue_s1ap_id  == INVALID_MME_UE_S1AP_ID) {
      OAILOG_CRITICAL (LOG_MME_APP, "MME_APP_INITIAL_UE_MESSAGE. MME_UE_S1AP_ID allocation Failed.\n");
      mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context_p);
      OAILOG_FUNC_OUT (LOG_MME_APP);
    }
    OAILOG_DEBUG (LOG_MME_APP, "MME_APP_INITAIL_UE_MESSAGE.Allocated new MME UE context and new mme_ue_s1ap_id. %d\n",ue_context_p->mme_ue_s1ap_id);
    ue_context_p->enb_ue_s1ap_id    = initial_pP->enb_ue_s1ap_id;
    MME_APP_ENB_S1AP_ID_KEY(ue_context_p->enb_s1ap_id_key, initial_pP->enb_id, initial_pP->enb_ue_s1ap_id);
    DevAssert (mme_insert_ue_context (&mme_app_desc.mme_ue_contexts, ue_context_p) == 0);
  }
  ue_context_p->sctp_assoc_id_key = initial_pP->sctp_assoc_id;
  ue_context_p->e_utran_cgi = initial_pP->ecgi;
  // Notify S1AP about the mapping between mme_ue_s1ap_id and sctp assoc id + enb_ue_s1ap_id 
  notify_s1ap_new_ue_mme_s1ap_id_association (ue_context_p);
  // Initialize timers to INVALID IDs
  ue_context_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
  ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  ue_context_p->initial_context_setup_rsp_timer.sec = MME_APP_INITIAL_CONTEXT_SETUP_RSP_TIMER_VALUE;

  s_tmsi_t s_tmsi = {0};
  if (initial_pP->is_s_tmsi_valid) {
    s_tmsi        = initial_pP->opt_s_tmsi;
  } else {
    s_tmsi.mme_code = 0;
    s_tmsi.m_tmsi   = INVALID_M_TMSI;
  }

  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_INITIAL_UE_MESSAGE ue id " MME_UE_S1AP_ID_FMT " ", ue_context_p->mme_ue_s1ap_id);
  nas_proc_establish_ind (ue_context_p->mme_ue_s1ap_id,
      is_mm_ctx_new,
      initial_pP->tai,
      initial_pP->ecgi,
      initial_pP->rrc_establishment_cause,
      s_tmsi,
      &initial_pP->nas);
  //   s1ap_initial_ue_message_t transparent; may be needed :
  // OLD CODE memcpy (&message_p->ittiMsg.nas_initial_ue_message.transparent, (const void*)&initial_pP->transparent, sizeof (message_p->ittiMsg.nas_initial_ue_message.transparent));

  initial_pP->nas = NULL;

  unlock_ue_contexts(ue_context_p);

  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_erab_setup_req (itti_erab_setup_req_t * const itti_erab_setup_req)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                    *ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, itti_erab_setup_req->ue_id);

  if (!ue_context_p) {
    MSC_LOG_EVENT (MSC_MMEAPP_MME, " NAS_ERAB_SETUP_REQ Unknown ue " MME_UE_S1AP_ID_FMT " ", itti_erab_setup_req->ue_id);
    OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist for UE " MME_UE_S1AP_ID_FMT "\n", itti_erab_setup_req->ue_id);
    // memory leak
    bdestroy_wrapper(&itti_erab_setup_req->nas_msg);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  bearer_context_t* bearer_context = mme_app_get_bearer_context(ue_context_p, itti_erab_setup_req->ebi);

  if (bearer_context) {
    MessageDef  *message_p = itti_alloc_new_message (TASK_MME_APP, S1AP_E_RAB_SETUP_REQ);
    itti_s1ap_e_rab_setup_req_t *s1ap_e_rab_setup_req = &message_p->ittiMsg.s1ap_e_rab_setup_req;

    s1ap_e_rab_setup_req->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
    s1ap_e_rab_setup_req->enb_ue_s1ap_id = ue_context_p->enb_ue_s1ap_id;

    // E-RAB to Be Setup List
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.no_of_items = 1;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_id = bearer_context->ebi;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.allocation_and_retention_priority.pre_emption_capability =
      bearer_context->preemption_capability;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.allocation_and_retention_priority.pre_emption_vulnerability =
      bearer_context->preemption_vulnerability;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.allocation_and_retention_priority.priority_level =
      bearer_context->priority_level;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.gbr_qos_information.e_rab_maximum_bit_rate_downlink    = itti_erab_setup_req->mbr_dl;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.gbr_qos_information.e_rab_maximum_bit_rate_uplink      = itti_erab_setup_req->mbr_ul;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.gbr_qos_information.e_rab_guaranteed_bit_rate_downlink = itti_erab_setup_req->gbr_dl;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.gbr_qos_information.e_rab_guaranteed_bit_rate_uplink   = itti_erab_setup_req->gbr_ul;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_level_qos_parameters.qci = bearer_context->qci;

    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].gtp_teid = bearer_context->s_gw_fteid_s1u.teid;
    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].transport_layer_address = fteid_ip_address_to_bstring(&bearer_context->s_gw_fteid_s1u);

    s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].nas_pdu = itti_erab_setup_req->nas_msg;
    itti_erab_setup_req->nas_msg = NULL;

    MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S1AP_MME, NULL, 0, "0 S1AP_E_RAB_SETUP_REQ ue id " MME_UE_S1AP_ID_FMT " ebi %u teid " TEID_FMT " ",
      ue_context_p->mme_ue_s1ap_id,
      s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].e_rab_id,
      s1ap_e_rab_setup_req->e_rab_to_be_setup_list.item[0].gtp_teid);
    itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);
  } else {
    OAILOG_DEBUG (LOG_MME_APP, "No bearer context found ue " MME_UE_S1AP_ID_FMT  " ebi %u\n", itti_erab_setup_req->ue_id, itti_erab_setup_req->ebi);
  }
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_delete_session_rsp (
  const itti_s11_delete_session_response_t * const delete_sess_resp_pP)
//------------------------------------------------------------------------------
{
  struct ue_mm_context_s                 *ue_context_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (delete_sess_resp_pP );
  OAILOG_DEBUG (LOG_MME_APP, "Received S11_DELETE_SESSION_RESPONSE from S+P-GW with teid " TEID_FMT "\n ",delete_sess_resp_pP->teid);
  ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, delete_sess_resp_pP->teid);

  if (!ue_context_p) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 DELETE_SESSION_RESPONSE local S11 teid " TEID_FMT " ", delete_sess_resp_pP->teid);
    OAILOG_WARNING (LOG_MME_APP, "We didn't find this teid in list of UE: %08x\n", delete_sess_resp_pP->teid);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  hashtable_uint64_ts_remove(mme_app_desc.mme_ue_contexts.tun11_ue_context_htbl,
                      (const hash_key_t) ue_context_p->mme_teid_s11);
  ue_context_p->mme_teid_s11 = 0;

  if (delete_sess_resp_pP->cause.cause_value != REQUEST_ACCEPTED) {
    OAILOG_WARNING (LOG_MME_APP, "***WARNING****S11 Delete Session Rsp: NACK received from SPGW : %08x\n", delete_sess_resp_pP->teid);
  }
  MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 DELETE_SESSION_RESPONSE local S11 teid " TEID_FMT " IMSI " IMSI_64_FMT " ",
    delete_sess_resp_pP->teid, ue_context_p->emm_context._imsi64);
  /*
   * Updating statistics
   */
  update_mme_app_stats_s1u_bearer_sub();
  update_mme_app_stats_default_bearer_sub();
  
  /*
   * If UE is already in idle state, skip asking eNB to release UE context and just clean up locally.
   * This can happen during implicit detach and UE initiated detach when UE sends detach req (type = switch off)
   */
  if (ECM_IDLE == ue_context_p->ecm_state) {
    ue_context_p->ue_context_rel_cause = S1AP_IMPLICIT_CONTEXT_RELEASE;
    // Notify S1AP to release S1AP UE context locally.
    mme_app_itti_ue_context_release (ue_context_p, ue_context_p->ue_context_rel_cause);
    // Free MME UE Context   
    mme_notify_ue_context_released (&mme_app_desc.mme_ue_contexts, ue_context_p);
    OAILOG_DEBUG (LOG_MME_APP, "Deleting UE context associated in MME for mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n ",
        ue_context_p->mme_ue_s1ap_id);
    mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context_p);
    // return now, otherwize will unlock ue context already free
    OAILOG_FUNC_OUT (LOG_MME_APP);
  } else {
    if (ue_context_p->ue_context_rel_cause == S1AP_INVALID_CAUSE) {
      ue_context_p->ue_context_rel_cause = S1AP_NAS_DETACH;
    }
    // Notify S1AP to send UE Context Release Command to eNB or free s1 context locally.
    mme_app_itti_ue_context_release (ue_context_p, ue_context_p->ue_context_rel_cause);
    ue_context_p->ue_context_rel_cause = S1AP_INVALID_CAUSE;
  }

  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}


//------------------------------------------------------------------------------
int
mme_app_handle_create_sess_resp (
  itti_s11_create_session_response_t * const create_sess_resp_pP)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                 *ue_context_p = NULL;
  bearer_context_t                       *current_bearer_p = NULL;
  MessageDef                             *message_p = NULL;
  ebi_t                                   bearer_id = 0;
  int                                     rc = RETURNok;

  DevAssert (create_sess_resp_pP );
  OAILOG_DEBUG (LOG_MME_APP, "Received S11_CREATE_SESSION_RESPONSE from S+P-GW\n");
  ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, create_sess_resp_pP->teid);

  if (ue_context_p == NULL) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 CREATE_SESSION_RESPONSE local S11 teid " TEID_FMT " ", create_sess_resp_pP->teid);

    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this teid in list of UE: %08x\n", create_sess_resp_pP->teid);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }

  MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 CREATE_SESSION_RESPONSE local S11 teid " TEID_FMT " IMSI " IMSI_64_FMT " ",
      create_sess_resp_pP->teid, ue_context_p->emm_context._imsi64);

    proc_tid_t  transaction_identifier = 0;
    pdn_cid_t   pdn_cx_id = 0;

  /* Whether SGW has created the session (IP address allocation, local GTP-U end point creation etc.)
   * successfully or not , it is indicated by cause value in create session response message.
   * If cause value is not equal to "REQUEST_ACCEPTED" then this implies that SGW could not allocate the resources for
   * the requested session. In this case, MME-APP sends PDN Connectivity fail message to NAS-ESM with the "cause" received
   * in S11 Session Create Response message.
   * NAS-ESM maps this "S11 cause" to "ESM cause" and sends it in PDN Connectivity Reject message to the UE.
   */

  if (create_sess_resp_pP->cause.cause_value != REQUEST_ACCEPTED) {
    // Send PDN CONNECTIVITY FAIL message  to NAS layer
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_PDN_CONNECTIVITY_FAIL);
    itti_nas_pdn_connectivity_fail_t *nas_pdn_connectivity_fail = &message_p->ittiMsg.nas_pdn_connectivity_fail;
    memset ((void *)nas_pdn_connectivity_fail, 0, sizeof (itti_nas_pdn_connectivity_fail_t));
    bearer_id = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].eps_bearer_id /* - 5 */ ;
    current_bearer_p = mme_app_get_bearer_context(ue_context_p, bearer_id);
    if (current_bearer_p) {
      transaction_identifier = current_bearer_p->transaction_identifier;
    }
    nas_pdn_connectivity_fail->pti = transaction_identifier;
    nas_pdn_connectivity_fail->ue_id = ue_context_p->mme_ue_s1ap_id;
    nas_pdn_connectivity_fail->cause = (pdn_conn_rsp_cause_t)(create_sess_resp_pP->cause.cause_value);
    rc = itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
    unlock_ue_contexts(ue_context_p);
    OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
  }

  //---------------------------------------------------------
  // Process itti_sgw_create_session_response_t.bearer_context_created
  //---------------------------------------------------------
  for (int i=0; i < create_sess_resp_pP->bearer_contexts_created.num_bearer_context; i++) {
    bearer_id = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].eps_bearer_id /* - 5 */ ;
    /*
     * Depending on s11 result we have to send reject or accept for bearers
     */
    DevCheck ((bearer_id < BEARERS_PER_UE) && (bearer_id >= 0), bearer_id, BEARERS_PER_UE, 0);

    if (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].cause.cause_value != REQUEST_ACCEPTED) {
      DevMessage ("Cases where bearer cause != REQUEST_ACCEPTED are not handled\n");
    }
    DevAssert (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].s1u_sgw_fteid.interface_type == S1_U_SGW_GTP_U);

    current_bearer_p = mme_app_get_bearer_context(ue_context_p, bearer_id);
    AssertFatal(current_bearer_p, "Could not get bearer context");

    update_mme_app_stats_default_bearer_add();

    current_bearer_p->bearer_state |= BEARER_STATE_SGW_CREATED;
    if (!i) {
      pdn_cx_id = current_bearer_p->pdn_cx_id;
      /*
       * Store the S-GW teid
       */
      AssertFatal((pdn_cx_id >= 0) && (pdn_cx_id < MAX_APN_PER_UE), "Bad pdn id for bearer");
      ue_context_p->pdn_contexts[pdn_cx_id]->s_gw_teid_s11_s4 = create_sess_resp_pP->s11_sgw_fteid.teid;
      transaction_identifier = current_bearer_p->transaction_identifier;
    }


    current_bearer_p->s_gw_fteid_s1u = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].s1u_sgw_fteid;
    current_bearer_p->p_gw_fteid_s5_s8_up = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].s5_s8_u_pgw_fteid;

    // if modified by pgw
    if (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos ) {
      current_bearer_p->qci                      = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->qci;
      current_bearer_p->priority_level           = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->pl;
      current_bearer_p->preemption_vulnerability = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->pvi;
      current_bearer_p->preemption_capability    = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->pci;

//TODO should be set in NAS_PDN_CONNECTIVITY_RSP message
      current_bearer_p->esm_ebr_context.gbr_dl   = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->gbr.br_dl;
      current_bearer_p->esm_ebr_context.gbr_ul   = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->gbr.br_ul;
      current_bearer_p->esm_ebr_context.mbr_dl   = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->mbr.br_dl;
      current_bearer_p->esm_ebr_context.mbr_ul   = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[i].bearer_level_qos->mbr.br_ul;
      OAILOG_DEBUG (LOG_MME_APP, "Set qci %u in bearer %u\n", current_bearer_p->qci, bearer_id);
    } else {
      OAILOG_DEBUG (LOG_MME_APP, "Set qci %u in bearer %u (qos not modified by P-GW)\n", current_bearer_p->qci, bearer_id);
    }
  }

  //uint8_t *keNB = NULL;
  message_p = itti_alloc_new_message (TASK_MME_APP, NAS_PDN_CONNECTIVITY_RSP);
  itti_nas_pdn_connectivity_rsp_t *nas_pdn_connectivity_rsp = &message_p->ittiMsg.nas_pdn_connectivity_rsp;

  nas_pdn_connectivity_rsp->pdn_cid = pdn_cx_id;
  nas_pdn_connectivity_rsp->pti = transaction_identifier;  // NAS internal ref
  nas_pdn_connectivity_rsp->ue_id = ue_context_p->mme_ue_s1ap_id;      // NAS internal ref

  nas_pdn_connectivity_rsp->pdn_addr = paa_to_bstring(&create_sess_resp_pP->paa);
  nas_pdn_connectivity_rsp->pdn_type = create_sess_resp_pP->paa.pdn_type;

  // ASSUME NO HO now, so assume 1 bearer only and is default bearer

  // here at this point OctetString are saved in resp, no loss of memory (apn, pdn_addr)
  nas_pdn_connectivity_rsp->ue_id                 = ue_context_p->mme_ue_s1ap_id;
  nas_pdn_connectivity_rsp->ebi                   = bearer_id;
  nas_pdn_connectivity_rsp->qci                   = current_bearer_p->qci;
  nas_pdn_connectivity_rsp->prio_level            = current_bearer_p->priority_level;
  nas_pdn_connectivity_rsp->pre_emp_vulnerability = current_bearer_p->preemption_vulnerability;
  nas_pdn_connectivity_rsp->pre_emp_capability    = current_bearer_p->preemption_capability;
  nas_pdn_connectivity_rsp->sgw_s1u_fteid         = current_bearer_p->s_gw_fteid_s1u;

  // optional IE
  nas_pdn_connectivity_rsp->ambr.br_ul            = ue_context_p->suscribed_ue_ambr.br_ul;
  nas_pdn_connectivity_rsp->ambr.br_dl            = ue_context_p->suscribed_ue_ambr.br_dl;

  // This IE is not applicable for TAU/RAU/Handover. If PGW decides to return PCO to the UE, PGW shall send PCO to
  // SGW. If SGW receives the PCO IE, SGW shall forward it to MME/SGSN.
  if (create_sess_resp_pP->pco.num_protocol_or_container_id) {
    copy_protocol_configuration_options (&nas_pdn_connectivity_rsp->pco, &create_sess_resp_pP->pco);
    clear_protocol_configuration_options(&create_sess_resp_pP->pco);
  }

  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_PDN_CONNECTIVITY_RSP sgw_s1u_teid %u ebi %u qci %u prio %u",
      current_bearer_p->s_gw_fteid_s1u.teid,
      bearer_id,
      current_bearer_p->qci,
      current_bearer_p->priority_level);

  rc = itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
}


//------------------------------------------------------------------------------
void
mme_app_handle_initial_context_setup_rsp (
  itti_mme_app_initial_context_setup_rsp_t * const initial_ctxt_setup_rsp_pP)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                 *ue_context_p = NULL;
  MessageDef                             *message_p = NULL;

  OAILOG_DEBUG (LOG_MME_APP, "Received MME_APP_INITIAL_CONTEXT_SETUP_RSP from S1AP\n");
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, initial_ctxt_setup_rsp_pP->ue_id);

  if (ue_context_p == NULL) {
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this mme_ue_s1ap_id in list of UE: " MME_UE_S1AP_ID_FMT "\n", initial_ctxt_setup_rsp_pP->ue_id);
    MSC_LOG_EVENT (MSC_MMEAPP_MME, " MME_APP_INITIAL_CONTEXT_SETUP_RSP Unknown ue %u", initial_ctxt_setup_rsp_pP->ue_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  // Stop Initial context setup process guard timer,if running 
  if (ue_context_p->initial_context_setup_rsp_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->initial_context_setup_rsp_timer.id, NULL)) {
      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Initial Context Setup Rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  }

  message_p = itti_alloc_new_message (TASK_MME_APP, S11_MODIFY_BEARER_REQUEST);
  AssertFatal (message_p , "itti_alloc_new_message Failed");
  itti_s11_modify_bearer_request_t *s11_modify_bearer_request = &message_p->ittiMsg.s11_modify_bearer_request;
  s11_modify_bearer_request->local_teid = ue_context_p->mme_teid_s11;
  /*
   * Delay Value in integer multiples of 50 millisecs, or zero
   */
  s11_modify_bearer_request->delay_dl_packet_notif_req = 0;  // TODO

  for (int item = 0; item < initial_ctxt_setup_rsp_pP->no_of_e_rabs; item++) {
    s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[item].eps_bearer_id     = initial_ctxt_setup_rsp_pP->e_rab_id[item];
    s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[item].s1_eNB_fteid.teid = initial_ctxt_setup_rsp_pP->gtp_teid[item];
    s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[item].s1_eNB_fteid.interface_type    = S1_U_ENODEB_GTP_U;

    if (!item) {
      ebi_t             ebi = initial_ctxt_setup_rsp_pP->e_rab_id[item];
      pdn_cid_t         cid = ue_context_p->bearer_contexts[EBI_TO_INDEX(ebi)]->pdn_cx_id;
      pdn_context_t    *pdn_context = ue_context_p->pdn_contexts[cid];

      s11_modify_bearer_request->peer_ip = pdn_context->s_gw_address_s11_s4.address.ipv4_address;
      s11_modify_bearer_request->teid    = pdn_context->s_gw_teid_s11_s4;
    }
    if (4 == blength(initial_ctxt_setup_rsp_pP->transport_layer_address[item])) {
      s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[item].s1_eNB_fteid.ipv4         = 1;
      memcpy(&s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[item].s1_eNB_fteid.ipv4_address,
          initial_ctxt_setup_rsp_pP->transport_layer_address[item]->data, blength(initial_ctxt_setup_rsp_pP->transport_layer_address[item]));
    } else if (16 == blength(initial_ctxt_setup_rsp_pP->transport_layer_address[item])) {
      s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[item].s1_eNB_fteid.ipv6         = 1;
      memcpy(&s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[item].s1_eNB_fteid.ipv6_address,
          initial_ctxt_setup_rsp_pP->transport_layer_address[item]->data,
          blength(initial_ctxt_setup_rsp_pP->transport_layer_address[item]));
    } else {
      AssertFatal(0, "TODO IP address %d bytes", blength(initial_ctxt_setup_rsp_pP->transport_layer_address[item]));
    }
    bdestroy_wrapper (&initial_ctxt_setup_rsp_pP->transport_layer_address[item]);
  }
  s11_modify_bearer_request->bearer_contexts_to_be_modified.num_bearer_context = initial_ctxt_setup_rsp_pP->no_of_e_rabs;

  s11_modify_bearer_request->bearer_contexts_to_be_removed.num_bearer_context = 0;

  s11_modify_bearer_request->mme_fq_csid.node_id_type = GLOBAL_UNICAST_IPv4; // TODO
  s11_modify_bearer_request->mme_fq_csid.csid = 0;   // TODO ...
  memset(&s11_modify_bearer_request->indication_flags, 0, sizeof(s11_modify_bearer_request->indication_flags));   // TODO
  s11_modify_bearer_request->rat_type = RAT_EUTRAN;
  /*
   * S11 stack specific parameter. Not used in standalone epc mode
   */
  s11_modify_bearer_request->trxn = NULL;
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME,  MSC_S11_MME ,
                      NULL, 0, "0 S11_MODIFY_BEARER_REQUEST teid %u ebi %u", s11_modify_bearer_request->teid,
                      s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id);
  itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);

  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_release_access_bearers_resp (
  const itti_s11_release_access_bearers_response_t * const rel_access_bearers_rsp_pP)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                    *ue_context_p = NULL;

  ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, rel_access_bearers_rsp_pP->teid);

  if (ue_context_p == NULL) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 RELEASE_ACCESS_BEARERS_RESPONSE local S11 teid " TEID_FMT " ",
    		rel_access_bearers_rsp_pP->teid);
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this teid in list of UE: %" PRIX32 "\n", rel_access_bearers_rsp_pP->teid);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 RELEASE_ACCESS_BEARERS_RESPONSE local S11 teid " TEID_FMT " IMSI " IMSI_64_FMT " ", rel_access_bearers_rsp_pP->teid, ue_context_p->emm_context._imsi64);
  /*
   * Updating statistics
   */
  update_mme_app_stats_s1u_bearer_sub();

  // Send UE Context Release Command
  mme_app_itti_ue_context_release(ue_context_p, ue_context_p->ue_context_rel_cause);
  if (ue_context_p->ue_context_rel_cause == S1AP_SCTP_SHUTDOWN_OR_RESET) {
    // Just cleanup the MME APP state associated with s1.
    mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context_p, ECM_IDLE);
  }
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_s11_create_bearer_req (
    const itti_s11_create_bearer_request_t * const create_bearer_request_pP)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  //MessageDef                             *message_p = NULL;
  struct ue_mm_context_s                    *ue_context_p = NULL;

  ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, create_bearer_request_pP->teid);

  if (ue_context_p == NULL) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 CREATE_BEARERS_REQUEST local S11 teid " TEID_FMT " ",
        create_bearer_request_pP->teid);
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this teid in list of UE: %" PRIX32 "\n", create_bearer_request_pP->teid);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  // check if default bearer already created
  ebi_t linked_eps_bearer_id = create_bearer_request_pP->linked_eps_bearer_id;
  bearer_context_t * linked_bc = mme_app_get_bearer_context(ue_context_p, linked_eps_bearer_id);
  if (!linked_bc) {
    // May create default EPS bearer ?
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 CREATE_BEARERS_REQUEST ue id " MME_UE_S1AP_ID_FMT " local S11 teid " TEID_FMT " ",
        ue_context_p->mme_ue_s1ap_id, create_bearer_request_pP->teid);
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find the linked bearer id %" PRIu8 " for UE: " MME_UE_S1AP_ID_FMT "\n",
          linked_eps_bearer_id, ue_context_p->mme_ue_s1ap_id);
    unlock_ue_contexts(ue_context_p);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  pdn_cid_t cid = linked_bc->pdn_cx_id;

  MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 CREATE_BEARERS_REQUEST ue id " MME_UE_S1AP_ID_FMT " PDN id %u IMSI " IMSI_64_FMT " n ebi %u",
        ue_context_p->mme_ue_s1ap_id, cid, ue_context_p->emm_context._imsi64, create_bearer_request_pP->bearer_contexts.num_bearer_context);

  mme_app_s11_proc_create_bearer_t* s11_proc_create_bearer = mme_app_create_s11_procedure_create_bearer(ue_context_p);
  s11_proc_create_bearer->proc.s11_trxn  = (uintptr_t)create_bearer_request_pP->trxn;

  for (int i = 0; i < create_bearer_request_pP->bearer_contexts.num_bearer_context; i++) {
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO THINK OF BEARER AGGREGATING SEVERAL SDFs, 1 bearer <-> (QCI, ARP)
    // TODO DELEGATE TO NAS THE CREATION OF THE BEARER
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    const bearer_context_within_create_bearer_request_t *msg_bc = &create_bearer_request_pP->bearer_contexts.bearer_contexts[i];
    bearer_context_t *  dedicated_bc = mme_app_create_bearer_context(ue_context_p, cid, msg_bc->eps_bearer_id, false);

    s11_proc_create_bearer->num_bearers++;
    s11_proc_create_bearer->bearer_status[EBI_TO_INDEX(dedicated_bc->ebi)] = S11_PROC_BEARER_PENDING;

    dedicated_bc->bearer_state   |= BEARER_STATE_SGW_CREATED;
    dedicated_bc->bearer_state   |= BEARER_STATE_MME_CREATED;

    dedicated_bc->s_gw_fteid_s1u      = msg_bc->s1u_sgw_fteid;
    dedicated_bc->p_gw_fteid_s5_s8_up = msg_bc->s5_s8_u_pgw_fteid;

    dedicated_bc->qci                      = msg_bc->bearer_level_qos.qci;
    dedicated_bc->priority_level           = msg_bc->bearer_level_qos.pl;
    dedicated_bc->preemption_vulnerability = msg_bc->bearer_level_qos.pvi;
    dedicated_bc->preemption_capability    = msg_bc->bearer_level_qos.pci;

    // forward request to NAS
    MessageDef  *message_p = itti_alloc_new_message (TASK_MME_APP, MME_APP_CREATE_DEDICATED_BEARER_REQ);
    AssertFatal (message_p , "itti_alloc_new_message Failed");
    MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).ue_id          = ue_context_p->mme_ue_s1ap_id;
    MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).cid            = cid;
    MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).ebi            = dedicated_bc->ebi;
    MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).linked_ebi     = ue_context_p->pdn_contexts[cid]->default_ebi;
    MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).bearer_qos     = msg_bc->bearer_level_qos;
    if (msg_bc->tft.numberofpacketfilters) {
      MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).tft = calloc(1, sizeof(traffic_flow_template_t));
      copy_traffic_flow_template(MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).tft, &msg_bc->tft);
    }
    if (msg_bc->pco.num_protocol_or_container_id) {
      MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).pco = calloc(1, sizeof(protocol_configuration_options_t));
      copy_protocol_configuration_options(MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).pco, &msg_bc->pco);
    }

    MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 MME_APP_CREATE_DEDICATED_BEARER_REQ mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " qci %u ebi %u cid %u",
        MME_APP_CREATE_DEDICATED_BEARER_REQ (message_p).ue_id, dedicated_bc->qci, dedicated_bc->ebi, cid);
    itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  }
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void mme_app_handle_e_rab_setup_rsp (itti_s1ap_e_rab_setup_rsp_t  * const e_rab_setup_rsp)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                 *ue_context_p = NULL;
  bool                                    send_s11_response = false;

  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, e_rab_setup_rsp->mme_ue_s1ap_id);

  if (ue_context_p == NULL) {
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this mme_ue_s1ap_id in list of UE: " MME_UE_S1AP_ID_FMT "\n", e_rab_setup_rsp->mme_ue_s1ap_id);
    MSC_LOG_EVENT (MSC_MMEAPP_MME, " S1AP_E_RAB_SETUP_RSP Unknown ue " MME_UE_S1AP_ID_FMT "\n", e_rab_setup_rsp->mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  for (int i = 0; i < e_rab_setup_rsp->e_rab_setup_list.no_of_items; i++) {
    e_rab_id_t e_rab_id = e_rab_setup_rsp->e_rab_setup_list.item[i].e_rab_id;
    bearer_context_t * bc = mme_app_get_bearer_context(ue_context_p, (ebi_t) e_rab_id);
    if (bc->bearer_state & BEARER_STATE_SGW_CREATED) {
      bc->enb_fteid_s1u.teid = e_rab_setup_rsp->e_rab_setup_list.item[i].gtp_teid;
      // Do not process transport_layer_address now
      //bstring e_rab_setup_rsp->e_rab_setup_list.item[i].transport_layer_address;
      ip_address_t enb_ip_address = {0};
      bstring_to_ip_address(e_rab_setup_rsp->e_rab_setup_list.item[i].transport_layer_address, &enb_ip_address);

      bc->enb_fteid_s1u.interface_type = S1_U_ENODEB_GTP_U;
      // TODO better than that later
      switch (enb_ip_address.pdn_type) {
      case IPv4:
        bc->enb_fteid_s1u.ipv4         = 1;
        bc->enb_fteid_s1u.ipv4_address = enb_ip_address.address.ipv4_address;
        break;
      case IPv6:
        bc->enb_fteid_s1u.ipv6         = 1;
        memcpy(&bc->enb_fteid_s1u.ipv6_address,
            &enb_ip_address.address.ipv6_address, sizeof(enb_ip_address.address.ipv6_address));
        break;
      default:
        AssertFatal(0, "Bug enb_ip_address->pdn_type");
      }
      bdestroy_wrapper (&e_rab_setup_rsp->e_rab_setup_list.item[i].transport_layer_address);

      AssertFatal(bc->bearer_state & BEARER_STATE_MME_CREATED, "TO DO check bearer state");
      bc->bearer_state |= BEARER_STATE_ENB_CREATED;

      if (ESM_EBR_ACTIVE == bc->esm_ebr_context.status) {
        send_s11_response = true;
      }
    }
  }
  for (int i = 0; i < e_rab_setup_rsp->e_rab_failed_to_setup_list.no_of_items; i++) {
    e_rab_id_t e_rab_id = e_rab_setup_rsp->e_rab_failed_to_setup_list.item[i].e_rab_id;
    bearer_context_t * bc = mme_app_get_bearer_context(ue_context_p, (ebi_t) e_rab_id);
    if (bc->bearer_state & BEARER_STATE_SGW_CREATED) {
      send_s11_response = true;
      //S1ap_Cause_t cause = e_rab_setup_rsp->e_rab_failed_to_setup_list.item[i].cause;
      AssertFatal(bc->bearer_state & BEARER_STATE_MME_CREATED, "TO DO check bearer state");
      bc->bearer_state &= (~BEARER_STATE_ENB_CREATED);
      bc->bearer_state &= (~BEARER_STATE_MME_CREATED);
    }
  }

  // check if UE already responded with NAS (may depend on eNB implementation?) -> send response to SGW
  if (send_s11_response) {
    MessageDef  *message_p = itti_alloc_new_message (TASK_MME_APP, S11_CREATE_BEARER_RESPONSE);
    AssertFatal (message_p , "itti_alloc_new_message Failed");
    itti_s11_create_bearer_response_t *s11_create_bearer_response = &message_p->ittiMsg.s11_create_bearer_response;
    s11_create_bearer_response->local_teid = ue_context_p->mme_teid_s11;
    s11_create_bearer_response->trxn = NULL;
    s11_create_bearer_response->cause.cause_value = 0;
    int msg_bearer_index = 0;

    for (int i = 0; i < e_rab_setup_rsp->e_rab_setup_list.no_of_items; i++) {
      e_rab_id_t e_rab_id = e_rab_setup_rsp->e_rab_setup_list.item[i].e_rab_id;
      bearer_context_t * bc = mme_app_get_bearer_context(ue_context_p, (ebi_t) e_rab_id);
      if (bc->bearer_state & BEARER_STATE_ENB_CREATED) {
        s11_create_bearer_response->cause.cause_value = REQUEST_ACCEPTED;
        s11_create_bearer_response->bearer_contexts.bearer_contexts[msg_bearer_index].eps_bearer_id = e_rab_id;
        s11_create_bearer_response->bearer_contexts.bearer_contexts[msg_bearer_index].cause.cause_value = REQUEST_ACCEPTED;
        //  FTEID eNB
        s11_create_bearer_response->bearer_contexts.bearer_contexts[msg_bearer_index].s1u_enb_fteid = bc->enb_fteid_s1u;

        // FTEID SGW S1U
        s11_create_bearer_response->bearer_contexts.bearer_contexts[msg_bearer_index].s1u_sgw_fteid = bc->s_gw_fteid_s1u;       ///< This IE shall be sent on the S11 interface. It shall be used
        s11_create_bearer_response->bearer_contexts.num_bearer_context++;
      }
    }

    for (int i = 0; i < e_rab_setup_rsp->e_rab_setup_list.no_of_items; i++) {
      e_rab_id_t e_rab_id = e_rab_setup_rsp->e_rab_setup_list.item[i].e_rab_id;
      bearer_context_t * bc = mme_app_get_bearer_context(ue_context_p, (ebi_t) e_rab_id);
      if (bc->bearer_state & BEARER_STATE_MME_CREATED) {
        if (REQUEST_ACCEPTED == s11_create_bearer_response->cause.cause_value) {
          s11_create_bearer_response->cause.cause_value = REQUEST_ACCEPTED_PARTIALLY;
        } else {
          s11_create_bearer_response->cause.cause_value = REQUEST_REJECTED;
        }
        s11_create_bearer_response->bearer_contexts.bearer_contexts[msg_bearer_index].eps_bearer_id = e_rab_id;
        s11_create_bearer_response->bearer_contexts.bearer_contexts[msg_bearer_index].cause.cause_value = REQUEST_REJECTED; // TODO translation of S1AP cause to SGW cause
        s11_create_bearer_response->bearer_contexts.num_bearer_context++;
        bc->bearer_state = BEARER_STATE_NULL;
      }
    }

    MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME,  MSC_S11_MME ,
                      NULL, 0, "0 S11_CREATE_BEARER_RESPONSE teid %u", s11_create_bearer_response->teid);
    itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  } else {
    // not send S11 response
    // TODO create a procedure with bearers to receive a response from NAS
  }
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}



//------------------------------------------------------------------------------
void
mme_app_handle_mobile_reachability_timer_expiry (struct ue_mm_context_s *ue_context_p)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_p != NULL);
  ue_context_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
  OAILOG_INFO (LOG_MME_APP, "Expired- Mobile Reachability Timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  // Start Implicit Detach timer 
  if (timer_setup (ue_context_p->implicit_detach_timer.sec, 0, 
                TASK_MME_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT, (void *)&(ue_context_p->mme_ue_s1ap_id), &(ue_context_p->implicit_detach_timer.id)) < 0) { 
    OAILOG_ERROR (LOG_MME_APP, "Failed to start Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  } else {
    OAILOG_DEBUG (LOG_MME_APP, "Started Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
void
mme_app_handle_implicit_detach_timer_expiry (struct ue_mm_context_s *ue_context_p)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_p != NULL);
  MessageDef                             *message_p = NULL;
  OAILOG_INFO (LOG_MME_APP, "Expired- Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  
  // Initiate Implicit Detach for the UE
  message_p = itti_alloc_new_message (TASK_MME_APP, NAS_IMPLICIT_DETACH_UE_IND);
  DevAssert (message_p != NULL);
  message_p->ittiMsg.nas_implicit_detach_ue_ind.ue_id = ue_context_p->mme_ue_s1ap_id;
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_IMPLICIT_DETACH_UE_IND_MESSAGE");
  itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_initial_context_setup_rsp_timer_expiry (struct ue_mm_context_s *ue_context_p)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_p != NULL);
  MessageDef                             *message_p = NULL;
  OAILOG_INFO (LOG_MME_APP, "Expired- Initial context setup rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  /* *********Abort the ongoing procedure*********
   * Check if UE is registered already that implies service request procedure is active. If so then release the S1AP
   * context and move the UE back to idle mode. Otherwise if UE is not yet registered that implies attach procedure is
   * active. If so,then abort the attach procedure and release the UE context. 
   */
  ue_context_p->ue_context_rel_cause = S1AP_INITIAL_CONTEXT_SETUP_FAILED;
  if (ue_context_p->mm_state == UE_UNREGISTERED) {
    // Initiate Implicit Detach for the UE
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_IMPLICIT_DETACH_UE_IND);
    DevAssert (message_p != NULL);
    message_p->ittiMsg.nas_implicit_detach_ue_ind.ue_id = ue_context_p->mme_ue_s1ap_id;
    itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  } else {
    // Release S1-U bearer and move the UE to idle mode
    for (pdn_cid_t i = 0; i < MAX_APN_PER_UE; i++) {
      if (ue_context_p->pdn_contexts[i]) {
        mme_app_send_s11_release_access_bearers_req(ue_context_p, i);
      }
    }
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
void
mme_app_handle_initial_context_setup_failure (
  const itti_mme_app_initial_context_setup_failure_t * const initial_ctxt_setup_failure_pP)
{
  struct ue_mm_context_s                 *ue_context_p = NULL;
  MessageDef                             *message_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  OAILOG_DEBUG (LOG_MME_APP, "Received MME_APP_INITIAL_CONTEXT_SETUP_FAILURE from S1AP\n");
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, initial_ctxt_setup_failure_pP->mme_ue_s1ap_id);

  if (ue_context_p == NULL) {
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this mme_ue_s1ap_id in list of UE: %d \n", initial_ctxt_setup_failure_pP->mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  // Stop Initial context setup process guard timer,if running 
  if (ue_context_p->initial_context_setup_rsp_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->initial_context_setup_rsp_timer.id, NULL)) {
      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Initial Context Setup Rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  }
  /* *********Abort the ongoing procedure*********
   * Check if UE is registered already that implies service request procedure is active. If so then release the S1AP
   * context and move the UE back to idle mode. Otherwise if UE is not yet registered that implies attach procedure is
   * active. If so,then abort the attach procedure and release the UE context. 
   */
  ue_context_p->ue_context_rel_cause = S1AP_INITIAL_CONTEXT_SETUP_FAILED;
  if (ue_context_p->mm_state == UE_UNREGISTERED) {
    // Initiate Implicit Detach for the UE
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_IMPLICIT_DETACH_UE_IND);
    DevAssert (message_p != NULL);
    message_p->ittiMsg.nas_implicit_detach_ue_ind.ue_id = ue_context_p->mme_ue_s1ap_id;
    itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  } else {
    // Release S1-U bearer and move the UE to idle mode 
    for (pdn_cid_t i = 0; i < MAX_APN_PER_UE; i++) {
      if (ue_context_p->pdn_contexts[i]) {
        mme_app_send_s11_release_access_bearers_req(ue_context_p, i);
      }
    }
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
static bool mme_app_construct_guti(const plmn_t * const plmn_p, const s_tmsi_t * const s_tmsi_p,  guti_t * const guti_p)
{
  /*
   * This is a helper function to construct GUTI from S-TMSI. It uses PLMN id and MME Group Id of the serving MME for
   * this purpose. 
   *
   */
  
  bool                                    is_guti_valid = false; // Set to true if serving MME is found and GUTI is constructed 
  uint8_t                                 num_mme       = 0;     // Number of configured MME in the MME pool  
  guti_p->m_tmsi = s_tmsi_p->m_tmsi;
  guti_p->gummei.mme_code = s_tmsi_p->mme_code;
  // Create GUTI by using PLMN Id and MME-Group Id of serving MME
  OAILOG_DEBUG (LOG_MME_APP,
                "Construct GUTI using S-TMSI received form UE and MME Group Id and PLMN id from MME Conf: %u, %u \n",
                s_tmsi_p->m_tmsi, s_tmsi_p->mme_code);
  mme_config_read_lock (&mme_config);
  /*
   * Check number of MMEs in the pool.
   * At present it is assumed that one MME is supported in MME pool but in case there are more 
   * than one MME configured then search the serving MME using MME code. 
   * Assumption is that within one PLMN only one pool of MME will be configured
   */
  if (mme_config.gummei.nb > 1) 
  {
    OAILOG_DEBUG (LOG_MME_APP, "More than one MMEs are configured.");
  }
  for (num_mme = 0; num_mme < mme_config.gummei.nb; num_mme++)
  {
    /*Verify that the MME code within S-TMSI is same as what is configured in MME conf*/
    if ((plmn_p->mcc_digit2 == mme_config.gummei.gummei[num_mme].plmn.mcc_digit2) &&
        (plmn_p->mcc_digit1 == mme_config.gummei.gummei[num_mme].plmn.mcc_digit1) &&
        (plmn_p->mnc_digit3 == mme_config.gummei.gummei[num_mme].plmn.mnc_digit3) &&
        (plmn_p->mcc_digit3 == mme_config.gummei.gummei[num_mme].plmn.mcc_digit3) &&
        (plmn_p->mnc_digit2 == mme_config.gummei.gummei[num_mme].plmn.mnc_digit2) && 
        (plmn_p->mnc_digit1 == mme_config.gummei.gummei[num_mme].plmn.mnc_digit1) &&
        (guti_p->gummei.mme_code == mme_config.gummei.gummei[num_mme].mme_code))
    {
      break;
    }
  }          
  if (num_mme >= mme_config.gummei.nb)
  {
    OAILOG_DEBUG (LOG_MME_APP, "No MME serves this UE");
  }
  else 
  {
    guti_p->gummei.plmn = mme_config.gummei.gummei[num_mme].plmn;
    guti_p->gummei.mme_gid = mme_config.gummei.gummei[num_mme].mme_gid;
    is_guti_valid = true;
  }
  mme_config_unlock (&mme_config);
  return is_guti_valid;
}

//------------------------------------------------------------------------------
static void notify_s1ap_new_ue_mme_s1ap_id_association (struct ue_mm_context_s *ue_context_p)
{
  MessageDef                             *message_p = NULL;
  itti_mme_app_s1ap_mme_ue_id_notification_t *notification_p = NULL;
  
  OAILOG_FUNC_IN (LOG_MME_APP);
  if (ue_context_p == NULL) {
    OAILOG_ERROR (LOG_MME_APP, " NULL UE context ptr\n" );
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  message_p = itti_alloc_new_message (TASK_MME_APP, MME_APP_S1AP_MME_UE_ID_NOTIFICATION);
  notification_p = &message_p->ittiMsg.mme_app_s1ap_mme_ue_id_notification;
  memset (notification_p, 0, sizeof (itti_mme_app_s1ap_mme_ue_id_notification_t)); 
  notification_p->enb_ue_s1ap_id = ue_context_p->enb_ue_s1ap_id; 
  notification_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
  notification_p->sctp_assoc_id  = ue_context_p->sctp_assoc_id_key;

  itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);
  OAILOG_DEBUG (LOG_MME_APP, " Sent MME_APP_S1AP_MME_UE_ID_NOTIFICATION to S1AP for UE Id %u\n", notification_p->mme_ue_s1ap_id);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void mme_app_handle_create_dedicated_bearer_rsp (itti_mme_app_create_dedicated_bearer_rsp_t   * const create_dedicated_bearer_rsp)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                 *ue_context_p = NULL;

  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, create_dedicated_bearer_rsp->ue_id);

  if (ue_context_p == NULL) {
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this mme_ue_s1ap_id in list of UE: " MME_UE_S1AP_ID_FMT "\n", create_dedicated_bearer_rsp->ue_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  // TODO:
  // Actually do it simple, because it appear we have to wait for NAS procedure reworking (work in progress on another branch)
  // for responding to S11 without mistakes (may be the create bearer procedure can be impacted by a S1 ue context release or
  // a UE originating  NAS procedure)
  mme_app_s11_proc_create_bearer_t *s11_proc_create = mme_app_get_s11_procedure_create_bearer(ue_context_p);
  if (s11_proc_create) {
    ebi_t ebi = create_dedicated_bearer_rsp->ebi;

    s11_proc_create->num_status_received++;
    s11_proc_create->bearer_status[EBI_TO_INDEX(ebi)] = S11_PROC_BEARER_SUCCESS;
    // if received all bearers creation results
    if (s11_proc_create->num_status_received == s11_proc_create->num_bearers) {
      mme_app_s11_procedure_create_bearer_send_response(ue_context_p, s11_proc_create);
      mme_app_delete_s11_procedure_create_bearer(ue_context_p);
    }
  }
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void mme_app_handle_create_dedicated_bearer_rej (itti_mme_app_create_dedicated_bearer_rej_t   * const create_dedicated_bearer_rej)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  struct ue_mm_context_s                 *ue_context_p = NULL;

  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, create_dedicated_bearer_rej->ue_id);

  if (ue_context_p == NULL) {
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this mme_ue_s1ap_id in list of UE: " MME_UE_S1AP_ID_FMT "\n", create_dedicated_bearer_rej->ue_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  // TODO:
  // Actually do it simple, because it appear we have to wait for NAS procedure reworking (work in progress on another branch)
  // for responding to S11 without mistakes (may be the create bearer procedure can be impacted by a S1 ue context release or
  // a UE originating  NAS procedure)
  mme_app_s11_proc_create_bearer_t *s11_proc_create = mme_app_get_s11_procedure_create_bearer(ue_context_p);
  if (s11_proc_create) {
    ebi_t ebi = create_dedicated_bearer_rej->ebi;

    s11_proc_create->num_status_received++;
    s11_proc_create->bearer_status[EBI_TO_INDEX(ebi)] = S11_PROC_BEARER_FAILED;
    // if received all bearers creation results
    if (s11_proc_create->num_status_received == s11_proc_create->num_bearers) {
      mme_app_s11_procedure_create_bearer_send_response(ue_context_p, s11_proc_create);
      mme_app_delete_s11_procedure_create_bearer(ue_context_p);
    }
  }
  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
// See 3GPP TS 23.401 version 10.13.0 Release 10: 5.4.4.2 MME Initiated Dedicated Bearer Deactivation
void mme_app_trigger_mme_initiated_dedicated_bearer_deactivation_procedure (ue_mm_context_t * const ue_context, const pdn_cid_t cid)
{
  OAILOG_DEBUG (LOG_MME_APP, "TODO \n");
}

