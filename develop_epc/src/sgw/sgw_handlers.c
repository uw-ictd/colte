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

/*! \file sgw_handlers.c
  \brief
  \author Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#define SGW
#define S11_HANDLERS_C

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>

#include "bstrlib.h"

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "conversions.h"
#include "hashtable.h"
#include "obj_hashtable.h"
#include "common_defs.h"
#include "intertask_interface.h"
#include "msc.h"
#include "log.h"
#include "sgw_ie_defs.h"
#include "3gpp_23.401.h"
#include "common_types.h"
#include "mme_config.h"
#include "sgw_defs.h"
#include "sgw_handlers.h"
#include "sgw_context_manager.h"
#include "sgw.h"
#include "pgw_lite_paa.h"
#include "pgw_pco.h"
#include "spgw_config.h"
#include "gtpv1u.h"
#include "pgw_ue_ip_address_alloc.h"
#include "pgw_pcef_emulation.h"
#include "sgw_context_manager.h"
#include "pgw_procedures.h"
#include "async_system.h"

extern sgw_app_t                        sgw_app;
extern spgw_config_t                    spgw_config;
extern struct gtp_tunnel_ops           *gtp_tunnel_ops;
static uint32_t                         g_gtpv1u_teid = 0;

//------------------------------------------------------------------------------
uint32_t sgw_get_new_s1u_teid (void)
{
  __sync_fetch_and_add(&g_gtpv1u_teid, 1);
  return g_gtpv1u_teid;
}


//------------------------------------------------------------------------------
int
sgw_handle_create_session_request (
  const itti_s11_create_session_request_t * const session_req_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  mme_sgw_tunnel_t                       *new_endpoint_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *s_plus_p_gw_eps_bearer_ctxt_info_p = NULL;
  sgw_eps_bearer_ctxt_t                 *eps_bearer_ctxt_p = NULL;

  /*
   * Upon reception of create session request from MME,
   * S-GW should create UE, eNB and MME contexts and forward message to P-GW.
   */
  if (session_req_pP->rat_type != RAT_EUTRAN) {
    OAILOG_WARNING (LOG_SPGW_APP, "Received session request with RAT != RAT_TYPE_EUTRAN: type %d\n", session_req_pP->rat_type);
  }

  /*
   * As we are abstracting GTP-C transport, FTeid ip address is useless.
   *  We just use the teid to identify MME tunnel. Normally we received either:
   *  - ipv4 address if ipv4 flag is set
   *  - ipv6 address if ipv6 flag is set
   *  - ipv4 and ipv6 if both flags are set
   *  Communication between MME and S-GW involves S11 interface so we are expecting
   *  S11_MME_GTP_C (11) as interface_type.
   */
  if ((session_req_pP->sender_fteid_for_cp.teid == 0) && (session_req_pP->sender_fteid_for_cp.interface_type != S11_MME_GTP_C)) {
    /*
     * MME sent request with teid = 0. This is not valid...
     */
    OAILOG_WARNING (LOG_SPGW_APP, "F-TEID parameter mismatch\n");
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
  }

  new_endpoint_p = sgw_cm_create_s11_tunnel (session_req_pP->sender_fteid_for_cp.teid, sgw_get_new_S11_tunnel_id ());

  if (new_endpoint_p == NULL) {
    OAILOG_WARNING (LOG_SPGW_APP, "Could not create new tunnel endpoint between S-GW and MME " "for S11 abstraction\n");
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
  }

  OAILOG_DEBUG (LOG_SPGW_APP, "Rx CREATE-SESSION-REQUEST MME S11 teid "TEID_FMT" S-GW S11 teid "TEID_FMT" APN %s EPS bearer Id %d\n",
      new_endpoint_p->remote_teid, new_endpoint_p->local_teid, session_req_pP->apn,
      session_req_pP->bearer_contexts_to_be_created.bearer_contexts[0].eps_bearer_id);

  s_plus_p_gw_eps_bearer_ctxt_info_p = sgw_cm_create_bearer_context_information_in_collection (new_endpoint_p->local_teid);
  if (s_plus_p_gw_eps_bearer_ctxt_info_p ) {
    /*
     * We try to create endpoint for S11 interface. A NULL endpoint means that
     * either the teid is already in list of known teid or ENOMEM error has been
     * raised during malloc.
     */
    //--------------------------------------------------
    // copy informations from create session request to bearer context information
    //--------------------------------------------------
    memcpy (&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.imsi, &session_req_pP->imsi, sizeof(imsi_t));
    memcpy (&s_plus_p_gw_eps_bearer_ctxt_info_p->pgw_eps_bearer_context_information.imsi, &session_req_pP->imsi, sizeof(imsi_t));
    s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.imsi_unauthenticated_indicator = 1;
    s_plus_p_gw_eps_bearer_ctxt_info_p->pgw_eps_bearer_context_information.imsi_unauthenticated_indicator = 1;
    s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_teid_S11 = session_req_pP->sender_fteid_for_cp.teid;
    s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.s_gw_teid_S11_S4 = new_endpoint_p->local_teid;
    s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn = session_req_pP->trxn;
    //s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_int_ip_address_S11 = session_req_pP->peer_ip;
    FTEID_T_2_IP_ADDRESS_T ((&session_req_pP->sender_fteid_for_cp), (&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_ip_address_S11));
    //--------------------------------------
    // PDN connection
    //--------------------------------------
    /*
     * pdn_connection = sgw_cm_create_pdn_connection();
     *
     * if (pdn_connection == NULL) {
     * // Malloc failed, may be ENOMEM error
     * SPGW_APP_ERROR("Failed to create new PDN connection\n");
     * OAILOG_FUNC_RETURN(LOG_SPGW_APP,  RETURNerror);
     * }
     */
    memset (&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection, 0, sizeof (sgw_pdn_connection_t));

    if (s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers_array == NULL) {
      OAILOG_ERROR (LOG_SPGW_APP, "Failed to create eps bearers collection object\n");
      DevMessage ("Failed to create eps bearers collection object\n");
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
    }

    if (session_req_pP->apn) {
      s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.apn_in_use = strdup (session_req_pP->apn);
    } else {
      s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.apn_in_use = "NO APN";
    }

    s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.default_bearer = session_req_pP->bearer_contexts_to_be_created.bearer_contexts[0].eps_bearer_id;
    //obj_hashtable_ts_insert(s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connections, pdn_connection->apn_in_use, strlen(pdn_connection->apn_in_use), pdn_connection);

    //--------------------------------------
    // EPS bearer entry
    //--------------------------------------
    // TODO several bearers
    eps_bearer_ctxt_p = sgw_cm_create_eps_bearer_ctxt_in_collection (&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
        session_req_pP->bearer_contexts_to_be_created.bearer_contexts[0].eps_bearer_id);
    sgw_display_s11teid2mme_mappings ();
    sgw_display_s11_bearer_context_information_mapping ();

    if (eps_bearer_ctxt_p == NULL) {
      OAILOG_ERROR (LOG_SPGW_APP, "Failed to create new EPS bearer entry\n");
      // TO DO free_wrapper new_bearer_ctxt_info_p and by cascade...
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
    }

    eps_bearer_ctxt_p->eps_bearer_qos = session_req_pP->bearer_contexts_to_be_created.bearer_contexts[0].bearer_level_qos;
    //s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_informationteid = teid;

    /*
     * Trying to insert the new tunnel into the tree.
     * If collision_p is not NULL (0), it means tunnel is already present.
     */
    //s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_informations_gw_ip_address_S11_S4 =
    memcpy (&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.saved_message, session_req_pP, sizeof (itti_s11_create_session_request_t));

    /*
     * Establishing EPS bearer. Requesting S1-U (GTPV1-U) task to create a
     * tunnel for S1 user plane interface. If status in response is successfull
     * (0), the tunnel endpoint is locally ready.
     *
     * The right way to implement this is to send GTPV1U_CREATE_TUNNEL_REQ to the gtp1u task
     * and handle the GTPV1U_CREATE_TUNNEL_RESP message in sgw_task. Current implementation
     * calls the sgw_handle_gtpv1uCreateTunnelResp() directly.
     */

    /*
    message_p = itti_alloc_new_message (TASK_SPGW_APP, GTPV1U_CREATE_TUNNEL_REQ);

    if (message_p == NULL) {
      sgw_cm_remove_s11_tunnel (new_endpoint_p->remote_teid);
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
    }
    */

    {
      Gtpv1uCreateTunnelResp                  createTunnelResp = {0};

      createTunnelResp.context_teid = new_endpoint_p->local_teid;
      createTunnelResp.eps_bearer_id = session_req_pP->bearer_contexts_to_be_created.bearer_contexts[0].eps_bearer_id;
      createTunnelResp.status = 0x00;
      createTunnelResp.S1u_teid = sgw_get_new_s1u_teid ();
      sgw_handle_gtpv1uCreateTunnelResp (&createTunnelResp);
    }
  } else {
    OAILOG_WARNING (LOG_SPGW_APP, "Could not create new transaction for SESSION_CREATE message\n");
    free_wrapper ((void**) &new_endpoint_p);
    new_endpoint_p = NULL;
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
  }
  OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNok);
}



