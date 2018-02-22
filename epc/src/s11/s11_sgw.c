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


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "log.h"
#include "assertions.h"
#include "queue.h"
#include "sgw_config.h"
#include "intertask_interface.h"
#include "timer.h"
#include "NwLog.h"
#include "NwGtpv2c.h"
#include "NwGtpv2cIe.h"
#include "NwGtpv2cMsg.h"
#include "sgw_ie_defs.h"
#include "s11_common.h"
#include "s11_sgw.h"
#include "s11_sgw_bearer_manager.h"
#include "s11_sgw_session_manager.h"


static NwGtpv2cStackHandleT             s11_sgw_stack_handle = 0;

/* ULP callback for the GTPv2-C stack */
//------------------------------------------------------------------------------
static NwRcT s11_sgw_ulp_process_stack_req_cb (NwGtpv2cUlpHandleT hUlp, NwGtpv2cUlpApiT * pUlpApi)
{
  int                                     ret = 0;

  DevAssert (pUlpApi );

  switch (pUlpApi->apiType) {
  case NW_GTPV2C_ULP_API_INITIAL_REQ_IND:
    OAILOG_DEBUG (LOG_S11, "Received initial req indication\n");

    switch (pUlpApi->apiInfo.initialReqIndInfo.msgType) {
    case NW_GTP_CREATE_SESSION_REQ:
      ret = s11_sgw_handle_create_session_request (&s11_sgw_stack_handle, pUlpApi);
      break;

    case NW_GTP_MODIFY_BEARER_REQ:
      ret = s11_sgw_handle_modify_bearer_request (&s11_sgw_stack_handle, pUlpApi);
      break;

    case NW_GTP_DELETE_SESSION_REQ:
      ret = s11_sgw_handle_delete_session_request (&s11_sgw_stack_handle, pUlpApi);
      break;

    case NW_GTP_RELEASE_ACCESS_BEARERS_REQ:
      ret = s11_sgw_handle_release_access_bearers_request (&s11_sgw_stack_handle, pUlpApi);
      break;

    default:
      OAILOG_WARNING (LOG_S11,  "Received unhandled message type %d\n", pUlpApi->apiInfo.initialReqIndInfo.msgType);
      break;
    }

    break;

  default:
    OAILOG_ERROR (LOG_S11, "Received unknown stack req message %d\n", pUlpApi->apiType);
    break;
  }

  return ret == -1 ? NW_FAILURE : NW_OK;
}

//------------------------------------------------------------------------------
static NwRcT s11_sgw_send_udp_msg (
  NwGtpv2cUdpHandleT udpHandle,
  uint8_t * buffer,
  uint32_t buffer_len,
  uint32_t peerIpAddr,
  uint32_t peerPort)
{
  // Create and alloc new message
  MessageDef                             *message_p;
  udp_data_req_t                         *udp_data_req_p;
  int                                     ret = 0;

  message_p = itti_alloc_new_message (TASK_S11, UDP_DATA_REQ);
  udp_data_req_p = &message_p->ittiMsg.udp_data_req;
  udp_data_req_p->peer_address = peerIpAddr;
  udp_data_req_p->peer_port = peerPort;
  udp_data_req_p->buffer = buffer;
  udp_data_req_p->buffer_length = buffer_len;
  ret = itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
  return ret == 0 ? NW_OK : NW_FAILURE;
}

//------------------------------------------------------------------------------
static NwRcT s11_sgw_log_wrapper (
  NwGtpv2cLogMgrHandleT hLogMgr,
  uint32_t logLevel,
  NwCharT * file,
  uint32_t line,
  NwCharT * logStr)
{
  OAILOG_DEBUG (LOG_S11, "%s\n", logStr);
  return NW_OK;
}

