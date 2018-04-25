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

/*! \file sgw_task.c
  \brief
  \author Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#define SGW
#define SGW_TASK_C

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "queue.h"
#include "dynamic_memory_check.h"
#include "hashtable.h"
#include "obj_hashtable.h"
#include "log.h"
#include "msc.h"
#include "intertask_interface.h"
#include "sgw_ie_defs.h"
#include "3gpp_23.401.h"
#include "mme_config.h"
#include "sgw_defs.h"
#include "sgw_handlers.h"
#include "sgw.h"
#include "spgw_config.h"
#include "pgw_lite_paa.h"

spgw_config_t                           spgw_config;
sgw_app_t                               sgw_app;
pgw_app_t                               pgw_app;

extern __pid_t g_pid;

static void sgw_exit(void);

//------------------------------------------------------------------------------
static void *sgw_intertask_interface (void *args_p)
{
  itti_mark_task_ready (TASK_SPGW_APP);
  OAILOG_START_USE ();
  MSC_START_USE ();

  while (1) {
    MessageDef                             *received_message_p = NULL;

    itti_receive_msg (TASK_SPGW_APP, &received_message_p);

    switch (ITTI_MSG_ID (received_message_p)) {
    case S11_CREATE_SESSION_REQUEST:{
        /*
         * We received a create session request from MME (with GTP abstraction here)
         * * * * procedures might be:
         * * * *      E-UTRAN Initial Attach
         * * * *      UE requests PDN connectivity
         */
        sgw_handle_create_session_request (&received_message_p->ittiMsg.s11_create_session_request);
      }
      break;

    case S11_MODIFY_BEARER_REQUEST:{
        sgw_handle_modify_bearer_request (&received_message_p->ittiMsg.s11_modify_bearer_request);
      }
      break;

    case S11_RELEASE_ACCESS_BEARERS_REQUEST:{
        sgw_handle_release_access_bearers_request (&received_message_p->ittiMsg.s11_release_access_bearers_request);
      }
      break;

    case S11_DELETE_SESSION_REQUEST:{
        sgw_handle_delete_session_request (&received_message_p->ittiMsg.s11_delete_session_request);
      }
      break;

    case GTPV1U_CREATE_TUNNEL_RESP:{
        OAILOG_DEBUG (LOG_SPGW_APP, "Received teid for S1-U: %u and status: %s\n", received_message_p->ittiMsg.gtpv1uCreateTunnelResp.S1u_teid, received_message_p->ittiMsg.gtpv1uCreateTunnelResp.status == 0 ? "Success" : "Failure");
        sgw_handle_gtpv1uCreateTunnelResp (&received_message_p->ittiMsg.gtpv1uCreateTunnelResp);
      }
      break;

    case GTPV1U_UPDATE_TUNNEL_RESP:{
        sgw_handle_gtpv1uUpdateTunnelResp (&received_message_p->ittiMsg.gtpv1uUpdateTunnelResp);
      }
      break;

    case SGI_CREATE_ENDPOINT_RESPONSE:{
        sgw_handle_sgi_endpoint_created (&received_message_p->ittiMsg.sgi_create_end_point_response);
      }
      break;

    case SGI_UPDATE_ENDPOINT_RESPONSE:{
        sgw_handle_sgi_endpoint_updated (&received_message_p->ittiMsg.sgi_update_end_point_response);
      }
      break;

    case TERMINATE_MESSAGE:{
        sgw_exit();
        itti_exit_task ();
      }
      break;

    case MESSAGE_TEST:
      OAILOG_DEBUG (LOG_SPGW_APP, "Received MESSAGE_TEST\n");
      break;

    default:{
        OAILOG_DEBUG (LOG_SPGW_APP, "Unkwnon message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
      }
      break;
    }

    itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
    received_message_p = NULL;
  }

  return NULL;
}

