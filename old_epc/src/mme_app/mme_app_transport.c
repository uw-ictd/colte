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


/*! \file mme_app_transport.c
  \brief
  \author Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intertask_interface.h"
#include "mme_config.h"

#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "sgw_ie_defs.h"
#include "mme_app_itti_messaging.h"
#include "secu_defs.h"

#include "assertions.h"
#include "common_types.h"
#include "msc.h"
#include "log.h"

//------------------------------------------------------------------------------
int mme_app_handle_nas_dl_req (
    itti_nas_dl_data_req_t *const nas_dl_req_pP)
//------------------------------------------------------------------------------
{
  MessageDef                             *message_p    = NULL;
  int                                     rc = RETURNok;
  enb_ue_s1ap_id_t                        enb_ue_s1ap_id;

  OAILOG_FUNC_IN (LOG_MME_APP);

  message_p = itti_alloc_new_message (TASK_MME_APP, S1AP_NAS_DL_DATA_REQ);
  memset ((void *)&message_p->ittiMsg.s1ap_nas_dl_data_req, 0, sizeof (itti_s1ap_nas_dl_data_req_t));

  ue_context_t   *ue_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, nas_dl_req_pP->ue_id);
  DevAssert(ue_context != NULL);
  if (ue_context) {
    enb_ue_s1ap_id = ue_context->enb_ue_s1ap_id;
  } else {
    OAILOG_WARNING (LOG_MME_APP, " MME_APP:DOWNLINK NAS TRANSPORT. Null UE Context for mme_ue_s1ap_id %d \n", nas_dl_req_pP->ue_id);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }
  
  S1AP_NAS_DL_DATA_REQ (message_p).enb_ue_s1ap_id         = enb_ue_s1ap_id;
  S1AP_NAS_DL_DATA_REQ (message_p).mme_ue_s1ap_id         = nas_dl_req_pP->ue_id;
  S1AP_NAS_DL_DATA_REQ (message_p).nas_msg                = nas_dl_req_pP->nas_msg;

  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME,TASK_S1AP,NULL, 0,
      "0 DOWNLINK NAS TRANSPORT enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT " ue id " MME_UE_S1AP_ID_FMT " ",
      enb_ue_s1ap_id, nas_dl_req_pP->ue_id);

  rc = itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);
  /*
   * Move the UE to ECM Connected State ,if not in connected state already 
   * S1 Signaling connection gets established via first DL NAS Trasnport message in some scenarios so check the state
   * first 
   */
  if (ue_context->ecm_state != ECM_CONNECTED)
  {
    OAILOG_DEBUG (LOG_MME_APP, "MME_APP:DOWNLINK NAS TRANSPORT. Establishing S1 sig connection. mme_ue_s1ap_id = %d,enb_ue_s1ap_id = %d \n", nas_dl_req_pP->ue_id, enb_ue_s1ap_id);
    mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context, ECM_CONNECTED);
  }

  // Check the transaction status. And trigger the UE context release command accrordingly.
  if (nas_dl_req_pP->transaction_status != AS_SUCCESS) {
    ue_context->ue_context_rel_cause = S1AP_NAS_NORMAL_RELEASE;
    // Notify S1AP to send UE Context Release Command to eNB.
    mme_app_itti_ue_context_release (ue_context, ue_context->ue_context_rel_cause);
  }

  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
}