//------------------------------------------------------------------------------
int
sgw_handle_sgi_endpoint_created (
  itti_sgi_create_end_point_response_t * const resp_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  itti_s11_create_session_response_t     *create_session_response_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *new_bearer_ctxt_info_p = NULL;
  MessageDef                             *message_p = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  int                                     rv = RETURNok;

  OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_CREATE_ENDPOINT_RESPONSE,Context: S11 teid "TEID_FMT", SGW S1U teid "TEID_FMT" EPS bearer id %u\n", resp_pP->context_teid, resp_pP->sgw_S1u_teid, resp_pP->eps_bearer_id);
  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, resp_pP->context_teid, (void **)&new_bearer_ctxt_info_p);

  message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_CREATE_SESSION_RESPONSE);

  if (message_p == NULL) {
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
  }

  create_session_response_p = &message_p->ittiMsg.s11_create_session_response;

  if (HASH_TABLE_OK == hash_rc) {
    create_session_response_p->teid = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_teid_S11;

    /*
     * Preparing to send create session response on S11 abstraction interface.
     * * * *  we set the cause value regarding the S1-U bearer establishment result status.
     */
    if (resp_pP->status == SGI_STATUS_OK) {
      create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.teid = resp_pP->sgw_S1u_teid;
      create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.interface_type = S1_U_SGW_GTP_U;
      create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv4 = 1;
      create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv4_address.s_addr = sgw_app.sgw_ip_address_S1u_S12_S4_up.s_addr;
      create_session_response_p->ambr.br_dl = 100000000;
      create_session_response_p->ambr.br_ul = 40000000;

      sgw_eps_bearer_ctxt_t *eps_bearer_ctxt_p =
          sgw_cm_get_eps_bearer_entry(&new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
              resp_pP->eps_bearer_id);
      AssertFatal (eps_bearer_ctxt_p, "ERROR UNABLE TO GET EPS BEARER ENTRY\n");
      AssertFatal (sizeof (eps_bearer_ctxt_p->paa) == sizeof (resp_pP->paa), "Mismatch in lengths");       // sceptic mode
      memcpy (&eps_bearer_ctxt_p->paa, &resp_pP->paa, sizeof (paa_t));
      memcpy (&create_session_response_p->paa, &resp_pP->paa, sizeof (paa_t));
      copy_protocol_configuration_options (&create_session_response_p->pco, &resp_pP->pco);
      clear_protocol_configuration_options(&resp_pP->pco);
      /*
       * Set the Cause information from bearer context created.
       * "Request accepted" is returned when the GTPv2 entity has accepted a control plane request.
       */
      create_session_response_p->cause.cause_value = REQUEST_ACCEPTED;
      create_session_response_p->bearer_contexts_created.bearer_contexts[0].cause.cause_value = REQUEST_ACCEPTED;
    } else {
      create_session_response_p->cause.cause_value = M_PDN_APN_NOT_ALLOWED;
      create_session_response_p->bearer_contexts_created.bearer_contexts[0].cause.cause_value = M_PDN_APN_NOT_ALLOWED;
    }

    create_session_response_p->s11_sgw_fteid.teid = resp_pP->context_teid;
    create_session_response_p->s11_sgw_fteid.interface_type = S11_SGW_GTP_C;
    create_session_response_p->s11_sgw_fteid.ipv4 = 1;
    create_session_response_p->s11_sgw_fteid.ipv4_address.s_addr = spgw_config.sgw_config.ipv4.S11.s_addr;

    create_session_response_p->bearer_contexts_created.bearer_contexts[0].eps_bearer_id = resp_pP->eps_bearer_id;
    create_session_response_p->bearer_contexts_created.num_bearer_context += 1;

    create_session_response_p->trxn = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
    create_session_response_p->peer_ip.s_addr = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_ip_address_S11.address.ipv4_address.s_addr;
  } else {
    create_session_response_p->cause.cause_value = CONTEXT_NOT_FOUND;
    create_session_response_p->bearer_contexts_created.bearer_contexts[0].cause.cause_value = CONTEXT_NOT_FOUND;
    create_session_response_p->bearer_contexts_created.num_bearer_context += 1;
  }

  OAILOG_DEBUG (LOG_SPGW_APP,
      "Tx CREATE-SESSION-RESPONSE SPGW -> TASK_S11, S11 MME teid "TEID_FMT" S11 S-GW teid "TEID_FMT" S1U teid "TEID_FMT" S1U addr 0x%x EPS bearer id %u status %d\n",
                  create_session_response_p->teid,
                  create_session_response_p->s11_sgw_fteid.teid,
                  create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.teid,
                  create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv4_address.s_addr,
                  create_session_response_p->bearer_contexts_created.bearer_contexts[0].eps_bearer_id,
                  create_session_response_p->bearer_contexts_created.bearer_contexts[0].cause.cause_value);
  MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME,
                      NULL, 0,
                      "0 S11_CREATE_SESSION_RESPONSE S11 MME teid "TEID_FMT" S11 S-GW teid "TEID_FMT" S1U teid "TEID_FMT" S1U@ 0x%x ebi %u status %d",
                      create_session_response_p->teid,
                      create_session_response_p->s11_sgw_fteid.teid,
                      create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.teid,
                      create_session_response_p->bearer_contexts_created.bearer_contexts[0].s1u_sgw_fteid.ipv4_address.s_addr,
                      create_session_response_p->bearer_contexts_created.bearer_contexts[0].eps_bearer_id,
                      create_session_response_p->bearer_contexts_created.bearer_contexts[0].cause.cause_value);
  rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
}



