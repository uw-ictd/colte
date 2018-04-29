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


/*! \file mme_app_location.c
   \brief
   \author Sebastien ROUX, Lionel GAUTHIER
   \version 1.0
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

#include "log.h"
#include "msc.h"
#include "assertions.h"
#include "common_types.h"
#include "conversions.h"
#include "intertask_interface.h"
#include "gcc_diag.h"
#include "common_defs.h"
#include "mme_config.h"
#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"

//------------------------------------------------------------------------------
void mme_app_send_s6a_cancel_location_ans (
  struct ue_mm_context_s *const ue_mm_context)
{
  OAILOG_DEBUG (LOG_MME_APP, "SMS CLR: SENDING CANCEL LOCATION ANSWER\n");
  return;
}
//   OAILOG_FUNC_IN (LOG_MME_APP);
//   MessageDef                             *message_p = NULL;
//   s6a_update_location_req_t              *s6a_ulr_p = NULL;
//   int                                     rc = RETURNok;


//   message_p = itti_alloc_new_message (TASK_MME_APP, S6A_UPDATE_LOCATION_REQ);

//   if (message_p == NULL) {
//     OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
//   }

//   s6a_ulr_p = &message_p->ittiMsg.s6a_update_location_req;
//   IMSI_TO_STRING((&ue_mm_context->emm_context._imsi),s6a_ulr_p->imsi, IMSI_BCD_DIGITS_MAX+1);
//   s6a_ulr_p->imsi_length = strlen (s6a_ulr_p->imsi);
//   s6a_ulr_p->initial_attach = INITIAL_ATTACH;
//   plmn_t visited_plmn = {0};
//   visited_plmn.mcc_digit1 = ue_mm_context->emm_context.originating_tai.mcc_digit1;
//   visited_plmn.mcc_digit2 = ue_mm_context->emm_context.originating_tai.mcc_digit2;
//   visited_plmn.mcc_digit3 = ue_mm_context->emm_context.originating_tai.mcc_digit3;
//   visited_plmn.mnc_digit1 = ue_mm_context->emm_context.originating_tai.mnc_digit1;
//   visited_plmn.mnc_digit2 = ue_mm_context->emm_context.originating_tai.mnc_digit2;
//   visited_plmn.mnc_digit3 = ue_mm_context->emm_context.originating_tai.mnc_digit3;

//   memcpy (&s6a_ulr_p->visited_plmn, &visited_plmn, sizeof (plmn_t));
//   s6a_ulr_p->rat_type = RAT_EUTRAN;
//   /*
//    * Check if we already have UE data
//    */
//   s6a_ulr_p->skip_subscriber_data = 0;
//   MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S6A_MME, NULL, 0, "0 S6A_UPDATE_LOCATION_REQ imsi %s", s6a_ulr_p->imsi);
//   rc =  itti_send_msg_to_task (TASK_S6A, INSTANCE_DEFAULT, message_p);
//   OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
// }



//------------------------------------------------------------------------------
void mme_app_handle_s6a_cancel_location_req (
  const s6a_cancel_location_req_t * const cancel_req_p)
  // const itti_nas_detach_req_t * const detach_req_p) is what we're modeling
{
  OAILOG_DEBUG (LOG_MME_APP, "SMS CLR: HANDLING CANCEL LOCATION REQUEST\n");
  return 0;

  struct ue_mm_context_s *ue_context = NULL;
  DevAssert(cancel_req_p != NULL);
  ue_context = mme_ue_context_exists_imsi (&mme_app_desc.mme_ue_contexts, cancel_req_p->imsi64);
  
  if (ue_context == NULL) {
    OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist -> Nothing to do :-) \n");
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }  

  // STEP ZERO: page the ue to wake it up if idle - can we just skip?!? Does NAS handle this?!?
  // STEP ONE: send the ue a detach request message - skip for now!
  MessageDef *message_p;
  message_p = itti_alloc_new_message(TASK_MME_APP, S1AP_DEREGISTER_UE_REQ);
  S1AP_DEREGISTER_UE_REQ (message_p).mme_ue_s1ap_id = ue_context->mme_ue_s1ap_id;
  // S1AP_UE_CONTEXT_RELEASE_COMMAND (message_p).cause = cause;
  // MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S1AP_MME, NULL, 0, "0 S1AP_UE_CONTEXT_RELEASE_COMMAND mme_ue_s1ap_id %06" PRIX32 " ",
                      // S1AP_UE_CONTEXT_RELEASE_COMMAND (message_p).mme_ue_s1ap_id);
  itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);

  // STEP TWO: proceed as if we've received a detach request message. This code is just
  // copied from mme_app_handle_detach_req
  if ((!ue_context->mme_teid_s11) && (!ue_context->nb_active_pdn_contexts)) {
    /* No Session.
     * If UE is already in idle state, skip asking eNB to release UE context and just clean up locally.
     */
    if (ECM_IDLE == ue_context->ecm_state) {
      ue_context->ue_context_rel_cause = S1AP_IMPLICIT_CONTEXT_RELEASE;
      // Notify S1AP to release S1AP UE context locally.
      mme_app_itti_ue_context_release (ue_context, ue_context->ue_context_rel_cause);
      // Free MME UE Context   
      mme_notify_ue_context_released (&mme_app_desc.mme_ue_contexts, ue_context);
      mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context);
    } else {
      if (ue_context->ue_context_rel_cause == S1AP_INVALID_CAUSE) {
          ue_context->ue_context_rel_cause = S1AP_NAS_DETACH;
      }
      // Notify S1AP to send UE Context Release Command to eNB.
      mme_app_itti_ue_context_release (ue_context, ue_context->ue_context_rel_cause);
      if (ue_context->ue_context_rel_cause == S1AP_SCTP_SHUTDOWN_OR_RESET) {
        // Just cleanup the MME APP state associated with s1.
        mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context, ECM_IDLE);
        // Free MME UE Context   
        mme_notify_ue_context_released (&mme_app_desc.mme_ue_contexts, ue_context);
        mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context);
      } else {
        ue_context->ue_context_rel_cause = S1AP_INVALID_CAUSE;
      }
    }
  } else {
    for (pdn_cid_t i = 0; i < MAX_APN_PER_UE; i++) {
      if (ue_context->pdn_contexts[i]) {
        // Send a DELETE_SESSION_REQUEST message to the SGW
        mme_app_send_delete_session_request(ue_context, ue_context->pdn_contexts[i]->default_ebi, i);
      }
    }
  }
  unlock_ue_contexts(ue_context);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

