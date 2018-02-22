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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assertions.h"
#include "intertask_interface.h"
#include "queue.h"
#include "NwLog.h"
#include "NwGtpv2c.h"
#include "NwGtpv2cIe.h"
#include "NwGtpv2cMsg.h"
#include "NwGtpv2cMsgParser.h"
#include "sgw_ie_defs.h"
#include "s11_common.h"
#include "s11_sgw_bearer_manager.h"
#include "s11_ie_formatter.h"
#include "log.h"

//------------------------------------------------------------------------------
int
s11_sgw_handle_modify_bearer_request (
  NwGtpv2cStackHandleT * stack_p,
  NwGtpv2cUlpApiT * pUlpApi)
{
  NwRcT                                   rc = NW_OK;
  uint8_t                                 offendingIeType,
                                          offendingIeInstance;
  uint16_t                                offendingIeLength;
  itti_s11_modify_bearer_request_t       *request_p;
  MessageDef                             *message_p;
  NwGtpv2cMsgParserT                     *pMsgParser;

  DevAssert (stack_p );
  message_p = itti_alloc_new_message (TASK_S11, S11_MODIFY_BEARER_REQUEST);
  request_p = &message_p->ittiMsg.s11_modify_bearer_request;
  memset(request_p, 0, sizeof(*request_p));
  request_p->trxn = (void *)pUlpApi->apiInfo.initialReqIndInfo.hTrxn;
  request_p->teid = nwGtpv2cMsgGetTeid (pUlpApi->hMsg);
  /*
   * Create a new message parser
   */
  rc = nwGtpv2cMsgParserNew (*stack_p, NW_GTP_MODIFY_BEARER_REQ, s11_ie_indication_generic, NULL, &pMsgParser);
  DevAssert (NW_OK == rc);
  /*
   * Indication Flags IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_INDICATION, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_indication_flags_ie_get, &request_p->indication_flags);
  DevAssert (NW_OK == rc);
  /*
   * MME-FQ-CSID IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_FQ_CSID, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_fqcsid_ie_get, &request_p->mme_fq_csid);
  DevAssert (NW_OK == rc);
  /*
   * RAT Type IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_RAT_TYPE, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_rat_type_ie_get, &request_p->rat_type);
  DevAssert (NW_OK == rc);
  /*
   * Delay Value IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_DELAY_VALUE, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_delay_value_ie_get, &request_p->delay_dl_packet_notif_req);
  DevAssert (NW_OK == rc);
  /*
   * Bearer Context to be modified IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_BEARER_CONTEXT, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_bearer_context_to_be_modified_ie_get, &request_p->bearer_contexts_to_be_modified);
  DevAssert (NW_OK == rc);
  rc = nwGtpv2cMsgParserRun (pMsgParser, pUlpApi->hMsg, &offendingIeType, &offendingIeInstance, &offendingIeLength);

  if (rc != NW_OK) {
    gtp_cause_t                             cause;
    NwGtpv2cUlpApiT                         ulp_req;

    memset (&ulp_req, 0, sizeof (NwGtpv2cUlpApiT));
    memset (&cause, 0, sizeof (gtp_cause_t));
    cause.offending_ie_type = offendingIeType;
    cause.offending_ie_length = offendingIeLength;
    cause.offending_ie_instance = offendingIeInstance;

    switch (rc) {
    case NW_GTPV2C_MANDATORY_IE_MISSING:
      OAILOG_DEBUG (LOG_S11, "Mandatory IE type '%u' of instance '%u' missing!\n", offendingIeType, offendingIeLength);
      cause.cause_value = NW_GTPV2C_CAUSE_MANDATORY_IE_MISSING;
      break;

    default:
      OAILOG_DEBUG (LOG_S11, "Unknown message parse error!\n");
      cause.cause_value = 0;
      break;
    }

    /*
     * Send Modify Bearer response with failure to Gtpv2c Stack Instance
     */
    ulp_req.apiType = NW_GTPV2C_ULP_API_TRIGGERED_RSP;
    ulp_req.apiInfo.triggeredRspInfo.hTrxn = pUlpApi->apiInfo.initialReqIndInfo.hTrxn;
    rc = nwGtpv2cMsgNew (*stack_p, NW_TRUE, NW_GTP_MODIFY_BEARER_RSP, 0, nwGtpv2cMsgGetSeqNumber (pUlpApi->hMsg), &(ulp_req.hMsg));
    s11_cause_ie_set (&(ulp_req.hMsg), &cause);
    OAILOG_DEBUG (LOG_S11, "Received NW_GTP_MODIFY_BEARER_REQ, Sending NW_GTP_MODIFY_BEARER_RSP!\n");
    rc = nwGtpv2cProcessUlpReq (*stack_p, &ulp_req);
    DevAssert (NW_OK == rc);
    itti_free (ITTI_MSG_ORIGIN_ID (message_p), message_p);
    message_p = NULL;
    rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
    DevAssert (NW_OK == rc);
    rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
    DevAssert (NW_OK == rc);
    return NW_OK;
  }

  rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
  DevAssert (NW_OK == rc);
  rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
  DevAssert (NW_OK == rc);
  return itti_send_msg_to_task (TASK_SPGW_APP, INSTANCE_DEFAULT, message_p);
}