//------------------------------------------------------------------------------
int
sgw_handle_gtpv1uCreateTunnelResp (
  const Gtpv1uCreateTunnelResp * const endpoint_created_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  itti_s11_create_session_response_t     *create_session_response_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *new_bearer_ctxt_info_p = NULL;
  MessageDef                             *message_p = NULL;
  sgw_eps_bearer_ctxt_t                  *eps_bearer_ctxt_p = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  struct in_addr                          inaddr;
  itti_sgi_create_end_point_response_t    sgi_create_endpoint_resp = {0};
  int                                     rv = RETURNok;
  char                                   *imsi = NULL;
  gtpv2c_cause_value_t                    cause = REQUEST_ACCEPTED;

  OAILOG_DEBUG (LOG_SPGW_APP, "Rx GTPV1U_CREATE_TUNNEL_RESP, Context S-GW S11 teid "TEID_FMT", S-GW S1U teid "TEID_FMT" EPS bearer id %u status %d\n",
                  endpoint_created_pP->context_teid, endpoint_created_pP->S1u_teid, endpoint_created_pP->eps_bearer_id, endpoint_created_pP->status);
  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, endpoint_created_pP->context_teid, (void **)&new_bearer_ctxt_info_p);

  if (HASH_TABLE_OK == hash_rc) {
    eps_bearer_ctxt_p =
        sgw_cm_get_eps_bearer_entry(&new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
            endpoint_created_pP->eps_bearer_id);
    DevAssert (eps_bearer_ctxt_p);
    OAILOG_DEBUG (LOG_SPGW_APP, "Updated eps_bearer_ctxt_p eps_b_id %u with SGW S1U teid "TEID_FMT"\n", endpoint_created_pP->eps_bearer_id, endpoint_created_pP->S1u_teid);
    eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up = endpoint_created_pP->S1u_teid;
    sgw_display_s11_bearer_context_information_mapping ();
    memset (&sgi_create_endpoint_resp, 0, sizeof (itti_sgi_create_end_point_response_t));

    //--------------------------------------------------------------------------
    // PCO processing
    //--------------------------------------------------------------------------
    protocol_configuration_options_t *pco_req = &new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.saved_message.pco;
    protocol_configuration_options_t pco_resp = {0};
    protocol_configuration_options_ids_t pco_ids;
    memset(&pco_ids, 0, sizeof pco_ids);

    // TODO: perhaps change to a nonfatal assert?
    AssertFatal (0 == pgw_process_pco_request(pco_req, &pco_resp, &pco_ids),
                 "Error in processing PCO in request");
    copy_protocol_configuration_options (&sgi_create_endpoint_resp.pco, &pco_resp);
    clear_protocol_configuration_options(&pco_resp);

    //--------------------------------------------------------------------------
    // IP forward will forward packets to this teid
    sgi_create_endpoint_resp.context_teid = endpoint_created_pP->context_teid;
    sgi_create_endpoint_resp.sgw_S1u_teid = endpoint_created_pP->S1u_teid;
    sgi_create_endpoint_resp.eps_bearer_id = endpoint_created_pP->eps_bearer_id;
    // TO DO NOW
    sgi_create_endpoint_resp.paa.pdn_type = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.saved_message.pdn_type;

    imsi = (char *) new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.imsi.u.value;
    switch (sgi_create_endpoint_resp.paa.pdn_type) {
    case IPv4:
      // Use NAS by default if no preference is set.
      //
      // For context, the protocol configuration options (PCO) section of the
      // packet from the UE is optional, which means that it is perfectly valid
      // for a UE to send no PCO preferences at all. The previous logic only
      // allocates an IPv4 address if the UE has explicitly set the PCO
      // parameter for allocating IPv4 via NAS signaling (as opposed to via
      // DHCPv4). This means that, in the absence of either parameter being set,
      // the does not know what to do, so we need a default option as well.
      //
      // Since we only support the NAS signaling option right now, we will
      // default to using NAS signaling UNLESS we see a preference for DHCPv4.
      // This means that all IPv4 addresses are now allocated via NAS signaling
      // unless specified otherwise.
      //
      // In the long run, we will want to evolve the logic to use whatever
      // information we have to choose the ``best" allocation method. This means
      // adding new bitfields to pco_ids in pgw_pco.h, setting them in pgw_pco.c
      // and using them here in conditional logic. We will also want to
      // implement different logic between the PDN types.
      if (!pco_ids.ci_ipv4_address_allocation_via_dhcpv4) {
        if (0 == allocate_ue_ipv4_address(imsi, &inaddr)) {
          sgi_create_endpoint_resp.paa.ipv4_address.s_addr = inaddr.s_addr;
          sgi_create_endpoint_resp.status = SGI_STATUS_OK; 
        } else {
          OAILOG_ERROR (LOG_SPGW_APP, "Failed to allocate IPv4 PAA for PDN type IPv4\n");
          //Check the error code returned by "IP address allocator" and handle it accordingly

          sgi_create_endpoint_resp.status = SGI_STATUS_ERROR_ALL_DYNAMIC_ADDRESSES_OCCUPIED;
        }
      }

      break;

    case IPv6:
          OAILOG_ERROR (LOG_SPGW_APP, "IPV6 PDN type NOT Supported\n");
          sgi_create_endpoint_resp.status = SGI_STATUS_ERROR_SERVICE_NOT_SUPPORTED;

      break;

    case IPv4_AND_v6:
      if (0 == allocate_ue_ipv4_address(imsi, &inaddr)) {
        sgi_create_endpoint_resp.paa.ipv4_address.s_addr = inaddr.s_addr;
        sgi_create_endpoint_resp.status = SGI_STATUS_OK;
      } else {
        OAILOG_ERROR (LOG_SPGW_APP, "Failed to allocate IPv4 PAA for PDN type IPv4_AND_v6\n");
        sgi_create_endpoint_resp.status = SGI_STATUS_ERROR_ALL_DYNAMIC_ADDRESSES_OCCUPIED;
      }
      break;

    default:
      AssertFatal (0, "BAD paa.pdn_type %d", sgi_create_endpoint_resp.paa.pdn_type);
      break;
    }

  } else {                      // if (HASH_TABLE_OK == hash_rc)
    OAILOG_DEBUG (LOG_SPGW_APP, "Rx S11_S1U_ENDPOINT_CREATED, Context: teid %u NOT FOUND\n", endpoint_created_pP->context_teid);
    sgi_create_endpoint_resp.status = SGI_STATUS_ERROR_CONTEXT_NOT_FOUND;
  }
  switch (sgi_create_endpoint_resp.status) {
  case SGI_STATUS_OK:
    // Send Create Session Response with ack
    sgw_handle_sgi_endpoint_created (&sgi_create_endpoint_resp);
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNok);

    break;

  case SGI_STATUS_ERROR_CONTEXT_NOT_FOUND:
    cause = CONTEXT_NOT_FOUND;

    break;

  case SGI_STATUS_ERROR_ALL_DYNAMIC_ADDRESSES_OCCUPIED:
    cause = ALL_DYNAMIC_ADDRESSES_ARE_OCCUPIED;

    break;

  case SGI_STATUS_ERROR_SERVICE_NOT_SUPPORTED:
    cause = SERVICE_NOT_SUPPORTED;

    break;

    default:
    cause = REQUEST_REJECTED; // Unspecified reason

   break;
  }
  // Send Create Session Response with Nack
  message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_CREATE_SESSION_RESPONSE);
  if (!message_p) {
    OAILOG_ERROR (LOG_SPGW_APP, "Message Create Session Response alloction failed\n");
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
  }
  create_session_response_p = &message_p->ittiMsg.s11_create_session_response;
  create_session_response_p->cause.cause_value = cause;
  create_session_response_p->bearer_contexts_created.bearer_contexts[0].cause.cause_value = cause;
  create_session_response_p->bearer_contexts_created.num_bearer_context += 1;
  if (new_bearer_ctxt_info_p) {
    create_session_response_p->teid = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_teid_S11;
    create_session_response_p->trxn = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
  }
  rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
  OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
}
//------------------------------------------------------------------------------
int
sgw_handle_gtpv1uUpdateTunnelResp (
  const Gtpv1uUpdateTunnelResp * const endpoint_updated_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  itti_s11_modify_bearer_response_t      *modify_response_p = NULL;
  itti_sgi_update_end_point_request_t    *update_request_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *new_bearer_ctxt_info_p = NULL;
  MessageDef                             *message_p = NULL;
  sgw_eps_bearer_ctxt_t                  *eps_bearer_ctxt_p = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  int                                     rv = RETURNok;

  OAILOG_DEBUG (LOG_SPGW_APP, "Rx GTPV1U_UPDATE_TUNNEL_RESP, Context teid "TEID_FMT", Tunnel " TEID_FMT " (eNB) <-> (SGW) " TEID_FMT ", EPS bearer id %u, status %d\n",
                  endpoint_updated_pP->context_teid, endpoint_updated_pP->enb_S1u_teid, endpoint_updated_pP->sgw_S1u_teid, endpoint_updated_pP->eps_bearer_id, endpoint_updated_pP->status);
  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, endpoint_updated_pP->context_teid, (void **)&new_bearer_ctxt_info_p);

  if (HASH_TABLE_OK == hash_rc) {
    eps_bearer_ctxt_p =
        sgw_cm_get_eps_bearer_entry(&new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
            endpoint_updated_pP->eps_bearer_id);

    if (NULL == eps_bearer_ctxt_p) {
      OAILOG_DEBUG (LOG_SPGW_APP, "Sending S11_MODIFY_BEARER_RESPONSE trxn %p bearer %u CONTEXT_NOT_FOUND (sgw_eps_bearers)\n", new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn, endpoint_updated_pP->eps_bearer_id);
      message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_MODIFY_BEARER_RESPONSE);

      if (!message_p) {
        OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
      }

      modify_response_p = &message_p->ittiMsg.s11_modify_bearer_response;
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id = endpoint_updated_pP->eps_bearer_id;
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->bearer_contexts_marked_for_removal.num_bearer_context += 1;
      modify_response_p->cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->trxn = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
      rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
    } else {
      message_p = itti_alloc_new_message (TASK_SPGW_APP, SGI_UPDATE_ENDPOINT_REQUEST);

      if (!message_p) {
        OAILOG_FUNC_RETURN(LOG_SPGW_APP,  -1);
      }

      update_request_p = &message_p->ittiMsg.sgi_update_end_point_request;

      update_request_p->context_teid = endpoint_updated_pP->context_teid;
      update_request_p->sgw_S1u_teid = endpoint_updated_pP->sgw_S1u_teid;
      update_request_p->enb_S1u_teid = endpoint_updated_pP->enb_S1u_teid;
      update_request_p->eps_bearer_id = endpoint_updated_pP->eps_bearer_id;
      // There is no such a task TASK_FW_IP
      rv = itti_send_msg_to_task (TASK_FW_IP, INSTANCE_DEFAULT, message_p);
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
    }
  } else {
    OAILOG_DEBUG (LOG_SPGW_APP, "Sending S11_MODIFY_BEARER_RESPONSE trxn %p bearer %u CONTEXT_NOT_FOUND (s11_bearer_context_information_hashtable)\n",
        new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn, endpoint_updated_pP->eps_bearer_id);
    message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_MODIFY_BEARER_RESPONSE);

    if (!message_p) {
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
    }

    modify_response_p = &message_p->ittiMsg.s11_modify_bearer_response;
    modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id = endpoint_updated_pP->eps_bearer_id;
    modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].cause.cause_value = CONTEXT_NOT_FOUND;
    modify_response_p->bearer_contexts_marked_for_removal.num_bearer_context += 1;
    modify_response_p->cause.cause_value = CONTEXT_NOT_FOUND;
    modify_response_p->trxn = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
    MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME,
                        NULL, 0, "0 S11_MODIFY_BEARER_RESPONSE ebi %u CONTEXT_NOT_FOUND trxn %u",
                        modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id,
                        modify_response_p->trxn);
    rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
  }

  OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
}

