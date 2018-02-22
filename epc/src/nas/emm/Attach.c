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

/*****************************************************************************

  Source      Attach.c

  Version     0.1

  Date        2012/12/04

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines the attach related EMM procedure executed by the
        Non-Access Stratum.

        To get internet connectivity from the network, the network
        have to know about the UE. When the UE is switched on, it
        has to initiate the attach procedure to get initial access
        to the network and register its presence to the Evolved
        Packet Core (EPC) network in order to receive EPS services.

        As a result of a successful attach procedure, a context is
        created for the UE in the MME, and a default bearer is esta-
        blished between the UE and the PDN-GW. The UE gets the home
        agent IPv4 and IPv6 addresses and full connectivity to the
        IP network.

        The network may also initiate the activation of additional
        dedicated bearers for the support of a specific service.

*****************************************************************************/

#include <string.h>

#include "gcc_diag.h"
#include "bstrlib.h"
#include "dynamic_memory_check.h"
#include "log.h"
#include "msc.h"
#include "obj_hashtable.h"
#include "nas_timer.h"

#include "conversions.h"
#include "3gpp_requirements_24.301.h"
#include "emm_proc.h"
#include "networkDef.h"
#include "emmData.h"
#include "emm_sap.h"
#include "esm_sap.h"
#include "emm_cause.h"
#include "NasSecurityAlgorithms.h"
#include "mme_api.h"
#include "mme_app_defs.h"
#include "mme_app_ue_context.h"
#include "mme_config.h"
#include "nas_itti_messaging.h"


/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* String representation of the EPS attach type */
static const char                      *_emm_attach_type_str[] = {
  "EPS", "IMSI", "EMERGENCY", "RESERVED"
};


/*
   --------------------------------------------------------------------------
        Internal data handled by the attach procedure in the MME
   --------------------------------------------------------------------------
*/
/*
   Timer handlers
*/
static void                            *_emm_attach_t3450_handler (
  void *);

/*
   Functions that may initiate EMM common procedures
*/
static int                              _emm_attach_identify (
  void *);
static int                              _emm_attach_security (
  void *);
static int                              _emm_attach (
  void *);

/*
   Abnormal case attach procedures
*/
static int                              _emm_attach_release (
  void *);
static int                              _emm_attach_reject (
  void *);
static int                              _emm_attach_abort (
  void *);

static int                              _emm_attach_have_changed (
  const emm_data_context_t * ctx,
  emm_proc_attach_type_t type,
  ksi_t ksi,
  guti_t * guti,
  imsi_t * imsi,
  imei_t * imei,
  int eea,
  int eia,
  int ucs2,
  int uea,
  int uia,
  int gea,
  int umts_present,
  int gprs_present);

static int                              _emm_attach_update (
  emm_data_context_t * ctx,
  mme_ue_s1ap_id_t ue_id,
  emm_proc_attach_type_t type,
  ksi_t ksi,
  bool is_native_guti,
  guti_t * guti,
  imsi_t * imsi,
  imei_t * imei,
  const tai_t   * const originating_tai,
  const int eea,
  const int eia,
  const int ucs2,
  const int uea,
  const int uia,
  const int gea,
  const bool umts_present,
  const bool gprs_present,
  const_bstring esm_msg_pP);

/*
   Internal data used for attach procedure
*/
typedef struct attach_data_s {
  unsigned int                            ue_id; /* UE identifier        */
#define ATTACH_COUNTER_MAX  5
  unsigned int                            retransmission_count; /* Retransmission counter   */
  bstring                                 esm_msg;      /* ESM message to be sent within
                                                         * the Attach Accept message    */
} attach_data_t;

static int                              _emm_attach_accept (
  emm_data_context_t * emm_ctx,
  attach_data_t * data);
static int                              _emm_attach_accept_retx(
  emm_data_context_t * emm_ctx,
  attach_data_t * data);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/


/*
   --------------------------------------------------------------------------
            Attach procedure executed by the MME
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_attach_request()                                 **
 **                                                                        **
 ** Description: Performs the UE requested attach procedure                **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.1.2.3                         **
 **      The network may initiate EMM common procedures, e.g. the  **
 **      identification, authentication and security mode control  **
 **      procedures during the attach procedure, depending on the  **
 **      information received in the ATTACH REQUEST message (e.g.  **
 **      IMSI, GUTI and KSI).                                      **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      type:      Type of the requested attach               **
 **      native_ksi:    true if the security context is of type    **
 **             native (for KSIASME)                       **
 **      ksi:       The NAS ket sey identifier                 **
 **      native_guti:   true if the provided GUTI is native GUTI   **
 **      guti:      The GUTI if provided by the UE             **
 **      imsi:      The IMSI if provided by the UE             **
 **      imei:      The IMEI if provided by the UE             **
 **      last_visited_registered_tai:       Identifies the last visited tracking area  **
 **             the UE is registered to                    **
 **      eea:       Supported EPS encryption algorithms        **
 **      eia:       Supported EPS integrity algorithms         **
 **      esm_msg_pP:   PDN connectivity request ESM message       **
 **      Others:    _emm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _emm_data                                  **
 **                                                                        **
 ***************************************************************************/
