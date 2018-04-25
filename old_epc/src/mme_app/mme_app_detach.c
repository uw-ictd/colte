/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assertions.h"
#include "msc.h"
#include "intertask_interface.h"
#include "mme_app_ue_context.h"
#include "mme_app_itti_messaging.h"
#include "mme_app_defs.h"

//------------------------------------------------------------------------------
void
mme_app_send_delete_session_request (
  struct ue_context_s                    *ue_context_p)
{
  MessageDef                             *message_p = NULL;

  message_p = itti_alloc_new_message (TASK_MME_APP, S11_DELETE_SESSION_REQUEST);
  AssertFatal (message_p , "itti_alloc_new_message Failed");
  memset ((void *)&message_p->ittiMsg.s11_delete_session_request, 0, sizeof (itti_s11_delete_session_request_t));
  S11_DELETE_SESSION_REQUEST (message_p).local_teid = ue_context_p->mme_s11_teid;
  S11_DELETE_SESSION_REQUEST (message_p).teid = ue_context_p->sgw_s11_teid;
  S11_DELETE_SESSION_REQUEST (message_p).lbi = ue_context_p->default_bearer_id;

  OAI_GCC_DIAG_OFF(pointer-to-int-cast);
  S11_DELETE_SESSION_REQUEST (message_p).sender_fteid_for_cp.teid = (teid_t) ue_context_p;
  OAI_GCC_DIAG_ON(pointer-to-int-cast);
  S11_DELETE_SESSION_REQUEST (message_p).sender_fteid_for_cp.interface_type = S11_MME_GTP_C;
  mme_config_read_lock (&mme_config);
  S11_DELETE_SESSION_REQUEST (message_p).sender_fteid_for_cp.ipv4_address = mme_config.ipv4.s11;
  mme_config_unlock (&mme_config);
  S11_DELETE_SESSION_REQUEST (message_p).sender_fteid_for_cp.ipv4 = 1;

  /*
   * S11 stack specific parameter. Not used in standalone epc mode
   */
  S11_DELETE_SESSION_REQUEST  (message_p).trxn = NULL;
  mme_config_read_lock (&mme_config);
  S11_DELETE_SESSION_REQUEST (message_p).peer_ip = mme_config.ipv4.sgw_s11;
  mme_config_unlock (&mme_config);

  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME,
                      NULL, 0, "0  S11_DELETE_SESSION_REQUEST teid %u lbi %u",
                      S11_DELETE_SESSION_REQUEST  (message_p).teid,
                      S11_DELETE_SESSION_REQUEST  (message_p).lbi);

  itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}


//------------------------------------------------------------------------------
void
mme_app_handle_detach_req (
  const itti_nas_detach_req_t * const detach_req_p)
{
  struct ue_context_s *ue_context    = NULL;

  DevAssert(detach_req_p != NULL);
  ue_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, detach_req_p->ue_id);
  if (ue_context == NULL) {
    OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist -> Nothing to do :-) \n");
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  if ((ue_context->mme_s11_teid == 0) && (ue_context->sgw_s11_teid == 0)) {
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
    // Send a DELETE_SESSION_REQUEST message to the SGW
    mme_app_send_delete_session_request (ue_context);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
