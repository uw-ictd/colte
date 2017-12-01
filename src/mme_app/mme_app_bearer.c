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


/*! \file mme_app_bearer.c
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assertions.h"
#include "log.h"
#include "msc.h"
#include "conversions.h"
#include "common_types.h"
#include "intertask_interface.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_itti_messaging.h"
#include "mme_config.h"
#include "emmData.h"
#include "mme_app_statistics.h"
#include "timer.h"
#include "s1ap_mme.h"

//----------------------------------------------------------------------------
static bool mme_app_construct_guti(const plmn_t * const plmn_p, const as_stmsi_t * const s_tmsi_p,  guti_t * const guti_p);
static void notify_s1ap_new_ue_mme_s1ap_id_association (struct ue_context_s *ue_context_p);

//------------------------------------------------------------------------------
int
mme_app_send_s11_release_access_bearers_req (
  struct ue_context_s *const ue_context_pP)
{
  /*
   * Keep the identifier to the default APN
   */
  MessageDef                             *message_p = NULL;
  itti_s11_release_access_bearers_request_t         *release_access_bearers_request_p = NULL;
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_pP );
  message_p = itti_alloc_new_message (TASK_MME_APP, S11_RELEASE_ACCESS_BEARERS_REQUEST);
  release_access_bearers_request_p = &message_p->ittiMsg.s11_release_access_bearers_request;
  memset ((void*)release_access_bearers_request_p, 0, sizeof (itti_s11_release_access_bearers_request_t));
  release_access_bearers_request_p->local_teid = ue_context_pP->mme_s11_teid;
  release_access_bearers_request_p->teid = ue_context_pP->sgw_s11_teid;
  release_access_bearers_request_p->list_of_rabs.num_ebi = 1;
  release_access_bearers_request_p->list_of_rabs.ebis[0] = ue_context_pP->default_bearer_id;
  release_access_bearers_request_p->originating_node = NODE_TYPE_MME;


  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 S11_RELEASE_ACCESS_BEARERS_REQUEST teid %u ebi %u",
      release_access_bearers_request_p->teid, release_access_bearers_request_p->list_of_rabs.ebis[0]);
  rc = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
}


//------------------------------------------------------------------------------
int
mme_app_send_s11_create_session_req (
  struct ue_context_s *const ue_context_pP)
{
  uint8_t                                 i = 0;

  /*
   * Keep the identifier to the default APN
   */
  context_identifier_t                    context_identifier = 0;
  MessageDef                             *message_p = NULL;
  itti_s11_create_session_request_t      *session_request_p = NULL;
  struct apn_configuration_s             *default_apn_p = NULL;
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_pP );
  OAILOG_DEBUG (LOG_MME_APP, "Handling imsi " IMSI_64_FMT "\n", ue_context_pP->imsi);

  if (ue_context_pP->sub_status != SS_SERVICE_GRANTED) {
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
  memset (session_request_p, 0, sizeof (itti_s11_create_session_request_t));
  /*
   * As the create session request is the first exchanged message and as
   * no tunnel had been previously setup, the distant teid is set to 0.
   * The remote teid will be provided in the response message.
   */
  session_request_p->teid = 0;
  IMSI64_TO_STRING (ue_context_pP->imsi, (char *)session_request_p->imsi.digit);
  // message content was set to 0
  session_request_p->imsi.length = strlen ((const char *)session_request_p->imsi.digit);
  /*
   * Copy the MSISDN
   */
  memcpy (session_request_p->msisdn.digit, ue_context_pP->msisdn, ue_context_pP->msisdn_length);
  session_request_p->msisdn.length = ue_context_pP->msisdn_length;
  session_request_p->rat_type = RAT_EUTRAN;
  /*
   * Copy the subscribed ambr to the sgw create session request message
   */
  memcpy (&session_request_p->ambr, &ue_context_pP->subscribed_ambr, sizeof (ambr_t));

  if (ue_context_pP->apn_profile.nb_apns == 0) {
    DevMessage ("No APN returned by the HSS");
  }

  context_identifier = ue_context_pP->apn_profile.context_identifier;

  for (i = 0; i < ue_context_pP->apn_profile.nb_apns; i++) {
    default_apn_p = &ue_context_pP->apn_profile.apn_configuration[i];

    /*
     * OK we got our default APN
     */
    if (default_apn_p->context_identifier == context_identifier)
      break;
  }

  if (!default_apn_p) {
    /*
     * Unfortunately we didn't find our default APN...
     */
    DevMessage ("No default APN found");
  }

  // Zero because default bearer (see 29.274)
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.gbr.br_ul = 0;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.gbr.br_dl = 0;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.mbr.br_ul = 0;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.mbr.br_dl = 0;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.qci = default_apn_p->subscribed_qos.qci;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.pvi = default_apn_p->subscribed_qos.allocation_retention_priority.pre_emp_vulnerability;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.pci = default_apn_p->subscribed_qos.allocation_retention_priority.pre_emp_capability;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos.pl = default_apn_p->subscribed_qos.allocation_retention_priority.priority_level;
  session_request_p->bearer_contexts_to_be_created.bearer_contexts[0].eps_bearer_id = 5;
  session_request_p->bearer_contexts_to_be_created.num_bearer_context = 1;
  /*
   * Asking for default bearer in initial UE message.
   * Use the address of ue_context as unique TEID: Need to find better here
   * and will generate unique id only for 32 bits platforms.
   */
  OAI_GCC_DIAG_OFF(pointer-to-int-cast);
  session_request_p->sender_fteid_for_cp.teid = (teid_t) ue_context_pP;
  OAI_GCC_DIAG_ON(pointer-to-int-cast);
  session_request_p->sender_fteid_for_cp.interface_type = S11_MME_GTP_C;
  mme_config_read_lock (&mme_config);
  session_request_p->sender_fteid_for_cp.ipv4_address = mme_config.ipv4.s11;
  mme_config_unlock (&mme_config);
  session_request_p->sender_fteid_for_cp.ipv4 = 1;

  //ue_context_pP->mme_s11_teid = session_request_p->sender_fteid_for_cp.teid;
  ue_context_pP->sgw_s11_teid = 0;
  mme_ue_context_update_coll_keys (&mme_app_desc.mme_ue_contexts, ue_context_pP,
                                   ue_context_pP->enb_s1ap_id_key,
                                   ue_context_pP->mme_ue_s1ap_id,
                                   ue_context_pP->imsi,
                                   session_request_p->sender_fteid_for_cp.teid,       // mme_s11_teid is new
                                   &ue_context_pP->guti);
  memcpy (session_request_p->apn, default_apn_p->service_selection, default_apn_p->service_selection_length);
  /*
   * Set PDN type for pdn_type and PAA even if this IE is redundant
   */
  session_request_p->pdn_type = default_apn_p->pdn_type;
  session_request_p->paa.pdn_type = default_apn_p->pdn_type;

  if (default_apn_p->nb_ip_address == 0) {
    /*
     * UE DHCPv4 allocated ip address
     */
    memset (session_request_p->paa.ipv4_address, 0, 4);
    memset (session_request_p->paa.ipv6_address, 0, 16);
  } else {
    uint8_t                                 j;

    for (j = 0; j < default_apn_p->nb_ip_address; j++) {
      ip_address_t                           *ip_address;

      ip_address = &default_apn_p->ip_address[j];

      if (ip_address->pdn_type == IPv4) {
        memcpy (session_request_p->paa.ipv4_address, ip_address->address.ipv4_address, 4);
      } else if (ip_address->pdn_type == IPv6) {
        memcpy (session_request_p->paa.ipv6_address, ip_address->address.ipv6_address, 16);
      }
      //             free(ip_address);
    }
  }

  copy_protocol_configuration_options (&session_request_p->pco, &ue_context_pP->pending_pdn_connectivity_req_pco);
  clear_protocol_configuration_options(&ue_context_pP->pending_pdn_connectivity_req_pco);

  mme_config_read_lock (&mme_config);
  session_request_p->peer_ip = mme_config.ipv4.sgw_s11;
  mme_config_unlock (&mme_config);
  session_request_p->serving_network.mcc[0] = ue_context_pP->e_utran_cgi.plmn.mcc_digit1;
  session_request_p->serving_network.mcc[1] = ue_context_pP->e_utran_cgi.plmn.mcc_digit2;
  session_request_p->serving_network.mcc[2] = ue_context_pP->e_utran_cgi.plmn.mcc_digit3;
  session_request_p->serving_network.mnc[0] = ue_context_pP->e_utran_cgi.plmn.mnc_digit1;
  session_request_p->serving_network.mnc[1] = ue_context_pP->e_utran_cgi.plmn.mnc_digit2;
  session_request_p->serving_network.mnc[2] = ue_context_pP->e_utran_cgi.plmn.mnc_digit3;
  session_request_p->selection_mode = MS_O_N_P_APN_S_V;
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0,
      "0 S11_CREATE_SESSION_REQUEST imsi " IMSI_64_FMT, ue_context_pP->imsi);
  rc = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
}



