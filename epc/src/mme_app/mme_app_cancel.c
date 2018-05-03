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
#include "mme_app_itti_messaging.h"

//------------------------------------------------------------------------------
void mme_app_handle_s6a_cancel_location_req (
  const s6a_cancel_location_req_t * const cancel_req_p)
  // const itti_nas_detach_req_t * const detach_req_p) is what we're modeling
{
  OAILOG_DEBUG (LOG_MME_APP, "SMS CLR: HANDLING CANCEL LOCATION REQUEST\n");

  struct ue_mm_context_s *ue_context = NULL;
  DevAssert(cancel_req_p != NULL);
  ue_context = mme_ue_context_exists_imsi (&mme_app_desc.mme_ue_contexts, cancel_req_p->imsi64);
  
  if (ue_context == NULL) {
    OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist -> Nothing to do. \n");
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }  

  // STEP ZERO: page the ue to wake it up if idle - can we just skip?!? Does NAS handle this?!?
  // (SMS TODO)

  // STEP ONE: send the ue a detach request message
  MessageDef *message_p;
  message_p = itti_alloc_new_message(TASK_MME_APP, S1AP_DEREGISTER_UE_REQ);
  S1AP_DEREGISTER_UE_REQ (message_p).mme_ue_s1ap_id = ue_context->mme_ue_s1ap_id;
  // MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S1AP_MME, NULL, 0, "0 S1AP_UE_CONTEXT_RELEASE_COMMAND mme_ue_s1ap_id %06" PRIX32 " ",
  //                     S1AP_UE_CONTEXT_RELEASE_COMMAND (message_p).mme_ue_s1ap_id);
  itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);

  // STEP TWO: proceed as if we've received a detach request message. This code is just copied from mme_app_handle_detach_req
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