//------------------------------------------------------------------------------
int
sgw_handle_sgi_endpoint_updated (
  const itti_sgi_update_end_point_response_t * const resp_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  itti_s11_modify_bearer_response_t      *modify_response_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *new_bearer_ctxt_info_p = NULL;
  MessageDef                             *message_p = NULL;
  sgw_eps_bearer_ctxt_t                 *eps_bearer_ctxt_p = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  hashtable_rc_t                          hash_rc2 = HASH_TABLE_OK;
  int                                     rv = RETURNok;
  mme_sgw_tunnel_t                       *tun_pair_p = NULL;


  OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_UPDATE_ENDPOINT_RESPONSE, Context teid " TEID_FMT " Tunnel " TEID_FMT " (eNB) <-> (SGW) " TEID_FMT " EPS bearer id %u, status %d\n",
                  resp_pP->context_teid, resp_pP->enb_S1u_teid, resp_pP->sgw_S1u_teid, resp_pP->eps_bearer_id, resp_pP->status);
  message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_MODIFY_BEARER_RESPONSE);

  if (!message_p) {
    OAILOG_FUNC_RETURN(LOG_SPGW_APP,  RETURNerror);
  }

  modify_response_p = &message_p->ittiMsg.s11_modify_bearer_response;
  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, resp_pP->context_teid, (void **)&new_bearer_ctxt_info_p);
  hash_rc2 = hashtable_ts_get (sgw_app.s11teid2mme_hashtable, resp_pP->context_teid /*local teid*/, (void **)&tun_pair_p);

  if ((HASH_TABLE_OK == hash_rc) && (HASH_TABLE_OK == hash_rc2)) {
    eps_bearer_ctxt_p =
        sgw_cm_get_eps_bearer_entry(&new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
            resp_pP->eps_bearer_id);

    if (NULL == eps_bearer_ctxt_p) {
      OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_UPDATE_ENDPOINT_RESPONSE: CONTEXT_NOT_FOUND (pdn_connection. context)\n");

      modify_response_p->teid = tun_pair_p->remote_teid;
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id = resp_pP->eps_bearer_id;
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->bearer_contexts_marked_for_removal.num_bearer_context += 1;
      modify_response_p->cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->trxn = 0;
      MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME,
                          NULL, 0, "0 S11_MODIFY_BEARER_RESPONSE ebi %u CONTEXT_NOT_FOUND trxn %u",
                          modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id,
                          modify_response_p->trxn);
      rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
    } else if (HASH_TABLE_OK == hash_rc) {
      OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_UPDATE_ENDPOINT_RESPONSE: REQUEST_ACCEPTED\n");
      // accept anyway
      modify_response_p->teid = tun_pair_p->remote_teid;
      modify_response_p->bearer_contexts_modified.bearer_contexts[0].eps_bearer_id = resp_pP->eps_bearer_id;
      modify_response_p->bearer_contexts_modified.bearer_contexts[0].cause.cause_value = REQUEST_ACCEPTED;
      modify_response_p->bearer_contexts_modified.num_bearer_context += 1;
      modify_response_p->cause.cause_value = REQUEST_ACCEPTED;
      modify_response_p->trxn = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
      // if default bearer
      //#pragma message  "TODO define constant for default eps_bearer id"

      // setup GTPv1-U tunnel
      struct in_addr enb = {.s_addr = 0};
      enb.s_addr = eps_bearer_ctxt_p->enb_ip_address_S1u.address.ipv4_address.s_addr;

      struct in_addr ue = {.s_addr = 0};
      ue.s_addr = eps_bearer_ctxt_p->paa.ipv4_address.s_addr;

      if (spgw_config.pgw_config.use_gtp_kernel_module) {
        rv = gtp_tunnel_ops->add_tunnel(ue, enb, eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up, eps_bearer_ctxt_p->enb_teid_S1u);
        if (rv < 0) {
          OAILOG_ERROR (LOG_SPGW_APP, "ERROR in setting up TUNNEL err=%d\n", rv);
        }
      }

      // may be removed
      if (TRAFFIC_FLOW_TEMPLATE_NB_PACKET_FILTERS_MAX > eps_bearer_ctxt_p->num_sdf) {
        int i = 0;
        while ((i < eps_bearer_ctxt_p->num_sdf) && (SDF_ID_NGBR_DEFAULT != eps_bearer_ctxt_p->sdf_id[i])) i++;
        if (i >= eps_bearer_ctxt_p->num_sdf) {
          eps_bearer_ctxt_p->sdf_id[eps_bearer_ctxt_p->num_sdf] = SDF_ID_NGBR_DEFAULT;
          eps_bearer_ctxt_p->num_sdf += 1;
        }
      }
    }

    MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME, NULL, 0, "0 S11_MODIFY_BEARER_RESPONSE ebi %u  trxn %u",
        modify_response_p->bearer_contexts_modified.bearer_contexts[0].eps_bearer_id, modify_response_p->trxn);
    rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
  } else {
    if (HASH_TABLE_OK != hash_rc2) {
      OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_UPDATE_ENDPOINT_RESPONSE: CONTEXT_NOT_FOUND (S11 context)\n");
      modify_response_p->teid = resp_pP->context_teid;    // TO BE CHECKED IF IT IS THIS TEID
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id = resp_pP->eps_bearer_id;
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->bearer_contexts_marked_for_removal.num_bearer_context += 1;
      modify_response_p->cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->trxn = 0;
      MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME,  MSC_S11_MME,
                        NULL, 0, "0 S11_MODIFY_BEARER_RESPONSE ebi %u CONTEXT_NOT_FOUND trxn %u",
                        modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id, modify_response_p->trxn);
      rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
    } else {
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
    }
  }
}

