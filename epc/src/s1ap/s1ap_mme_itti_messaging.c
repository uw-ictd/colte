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


#include "s1ap_mme_itti_messaging.h"

//------------------------------------------------------------------------------
int
s1ap_mme_itti_send_sctp_request (
  STOLEN_REF bstring *payload,
  const sctp_assoc_id_t assoc_id,
  const sctp_stream_id_t stream,
  const mme_ue_s1ap_id_t ue_id)
{
  MessageDef                             *message_p = NULL;

  message_p = itti_alloc_new_message (TASK_S1AP, SCTP_DATA_REQ);
  SCTP_DATA_REQ (message_p).payload = *payload;
  *payload = NULL;
  SCTP_DATA_REQ (message_p).assoc_id = assoc_id;
  SCTP_DATA_REQ (message_p).stream = stream;
  SCTP_DATA_REQ (message_p).mme_ue_s1ap_id = ue_id;
  return itti_send_msg_to_task (TASK_SCTP, INSTANCE_DEFAULT, message_p);
}

//------------------------------------------------------------------------------
int
s1ap_mme_itti_nas_uplink_ind (
  const mme_ue_s1ap_id_t  ue_id,
  STOLEN_REF bstring *payload,
  const tai_t      const* tai,
  const ecgi_t     const* cgi)
{
  MessageDef                             *message_p = NULL;

  message_p = itti_alloc_new_message (TASK_S1AP, NAS_UPLINK_DATA_IND);
  NAS_UL_DATA_IND (message_p).ue_id          = ue_id;
  NAS_UL_DATA_IND (message_p).nas_msg        = *payload;
  *payload = NULL;
  NAS_UL_DATA_IND (message_p).tai            = *tai;
  NAS_UL_DATA_IND (message_p).cgi            = *cgi;

  MSC_LOG_TX_MESSAGE (MSC_S1AP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_UPLINK_DATA_IND ue_id " MME_UE_S1AP_ID_FMT " len %u",
      NAS_UL_DATA_IND (message_p).ue_id, blength(NAS_UL_DATA_IND (message_p).nas_msg));
  return itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
}

//------------------------------------------------------------------------------
int
s1ap_mme_itti_nas_downlink_cnf (
  const mme_ue_s1ap_id_t ue_id,
  const bool             is_success)
{
  MessageDef                             *message_p = NULL;

  message_p = itti_alloc_new_message (TASK_S1AP, NAS_DOWNLINK_DATA_CNF);
  NAS_DL_DATA_CNF (message_p).ue_id = ue_id;
  if (is_success) {
    NAS_DL_DATA_CNF (message_p).err_code = AS_SUCCESS;
  } else {
    NAS_DL_DATA_CNF (message_p).err_code = AS_FAILURE;
  }
  MSC_LOG_TX_MESSAGE (MSC_S1AP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_DOWNLINK_DATA_CNF ue_id " MME_UE_S1AP_ID_FMT " err_code %u",
      NAS_DL_DATA_CNF (message_p).ue_id, NAS_DL_DATA_CNF (message_p).err_code);
  return itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
}