//------------------------------------------------------------------------------
int
s11_sgw_handle_modify_bearer_response (
  NwGtpv2cStackHandleT * stack_p,
  itti_s11_modify_bearer_response_t * response_p)
{
  gtp_cause_t                             cause;
  NwRcT                                   rc;
  NwGtpv2cUlpApiT                         ulp_req;
  NwGtpv2cTrxnHandleT                     trxn;

  DevAssert (stack_p );
  DevAssert (response_p );
  trxn = (NwGtpv2cTrxnHandleT) response_p->trxn;
  /*
   * Prepare a modify bearer response to send to MME.
   */
  memset (&ulp_req, 0, sizeof (NwGtpv2cUlpApiT));
  memset (&cause, 0, sizeof (gtp_cause_t));
  ulp_req.apiType = NW_GTPV2C_ULP_API_TRIGGERED_RSP;
  ulp_req.apiInfo.triggeredRspInfo.hTrxn = trxn;
  rc = nwGtpv2cMsgNew (*stack_p, NW_TRUE, NW_GTP_MODIFY_BEARER_RSP, 0, 0, &(ulp_req.hMsg));
  DevAssert (NW_OK == rc);
  /*
   * Set the remote TEID
   */
  rc = nwGtpv2cMsgSetTeid (ulp_req.hMsg, response_p->teid);
  DevAssert (NW_OK == rc);
  cause.cause_value = (uint8_t) response_p->cause;
  s11_cause_ie_set (&(ulp_req.hMsg), &cause);
  rc = nwGtpv2cProcessUlpReq (*stack_p, &ulp_req);
  DevAssert (NW_OK == rc);
  return RETURNok;
}