int
emm_proc_attach_request (
  mme_ue_s1ap_id_t ue_id,
  emm_proc_attach_type_t type,
  AdditionalUpdateType additional_update_type,
  bool is_native_ksi,
  ksi_t     ksi,
  bool is_native_guti,
  guti_t * guti,
  imsi_t * imsi,
  imei_t * imei,
  tai_t * last_visited_registered_tai,
  const tai_t   * const originating_tai,
  const ecgi_t   * const originating_ecgi,
  const int eea,
  const int eia,
  const int ucs2,
  const int uea,
  const int uia,
  const int gea,
  const bool umts_present,
  const bool gprs_present,
  const_bstring esm_msg_pP,
  const nas_message_decode_status_t  * const decode_status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_data_context_t                      ue_ctx;
  emm_fsm_state_t                         fsm_state = EMM_DEREGISTERED;
  bool                                    create_new_emm_ctxt = false;
  emm_data_context_t                     *new_emm_ctx = NULL;
  emm_data_context_t                     *imsi_emm_ctx = NULL;
  emm_data_context_t                     *guti_emm_ctx = NULL;
  imsi64_t                                imsi64  = INVALID_IMSI64;
  mme_ue_s1ap_id_t                        old_ue_id = INVALID_MME_UE_S1AP_ID;

  if (imsi) {
    IMSI_TO_IMSI64(imsi,imsi64);
  }

  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - EPS attach type = %s (%d) requested (ue_id=" MME_UE_S1AP_ID_FMT ")\n", _emm_attach_type_str[type], type, ue_id);
  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - umts_present = %u gprs_present = %u\n", umts_present, gprs_present);
  /*
   * Initialize the temporary UE context
   */
  memset (&ue_ctx, 0, sizeof (emm_data_context_t));
  ue_ctx.is_dynamic = false;
  ue_ctx.ue_id = ue_id;
  
  // Check whether request if for emergency bearer service.

  /*
   * Requirement MME24.301R10_5.5.1.1_1
   * MME not configured to support attach for emergency bearer services
   * shall reject any request to attach with an attach type set to "EPS
   * emergency attach".
   */
  if (!(_emm_data.conf.eps_network_feature_support & EPS_NETWORK_FEATURE_SUPPORT_EMERGENCY_BEARER_SERVICES_IN_S1_MODE_SUPPORTED) &&
      (type == EMM_ATTACH_TYPE_EMERGENCY)) {
    REQUIREMENT_3GPP_24_301(R10_5_5_1__1);
    // TODO: update this if/when emergency attach is supported
    ue_ctx.emm_cause =
      imei ? EMM_CAUSE_IMEI_NOT_ACCEPTED : EMM_CAUSE_NOT_AUTHORIZED_IN_PLMN;
    /*
     * Do not accept the UE to attach for emergency services
     */
    rc = _emm_attach_reject (&ue_ctx);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  /*
   * Get the UE's EMM context if it exists
   */
   new_emm_ctx = emm_data_context_get (&_emm_data, ue_id);
   if (new_emm_ctx == NULL)
   {
    // Search UE context using GUTI -  
    if (guti) { // no need for  && (is_native_guti)
      guti_emm_ctx = emm_data_context_get_by_guti (&_emm_data, guti);
      if (guti_emm_ctx) {
        
        /* 
         * This implies either UE or eNB has not sent S-TMSI in intial UE message even though UE has old GUTI. 
         * Trigger clean up
         */
          emm_sap_t                               emm_sap = {0};
          emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
          emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = guti_emm_ctx->ue_id;
          rc = emm_sap_send (&emm_sap);
      }
      // Allocate new context and process the new request as fresh attach request
      create_new_emm_ctxt = true;
    }
    if (imsi) {
      imsi_emm_ctx = emm_data_context_get_by_imsi (&_emm_data, imsi64);
      if (imsi_emm_ctx) {
        old_ue_id = imsi_emm_ctx->ue_id; 
        fsm_state = emm_fsm_get_status (old_ue_id, imsi_emm_ctx);
        if (emm_ctx_is_common_procedure_running(imsi_emm_ctx, EMM_CTXT_COMMON_PROC_SMC)) {
          REQUIREMENT_3GPP_24_301(R10_5_4_3_7_c);
          emm_sap_t                               emm_sap = {0};
          emm_sap.primitive = EMMREG_PROC_ABORT;
          emm_sap.u.emm_reg.ue_id = imsi_emm_ctx->ue_id;
          // TODOdata->notify_failure = true;
          rc = emm_sap_send (&emm_sap);
          // trigger clean up 
          emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
          emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = old_ue_id;
          rc = emm_sap_send (&emm_sap);
          // Allocate new context and process the new request as fresh attach request
          create_new_emm_ctxt = true;
        }
        if (emm_ctx_is_common_procedure_running(imsi_emm_ctx, EMM_CTXT_COMMON_PROC_IDENT)) {
          if (emm_ctx_is_specific_procedure(imsi_emm_ctx, EMM_CTXT_SPEC_PROC_ATTACH)) {
            if (emm_ctx_is_specific_procedure(imsi_emm_ctx, EMM_CTXT_SPEC_PROC_ATTACH_ACCEPT_SENT | EMM_CTXT_SPEC_PROC_ATTACH_REJECT_SENT)) {
              REQUIREMENT_3GPP_24_301(R10_5_4_4_6_c); // continue
              // TODO Need to be reviwed and corrected
            } else {
              REQUIREMENT_3GPP_24_301(R10_5_4_4_6_d);
              emm_sap_t                               emm_sap = {0};
              emm_sap.primitive = EMMREG_PROC_ABORT;
              emm_sap.u.emm_reg.ue_id = imsi_emm_ctx->ue_id;
              // TODOdata->notify_failure = true;
              rc = emm_sap_send (&emm_sap);
              // TODO Need to be reviwed and corrected
              // trigger clean up 
              emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
              emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = old_ue_id;
              rc = emm_sap_send (&emm_sap);
              // Allocate new context and process the new request as fresh attach request
              create_new_emm_ctxt = true;
            }
          } else {
              // TODO Need to be reviwed and corrected
            REQUIREMENT_3GPP_24_301(R10_5_4_4_6_c); // continue
          }
        }
        if (EMM_REGISTERED == fsm_state) {
          REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_f);
          if (imsi_emm_ctx->is_has_been_attached) {
            OAILOG_TRACE (LOG_NAS_EMM, "EMM-PROC  - the new ATTACH REQUEST is progressed\n");
            // Trigger clean up
            emm_sap_t                               emm_sap = {0};
            emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
            emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = imsi_emm_ctx->ue_id;
            rc = emm_sap_send (&emm_sap);
            // Allocate new context and process the new request as fresh attach request
            create_new_emm_ctxt = true;
          }
        } else if (emm_ctx_is_specific_procedure(imsi_emm_ctx, EMM_CTXT_SPEC_PROC_ATTACH_ACCEPT_SENT)) {// && (!emm_ctx->is_attach_complete_received): implicit
          imsi_emm_ctx->num_attach_request++;
          if (_emm_attach_have_changed (imsi_emm_ctx, type, ksi, guti, imsi, imei, eea, eia, ucs2, uea, uia, gea, umts_present, gprs_present)) {
            OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Attach parameters have changed\n");
            REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_d__1);
            /*
             * If one or more of the information elements in the ATTACH REQUEST message differ from the ones
             * received within the previous ATTACH REQUEST message, the previously initiated attach procedure shall
             * be aborted if the ATTACH COMPLETE message has not been received and the new attach procedure shall
             * be progressed;
             */
              emm_sap_t                               emm_sap = {0};

              emm_sap.primitive = EMMREG_PROC_ABORT;
              emm_sap.u.emm_reg.ue_id = old_ue_id;
              emm_sap.u.emm_reg.ctx = imsi_emm_ctx;
              rc = emm_sap_send (&emm_sap);
              // trigger clean up 
              emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
              emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = old_ue_id;
              rc = emm_sap_send (&emm_sap);
              // Allocate new context and process the new request as fresh attach request
              create_new_emm_ctxt = true;
            }
            else {
              imsi_emm_ctx->num_attach_request++;
              REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_d__2);
              /*
               * - if the information elements do not differ, then the ATTACH ACCEPT message shall be resent and the timer
               * T3450 shall be restarted if an ATTACH COMPLETE message is expected. In that case, the retransmission
               * counter related to T3450 is not incremented.
               */
              attach_data_t *data = (attach_data_t *) emm_proc_common_get_args (ue_id);

              _emm_attach_accept_retx(imsi_emm_ctx, data);
              // Clean up new UE context that was created to handle new attach request 
              nas_itti_detach_req(ue_id);
              OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
            }

        } else if ((imsi_emm_ctx) && (0 < imsi_emm_ctx->num_attach_request) &&
          (!emm_ctx_is_specific_procedure(imsi_emm_ctx, EMM_CTXT_SPEC_PROC_ATTACH_ACCEPT_SENT)) &&
          (!emm_ctx_is_specific_procedure(imsi_emm_ctx, EMM_CTXT_SPEC_PROC_ATTACH_REJECT_SENT))) {
          
          if (_emm_attach_have_changed (imsi_emm_ctx, type, ksi, guti, imsi, imei, eea, eia, ucs2, uea, uia, gea, umts_present, gprs_present)) {
            OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Attach parameters have changed\n");
            REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_e__1);
            /*
             * If one or more of the information elements in the ATTACH REQUEST message differs from the ones
             * received within the previous ATTACH REQUEST message, the previously initiated attach procedure shall
             * be aborted and the new attach procedure shall be executed;
             */
            emm_sap_t                               emm_sap = {0};

            emm_sap.primitive = EMMREG_PROC_ABORT;
            emm_sap.u.emm_reg.ue_id = old_ue_id;
            emm_sap.u.emm_reg.ctx = imsi_emm_ctx;
            rc = emm_sap_send (&emm_sap);
            // trigger clean up 
            emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
            emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = old_ue_id;
            rc = emm_sap_send (&emm_sap);
            // Allocate new context and process the new request as fresh attach request
            create_new_emm_ctxt = true;
          } else {
   
            REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_e__2);
            /*
             * if the information elements do not differ, then the network shall continue with the previous attach procedure
             * and shall ignore the second ATTACH REQUEST message.
             */
             // Clean up new UE context that was created to handle new attach request 
             nas_itti_detach_req(ue_id);
             OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Received duplicated Attach Request\n");
             OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
          }
        } 
      }// if imsi_emm_ctx != NULL
      // Allocate new context and process the new request as fresh attach request
      create_new_emm_ctxt = true;
    }// If IMSI !=NULL 
  } else {
    // This implies UE has GUTI from previous registration procedure
    // Cleanup and remove current EMM context - TODO - IP address release
    /* Note - 
     * Since we dont support IP address release here , this can result in duplicate session  allocation.
     * So rejecting the attach and deleting the old context so that next attach from UE can be handled w/o 
     * causing duplicate session allocation.
     */
    rc = _emm_attach_reject (new_emm_ctx);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
    #if 0 
    // TODO - send session delete request towards SGW 
    /*
     * Notify ESM that all EPS bearer contexts allocated for this UE have
     * to be locally deactivated
     */
    MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_ESM_MME, NULL, 0, "0 ESM_EPS_BEARER_CONTEXT_DEACTIVATE_REQ ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
    esm_sap_t                               esm_sap = {0};

    esm_sap.primitive = ESM_EPS_BEARER_CONTEXT_DEACTIVATE_REQ;
    esm_sap.ue_id = ue_id;
    esm_sap.ctx = new_emm_ctx;
    esm_sap.data.eps_bearer_context_deactivate.ebi = ESM_SAP_ALL_EBI;
    rc = esm_sap_send (&esm_sap);

    emm_sap_t                               emm_sap = {0};

    /*
     * Notify EMM that the UE has been implicitly detached
     */
    emm_sap.primitive = EMMREG_DETACH_REQ;
    emm_sap.u.emm_reg.ue_id = ue_id;
    emm_sap.u.emm_reg.ctx = new_emm_ctx;
    rc = emm_sap_send (&emm_sap);
	
    _clear_emm_ctxt(new_emm_ctx);
    create_new_emm_ctxt = true;
    #endif 
  }
  if (create_new_emm_ctxt) {
    /*
     * Create UE's EMM context
     */
    new_emm_ctx = (emm_data_context_t *) calloc (1, sizeof (emm_data_context_t));

    if (!new_emm_ctx) {
      OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Failed to create EMM context\n");
      ue_ctx.emm_cause = EMM_CAUSE_ILLEGAL_UE;
      /*
       * Do not accept the UE to attach to the network
       */
      rc = _emm_attach_reject (&ue_ctx);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
    }
    new_emm_ctx->num_attach_request++;
    new_emm_ctx->ue_id = ue_id; 
    OAILOG_NOTICE (LOG_NAS_EMM, "EMM-PROC  - Create EMM context ue_id = " MME_UE_S1AP_ID_FMT "\n", ue_id);
    new_emm_ctx->is_dynamic = true;
    bdestroy(new_emm_ctx->esm_msg);
    new_emm_ctx->attach_type = type;
    new_emm_ctx->additional_update_type = additional_update_type;
    new_emm_ctx->emm_cause = EMM_CAUSE_SUCCESS;
    new_emm_ctx->_emm_fsm_status = EMM_INVALID;
    new_emm_ctx->T3450.id = NAS_TIMER_INACTIVE_ID;
    new_emm_ctx->T3450.sec = T3450_DEFAULT_VALUE;
    new_emm_ctx->T3460.id = NAS_TIMER_INACTIVE_ID;
    new_emm_ctx->T3460.sec = T3460_DEFAULT_VALUE;
    new_emm_ctx->T3470.id = NAS_TIMER_INACTIVE_ID;
    new_emm_ctx->T3470.sec = T3470_DEFAULT_VALUE;
    emm_fsm_set_status (ue_id, new_emm_ctx, EMM_DEREGISTERED);

    emm_ctx_clear_guti(new_emm_ctx);
    emm_ctx_clear_old_guti(new_emm_ctx);
    emm_ctx_clear_imsi(new_emm_ctx);
    emm_ctx_clear_imei(new_emm_ctx);
    emm_ctx_clear_imeisv(new_emm_ctx);
    emm_ctx_clear_lvr_tai(new_emm_ctx);
    emm_ctx_clear_security(new_emm_ctx);
    emm_ctx_clear_non_current_security(new_emm_ctx);
    emm_ctx_clear_auth_vectors(new_emm_ctx);
    emm_ctx_clear_ms_nw_cap(new_emm_ctx);
    emm_ctx_clear_ue_nw_cap_ie(new_emm_ctx);
    emm_ctx_clear_current_drx_parameter(new_emm_ctx);
    emm_ctx_clear_pending_current_drx_parameter(new_emm_ctx);
    emm_ctx_clear_eps_bearer_context_status(new_emm_ctx);

    /*
     * Initialize EMM timers
     */
    new_emm_ctx->ue_id = ue_id; 
    if (RETURNok != emm_data_context_add (&_emm_data, new_emm_ctx)) {
      OAILOG_CRITICAL(LOG_NAS_EMM, "EMM-PROC  - Attach EMM Context could not be inserted in hastables\n");
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
    }

    if (last_visited_registered_tai) {
      emm_ctx_set_valid_lvr_tai(new_emm_ctx, last_visited_registered_tai);
    } else {
      emm_ctx_clear_lvr_tai(new_emm_ctx);
    }

    /*
     * Update the EMM context with the current attach procedure parameters
     */
    rc = _emm_attach_update (new_emm_ctx, ue_id, type, ksi, is_native_guti, guti, imsi, imei, originating_tai, eea, eia, ucs2, uea, uia, gea, umts_present, gprs_present, esm_msg_pP);

    if (rc != RETURNok) {
      OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Failed to update EMM context\n");
      /*
       * Do not accept the UE to attach to the network
       */
      new_emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
      rc = _emm_attach_reject (new_emm_ctx);
    } else {
      /*
       * Performs the sequence: UE identification, authentication, security mode
       */
      rc = _emm_attach_identify (new_emm_ctx);
    }
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:        emm_proc_attach_reject()                                  **
 **                                                                        **
 ** Description: Performs the protocol error abnormal case                 **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.1.2.7, case b                 **
 **              If the ATTACH REQUEST message is received with a protocol **
 **              error, the network shall return an ATTACH REJECT message. **
 **                                                                        **
 ** Inputs:  ue_id:              UE lower layer identifier                  **
 **                  emm_cause: EMM cause code to be reported              **
 **                  Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **                  Return:    RETURNok, RETURNerror                      **
 **                  Others:    _emm_data                                  **
 **                                                                        **
 ***************************************************************************/
int
emm_proc_attach_reject (
  mme_ue_s1ap_id_t ue_id,
  int emm_cause)
{
  int                                     rc = RETURNerror;
  OAILOG_FUNC_IN (LOG_NAS_EMM);

  if (ue_id == INVALID_MME_UE_S1AP_ID) {
    // Create temporary UE context
    emm_data_context_t                      ue_ctx;
    memset (&ue_ctx, 0, sizeof (emm_data_context_t));
    ue_ctx.is_dynamic = false;
    ue_ctx.ue_id = ue_id;
    ue_ctx.emm_cause = EMM_CAUSE_ILLEGAL_UE;
    rc = _emm_attach_reject (&ue_ctx);
  } else {
    emm_data_context_t * emm_ctx_p = emm_data_context_get (&_emm_data, ue_id);
    emm_ctx_p->emm_cause = emm_cause;
    rc = _emm_attach_reject (emm_ctx_p);
 }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_attach_complete()                                **
 **                                                                        **
 ** Description: Terminates the attach procedure upon receiving Attach     **
 **      Complete message from the UE.                             **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.1.2.4                         **
 **      Upon receiving an ATTACH COMPLETE message, the MME shall  **
 **      stop timer T3450, enter state EMM-REGISTERED and consider **
 **      the GUTI sent in the ATTACH ACCEPT message as valid.      **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      esm_msg_pP:   Activate default EPS bearer context accept **
 **             ESM message                                **
 **      Others:    _emm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _emm_data, T3450                           **
 **                                                                        **
 ***************************************************************************/
int
emm_proc_attach_complete (
  mme_ue_s1ap_id_t ue_id,
  const_bstring esm_msg_pP)
{
  emm_data_context_t                     *emm_ctx = NULL;
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};
  esm_sap_t                               esm_sap = {0};

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - EPS attach complete (ue_id=" MME_UE_S1AP_ID_FMT ")\n", ue_id);
  REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__20);
  /*
   * Release retransmission timer parameters
   */
  emm_proc_common_clear_args(ue_id);

  /*
   * Get the UE context
   */
  emm_ctx = emm_data_context_get (&_emm_data, ue_id);

  if (emm_ctx) {
    /*
     * Upon receiving an ATTACH COMPLETE message, the MME shall stop timer T3450
     */
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3450 (%d)\n", emm_ctx->T3450.id);
    emm_ctx->T3450.id = nas_timer_stop (emm_ctx->T3450.id);
    MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3450 stopped UE " MME_UE_S1AP_ID_FMT " ", ue_id);
    /*
     * Upon receiving an ATTACH COMPLETE message, the MME shall enter state EMM-REGISTERED
     * and consider the GUTI sent in the ATTACH ACCEPT message as valid.
     */
    emm_ctx_set_attribute_valid(emm_ctx, EMM_CTXT_MEMBER_GUTI);
    emm_data_context_add_guti(&_emm_data, emm_ctx);
    emm_ctx_clear_old_guti(emm_ctx);

    /*
     * Forward the Activate Default EPS Bearer Context Accept message
     * to the EPS session management sublayer
     */
    esm_sap.primitive = ESM_DEFAULT_EPS_BEARER_CONTEXT_ACTIVATE_CNF;
    esm_sap.is_standalone = false;
    esm_sap.ue_id = ue_id;
    esm_sap.recv = esm_msg_pP;
    esm_sap.ctx = emm_ctx;
    rc = esm_sap_send (&esm_sap);
  } else {
    OAILOG_ERROR (LOG_NAS_EMM, "EMM-PROC  - No EMM context exists\n");
  }

  if ((rc != RETURNerror) && (esm_sap.err == ESM_SAP_SUCCESS)) {
    /*
     * Set the network attachment indicator
     */
    emm_ctx->is_attached = true;
    emm_ctx->is_has_been_attached = true;
    /*
     * Notify EMM that attach procedure has successfully completed
     */
    emm_sap.primitive = EMMREG_ATTACH_CNF;
    emm_sap.u.emm_reg.ue_id = ue_id;
    emm_sap.u.emm_reg.ctx = emm_ctx;
    rc = emm_sap_send (&emm_sap);
  } else if (esm_sap.err != ESM_SAP_DISCARDED) {
    /*
     * Notify EMM that attach procedure failed
     */
    emm_sap.primitive = EMMREG_ATTACH_REJ;
    emm_sap.u.emm_reg.ue_id = ue_id;
    emm_sap.u.emm_reg.ctx = emm_ctx;
    rc = emm_sap_send (&emm_sap);
  } else {
    /*
     * ESM procedure failed and, received message has been discarded or
     * Status message has been returned; ignore ESM procedure failure
     */
    rc = RETURNok;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}


/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/



/*
 * --------------------------------------------------------------------------
 * Timer handlers
 * --------------------------------------------------------------------------
 */

/*
 *
 * Name:    _emm_attach_t3450_handler()
 *
 * Description: T3450 timeout handler
 *
 *              3GPP TS 24.301, section 5.5.1.2.7, case c
 *      On the first expiry of the timer T3450, the network shall
 *      retransmit the ATTACH ACCEPT message and shall reset and
 *      restart timer T3450. This retransmission is repeated four
 *      times, i.e. on the fifth expiry of timer T3450, the at-
 *      tach procedure shall be aborted and the MME enters state
 *      EMM-DEREGISTERED.
 *
 * Inputs:  args:      handler parameters
 *      Others:    None
 *
 * Outputs:     None
 *      Return:    None
 *      Others:    None
 *
 */
static void                            *
_emm_attach_t3450_handler (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  attach_data_t                          *data = (attach_data_t *) (args);

  /*
   * Increment the retransmission counter
   */
  data->retransmission_count += 1;
  OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - T3450 timer expired, retransmission " "counter = %d\n", data->retransmission_count);
  /*
   * Get the UE's EMM context
   */
  emm_data_context_t                     *emm_ctx = NULL;

  emm_ctx = emm_data_context_get (&_emm_data, data->ue_id);

  if (data->retransmission_count < ATTACH_COUNTER_MAX) {
    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_c__1);
    /*
     * On the first expiry of the timer, the network shall retransmit the ATTACH ACCEPT message and shall reset and
     * restart timer T3450.
     */
    _emm_attach_accept_retx (emm_ctx, data);
  } else {
    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_c__2);
    /*
     * Abort the attach procedure
     */
    _emm_attach_abort (data);
  }
  // TODO REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_c__3) not coded

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, NULL);
}