//------------------------------------------------------------------------------
static NwRcT s11_sgw_start_timer_wrapper (
  NwGtpv2cTimerMgrHandleT tmrMgrHandle,
  uint32_t timeoutSec,
  uint32_t timeoutUsec,
  uint32_t tmrType,
  void *timeoutArg,
  NwGtpv2cTimerHandleT * hTmr)
{
  long                                    timer_id;
  int                                     ret = 0;

  if (tmrType == NW_GTPV2C_TMR_TYPE_REPETITIVE) {
    ret = timer_setup (timeoutSec, timeoutUsec, TASK_S11, INSTANCE_DEFAULT, TIMER_PERIODIC, timeoutArg, &timer_id);
  } else {
    ret = timer_setup (timeoutSec, timeoutUsec, TASK_S11, INSTANCE_DEFAULT, TIMER_ONE_SHOT, timeoutArg, &timer_id);
  }

  return ret == 0 ? NW_OK : NW_FAILURE;
}

//------------------------------------------------------------------------------
static NwRcT s11_sgw_stop_timer_wrapper (
  NwGtpv2cTimerMgrHandleT tmrMgrHandle,
  NwGtpv2cTimerHandleT tmrHandle)
{
  int                                     ret;
  long                                    timer_id;

  timer_id = (long)tmrHandle;
  ret = timer_remove (timer_id);        //TODO
  return ret == 0 ? NW_OK : NW_FAILURE;
}

