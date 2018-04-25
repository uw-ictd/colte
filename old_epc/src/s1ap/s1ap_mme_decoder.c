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


/*! \file s1ap_mme_decoder.c
   \brief s1ap decode procedures for MME
   \author Sebastien ROUX <sebastien.roux@eurecom.fr>
   \date 2012
   \version 0.1
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "assertions.h"
#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_handlers.h"
#include "dynamic_memory_check.h"

static int
s1ap_mme_decode_initiating (
  s1ap_message *message,
  S1ap_InitiatingMessage_t *initiating_p,
  MessagesIds *message_id) {
  int                                     ret = -1;
  MessageDef                             *message_p = NULL;
  char                                   *message_string = NULL;
  size_t                                  message_string_size;
  
  OAILOG_FUNC_IN (LOG_S1AP);
 
  DevAssert (initiating_p != NULL);
  message_string = calloc (10000, sizeof (char));
  s1ap_string_total_size = 0;
  message->procedureCode = initiating_p->procedureCode;
  message->criticality = initiating_p->criticality;

  switch (initiating_p->procedureCode) {
    case S1ap_ProcedureCode_id_uplinkNASTransport: {
        ret = s1ap_decode_s1ap_uplinknastransporties (&message->msg.s1ap_UplinkNASTransportIEs, &initiating_p->value);
        s1ap_xer_print_s1ap_uplinknastransport (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_UPLINK_NAS_LOG;
      }
      break;

    case S1ap_ProcedureCode_id_S1Setup: {
        ret = s1ap_decode_s1ap_s1setuprequesties (&message->msg.s1ap_S1SetupRequestIEs, &initiating_p->value);
        s1ap_xer_print_s1ap_s1setuprequest (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_S1_SETUP_LOG;
      }
      break;

    case S1ap_ProcedureCode_id_initialUEMessage: {
        ret = s1ap_decode_s1ap_initialuemessageies (&message->msg.s1ap_InitialUEMessageIEs, &initiating_p->value);
        s1ap_xer_print_s1ap_initialuemessage (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_INITIAL_UE_MESSAGE_LOG;
      }
      break;

    case S1ap_ProcedureCode_id_UEContextReleaseRequest: {
        ret = s1ap_decode_s1ap_uecontextreleaserequesties (&message->msg.s1ap_UEContextReleaseRequestIEs, &initiating_p->value);
        s1ap_xer_print_s1ap_uecontextreleaserequest (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_UE_CONTEXT_RELEASE_REQ_LOG;
      }
      break;

    case S1ap_ProcedureCode_id_UECapabilityInfoIndication: {
        ret = s1ap_decode_s1ap_uecapabilityinfoindicationies (&message->msg.s1ap_UECapabilityInfoIndicationIEs, &initiating_p->value);
        s1ap_xer_print_s1ap_uecapabilityinfoindication (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_UE_CAPABILITY_IND_LOG;
      }
      break;

    case S1ap_ProcedureCode_id_NASNonDeliveryIndication: {
        ret = s1ap_decode_s1ap_nasnondeliveryindication_ies (&message->msg.s1ap_NASNonDeliveryIndication_IEs, &initiating_p->value);
        s1ap_xer_print_s1ap_nasnondeliveryindication_ (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_NAS_NON_DELIVERY_IND_LOG;
      }
      break;
    
    case S1ap_ProcedureCode_id_ErrorIndication: {
        OAILOG_ERROR (LOG_S1AP, "Error Indication is received. Ignoring it. Procedure code = %d\n", (int)initiating_p->procedureCode);
        ret = s1ap_decode_s1ap_errorindicationies (&message->msg.s1ap_ErrorIndicationIEs, &initiating_p->value);
        if (ret != -1) {
          ret = free_s1ap_errorindication(&message->msg.s1ap_ErrorIndicationIEs);
        }
        OAILOG_FUNC_RETURN (LOG_S1AP, ret);
      }
      break;
    
    case S1ap_ProcedureCode_id_Reset: {
        OAILOG_ERROR (LOG_S1AP, "RESET is received. Ignoring it. Procedure code = %d\n", (int)initiating_p->procedureCode);
        OAILOG_FUNC_RETURN (LOG_S1AP, ret);
        /*
         * TODO- Add handling for eNB initiated RESET message.
         */
        // ret = s1ap_decode_s1ap_reset_ies (&message->msg.s1ap_Reset_IEs, &initiating_p->value);
      }
      break;
    
    case S1ap_ProcedureCode_id_ENBConfigurationUpdate: {
        OAILOG_ERROR (LOG_S1AP, "eNB Configuration update is received. Ignoring it. Procedure code = %d\n", (int)initiating_p->procedureCode);
        OAILOG_FUNC_RETURN (LOG_S1AP, ret);
        /*
         * TODO- Add handling for eNB Configuration Update
         */
        // ret = s1ap_decode_s1ap_enbconfigurationupdate_ies (&message->msg.s1ap_ENBConfigurationUpdate_IEs, &initiating_p->value);
      }
      break;

    default: {
        OAILOG_ERROR (LOG_S1AP, "Unknown procedure ID (%d) for initiating message\n", (int)initiating_p->procedureCode);
        OAILOG_FUNC_RETURN (LOG_S1AP, ret);
      }
      break;
  }

  message_string_size = strlen (message_string);
  message_p = itti_alloc_new_message_sized (TASK_S1AP, *message_id, message_string_size + sizeof (IttiMsgText));
  message_p->ittiMsg.s1ap_uplink_nas_log.size = message_string_size;
  memcpy (&message_p->ittiMsg.s1ap_uplink_nas_log.text, message_string, message_string_size);
  itti_send_msg_to_task (TASK_UNKNOWN, INSTANCE_DEFAULT, message_p);
  free_wrapper ((void**) &message_string);
  OAILOG_FUNC_RETURN (LOG_S1AP, ret);
}

