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
#include <stdlib.h>
#include <string.h>

#include "intertask_interface.h"
#include "timer.h"
#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_statistics.h"
#include "assertions.h"
#include "msc.h"

mme_app_desc_t                          mme_app_desc = {.rw_lock = PTHREAD_RWLOCK_INITIALIZER, 0} ;

void     *mme_app_thread (void *args);


void *mme_app_thread (
  void *args)
{
  struct ue_context_s                    *ue_context_p = NULL;
  itti_mark_task_ready (TASK_MME_APP);
  MSC_START_USE ();

  while (1) {
    MessageDef                             *received_message_p = NULL;

    /*
     * Trying to fetch a message from the message queue.
     * If the queue is empty, this function will block till a
     * message is sent to the task.
     */
    itti_receive_msg (TASK_MME_APP, &received_message_p);
    DevAssert (received_message_p );

    switch (ITTI_MSG_ID (received_message_p)) {

    case S6A_UPDATE_LOCATION_ANS:{
        /*
         * We received the update location answer message from HSS -> Handle it
         */
        mme_app_handle_s6a_update_location_ans (&received_message_p->ittiMsg.s6a_update_location_ans);
      }
      break;

    case S11_CREATE_SESSION_RESPONSE:{
        mme_app_handle_create_sess_resp (&received_message_p->ittiMsg.s11_create_session_response);
      }
      break;

    case S11_MODIFY_BEARER_RESPONSE:{
        ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, received_message_p->ittiMsg.s11_modify_bearer_response.teid);

        if (ue_context_p == NULL) {
          MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 MODIFY_BEARER_RESPONSE local S11 teid " TEID_FMT " ",
            received_message_p->ittiMsg.s11_modify_bearer_response.teid);
          OAILOG_WARNING (LOG_MME_APP, "We didn't find this teid in list of UE: %08x\n", received_message_p->ittiMsg.s11_modify_bearer_response.teid);
        } else {
          MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 MODIFY_BEARER_RESPONSE local S11 teid " TEID_FMT " IMSI " IMSI_64_FMT " ",
            received_message_p->ittiMsg.s11_modify_bearer_response.teid, ue_context_p->imsi);
          /*
           * Updating statistics
           */
          update_mme_app_stats_s1u_bearer_add();
        }
      }
      break;

    case S11_RELEASE_ACCESS_BEARERS_RESPONSE:{
        mme_app_handle_release_access_bearers_resp (&received_message_p->ittiMsg.s11_release_access_bearers_response);
      }
      break;

    case S11_DELETE_SESSION_RESPONSE: {
        mme_app_handle_delete_session_rsp (&received_message_p->ittiMsg.s11_delete_session_response);
      }
      break;

    case NAS_PDN_CONNECTIVITY_REQ:{
        mme_app_handle_nas_pdn_connectivity_req (&received_message_p->ittiMsg.nas_pdn_connectivity_req);
      }
      break;

    case NAS_DETACH_REQ: {
        mme_app_handle_detach_req(&received_message_p->ittiMsg.nas_detach_req);
      }
      break;

    case NAS_CONNECTION_ESTABLISHMENT_CNF:{
        mme_app_handle_conn_est_cnf (&NAS_CONNECTION_ESTABLISHMENT_CNF (received_message_p));
      }
      break;

      // From S1AP Initiating Message/EMM Attach Request
    case MME_APP_INITIAL_UE_MESSAGE:{
        mme_app_handle_initial_ue_message (&MME_APP_INITIAL_UE_MESSAGE (received_message_p));
      }
      break;

    case MME_APP_INITIAL_CONTEXT_SETUP_RSP:{
        mme_app_handle_initial_context_setup_rsp (&MME_APP_INITIAL_CONTEXT_SETUP_RSP (received_message_p));
      }
      break;
    
    case MME_APP_INITIAL_CONTEXT_SETUP_FAILURE:{
        mme_app_handle_initial_context_setup_failure (&MME_APP_INITIAL_CONTEXT_SETUP_FAILURE (received_message_p));
      }
      break;

    case TIMER_HAS_EXPIRED:{
        /*
         * Check statistic timer
         */
        if (received_message_p->ittiMsg.timer_has_expired.timer_id == mme_app_desc.statistic_timer_id) {
          mme_app_statistics_display ();
        } else if (received_message_p->ittiMsg.timer_has_expired.arg != NULL) { 
          mme_ue_s1ap_id_t mme_ue_s1ap_id = *((mme_ue_s1ap_id_t *)(received_message_p->ittiMsg.timer_has_expired.arg));
          ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, mme_ue_s1ap_id);
          if (ue_context_p == NULL) {
            OAILOG_WARNING (LOG_MME_APP, "Timer expired but no assoicated UE context for UE id %d\n",mme_ue_s1ap_id);
            break;
          }
          if (received_message_p->ittiMsg.timer_has_expired.timer_id == ue_context_p->mobile_reachability_timer.id) {
            // Mobile Reachability Timer expiry handler 
            mme_app_handle_mobile_reachability_timer_expiry (ue_context_p);
          } else if (received_message_p->ittiMsg.timer_has_expired.timer_id == ue_context_p->implicit_detach_timer.id) {
            // Implicit Detach Timer expiry handler 
            mme_app_handle_implicit_detach_timer_expiry (ue_context_p);
          } else if (received_message_p->ittiMsg.timer_has_expired.timer_id == ue_context_p->initial_context_setup_rsp_timer.id) {
            // Initial Context Setup Rsp Timer expiry handler
            mme_app_handle_initial_context_setup_rsp_timer_expiry (ue_context_p);
          } else {
            OAILOG_WARNING (LOG_MME_APP, "Timer expired but no assoicated timer_id for UE id %d\n",mme_ue_s1ap_id);
          }
        }
      }
      break;

    case TERMINATE_MESSAGE:{
        /*
         * Termination message received TODO -> release any data allocated
         */
        timer_remove(mme_app_desc.statistic_timer_id);
        hashtable_ts_destroy (mme_app_desc.mme_ue_contexts.imsi_ue_context_htbl);
        hashtable_ts_destroy (mme_app_desc.mme_ue_contexts.tun11_ue_context_htbl);
        hashtable_ts_destroy (mme_app_desc.mme_ue_contexts.mme_ue_s1ap_id_ue_context_htbl);
        hashtable_ts_destroy (mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl);
        obj_hashtable_ts_destroy (mme_app_desc.mme_ue_contexts.guti_ue_context_htbl);
        itti_exit_task ();
      }
      break;

    case S1AP_UE_CAPABILITIES_IND:{
        mme_app_handle_s1ap_ue_capabilities_ind (&received_message_p->ittiMsg.s1ap_ue_cap_ind);
      }
      break;

    case S1AP_UE_CONTEXT_RELEASE_REQ:{
        mme_app_handle_s1ap_ue_context_release_req (&received_message_p->ittiMsg.s1ap_ue_context_release_req);
      }
      break;

    case S1AP_UE_CONTEXT_RELEASE_COMPLETE:{
        mme_app_handle_s1ap_ue_context_release_complete (&received_message_p->ittiMsg.s1ap_ue_context_release_complete);
      }
      break;

    case NAS_DOWNLINK_DATA_REQ: {
        mme_app_handle_nas_dl_req (&received_message_p->ittiMsg.nas_dl_data_req);
      }
      break;

    case S1AP_ENB_DEREGISTERED_IND: {
        mme_app_handle_enb_deregister_ind(&received_message_p->ittiMsg.s1ap_eNB_deregistered_ind);
    }
    break;

    default:{
        OAILOG_DEBUG (LOG_MME_APP, "Unkwnon message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
        AssertFatal (0, "Unkwnon message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
      }
      break;
    }

    itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
    received_message_p = NULL;
  }

  return NULL;
}

