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
#include "hashtable.h"
#include "msc.h"

#include "NwGtpv2c.h"
#include "NwGtpv2cIe.h"
#include "NwGtpv2cMsg.h"
#include "NwGtpv2cMsgParser.h"

#include "s11_common.h"
#include "s11_mme_session_manager.h"
#include "s11_ie_formatter.h"

extern hash_table_ts_t                        *s11_mme_teid_2_gtv2c_teid_handle;

//------------------------------------------------------------------------------
int
s11_mme_create_session_request (
  NwGtpv2cStackHandleT * stack_p,
  itti_s11_create_session_request_t * req_p)
{
  NwGtpv2cUlpApiT                         ulp_req;
  NwRcT                                   rc;
  uint8_t                                 restart_counter = 0;

  DevAssert (stack_p );
  DevAssert (req_p );
  memset (&ulp_req, 0, sizeof (NwGtpv2cUlpApiT));
  ulp_req.apiType = NW_GTPV2C_ULP_API_INITIAL_REQ;
  /*
   * Prepare a new Create Session Request msg
   */
  rc = nwGtpv2cMsgNew (*stack_p, NW_TRUE, NW_GTP_CREATE_SESSION_REQ, req_p->teid, 0, &(ulp_req.hMsg));
  ulp_req.apiInfo.initialReqInfo.peerIp     = req_p->peer_ip;
  ulp_req.apiInfo.initialReqInfo.teidLocal  = req_p->sender_fteid_for_cp.teid;
  ulp_req.apiInfo.initialReqInfo.hUlpTunnel = 0;
  ulp_req.apiInfo.initialReqInfo.hTunnel    = 0;
  /*
   * Add recovery if contacting the peer for the first time
   */
  rc = nwGtpv2cMsgAddIe ((ulp_req.hMsg), NW_GTPV2C_IE_RECOVERY, 1, 0, (uint8_t *) & restart_counter);
  DevAssert (NW_OK == rc);
  /*
   * Putting the information Elements
   */
  s11_imsi_ie_set (&(ulp_req.hMsg), &req_p->imsi);
  s11_rat_type_ie_set (&(ulp_req.hMsg), &req_p->rat_type);
  s11_pdn_type_ie_set (&(ulp_req.hMsg), &req_p->pdn_type);
  /*
   * Sender F-TEID for Control Plane (MME S11)
   */
  rc = nwGtpv2cMsgAddIeFteid ((ulp_req.hMsg), NW_GTPV2C_IE_INSTANCE_ZERO,
                              S11_MME_GTP_C,
                              req_p->sender_fteid_for_cp.teid,
                              req_p->sender_fteid_for_cp.ipv4 ? ntohl(req_p->sender_fteid_for_cp.ipv4_address) : 0,
                              req_p->sender_fteid_for_cp.ipv6 ? req_p->sender_fteid_for_cp.ipv6_address : NULL);
  /*
   * The P-GW TEID should be present on the S11 interface.
   * * * * In case of an initial attach it should be set to 0...
   */
  rc = nwGtpv2cMsgAddIeFteid ((ulp_req.hMsg), NW_GTPV2C_IE_INSTANCE_ONE,
                              S5_S8_PGW_GTP_C,
                              req_p->pgw_address_for_cp.teid,
                              req_p->pgw_address_for_cp.ipv4 ? ntohl(req_p->pgw_address_for_cp.ipv4_address) : 0,
                              req_p->pgw_address_for_cp.ipv6 ? req_p->pgw_address_for_cp.ipv6_address : NULL);
  s11_apn_ie_set (&(ulp_req.hMsg), req_p->apn);
  s11_serving_network_ie_set (&(ulp_req.hMsg), &req_p->serving_network);
  s11_pco_ie_set (&(ulp_req.hMsg), &req_p->pco);
  for (int i = 0; i < req_p->bearer_contexts_to_be_created.num_bearer_context; i++) {
    s11_bearer_context_to_be_created_ie_set (&(ulp_req.hMsg), &req_p->bearer_contexts_to_be_created.bearer_contexts[i]);
  }
  rc = nwGtpv2cProcessUlpReq (*stack_p, &ulp_req);
  DevAssert (NW_OK == rc);
  MSC_LOG_TX_MESSAGE (MSC_S11_MME, MSC_SGW, NULL, 0, "0 CREATE_SESSION_REQUEST local S11 teid " TEID_FMT " num bearers ctx %u",
    req_p->sender_fteid_for_cp.teid, req_p->bearer_contexts_to_be_created.num_bearer_context);

  hashtable_rc_t hash_rc = hashtable_ts_insert(s11_mme_teid_2_gtv2c_teid_handle,
      (hash_key_t) req_p->sender_fteid_for_cp.teid,
      (void *)ulp_req.apiInfo.initialReqInfo.hTunnel);
  if (HASH_TABLE_OK == hash_rc) {
    return RETURNok;
  } else {
    OAILOG_WARNING (LOG_S11, "Could not save GTPv2-C hTunnel %p for local teid %X\n", (void*)ulp_req.apiInfo.initialReqInfo.hTunnel, ulp_req.apiInfo.initialReqInfo.teidLocal);
    return RETURNerror;
  }
}