//------------------------------------------------------------------------------
int
sgw_handle_sgi_endpoint_deleted (
  const itti_sgi_delete_end_point_request_t * const resp_pP)
  {
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  s_plus_p_gw_eps_bearer_context_information_t *new_bearer_ctxt_info_p = NULL;
  sgw_eps_bearer_ctxt_t                 *eps_bearer_ctxt_p = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  int                                     rv = RETURNok;
  char                                   *imsi = NULL;
  struct in_addr                          inaddr;

  imsi = (char *) new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.imsi.u.value;
  switch (resp_pP->paa.pdn_type) {
    case IPv4:
      inaddr.s_addr = resp_pP->paa.ipv4_address.s_addr;
      if (!release_ue_ipv4_address(imsi, &inaddr)) {
        OAILOG_DEBUG (LOG_SPGW_APP, "Released IPv4 PAA for PDN type IPv4\n");
      } else {
        OAILOG_ERROR (LOG_SPGW_APP, "Failed to release IPv4 PAA for PDN type IPv4\n");
      }
      break;

    case IPv6:
      OAILOG_ERROR (LOG_SPGW_APP, "Failed to release IPv6 PAA for PDN type IPv6\n");
      break;

    case IPv4_AND_v6:
      inaddr.s_addr = resp_pP->paa.ipv4_address.s_addr;
      if (!release_ue_ipv4_address(imsi, &inaddr)) {
        OAILOG_DEBUG (LOG_SPGW_APP, "Released IPv4 PAA for PDN type IPv4_AND_v6\n");
      } else {
        OAILOG_ERROR (LOG_SPGW_APP, "Failed to release IPv4 PAA for PDN type IPv4_AND_v6\n");
      }
      break;

    default:
      AssertFatal (0, "Bad paa.pdn_type %d", resp_pP->paa.pdn_type);
      break;
  }

  OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_DELETE_ENDPOINT_REQUEST, Context teid %u, SGW S1U teid %u, EPS bearer id %u\n",
                resp_pP->context_teid, resp_pP->sgw_S1u_teid, resp_pP->eps_bearer_id);

  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, resp_pP->context_teid, (void **)&new_bearer_ctxt_info_p);

  if (HASH_TABLE_OK == hash_rc) {
    eps_bearer_ctxt_p =
        sgw_cm_get_eps_bearer_entry(&new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
            resp_pP->eps_bearer_id);

    if (NULL == eps_bearer_ctxt_p) {
      OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_DELETE_ENDPOINT_REQUEST: CONTEXT_NOT_FOUND (pdn_connection.sgw_eps_bearers context)\n");
    } else {
      OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_DELETE_ENDPOINT_REQUEST: REQUEST_ACCEPTED\n");
      // if default bearer
      //#pragma message  "TODO define constant for default eps_bearer id"
      rv = gtp_tunnel_ops->del_tunnel(eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up, eps_bearer_ctxt_p->enb_teid_S1u);
      if (rv < 0) {
        OAILOG_ERROR (LOG_SPGW_APP, "ERROR in deleting TUNNEL\n");
      }
    }
  } else {
    OAILOG_DEBUG (LOG_SPGW_APP, "Rx SGI_DELETE_ENDPOINT_RESPONSE: CONTEXT_NOT_FOUND (S11 context)\n");
  }
  OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
}


//------------------------------------------------------------------------------
int
sgw_handle_modify_bearer_request (
  const itti_s11_modify_bearer_request_t * const modify_bearer_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  itti_s11_modify_bearer_response_t            *modify_response_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *new_bearer_ctxt_info_p = NULL;
  MessageDef                             *message_p = NULL;
  sgw_eps_bearer_ctxt_t                 *eps_bearer_ctxt_p = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  int                                     rv = RETURNok;


  OAILOG_DEBUG (LOG_SPGW_APP, "Rx MODIFY_BEARER_REQUEST, teid "TEID_FMT"\n", modify_bearer_pP->teid);
  sgw_display_s11teid2mme_mappings ();
  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, modify_bearer_pP->teid, (void **)&new_bearer_ctxt_info_p);

  if (HASH_TABLE_OK == hash_rc) {
    new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.default_bearer =
        modify_bearer_pP->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id;
    new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn = modify_bearer_pP->trxn;

    eps_bearer_ctxt_p =
        sgw_cm_get_eps_bearer_entry(&new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
            modify_bearer_pP->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id);

    if (NULL == eps_bearer_ctxt_p) {
      message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_MODIFY_BEARER_RESPONSE);

      if (!message_p) {
        OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
      }

      modify_response_p = &message_p->ittiMsg.s11_modify_bearer_response;
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id =
          modify_bearer_pP->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id;
      modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->bearer_contexts_marked_for_removal.num_bearer_context += 1;
      modify_response_p->cause.cause_value = CONTEXT_NOT_FOUND;
      modify_response_p->trxn = modify_bearer_pP->trxn;
      OAILOG_DEBUG (LOG_SPGW_APP, "Rx MODIFY_BEARER_REQUEST, eps_bearer_id %u CONTEXT_NOT_FOUND\n",
          modify_bearer_pP->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id);
      MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME,
                          NULL, 0, "0 S11_MODIFY_BEARER_RESPONSE ebi %u CONTEXT_NOT_FOUND trxn %u",
                          modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id, modify_response_p->trxn);
      rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
    } else {
      // TO DO
      FTEID_T_2_IP_ADDRESS_T ((&modify_bearer_pP->bearer_contexts_to_be_modified.bearer_contexts[0].s1_eNB_fteid), (&eps_bearer_ctxt_p->enb_ip_address_S1u));
      eps_bearer_ctxt_p->enb_teid_S1u = modify_bearer_pP->bearer_contexts_to_be_modified.bearer_contexts[0].s1_eNB_fteid.teid;
      {
        itti_sgi_update_end_point_response_t                   sgi_update_end_point_resp = {0};

        sgi_update_end_point_resp.context_teid = modify_bearer_pP->teid;
        sgi_update_end_point_resp.sgw_S1u_teid = eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up;
        sgi_update_end_point_resp.enb_S1u_teid = eps_bearer_ctxt_p->enb_teid_S1u;
        sgi_update_end_point_resp.eps_bearer_id = eps_bearer_ctxt_p->eps_bearer_id;
        sgi_update_end_point_resp.status = 0x00;
        rv = sgw_handle_sgi_endpoint_updated (&sgi_update_end_point_resp);
        if (RETURNok == rv) {
          if (spgw_config.pgw_config.pcef.automatic_push_dedicated_bearer_sdf_identifier) {
            // upon S/P-GW config, establish a dedicated radio bearer
            sgw_no_pcef_create_dedicated_bearer(modify_bearer_pP->teid);
          }
        }
      }
    }
  } else {
    message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_MODIFY_BEARER_RESPONSE);

    if (!message_p) {
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, -1);
    }

    modify_response_p = &message_p->ittiMsg.s11_modify_bearer_response;
    modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].eps_bearer_id = modify_bearer_pP->bearer_contexts_to_be_modified.bearer_contexts[0].eps_bearer_id;
    modify_response_p->bearer_contexts_marked_for_removal.bearer_contexts[0].cause.cause_value = CONTEXT_NOT_FOUND;
    modify_response_p->bearer_contexts_marked_for_removal.num_bearer_context += 1;
    modify_response_p->cause.cause_value = CONTEXT_NOT_FOUND;
    modify_response_p->trxn = modify_bearer_pP->trxn;
    OAILOG_DEBUG (LOG_SPGW_APP, "Rx MODIFY_BEARER_REQUEST, teid "TEID_FMT" CONTEXT_NOT_FOUND\n", modify_bearer_pP->teid);
    rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
  }

  OAILOG_FUNC_RETURN(LOG_SPGW_APP,  RETURNerror);
}



