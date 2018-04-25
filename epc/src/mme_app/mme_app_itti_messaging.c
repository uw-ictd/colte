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

/*! \file mme_app_itti_messaging.c
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>
#include "bstrlib.h"

#include "dynamic_memory_check.h"
#include "log.h"
#include "msc.h"
#include "assertions.h"
#include "conversions.h"
#include "common_types.h"
#include "intertask_interface.h"
#include "gcc_diag.h"
#include "mme_config.h"
#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_apn_selection.h"
#include "mme_app_pdn_context.h"
#include "mme_app_bearer_context.h"
#include "sgw_ie_defs.h"
#include "common_defs.h"
#include "mme_app_itti_messaging.h"
#include "mme_app_sgw_selection.h"

//------------------------------------------------------------------------------
void mme_app_itti_ue_context_release(struct ue_mm_context_s *ue_context_p, enum s1cause cause)
{
  MessageDef *message_p;

  message_p = itti_alloc_new_message(TASK_MME_APP, S1AP_UE_CONTEXT_RELEASE_COMMAND);

  S1AP_UE_CONTEXT_RELEASE_COMMAND (message_p).mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
  S1AP_UE_CONTEXT_RELEASE_COMMAND (message_p).enb_ue_s1ap_id = ue_context_p->enb_ue_s1ap_id;
  S1AP_UE_CONTEXT_RELEASE_COMMAND (message_p).cause = cause;
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S1AP_MME, NULL, 0, "0 S1AP_UE_CONTEXT_RELEASE_COMMAND mme_ue_s1ap_id %06" PRIX32 " ",
                      S1AP_UE_CONTEXT_RELEASE_COMMAND (message_p).mme_ue_s1ap_id);
  itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
int mme_app_send_s11_release_access_bearers_req (struct ue_mm_context_s *const ue_mm_context, const pdn_cid_t pdn_index)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  /*
   * Keep the identifier to the default APN
   */
  MessageDef                             *message_p = NULL;
  itti_s11_release_access_bearers_request_t         *release_access_bearers_request_p = NULL;
  int                                     rc = RETURNok;

  DevAssert (ue_mm_context );
  message_p = itti_alloc_new_message (TASK_MME_APP, S11_RELEASE_ACCESS_BEARERS_REQUEST);
  release_access_bearers_request_p = &message_p->ittiMsg.s11_release_access_bearers_request;
  release_access_bearers_request_p->local_teid = ue_mm_context->mme_teid_s11;
  pdn_context_t * pdn_connection = ue_mm_context->pdn_contexts[pdn_index];
  release_access_bearers_request_p->teid = pdn_connection->s_gw_teid_s11_s4;
  release_access_bearers_request_p->peer_ip = pdn_connection->s_gw_address_s11_s4.address.ipv4_address;

  release_access_bearers_request_p->originating_node = NODE_TYPE_MME;


  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 S11_RELEASE_ACCESS_BEARERS_REQUEST teid %u", release_access_bearers_request_p->teid);
  rc = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
}