//------------------------------------------------------------------------------
int sgw_init (spgw_config_t *spgw_config_pP)
{
  OAILOG_DEBUG (LOG_SPGW_APP, "Initializing SPGW-APP  task interface\n");

  if ( gtpv1u_init (spgw_config_pP) < 0) {
    OAILOG_ALERT (LOG_SPGW_APP, "Initializing GTPv1-U ERROR\n");
    return RETURNerror;
  }

  pgw_load_pool_ip_addresses ();

  bstring b = bfromcstr("sgw_s11teid2mme_hashtable");
  sgw_app.s11teid2mme_hashtable = hashtable_ts_create (512, NULL, NULL, b);
  btrunc(b, 0);

  if (sgw_app.s11teid2mme_hashtable == NULL) {
    perror ("hashtable_ts_create");
    bdestroy(b);
    OAILOG_ALERT (LOG_SPGW_APP, "Initializing SPGW-APP task interface: ERROR\n");
    return RETURNerror;
  }

  /*sgw_app.s1uteid2enb_hashtable = hashtable_ts_create (512, NULL, NULL, "sgw_s1uteid2enb_hashtable");

  if (sgw_app.s1uteid2enb_hashtable == NULL) {
    perror ("hashtable_ts_create");
    OAILOG_ALERT (LOG_SPGW_APP, "Initializing SPGW-APP task interface: ERROR\n");
    return RETURNerror;
  }*/

  bassigncstr(b, "sgw_s11_bearer_context_information_hashtable");
  sgw_app.s11_bearer_context_information_hashtable = hashtable_ts_create (512, NULL,
          (void (*)(void**))sgw_cm_free_s_plus_p_gw_eps_bearer_context_information,b);
  bdestroy(b);

  if (sgw_app.s11_bearer_context_information_hashtable == NULL) {
    perror ("hashtable_ts_create");
    OAILOG_ALERT (LOG_SPGW_APP, "Initializing SPGW-APP task interface: ERROR\n");
    return RETURNerror;
  }

  sgw_app.sgw_if_name_S1u_S12_S4_up    = bstrcpy(spgw_config_pP->sgw_config.ipv4.if_name_S1u_S12_S4_up);
  sgw_app.sgw_ip_address_S1u_S12_S4_up = spgw_config_pP->sgw_config.ipv4.S1u_S12_S4_up;
  sgw_app.sgw_if_name_S11_S4           = bstrcpy(spgw_config_pP->sgw_config.ipv4.if_name_S11);
  sgw_app.sgw_ip_address_S11_S4        = spgw_config_pP->sgw_config.ipv4.S11;

  sgw_app.sgw_ip_address_S5_S8_up      = spgw_config_pP->sgw_config.ipv4.S5_S8_up;

  if (itti_create_task (TASK_SPGW_APP, &sgw_intertask_interface, NULL) < 0) {
    perror ("pthread_create");
    OAILOG_ALERT (LOG_SPGW_APP, "Initializing SPGW-APP task interface: ERROR\n");
    return RETURNerror;
  }

  FILE *fp = NULL;
  bstring  filename = bformat("/tmp/spgw_%d.status", g_pid);
  fp = fopen(bdata(filename), "w+");
  bdestroy(filename);
  fprintf(fp, "STARTED\n");
  fflush(fp);
  fclose(fp);

  OAILOG_DEBUG (LOG_SPGW_APP, "Initializing SPGW-APP task interface: DONE\n");
  return RETURNok;
}

//------------------------------------------------------------------------------
static void sgw_exit(void)
{

  if (sgw_app.s11teid2mme_hashtable) {
    hashtable_ts_destroy (sgw_app.s11teid2mme_hashtable);
  }
  /*if (sgw_app.s1uteid2enb_hashtable) {
    hashtable_destroy (sgw_app.s1uteid2enb_hashtable);
  }*/
  if (sgw_app.s11_bearer_context_information_hashtable) {
    hashtable_ts_destroy (sgw_app.s11_bearer_context_information_hashtable);
  }

  //P-GW code
  struct conf_ipv4_list_elm_s   *conf_ipv4_p = NULL;

  while ((conf_ipv4_p = STAILQ_FIRST (&spgw_config.pgw_config.ipv4_pool_list))) {
    STAILQ_REMOVE_HEAD (&spgw_config.pgw_config.ipv4_pool_list, ipv4_entries);
    free_wrapper ((void**) &conf_ipv4_p);
  }
}