//------------------------------------------------------------------------------
int
mme_app_handle_nas_pdn_connectivity_req (
  itti_nas_pdn_connectivity_req_t * const nas_pdn_connectivity_req_pP)
{
  struct ue_context_s                    *ue_context_p = NULL;
  imsi64_t                                imsi64 = INVALID_IMSI64;
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (nas_pdn_connectivity_req_pP );
  IMSI_STRING_TO_IMSI64 ((char *)nas_pdn_connectivity_req_pP->imsi, &imsi64);
  OAILOG_DEBUG (LOG_MME_APP, "Received NAS_PDN_CONNECTIVITY_REQ from NAS Handling imsi " IMSI_64_FMT "\n", imsi64);

  if ((ue_context_p = mme_ue_context_exists_imsi (&mme_app_desc.mme_ue_contexts, imsi64)) == NULL) {
    MSC_LOG_EVENT (MSC_MMEAPP_MME, "NAS_PDN_CONNECTIVITY_REQ Unknown imsi " IMSI_64_FMT, imsi64);
    OAILOG_ERROR (LOG_MME_APP, "That's embarrassing as we don't know this IMSI\n");
    mme_ue_context_dump_coll_keys();
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }

  /*
   * Consider the UE authenticated
   */
  ue_context_p->imsi_auth = IMSI_AUTHENTICATED;
  // Temp: save request, in near future merge wisely params in context
  memset (ue_context_p->pending_pdn_connectivity_req_imsi, 0, 16);
  AssertFatal ((nas_pdn_connectivity_req_pP->imsi_length > 0)
               && (nas_pdn_connectivity_req_pP->imsi_length < 16), "BAD IMSI LENGTH %d", nas_pdn_connectivity_req_pP->imsi_length);
  AssertFatal ((nas_pdn_connectivity_req_pP->imsi_length > 0)
               && (nas_pdn_connectivity_req_pP->imsi_length < 16), "STOP ON IMSI LENGTH %d", nas_pdn_connectivity_req_pP->imsi_length);
  memcpy (ue_context_p->pending_pdn_connectivity_req_imsi, nas_pdn_connectivity_req_pP->imsi, nas_pdn_connectivity_req_pP->imsi_length);
  ue_context_p->pending_pdn_connectivity_req_imsi_length = nas_pdn_connectivity_req_pP->imsi_length;

  // copy
  if (ue_context_p->pending_pdn_connectivity_req_apn) {
    bdestroy (ue_context_p->pending_pdn_connectivity_req_apn);
  }
  ue_context_p->pending_pdn_connectivity_req_apn =  nas_pdn_connectivity_req_pP->apn;
  nas_pdn_connectivity_req_pP->apn = NULL;

  // copy
  if (ue_context_p->pending_pdn_connectivity_req_pdn_addr) {
    bdestroy (ue_context_p->pending_pdn_connectivity_req_pdn_addr);
  }
  ue_context_p->pending_pdn_connectivity_req_pdn_addr =  nas_pdn_connectivity_req_pP->pdn_addr;
  nas_pdn_connectivity_req_pP->pdn_addr = NULL;

  ue_context_p->pending_pdn_connectivity_req_pti = nas_pdn_connectivity_req_pP->pti;
  ue_context_p->pending_pdn_connectivity_req_ue_id = nas_pdn_connectivity_req_pP->ue_id;
  copy_protocol_configuration_options (&ue_context_p->pending_pdn_connectivity_req_pco, &nas_pdn_connectivity_req_pP->pco);
  clear_protocol_configuration_options(&nas_pdn_connectivity_req_pP->pco);
#define TEMPORARY_DEBUG 1
#if TEMPORARY_DEBUG
  bstring b = protocol_configuration_options_to_xml(&ue_context_p->pending_pdn_connectivity_req_pco);
  OAILOG_DEBUG (LOG_MME_APP, "PCO %s\n", bdata(b));
  bdestroy(b);
#endif

  memcpy (&ue_context_p->pending_pdn_connectivity_req_qos, &nas_pdn_connectivity_req_pP->qos, sizeof (network_qos_t));
  ue_context_p->pending_pdn_connectivity_req_proc_data = nas_pdn_connectivity_req_pP->proc_data;
  nas_pdn_connectivity_req_pP->proc_data = NULL;
  ue_context_p->pending_pdn_connectivity_req_request_type = nas_pdn_connectivity_req_pP->request_type;
  //if ((nas_pdn_connectivity_req_pP->apn.value == NULL) || (nas_pdn_connectivity_req_pP->apn.length == 0)) {
  /*
   * TODO: Get keys...
   */
  /*
   * Now generate S6A ULR
   */
  rc =  mme_app_send_s6a_update_location_req (ue_context_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
  //} else {
  //return mme_app_send_s11_create_session_req(ue_context_p);
  //}
  //return -1;
}



// sent by NAS
//------------------------------------------------------------------------------
void
mme_app_handle_conn_est_cnf (
  const itti_nas_conn_est_cnf_t * const nas_conn_est_cnf_pP)
{
  struct ue_context_s                    *ue_context_p = NULL;
  MessageDef                             *message_p = NULL;
  itti_mme_app_connection_establishment_cnf_t *establishment_cnf_p = NULL;
  bearer_context_t                       *current_bearer_p = NULL;
  ebi_t                                   bearer_id = 0;

  OAILOG_FUNC_IN (LOG_MME_APP);
  OAILOG_DEBUG (LOG_MME_APP, "Received NAS_CONNECTION_ESTABLISHMENT_CNF from NAS\n");
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, nas_conn_est_cnf_pP->ue_id);

  if (ue_context_p == NULL) {
    MSC_LOG_EVENT (MSC_MMEAPP_MME, "NAS_CONNECTION_ESTABLISHMENT_CNF Unknown ue %u", nas_conn_est_cnf_pP->ue_id);
    OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist for UE %06" PRIX32 "/dec%u\n", nas_conn_est_cnf_pP->ue_id, nas_conn_est_cnf_pP->ue_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  message_p = itti_alloc_new_message (TASK_MME_APP, MME_APP_CONNECTION_ESTABLISHMENT_CNF);
  establishment_cnf_p = &message_p->ittiMsg.mme_app_connection_establishment_cnf;
  memset (establishment_cnf_p, 0, sizeof (itti_mme_app_connection_establishment_cnf_t));
  memcpy (&establishment_cnf_p->nas_conn_est_cnf, nas_conn_est_cnf_pP, sizeof (itti_nas_conn_est_cnf_t));

  // Copy UE radio capabilities into message if it exists
  OAILOG_DEBUG (LOG_MME_APP, "UE radio context already cached: %s\n",
               ue_context_p->ue_radio_cap_length ? "yes" : "no");
  establishment_cnf_p->ue_radio_cap_length = ue_context_p->ue_radio_cap_length;
  if (establishment_cnf_p->ue_radio_cap_length) {
    establishment_cnf_p->ue_radio_capabilities = 
                (uint8_t*) calloc (establishment_cnf_p->ue_radio_cap_length, sizeof *establishment_cnf_p->ue_radio_capabilities);
    memcpy (establishment_cnf_p->ue_radio_capabilities,
            ue_context_p->ue_radio_capabilities,
            establishment_cnf_p->ue_radio_cap_length);
  }

  bearer_id = ue_context_p->default_bearer_id;
  current_bearer_p = &ue_context_p->eps_bearers[bearer_id];
  establishment_cnf_p->eps_bearer_id = bearer_id;
  establishment_cnf_p->bearer_s1u_sgw_fteid.interface_type = S1_U_SGW_GTP_U;
  establishment_cnf_p->bearer_s1u_sgw_fteid.teid = current_bearer_p->s_gw_teid;

  if ((current_bearer_p->s_gw_address.pdn_type == IPv4)
      || (current_bearer_p->s_gw_address.pdn_type == IPv4_AND_v6)) {
    establishment_cnf_p->bearer_s1u_sgw_fteid.ipv4 = 1;
    memcpy (&establishment_cnf_p->bearer_s1u_sgw_fteid.ipv4_address, current_bearer_p->s_gw_address.address.ipv4_address, 4);
  }

  if ((current_bearer_p->s_gw_address.pdn_type == IPv6)
      || (current_bearer_p->s_gw_address.pdn_type == IPv4_AND_v6)) {
    establishment_cnf_p->bearer_s1u_sgw_fteid.ipv6 = 1;
    memcpy (establishment_cnf_p->bearer_s1u_sgw_fteid.ipv6_address, current_bearer_p->s_gw_address.address.ipv6_address, 16);
  }

  establishment_cnf_p->bearer_qos_qci = current_bearer_p->qci;
  establishment_cnf_p->bearer_qos_prio_level = current_bearer_p->prio_level;
  establishment_cnf_p->bearer_qos_pre_emp_vulnerability = current_bearer_p->pre_emp_vulnerability;
  establishment_cnf_p->bearer_qos_pre_emp_capability = current_bearer_p->pre_emp_capability;
//#pragma message  "Check ue_context_p ambr"
  establishment_cnf_p->ambr.br_ul = ue_context_p->subscribed_ambr.br_ul;
  establishment_cnf_p->ambr.br_dl = ue_context_p->subscribed_ambr.br_dl;
  establishment_cnf_p->security_capabilities_encryption_algorithms =
    nas_conn_est_cnf_pP->encryption_algorithm_capabilities;
  establishment_cnf_p->security_capabilities_integrity_algorithms =
    nas_conn_est_cnf_pP->integrity_algorithm_capabilities;
  memcpy(establishment_cnf_p->kenb, nas_conn_est_cnf_pP->kenb, AUTH_KASME_SIZE);

  OAILOG_DEBUG (LOG_MME_APP, "security_capabilities_encryption_algorithms 0x%04X\n", establishment_cnf_p->security_capabilities_encryption_algorithms);
  OAILOG_DEBUG (LOG_MME_APP, "security_capabilities_integrity_algorithms  0x%04X\n", establishment_cnf_p->security_capabilities_integrity_algorithms);

  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_S1AP_MME, NULL, 0,
                      "0 MME_APP_CONNECTION_ESTABLISHMENT_CNF ebi %u s1u_sgw teid %u qci %u prio level %u sea 0x%x sia 0x%x",
                      establishment_cnf_p->eps_bearer_id,
                      establishment_cnf_p->bearer_s1u_sgw_fteid.teid,
                      establishment_cnf_p->bearer_qos_qci, establishment_cnf_p->bearer_qos_prio_level, establishment_cnf_p->security_capabilities_encryption_algorithms, establishment_cnf_p->security_capabilities_integrity_algorithms);
  itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);

  /*
   * Move the UE to ECM Connected State.However if S1-U bearer establishment fails then we need to move the UE to idle.
   * S1 Signaling connection gets established via first DL NAS Trasnport message in some scenarios so check the state
   * first 
   */
  if (ue_context_p->ecm_state != ECM_CONNECTED)  
  {
    mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts,ue_context_p,ECM_CONNECTED);
  }

  /* Start timer to wait for Initial UE Context Response from eNB
   * If timer expires treat this as failure of ongoing procedure and abort corresponding NAS procedure such as ATTACH
   * or SERVICE REQUEST. Send UE context release command to eNB
   */
  if (timer_setup (ue_context_p->initial_context_setup_rsp_timer.sec, 0, 
                TASK_MME_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT, (void *) &(ue_context_p->mme_ue_s1ap_id), &(ue_context_p->initial_context_setup_rsp_timer.id)) < 0) { 
    OAILOG_ERROR (LOG_MME_APP, "Failed to start initial context setup response timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  } else {
    OAILOG_DEBUG (LOG_MME_APP, "MME APP : Sent Initial context Setup Request and Started guard timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

// sent by S1AP
//------------------------------------------------------------------------------
void
mme_app_handle_initial_ue_message (
  itti_mme_app_initial_ue_message_t * const initial_pP)
{
  struct ue_context_s                    *ue_context_p = NULL;
  MessageDef                             *message_p = NULL;
  bool                                    is_guti_valid = false;
  emm_data_context_t                     *ue_nas_ctx = NULL;
  enb_s1ap_id_key_t                       enb_s1ap_id_key = INVALID_ENB_UE_S1AP_ID_KEY;
  void                                   *id = NULL;
  OAILOG_FUNC_IN (LOG_MME_APP);
  OAILOG_DEBUG (LOG_MME_APP, "Received MME_APP_INITIAL_UE_MESSAGE from S1AP\n");
    
  DevAssert(INVALID_MME_UE_S1AP_ID == initial_pP->mme_ue_s1ap_id);
   
  // Check if there is any existing UE context using S-TMSI/GUTI
  if (initial_pP->is_s_tmsi_valid) 
  {
    OAILOG_DEBUG (LOG_MME_APP, "INITIAL UE Message: Valid mme_code %u and S-TMSI %u received from eNB.\n",
                  initial_pP->opt_s_tmsi.mme_code, initial_pP->opt_s_tmsi.m_tmsi);
    guti_t guti = {.gummei.plmn = {0}, .gummei.mme_gid = 0, .gummei.mme_code = 0, .m_tmsi = INVALID_M_TMSI};
    is_guti_valid = mme_app_construct_guti(&(initial_pP->tai.plmn),&(initial_pP->opt_s_tmsi),&guti);
    if (is_guti_valid)
    {
      ue_nas_ctx = emm_data_context_get_by_guti (&_emm_data, &guti);
      if (ue_nas_ctx) 
      {
        // Get the UE context using mme_ue_s1ap_id 
        ue_context_p =  mme_ue_context_exists_mme_ue_s1ap_id(&mme_app_desc.mme_ue_contexts,ue_nas_ctx->ue_id);
        DevAssert(ue_context_p != NULL);
        if ((ue_context_p != NULL) && (ue_context_p->mme_ue_s1ap_id == ue_nas_ctx->ue_id)) {
          initial_pP->mme_ue_s1ap_id = ue_nas_ctx->ue_id;
          if (ue_context_p->enb_s1ap_id_key != INVALID_ENB_UE_S1AP_ID_KEY)
          {
            /*
             * Ideally this should never happen. When UE move to IDLE this key is set to INVALID.
             * Note - This can happen if eNB detects RLF late and by that time UE sends Initial NAS message via new RRC
             * connection 
             * However if this key is valid, remove the key from the hashtable.
             */

            OAILOG_ERROR (LOG_MME_APP, "MME_APP_INITAIL_UE_MESSAGE.ERROR***** enb_s1ap_id_key %ld has valid value.\n" ,ue_context_p->enb_s1ap_id_key);
            hashtable_ts_remove (mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->enb_s1ap_id_key, (void **)&id);
            ue_context_p->enb_s1ap_id_key = INVALID_ENB_UE_S1AP_ID_KEY;
          }
          // Update MME UE context with new enb_ue_s1ap_id
          ue_context_p->enb_ue_s1ap_id = initial_pP->enb_ue_s1ap_id;
          // regenerate the enb_s1ap_id_key as enb_ue_s1ap_id is changed.
          MME_APP_ENB_S1AP_ID_KEY(enb_s1ap_id_key, initial_pP->enb_id, initial_pP->enb_ue_s1ap_id);
          // Update enb_s1ap_id_key in hashtable  
          mme_ue_context_update_coll_keys( &mme_app_desc.mme_ue_contexts,
                ue_context_p,
                enb_s1ap_id_key,
                ue_nas_ctx->ue_id,
                ue_nas_ctx->_imsi64,
                ue_context_p->mme_s11_teid,
                &guti);
        }
      } else {
          OAILOG_DEBUG (LOG_MME_APP, "MME_APP_INITIAL_UE_MESSAGE with mme code %u and S-TMSI %u:"
            "no UE context found \n", initial_pP->opt_s_tmsi.mme_code, initial_pP->opt_s_tmsi.m_tmsi);
      }
    } else {
      OAILOG_DEBUG (LOG_MME_APP, "No MME is configured with MME code %u received in S-TMSI %u from UE.\n",
                    initial_pP->opt_s_tmsi.mme_code, initial_pP->opt_s_tmsi.m_tmsi);
    }
  } else {
    OAILOG_DEBUG (LOG_MME_APP, "MME_APP_INITIAL_UE_MESSAGE from S1AP,without S-TMSI. \n");
  }
  // create a new ue context if nothing is found
  if (!(ue_context_p)) {
    OAILOG_DEBUG (LOG_MME_APP, "UE context doesn't exist -> create one\n");
    if ((ue_context_p = mme_create_new_ue_context ()) == NULL) {
      /*
       * Error during ue context malloc
       */
      DevMessage ("mme_create_new_ue_context");
      OAILOG_FUNC_OUT (LOG_MME_APP);
    }
    // Allocate new mme_ue_s1ap_id
    ue_context_p->mme_ue_s1ap_id    = mme_app_ctx_get_new_ue_id ();
    if (ue_context_p->mme_ue_s1ap_id  == INVALID_MME_UE_S1AP_ID) {
      OAILOG_CRITICAL (LOG_MME_APP, "MME_APP_INITIAL_UE_MESSAGE. MME_UE_S1AP_ID allocation Failed.\n");
      mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context_p);
      OAILOG_FUNC_OUT (LOG_MME_APP);
    }
    OAILOG_DEBUG (LOG_MME_APP, "MME_APP_INITAIL_UE_MESSAGE.Allocated new MME UE context and new mme_ue_s1ap_id. %d\n",ue_context_p->mme_ue_s1ap_id);
    ue_context_p->enb_ue_s1ap_id    = initial_pP->enb_ue_s1ap_id;
    MME_APP_ENB_S1AP_ID_KEY(ue_context_p->enb_s1ap_id_key, initial_pP->enb_id, initial_pP->enb_ue_s1ap_id);
    DevAssert (mme_insert_ue_context (&mme_app_desc.mme_ue_contexts, ue_context_p) == 0);
  }
  ue_context_p->sctp_assoc_id_key = initial_pP->sctp_assoc_id;
  ue_context_p->e_utran_cgi = initial_pP->cgi;
  // Notify S1AP about the mapping between mme_ue_s1ap_id and sctp assoc id + enb_ue_s1ap_id 
  notify_s1ap_new_ue_mme_s1ap_id_association (ue_context_p);
  // Initialize timers to INVALID IDs
  ue_context_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
  ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  ue_context_p->initial_context_setup_rsp_timer.sec = MME_APP_INITIAL_CONTEXT_SETUP_RSP_TIMER_VALUE;

  message_p = itti_alloc_new_message (TASK_MME_APP, NAS_INITIAL_UE_MESSAGE);
  // do this because of same message types name but not same struct in different .h
  message_p->ittiMsg.nas_initial_ue_message.nas.ue_id           = ue_context_p->mme_ue_s1ap_id;
  message_p->ittiMsg.nas_initial_ue_message.nas.tai             = initial_pP->tai;
  message_p->ittiMsg.nas_initial_ue_message.nas.cgi             = initial_pP->cgi;
  message_p->ittiMsg.nas_initial_ue_message.nas.as_cause        = initial_pP->as_cause;
  if (initial_pP->is_s_tmsi_valid) {
    message_p->ittiMsg.nas_initial_ue_message.nas.s_tmsi        = initial_pP->opt_s_tmsi;
  } else {
    message_p->ittiMsg.nas_initial_ue_message.nas.s_tmsi.mme_code = 0;
    message_p->ittiMsg.nas_initial_ue_message.nas.s_tmsi.m_tmsi   = INVALID_M_TMSI;
  }
  message_p->ittiMsg.nas_initial_ue_message.nas.initial_nas_msg   =  initial_pP->nas;
  memcpy (&message_p->ittiMsg.nas_initial_ue_message.transparent, (const void*)&initial_pP->transparent, sizeof (message_p->ittiMsg.nas_initial_ue_message.transparent));
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_INITIAL_UE_MESSAGE");
  itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_delete_session_rsp (
  const itti_s11_delete_session_response_t * const delete_sess_resp_pP)
//------------------------------------------------------------------------------
{
  struct ue_context_s                    *ue_context_p = NULL;
  void                                   *id = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (delete_sess_resp_pP );
  OAILOG_DEBUG (LOG_MME_APP, "Received S11_DELETE_SESSION_RESPONSE from S+P-GW with teid " TEID_FMT "\n ",delete_sess_resp_pP->teid);
  ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, delete_sess_resp_pP->teid);

  if (ue_context_p == NULL) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 DELETE_SESSION_RESPONSE local S11 teid " TEID_FMT " ", delete_sess_resp_pP->teid);
    OAILOG_WARNING (LOG_MME_APP, "We didn't find this teid in list of UE: %08x\n", delete_sess_resp_pP->teid);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  hashtable_ts_remove(mme_app_desc.mme_ue_contexts.tun11_ue_context_htbl,
                      (const hash_key_t) ue_context_p->mme_s11_teid, &id);
  ue_context_p->mme_s11_teid = 0;
  ue_context_p->sgw_s11_teid = 0;

  if (delete_sess_resp_pP->cause != REQUEST_ACCEPTED) {
    OAILOG_WARNING (LOG_MME_APP, "***WARNING****S11 Delete Session Rsp: NACK received from SPGW : %08x\n", delete_sess_resp_pP->teid);
  }
  MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 DELETE_SESSION_RESPONSE local S11 teid " TEID_FMT " IMSI " IMSI_64_FMT " ",
    delete_sess_resp_pP->teid, ue_context_p->imsi);
  /*
   * Updating statistics
   */
  update_mme_app_stats_s1u_bearer_sub();
  update_mme_app_stats_default_bearer_sub();
  
  /*
   * If UE is already in idle state, skip asking eNB to release UE context and just clean up locally.
   * This can happen during implicit detach and UE initiated detach when UE sends detach req (type = switch off)
   */
  if (ECM_IDLE == ue_context_p->ecm_state) {
    ue_context_p->ue_context_rel_cause = S1AP_IMPLICIT_CONTEXT_RELEASE;
    // Notify S1AP to release S1AP UE context locally.
    mme_app_itti_ue_context_release (ue_context_p, ue_context_p->ue_context_rel_cause);
    // Free MME UE Context   
    mme_notify_ue_context_released (&mme_app_desc.mme_ue_contexts, ue_context_p);
    mme_remove_ue_context (&mme_app_desc.mme_ue_contexts, ue_context_p);
  } else {
    if (ue_context_p->ue_context_rel_cause == S1AP_INVALID_CAUSE) {
      ue_context_p->ue_context_rel_cause = S1AP_NAS_DETACH;
    }
    // Notify S1AP to send UE Context Release Command to eNB or free s1 context locally.
    mme_app_itti_ue_context_release (ue_context_p, ue_context_p->ue_context_rel_cause);
    ue_context_p->ue_context_rel_cause = S1AP_INVALID_CAUSE;
  }

  OAILOG_FUNC_OUT (LOG_MME_APP);
}


//------------------------------------------------------------------------------
int
mme_app_handle_create_sess_resp (
  itti_s11_create_session_response_t * const create_sess_resp_pP)
{
  struct ue_context_s                    *ue_context_p = NULL;
  bearer_context_t                       *current_bearer_p = NULL;
  MessageDef                             *message_p = NULL;
  int16_t                                 bearer_id =0;
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (create_sess_resp_pP );
  OAILOG_DEBUG (LOG_MME_APP, "Received S11_CREATE_SESSION_RESPONSE from S+P-GW\n");
  ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, create_sess_resp_pP->teid);

  if (ue_context_p == NULL) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 CREATE_SESSION_RESPONSE local S11 teid " TEID_FMT " ", create_sess_resp_pP->teid);

    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this teid in list of UE: %08x\n", create_sess_resp_pP->teid);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }
  MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 CREATE_SESSION_RESPONSE local S11 teid " TEID_FMT " IMSI " IMSI_64_FMT " ",
    create_sess_resp_pP->teid, ue_context_p->imsi);

  /* Whether SGW has created the session (IP address allocation, local GTP-U end point creation etc.) 
   * successfully or not , it is indicated by cause value in create session response message.
   * If cause value is not equal to "REQUEST_ACCEPTED" then this implies that SGW could not allocate the resources for
   * the requested session. In this case, MME-APP sends PDN Connectivity fail message to NAS-ESM with the "cause" received
   * in S11 Session Create Response message. 
   * NAS-ESM maps this "S11 cause" to "ESM cause" and sends it in PDN Connectivity Reject message to the UE.
   */

  if (create_sess_resp_pP->cause != REQUEST_ACCEPTED) {
   // Send PDN CONNECTIVITY FAIL message  to NAS layer 
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_PDN_CONNECTIVITY_FAIL);
    itti_nas_pdn_connectivity_fail_t *nas_pdn_connectivity_fail = &message_p->ittiMsg.nas_pdn_connectivity_fail;
    memset ((void *)nas_pdn_connectivity_fail, 0, sizeof (itti_nas_pdn_connectivity_fail_t));
    nas_pdn_connectivity_fail->pti = ue_context_p->pending_pdn_connectivity_req_pti;  
    nas_pdn_connectivity_fail->ue_id = ue_context_p->pending_pdn_connectivity_req_ue_id; 
    nas_pdn_connectivity_fail->cause = (pdn_conn_rsp_cause_t)(create_sess_resp_pP->cause); 
    rc = itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
  }


  /*
   * Store the S-GW teid
   */
  ue_context_p->sgw_s11_teid = create_sess_resp_pP->s11_sgw_teid.teid;
  //---------------------------------------------------------
  // Process itti_sgw_create_session_response_t.bearer_context_created
  //---------------------------------------------------------
  bearer_id = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].eps_bearer_id /* - 5 */ ;
  /*
   * Depending on s11 result we have to send reject or accept for bearers
   */
  DevCheck ((bearer_id < BEARERS_PER_UE)
            && (bearer_id >= 0), bearer_id, BEARERS_PER_UE, 0);
  ue_context_p->default_bearer_id = bearer_id;

  if (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].cause != REQUEST_ACCEPTED) {
    DevMessage ("Cases where bearer cause != REQUEST_ACCEPTED are not handled\n");
  }

  DevAssert (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.interface_type == S1_U_SGW_GTP_U);
  /*
   * Updating statistics
   */
  update_mme_app_stats_default_bearer_add();

  current_bearer_p = &ue_context_p->eps_bearers[bearer_id];
  current_bearer_p->s_gw_teid = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.teid;

  switch (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv4 +
      (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv6 << 1)) {
  default:
  case 0:{
      /*
       * No address provided: impossible case
       */
      DevMessage ("No ip address for user-plane provided...\n");
    }
    break;

  case 1:{
      /*
       * Only IPv4 address
       */
      current_bearer_p->s_gw_address.pdn_type = IPv4;
      memcpy (current_bearer_p->s_gw_address.address.ipv4_address, &create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv4_address, 4);
    }
    break;

  case 2:{
      /*
       * Only IPv6 address
       */
      current_bearer_p->s_gw_address.pdn_type = IPv6;
      memcpy (current_bearer_p->s_gw_address.address.ipv6_address, create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv6_address, 16);
    }
    break;

  case 3:{
      /*
       * Both IPv4 and Ipv6
       */
      current_bearer_p->s_gw_address.pdn_type = IPv4_AND_v6;
      memcpy (current_bearer_p->s_gw_address.address.ipv4_address, &create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv4_address, 4);
      memcpy (current_bearer_p->s_gw_address.address.ipv6_address, create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv6_address, 16);
    }
    break;
  }

  current_bearer_p->p_gw_teid = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].s5_s8_u_pgw_fteid.teid;
  memset (&current_bearer_p->p_gw_address, 0, sizeof (ip_address_t));

  if (create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].bearer_level_qos ) {
    current_bearer_p->qci = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].bearer_level_qos->qci;
    current_bearer_p->prio_level = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].bearer_level_qos->pl;
    current_bearer_p->pre_emp_vulnerability = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].bearer_level_qos->pvi;
    current_bearer_p->pre_emp_capability = create_sess_resp_pP->bearer_contexts_created.bearer_contexts[0].bearer_level_qos->pci;
    OAILOG_DEBUG (LOG_MME_APP, "Set qci %u in bearer %u\n", current_bearer_p->qci, ue_context_p->default_bearer_id);
  } else {
    // if null, it is not modified
    //current_bearer_p->qci                    = ue_context_p->pending_pdn_connectivity_req_qos.qci;
//#pragma message  "may force QCI here to 9"
    current_bearer_p->qci = 9;
    current_bearer_p->prio_level = 1;
    current_bearer_p->pre_emp_vulnerability = PRE_EMPTION_VULNERABILITY_ENABLED;
    current_bearer_p->pre_emp_capability = PRE_EMPTION_CAPABILITY_ENABLED;
    OAILOG_DEBUG (LOG_MME_APP, "Set qci %u in bearer %u (qos not modified by S/P-GW)\n", current_bearer_p->qci, ue_context_p->default_bearer_id);
  }

  mme_app_dump_ue_contexts (&mme_app_desc.mme_ue_contexts);
  {
    //uint8_t *keNB = NULL;
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_PDN_CONNECTIVITY_RSP);
    itti_nas_pdn_connectivity_rsp_t *nas_pdn_connectivity_rsp = &message_p->ittiMsg.nas_pdn_connectivity_rsp;
    memset ((void *)nas_pdn_connectivity_rsp, 0, sizeof (itti_nas_pdn_connectivity_rsp_t));
    // moved to NAS_CONNECTION_ESTABLISHMENT_CONF, keNB not handled in NAS MME
    //derive_keNB(ue_context_p->vector_in_use->kasme, 156, &keNB);
    //memcpy(NAS_PDN_CONNECTIVITY_RSP(message_p).keNB, keNB, 32);
    //free(keNB);
    nas_pdn_connectivity_rsp->pti = ue_context_p->pending_pdn_connectivity_req_pti;  // NAS internal ref
    nas_pdn_connectivity_rsp->ue_id = ue_context_p->pending_pdn_connectivity_req_ue_id;      // NAS internal ref

    // TO REWORK:
    if (ue_context_p->pending_pdn_connectivity_req_apn) {
      nas_pdn_connectivity_rsp->apn = bstrcpy (ue_context_p->pending_pdn_connectivity_req_apn);
      OAILOG_DEBUG (LOG_MME_APP, "SET APN FROM NAS PDN CONNECTIVITY CREATE: %s\n", bdata(nas_pdn_connectivity_rsp->apn));
    } else {
      int                                     i;
      context_identifier_t                    context_identifier = ue_context_p->apn_profile.context_identifier;

      for (i = 0; i < ue_context_p->apn_profile.nb_apns; i++) {
        if (ue_context_p->apn_profile.apn_configuration[i].context_identifier == context_identifier) {
          AssertFatal (ue_context_p->apn_profile.apn_configuration[i].service_selection_length > 0, "Bad APN string (len = 0)");

          if (ue_context_p->apn_profile.apn_configuration[i].service_selection_length > 0) {
            nas_pdn_connectivity_rsp->apn = blk2bstr(ue_context_p->apn_profile.apn_configuration[i].service_selection,
                ue_context_p->apn_profile.apn_configuration[i].service_selection_length);
            AssertFatal (ue_context_p->apn_profile.apn_configuration[i].service_selection_length <= APN_MAX_LENGTH, "Bad APN string length %d",
                ue_context_p->apn_profile.apn_configuration[i].service_selection_length);

            OAILOG_DEBUG (LOG_MME_APP, "SET APN FROM HSS ULA: %s\n", bdata(nas_pdn_connectivity_rsp->apn));
            break;
          }
        }
      }
    }

    OAILOG_DEBUG (LOG_MME_APP, "APN: %s\n", bdata(nas_pdn_connectivity_rsp->apn));

    switch (create_sess_resp_pP->paa.pdn_type) {
    case IPv4:
      nas_pdn_connectivity_rsp->pdn_addr = blk2bstr(create_sess_resp_pP->paa.ipv4_address, 4);
      DevAssert (nas_pdn_connectivity_rsp->pdn_addr);
      break;

    case IPv6:
      DevAssert (create_sess_resp_pP->paa.ipv6_prefix_length == 64);    // NAS seems to only support 64 bits
      nas_pdn_connectivity_rsp->pdn_addr = blk2bstr(create_sess_resp_pP->paa.ipv6_address, create_sess_resp_pP->paa.ipv6_prefix_length / 8);
      DevAssert (nas_pdn_connectivity_rsp->pdn_addr);
      break;

    case IPv4_AND_v6:
      DevAssert (create_sess_resp_pP->paa.ipv6_prefix_length == 64);    // NAS seems to only support 64 bits
      nas_pdn_connectivity_rsp->pdn_addr = blk2bstr(create_sess_resp_pP->paa.ipv4_address, 4 + create_sess_resp_pP->paa.ipv6_prefix_length / 8);
      DevAssert (nas_pdn_connectivity_rsp->pdn_addr);
      bcatblk(nas_pdn_connectivity_rsp->pdn_addr, create_sess_resp_pP->paa.ipv6_address, create_sess_resp_pP->paa.ipv6_prefix_length / 8);
      break;

    case IPv4_OR_v6:
      nas_pdn_connectivity_rsp->pdn_addr = blk2bstr(create_sess_resp_pP->paa.ipv4_address, 4);
      DevAssert (nas_pdn_connectivity_rsp->pdn_addr);
      break;

    default:
      DevAssert (0);
    }

    nas_pdn_connectivity_rsp->pdn_type = create_sess_resp_pP->paa.pdn_type;
    nas_pdn_connectivity_rsp->proc_data = ue_context_p->pending_pdn_connectivity_req_proc_data;      // NAS internal ref
    ue_context_p->pending_pdn_connectivity_req_proc_data = NULL;