//------------------------------------------------------------------------------
int
s11_mme_handle_create_session_response (
  NwGtpv2cStackHandleT * stack_p,
  NwGtpv2cUlpApiT * pUlpApi)
{
  NwRcT                                   rc = NW_OK;
  uint8_t                                 offendingIeType,
                                          offendingIeInstance;
  uint16_t                                offendingIeLength;
  itti_s11_create_session_response_t     *resp_p;
  MessageDef                             *message_p;
  NwGtpv2cMsgParserT                     *pMsgParser;

  DevAssert (stack_p );
  message_p = itti_alloc_new_message (TASK_S11, S11_CREATE_SESSION_RESPONSE);
  resp_p = &message_p->ittiMsg.s11_create_session_response;
  memset(resp_p, 0, sizeof(*resp_p));

  resp_p->teid = nwGtpv2cMsgGetTeid(pUlpApi->hMsg);

  /*
   * Create a new message parser
   */
  rc = nwGtpv2cMsgParserNew (*stack_p, NW_GTP_CREATE_SESSION_RSP, s11_ie_indication_generic, NULL, &pMsgParser);
  DevAssert (NW_OK == rc);
  /*
   * Cause IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_CAUSE, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_MANDATORY,
      s11_cause_ie_get, &resp_p->cause);
  DevAssert (NW_OK == rc);
  /*
   * Sender FTEID for CP IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_FTEID, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_fteid_ie_get, &resp_p->s11_sgw_teid);
  DevAssert (NW_OK == rc);
  /*
   * Sender FTEID for PGW S5/S8 IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_FTEID, NW_GTPV2C_IE_INSTANCE_ONE, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_fteid_ie_get, &resp_p->s5_s8_pgw_teid);
  DevAssert (NW_OK == rc);
  /*
   * PAA IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_PAA, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_paa_ie_get, &resp_p->paa);
  DevAssert (NW_OK == rc);
  /*
   * PCO IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_PCO, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_pco_ie_get, &resp_p->pco);
  DevAssert (NW_OK == rc);
  /*
   * Bearer Contexts Created IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_BEARER_CONTEXT, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_bearer_context_created_ie_get, &resp_p->bearer_contexts_created);
  DevAssert (NW_OK == rc);
  /*
   * Run the parser
   */
  rc = nwGtpv2cMsgParserRun (pMsgParser, (pUlpApi->hMsg), &offendingIeType, &offendingIeInstance, &offendingIeLength);

  if (rc != NW_OK) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_S11_MME, MSC_SGW, NULL, 0, "0 CREATE_SESSION_RESPONSE local S11 teid " TEID_FMT " ", resp_p->teid);
    /*
     * TODO: handle this case
     */
    itti_free (ITTI_MSG_ORIGIN_ID (message_p), message_p);
    message_p = NULL;
    rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
    DevAssert (NW_OK == rc);
    rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
    DevAssert (NW_OK == rc);
    return RETURNerror;
  }

  rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
  DevAssert (NW_OK == rc);
  rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
  DevAssert (NW_OK == rc);

  MSC_LOG_RX_MESSAGE (MSC_S11_MME, MSC_SGW, NULL, 0, "0 CREATE_SESSION_RESPONSE local S11 teid " TEID_FMT " num bearer ctxt %u", resp_p->teid,
    resp_p->bearer_contexts_created.num_bearer_context);
  return itti_send_msg_to_task (TASK_MME_APP, INSTANCE_DEFAULT, message_p);
}