int
mme_app_init (
  const mme_config_t * mme_config_p)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  memset (&mme_app_desc, 0, sizeof (mme_app_desc));
  pthread_rwlock_init (&mme_app_desc.rw_lock, NULL);
  bstring b = bfromcstr("mme_app_imsi_ue_context_htbl");
  mme_app_desc.mme_ue_contexts.imsi_ue_context_htbl = hashtable_ts_create (mme_config.max_ues, NULL, hash_free_int_func, b);
  btrunc(b, 0);
  bassigncstr(b, "mme_app_tun11_ue_context_htbl");
  mme_app_desc.mme_ue_contexts.tun11_ue_context_htbl = hashtable_ts_create (mme_config.max_ues, NULL, hash_free_int_func, b);
  AssertFatal(sizeof(uintptr_t) >= sizeof(uint64_t), "Problem with mme_ue_s1ap_id_ue_context_htbl in MME_APP");
  btrunc(b, 0);
  bassigncstr(b, "mme_app_mme_ue_s1ap_id_ue_context_htbl");
  mme_app_desc.mme_ue_contexts.mme_ue_s1ap_id_ue_context_htbl = hashtable_ts_create (mme_config.max_ues, NULL, NULL, b);
  btrunc(b, 0);
  bassigncstr(b, "mme_app_enb_ue_s1ap_id_ue_context_htbl");
  mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl = hashtable_ts_create (mme_config.max_ues, NULL, hash_free_int_func, b);
  btrunc(b, 0);
  bassigncstr(b, "mme_app_guti_ue_context_htbl");
  mme_app_desc.mme_ue_contexts.guti_ue_context_htbl = obj_hashtable_ts_create (mme_config.max_ues, NULL, hash_free_int_func, hash_free_int_func, b);
  bdestroy(b);

  /*
   * Create the thread associated with MME applicative layer
   */
  if (itti_create_task (TASK_MME_APP, &mme_app_thread, NULL) < 0) {
    OAILOG_ERROR (LOG_MME_APP, "MME APP create task failed\n");
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }

  mme_app_desc.statistic_timer_period = mme_config_p->mme_statistic_timer;

  /*
   * Request for periodic timer
   */
  if (timer_setup (mme_config_p->mme_statistic_timer, 0, TASK_MME_APP, INSTANCE_DEFAULT, TIMER_PERIODIC, NULL, &mme_app_desc.statistic_timer_id) < 0) {
    OAILOG_ERROR (LOG_MME_APP, "Failed to request new timer for statistics with %ds " "of periocidity\n", mme_config_p->mme_statistic_timer);
    mme_app_desc.statistic_timer_id = 0;
  }

  OAILOG_DEBUG (LOG_MME_APP, "Initializing MME applicative layer: DONE\n");
  OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNok);
}