// void
// mme_app_handle_detach_req (
//   const itti_nas_detach_req_t * const detach_req_p)
// {
//   struct ue_mm_context_s *ue_context    = NULL;

//   DevAssert(detach_req_p != NULL);
//   ue_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, detach_req_p->ue_id);
//   if (ue_context == NULL) {
//     OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist -> Nothing to do :-) \n");
//     OAILOG_FUNC_OUT (LOG_MME_APP);
//   }
//   if ((!ue_context->mme_teid_s11) && (!ue_context->nb_active_pdn_contexts)) {
//     /* No Session.
//      * If UE is already in idle state, skip asking eNB to release UE context and just clean up locally.
//      */
//     if (ECM_IDLE == ue_context->ecm_state) {
//       ue_context->ue_context_rel_cause = S1AP_IMPLICIT_CONTEXT_RELEASE;
//       // Notify S1AP to release S1AP UE context locally.
//       mme_app_itti_ue_context_release (ue_context, ue_context->ue_context_rel_cause);
//       // Free MME UE Context   
//       mme_notify_ue_context_released (&mme_app_desc.mme_ue_contexts, ue_context);
//       mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context);
//     } else {
//       if (ue_context->ue_context_rel_cause == S1AP_INVALID_CAUSE) {
//           ue_context->ue_context_rel_cause = S1AP_NAS_DETACH;
//       }
//       // Notify S1AP to send UE Context Release Command to eNB.
//       mme_app_itti_ue_context_release (ue_context, ue_context->ue_context_rel_cause);
//       if (ue_context->ue_context_rel_cause == S1AP_SCTP_SHUTDOWN_OR_RESET) {
//         // Just cleanup the MME APP state associated with s1.
//         mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context, ECM_IDLE);
//         // Free MME UE Context   
//         mme_notify_ue_context_released (&mme_app_desc.mme_ue_contexts, ue_context);
//         mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context);
//       } else {
//         ue_context->ue_context_rel_cause = S1AP_INVALID_CAUSE;
//       }
//     }
//   } else {
//     for (pdn_cid_t i = 0; i < MAX_APN_PER_UE; i++) {
//       if (ue_context->pdn_contexts[i]) {
//         // Send a DELETE_SESSION_REQUEST message to the SGW
//         mme_app_send_delete_session_request(ue_context, ue_context->pdn_contexts[i]->default_ebi, i);
//       }
//     }
//   }
//   unlock_ue_contexts(ue_context);
//   OAILOG_FUNC_OUT (LOG_MME_APP);
// }






  // OAILOG_FUNC_IN (LOG_MME_APP);
  // uint64_t                                imsi64 = 0;
  // struct ue_mm_context_s                 *ue_mm_context = NULL;
  // int                                     rc = RETURNok;

  // DevAssert (ula_pP );

  // if (ula_pP->result.present == S6A_RESULT_BASE) {
  //   if (ula_pP->result.choice.base != DIAMETER_SUCCESS) {
  //     /*
  //      * The update location procedure has failed. Notify the NAS layer
  //      * and don't initiate the bearer creation on S-GW side.
  //      */
  //     OAILOG_DEBUG (LOG_MME_APP, "ULR/ULA procedure returned non success (ULA.result.choice.base=%d)\n", ula_pP->result.choice.base);
  //     DevMessage ("ULR/ULA procedure returned non success\n");
  //   }
  // } else {
  //   /*
  //    * The update location procedure has failed. Notify the NAS layer
  //    * and don't initiate the bearer creation on S-GW side.
  //    */
  //   OAILOG_DEBUG (LOG_MME_APP, "ULR/ULA procedure returned non success (ULA.result.present=%d)\n", ula_pP->result.present);
  //   DevMessage ("ULR/ULA procedure returned non success\n");
  // }

  // IMSI_STRING_TO_IMSI64 ((char *)ula_pP->imsi, &imsi64);
  // OAILOG_DEBUG (LOG_MME_APP, "Handling imsi %s\n", ula_pP->imsi);

  // if ((ue_mm_context = mme_ue_context_exists_imsi (&mme_app_desc.mme_ue_contexts, imsi64)) == NULL) {
  //   OAILOG_ERROR (LOG_MME_APP, "That's embarrassing as we don't know this IMSI\n");
  //   MSC_LOG_EVENT (MSC_MMEAPP_MME, "0 S6A_UPDATE_LOCATION unknown imsi %s", ula_pP->imsi);
  //   OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  // }

  // ue_mm_context->subscription_known = SUBSCRIPTION_KNOWN;
  // ue_mm_context->sub_status = ula_pP->subscription_data.subscriber_status;
  // ue_mm_context->access_restriction_data = ula_pP->subscription_data.access_restriction;
  // /*
  //  * Copy the subscribed ambr to the sgw create session request message
  //  */
  // memcpy (&ue_mm_context->suscribed_ue_ambr, &ula_pP->subscription_data.subscribed_ambr, sizeof (ambr_t));
  // // In Activate Default EPS Bearer Context Setup Request message APN-AMPBR is forced to 200Mbps and 100 Mbps for DL
  // // and UL respectively. Since as of now we support only one bearer, forcing AMBR as well to APN-AMBR values.
  // ue_mm_context->subscribed_ambr.br_ul = 100000000; // Setting it to 100 Mbps
  // ue_mm_context->subscribed_ambr.br_dl = 200000000; // Setting it to 200 Mbps
  // // TODO task#14477798 - Configure the policy driven values in HSS and use those here and in NAS.
  // //ue_mm_context->subscribed_ambr.br_ul = ue_mm_context->subscribed_ambr.br_ul; // Setting it to 100 Mbps
  // //ue_mm_context->subscribed_ambr.br_dl = ue_mm_context->subscribed_ambr.br_dl; // Setting it to 200 Mbps

  // ue_mm_context->msisdn = blk2bstr(ula_pP->subscription_data.msisdn, ula_pP->subscription_data.msisdn_length);
  // AssertFatal (ula_pP->subscription_data.msisdn_length != 0, "MSISDN LENGTH IS 0");
  // AssertFatal (ula_pP->subscription_data.msisdn_length <= MSISDN_LENGTH, "MSISDN LENGTH is too high %u", MSISDN_LENGTH);

  // ue_mm_context->rau_tau_timer = ula_pP->subscription_data.rau_tau_timer;
  // ue_mm_context->network_access_mode = ula_pP->subscription_data.access_mode;
  // memcpy (&ue_mm_context->apn_config_profile, &ula_pP->subscription_data.apn_config_profile, sizeof (apn_config_profile_t));


  // MessageDef                             *message_p = NULL;
  // itti_nas_pdn_config_rsp_t              *nas_pdn_config_rsp = NULL;

  // message_p = itti_alloc_new_message (TASK_MME_APP, NAS_PDN_CONFIG_RSP);

  // if (message_p == NULL) {
  //   unlock_ue_contexts(ue_mm_context);
  //   OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  // }

  // /*
  //  * Set the value of  Mobile Reachability timer based on value of T3412 (Periodic TAU timer) sent in Attach accept /TAU accept.
  //  * Set it to MME_APP_DELTA_T3412_REACHABILITY_TIMER minutes greater than T3412.
  //  * Set the value of Implicit timer. Set it to MME_APP_DELTA_REACHABILITY_IMPLICIT_DETACH_TIMER minutes greater than  Mobile Reachability timer 
  //  */
  // ue_mm_context->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
  // ue_mm_context->mobile_reachability_timer.sec = ((mme_config.nas_config.t3412_min) + MME_APP_DELTA_T3412_REACHABILITY_TIMER) * 60;
  // ue_mm_context->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  // ue_mm_context->implicit_detach_timer.sec = (ue_mm_context->mobile_reachability_timer.sec) + MME_APP_DELTA_REACHABILITY_IMPLICIT_DETACH_TIMER * 60;


  // nas_pdn_config_rsp = &message_p->ittiMsg.nas_pdn_config_rsp;
  // nas_pdn_config_rsp->ue_id = ue_mm_context->mme_ue_s1ap_id;
  // MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_PDN_CONFIG_RESP imsi %s", ula_pP->imsi);
  // rc =  itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);

  // unlock_ue_contexts(ue_mm_context);
  // OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
// }
