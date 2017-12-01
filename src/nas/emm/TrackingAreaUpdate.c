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
  Source      TrackingAreaUpdate.c

  Version     0.1

  Date        2013/05/07

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines the tracking area update EMM procedure executed by the
        Non-Access Stratum.

        The tracking area updating procedure is always initiated by the
        UE and is used to update the registration of the actual tracking
        area of a UE in the network, to periodically notify the availa-
        bility of the UE to the network, for MME load balancing, to up-
        date certain UE specific parameters in the network.

*****************************************************************************/
#include <string.h>             // memcmp, memcpy
#include <stdlib.h>             // malloc, free_wrapper

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "log.h"
#include "msc.h"
#include "nas_timer.h"
#include "3gpp_requirements_24.301.h"
#include "emm_proc.h"
#include "emm_sap.h"
#include "mme_app_defs.h"
#include "emm_cause.h"


/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/*
   --------------------------------------------------------------------------
     Internal data handled by the tracking area update procedure in the MME
   --------------------------------------------------------------------------
*/
/*
   Internal data used for attach procedure
*/
typedef struct tau_accept_data_s {
  unsigned int                            ue_id; /* UE identifier        */
  unsigned int                            active_flag; /* active flag IE in TAU Request  */
#define TAU_COUNTER_MAX  5
  unsigned int                            retransmission_count; /* Retransmission counter   */

} tau_accept_data_t;

int emm_network_capability_have_changed (
  const emm_data_context_t * ctx,
  int eea,
  int eia,
  int ucs2,
  int uea,
  int uia,
  int gea,
  int umts_present,
  int gprs_present);

static int emm_proc_tracking_area_update_accept (const mme_ue_s1ap_id_t ue_id, const int active_flag);

/* TODO Commented some function declarations below since these were called from the code that got removed from TAU request
 * handling function. Reason this code was removed: This portion of code was incomplete and was related to handling of
 * some optional IEs /scenarios that were not relevant for the TAU periodic update handling and might have resulted in 
 * unexpected behaviour/instability.
 * At present support for TAU is limited to handling of perodic TAU request only  mandatoty IEs . 
 * Other aspects of TAU are TODOs for future.
 */