//#pragma message  "QOS hardcoded here"
    //memcpy(&NAS_PDN_CONNECTIVITY_RSP(message_p).qos,
    //        &ue_context_p->pending_pdn_connectivity_req_qos,
    //        sizeof(network_qos_t));
    nas_pdn_connectivity_rsp->qos.gbrUL = 64;        /* 64=64kb/s   Guaranteed Bit Rate for uplink   */
    nas_pdn_connectivity_rsp->qos.gbrDL = 120;       /* 120=512kb/s Guaranteed Bit Rate for downlink */
    nas_pdn_connectivity_rsp->qos.mbrUL = 72;        /* 72=128kb/s   Maximum Bit Rate for uplink      */
    nas_pdn_connectivity_rsp->qos.mbrDL = 135;       /*135=1024kb/s Maximum Bit Rate for downlink    */
    /*
     * Note : Above values are insignificant because bearer with QCI = 9 is NON-GBR bearer and ESM would not include GBR and MBR values
     * in Activate Default EPS Bearer Context Setup Request message 
     */ 
    nas_pdn_connectivity_rsp->qos.qci = 9;   /* QoS Class Identifier                           */
    nas_pdn_connectivity_rsp->request_type = ue_context_p->pending_pdn_connectivity_req_request_type;        // NAS internal ref
    ue_context_p->pending_pdn_connectivity_req_request_type = 0;
    // here at this point OctetString are saved in resp, no loss of memory (apn, pdn_addr)
    nas_pdn_connectivity_rsp->ue_id = ue_context_p->mme_ue_s1ap_id;
    nas_pdn_connectivity_rsp->ebi = bearer_id;
    nas_pdn_connectivity_rsp->qci = current_bearer_p->qci;
    nas_pdn_connectivity_rsp->prio_level = current_bearer_p->prio_level;
    nas_pdn_connectivity_rsp->pre_emp_vulnerability = current_bearer_p->pre_emp_vulnerability;
    nas_pdn_connectivity_rsp->pre_emp_capability = current_bearer_p->pre_emp_capability;
    nas_pdn_connectivity_rsp->sgw_s1u_teid = current_bearer_p->s_gw_teid;
    memcpy (&nas_pdn_connectivity_rsp->sgw_s1u_address, &current_bearer_p->s_gw_address, sizeof (ip_address_t));
    nas_pdn_connectivity_rsp->ambr.br_ul = ue_context_p->subscribed_ambr.br_ul;
    nas_pdn_connectivity_rsp->ambr.br_dl = ue_context_p->subscribed_ambr.br_dl;
    copy_protocol_configuration_options (&nas_pdn_connectivity_rsp->pco, &create_sess_resp_pP->pco);
    clear_protocol_configuration_options(&create_sess_resp_pP->pco);

    MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_PDN_CONNECTIVITY_RSP sgw_s1u_teid %u ebi %u qci %u prio %u", current_bearer_p->s_gw_teid, bearer_id, current_bearer_p->qci, current_bearer_p->prio_level);

    rc = itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN (LOG_MME_APP, rc);
  }
  OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNok);
}