static int
s1ap_mme_decode_successfull_outcome (
  s1ap_message *message,
  S1ap_SuccessfulOutcome_t *successfullOutcome_p,
  MessagesIds *message_id) {
  int                                     ret = -1;
  MessageDef                             *message_p = NULL;
  char                                   *message_string = NULL;
  size_t                                  message_string_size = 0;
  DevAssert (successfullOutcome_p != NULL);
  message_string = calloc (10000, sizeof (char));
  s1ap_string_total_size = 0;
  message->procedureCode = successfullOutcome_p->procedureCode;
  message->criticality = successfullOutcome_p->criticality;

  switch (successfullOutcome_p->procedureCode) {
    case S1ap_ProcedureCode_id_InitialContextSetup: {
        ret = s1ap_decode_s1ap_initialcontextsetupresponseies (&message->msg.s1ap_InitialContextSetupResponseIEs, &successfullOutcome_p->value);
        s1ap_xer_print_s1ap_initialcontextsetupresponse (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_INITIAL_CONTEXT_SETUP_LOG;
      }
      break;

    case S1ap_ProcedureCode_id_UEContextRelease: {
        ret = s1ap_decode_s1ap_uecontextreleasecompleteies (&message->msg.s1ap_UEContextReleaseCompleteIEs, &successfullOutcome_p->value);
        s1ap_xer_print_s1ap_uecontextreleasecomplete (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_UE_CONTEXT_RELEASE_LOG;
      }
      break;

    default: {
        OAILOG_ERROR (LOG_S1AP, "Unknown procedure ID (%ld) for successfull outcome message\n", successfullOutcome_p->procedureCode);
      }
      break;
  }

  message_string_size = strlen (message_string);
  message_p = itti_alloc_new_message_sized (TASK_S1AP, *message_id, message_string_size + sizeof (IttiMsgText));
  message_p->ittiMsg.s1ap_initial_context_setup_log.size = message_string_size;
  memcpy (&message_p->ittiMsg.s1ap_initial_context_setup_log.text, message_string, message_string_size);
  itti_send_msg_to_task (TASK_UNKNOWN, INSTANCE_DEFAULT, message_p);
  free_wrapper ((void**) &message_string);
  return ret;
}

static int
s1ap_mme_decode_unsuccessfull_outcome (
  s1ap_message *message,
  S1ap_UnsuccessfulOutcome_t *unSuccessfulOutcome_p,
  MessagesIds *message_id) {
  int                                     ret = -1;
  MessageDef                             *message_p = NULL;
  char                                   *message_string = NULL;
  size_t                                  message_string_size = 0;
  DevAssert (unSuccessfulOutcome_p != NULL);
  message_string = calloc (10000, sizeof (char));
  s1ap_string_total_size = 0;
  message->procedureCode = unSuccessfulOutcome_p->procedureCode;
  message->criticality = unSuccessfulOutcome_p->criticality;

  switch (unSuccessfulOutcome_p->procedureCode) {
    case S1ap_ProcedureCode_id_InitialContextSetup: {
        ret = s1ap_decode_s1ap_initialcontextsetupfailureies (&message->msg.s1ap_InitialContextSetupFailureIEs, &unSuccessfulOutcome_p->value);
        s1ap_xer_print_s1ap_initialcontextsetupfailure (s1ap_xer__print2sp, message_string, message);
        *message_id = S1AP_INITIAL_CONTEXT_SETUP_LOG;
      }
      break;

    default: {
        OAILOG_ERROR (LOG_S1AP, "Unknown procedure ID (%d) for unsuccessfull outcome message\n", (int)unSuccessfulOutcome_p->procedureCode);
      }
      break;
  }

  message_string_size = strlen (message_string);
  message_p = itti_alloc_new_message_sized (TASK_S1AP, *message_id, message_string_size + sizeof (IttiMsgText));
  message_p->ittiMsg.s1ap_initial_context_setup_log.size = message_string_size;
  memcpy (&message_p->ittiMsg.s1ap_initial_context_setup_log.text, message_string, message_string_size);
  itti_send_msg_to_task (TASK_UNKNOWN, INSTANCE_DEFAULT, message_p);
  free_wrapper ((void**) &message_string);
  return ret;
}

int
s1ap_mme_decode_pdu (
  s1ap_message *message,
  const_bstring const raw,
  MessagesIds *message_id) {
  S1AP_PDU_t                              pdu = {(S1AP_PDU_PR_NOTHING)};
  S1AP_PDU_t                             *pdu_p = &pdu;
  asn_dec_rval_t                          dec_ret = {(RC_OK)};
  DevAssert (raw != NULL);
  memset ((void *)pdu_p, 0, sizeof (S1AP_PDU_t));
  dec_ret = aper_decode (NULL, &asn_DEF_S1AP_PDU, (void **)&pdu_p, bdata(raw), blength(raw), 0, 0);

  if (dec_ret.code != RC_OK) {
    OAILOG_ERROR (LOG_S1AP, "Failed to decode PDU\n");
    return -1;
  }

  message->direction = pdu_p->present;

  switch (pdu_p->present) {
    case S1AP_PDU_PR_initiatingMessage:
      return s1ap_mme_decode_initiating (message, &pdu_p->choice.initiatingMessage, message_id);

    case S1AP_PDU_PR_successfulOutcome:
      return s1ap_mme_decode_successfull_outcome (message, &pdu_p->choice.successfulOutcome, message_id);

    case S1AP_PDU_PR_unsuccessfulOutcome:
      return s1ap_mme_decode_unsuccessfull_outcome (message, &pdu_p->choice.unsuccessfulOutcome, message_id);

    default:
      OAILOG_ERROR (LOG_S1AP, "Unknown message outcome (%d) or not implemented", (int)pdu_p->present);
      break;
  }
  ASN_STRUCT_FREE(asn_DEF_S1AP_PDU, pdu_p);

  return -1;
}

int s1ap_free_mme_decode_pdu(
    s1ap_message *message, MessagesIds message_id) {
  switch(message_id) {
  case S1AP_UPLINK_NAS_LOG:
    return free_s1ap_uplinknastransport(&message->msg.s1ap_UplinkNASTransportIEs);
  case S1AP_S1_SETUP_LOG:
    return free_s1ap_s1setuprequest(&message->msg.s1ap_S1SetupRequestIEs);
  case S1AP_INITIAL_UE_MESSAGE_LOG:
    return free_s1ap_initialuemessage(&message->msg.s1ap_InitialUEMessageIEs);
  case S1AP_UE_CONTEXT_RELEASE_REQ_LOG:
    return free_s1ap_uecontextreleaserequest(&message->msg.s1ap_UEContextReleaseRequestIEs);
  case S1AP_UE_CAPABILITY_IND_LOG:
    return free_s1ap_uecapabilityinfoindication(&message->msg.s1ap_UECapabilityInfoIndicationIEs);
  case S1AP_NAS_NON_DELIVERY_IND_LOG:
    return free_s1ap_nasnondeliveryindication_(&message->msg.s1ap_NASNonDeliveryIndication_IEs);
  case S1AP_UE_CONTEXT_RELEASE_LOG:
    return free_s1ap_uecontextreleasecomplete(&message->msg.s1ap_UEContextReleaseCompleteIEs);
  case S1AP_INITIAL_CONTEXT_SETUP_LOG:
    if (message->direction == S1AP_PDU_PR_successfulOutcome) {
      return free_s1ap_initialcontextsetupresponse(&message->msg.s1ap_InitialContextSetupResponseIEs);
    } else {
      return free_s1ap_initialcontextsetupfailure(&message->msg.s1ap_InitialContextSetupFailureIEs);
    }
  default:
    DevAssert(false);

  }
}