//------------------------------------------------------------------------------
int
s11_mme_delete_session_request (
  NwGtpv2cStackHandleT * stack_p,
  itti_s11_delete_session_request_t * req_p)
{
  NwGtpv2cUlpApiT                         ulp_req;
  NwRcT                                   rc;
  //uint8_t                                 restart_counter = 0;

  DevAssert (stack_p );
  DevAssert (req_p );
  memset (&ulp_req, 0, sizeof (NwGtpv2cUlpApiT));
  ulp_req.apiType = NW_GTPV2C_ULP_API_INITIAL_REQ;
  /*
   * Prepare a new Delete Session Request msg
   */
  rc = nwGtpv2cMsgNew (*stack_p, NW_TRUE, NW_GTP_DELETE_SESSION_REQ, req_p->teid, 0, &(ulp_req.hMsg));
  ulp_req.apiInfo.initialReqInfo.peerIp = req_p->peer_ip;
  ulp_req.apiInfo.initialReqInfo.teidLocal = req_p->local_teid;
  hashtable_rc_t hash_rc = hashtable_ts_get(s11_mme_teid_2_gtv2c_teid_handle,
      (hash_key_t) ulp_req.apiInfo.initialReqInfo.teidLocal,
      (void **)(uintptr_t)&ulp_req.apiInfo.initialReqInfo.hTunnel);

  if (HASH_TABLE_OK != hash_rc) {
    OAILOG_WARNING (LOG_S11, "Could not get GTPv2-C hTunnel for local teid %X\n", ulp_req.apiInfo.initialReqInfo.teidLocal);
    return RETURNerror;
  }

  /*
   * Putting the information Elements
   */
  /*
   * Sender F-TEID for Control Plane (MME S11)
   */
  rc = nwGtpv2cMsgAddIeFteid ((ulp_req.hMsg), NW_GTPV2C_IE_INSTANCE_ZERO,
                              S11_MME_GTP_C,
                              req_p->sender_fteid_for_cp.teid,
                              req_p->sender_fteid_for_cp.ipv4 ? ntohl(req_p->sender_fteid_for_cp.ipv4_address) : 0,
                              req_p->sender_fteid_for_cp.ipv6 ? req_p->sender_fteid_for_cp.ipv6_address : NULL);

  s11_ebi_ie_set (&(ulp_req.hMsg), (unsigned)req_p->lbi);

  s11_indication_flags_ie_set (&(ulp_req.hMsg), &req_p->indication_flags);

  rc = nwGtpv2cProcessUlpReq (*stack_p, &ulp_req);
  DevAssert (NW_OK == rc);
  MSC_LOG_TX_MESSAGE (MSC_S11_MME, MSC_SGW, NULL, 0, "0 DELETE_SESSION_REQUEST local S11 teid " TEID_FMT " ",
		  req_p->local_teid);

  return RETURNok;
}