//------------------------------------------------------------------------------
void
mme_app_handle_initial_context_setup_rsp (
  const itti_mme_app_initial_context_setup_rsp_t * const initial_ctxt_setup_rsp_pP)
{
  struct ue_context_s                    *ue_context_p = NULL;
  MessageDef                             *message_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  OAILOG_DEBUG (LOG_MME_APP, "Received MME_APP_INITIAL_CONTEXT_SETUP_RSP from S1AP\n");
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, initial_ctxt_setup_rsp_pP->mme_ue_s1ap_id);

  if (ue_context_p == NULL) {
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this mme_ue_s1ap_id in list of UE: %08x %d(dec)\n", initial_ctxt_setup_rsp_pP->mme_ue_s1ap_id, initial_ctxt_setup_rsp_pP->mme_ue_s1ap_id);
    MSC_LOG_EVENT (MSC_MMEAPP_MME, "MME_APP_INITIAL_CONTEXT_SETUP_RSP Unknown ue %u", initial_ctxt_setup_rsp_pP->mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  // Stop Initial context setup process guard timer,if running 
  if (ue_context_p->initial_context_setup_rsp_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->initial_context_setup_rsp_timer.id)) {
      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Initial Context Setup Rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  }
  message_p = itti_alloc_new_message (TASK_MME_APP, S11_MODIFY_BEARER_REQUEST);
  AssertFatal (message_p , "itti_alloc_new_message Failed");
  itti_s11_modify_bearer_request_t *s11_modify_bearer_request = &message_p->ittiMsg.s11_modify_bearer_request;
  memset ((void *)s11_modify_bearer_request, 0, sizeof (*s11_modify_bearer_request));
  s11_modify_bearer_request->peer_ip = mme_config.ipv4.sgw_s11;
  s11_modify_bearer_request->teid = ue_context_p->sgw_s11_teid;
  s11_modify_bearer_request->local_teid = ue_context_p->mme_s11_teid;
  /*
   * Delay Value in integer multiples of 50 millisecs, or zero
   */
  s11_modify_bearer_request->delay_dl_packet_notif_req = 0;  // TO DO
  s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id = initial_ctxt_setup_rsp_pP->eps_bearer_id;
  memcpy (&s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[0].s1_eNB_fteid,
      &initial_ctxt_setup_rsp_pP->bearer_s1u_enb_fteid,
      sizeof (s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[0].s1_eNB_fteid));
  s11_modify_bearer_request->bearer_contexts_to_be_modified.num_bearer_context = 1;

  s11_modify_bearer_request->bearer_contexts_to_be_removed.num_bearer_context = 0;

  s11_modify_bearer_request->mme_fq_csid.node_id_type = GLOBAL_UNICAST_IPv4; // TO DO
  s11_modify_bearer_request->mme_fq_csid.csid = 0;   // TO DO ...
  memset(&s11_modify_bearer_request->indication_flags, 0, sizeof(s11_modify_bearer_request->indication_flags));   // TO DO
  s11_modify_bearer_request->rat_type = RAT_EUTRAN;
  /*
   * S11 stack specific parameter. Not used in standalone epc mode
   */
  s11_modify_bearer_request->trxn = NULL;
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME,  MSC_S11_MME ,
                      NULL, 0, "0 S11_MODIFY_BEARER_REQUEST teid %u ebi %u", s11_modify_bearer_request->teid,
                      s11_modify_bearer_request->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id);
  itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);

  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_release_access_bearers_resp (
  const itti_s11_release_access_bearers_response_t * const rel_access_bearers_rsp_pP)
{
  struct ue_context_s                    *ue_context_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  ue_context_p = mme_ue_context_exists_s11_teid (&mme_app_desc.mme_ue_contexts, rel_access_bearers_rsp_pP->teid);

  if (ue_context_p == NULL) {
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 RELEASE_ACCESS_BEARERS_RESPONSE local S11 teid " TEID_FMT " ",
    		rel_access_bearers_rsp_pP->teid);
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this teid in list of UE: %" PRIX32 "\n", rel_access_bearers_rsp_pP->teid);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  MSC_LOG_RX_MESSAGE (MSC_MMEAPP_MME, MSC_S11_MME, NULL, 0, "0 RELEASE_ACCESS_BEARERS_RESPONSE local S11 teid " TEID_FMT " IMSI " IMSI_64_FMT " ",
    rel_access_bearers_rsp_pP->teid, ue_context_p->imsi);
  /*
   * Updating statistics
   */
  update_mme_app_stats_s1u_bearer_sub();

  // Send UE Context Release Command
  mme_app_itti_ue_context_release(ue_context_p, ue_context_p->ue_context_rel_cause);
  if (ue_context_p->ue_context_rel_cause == S1AP_SCTP_SHUTDOWN_OR_RESET) {
    // Just cleanup the MME APP state associated with s1.
    mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context_p, ECM_IDLE);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
