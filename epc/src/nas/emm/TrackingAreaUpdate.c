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
#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "log.h"
#include "msc.h"
#include "nas_timer.h"
#include "3gpp_requirements_24.301.h"
#include "common_types.h"
#include "common_defs.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"
#include "3gpp_29.274.h"
#include "mme_app_ue_context.h"
#include "emm_proc.h"
#include "common_defs.h"
#include "emm_data.h"
#include "emm_sap.h"
#include "emm_cause.h"
#include "mme_config.h"
#include "mme_app_defs.h"


/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* TODO Commented some function declarations below since these were called from the code that got removed from TAU request
 * handling function. Reason this code was removed: This portion of code was incomplete and was related to handling of
 * some optional IEs /scenarios that were not relevant for the TAU periodic update handling and might have resulted in 
 * unexpected behaviour/instability.
 * At present support for TAU is limited to handling of periodic TAU request only  mandatory IEs .
 * Other aspects of TAU are TODOs for future.
 */

static int _emm_tracking_area_update_reject( const mme_ue_s1ap_id_t ue_id, const int emm_cause);
static int _emm_tracking_area_update_accept (nas_emm_tau_proc_t * const tau_proc);
static int _emm_tracking_area_update_abort (struct emm_context_s *emm_context, struct nas_base_proc_s * base_proc);
static void _emm_tracking_area_update_t3450_handler (void *args);


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

//------------------------------------------------------------------------------
int emm_proc_tracking_area_update_request (
  const mme_ue_s1ap_id_t ue_id,
  emm_tau_request_ies_t *ies,
  int *emm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  ue_mm_context_t                        *ue_mm_context = NULL;
  emm_context_t                          *emm_context = NULL;

  *emm_cause = EMM_CAUSE_SUCCESS;
  /*
   * Get the UE's EMM context if it exists
   */

  ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, ue_id);
  if (ue_mm_context) {
    emm_context = &ue_mm_context->emm_context;
  }

  // May be the MME APP module did not find the context, but if we have the GUTI, we may find it
  if (! ue_mm_context) {
    if (INVALID_M_TMSI != ies->old_guti.m_tmsi) {

      ue_mm_context = mme_ue_context_exists_guti (&mme_app_desc.mme_ue_contexts, &ies->old_guti);

      if ( ue_mm_context) {
        emm_context = &ue_mm_context->emm_context;
        //TODO
        // ...anyway now for simplicity we reject the TAU (else have to re-do integrity checking on NAS msg)
        rc = _emm_tracking_area_update_reject (ue_id, EMM_CAUSE_IMPLICITLY_DETACHED);
        free_emm_tau_request_ies(&ies);
        unlock_ue_contexts(ue_mm_context);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
      } else {
        // NO S10
        rc = _emm_tracking_area_update_reject (ue_id, EMM_CAUSE_IMPLICITLY_DETACHED);
        free_emm_tau_request_ies(&ies);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
      }
    }
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  OAILOG_DEBUG(LOG_NAS_EMM, "EMM-PROC-  Tracking Area Update request. TAU_Type=%d, active_flag=%d)\n",
      ies->eps_update_type.eps_update_type_value, ies->eps_update_type.active_flag);
  // Check if it is not periodic update.
  if ( EPS_UPDATE_TYPE_PERIODIC_UPDATING != ies->eps_update_type.eps_update_type_value) {
    /*
     * MME24.301R10_5.5.3.2.4_6 Normal and periodic tracking area updating procedure accepted by the network UE - EPS update type
     * If the EPS update type IE included in the TRACKING AREA UPDATE REQUEST message indicates "periodic updating", and the UE was
     * previously successfully attached for EPS and non-EPS services, subject to operator policies the MME should allocate a TAI
     * list that does not span more than one location area.
     */
    // This IE not implemented
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC- Sending Tracking Area Update Reject. ue_id=" MME_UE_S1AP_ID_FMT ", cause=%d)\n",
        ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    rc = _emm_tracking_area_update_reject (ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    free_emm_tau_request_ies(&ies);
    unlock_ue_contexts(ue_mm_context);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }

  /*
   * Requirements MME24.301R10_5.5.3.2.4_3
   */
  if (ies->is_ue_radio_capability_information_update_needed) {
    OAILOG_DEBUG (LOG_NAS_EMM, "UE context exists: %s\n", emm_context ? "yes" : "no");
    if (ue_mm_context) {
      // Note: this is safe from double-free errors because it sets to NULL
      // after freeing, which free treats as a no-op.
      bdestroy_wrapper(&ue_mm_context->ue_radio_capability);
    }
  }

  /*
   * Requirement MME24.301R10_5.5.3.2.4_5a
   */
  if (ies->eps_bearer_context_status) {
    // This IE is not implemented
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC- Sending Tracking Area Update Reject.EPS Bearer Context Status IE not supported. ue_id=" MME_UE_S1AP_ID_FMT ", cause=%d)\n",
        ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    rc = _emm_tracking_area_update_reject (ue_id, EMM_CAUSE_IE_NOT_IMPLEMENTED);
    free_emm_tau_request_ies(&ies);
    unlock_ue_contexts(ue_mm_context);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  /*
   * Requirement MME24.301R10_5.5.3.2.4_6
   */
  if ( EPS_UPDATE_TYPE_PERIODIC_UPDATING == ies->eps_update_type.eps_update_type_value) {
    /*
     * MME24.301R10_5.5.3.2.4_6 Normal and periodic tracking area updating procedure accepted by the network UE - EPS update type
     * If the EPS update type IE included in the TRACKING AREA UPDATE REQUEST message indicates "periodic updating", and the UE was
     * previously successfully attached for EPS and non-EPS services, subject to operator policies the MME should allocate a TAI
     * list that does not span more than one location area.
     */
    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC- Sending Tracking Area Update Accept. ue_id=" MME_UE_S1AP_ID_FMT ", active flag=%d)\n",
        ue_id, ies->eps_update_type.active_flag);
    // Handle periodic TAU
    nas_emm_tau_proc_t    *tau_proc = get_nas_specific_procedure_tau(emm_context);
    if (!tau_proc) {
      tau_proc = nas_new_tau_procedure(emm_context);
      if (tau_proc) {
        tau_proc->ies = ies;
        tau_proc->ue_id = ue_id;
        tau_proc->emm_spec_proc.emm_proc.base_proc.abort         = _emm_tracking_area_update_abort;
        tau_proc->emm_spec_proc.emm_proc.base_proc.time_out      = _emm_tracking_area_update_t3450_handler;
        rc = _emm_tracking_area_update_accept (tau_proc);
        unlock_ue_contexts(ue_mm_context);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
      }
    }
  }

  free_emm_tau_request_ies(&ies);
  unlock_ue_contexts(ue_mm_context);
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