/*
 * --------------------------------------------------------------------------
 * Abnormal cases in the MME
 * --------------------------------------------------------------------------
 */

/*
 *
 * Name:    _emm_attach_release()
 *
 * Description: Releases the UE context data.
 *
 * Inputs:  args:      Data to be released
 *      Others:    None
 *
 * Outputs:     None
 *      Return:    None
 *      Others:    None
 *
 */
static int
_emm_attach_release (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_data_context_t                     *emm_ctx = (emm_data_context_t *) (args);

  if (emm_ctx) {
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Release UE context data (ue_id=" MME_UE_S1AP_ID_FMT ")\n", emm_ctx->ue_id);
    unsigned int                            ue_id = emm_ctx->ue_id;

    /*
     * Notify EMM that the attach procedure is aborted
     */
    emm_sap_t                               emm_sap = {0};


    emm_sap.primitive = EMMREG_PROC_ABORT;
    emm_sap.u.emm_reg.ue_id = ue_id;
    emm_sap.u.emm_reg.ctx = emm_ctx;
    rc = emm_sap_send (&emm_sap);
    
    /*
     * Release the EMM context
     */
    _clear_emm_ctxt(emm_ctx);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/*
 *
 * Name:    _emm_attach_reject()
 *
 * Description: Performs the attach procedure not accepted by the network.
 *
 *              3GPP TS 24.301, section 5.5.1.2.5
 *      If the attach request cannot be accepted by the network,
 *      the MME shall send an ATTACH REJECT message to the UE in-
 *      including an appropriate EMM cause value.
 *
 * Inputs:  args:      UE context data
 *      Others:    None
 *
 * Outputs:     None
 *      Return:    RETURNok, RETURNerror
 *      Others:    None
 *
 */
static int
_emm_attach_reject (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_data_context_t                     *emm_ctx = (emm_data_context_t *) (args);
  attach_data_t                          *data_p = NULL;

  if (emm_ctx) {
    emm_sap_t                               emm_sap = {0};
    data_p = (attach_data_t *) emm_proc_common_get_args (emm_ctx->ue_id);
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - EMM attach procedure not accepted " "by the network (ue_id=" MME_UE_S1AP_ID_FMT ", cause=%d)\n", emm_ctx->ue_id, emm_ctx->emm_cause);
    /*
     * Notify EMM-AS SAP that Attach Reject message has to be sent
     * onto the network
     */
    emm_sap.primitive = EMMAS_ESTABLISH_REJ;
    emm_sap.u.emm_as.u.establish.ue_id = emm_ctx->ue_id;
    emm_sap.u.emm_as.u.establish.eps_id.guti = NULL;

    emm_sap.u.emm_as.u.establish.emm_cause = emm_ctx->emm_cause;
    emm_sap.u.emm_as.u.establish.nas_info = EMM_AS_NAS_INFO_ATTACH;

    if (emm_ctx->emm_cause != EMM_CAUSE_ESM_FAILURE) {
      emm_sap.u.emm_as.u.establish.nas_msg = NULL;
    } else if (emm_ctx->esm_msg) {
      emm_sap.u.emm_as.u.establish.nas_msg = data_p->esm_msg;
    } else {
      OAILOG_ERROR (LOG_NAS_EMM, "EMM-PROC  - ESM message is missing\n");
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
    }

    /*
     * Setup EPS NAS security data
     */
    emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, &emm_ctx->_security, false, false);
    rc = emm_sap_send (&emm_sap);

    /*
     * Release the UE context, even if the network failed to send the
     * ATTACH REJECT message
     */
    if (emm_ctx->is_dynamic) {
      rc = _emm_attach_release (emm_ctx);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/*
 *
 * Name:    _emm_attach_abort()
 *
 * Description: Aborts the attach procedure
 *
 * Inputs:  args:      Attach procedure data to be released
 *      Others:    None
 *
 * Outputs:     None
 *      Return:    RETURNok, RETURNerror
 *      Others:    T3450
 *
 */
static int
_emm_attach_abort (
  void *args)
{
  int                                     rc = RETURNerror;
  emm_data_context_t                     *ctx = NULL;
  attach_data_t                          *data = NULL;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  data = (attach_data_t *) (args);

  if (data) {
    unsigned int                            ue_id = data->ue_id;

    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Abort the attach procedure (ue_id=" MME_UE_S1AP_ID_FMT ")\n", ue_id);
    ctx = emm_data_context_get (&_emm_data, ue_id);

    if (ctx) {
      /*
       * Stop timer T3450
       */
      if (ctx->T3450.id != NAS_TIMER_INACTIVE_ID) {
        OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3450 (%d)\n", ctx->T3450.id);
        ctx->T3450.id = nas_timer_stop (ctx->T3450.id);
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3450 stopped UE " MME_UE_S1AP_ID_FMT " ", data->ue_id);
      }
      /*
       * Release retransmission timer parameters
       */
      bdestroy (data->esm_msg);
      free_wrapper ((void**) &data);
      // Trigger clean up
      emm_sap_t                               emm_sap = {0};
      emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
      emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = ctx->ue_id;
      rc = emm_sap_send (&emm_sap);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/*
 * --------------------------------------------------------------------------
 * Functions that may initiate EMM common procedures
 * --------------------------------------------------------------------------
 */

/*
 * Name:    _emm_attach_identify()
 *
 * Description: Performs UE's identification. May initiates identification, authentication and security mode control EMM common procedures.
 *
 * Inputs:  args:      Identification argument parameters
 *      Others:    None
 *
 * Outputs:     None
 *      Return:    RETURNok, RETURNerror
 *      Others:    _emm_data
 *
 */
static int
_emm_attach_identify (
  void *args)
{
  int                                     rc = RETURNerror;
  emm_data_context_t                     *emm_ctx = (emm_data_context_t *) (args);

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  REQUIREMENT_3GPP_24_301(R10_5_5_1_2_3__1);
  OAILOG_INFO (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Identify incoming UE using %s\n",
      emm_ctx->ue_id,
      IS_EMM_CTXT_VALID_IMSI(emm_ctx)   ? "IMSI" :
      IS_EMM_CTXT_PRESENT_GUTI(emm_ctx) ? "GUTI" :
      IS_EMM_CTXT_VALID_IMEI(emm_ctx)   ? "IMEI" : "none");

  /*
   * UE's identification
   * -------------------
   */
  if (IS_EMM_CTXT_PRESENT_IMSI(emm_ctx)) {
    // The UE identifies itself using an IMSI
    if (!IS_EMM_CTXT_PRESENT_AUTH_VECTORS(emm_ctx)) {
      // Ask upper layer to fetch new security context
      nas_itti_auth_info_req (emm_ctx->ue_id, emm_ctx->_imsi64, true, &emm_ctx->originating_tai.plmn,
                              MAX_EPS_AUTH_VECTORS, NULL);
      rc = RETURNok;
    } else {
      ksi_t                                   eksi = 0;
      int                                     vindex = 0;

      if (emm_ctx->_security.eksi != KSI_NO_KEY_AVAILABLE) {
        REQUIREMENT_3GPP_24_301(R10_5_4_2_4__2);
        eksi = (emm_ctx->_security.eksi + 1) % (EKSI_MAX_VALUE + 1);
      }
      for (vindex = 0; vindex < MAX_EPS_AUTH_VECTORS; vindex++) {
        if (IS_EMM_CTXT_PRESENT_AUTH_VECTOR(emm_ctx, vindex)) {
          break;
        }
      }
      // eksi should always be 0
      /*if (!IS_EMM_CTXT_PRESENT_AUTH_VECTORS(emm_ctx)) {
        // Ask upper layer to fetch new security context
        nas_itti_auth_info_req (emm_ctx->ue_id, emm_ctx->_imsi64, true, &emm_ctx->originating_tai.plmn, MAX_EPS_AUTH_VECTORS, NULL);
        rc = RETURNok;
      } else */{
        emm_ctx_set_security_vector_index(emm_ctx, vindex);
        rc = emm_proc_authentication (emm_ctx, emm_ctx->ue_id, eksi,
          emm_ctx->_vector[vindex].rand, emm_ctx->_vector[vindex].autn, emm_attach_security, NULL, NULL);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
      }
    }
  } else if (IS_EMM_CTXT_PRESENT_GUTI(emm_ctx)) {
    // The UE identifies itself using a GUTI
    //LG Force identification here
    emm_ctx_clear_attribute_valid(emm_ctx, EMM_CTXT_MEMBER_AUTH_VECTORS);
    OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Failed to identify the UE using provided GUTI (tmsi=%u)\n", emm_ctx->ue_id, emm_ctx->_guti.m_tmsi);
    /*
     * 3GPP TS 24.401, Figure 5.3.2.1-1, point 4
     * The UE was attempting to attach to the network using a GUTI
     * that is not known by the network; the MME shall initiate an
     * identification procedure to retrieve the IMSI from the UE.
     */
    rc = emm_proc_identification (emm_ctx->ue_id, emm_ctx, EMM_IDENT_TYPE_IMSI, _emm_attach_identify, _emm_attach_release, _emm_attach_release);

    if (rc != RETURNok) {
      // Failed to initiate the identification procedure
      OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT "EMM-PROC  - Failed to initiate identification procedure\n", emm_ctx->ue_id);
      emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
      // Do not accept the UE to attach to the network
      rc = _emm_attach_reject (emm_ctx);
    }

    // Relevant callback will be executed when identification
    // procedure completes
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);

  } else if (emm_ctx->is_guti_based_attach) {

    OAILOG_NOTICE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - GUTI based Attach Request \n", emm_ctx->ue_id);
    /*
     * 3GPP TS 24.401, Figure 5.3.2.1-1, point 4
     * The UE was attempting to attach to the network using a GUTI
     * that is not known by the network; the MME shall initiate an
     * identification procedure to retrieve the IMSI from the UE.
     */
    rc = emm_proc_identification (emm_ctx->ue_id, emm_ctx, EMM_IDENT_TYPE_IMSI, _emm_attach_identify, _emm_attach_release, _emm_attach_release);

    if (rc != RETURNok) {
      // Failed to initiate the identification procedure
      OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT "EMM-PROC  - Failed to initiate identification procedure\n", emm_ctx->ue_id);
      emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
      // Do not accept the UE to attach to the network
      rc = _emm_attach_reject (emm_ctx);
    }

    // Relevant callback will be executed when identification
    // procedure completes
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  } else if ((IS_EMM_CTXT_VALID_IMEI(emm_ctx)) && (emm_ctx->is_emergency)) {
    /*
     * The UE is attempting to attach to the network for emergency
     * services using an IMEI
     */
     AssertFatal(0 != 0, "TODO emergency services...");
    if (rc != RETURNok) {
      emm_ctx_clear_auth_vectors(emm_ctx);
      OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - " "Failed to identify the UE using provided IMEI\n", emm_ctx->ue_id);
      emm_ctx->emm_cause = EMM_CAUSE_IMEI_NOT_ACCEPTED;
    }
  } else {
    OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - UE's identity is not available\n", emm_ctx->ue_id);
    emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
  }

  /*
   * UE's authentication
   * -------------------
   */
  if (rc != RETURNerror) {
    if (IS_EMM_CTXT_VALID_SECURITY(emm_ctx)) {
      /*
       * A security context exists for the UE in the network;
       * proceed with the attach procedure.
       */
      rc = _emm_attach (emm_ctx);
    } else if ((emm_ctx->is_emergency) && (_emm_data.conf.features & MME_API_UNAUTHENTICATED_IMSI)) {
      /*
       * 3GPP TS 24.301, section 5.5.1.2.3
       * 3GPP TS 24.401, Figure 5.3.2.1-1, point 5a
       * MME configured to support Emergency Attach for unauthenticated
       * IMSIs may choose to skip the authentication procedure even if
       * no EPS security context is available and proceed directly to the
       * execution of the security mode control procedure.
       */
      rc = _emm_attach_security (emm_ctx);
    }
  }

  if (rc != RETURNok) {
    // Do not accept the UE to attach to the network
    rc = _emm_attach_reject (emm_ctx);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:        _emm_attach_security()                                    **
 **                                                                        **
 ** Description: Initiates security mode control EMM common procedure.     **
 **                                                                        **
 ** Inputs:          args:      security argument parameters               **
 **                  Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **                  Return:    RETURNok, RETURNerror                      **
 **                  Others:    _emm_data                                  **
 **                                                                        **
 ***************************************************************************/
int
emm_attach_security (
  void *args)
{
  return _emm_attach_security (args);
}

static int
_emm_attach_security (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_data_context_t                     *emm_ctx = (emm_data_context_t *) (args);

  REQUIREMENT_3GPP_24_301(R10_5_5_1_2_3__1);
  OAILOG_INFO (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Setup NAS security\n", emm_ctx->ue_id);

  /*
   * Create new NAS security context
   */
  emm_ctx_clear_security(emm_ctx);

  /*
   * Initialize the security mode control procedure
   */
  rc = emm_proc_security_mode_control (emm_ctx->ue_id, emm_ctx->ue_ksi,
                                       emm_ctx->eea, emm_ctx->eia, emm_ctx->ucs2, emm_ctx->uea,
                                       emm_ctx->uia, emm_ctx->gea, emm_ctx->umts_present, emm_ctx->gprs_present,
                                       _emm_attach, _emm_attach_release, _emm_attach_release);

  if (rc != RETURNok) {
    /*
     * Failed to initiate the security mode control procedure
     */
    OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT "EMM-PROC  - Failed to initiate security mode control procedure\n", emm_ctx->ue_id);
    emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
    /*
     * Do not accept the UE to attach to the network
     */
    rc = _emm_attach_reject (emm_ctx);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/*
   --------------------------------------------------------------------------
                MME specific local functions
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_attach()                                             **
 **                                                                        **
 ** Description: Performs the attach signalling procedure while a context  **
 **      exists for the incoming UE in the network.                **
 **                                                                        **
 **              3GPP TS 24.301, section 5.5.1.2.4                         **
 **      Upon receiving the ATTACH REQUEST message, the MME shall  **
 **      send an ATTACH ACCEPT message to the UE and start timer   **
 **      T3450.                                                    **
 **                                                                        **
 ** Inputs:  args:      attach argument parameters                 **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _emm_data                                  **
 **                                                                        **
 ***************************************************************************/
static int
_emm_attach (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  esm_sap_t                               esm_sap = {0};
  int                                     rc = RETURNerror;
  emm_data_context_t                     *emm_ctx = (emm_data_context_t *) (args);

  OAILOG_INFO (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Attach UE \n", emm_ctx->ue_id);
  /*
   * 3GPP TS 24.401, Figure 5.3.2.1-1, point 5a
   * At this point, all NAS messages shall be protected by the NAS security
   * functions (integrity and ciphering) indicated by the MME unless the UE
   * is emergency attached and not successfully authenticated.
   */
  /*
   * Notify ESM that PDN connectivity is requested
   */
  esm_sap.primitive = ESM_PDN_CONNECTIVITY_REQ;
  esm_sap.is_standalone = false;
  esm_sap.ue_id = emm_ctx->ue_id;
  esm_sap.ctx = emm_ctx;
  esm_sap.recv = emm_ctx->esm_msg;
  rc = esm_sap_send (&esm_sap);

  if ((rc != RETURNerror) && (esm_sap.err == ESM_SAP_SUCCESS)) {
    /*
     * The attach request is accepted by the network
     */
    /*
     * Delete the stored UE radio capability information, if any
     */
    /*
     * Store the UE network capability
     */
    /*
     * Assign the TAI list the UE is registered to
     */
    /*
     * Allocate parameters of the retransmission timer callback
     */
    attach_data_t                          *data = (attach_data_t *) calloc (1, sizeof (attach_data_t));

    if (data) {
      /*
       * Setup ongoing EMM procedure callback functions
       */
      rc = emm_proc_common_initialize (emm_ctx->ue_id, NULL, NULL, NULL, NULL, NULL, _emm_attach_abort, data);

      if (rc != RETURNok) {
        OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " Failed to initialize EMM callback functions\n", emm_ctx->ue_id);
        free_wrapper ((void**) &data);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
      }

      /*
       * Set the UE identifier
       */
      data->ue_id = emm_ctx->ue_id;
      /*
       * Reset the retransmission counter
       */
      data->retransmission_count = 0;
#if ORIGINAL_CODE
      /*
       * Setup the ESM message container
       */
      data->esm_msg.value = (uint8_t *) malloc (esm_sap.send.length);

      if (data->esm_msg.value) {
        data->esm_msg.length = esm_sap.send.length;
        memcpy (data->esm_msg.value, esm_sap.send.value, esm_sap.send.length);
      } else {
        data->esm_msg.length = 0;
      }

      /*
       * Send attach accept message to the UE
       */
      rc = _emm_attach_accept (emm_ctx, data);

      if (rc != RETURNerror) {
        if (emm_ctx->guti_is_new && emm_ctx->old_guti) {
          /*
           * Implicit GUTI reallocation;
           * Notify EMM that common procedure has been initiated
           */
          emm_sap_t                               emm_sap = {0};

          emm_sap.primitive = EMMREG_COMMON_PROC_REQ;
          emm_sap.u.emm_reg.ue_id = data->ue_id;
          rc = emm_sap_send (&emm_sap);
        }
      }
#else
      rc = RETURNok;
#endif
    }
  } else if (esm_sap.err != ESM_SAP_DISCARDED) {
    /*
     * The attach procedure failed due to an ESM procedure failure
     */
    emm_ctx->emm_cause = EMM_CAUSE_ESM_FAILURE;

    /*
     * Setup the ESM message container to include PDN Connectivity Reject
     * message within the Attach Reject message
     */
    bdestroy(emm_ctx->esm_msg);
    emm_ctx->esm_msg = esm_sap.send;
    rc = _emm_attach_reject (emm_ctx);
  } else {
    /*
     * ESM procedure failed and, received message has been discarded or
     * Status message has been returned; ignore ESM procedure failure
     */
    rc = RETURNok;
  }

  if (rc != RETURNok) {
    /*
     * The attach procedure failed
     */
    OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Failed to respond to Attach Request\n", emm_ctx->ue_id);
    emm_ctx->emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
    /*
     * Do not accept the UE to attach to the network
     */
    rc = _emm_attach_reject (emm_ctx);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

int
emm_cn_wrapper_attach_accept (
  emm_data_context_t * emm_ctx,
  void *data)
{
  return _emm_attach_accept (emm_ctx, (attach_data_t *) data);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_attach_accept()                                      **
 **                                                                        **
 ** Description: Sends ATTACH ACCEPT message and start timer T3450         **
 **                                                                        **
 ** Inputs:  data:      Attach accept retransmission data          **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3450                                      **
 **                                                                        **
 ***************************************************************************/
static int
_emm_attach_accept (
  emm_data_context_t * emm_ctx,
  attach_data_t * data)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     i = 0;
  int                                     rc = RETURNerror;


  // may be caused by timer not stopped when deleted context
  if (emm_ctx) {
    /*
     * Notify EMM-AS SAP that Attach Accept message together with an Activate
     * Default EPS Bearer Context Request message has to be sent to the UE
     */
    emm_sap.primitive = EMMAS_ESTABLISH_CNF;
    emm_sap.u.emm_as.u.establish.ue_id = emm_ctx->ue_id;
    emm_sap.u.emm_as.u.establish.nas_info = EMM_AS_NAS_INFO_ATTACH;

    NO_REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__3);
    //----------------------------------------
    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__4);
    emm_ctx_set_attribute_valid(emm_ctx, EMM_CTXT_MEMBER_UE_NETWORK_CAPABILITY_IE);
    emm_ctx_set_attribute_valid(emm_ctx, EMM_CTXT_MEMBER_MS_NETWORK_CAPABILITY_IE);
    //----------------------------------------
    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__5);
    emm_ctx_set_valid_current_drx_parameter(emm_ctx, &emm_ctx->_pending_drx_parameter);
    emm_ctx_clear_pending_current_drx_parameter(emm_ctx);
    //----------------------------------------
    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__9);
    // the set of emm_sap.u.emm_as.u.establish.new_guti is for including the GUTI in the attach accept message
    //ONLY ONE MME NOW NO S10
    if (!IS_EMM_CTXT_PRESENT_GUTI(emm_ctx)) {
      // Sure it is an unknown GUTI in this MME
      guti_t old_guti = emm_ctx->_old_guti;
      guti_t guti     = {.gummei.plmn = {0},
                         .gummei.mme_gid = 0,
                         .gummei.mme_code = 0,
                         .m_tmsi = INVALID_M_TMSI};
      clear_guti(&guti);

      rc = mme_api_new_guti (&emm_ctx->_imsi, &old_guti, &guti, &emm_ctx->originating_tai, &emm_ctx->_tai_list);
      if ( RETURNok == rc) {
        emm_ctx_set_guti(emm_ctx, &guti);
        emm_ctx_set_attribute_valid(emm_ctx, EMM_CTXT_MEMBER_TAI_LIST);
        //----------------------------------------
        REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__6);
        REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__10);
        emm_sap.u.emm_as.u.establish.tai_list.list_type = emm_ctx->_tai_list.list_type;
        emm_sap.u.emm_as.u.establish.tai_list.n_tais    = emm_ctx->_tai_list.n_tais;
        for (i=0; i < emm_ctx->_tai_list.n_tais; i++) {
          memcpy(&emm_sap.u.emm_as.u.establish.tai_list.tai[i], &emm_ctx->_tai_list.tai[i], sizeof(tai_t));
        }
      } else {
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
      }
    } else {
      // Set the TAI attributes from the stored context for resends.
      emm_sap.u.emm_as.u.establish.tai_list.list_type = emm_ctx->_tai_list.list_type;
      emm_sap.u.emm_as.u.establish.tai_list.n_tais    = emm_ctx->_tai_list.n_tais;
      for (i=0; i < emm_ctx->_tai_list.n_tais; i++) {
        memcpy(&emm_sap.u.emm_as.u.establish.tai_list.tai[i], &emm_ctx->_tai_list.tai[i], sizeof(tai_t));
      }
    }

    emm_sap.u.emm_as.u.establish.eps_id.guti = &emm_ctx->_guti;

    if (!IS_EMM_CTXT_VALID_GUTI(emm_ctx) &&
         IS_EMM_CTXT_PRESENT_GUTI(emm_ctx) &&
         IS_EMM_CTXT_PRESENT_OLD_GUTI(emm_ctx)) {
      /*
       * Implicit GUTI reallocation;
       * include the new assigned GUTI in the Attach Accept message
       */
      OAILOG_INFO (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Implicit GUTI reallocation, include the new assigned GUTI in the Attach Accept message\n",
          emm_ctx->ue_id);
      emm_sap.u.emm_as.u.establish.new_guti    = &emm_ctx->_guti;
    } else if (!IS_EMM_CTXT_VALID_GUTI(emm_ctx) &&
        IS_EMM_CTXT_PRESENT_GUTI(emm_ctx)) {
      /*
       * include the new assigned GUTI in the Attach Accept message
       */
      OAILOG_INFO (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Include the new assigned GUTI in the Attach Accept message\n", emm_ctx->ue_id);
      emm_sap.u.emm_as.u.establish.new_guti    = &emm_ctx->_guti;
    } else { // IS_EMM_CTXT_VALID_GUTI(emm_ctx) is true
      emm_sap.u.emm_as.u.establish.new_guti  = NULL;
    }
    //----------------------------------------
    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__14);
    emm_sap.u.emm_as.u.establish.eps_network_feature_support = &_emm_data.conf.eps_network_feature_support;

    /*
     * Delete any preexisting UE radio capabilities, pursuant to
     * GPP 24.310:5.5.1.2.4
     */
    ue_context_t *ue_context_p =
      mme_ue_context_exists_mme_ue_s1ap_id(&mme_app_desc.mme_ue_contexts,
                                           emm_ctx->ue_id);

    OAILOG_DEBUG (LOG_NAS_EMM,
                 "UE context already exists: %s\n",
                 ue_context_p ? "yes" : "no");
    if (ue_context_p) {
      // Note: this is safe from double-free errors because it sets to NULL
      // after freeing, which free treats as a no-op.
      free_wrapper((void**) &ue_context_p->ue_radio_capabilities);
      ue_context_p->ue_radio_cap_length = 0;  // Logically "deletes" info
    }
    /*
     * Setup EPS NAS security data
     */
    emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, &emm_ctx->_security, false, true);
    emm_sap.u.emm_as.u.establish.encryption = emm_ctx->_security.selected_algorithms.encryption;
    emm_sap.u.emm_as.u.establish.integrity = emm_ctx->_security.selected_algorithms.integrity;
    OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - encryption = 0x%X (0x%X)\n",
        emm_ctx->ue_id, emm_sap.u.emm_as.u.establish.encryption, emm_ctx->_security.selected_algorithms.encryption);
    OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - integrity  = 0x%X (0x%X)\n",
        emm_ctx->ue_id, emm_sap.u.emm_as.u.establish.integrity, emm_ctx->_security.selected_algorithms.integrity);
    /*
     * Get the activate default EPS bearer context request message to
     * transfer within the ESM container of the attach accept message
     */
    emm_sap.u.emm_as.u.establish.nas_msg = data->esm_msg;
    OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - nas_msg  src size = %d nas_msg  dst size = %d \n",
        emm_ctx->ue_id, blength(data->esm_msg), blength(emm_sap.u.emm_as.u.establish.nas_msg));

    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_4__2);
    rc = emm_sap_send (&emm_sap);

    if (RETURNerror != rc) {
      if (emm_ctx->T3450.id != NAS_TIMER_INACTIVE_ID) {
        /*
         * Re-start T3450 timer
         */
        emm_ctx->T3450.id = nas_timer_restart (emm_ctx->T3450.id);
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3450 restarted UE " MME_UE_S1AP_ID_FMT "", data->ue_id);
      } else {
        /*
         * Start T3450 timer
         */
        emm_ctx->T3450.id = nas_timer_start (emm_ctx->T3450.sec, _emm_attach_t3450_handler, data);
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3450 started UE " MME_UE_S1AP_ID_FMT " ", data->ue_id);
      }

      OAILOG_INFO (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "Timer T3450 (%d) expires in %ld seconds\n",
          emm_ctx->ue_id, emm_ctx->T3450.id, emm_ctx->T3450.sec);
    }
  } else {
    OAILOG_WARNING (LOG_NAS_EMM, "emm_ctx NULL\n");
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
/****************************************************************************
 **                                                                        **
 ** Name:    _emm_attach_accept_retx()                                     **
 **                                                                        **
 ** Description: Retransmit ATTACH ACCEPT message and restart timer T3450  **
 **                                                                        **
 ** Inputs:  data:      Attach accept retransmission data                  **
 **      Others:    None                                                   **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                                  **
 **      Others:    T3450                                                  **
 **                                                                        **
 ***************************************************************************/
static int
_emm_attach_accept_retx (
  emm_data_context_t * emm_ctx,
  attach_data_t * data)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     i = 0;
  int                                     rc = RETURNerror;
  if (!emm_ctx) {
    OAILOG_WARNING (LOG_NAS_EMM, "emm_ctx NULL\n");
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  if (!IS_EMM_CTXT_PRESENT_GUTI(emm_ctx)) {
    OAILOG_WARNING (LOG_NAS_EMM, " No GUTI present in emm_ctx. Abormal case. Skipping Retx of Attach Accept NULL\n");
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  /*
   * Notify EMM-AS SAP that Attach Accept message together with an Activate
   * Default EPS Bearer Context Request message has to be sent to the UE.
   * Retx of Attach Accept needs to be done via DL NAS Transport S1AP message
   */
  emm_sap.primitive = EMMAS_DATA_REQ;
  emm_sap.u.emm_as.u.data.ue_id = emm_ctx->ue_id;
  emm_sap.u.emm_as.u.data.nas_info = EMM_AS_NAS_DATA_ATTACH_ACCEPT;
  emm_sap.u.emm_as.u.data.tai_list.list_type = emm_ctx->_tai_list.list_type;
  emm_sap.u.emm_as.u.data.tai_list.n_tais    = emm_ctx->_tai_list.n_tais;
  for (i = 0; i < emm_ctx->_tai_list.n_tais; i++) {
    memcpy(&emm_sap.u.emm_as.u.data.tai_list.tai[i], &emm_ctx->_tai_list.tai[i], sizeof(tai_t));
  }
  emm_sap.u.emm_as.u.data.eps_id.guti = &emm_ctx->_guti;
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Include the same GUTI in the Attach Accept Retx message\n", emm_ctx->ue_id);
  emm_sap.u.emm_as.u.data.new_guti    = &emm_ctx->_guti;
  emm_sap.u.emm_as.u.data.eps_network_feature_support = &_emm_data.conf.eps_network_feature_support;
  /*
   * Setup EPS NAS security data
   */
  emm_as_set_security_data (&emm_sap.u.emm_as.u.data.sctx, &emm_ctx->_security, false, true);
  emm_sap.u.emm_as.u.data.encryption = emm_ctx->_security.selected_algorithms.encryption;
  emm_sap.u.emm_as.u.data.integrity = emm_ctx->_security.selected_algorithms.integrity;
  /*
   * Get the activate default EPS bearer context request message to
   * transfer within the ESM container of the attach accept message
   */
  emm_sap.u.emm_as.u.data.nas_msg = data->esm_msg;
  OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - nas_msg  src size = %d nas_msg  dst size = %d \n",
    emm_ctx->ue_id, blength(data->esm_msg), blength(emm_sap.u.emm_as.u.data.nas_msg));

  rc = emm_sap_send (&emm_sap);

  if (RETURNerror != rc) {
    OAILOG_INFO (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  -Sent Retx Attach Accept message\n", emm_ctx->ue_id);
    /*
     * Re-start T3450 timer
     */
    emm_ctx->T3450.id = nas_timer_restart (emm_ctx->T3450.id);
    OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  T3450 restarted\n", emm_ctx->ue_id);
    OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "Timer T3450 (%d) expires in %ld seconds\n",
      emm_ctx->ue_id, emm_ctx->T3450.id, emm_ctx->T3450.sec);
  } else {
    OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Send failed- Retx Attach Accept message\n", emm_ctx->ue_id);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_attach_have_changed()                                **
 **                                                                        **
 ** Description: Check whether the given attach parameters differs from    **
 **      those previously stored when the attach procedure has     **
 **      been initiated.                                           **
 **                                                                        **
 ** Inputs:  ctx:       EMM context of the UE in the network       **
 **      type:      Type of the requested attach               **
 **      ksi:       Security ket sey identifier                **
 **      guti:      The GUTI provided by the UE                **
 **      imsi:      The IMSI provided by the UE                **
 **      imei:      The IMEI provided by the UE                **
 **      eea:       Supported EPS encryption algorithms        **
 **      eia:       Supported EPS integrity algorithms         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    true if at least one of the parameters     **
 **             differs; false otherwise.                  **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int
_emm_attach_have_changed (
  const emm_data_context_t * ctx,
  emm_proc_attach_type_t type,
  ksi_t ksi,
  guti_t * guti,
  imsi_t * imsi,
  imei_t * imei,
  int eea,
  int eia,
  int ucs2,
  int uea,
  int uia,
  int gea,
  int umts_present,
  int gprs_present)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);

  /*
   * Emergency bearer services indicator
   */
  if ((type == EMM_ATTACH_TYPE_EMERGENCY) != ctx->is_emergency) {
    OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: type EMM_ATTACH_TYPE_EMERGENCY \n", ctx->ue_id);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  /*
   * Security key set identifier
   */
  if (ksi != ctx->ue_ksi) {
    OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: ue_ksi %d -> %d \n", ctx->ue_id, ctx->ue_ksi, ksi);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  /*
   * Supported EPS encryption algorithms
   */
  if (eea != ctx->eea) {
    OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: eea 0x%x -> 0x%x \n", ctx->ue_id, ctx->eea, eea);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  /*
   * Supported EPS integrity algorithms
   */
  if (eia != ctx->eia) {
    OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: eia 0x%x -> 0x%x \n", ctx->ue_id, ctx->eia, eia);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if (umts_present != ctx->umts_present) {
    OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: umts_present %d -> %d \n", ctx->ue_id, ctx->umts_present, umts_present);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((ctx->umts_present) && (umts_present)) {
    if (ucs2 != ctx->ucs2) {
      OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: ucs2 %u -> %u \n", ctx->ue_id, ctx->ucs2, ucs2);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }

    /*
     * Supported UMTS encryption algorithms
     */
    if (uea != ctx->uea) {
      OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: uea %u -> %u \n", ctx->ue_id, ctx->uea, uea);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }

    /*
     * Supported UMTS integrity algorithms
     */
    if (uia != ctx->uia) {
      OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: uia %u -> %u \n", ctx->ue_id, ctx->uia, uia);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
  }

  if (gprs_present != ctx->gprs_present) {
    OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: gprs_present %u -> %u \n", ctx->ue_id, ctx->gprs_present, gprs_present);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((ctx->gprs_present) && (gprs_present)) {
    if (gea != ctx->gea) {
      OAILOG_DEBUG (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed: gea 0x%X -> 0x%X \n", ctx->ue_id, ctx->gea, gea);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
  }

  /*
   * The GUTI if provided by the UE
   */
  if ((guti) && (!IS_EMM_CTXT_PRESENT_OLD_GUTI(ctx))) {
    OAILOG_INFO (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed:  guti None ->  " GUTI_FMT "\n", ctx->ue_id, GUTI_ARG(guti));
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((!guti) && (IS_EMM_CTXT_PRESENT_GUTI(ctx))) {
    OAILOG_INFO (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed:  guti " GUTI_FMT " -> None\n", ctx->ue_id, GUTI_ARG(guti));
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((guti) && (IS_EMM_CTXT_PRESENT_GUTI(ctx))) {
    if (guti->m_tmsi != ctx->_guti.m_tmsi) {
      OAILOG_INFO (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed:  guti/tmsi " GUTI_FMT " -> " GUTI_FMT "\n", ctx->ue_id, GUTI_ARG(&ctx->_guti), GUTI_ARG(guti));
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
    if ((guti->gummei.mme_code != ctx->_guti.gummei.mme_code) ||
        (guti->gummei.mme_gid != ctx->_guti.gummei.mme_gid) ||
        (guti->gummei.plmn.mcc_digit1 != ctx->_guti.gummei.plmn.mcc_digit1) ||
        (guti->gummei.plmn.mcc_digit2 != ctx->_guti.gummei.plmn.mcc_digit2) ||
        (guti->gummei.plmn.mcc_digit3 != ctx->_guti.gummei.plmn.mcc_digit3) ||
        (guti->gummei.plmn.mnc_digit1 != ctx->_guti.gummei.plmn.mnc_digit1) ||
        (guti->gummei.plmn.mnc_digit2 != ctx->_guti.gummei.plmn.mnc_digit2) ||
        (guti->gummei.plmn.mnc_digit3 != ctx->_guti.gummei.plmn.mnc_digit3)) {
      OAILOG_INFO (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "  attach changed:  guti/tmsi " GUTI_FMT " -> " GUTI_FMT "\n", ctx->ue_id, GUTI_ARG(&ctx->_guti), GUTI_ARG(guti));
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
  }

  /*
   * The IMSI if provided by the UE
   */
  if ((imsi) && (!IS_EMM_CTXT_VALID_IMSI(ctx))) {
    char                                    imsi_str[16];

    IMSI_TO_STRING (imsi, imsi_str, 16);
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_attach_have_changed: imsi %s/NULL (ctxt)\n", imsi_str);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((!imsi) && (IS_EMM_CTXT_VALID_IMSI(ctx))) {
    char                                    imsi_str[16];

    IMSI_TO_STRING (&ctx->_imsi, imsi_str, 16);
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_attach_have_changed: imsi NULL/%s (ctxt)\n", imsi_str);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((imsi) && (IS_EMM_CTXT_VALID_IMSI(ctx))) {
    if (memcmp (imsi, &ctx->_imsi, sizeof (ctx->_imsi)) != 0) {
      char                                    imsi_str[16];
      char                                    imsi2_str[16];

      IMSI_TO_STRING (imsi, imsi_str, 16);
      IMSI_TO_STRING (&ctx->_imsi, imsi2_str, 16);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_attach_have_changed: imsi %s/%s (ctxt)\n", imsi_str, imsi2_str);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
  }

  /*
   * The IMEI if provided by the UE
   */
  if ((imei) && (!IS_EMM_CTXT_VALID_IMEI(ctx))) {
    char                                    imei_str[16];

    IMEI_TO_STRING (imei, imei_str, 16);
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_attach_have_changed: imei %s/NULL (ctxt)\n", imei_str);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((!imei) && (IS_EMM_CTXT_VALID_IMEI(ctx))) {
    char                                    imei_str[16];

    IMEI_TO_STRING (&ctx->_imei, imei_str, 16);
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_attach_have_changed: imei NULL/%s (ctxt)\n", imei_str);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((imei) && (IS_EMM_CTXT_VALID_IMEI(ctx))) {
    if (memcmp (imei, &ctx->_imei, sizeof (ctx->_imei)) != 0) {
      char                                    imei_str[16];
      char                                    imei2_str[16];

      IMEI_TO_STRING (imei, imei_str, 16);
      IMEI_TO_STRING (&ctx->_imei, imei2_str, 16);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_attach_have_changed: imei %s/%s (ctxt)\n", imei_str, imei2_str);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, false);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_attach_update()                                      **
 **                                                                        **
 ** Description: Update the EMM context with the given attach procedure    **
 **      parameters.                                               **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      type:      Type of the requested attach               **
 **      ksi:       Security ket sey identifier                **
 **      guti:      The GUTI provided by the UE                **
 **      imsi:      The IMSI provided by the UE                **
 **      imei:      The IMEI provided by the UE                **
 **      eea:       Supported EPS encryption algorithms        **
 **      originating_tai Originating TAI (from eNB TAI)        **
 **      eia:       Supported EPS integrity algorithms         **
 **      esm_msg_pP:   ESM message contained with the attach re-  **
 **             quest                                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     ctx:       EMM context of the UE in the network       **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int
_emm_attach_update (
  emm_data_context_t * ctx,
  mme_ue_s1ap_id_t ue_id,
  emm_proc_attach_type_t type,
  ksi_t ksi,
  bool is_native_guti,
  guti_t * guti,
  imsi_t * imsi,
  imei_t * imei,
  const tai_t   * const originating_tai,
  const int eea,
  const int eia,
  const int ucs2,
  const int uea,
  const int uia,
  const int gea,
  const bool umts_present,
  const bool gprs_present,
  const_bstring esm_msg_pP)
{

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  /*
   * UE identifier
   */
  ctx->ue_id = ue_id;
  /*
   * Emergency bearer services indicator
   */
  ctx->is_emergency = (type == EMM_ATTACH_TYPE_EMERGENCY);
  /*
   * Security key set identifier
   */
  ctx->ue_ksi = ksi;
  /*
   * Supported EPS encryption algorithms
   */
  ctx->eea = eea;
  /*
   * Supported EPS integrity algorithms
   */
  ctx->eia = eia;
  ctx->ucs2 = ucs2;
  ctx->uea = uea;
  ctx->uia = uia;
  ctx->gea = gea;
  ctx->umts_present = umts_present;
  ctx->gprs_present = gprs_present;

  ctx->originating_tai = *originating_tai;
  ctx->is_guti_based_attach = false;

  /*
   * The GUTI if provided by the UE. Trigger UE Identity Procedure to fetch IMSI
   */
  if (guti) {
   ctx->is_guti_based_attach = true;
  }
  /*
   * The IMSI if provided by the UE
   */
  if (imsi) {
    imsi64_t new_imsi64 = INVALID_IMSI64;
    IMSI_TO_IMSI64(imsi,new_imsi64);
    if (new_imsi64 != ctx->_imsi64) {
      emm_ctx_set_valid_imsi(ctx, imsi, new_imsi64);
      emm_data_context_add_imsi (&_emm_data, ctx);
    }
  }

  /*
   * The IMEI if provided by the UE
   */
  if (imei) {
    emm_ctx_set_valid_imei(ctx, imei);
  }

  /*
   * The ESM message contained within the attach request
   */
  if (esm_msg_pP) {
    bdestroy(ctx->esm_msg);
    if (!(ctx->esm_msg = bstrcpy(esm_msg_pP))) {
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
    }
  }
  /*
   * Attachment indicator
   */
  ctx->is_attached = false;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
}