//------------------------------------------------------------------------------
int
s11_mme_handle_delete_session_response (
  NwGtpv2cStackHandleT * stack_p,
  NwGtpv2cUlpApiT * pUlpApi)
{
  NwRcT                                   rc = NW_OK;
  uint8_t                                 offendingIeType,
                                          offendingIeInstance;
  uint16_t                                offendingIeLength;
  itti_s11_delete_session_response_t     *resp_p;
  MessageDef                             *message_p;
  NwGtpv2cMsgParserT                     *pMsgParser;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;

  DevAssert (stack_p );
  message_p = itti_alloc_new_message (TASK_S11, S11_DELETE_SESSION_RESPONSE);
  resp_p = &message_p->ittiMsg.s11_delete_session_response;

  resp_p->teid = nwGtpv2cMsgGetTeid(pUlpApi->hMsg);

  /*
   * Create a new message parser
   */
  rc = nwGtpv2cMsgParserNew (*stack_p, NW_GTP_DELETE_SESSION_RSP, s11_ie_indication_generic, NULL, &pMsgParser);
  DevAssert (NW_OK == rc);
  /*
   * Cause IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_CAUSE, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_MANDATORY,
      s11_cause_ie_get, &resp_p->cause);
  DevAssert (NW_OK == rc);
  /*
   * Recovery IE
   */
  /* TODO rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_RECOVERY, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL, s11_fteid_ie_get,
		  &resp_p->recovery);
  DevAssert (NW_OK == rc); */
  /*
   * PCO IE
   */
  rc = nwGtpv2cMsgParserAddIe (pMsgParser, NW_GTPV2C_IE_PCO, NW_GTPV2C_IE_INSTANCE_ZERO, NW_GTPV2C_IE_PRESENCE_CONDITIONAL,
      s11_pco_ie_get, &resp_p->pco);
  DevAssert (NW_OK == rc);

  /*
   * Run the parser
   */
  rc = nwGtpv2cMsgParserRun (pMsgParser, (pUlpApi->hMsg), &offendingIeType, &offendingIeInstance, &offendingIeLength);

  if (rc != NW_OK) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_S11_MME, MSC_SGW, NULL, 0, "0 DELETE_SESSION_RESPONSE local S11 teid " TEID_FMT " ", resp_p->teid);
    /*
     * TODO: handle this case
     */
    itti_free (ITTI_MSG_ORIGIN_ID (message_p), message_p);
    message_p = NULL;
    rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
    DevAssert (NW_OK == rc);
    rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
    DevAssert (NW_OK == rc);
    return RETURNerror;
  }

  rc = nwGtpv2cMsgParserDelete (*stack_p, pMsgParser);
  DevAssert (NW_OK == rc);
  rc = nwGtpv2cMsgDelete (*stack_p, (pUlpApi->hMsg));
  DevAssert (NW_OK == rc);

  MSC_LOG_RX_MESSAGE (MSC_S11_MME, MSC_SGW, NULL, 0, "0 DELETE_SESSION_RESPONSE local S11 teid " TEID_FMT " ", resp_p->teid);

  // delete local tunnel
  NwGtpv2cUlpApiT                         ulp_req;
  memset (&ulp_req, 0, sizeof (NwGtpv2cUlpApiT));
  ulp_req.apiType = NW_GTPV2C_ULP_DELETE_LOCAL_TUNNEL;
  hash_rc = hashtable_ts_get(s11_mme_teid_2_gtv2c_teid_handle,
      (hash_key_t) resp_p->teid,
      (void **)(uintptr_t)&ulp_req.apiInfo.deleteLocalTunnelInfo.hTunnel);
  if (HASH_TABLE_OK != hash_rc) {
    OAILOG_ERROR (LOG_S11, "Could not get GTPv2-C hTunnel for local teid %X\n", resp_p->teid);
    MSC_LOG_EVENT (MSC_S11_MME, "Failed to deleted teid " TEID_FMT "", resp_p->teid);
  } else {
    rc = nwGtpv2cProcessUlpReq (*stack_p, &ulp_req);
    DevAssert (NW_OK == rc);
    MSC_LOG_EVENT (MSC_S11_MME, "Deleted teid " TEID_FMT "", resp_p->teid);
  }

  hash_rc = hashtable_ts_free(s11_mme_teid_2_gtv2c_teid_handle, (hash_key_t) resp_p->teid);

  DevAssert (HASH_TABLE_OK == hash_rc);

  return itti_send_msg_to_task (TASK_MME_APP, INSTANCE_DEFAULT, message_p);
}