//------------------------------------------------------------------------------
int
s11_sgw_handle_release_access_bearers_request (
  NwGtpv2cStackHandleT * stack_p,
  NwGtpv2cUlpApiT * pUlpApi)
{
  NwRcT                                   rc = NW_OK;
  uint8_t                                 offendingIeType,
                                          offendingIeInstance;
  uint16_t                                offendingIeLength;
  itti_s11_release_access_bearers_request_t  *request_p = NULL;
  MessageDef                             *message_p = NULL;
  NwGtpv2cMsgParserT                     *pMsgParser = NULL;

  DevAssert (stack_p );
  message_p = itti_alloc_new_message (TASK_S11, S11_RELEASE_ACCESS_BEARERS_REQUEST);
  request_p = &message_p->ittiMsg.s11_release_access_bearers_request;
  memset((void*)request_p, 0, sizeof(*request_p));

  request_p->trxn = (void *)pUlpApi->apiInfo.initialReqIndInfo.hTrxn;
  request_p->teid = nwGtpv2cMsgGetTeid (pUlpApi->hMsg);
  /*
   * Create a new message parser
   */
  rc = nwGtpv2cMsgParserNew (*stack_p, NW_GTP_RELEASE_ACCESS_BEARERS_REQ, s11_ie_indication_generic, NULL, &pMsgParser);
  DevAssert (NW_OK == rc);


  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_NODE_TYPE, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_node_type_ie_get, &request_p->originating_node);

  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_EBI, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_ebi_ie_get_list, &request_p->list_of_rabs);
  DevAssert (NW_OK == rc);

  rc = nwGtpv2cMsgParserRun (pMsgParser, pUlpApi->hMsg, &offendingIeType, &offendingIeInstance, &offendingIeLength);

  if (rc != NW_OK) {
    gtp_cause_t                             cause;
    NwGtpv2cUlpApiT                         ulp_req;

    memset (&ulp_req, 0, sizeof (NwGtpv2cUlpApiT));
    memset (&cause, 0, sizeof (gtp_cause_t));
    cause.offending_ie_type = offendingIeType;
    cause.offending_ie_length = offendingIeLength;
    cause.offending_ie_instance = offendingIeInstance;

    switch (rc) {
    case NW_GTPV2C_MANDATORY_IE_MISSING:
      OAILOG_DEBUG (LOG_S11, "Mandatory IE type '%u' of instance '%u' missing!\n", offendingIeType, offendingIeLength);
      cause.cause_value = NW_GTPV2C_CAUSE_MANDATORY_IE_MISSING;
      break;

    default:
      OAILOG_DEBUG (LOG_S11, "Unknown message parse error!\n");
      cause.cause_value = 0;
      break;
    }

    /*
     * Send Release Access bearer response with failure to Gtpv2c Stack Instance
     */
    ulp_req.apiType = NW_GTPV2C_ULP_API_TRIGGERED_RSP;
    ulp_req.apiInfo.triggeredRspInfo.hTrxn = pUlpApi->apiInfo.initialReqIndInfo.hTrxn;
    rc = nwGtpv2cMsgNew (*stack_p, NW_TRUE, NW_GTP_RELEASE_ACCESS_BEARERS_RSP, 0, nwGtpv2cMsgGetSeqNumber (pUlpApi->hMsg), &(ulp_req.hMsg));
    s11_cause_ie_set (&(ulp_req.hMsg), &cause);
    OAILOG_DEBUG (LOG_S11, "Received NW_GTP_RELEASE_ACCESS_BEARERS_REQ, Sending NW_GTP_RELEASE_ACCESS_BEARERS_RSP!\n");
    rc = nwGtpv2cProcessUlpReq (*stack_p, &ulp_req);
    DevAssert (NW_OK == rc);
    itti_free (ITTI_MSG_ORIGIN_ID (message_p), message_p);
    message_p = NULL;
    rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
    DevAssert (NW_OK == rc);
    rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
    DevAssert (NW_OK == rc);
    return RETURNok;
  }

  rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
  DevAssert (NW_OK == rc);
  rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
  DevAssert (NW_OK == rc);

  return itti_send_msg_to_task (TASK_SPGW_APP, INSTANCE_DEFAULT, message_p);
}

//------------------------------------------------------------------------------
int
s11_sgw_handle_release_access_bearers_response (
  NwGtpv2cStackHandleT * stack_p,
  itti_s11_release_access_bearers_response_t * response_p)
{
  gtp_cause_t                             cause;
  NwRcT                                   rc;
  NwGtpv2cUlpApiT                         ulp_req;
  NwGtpv2cTrxnHandleT                     trxn;

  DevAssert (stack_p );
  DevAssert (response_p );
  trxn = (NwGtpv2cTrxnHandleT) response_p->trxn;
  /*
   * Prepare a release access bearer response to send to MME.
   */
  memset (&ulp_req, 0, sizeof (NwGtpv2cUlpApiT));
  memset (&cause, 0, sizeof (gtp_cause_t));
  ulp_req.apiType = NW_GTPV2C_ULP_API_TRIGGERED_RSP;
  ulp_req.apiInfo.triggeredRspInfo.hTrxn = trxn;
  rc = nwGtpv2cMsgNew (*stack_p, NW_TRUE, NW_GTP_RELEASE_ACCESS_BEARERS_RSP, 0, 0, &(ulp_req.hMsg));
  DevAssert (NW_OK == rc);
  /*
   * Set the remote TEID
   */
  rc = nwGtpv2cMsgSetTeid (ulp_req.hMsg, response_p->teid);
  DevAssert (NW_OK == rc);
  cause.cause_value = (uint8_t) response_p->cause;
  s11_cause_ie_set (&(ulp_req.hMsg), &cause);
  rc = nwGtpv2cProcessUlpReq (*stack_p, &ulp_req);
  DevAssert (NW_OK == rc);
  return RETURNok;
}
