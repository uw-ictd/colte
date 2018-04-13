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

/*! \file s11_mme_task.c
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

#include "bstrlib.h"

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "hashtable.h"
#include "log.h"
#include "msc.h"
#include "mme_config.h"
#include "intertask_interface.h"
#include "itti_free_defined_msg.h"
#include "timer.h"
#include "NwLog.h"
#include "NwGtpv2c.h"
#include "NwGtpv2cMsg.h"
#include "s11_mme.h"
#include "s11_mme_session_manager.h"
#include "s11_mme_bearer_manager.h"

static nw_gtpv2c_stack_handle_t             s11_mme_stack_handle = 0;
// Store the GTPv2-C teid handle
hash_table_ts_t                        *s11_mme_teid_2_gtv2c_teid_handle = NULL;
static void s11_mme_exit (void);
//------------------------------------------------------------------------------
static nw_rc_t
s11_mme_log_wrapper (
  nw_gtpv2c_log_mgr_handle_t hLogMgr,
  uint32_t logLevel,
  char * file,
  uint32_t line,
  char * logStr)
{
  OAILOG_DEBUG (LOG_S11, "%s\n", logStr);
  return NW_OK;
}

//------------------------------------------------------------------------------
static nw_rc_t
s11_mme_ulp_process_stack_req_cb (
  nw_gtpv2c_ulp_handle_t hUlp,
  nw_gtpv2c_ulp_api_t * pUlpApi)
{
  //     NwRcT rc = NW_OK;
  int                                     ret = 0;

  DevAssert (pUlpApi );

  switch (pUlpApi->apiType) {
    case NW_GTPV2C_ULP_API_TRIGGERED_RSP_IND:
      switch (pUlpApi->u_api_info.triggeredRspIndInfo.msgType) {
      case NW_GTP_CREATE_SESSION_RSP:
        ret = s11_mme_handle_create_session_response (&s11_mme_stack_handle, pUlpApi);
        break;

      case NW_GTP_DELETE_SESSION_RSP:
        ret = s11_mme_handle_delete_session_response (&s11_mme_stack_handle, pUlpApi);
        break;

      case NW_GTP_MODIFY_BEARER_RSP:
        ret = s11_mme_handle_modify_bearer_response (&s11_mme_stack_handle, pUlpApi);
        break;

      case NW_GTP_RELEASE_ACCESS_BEARERS_RSP:
        ret = s11_mme_handle_release_access_bearer_response (&s11_mme_stack_handle, pUlpApi);
        break;

      default:
        OAILOG_WARNING (LOG_S11, "Received unhandled TRIGGERED_RSP_IND message type %d\n", pUlpApi->u_api_info.triggeredRspIndInfo.msgType);
      }
      break;

    case NW_GTPV2C_ULP_API_INITIAL_REQ_IND:
      switch (pUlpApi->u_api_info.initialReqIndInfo.msgType) {
        case NW_GTP_CREATE_BEARER_REQ:
          ret = s11_mme_handle_create_bearer_request (&s11_mme_stack_handle, pUlpApi);
          break;

        default:
          OAILOG_WARNING (LOG_S11, "Received unhandled INITIAL_REQ_IND message type %d\n", pUlpApi->u_api_info.initialReqIndInfo.msgType);
      }
      break;

    default:
      OAILOG_WARNING (LOG_S11, "Received unhandled message type %d\n", pUlpApi->apiType);
      break;
  }

  return ret == 0 ? NW_OK : NW_FAILURE;
}

//------------------------------------------------------------------------------
static nw_rc_t
s11_mme_send_udp_msg (
  nw_gtpv2c_udp_handle_t udpHandle,
  uint8_t * buffer,
  uint32_t buffer_len,
  struct in_addr *peerIpAddr,
  uint16_t peerPort)
{
  // Create and alloc new message
  MessageDef                             *message_p;
  udp_data_req_t                         *udp_data_req_p;
  int                                     ret = 0;

  message_p = itti_alloc_new_message (TASK_S11, UDP_DATA_REQ);
  udp_data_req_p = &message_p->ittiMsg.udp_data_req;
  udp_data_req_p->peer_address.s_addr = peerIpAddr->s_addr;
  udp_data_req_p->peer_port = peerPort;
  udp_data_req_p->buffer = buffer;
  udp_data_req_p->buffer_length = buffer_len;
  ret = itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
  return ((ret == 0) ? NW_OK : NW_FAILURE);
}

//------------------------------------------------------------------------------
static nw_rc_t
s11_mme_start_timer_wrapper (
  nw_gtpv2c_timer_mgr_handle_t tmrMgrHandle,
  uint32_t timeoutSec,
  uint32_t timeoutUsec,
  uint32_t tmrType,
  void *timeoutArg,
  nw_gtpv2c_timer_handle_t * hTmr)
{
  long                                    timer_id;
  int                                     ret = 0;

  if (tmrType == NW_GTPV2C_TMR_TYPE_REPETITIVE) {
    ret = timer_setup (timeoutSec, timeoutUsec, TASK_S11, INSTANCE_DEFAULT, TIMER_PERIODIC, timeoutArg, &timer_id);
  } else {
    ret = timer_setup (timeoutSec, timeoutUsec, TASK_S11, INSTANCE_DEFAULT, TIMER_ONE_SHOT, timeoutArg, &timer_id);
  }

  *hTmr = (nw_gtpv2c_timer_handle_t) timer_id;
  return ((ret == 0) ? NW_OK : NW_FAILURE);
}

//------------------------------------------------------------------------------
static nw_rc_t
s11_mme_stop_timer_wrapper (
  nw_gtpv2c_timer_mgr_handle_t tmrMgrHandle,
  nw_gtpv2c_timer_handle_t tmrHandle)
{
  long       timer_id= (long)tmrHandle;
  void      *timeoutArg = NULL;

  return ((timer_remove (timer_id, &timeoutArg) == 0) ? NW_OK : NW_FAILURE);
}

static void                            *
s11_mme_thread (
  void *args)
{
  itti_mark_task_ready (TASK_S11);

  while (1) {
    MessageDef                             *received_message_p = NULL;

    itti_receive_msg (TASK_S11, &received_message_p);
    assert (received_message_p );

    switch (ITTI_MSG_ID (received_message_p)) {
    case MESSAGE_TEST:{
        OAI_FPRINTF_INFO("TASK_S11 received MESSAGE_TEST\n");
      }
      break;

    case S11_CREATE_BEARER_RESPONSE:{
      s11_mme_create_bearer_response (&s11_mme_stack_handle, &received_message_p->ittiMsg.s11_create_bearer_response);
      }
      break;

    case S11_CREATE_SESSION_REQUEST:{
        s11_mme_create_session_request (&s11_mme_stack_handle, &received_message_p->ittiMsg.s11_create_session_request);
      }
      break;

    case S11_DELETE_SESSION_REQUEST:{
        s11_mme_delete_session_request (&s11_mme_stack_handle, &received_message_p->ittiMsg.s11_delete_session_request);
      }
      break;

    case S11_MODIFY_BEARER_REQUEST:{
        s11_mme_modify_bearer_request (&s11_mme_stack_handle, &received_message_p->ittiMsg.s11_modify_bearer_request);
      }
      break;

    case S11_RELEASE_ACCESS_BEARERS_REQUEST:{
        s11_mme_release_access_bearers_request (&s11_mme_stack_handle, &received_message_p->ittiMsg.s11_release_access_bearers_request);
      }
      break;

    case TERMINATE_MESSAGE:{
        s11_mme_exit();
        OAI_FPRINTF_INFO("TASK_S11 terminated\n");
        itti_exit_task ();
      }
      break;

    case TIMER_HAS_EXPIRED:{
        OAILOG_DEBUG (LOG_S11, "Processing timeout for timer_id 0x%lx and arg %p\n", received_message_p->ittiMsg.timer_has_expired.timer_id, received_message_p->ittiMsg.timer_has_expired.arg);
        DevAssert (nwGtpv2cProcessTimeout (received_message_p->ittiMsg.timer_has_expired.arg) == NW_OK);
      }
      break;

    case UDP_DATA_IND:{
        /*
         * We received new data to handle from the UDP layer
         */
        nw_rc_t                                   rc;
        udp_data_ind_t                         *udp_data_ind;

        udp_data_ind = &received_message_p->ittiMsg.udp_data_ind;
        rc = nwGtpv2cProcessUdpReq (s11_mme_stack_handle, udp_data_ind->buffer, udp_data_ind->buffer_length, udp_data_ind->peer_port, &udp_data_ind->peer_address);
        DevAssert (rc == NW_OK);
      }
      break;

    default:
        OAILOG_ERROR (LOG_S11, "Unkwnon message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
    }

    itti_free_msg_content(received_message_p);
    itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
    received_message_p = NULL;
  }

  return NULL;
}