//------------------------------------------------------------------------------
int
sgw_handle_delete_session_request (
  const itti_s11_delete_session_request_t * const delete_session_req_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  itti_s11_delete_session_response_t      *delete_session_resp_p = NULL;
  MessageDef                              *message_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *ctx_p = NULL;
  int                                     rv = RETURNok;

  message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_DELETE_SESSION_RESPONSE);

  if (!message_p) {
    OAILOG_FUNC_RETURN(LOG_SPGW_APP,  RETURNerror);
  }
  delete_session_resp_p = &message_p->ittiMsg.s11_delete_session_response;
  OAILOG_WARNING (LOG_SPGW_APP, "Delete session handler needs to be completed...\n");

  if (delete_session_req_pP->indication_flags.oi) {
    OAILOG_DEBUG (LOG_SPGW_APP, "OI flag is set for this message indicating the request" "should be forwarded to P-GW entity\n");
  }

  hash_rc = hashtable_ts_get (
      sgw_app.s11_bearer_context_information_hashtable,
      delete_session_req_pP->teid, (void **)&ctx_p);

  if (HASH_TABLE_OK == hash_rc) {
    if ((delete_session_req_pP->sender_fteid_for_cp.ipv4 ) && (delete_session_req_pP->sender_fteid_for_cp.ipv6 )) {
      /*
       * Sender F-TEID IE present
       */
      if (delete_session_req_pP->teid != ctx_p->sgw_eps_bearer_context_information.mme_teid_S11) {
        delete_session_resp_p->teid = ctx_p->sgw_eps_bearer_context_information.mme_teid_S11;
        delete_session_resp_p->cause.cause_value = INVALID_PEER;
        OAILOG_DEBUG (LOG_SPGW_APP, "Mismatch in MME Teid for CP\n");
      } else {
        delete_session_resp_p->teid = delete_session_req_pP->sender_fteid_for_cp.teid;
      }
    } else {
      delete_session_resp_p->cause.cause_value = REQUEST_ACCEPTED;
      delete_session_resp_p->teid = ctx_p->sgw_eps_bearer_context_information.mme_teid_S11;

      itti_sgi_delete_end_point_request_t       sgi_delete_end_point_request;
      sgw_eps_bearer_ctxt_t                    *eps_bearer_ctxt_p = NULL;

      for (int ebix = 0; ebix < BEARERS_PER_UE; ebix++) {
        ebi_t ebi = INDEX_TO_EBI(ebix);
        eps_bearer_ctxt_p = sgw_cm_get_eps_bearer_entry(&ctx_p->sgw_eps_bearer_context_information.pdn_connection, ebi);

        if (eps_bearer_ctxt_p) {
          if (ebi != delete_session_req_pP->lbi) {
            rv = gtp_tunnel_ops->del_tunnel(eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up, eps_bearer_ctxt_p->enb_teid_S1u);
            if (rv < 0) {
              OAILOG_ERROR (LOG_SPGW_APP, "ERROR in deleting TUNNEL " TEID_FMT " (eNB) <-> (SGW) " TEID_FMT "\n",
                  eps_bearer_ctxt_p->enb_teid_S1u, eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up);
            }

#if ENABLE_SDF_MARKING
            for (int sdfx = 0; sdfx < eps_bearer_ctxt_p->num_sdf; sdfx++) {
              if (eps_bearer_ctxt_p->sdf_id[sdfx]) {
                bstring marking_command = bformat(
                    "iptables -D POSTROUTING -t mangle --out-interface gtp0 --dest %"PRIu8".%"PRIu8".%"PRIu8".%"PRIu8"/32 -m mark --mark 0x%04X -j MARK --set-mark %d",
                    NIPADDR(eps_bearer_ctxt_p->paa.ipv4_address.s_addr), eps_bearer_ctxt_p->sdf_id[sdfx], eps_bearer_ctxt_p->eps_bearer_id);
                async_system_command (TASK_SPGW_APP, false, bdata(marking_command));
                bdestroy_wrapper(&marking_command);
              }
            }
#endif
            eps_bearer_ctxt_p->num_sdf = 0;
          }
        }
      }

      eps_bearer_ctxt_p = sgw_cm_get_eps_bearer_entry(&ctx_p->sgw_eps_bearer_context_information.pdn_connection, delete_session_req_pP->lbi);
      if (eps_bearer_ctxt_p) {
        if (spgw_config.pgw_config.use_gtp_kernel_module) {
          rv = gtp_tunnel_ops->del_tunnel(eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up, eps_bearer_ctxt_p->enb_teid_S1u);
          if (rv < 0) {
            OAILOG_ERROR (LOG_SPGW_APP, "ERROR in deleting TUNNEL " TEID_FMT " (eNB) <-> (SGW) " TEID_FMT "\n",
                eps_bearer_ctxt_p->enb_teid_S1u, eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up);
          }
#if ENABLE_SDF_MARKING
          for (int sdfx = 0; sdfx < eps_bearer_ctxt_p->num_sdf; sdfx++) {
            if (eps_bearer_ctxt_p->sdf_id[sdfx]) {
              bstring marking_command = bformat(
                  "iptables -D POSTROUTING -t mangle --out-interface gtp0 --dest %"PRIu8".%"PRIu8".%"PRIu8".%"PRIu8"/32 -m mark --mark 0x%04X -j MARK --set-mark %d",
                  NIPADDR(eps_bearer_ctxt_p->paa.ipv4_address.s_addr), eps_bearer_ctxt_p->sdf_id[sdfx], eps_bearer_ctxt_p->eps_bearer_id);
              async_system_command (TASK_SPGW_APP, false, bdata(marking_command));
              bdestroy_wrapper(&marking_command);
            }
          }
#endif
        }
        eps_bearer_ctxt_p->num_sdf = 0;

        sgi_delete_end_point_request.context_teid = delete_session_req_pP->teid ;
        sgi_delete_end_point_request.sgw_S1u_teid = eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up;
        sgi_delete_end_point_request.eps_bearer_id = delete_session_req_pP->lbi;
        sgi_delete_end_point_request.pdn_type = ctx_p->sgw_eps_bearer_context_information.saved_message.pdn_type;
        memcpy (&sgi_delete_end_point_request.paa, &eps_bearer_ctxt_p->paa, sizeof (paa_t));

        sgw_handle_sgi_endpoint_deleted (&sgi_delete_end_point_request);
      } else {
        OAILOG_WARNING (LOG_SPGW_APP, "Can't find eps_bearer_entry for MME TEID "TEID_FMT" lbi %u\n", delete_session_req_pP->teid, delete_session_req_pP->lbi);
      }
      
      /*
       * Remove eps bearer context, S11 bearer context and s11 tunnel
       */

      sgw_cm_remove_eps_bearer_entry(
            &ctx_p->sgw_eps_bearer_context_information.pdn_connection,
            delete_session_req_pP->lbi);
      sgw_cm_remove_bearer_context_information(delete_session_req_pP->teid);
      sgw_cm_remove_s11_tunnel(delete_session_req_pP->teid);
    }

    delete_session_resp_p->trxn = delete_session_req_pP->trxn;
    delete_session_resp_p->peer_ip.s_addr = delete_session_req_pP->peer_ip.s_addr;
    MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME, NULL, 0, "0 S11_DELETE_SESSION_RESPONSE teid " TEID_FMT " cause %u trxn %u", delete_session_resp_p->teid, delete_session_resp_p->cause, delete_session_resp_p->trxn);
    rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);

  } else {
    /*
     * Context not found... set the cause to CONTEXT_NOT_FOUND
     * * * * 3GPP TS 29.274 #7.2.10.1
     */

    if ((delete_session_req_pP->sender_fteid_for_cp.ipv4 == 0) && (delete_session_req_pP->sender_fteid_for_cp.ipv6 == 0)) {
      delete_session_resp_p->teid = 0;
    } else {
      delete_session_resp_p->teid = delete_session_req_pP->sender_fteid_for_cp.teid;
    }

    delete_session_resp_p->cause.cause_value = CONTEXT_NOT_FOUND;
    delete_session_resp_p->trxn = delete_session_req_pP->trxn;
    delete_session_resp_p->peer_ip.s_addr = delete_session_req_pP->peer_ip.s_addr;
    MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME, NULL, 0, "0 S11_DELETE_SESSION_RESPONSE CONTEXT_NOT_FOUND trxn %u", delete_session_resp_p->trxn);
    rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
  }

  OAILOG_FUNC_RETURN(LOG_SPGW_APP, RETURNerror);
}

//------------------------------------------------------------------------------
static void sgw_release_all_enb_related_information (sgw_eps_bearer_ctxt_t * const eps_bearer_ctxt)
{

  OAILOG_FUNC_IN(LOG_SPGW_APP);
  if ( eps_bearer_ctxt) {
    memset (&eps_bearer_ctxt->enb_ip_address_S1u, 0, sizeof (eps_bearer_ctxt->enb_ip_address_S1u));
    eps_bearer_ctxt->enb_teid_S1u = INVALID_TEID;
  }
  OAILOG_FUNC_OUT(LOG_SPGW_APP);
}