//------------------------------------------------------------------------------
static void *s11_sgw_thread (void *args)
{
  itti_mark_task_ready (TASK_S11);
  OAILOG_START_USE ();

  while (1) {
    MessageDef                             *received_message_p = NULL;

    itti_receive_msg (TASK_S11, &received_message_p);

    switch (ITTI_MSG_ID (received_message_p)) {
    case UDP_DATA_IND:{
        /*
         * We received new data to handle from the UDP layer
         */
        NwRcT                                   rc;
        udp_data_ind_t                         *udp_data_ind;

        udp_data_ind = &received_message_p->ittiMsg.udp_data_ind;
        OAILOG_DEBUG (LOG_S11, "Processing new data indication from UDP\n");
        rc = nwGtpv2cProcessUdpReq (s11_sgw_stack_handle, udp_data_ind->buffer, udp_data_ind->buffer_length, udp_data_ind->peer_port, udp_data_ind->peer_address);
        DevAssert (rc == NW_OK);
      }
      break;

    case S11_CREATE_SESSION_RESPONSE:{
        OAILOG_DEBUG (LOG_S11, "Received S11_CREATE_SESSION_RESPONSE from S-PGW APP\n");
        s11_sgw_handle_create_session_response (&s11_sgw_stack_handle, &received_message_p->ittiMsg.s11_create_session_response);
      }
      break;

    case S11_MODIFY_BEARER_RESPONSE:{
        OAILOG_DEBUG (LOG_S11, "Received S11_MODIFY_BEARER_RESPONSE from S-PGW APP\n");
        s11_sgw_handle_modify_bearer_response (&s11_sgw_stack_handle, &received_message_p->ittiMsg.s11_modify_bearer_response);
      }
      break;

    case S11_DELETE_SESSION_RESPONSE:{
        OAILOG_DEBUG (LOG_S11, "Received S11_DELETE_SESSION_RESPONSE from S-PGW APP\n");
        s11_sgw_handle_delete_session_response (&s11_sgw_stack_handle, &received_message_p->ittiMsg.s11_delete_session_response);
      }
      break;

    case S11_RELEASE_ACCESS_BEARERS_RESPONSE:{
        OAILOG_DEBUG (LOG_S11, "Received S11_RELEASE_ACCESS_BEARERS_RESPONSE from S-PGW APP\n");
        s11_sgw_handle_release_access_bearers_response (&s11_sgw_stack_handle, &received_message_p->ittiMsg.s11_release_access_bearers_response);
      }
      break;

    case TIMER_HAS_EXPIRED:{
        OAILOG_DEBUG (LOG_S11, "Received event TIMER_HAS_EXPIRED for timer_id 0x%lx and arg %p\n",
            received_message_p->ittiMsg.timer_has_expired.timer_id, received_message_p->ittiMsg.timer_has_expired.arg);
        DevAssert (nwGtpv2cProcessTimeout (received_message_p->ittiMsg.timer_has_expired.arg) == NW_OK);
      }
      break;

    default:{
        OAILOG_ERROR (LOG_S11, "Unkwnon message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
      }
      break;
    }

    itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
    received_message_p = NULL;
  }

  return NULL;
}

//------------------------------------------------------------------------------
static int s11_send_init_udp (char *address, uint16_t port_number)
{
  MessageDef                             *message_p;

  message_p = itti_alloc_new_message (TASK_S11, UDP_INIT);

  if (message_p == NULL) {
    return RETURNerror;
  }

  message_p->ittiMsg.udp_init.port = port_number;
  //LG message_p->ittiMsg.udpInit.address = "0.0.0.0"; //ANY address
  message_p->ittiMsg.udp_init.address = address;
  OAILOG_DEBUG (LOG_S11, "Tx UDP_INIT IP addr %s\n", message_p->ittiMsg.udp_init.address);
  return itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
}

//------------------------------------------------------------------------------
int s11_sgw_init (sgw_config_t * config_p)
{
  int                                     ret = 0;
  NwGtpv2cUlpEntityT                      ulp;
  NwGtpv2cUdpEntityT                      udp;
  NwGtpv2cTimerMgrEntityT                 tmrMgr;
  NwGtpv2cLogMgrEntityT                   logMgr;
  struct in_addr                          addr;
  char                                   *s11_address_str = NULL;

  OAILOG_DEBUG (LOG_S11, "Initializing S11 interface\n");

  if (nwGtpv2cInitialize (&s11_sgw_stack_handle) != NW_OK) {
    OAILOG_ERROR (LOG_S11, "Failed to initialize gtpv2-c stack\n");
    goto fail;
  }

  /*
   * Set ULP entity
   */
  ulp.hUlp = (NwGtpv2cUlpHandleT) NULL;
  ulp.ulpReqCallback = s11_sgw_ulp_process_stack_req_cb;
  DevAssert (NW_OK == nwGtpv2cSetUlpEntity (s11_sgw_stack_handle, &ulp));
  /*
   * Set UDP entity
   */
  udp.hUdp = (NwGtpv2cUdpHandleT) NULL;
  udp.udpDataReqCallback = s11_sgw_send_udp_msg;
  DevAssert (NW_OK == nwGtpv2cSetUdpEntity (s11_sgw_stack_handle, &udp));
  /*
   * Set Timer entity
   */
  tmrMgr.tmrMgrHandle = (NwGtpv2cTimerMgrHandleT) NULL;
  tmrMgr.tmrStartCallback = s11_sgw_start_timer_wrapper;
  tmrMgr.tmrStopCallback = s11_sgw_stop_timer_wrapper;
  DevAssert (NW_OK == nwGtpv2cSetTimerMgrEntity (s11_sgw_stack_handle, &tmrMgr));
  logMgr.logMgrHandle = 0;
  logMgr.logReqCallback = s11_sgw_log_wrapper;
  DevAssert (NW_OK == nwGtpv2cSetLogMgrEntity (s11_sgw_stack_handle, &logMgr));

  if (itti_create_task (TASK_S11, &s11_sgw_thread, NULL) < 0) {
    OAILOG_ERROR (LOG_S11, "S11 pthread_create: %s\n", strerror (errno));
    goto fail;
  }

  DevAssert (NW_OK == nwGtpv2cSetLogLevel (s11_sgw_stack_handle, NW_LOG_LEVEL_DEBG));
  sgw_config_read_lock (config_p);
  addr.s_addr = config_p->ipv4.S11;
  sgw_config_unlock (config_p);
  s11_address_str = inet_ntoa (addr);
  DevAssert (s11_address_str );
  s11_send_init_udp (s11_address_str, 2123);
  OAILOG_DEBUG (LOG_S11, "Initializing S11 interface: DONE\n");
  return ret;
fail:
  OAILOG_DEBUG (LOG_S11, "Initializing S11 interface: FAILURE\n");
  return RETURNerror;
}