//------------------------------------------------------------------------------
static int
s11_send_init_udp (
  struct in_addr *address,
  uint16_t port_number)
{
  MessageDef                             *message_p = itti_alloc_new_message (TASK_S11, UDP_INIT);
  if (message_p == NULL) {
    return RETURNerror;
  }
  message_p->ittiMsg.udp_init.port = port_number;
  message_p->ittiMsg.udp_init.address.s_addr = address->s_addr;
  char ipv4[INET_ADDRSTRLEN];
  inet_ntop (AF_INET, (void*)&message_p->ittiMsg.udp_init.address, ipv4, INET_ADDRSTRLEN);
  OAILOG_DEBUG (LOG_S11, "Tx UDP_INIT IP addr %s:%" PRIu16 "\n", ipv4, message_p->ittiMsg.udp_init.port);
  return itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
}

//------------------------------------------------------------------------------
int s11_mme_init (const mme_config_t * const mme_config_p)
{
  int                                     ret = 0;
  nw_gtpv2c_ulp_entity_t                      ulp;
  nw_gtpv2c_udp_entity_t                      udp;
  nw_gtpv2c_timer_mgr_entity_t                 tmrMgr;
  nw_gtpv2c_log_mgr_entity_t                   logMgr;

  OAILOG_DEBUG (LOG_S11, "Initializing S11 interface\n");

  if (nwGtpv2cInitialize (&s11_mme_stack_handle) != NW_OK) {
    OAILOG_ERROR (LOG_S11, "Failed to initialize gtpv2-c stack\n");
    goto fail;
  }

  /*
   * Set ULP entity
   */
  ulp.hUlp = (nw_gtpv2c_ulp_handle_t) NULL;
  ulp.ulpReqCallback = s11_mme_ulp_process_stack_req_cb;
  DevAssert (NW_OK == nwGtpv2cSetUlpEntity (s11_mme_stack_handle, &ulp));
  /*
   * Set UDP entity
   */
  udp.hUdp = (nw_gtpv2c_udp_handle_t) NULL;
  udp.udpDataReqCallback = s11_mme_send_udp_msg;
  DevAssert (NW_OK == nwGtpv2cSetUdpEntity (s11_mme_stack_handle, &udp));
  /*
   * Set Timer entity
   */
  tmrMgr.tmrMgrHandle = (nw_gtpv2c_timer_mgr_handle_t) NULL;
  tmrMgr.tmrStartCallback = s11_mme_start_timer_wrapper;
  tmrMgr.tmrStopCallback = s11_mme_stop_timer_wrapper;
  DevAssert (NW_OK == nwGtpv2cSetTimerMgrEntity (s11_mme_stack_handle, &tmrMgr));
  logMgr.logMgrHandle = 0;
  logMgr.logReqCallback = s11_mme_log_wrapper;
  DevAssert (NW_OK == nwGtpv2cSetLogMgrEntity (s11_mme_stack_handle, &logMgr));

  if (itti_create_task (TASK_S11, &s11_mme_thread, NULL) < 0) {
    OAILOG_ERROR (LOG_S11, "gtpv1u phtread_create: %s\n", strerror (errno));
    goto fail;
  }

  DevAssert (NW_OK == nwGtpv2cSetLogLevel (s11_mme_stack_handle, NW_LOG_LEVEL_DEBG));
  mme_config_read_lock (&mme_config);
  s11_send_init_udp (&mme_config.ipv4.s11, mme_config.ipv4.port_s11);
  mme_config_unlock (&mme_config);

  bstring b = bfromcstr("s11_mme_teid_2_gtv2c_teid_handle");
  s11_mme_teid_2_gtv2c_teid_handle = hashtable_ts_create(mme_config_p->max_ues, HASH_TABLE_DEFAULT_HASH_FUNC, hash_free_int_func, b);
  bdestroy_wrapper (&b);

  OAILOG_DEBUG (LOG_S11, "Initializing S11 interface: DONE\n");
  return ret;
fail:
  OAILOG_DEBUG (LOG_S11, "Initializing S11 interface: FAILURE\n");
  return RETURNerror;
}
//------------------------------------------------------------------------------
static void s11_mme_exit (void)
{
  if (nwGtpv2cFinalize(s11_mme_stack_handle) != NW_OK) {
    OAI_FPRINTF_ERR ("An error occurred during tear down of nwGtp s11 stack.\n");
  }
  if (hashtable_ts_destroy(s11_mme_teid_2_gtv2c_teid_handle) != HASH_TABLE_OK) {
    OAI_FPRINTF_ERR("An error occured while destroying s11 teid hash table");
  }
}