/* From GPP TS 23.401 version 11.11.0 Release 11, section 5.3.5 S1 release procedure:
   The S-GW releases all eNodeB related information (address and TEIDs) for the UE and responds with a Release
   Access Bearers Response message to the MME. Other elements of the UE's S-GW context are not affected. The
   S-GW retains the S1-U configuration that the S-GW allocated for the UE's bearers. The S-GW starts buffering
   downlink packets received for the UE and initiating the "Network Triggered Service Request" procedure,
   described in clause 5.3.4.3, if downlink packets arrive for the UE.
*/
//------------------------------------------------------------------------------
int
sgw_handle_release_access_bearers_request (
  const itti_s11_release_access_bearers_request_t * const release_access_bearers_req_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  itti_s11_release_access_bearers_response_t        *release_access_bearers_resp_p = NULL;
  MessageDef                             *message_p = NULL;
  s_plus_p_gw_eps_bearer_context_information_t *ctx_p = NULL;
  int                                     rv = RETURNok;

  OAILOG_DEBUG (LOG_SPGW_APP, "Release Access Bearer Request Received in SGW\n");

  message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_RELEASE_ACCESS_BEARERS_RESPONSE);

  if (message_p == NULL) {
    OAILOG_FUNC_RETURN(LOG_SPGW_APP,  RETURNerror);
  }

  release_access_bearers_resp_p = &message_p->ittiMsg.s11_release_access_bearers_response;

  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, release_access_bearers_req_pP->teid, (void **)&ctx_p);

  if (HASH_TABLE_OK == hash_rc) {
    release_access_bearers_resp_p->cause.cause_value = REQUEST_ACCEPTED;
    release_access_bearers_resp_p->teid = ctx_p->sgw_eps_bearer_context_information.mme_teid_S11;
    release_access_bearers_resp_p->trxn = release_access_bearers_req_pP->trxn;
//#pragma message  "TODO Here the release (sgw_handle_release_access_bearers_request)"
    // TODO iterator
    for (int ebx = 0; ebx < BEARERS_PER_UE; ebx++) {
      sgw_eps_bearer_ctxt_t * eps_bearer_ctxt = ctx_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers_array[ebx];
      if (eps_bearer_ctxt) {
        sgw_release_all_enb_related_information(eps_bearer_ctxt);
      }
    }
    // TODO The S-GW starts buffering downlink packets received for the UE
    // (set target on GTPUSP to order the buffering)
    MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME, NULL, 0, "0 S11_RELEASE_ACCESS_BEARERS_RESPONSE S11 MME teid " TEID_FMT " cause REQUEST_ACCEPTED", release_access_bearers_resp_p->teid);
    rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);

    OAILOG_DEBUG (LOG_SPGW_APP, "Release Access Bearer Respone sent to SGW\n");
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
  } else {
    release_access_bearers_resp_p->cause.cause_value = CONTEXT_NOT_FOUND;
    release_access_bearers_resp_p->teid = 0;
    MSC_LOG_TX_MESSAGE (MSC_SP_GWAPP_MME, MSC_S11_MME, NULL, 0, "0 S11_RELEASE_ACCESS_BEARERS_RESPONSE cause CONTEXT_NOT_FOUND");
    rv = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
    OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
  }
}
//------------------------------------------------------------------------------
// hardcoded parameters as a starting point
int sgw_no_pcef_create_dedicated_bearer(s11_teid_t teid)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  int                                    rc = RETURNerror;

  s_plus_p_gw_eps_bearer_context_information_t *s_plus_p_gw_eps_bearer_ctxt_info_p = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;

  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, teid, (void **)&s_plus_p_gw_eps_bearer_ctxt_info_p);

  if (HASH_TABLE_OK == hash_rc) {

    MessageDef                             *message_p = itti_alloc_new_message (TASK_SPGW_APP, S11_CREATE_BEARER_REQUEST);

    if (message_p) {

      itti_s11_create_bearer_request_t *s11_create_bearer_request = &message_p->ittiMsg.s11_create_bearer_request;

      //s11_create_bearer_request->trxn = s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
      s11_create_bearer_request->peer_ip.s_addr = s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_ip_address_S11.address.ipv4_address.s_addr;
      s11_create_bearer_request->local_teid = s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.s_gw_teid_S11_S4;

      s11_create_bearer_request->teid = s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_teid_S11;
      //s11_create_bearer_request->pti;
      OAILOG_DEBUG (LOG_SPGW_APP, "Creating bearer teid " TEID_FMT " remote teid " TEID_FMT "\n", teid, s11_create_bearer_request->teid);

      sgw_eps_bearer_ctxt_t *eps_bearer_ctxt_p  = calloc (1, sizeof (sgw_eps_bearer_ctxt_t));
      sgw_eps_bearer_ctxt_t *default_eps_bearer_entry_p =
                sgw_cm_get_eps_bearer_entry(&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection,
                    s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.default_bearer);

      uint8_t number_of_packet_filters = 0;
      rc = pgw_pcef_get_sdf_parameters (spgw_config.pgw_config.pcef.automatic_push_dedicated_bearer_sdf_identifier,
          &eps_bearer_ctxt_p->eps_bearer_qos,
          &eps_bearer_ctxt_p->tft.packetfilterlist.createnewtft[0],
          &number_of_packet_filters);

      eps_bearer_ctxt_p->eps_bearer_id = 0;
      eps_bearer_ctxt_p->paa = default_eps_bearer_entry_p->paa;
      eps_bearer_ctxt_p->s_gw_ip_address_S1u_S12_S4_up = default_eps_bearer_entry_p->s_gw_ip_address_S1u_S12_S4_up;
      eps_bearer_ctxt_p->s_gw_ip_address_S5_S8_up = default_eps_bearer_entry_p->s_gw_ip_address_S5_S8_up;
      eps_bearer_ctxt_p->tft.tftoperationcode = TRAFFIC_FLOW_TEMPLATE_OPCODE_CREATE_NEW_TFT;
      eps_bearer_ctxt_p->tft.ebit = TRAFFIC_FLOW_TEMPLATE_PARAMETER_LIST_IS_NOT_INCLUDED;
      eps_bearer_ctxt_p->tft.numberofpacketfilters = number_of_packet_filters;

      eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up = sgw_get_new_s1u_teid ();
      eps_bearer_ctxt_p->s_gw_ip_address_S1u_S12_S4_up.pdn_type = IPv4;
      eps_bearer_ctxt_p->s_gw_ip_address_S1u_S12_S4_up.address.ipv4_address.s_addr = sgw_app.sgw_ip_address_S1u_S12_S4_up.s_addr;

      // Put in cache the sgw_eps_bearer_entry_t
      // TODO create a procedure with a time out
      pgw_ni_cbr_proc_t* pgw_ni_cbr_proc    = pgw_create_procedure_create_bearer(s_plus_p_gw_eps_bearer_ctxt_info_p);
      pgw_ni_cbr_proc->sdf_id               = spgw_config.pgw_config.pcef.automatic_push_dedicated_bearer_sdf_identifier;
      pgw_ni_cbr_proc->teid                 = teid;
      struct sgw_eps_bearer_entry_wrapper_s *sgw_eps_bearer_entry_wrapper = calloc(1, sizeof(*sgw_eps_bearer_entry_wrapper));
      sgw_eps_bearer_entry_wrapper->sgw_eps_bearer_entry = eps_bearer_ctxt_p;
      LIST_INSERT_HEAD((pgw_ni_cbr_proc->pending_eps_bearers), sgw_eps_bearer_entry_wrapper, entries);

      s11_create_bearer_request->linked_eps_bearer_id = s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.default_bearer; ///< M: This IE shall be included to indicate the default bearer
      //s11_create_bearer_request->pco;
      s11_create_bearer_request->bearer_contexts.num_bearer_context = 1;
      s11_create_bearer_request->bearer_contexts.bearer_contexts[0].eps_bearer_id = 0;
      memcpy(&s11_create_bearer_request->bearer_contexts.bearer_contexts[0].tft, &eps_bearer_ctxt_p->tft, sizeof(eps_bearer_ctxt_p->tft));
      // TODO remove hardcoded
      s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s1u_sgw_fteid.ipv4           = 1;
      s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s1u_sgw_fteid.ipv6           = 0;
      s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s1u_sgw_fteid.interface_type = S1_U_SGW_GTP_U;
      s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s1u_sgw_fteid.teid           = eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up;
      s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s1u_sgw_fteid.ipv4_address.s_addr   = eps_bearer_ctxt_p->s_gw_ip_address_S1u_S12_S4_up.address.ipv4_address.s_addr;

      //s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s5_s8_u_pgw_fteid =;
      //s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s12_sgw_fteid     =;
      //s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s4_u_sgw_fteid    =;
      //s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s2b_u_pgw_fteid   =;
      //s11_create_bearer_request->bearer_contexts.bearer_contexts[0].s2a_u_pgw_fteid   =;
      memcpy(&s11_create_bearer_request->bearer_contexts.bearer_contexts[0].bearer_level_qos,
          &eps_bearer_ctxt_p->eps_bearer_qos,
          sizeof(eps_bearer_ctxt_p->eps_bearer_qos));

      rc = itti_send_msg_to_task (TASK_S11, INSTANCE_DEFAULT, message_p);
      OAILOG_FUNC_RETURN(LOG_SPGW_APP, rc);
    }
  }
  OAILOG_FUNC_RETURN(LOG_SPGW_APP, rc);
}

