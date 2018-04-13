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
/*! \file gtpv1u_task.c
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "bstrlib.h"
#include "queue.h"

#include "dynamic_memory_check.h"
#include "log.h"
#include "assertions.h"
#include "conversions.h"
#include "3gpp_23.003.h"
#include "3gpp_24.008.h"
#include "3gpp_33.401.h"
#include "3gpp_24.007.h"
#include "3gpp_36.401.h"
#include "3gpp_36.331.h"
#include "3gpp_24.301.h"
#include "security_types.h"
#include "common_types.h"
#include "common_defs.h"
#include "intertask_interface.h"
#include "gtpv1u.h"
#include "sgw_config.h"
#include "pgw_config.h"
#include "spgw_config.h"
#include "gtpv1u_sgw_defs.h"
#include "sgw.h"

extern sgw_app_t sgw_app;

const struct gtp_tunnel_ops *gtp_tunnel_ops;

static void  *gtpv1u_thread (void *args)
{
  itti_mark_task_ready (TASK_GTPV1_U);

  gtpv1u_data_t * gtpv1u_data = (gtpv1u_data_t*)args;

  while (1) {
    /*
     * Trying to fetch a message from the message queue.
     * * * * If the queue is empty, this function will block till a
     * * * * message is sent to the task.
     */
    MessageDef                             *received_message_p = NULL;

    itti_receive_msg (TASK_GTPV1_U, &received_message_p);
    DevAssert (received_message_p != NULL);

    switch (ITTI_MSG_ID (received_message_p)) {

    case TERMINATE_MESSAGE:
      gtpv1u_exit (gtpv1u_data);
      break;

    default:{
        OAILOG_ERROR (LOG_GTPV1U , "Unkwnon message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
      }
      break;
    }

    itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
    received_message_p = NULL;
  }

  return NULL;
}

//------------------------------------------------------------------------------
int gtpv1u_init (spgw_config_t *spgw_config)
{
  int rv = 0;

  OAILOG_DEBUG (LOG_GTPV1U , "Initializing GTPV1U interface\n");
  memset (&sgw_app.gtpv1u_data, 0, sizeof (sgw_app.gtpv1u_data));
  sgw_app.gtpv1u_data.sgw_ip_address_for_S1u_S12_S4_up = sgw_app.sgw_ip_address_S1u_S12_S4_up;

  // START-GTP quick integration only for evaluation purpose

  OAILOG_DEBUG (LOG_GTPV1U , "Initializing gtp_tunnel_ops\n");
  gtp_tunnel_ops = gtp_tunnel_ops_init();
  if (gtp_tunnel_ops == NULL) {
    OAILOG_CRITICAL (LOG_GTPV1U, "ERROR in initializing gtp_tunnel_ops\n");
    return -1;
  }

  // Clean hard previous mappings.
  rv = gtp_tunnel_ops->reset();
  if (rv != 0) {
    OAILOG_CRITICAL (LOG_GTPV1U, "ERROR clean existing gtp states.\n");
    return -1;
  }
  AssertFatal(spgw_config->pgw_config.num_ue_pool == 1, "No more than 1 UE pool allowed actually");
  for (int i = 0; i < spgw_config->pgw_config.num_ue_pool; i++) {
    // GTP device uses the same MTU as SGi.
    gtp_tunnel_ops->init(&spgw_config->pgw_config.ue_pool_addr[i],
                         spgw_config->pgw_config.ue_pool_mask[i], spgw_config->pgw_config.ipv4.mtu_SGI,
                         &sgw_app.gtpv1u_data.fd0, &sgw_app.gtpv1u_data.fd1u);
  }
  // END-GTP quick integration only for evaluation purpose

  if (itti_create_task (TASK_GTPV1_U, &gtpv1u_thread, &sgw_app.gtpv1u_data) < 0) {
    OAILOG_ERROR (LOG_GTPV1U , "gtpv1u phtread_create: %s", strerror (errno));
    gtp_tunnel_ops->uninit();
    return -1;
  }

  OAILOG_DEBUG (LOG_GTPV1U , "Initializing GTPV1U interface: DONE\n");
  return 0;
}

//------------------------------------------------------------------------------
void gtpv1u_exit (gtpv1u_data_t * const gtpv1u_data)
{
  // START-GTP quick integration only for evaluation purpose
//  void * res = 0;
//  int rv  = pthread_cancel(gtpv1u_data->reader_thread);
//  if (rv != 0) {
//    OAILOG_ERROR (LOG_GTPV1U , "gtp_decaps1u pthread_cancel");
//  }
//  rv = pthread_join(gtpv1u_data->reader_thread, &res);
//  if (rv != 0)
//    OAILOG_ERROR (LOG_GTPV1U , "gtp_decaps1u pthread_join");
//
//  if (res == PTHREAD_CANCELED) {
//    OAILOG_DEBUG (LOG_GTPV1U , "gtp_decaps1u thread was canceled\n");
//  } else {
//    OAILOG_ERROR (LOG_GTPV1U , "gtp_decaps1u thread wasn't canceled\n");
//  }

  gtp_tunnel_ops->uninit();
  // END-GTP quick integration only for evaluation purpose
  itti_exit_task ();
}