//static int _emm_tracking_area_update (void *args);
//static int _emm_tracking_area_update_security (void *args);
static int _emm_tracking_area_update_reject (mme_ue_s1ap_id_t ue_id, int emm_cause);
static int _emm_tracking_area_update_accept (emm_data_context_t * emm_ctx,tau_accept_data_t * data);
static int _emm_tracking_area_update_abort (void *args);
//static int _emm_tracking_area_update_reject_cb(void *args) ;


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/
int
emm_network_capability_have_changed (
  const emm_data_context_t * ctx,
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
   * Supported EPS encryption algorithms
   */
  if (eea != ctx->eea) {
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: eea 0x%x/0x%x (ctxt)", eea, ctx->eea);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  /*
   * Supported EPS integrity algorithms
   */
  if (eia != ctx->eia) {
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: eia 0x%x/0x%x (ctxt)", eia, ctx->eia);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if (umts_present != ctx->umts_present) {
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: umts_present %u/%u (ctxt)", umts_present, ctx->umts_present);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((ctx->umts_present) && (umts_present)) {
    if (ucs2 != ctx->ucs2) {
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: ucs2 %u/%u (ctxt)", ucs2, ctx->ucs2);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }

    /*
     * Supported UMTS encryption algorithms
     */
    if (uea != ctx->uea) {
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: uea 0x%x/0x%x (ctxt)", uea, ctx->uea);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }

    /*
     * Supported UMTS integrity algorithms
     */
    if (uia != ctx->uia) {
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: uia 0x%x/0x%x (ctxt)", uia, ctx->uia);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
  }

  if (gprs_present != ctx->gprs_present) {
    OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: gprs_present %u/%u (ctxt)", gprs_present, ctx->gprs_present);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
  }

  if ((ctx->gprs_present) && (gprs_present)) {
    if (gea != ctx->gea) {
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  _emm_network_capability_have_changed: gea 0x%x/0x%x (ctxt)", gea, ctx->gea);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, true);
    }
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, false);
}


int
emm_proc_tracking_area_update_request (
  const mme_ue_s1ap_id_t ue_id,
  const tracking_area_update_request_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t  * decode_status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_data_context_t                     *ue_ctx = NULL;
  *emm_cause = EMM_CAUSE_SUCCESS;
  uint8_t  active_flag = 0;
  /*
   * Get the UE's EMM context if it exists
   */

  ue_ctx = emm_data_context_get (&_emm_data, ue_id);
  
  if (! ue_ctx) {
    if (EPS_MOBILE_IDENTITY_GUTI == msg->oldguti.guti.typeofidentity) {
      guti_t guti;
      guti.m_tmsi = msg->oldguti.guti.mtmsi;
      guti.gummei.mme_gid = msg->oldguti.guti.mmegroupid;
      guti.gummei.mme_code = msg->oldguti.guti.mmecode;
      guti.gummei.plmn.mcc_digit1 = msg->oldguti.guti.mccdigit1;
      guti.gummei.plmn.mcc_digit2 = msg->oldguti.guti.mccdigit2;
      guti.gummei.plmn.mcc_digit3 = msg->oldguti.guti.mccdigit3;
      guti.gummei.plmn.mnc_digit1 = msg->oldguti.guti.mncdigit1;
      guti.gummei.plmn.mnc_digit2 = msg->oldguti.guti.mncdigit2;
      guti.gummei.plmn.mnc_digit3 = msg->oldguti.guti.mncdigit3;

      ue_ctx = emm_data_context_get_by_guti (&_emm_data, &guti);

      if ( ue_ctx) {
        //TODO
        // ...anyway now for simplicity we reject the TAU (else have to re-do integrity checking on NAS msg)
        rc = emm_proc_tracking_area_update_reject (ue_id, EMM_CAUSE_IMPLICITLY_DETACHED);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
      } else {
        // NO S10
        rc = emm_proc_tracking_area_update_reject (ue_id, EMM_CAUSE_IMPLICITLY_DETACHED);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
      }
    }
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  OAILOG_DEBUG(LOG_NAS_EMM, "EMM-PROC-  Tracking Area Update request. TAU_Type=%d, active_flag=%d)\n",
        msg->epsupdatetype.epsupdatetypevalue, msg->epsupdatetype.activeflag);
  // Check if it is not periodic update.
  if ( EPS_UPDATE_TYPE_PERIODIC_UPDATING != msg->epsupdatetype.epsupdatetypevalue) {
    /*
     * MME24.301R10_5.5.3.2.4_6 Normal and periodic tracking area updating procedure accepted by the network UE - EPS update type
     * If the EPS update type IE included in the TRACKING AREA UPDATE REQUEST message indicates "periodic updating", and the UE was
     * previously successfully attached for EPS and non-EPS services, subject to operator policies the MME should allocate a TAI
     * list that does not span more than one location area.
     */
    // This IE not implemented
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC- Sending Tracking Area Update Reject. ue_id=" MME_UE_S1AP_ID_FMT ", cause=%d)\n",
        ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    rc = emm_proc_tracking_area_update_reject (ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  #if 0
  /*
   * Requirements MME24.301R10_5.5.3.2.4_2
   */
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_UE_NETWORK_CAPABILITY_IEI) {
    emm_ctx_set_ue_nw_cap_ie((emm_data_context_t * const)ue_ctx, &msg->uenetworkcapability);
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_MS_NETWORK_CAPABILITY_IEI) {
    emm_ctx_set_attribute_present(ue_ctx, EMM_CTXT_MEMBER_MS_NETWORK_CAPABILITY_IE);
    ue_ctx->_ms_network_capability_ie = msg->msnetworkcapability;
    ue_ctx->gea = (msg->msnetworkcapability.gea1 << 6)| msg->msnetworkcapability.egea;
    ue_ctx->gprs_present = true;
  }
  #endif 
  /*
   * Requirements MME24.301R10_5.5.3.2.4_3
   */
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_UE_RADIO_CAPABILITY_INFORMATION_UPDATE_NEEDED_IEI) {
    if (0 != msg->ueradiocapabilityinformationupdateneeded) {
      ue_context_t *ue_context_p =
        mme_ue_context_exists_mme_ue_s1ap_id(&mme_app_desc.mme_ue_contexts,
                                               ue_id);

      OAILOG_DEBUG (LOG_NAS_EMM, "UE context exists: %s\n", ue_context_p ? "yes" : "no");
      if (ue_context_p) {
        // Note: this is safe from double-free errors because it sets to NULL
        // after freeing, which free treats as a no-op.

        free_wrapper((void**) &ue_context_p->ue_radio_capabilities);
        ue_context_p->ue_radio_cap_length = 0;  // Logically "deletes" info
      }
    }
  }
  #if 0
  /*
   * Requirements MME24.301R10_5.5.3.2.4_4
   */
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_DRX_PARAMETER_IEI) {
    emm_ctx_set_current_drx_parameter((emm_data_context_t * const)ue_ctx, &msg->drxparameter);
  }
  #endif
  /*
   * Requirement MME24.301R10_5.5.3.2.4_5a
   */
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_EPS_BEARER_CONTEXT_STATUS_IEI) {
    // This IE is not implemented
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC- Sending Tracking Area Update Reject.EPS Bearer Context Status IE not supported. ue_id=" MME_UE_S1AP_ID_FMT ", cause=%d)\n",
        ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    rc = emm_proc_tracking_area_update_reject (ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
    // emm_ctx_set_eps_bearer_context_status(ue_ctx, &msg->epsbearercontextstatus);
    //#pragma message  "TODO Requirement MME24.301R10_5.5.3.2.4_5a: TAU Request: Deactivate EPS bearers if necessary (S11 Modify Bearer Request)"
  }
  /*
   * Requirement MME24.301R10_5.5.3.2.4_6
   */
  if ( EPS_UPDATE_TYPE_PERIODIC_UPDATING == msg->epsupdatetype.epsupdatetypevalue) {
    /*
     * MME24.301R10_5.5.3.2.4_6 Normal and periodic tracking area updating procedure accepted by the network UE - EPS update type
     * If the EPS update type IE included in the TRACKING AREA UPDATE REQUEST message indicates "periodic updating", and the UE was
     * previously successfully attached for EPS and non-EPS services, subject to operator policies the MME should allocate a TAI
     * list that does not span more than one location area.
     */
    active_flag = msg->epsupdatetype.activeflag;
    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC- Sending Tracking Area Update Accept. ue_id=" MME_UE_S1AP_ID_FMT ", active flag=%d)\n",
        ue_id, active_flag);
    // Handle periodic TAU
    rc = emm_proc_tracking_area_update_accept (ue_id, active_flag);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
/****************************************************************************
 **                                                                        **
 ** Name:        emm_proc_tracking_area_update_reject()                    **
 **                                                                        **
 ** Description:                                                           **
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
int emm_proc_tracking_area_update_reject (
  const mme_ue_s1ap_id_t ue_id,
  const int emm_cause)
{
  int                                     rc = RETURNerror;
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  rc = _emm_tracking_area_update_reject (ue_id, emm_cause);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
static int emm_proc_tracking_area_update_accept (
  const mme_ue_s1ap_id_t ue_id, const int active_flag)
{
  int                                     rc = RETURNerror;
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_data_context_t                     *emm_ctx = emm_data_context_get (&_emm_data, ue_id);
  DevAssert(emm_ctx != NULL);
  if (!emm_ctx) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMM-PROC  -No EMM context for ue_id %u\n", ue_id);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  tau_accept_data_t                      *data = (tau_accept_data_t *) calloc (1, sizeof (tau_accept_data_t));
  DevAssert(data != NULL);
  if (!data) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMM-PROC  - Memory allocation failure while processing TAU Request. ue_id %u\n", ue_id);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  data->active_flag =  active_flag;
  rc = _emm_tracking_area_update_accept (emm_ctx, data);
  // Free TAU data as at present new GUTI is not sent in TAU Accept and hence no response is expected from ue. 
  free_wrapper ((void**) &data); 
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/* TODO - Compiled out this function to remove compiler warnings since we don't expect TAU Complete from UE as we dont support implicit 
 * GUTI re-allocation during TAU procedure.
 */
#if 0
static int
_emm_tracking_area_update (
    void *args)
{
  int                                     rc = RETURNerror;
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_data_context_t                     *emm_ctx = (emm_data_context_t *) (args);
  tau_accept_data_t                      *data = (tau_accept_data_t *) calloc (1, sizeof (tau_accept_data_t));

  if (data ) {
    /*
     * Setup ongoing EMM procedure callback functions
     */
    rc = emm_proc_common_initialize (emm_ctx->ue_id, NULL, NULL, NULL, NULL, NULL, _emm_tracking_area_update_abort, data);

    if (rc != RETURNok) {
      OAILOG_WARNING (LOG_NAS_EMM, "Failed to initialize EMM callback functions");
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
    rc = RETURNok;

    // Send TAU accept to the UE
    rc = _emm_tracking_area_update_accept (emm_ctx, data);
  } else {
    /*
     * The TAU procedure failed
     */
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Failed to respond to TAU Request");
    emm_ctx->emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
    rc = _emm_tracking_area_update_reject (emm_ctx->ue_id, emm_ctx->emm_cause);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
#endif
/*
 * --------------------------------------------------------------------------
 * Timer handlers
 * --------------------------------------------------------------------------
 */

/** \fn void _emm_tau_t3450_handler(void *args);
\brief T3450 timeout handler
On the first expiry of the timer, the network shall retransmit the TRACKING AREA UPDATE ACCEPT
message and shall reset and restart timer T3450. The retransmission is performed four times, i.e. on the fifth
expiry of timer T3450, the tracking area updating procedure is aborted. Both, the old and the new GUTI shall be
considered as valid until the old GUTI can be considered as invalid by the network (see subclause 5.4.1.4).
During this period the network acts as described for case a above.
@param [in]args TAU accept data
*/
//------------------------------------------------------------------------------
static void                            *
_emm_tau_t3450_handler (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  tau_accept_data_t                      *data = (tau_accept_data_t *) (args);

  // Requirement MME24.301R10_5.5.3.2.7_c Abnormal cases on the network side - T3450 time-out
  /*
   * Increment the retransmission counter
   */
  data->retransmission_count += 1;
  OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - T3450 timer expired, retransmission counter = %d", data->retransmission_count);
  /*
   * Get the UE's EMM context
   */
  emm_data_context_t *emm_ctx = emm_ctx = emm_data_context_get (&_emm_data, data->ue_id);

  if (data->retransmission_count < TAU_COUNTER_MAX) {
    /*
     * Send attach accept message to the UE
     */
    _emm_tracking_area_update_accept (emm_ctx, data);
  } else {
    /*
     * Abort the attach procedure
     */
    _emm_tracking_area_update_abort (data);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, NULL);
}

/* TODO - Compiled out this function to remove compiler warnings since we don't support reauthetication and change in
 * security context during periodic TAU procedure.
  */
#if 0
/** \fn void _emm_tracking_area_update_security(void *args);
    \brief Performs the tracking area update procedure not accepted by the network.
     @param [in]args UE EMM context data
     @returns status of operation
*/
//------------------------------------------------------------------------------
static int
_emm_tracking_area_update_security (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_data_context_t                     *emm_ctx = (emm_data_context_t *) (args);

  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Setup NAS security (ue_id=" MME_UE_S1AP_ID_FMT ")", emm_ctx->ue_id);

  /*
   * Create new NAS security context
   */

  emm_ctx_clear_security(emm_ctx);

  /*
   * Initialize the security mode control procedure
   */
  rc = emm_proc_security_mode_control (emm_ctx->ue_id, 0,        // TODO: eksi != 0
                                       emm_ctx->eea, emm_ctx->eia, emm_ctx->ucs2,
                                       emm_ctx->uea, emm_ctx->uia, emm_ctx->gea,
                                       emm_ctx->umts_present, emm_ctx->gprs_present,
                                       _emm_tracking_area_update,
                                       _emm_tracking_area_update_reject_cb,
                                       _emm_tracking_area_update_reject_cb);

  if (rc != RETURNok) {
    /*
     * Failed to initiate the security mode control procedure
     */
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Failed to initiate security mode control procedure");
    emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
    /*
     * Do not accept the UE to attach to the network
     */
    rc = _emm_tracking_area_update_reject (emm_ctx->ue_id,emm_ctx->emm_cause);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
#endif

/** \fn  _emm_tracking_area_update_reject();
    \brief Performs the tracking area update procedure not accepted by the network.
     @param [in]args UE EMM context data
     @returns status of operation
*/
//------------------------------------------------------------------------------
static int
_emm_tracking_area_update_reject (mme_ue_s1ap_id_t ue_id, int emm_cause) 
  
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;
  emm_data_context_t                     *emm_ctx = emm_data_context_get (&_emm_data, ue_id);
  emm_sap_t                               emm_sap = {0};

  OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC- Sending Tracking Area Update Reject. ue_id=" MME_UE_S1AP_ID_FMT ", cause=%d)\n",
        ue_id, emm_cause);
  /*
   * Notify EMM-AS SAP that Tracking Area Update Reject message has to be sent
   * onto the network
   */
  emm_sap.primitive = EMMAS_ESTABLISH_REJ;
  emm_sap.u.emm_as.u.establish.ue_id = ue_id;
  emm_sap.u.emm_as.u.establish.eps_id.guti = NULL;

  emm_sap.u.emm_as.u.establish.emm_cause = emm_cause;
  emm_sap.u.emm_as.u.establish.nas_info = EMM_AS_NAS_INFO_TAU;
  emm_sap.u.emm_as.u.establish.nas_msg = NULL;
  /*
   * Setup EPS NAS security data
   */
  if (emm_ctx) {
     emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, &emm_ctx->_security, false, false);
  } else {
      emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, NULL, false, false);
  }
  rc = emm_sap_send (&emm_sap);

  // Release EMM context 
  if (emm_ctx) {
    if(emm_ctx->is_dynamic) {
      _clear_emm_ctxt(emm_ctx); 
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
/* TODO Compiled out since it was called in function _emm_tracking_area_update_security that itself is compiled out
 */
#if 0
static int
_emm_tracking_area_update_reject_cb(void *args) {
  int rc = RETURNerror;
  emm_data_context_t *emm_ctx = (emm_data_context_t *) (args);
  if (emm_ctx) {
    return _emm_tracking_area_update_reject(emm_ctx->ue_id, emm_ctx->emm_cause);
  }
  return rc;
}
#endif
/** \fn void _emm_tracking_area_update_accept (emm_data_context_t * emm_ctx,tau_accept_data_t * data);
    \brief Sends ATTACH ACCEPT message and start timer T3450.
     @param [in]emm_ctx UE EMM context data
     @param [in]data    UE TAU accept data
     @returns status of operation (RETURNok, RETURNerror)
*/
//------------------------------------------------------------------------------
static int
_emm_tracking_area_update_accept (
  emm_data_context_t * emm_ctx,
  tau_accept_data_t * data)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;
  emm_sap_t                               emm_sap = {0};

  if (emm_ctx) {

   if (data->active_flag) {
      /* If active flag is set to true in TAU request then re-establish bearer also for the UE while sending TAU
       * Accept message
       */
      emm_sap.primitive = EMMAS_ESTABLISH_CNF;
      emm_sap.u.emm_as.u.establish.ue_id = emm_ctx->ue_id;

      emm_sap.u.emm_as.u.establish.eps_update_result = EPS_UPDATE_RESULT_TA_UPDATED;
      emm_sap.u.emm_as.u.establish.eps_id.guti = &emm_ctx->_guti;
      emm_sap.u.emm_as.u.establish.new_guti = NULL;

      emm_sap.u.emm_as.u.establish.tai_list.list_type = emm_ctx->_tai_list.list_type;
      emm_sap.u.emm_as.u.establish.tai_list.n_tais    = 0;
      emm_sap.u.emm_as.u.establish.nas_info = EMM_AS_NAS_INFO_TAU;

      // TODO Reminder
      emm_sap.u.emm_as.u.establish.eps_bearer_context_status = NULL;
      emm_sap.u.emm_as.u.establish.location_area_identification = NULL;
      emm_sap.u.emm_as.u.establish.combined_tau_emm_cause = NULL;


      emm_sap.u.emm_as.u.establish.t3423 = NULL;
      emm_sap.u.emm_as.u.establish.t3412 = NULL;
      emm_sap.u.emm_as.u.establish.t3402 = NULL;
      // TODO Reminder
      emm_sap.u.emm_as.u.establish.equivalent_plmns = NULL;
      emm_sap.u.emm_as.u.establish.emergency_number_list = NULL;

      emm_sap.u.emm_as.u.establish.eps_network_feature_support = NULL;
      emm_sap.u.emm_as.u.establish.additional_update_result = NULL;
      emm_sap.u.emm_as.u.establish.t3412_extended = NULL;
      emm_sap.u.emm_as.u.establish.nas_msg = NULL; // No ESM container message in TAU Accept message 


      /*
       * Setup EPS NAS security data
       */
      emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, &emm_ctx->_security, false, true);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - encryption = 0x%X ", emm_sap.u.emm_as.u.establish.encryption);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - integrity  = 0x%X ", emm_sap.u.emm_as.u.establish.integrity);
      emm_sap.u.emm_as.u.establish.encryption = emm_ctx->_security.selected_algorithms.encryption;
      emm_sap.u.emm_as.u.establish.integrity = emm_ctx->_security.selected_algorithms.integrity;
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - encryption = 0x%X (0x%X)", emm_sap.u.emm_as.u.establish.encryption, emm_ctx->_security.selected_algorithms.encryption);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - integrity  = 0x%X (0x%X)", emm_sap.u.emm_as.u.establish.integrity, emm_ctx->_security.selected_algorithms.integrity);
    
      rc = emm_sap_send (&emm_sap);

      if (rc != RETURNerror) {
        if (emm_sap.u.emm_as.u.establish.new_guti != NULL) {

          if (emm_ctx->T3450.id != NAS_TIMER_INACTIVE_ID) {
            /*
             * Re-start T3450 timer
             */
            emm_ctx->T3450.id = nas_timer_restart (emm_ctx->T3450.id);
            MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3450 restarted UE " MME_UE_S1AP_ID_FMT " (TAU)", data->ue_id);
          } else {
            /*
             * Start T3450 timer
             */
            emm_ctx->T3450.id = nas_timer_start (emm_ctx->T3450.sec, _emm_tau_t3450_handler, data);
            MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3450 started UE " MME_UE_S1AP_ID_FMT " (TAU)", data->ue_id);
          }

          OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Timer T3450 (%d) expires in %ld seconds (TAU)", emm_ctx->T3450.id, emm_ctx->T3450.sec);
        }
      }
    } // Active Flag
    else {

      /* If active flag is not set to true in TAU request then just send TAU accept. After sending TAU accept initiate
       * S1 context release procedure for the UE if new GUTI is not sent in TAU accept message. Note - At present implicit GUTI
       * reallocation is not supported and hence GUTI is not sent in TAU accept message.
       */
      emm_as_data_t                          *emm_as = &emm_sap.u.emm_as.u.data;

      /*
       * Setup NAS information message to transfer
       */
      emm_as->nas_info = EMM_AS_NAS_DATA_TAU;
      emm_as->nas_msg = NULL; // No ESM container
      /*
       * Set the UE identifier
       */
      emm_as->ue_id = emm_ctx->ue_id;

      /*
       * Setup EPS NAS security data
       */
      emm_as_set_security_data (&emm_as->sctx, &emm_ctx->_security, false, true);
      /*
       * Notify EMM-AS SAP that TAU Accept message has to be sent to the network
       */
      emm_sap.primitive = EMMAS_DATA_REQ;
      rc = emm_sap_send (&emm_sap);
    }
  } else {
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - emm_ctx NULL");
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int
_emm_tracking_area_update_abort (
  void *args)
{
  int                                     rc = RETURNerror;
  emm_data_context_t                     *ctx = NULL;
  tau_accept_data_t                      *data;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  data = (tau_accept_data_t *) (args);

  if (data) {
    unsigned int                            ue_id = data->ue_id;

    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Abort the TAU procedure (ue_id=" MME_UE_S1AP_ID_FMT ")", ue_id);
    ctx = emm_data_context_get (&_emm_data, ue_id);

    if (ctx) {
      /*
       * Stop timer T3450
       */
      if (ctx->T3450.id != NAS_TIMER_INACTIVE_ID) {
        OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3450 (%d)", ctx->T3450.id);
        ctx->T3450.id = nas_timer_stop (ctx->T3450.id);
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3450 stopped UE " MME_UE_S1AP_ID_FMT " (TAU)", data->ue_id);
      }
    }

    /*
     * Release retransmission timer parameters
     */
    // no contained struct to free
    free_wrapper ((void**) &data);

    /*
     * Notify EMM that EPS attach procedure failed
     */
    emm_sap_t                               emm_sap = {0};

    emm_sap.primitive = EMMREG_ATTACH_REJ;
    emm_sap.u.emm_reg.ue_id = ue_id;
    emm_sap.u.emm_reg.ctx = ctx;
    rc = emm_sap_send (&emm_sap);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