//------------------------------------------------------------------------------
int
sgw_handle_create_bearer_response (
  const itti_s11_create_bearer_response_t * const create_bearer_response_pP)
{
  OAILOG_FUNC_IN(LOG_SPGW_APP);
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  s_plus_p_gw_eps_bearer_context_information_t *ctx_p = NULL;
  int                                     rv = RETURNok;

  hash_rc = hashtable_ts_get (sgw_app.s11_bearer_context_information_hashtable, create_bearer_response_pP->teid, (void **)&ctx_p);

  if (HASH_TABLE_OK == hash_rc) {
    if ((REQUEST_ACCEPTED == create_bearer_response_pP->cause.cause_value) ||
        (REQUEST_ACCEPTED_PARTIALLY == create_bearer_response_pP->cause.cause_value)) {
      for (int i=0; i < create_bearer_response_pP->bearer_contexts.num_bearer_context; i++) {
        if (REQUEST_ACCEPTED == create_bearer_response_pP->bearer_contexts.bearer_contexts[i].cause.cause_value) {
          sgw_eps_bearer_ctxt_t                 *eps_bearer_ctxt_p = NULL;
          struct sgw_eps_bearer_entry_wrapper_s  *sgw_eps_bearer_entry_wrapper = NULL;
          struct sgw_eps_bearer_entry_wrapper_s  *sgw_eps_bearer_entry_wrapper2 = NULL;

          pgw_ni_cbr_proc_t* pgw_ni_cbr_proc    = pgw_get_procedure_create_bearer(ctx_p);

          if (pgw_ni_cbr_proc) {

            sgw_eps_bearer_entry_wrapper = LIST_FIRST(pgw_ni_cbr_proc->pending_eps_bearers);
            while (sgw_eps_bearer_entry_wrapper != NULL) {
              // Save
              sgw_eps_bearer_entry_wrapper2 = LIST_NEXT(sgw_eps_bearer_entry_wrapper, entries);
              eps_bearer_ctxt_p = sgw_eps_bearer_entry_wrapper->sgw_eps_bearer_entry;
              // This comparison may be enough, else compare IP address also
              if (create_bearer_response_pP->bearer_contexts.bearer_contexts[i].s1u_sgw_fteid.teid == eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up) {
                // List management
                LIST_REMOVE(sgw_eps_bearer_entry_wrapper, entries);
                free_wrapper((void**)&sgw_eps_bearer_entry_wrapper);

                eps_bearer_ctxt_p->eps_bearer_id = create_bearer_response_pP->bearer_contexts.bearer_contexts[i].eps_bearer_id;

                bstring bip = fteid_ip_address_to_bstring(&create_bearer_response_pP->bearer_contexts.bearer_contexts[i].s1u_enb_fteid);
                bstring_to_ip_address(bip, &eps_bearer_ctxt_p->enb_ip_address_S1u);
                eps_bearer_ctxt_p->enb_teid_S1u = create_bearer_response_pP->bearer_contexts.bearer_contexts[i].s1u_enb_fteid.teid;
                bdestroy_wrapper(&bip);

                eps_bearer_ctxt_p = sgw_cm_insert_eps_bearer_ctxt_in_collection (&ctx_p->sgw_eps_bearer_context_information.pdn_connection, eps_bearer_ctxt_p);


                if (HASH_TABLE_OK == hash_rc) {
                  struct in_addr enb = {.s_addr = 0};
                  enb.s_addr = eps_bearer_ctxt_p->enb_ip_address_S1u.address.ipv4_address.s_addr;

                  struct in_addr ue = {.s_addr = 0};
                  ue.s_addr = eps_bearer_ctxt_p->paa.ipv4_address.s_addr;

                  if (spgw_config.pgw_config.use_gtp_kernel_module) {
                    rv = gtp_tunnel_ops->add_tunnel(ue, enb, eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up, eps_bearer_ctxt_p->enb_teid_S1u);
                    if (rv < 0) {
                      OAILOG_ERROR (LOG_SPGW_APP, "ERROR in setting up TUNNEL err=%d\n", rv);
                    }

                    if (rv < 0) {
                      OAILOG_INFO (LOG_SPGW_APP, "Failed to setup EPS bearer id %u tunnel " TEID_FMT " (eNB) <-> (SGW) " TEID_FMT "\n",
                          eps_bearer_ctxt_p->eps_bearer_id, eps_bearer_ctxt_p->enb_teid_S1u, eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up);
                    } else {

#if ENABLE_SDF_MARKING
                      bstring marking_command = bformat(
                          "iptables -A POSTROUTING -t mangle --out-interface gtp0 --dest %"PRIu8".%"PRIu8".%"PRIu8".%"PRIu8"/32 -m mark --mark 0x%04X -j MARK --set-mark %d",
                          NIPADDR(eps_bearer_ctxt_p->paa.ipv4_address.s_addr), pgw_ni_cbr_proc->sdf_id, eps_bearer_ctxt_p->eps_bearer_id);
                      async_system_command (TASK_SPGW_APP, false, bdata(marking_command));

                      AssertFatal((TRAFFIC_FLOW_TEMPLATE_NB_PACKET_FILTERS_MAX > eps_bearer_ctxt_p->num_sdf), "Too much flows aggregated in this Bearer (should not happen => see MME)");
                      if (TRAFFIC_FLOW_TEMPLATE_NB_PACKET_FILTERS_MAX > eps_bearer_ctxt_p->num_sdf) {
                        eps_bearer_ctxt_p->sdf_id[eps_bearer_ctxt_p->num_sdf] = pgw_ni_cbr_proc->sdf_id;
                        eps_bearer_ctxt_p->num_sdf += 1;
                      }

                      bdestroy_wrapper(&marking_command);
#endif
                      OAILOG_INFO (LOG_SPGW_APP, "Setup EPS bearer id %u tunnel " TEID_FMT " (eNB) <-> (SGW) " TEID_FMT "\n",
                          eps_bearer_ctxt_p->eps_bearer_id, eps_bearer_ctxt_p->enb_teid_S1u, eps_bearer_ctxt_p->s_gw_teid_S1u_S12_S4_up);
                    }
                  }
                } else {
                  OAILOG_INFO (LOG_SPGW_APP, "Failed to setup EPS bearer id %u\n", eps_bearer_ctxt_p->eps_bearer_id);
                }
                // Restore
                sgw_eps_bearer_entry_wrapper = sgw_eps_bearer_entry_wrapper2;

                break;
              }
            }
            sgw_eps_bearer_entry_wrapper = LIST_FIRST(pgw_ni_cbr_proc->pending_eps_bearers);
            if (!sgw_eps_bearer_entry_wrapper) {
              LIST_INIT(pgw_ni_cbr_proc->pending_eps_bearers);
              free_wrapper((void**)&pgw_ni_cbr_proc->pending_eps_bearers);

              LIST_REMOVE((pgw_base_proc_t *)pgw_ni_cbr_proc, entries);
              pgw_free_procedure_create_bearer(&pgw_ni_cbr_proc);
            }
          }
        } else {
          OAILOG_DEBUG (LOG_SPGW_APP, "Creation of bearer " TEID_FMT "\n", create_bearer_response_pP->teid);
        }
      }
    }
  } else {
    // context not found
    OAILOG_DEBUG (LOG_SPGW_APP, "Context not found for teid " TEID_FMT "\n", create_bearer_response_pP->teid);
  }

  OAILOG_FUNC_RETURN(LOG_SPGW_APP, rv);
}