void
mme_app_handle_mobile_reachability_timer_expiry (struct ue_context_s *ue_context_p) 
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_p != NULL);
  ue_context_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
  OAILOG_INFO (LOG_MME_APP, "Expired- Mobile Reachability Timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  // Start Implicit Detach timer 
  if (timer_setup (ue_context_p->implicit_detach_timer.sec, 0, 
                TASK_MME_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT, (void *)&(ue_context_p->mme_ue_s1ap_id), &(ue_context_p->implicit_detach_timer.id)) < 0) { 
    OAILOG_ERROR (LOG_MME_APP, "Failed to start Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  } else {
    OAILOG_DEBUG (LOG_MME_APP, "Started Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
void
mme_app_handle_implicit_detach_timer_expiry (struct ue_context_s *ue_context_p) 
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_p != NULL);
  MessageDef                             *message_p = NULL;
  OAILOG_INFO (LOG_MME_APP, "Expired- Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  
  // Initiate Implicit Detach for the UE
  message_p = itti_alloc_new_message (TASK_MME_APP, NAS_IMPLICIT_DETACH_UE_IND);
  DevAssert (message_p != NULL);
  message_p->ittiMsg.nas_implicit_detach_ue_ind.ue_id = ue_context_p->mme_ue_s1ap_id;
  MSC_LOG_TX_MESSAGE (MSC_MMEAPP_MME, MSC_NAS_MME, NULL, 0, "0 NAS_IMPLICIT_DETACH_UE_IND_MESSAGE");
  itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
void
mme_app_handle_initial_context_setup_rsp_timer_expiry (struct ue_context_s *ue_context_p)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (ue_context_p != NULL);
  MessageDef                             *message_p = NULL;
  OAILOG_INFO (LOG_MME_APP, "Expired- Initial context setup rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
  ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  /* *********Abort the ongoing procedure*********
   * Check if UE is registered already that implies service request procedure is active. If so then release the S1AP
   * context and move the UE back to idle mode. Otherwise if UE is not yet registered that implies attach procedure is
   * active. If so,then abort the attach procedure and release the UE context. 
   */
  ue_context_p->ue_context_rel_cause = S1AP_INITIAL_CONTEXT_SETUP_FAILED;
  if (ue_context_p->mm_state == UE_UNREGISTERED) {
    // Initiate Implicit Detach for the UE
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_IMPLICIT_DETACH_UE_IND);
    DevAssert (message_p != NULL);
    message_p->ittiMsg.nas_implicit_detach_ue_ind.ue_id = ue_context_p->mme_ue_s1ap_id;
    itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  } else {
    // Release S1-U bearer and move the UE to idle mode 
    mme_app_send_s11_release_access_bearers_req(ue_context_p);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
void
mme_app_handle_initial_context_setup_failure (
  const itti_mme_app_initial_context_setup_failure_t * const initial_ctxt_setup_failure_pP)
{
  struct ue_context_s                    *ue_context_p = NULL;
  MessageDef                             *message_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  OAILOG_DEBUG (LOG_MME_APP, "Received MME_APP_INITIAL_CONTEXT_SETUP_FAILURE from S1AP\n");
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, initial_ctxt_setup_failure_pP->mme_ue_s1ap_id);

  if (ue_context_p == NULL) {
    OAILOG_DEBUG (LOG_MME_APP, "We didn't find this mme_ue_s1ap_id in list of UE: %d \n", initial_ctxt_setup_failure_pP->mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  // Stop Initial context setup process guard timer,if running 
  if (ue_context_p->initial_context_setup_rsp_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->initial_context_setup_rsp_timer.id)) {
      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Initial Context Setup Rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  }
  /* *********Abort the ongoing procedure*********
   * Check if UE is registered already that implies service request procedure is active. If so then release the S1AP
   * context and move the UE back to idle mode. Otherwise if UE is not yet registered that implies attach procedure is
   * active. If so,then abort the attach procedure and release the UE context. 
   */
  ue_context_p->ue_context_rel_cause = S1AP_INITIAL_CONTEXT_SETUP_FAILED;
  if (ue_context_p->mm_state == UE_UNREGISTERED) {
    // Initiate Implicit Detach for the UE
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_IMPLICIT_DETACH_UE_IND);
    DevAssert (message_p != NULL);
    message_p->ittiMsg.nas_implicit_detach_ue_ind.ue_id = ue_context_p->mme_ue_s1ap_id;
    itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  } else {
    // Release S1-U bearer and move the UE to idle mode 
    mme_app_send_s11_release_access_bearers_req(ue_context_p);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
static bool mme_app_construct_guti(const plmn_t * const plmn_p, const as_stmsi_t * const s_tmsi_p,  guti_t * const guti_p)
{
  /*
   * This is a helper function to construct GUTI from S-TMSI. It uses PLMN id and MME Group Id of the serving MME for
   * this purpose. 
   *
   */
  
  bool                                    is_guti_valid = false; // Set to true if serving MME is found and GUTI is constructed 
  uint8_t                                 num_mme       = 0;     // Number of configured MME in the MME pool  
  guti_p->m_tmsi = s_tmsi_p->m_tmsi;
  guti_p->gummei.mme_code = s_tmsi_p->mme_code;
  // Create GUTI by using PLMN Id and MME-Group Id of serving MME
  OAILOG_DEBUG (LOG_MME_APP,
                "Construct GUTI using S-TMSI received form UE and MME Group Id and PLMN id from MME Conf: %u, %u \n",
                s_tmsi_p->m_tmsi, s_tmsi_p->mme_code);
  mme_config_read_lock (&mme_config);
  /*
   * Check number of MMEs in the pool.
   * At present it is assumed that one MME is supported in MME pool but in case there are more 
   * than one MME configured then search the serving MME using MME code. 
   * Assumption is that within one PLMN only one pool of MME will be configured
   */
  if (mme_config.gummei.nb > 1) 
  {
    OAILOG_DEBUG (LOG_MME_APP, "More than one MMEs are configured.");
  }
  for (num_mme = 0; num_mme < mme_config.gummei.nb; num_mme++)
  {
    /*Verify that the MME code within S-TMSI is same as what is configured in MME conf*/
    if ((plmn_p->mcc_digit2 == mme_config.gummei.gummei[num_mme].plmn.mcc_digit2) &&
        (plmn_p->mcc_digit1 == mme_config.gummei.gummei[num_mme].plmn.mcc_digit1) &&
        (plmn_p->mnc_digit3 == mme_config.gummei.gummei[num_mme].plmn.mnc_digit3) &&
        (plmn_p->mcc_digit3 == mme_config.gummei.gummei[num_mme].plmn.mcc_digit3) &&
        (plmn_p->mnc_digit2 == mme_config.gummei.gummei[num_mme].plmn.mnc_digit2) && 
        (plmn_p->mnc_digit1 == mme_config.gummei.gummei[num_mme].plmn.mnc_digit1) &&
        (guti_p->gummei.mme_code == mme_config.gummei.gummei[num_mme].mme_code))
    {
      break;
    }
  }          
  if (num_mme >= mme_config.gummei.nb)
  {
    OAILOG_DEBUG (LOG_MME_APP, "No MME serves this UE");
  }
  else 
  {
    guti_p->gummei.plmn = mme_config.gummei.gummei[num_mme].plmn;
    guti_p->gummei.mme_gid = mme_config.gummei.gummei[num_mme].mme_gid;
    is_guti_valid = true;
  }
  mme_config_unlock (&mme_config);
  return is_guti_valid;
}

//------------------------------------------------------------------------------
static void notify_s1ap_new_ue_mme_s1ap_id_association (struct ue_context_s *ue_context_p)
{
  MessageDef                             *message_p = NULL;
  itti_mme_app_s1ap_mme_ue_id_notification_t *notification_p = NULL;
  
  OAILOG_FUNC_IN (LOG_MME_APP);
  if (ue_context_p == NULL) {
    OAILOG_ERROR (LOG_MME_APP, " NULL UE context ptr\n" );
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  message_p = itti_alloc_new_message (TASK_MME_APP, MME_APP_S1AP_MME_UE_ID_NOTIFICATION);
  notification_p = &message_p->ittiMsg.mme_app_s1ap_mme_ue_id_notification;
  memset (notification_p, 0, sizeof (itti_mme_app_s1ap_mme_ue_id_notification_t)); 
  notification_p->enb_ue_s1ap_id = ue_context_p->enb_ue_s1ap_id; 
  notification_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
  notification_p->sctp_assoc_id  = ue_context_p->sctp_assoc_id_key;

  itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);
  OAILOG_DEBUG (LOG_MME_APP, " Sent MME_APP_S1AP_MME_UE_ID_NOTIFICATION to S1AP for UE Id %u\n", notification_p->mme_ue_s1ap_id);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