/* TODO - Compiled out this function to remove compiler warnings since we don't expect TAU Complete from UE as we dont support implicit 
 * GUTI re-allocation during TAU procedure.
 */
#if 0
static int _emm_tracking_area_update (void *args)
...
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
static void _emm_tracking_area_update_t3450_handler (void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_context_t                       *emm_context = (emm_context_t *) (args);

  if (!(emm_context)) {
    OAILOG_ERROR (LOG_NAS_EMM, "T3450 timer expired No EMM context\n");
    OAILOG_FUNC_OUT (LOG_NAS_EMM);
  }
  nas_emm_tau_proc_t    *tau_proc = get_nas_specific_procedure_tau(emm_context);

  if (tau_proc){

  // Requirement MME24.301R10_5.5.3.2.7_c Abnormal cases on the network side - T3450 time-out
  /*
   * Increment the retransmission counter
   */
    tau_proc->retransmission_count += 1;
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - T3450 timer expired, retransmission counter = %d", tau_proc->retransmission_count);
    /*
     * Get the UE's EMM context
     */

    if (tau_proc->retransmission_count < TAU_COUNTER_MAX) {
      /*
       * Send attach accept message to the UE
       */
      _emm_tracking_area_update_accept (tau_proc);
    } else {
      /*
       * Abort the attach procedure
       */
      /*
       * Abort the security mode control procedure
       */
      emm_sap_t                               emm_sap = {0};
      emm_sap.primitive = EMMREG_ATTACH_ABORT;
      emm_sap.u.emm_reg.ue_id     = tau_proc->ue_id;
      emm_sap.u.emm_reg.ctx       = emm_context;
      emm_sap.u.emm_reg.notify    = true;
      emm_sap.u.emm_reg.free_proc = true;
      emm_sap.u.emm_reg.u.attach.is_emergency = false;
      emm_sap_send (&emm_sap);
    }
  }
  OAILOG_FUNC_OUT (LOG_NAS_EMM);
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
static int _emm_tracking_area_update_security (emm_context_t * emm_context)
...
#endif

/** \fn  _emm_tracking_area_update_reject();
    \brief Performs the tracking area update procedure not accepted by the network.
     @param [in]args UE EMM context data
     @returns status of operation
*/
//------------------------------------------------------------------------------
static int _emm_tracking_area_update_reject( const mme_ue_s1ap_id_t ue_id, const int emm_cause)
  
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;
  emm_sap_t                               emm_sap = {0};
  ue_mm_context_t                        *ue_mm_context = NULL;
  emm_context_t                          *emm_context = NULL;

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
  ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, ue_id);
  if (ue_mm_context) {
    emm_context = &ue_mm_context->emm_context;
  }

  if (emm_context) {
     emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, &emm_context->_security, false, false);
  } else {
      emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, NULL, false, false);
  }
  rc = emm_sap_send (&emm_sap);

  // Release EMM context 
  if (emm_context) {
    if(emm_context->is_dynamic) {
      _clear_emm_ctxt(emm_context);
    }
  }

  unlock_ue_contexts(ue_mm_context);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/** \fn void _emm_tracking_area_update_accept (emm_context_t * emm_context,tau_data_t * data);
    \brief Sends ATTACH ACCEPT message and start timer T3450.
     @param [in]emm_context UE EMM context data
     @param [in]data    UE TAU accept data
     @returns status of operation (RETURNok, RETURNerror)
*/
//------------------------------------------------------------------------------
static int _emm_tracking_area_update_accept (nas_emm_tau_proc_t * const tau_proc)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};
  ue_mm_context_t                        *ue_mm_context = NULL;
  emm_context_t                          *emm_context = NULL;

  if ((tau_proc) && (tau_proc->ies)) {

    ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, tau_proc->ue_id);
    if (ue_mm_context) {
      emm_context = &ue_mm_context->emm_context;
    } else {
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
    }

    if (tau_proc->ies->eps_update_type.active_flag) {
      /* If active flag is set to true in TAU request then re-establish bearer also for the UE while sending TAU
       * Accept message
       */
      emm_sap.primitive = EMMAS_ESTABLISH_CNF;
      emm_sap.u.emm_as.u.establish.ue_id = tau_proc->ue_id;

      emm_sap.u.emm_as.u.establish.eps_update_result = EPS_UPDATE_RESULT_TA_UPDATED;
      emm_sap.u.emm_as.u.establish.eps_id.guti = &emm_context->_guti;
      emm_sap.u.emm_as.u.establish.new_guti = NULL;

      emm_sap.u.emm_as.u.establish.tai_list.numberoflists = 0;
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
      emm_as_set_security_data (&emm_sap.u.emm_as.u.establish.sctx, &emm_context->_security, false, true);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - encryption = 0x%X\n", emm_sap.u.emm_as.u.establish.encryption);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - integrity  = 0x%X\n", emm_sap.u.emm_as.u.establish.integrity);
      emm_sap.u.emm_as.u.establish.encryption = emm_context->_security.selected_algorithms.encryption;
      emm_sap.u.emm_as.u.establish.integrity = emm_context->_security.selected_algorithms.integrity;
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - encryption = 0x%X (0x%X)\n", emm_sap.u.emm_as.u.establish.encryption, emm_context->_security.selected_algorithms.encryption);
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - integrity  = 0x%X (0x%X)\n", emm_sap.u.emm_as.u.establish.integrity, emm_context->_security.selected_algorithms.integrity);
    
      rc = emm_sap_send (&emm_sap);

      if (rc != RETURNerror) {
        if (emm_sap.u.emm_as.u.establish.new_guti != NULL) {
          /*
           * Re-start T3450 timer
           */
          void * timer_callback_arg = NULL;
          nas_stop_T3450(tau_proc->ue_id, &tau_proc->T3450, timer_callback_arg);
          nas_start_T3450 (tau_proc->ue_id, &tau_proc->T3450, tau_proc->emm_spec_proc.emm_proc.base_proc.time_out, emm_context);
          unlock_ue_contexts(ue_mm_context);
          OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
        }
      }
      nas_delete_tau_procedure(emm_context);
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
      emm_as->ue_id = tau_proc->ue_id;

      /*
       * Setup EPS NAS security data
       */
      emm_as_set_security_data (&emm_as->sctx, &emm_context->_security, false, true);
      /*
       * Notify EMM-AS SAP that TAU Accept message has to be sent to the network
       */
      emm_sap.primitive = EMMAS_DATA_REQ;
      rc = emm_sap_send (&emm_sap);

      nas_delete_tau_procedure(emm_context);
    }
  } else {
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - TAU procedure NULL");
  }
  unlock_ue_contexts(ue_mm_context);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_tracking_area_update_abort (struct emm_context_s *emm_context, struct nas_base_proc_s * base_proc)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;

  if (emm_context) {
    nas_emm_tau_proc_t    *tau_proc = get_nas_specific_procedure_tau(emm_context);

    if (tau_proc) {
      mme_ue_s1ap_id_t                        ue_id = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context)->mme_ue_s1ap_id;
      OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Abort the TAU procedure (ue_id=" MME_UE_S1AP_ID_FMT ")", ue_id);

      /*
       * Stop timer T3450
       */
      void * timer_callback_args = NULL;
      nas_stop_T3450(tau_proc->ue_id, &tau_proc->T3450, timer_callback_args);

      /*
       * Notify EMM that EPS attach procedure failed
       */
      emm_sap_t                               emm_sap = {0};

      emm_sap.primitive = EMMREG_ATTACH_REJ;
      emm_sap.u.emm_reg.ue_id = ue_id;
      emm_sap.u.emm_reg.ctx = emm_context;
      MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMREG_ATTACH_REJ ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
      rc = emm_sap_send (&emm_sap);
    }
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
void free_emm_tau_request_ies(emm_tau_request_ies_t ** const ies)
{
  if ((*ies)->additional_guti) {
    free_wrapper((void**)&((*ies)->additional_guti));
  }
  if ((*ies)->ue_network_capability) {
    free_wrapper((void**)&((*ies)->ue_network_capability));
  }
  if ((*ies)->last_visited_registered_tai) {
    free_wrapper((void**)&((*ies)->last_visited_registered_tai));
  }
  if ((*ies)->last_visited_registered_tai) {
    free_wrapper((void**)&((*ies)->last_visited_registered_tai));
  }
  if ((*ies)->drx_parameter) {
    free_wrapper((void**)&((*ies)->drx_parameter));
  }
  if ((*ies)->eps_bearer_context_status) {
    free_wrapper((void**)&((*ies)->eps_bearer_context_status));
  }
  if ((*ies)->ms_network_capability) {
    free_wrapper((void**)&((*ies)->ms_network_capability));
  }
  if ((*ies)->tmsi_status) {
    free_wrapper((void**)&((*ies)->tmsi_status));
  }
  if ((*ies)->mobile_station_classmark2) {
    free_wrapper((void**)&((*ies)->mobile_station_classmark2));
  }
  if ((*ies)->mobile_station_classmark3) {
    free_wrapper((void**)&((*ies)->mobile_station_classmark3));
  }
  if ((*ies)->supported_codecs) {
    free_wrapper((void**)&((*ies)->supported_codecs));
  }
  if ((*ies)->additional_updatetype) {
    free_wrapper((void**)&((*ies)->additional_updatetype));
  }
  if ((*ies)->old_guti_type) {
    free_wrapper((void**)&((*ies)->old_guti_type));
  }
  free_wrapper((void**)ies);
}