//------------------------------------------------------------------------------
int mme_app_send_s11_create_session_req (struct ue_mm_context_s *const ue_mm_context, const pdn_cid_t pdn_cid)
{
  OAILOG_FUNC_IN (LOG_MME_APP);

  /*
   * Keep the identifier to the default APN
   */
  MessageDef                             *message_p = NULL;
  itti_s11_create_session_request_t      *session_request_p = NULL;
  int                                     rc = RETURNok;

  DevAssert (ue_mm_context );
  OAILOG_DEBUG (LOG_MME_APP, "Handling imsi " IMSI_64_FMT "\n", ue_mm_context->emm_context._imsi64);

  if (ue_mm_context->subscriber_status != SS_SERVICE_GRANTED) {
    /*
     * HSS rejected the bearer creation or roaming is not allowed for this
     * UE. This result will trigger an ESM Failure message sent to UE.
     */
    DevMessage ("Not implemented: ACCESS NOT GRANTED, send ESM Failure to NAS\n");
  }

  message_p = itti_alloc_new_message (TASK_MME_APP, S11_CREATE_SESSION_REQUEST);
  /*
   * WARNING:
   * Some parameters should be provided by NAS Layer:
   * - ue_time_zone
   * - mei
   * - uli
   * - uci
   * Some parameters should be provided by HSS:
   * - PGW address for CP
   * - paa
   * - ambr
   * and by MME Application layer:
   * - selection_mode
   * Set these parameters with random values for now.
   */
  session_request_p = &message_p->ittiMsg.s11_create_session_request;
  /*
   * As the create session request is the first exchanged message and as
   * no tunnel had been previously setup, the distant teid is set to 0.
   * The remote teid will be provided in the response message.
   */
  session_request_p->teid = 0;
  memcpy(&session_request_p->imsi, &ue_mm_context->emm_context._imsi, sizeof(session_request_p->imsi));

  /*
   * Copy the MSISDN
   */
  if (ue_mm_context->msisdn) {
    memcpy (session_request_p->msisdn.digit, ue_mm_context->msisdn->data, ue_mm_context->msisdn->slen);
    session_request_p->msisdn.length = ue_mm_context->msisdn->slen;
  } else {
    session_request_p->msisdn.length = 0;
  }
  session_request_p->rat_type = RAT_EUTRAN;
  /*
   * Copy the subscribed ambr to the sgw create session request message
   */
  memcpy (&session_request_p->ambr, &ue_mm_context->suscribed_ue_ambr, sizeof (ambr_t));


  // default bearer already created by NAS
  bearer_context_t *bc = mme_app_get_bearer_context(ue_mm_context, ue_mm_context->pdn_contexts[pdn_cid]->default_ebi);

  bc->bearer_state   |= BEARER_STATE_MME_CREATED;

  // Zero because default bearer (see 29.274)
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.gbr.br_ul = bc->esm_ebr_context.gbr_ul;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.gbr.br_dl = bc->esm_ebr_context.gbr_dl;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.mbr.br_ul = bc->esm_ebr_context.mbr_ul;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.mbr.br_dl = bc->esm_ebr_context.mbr_dl;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.qci       = bc->qci;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.pvi       = bc->preemption_vulnerability;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.pci       = bc->preemption_capability;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.pl        = bc->priority_level;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].eps_bearer_id              = bc->ebi;
  session_request_p->bearer_contexts_to_be_created.num_bearer_context = 1;
  /*
   * Asking for default bearer in initial UE message.
   * Use the address of ue_context as unique TEID: Need to find better here
   * and will generate unique id only for 32 bits platforms.
   */
  OAI_GCC_DIAG_OFF(pointer-to-int-cast);
  session_request_p->sender_fteid_for_cp.teid = (teid_t) ue_mm_context;
  OAI_GCC_DIAG_ON(pointer-to-int-cast);
  session_request_p->sender_fteid_for_cp.interface_type = S11_MME_GTP_C;
  mme_config_read_lock (&mme_config);
  session_request_p->sender_fteid_for_cp.ipv4_address.s_addr = mme_config.ipv4.s11.s_addr;
  mme_config_unlock (&mme_config);
  session_request_p->sender_fteid_for_cp.ipv4 = 1;

  //ue_mm_context->mme_teid_s11 = session_request_p->sender_fteid_for_cp.teid;
  ue_mm_context->pdn_contexts[pdn_cid]->s_gw_teid_s11_s4 = 0;
  mme_ue_context_update_coll_keys (&mme_app_desc.mme_ue_contexts, ue_mm_context,
                                   ue_mm_context->enb_s1ap_id_key,
                                   ue_mm_context->mme_ue_s1ap_id,
                                   ue_mm_context->emm_context._imsi64,
                                   session_request_p->sender_fteid_for_cp.teid,       // mme_teid_s11 is new
                                   &ue_mm_context->emm_context._guti);
  struct apn_configuration_s *selected_apn_config_p = mme_app_get_apn_config(ue_mm_context, ue_mm_context->pdn_contexts[pdn_cid]->context_identifier);

  memcpy (session_request_p->apn, selected_apn_config_p->service_selection, selected_apn_config_p->service_selection_length);
  /*
   * Set PDN type for pdn_type and PAA even if this IE is redundant
   */
  session_request_p->pdn_type = selected_apn_config_p->pdn_type;
  session_request_p->paa.pdn_type = selected_apn_config_p->pdn_type;

  if (selected_apn_config_p->nb_ip_address == 0) {
    /*
     * UE DHCPv4 allocated ip address
     */
    session_request_p->paa.ipv4_address.s_addr = INADDR_ANY;
    session_request_p->paa.ipv6_address = in6addr_any;
  } else {
    uint8_t                                 j;

    for (j = 0; j < selected_apn_config_p->nb_ip_address; j++) {
      ip_address_t                           *ip_address = &selected_apn_config_p->ip_address[j];

      if (ip_address->pdn_type == IPv4) {
        session_request_p->paa.ipv4_address.s_addr = ip_address->address.ipv4_address.s_addr;
      } else if (ip_address->pdn_type == IPv6) {
        memcpy (&session_request_p->paa.ipv6_address, &ip_address->address.ipv6_address, sizeof(session_request_p->paa.ipv6_address));
      }
    }
  }

  if (ue_mm_context->pdn_contexts[pdn_cid]->pco) {
    copy_protocol_configuration_options (&session_request_p->pco, ue_mm_context->pdn_contexts[pdn_cid]->pco);
  }

  // TODO perform SGW selection
  // Actually, since S and P GW are bundled together, there is no PGW selection (based on PGW id in ULA, or DNS query based on FQDN)
  if (1) {
    // TODO prototype may change
    mme_app_select_sgw(&ue_mm_context->emm_context.originating_tai, &session_request_p->peer_ip);
  }

  session_request_p->serving_network.mcc[0] = ue_mm_context->e_utran_cgi.plmn.mcc_digit1;
  session_request_p->serving_network.mcc[1] = ue_mm_context->e_utran_cgi.plmn.mcc_digit2;
  session_request_p->serving_network.mcc[2] = ue_mm_context->e_utran_cgi.plmn.mcc_digit3;
  session_request_p->serving_network.mnc[0] = ue_mm_context->e_utran_cgi.plmn.mnc_digit1;
  session_request_p->serving_network.mnc[1] = ue_mm_context->e_utran_cgi.plmn.mnc_digit2;
  session_request_p->serving_network.mnc[2] = ue_mm_context->e_utran_cgi.plmn.mnc_digit3;
  session_request_p->selection_mode = MS_O_N_P_APN_S_V;
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0,
      "0 S11_CREATE_SESSION_REQUEST imsi " IMSI_64_FMT, ue_mm_context->emm_context._imsi64);
  rc = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
}
