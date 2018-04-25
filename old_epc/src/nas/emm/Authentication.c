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
  Source      Authentication.c

  Version     0.1

  Date        2013/03/04

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines the authentication EMM procedure executed by the
        Non-Access Stratum.

        The purpose of the EPS authentication and key agreement (AKA)
        procedure is to provide mutual authentication between the user
        and the network and to agree on a key KASME. The procedure is
        always initiated and controlled by the network. However, the
        UE can reject the EPS authentication challenge sent by the
        network.

        A partial native EPS security context is established in the
        UE and the network when an EPS authentication is successfully
        performed. The computed key material KASME is used as the
        root for the EPS integrity protection and ciphering key
        hierarchy.

*****************************************************************************/

#include <stdlib.h>             // malloc, free_wrapper
#include <string.h>             // memcpy, memcmp, memset
#include <arpa/inet.h>          // htons
#include <securityDef.h>

#include "log.h"
#include "msc.h"
#include "3gpp_requirements_24.301.h"
#include "emm_proc.h"
#include "nas_proc.h"
#include "emm_sap.h"
#include "nas_itti_messaging.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/*
   --------------------------------------------------------------------------
    Internal data handled by the authentication procedure in the UE
   --------------------------------------------------------------------------
*/

/*
   --------------------------------------------------------------------------
    Internal data handled by the authentication procedure in the MME
   --------------------------------------------------------------------------
*/
//   Timer handler
static void *_authentication_t3460_handler (void *);

// Function executed when occurs a lower layer failure
static int _authentication_ll_failure (void *args);

// Function executed when occurs a lower layer non delivered indication
static int _authentication_non_delivered (void *args);

/*
   Function executed whenever the ongoing EMM procedure that initiated
   the authentication procedure is aborted or the maximum value of the
   retransmission timer counter is exceed
*/
static int _authentication_abort (void *);

/*
   Internal data used for authentication procedure
*/
typedef struct {
  mme_ue_s1ap_id_t                        ue_id; /* UE identifier        */
#define AUTHENTICATION_COUNTER_MAX  5
  unsigned int                            retransmission_count; /* Retransmission counter   */
  ksi_t                                   ksi;  /* NAS key set identifier   */
  uint8_t                                 rand[AUTH_RAND_SIZE]; /* Random challenge number  */
  uint8_t                                 autn[AUTH_AUTN_SIZE]; /* Authentication token     */
  bool                                    notify_failure;       /* Indicates whether the identification
                                                                 * procedure failure shall be notified
                                                                 * to the ongoing EMM procedure */
} authentication_data_t;

static int                              _authentication_check_imsi_5_4_2_5__1 (void * args);
static int                              _authentication_request (authentication_data_t * data);
static int                              _authentication_reject (void* args);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/


/*
   --------------------------------------------------------------------------
        Authentication procedure executed by the MME
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_authentication()                                 **
 **                                                                        **
 ** Description: Initiates authentication procedure to establish partial   **
 **      native EPS security context in the UE and the MME.        **
 **                                                                        **
 **              3GPP TS 24.301, section 5.4.2.2                           **
 **      The network initiates the authentication procedure by     **
 **      sending an AUTHENTICATION REQUEST message to the UE and   **
 **      starting the timer T3460. The AUTHENTICATION REQUEST mes- **
 **      sage contains the parameters necessary to calculate the   **
 **      authentication response.                                  **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      ksi:       NAS key set identifier                     **
 **      rand:      Random challenge number                    **
 **      autn:      Authentication token                       **
 **      success:   Callback function executed when the authen-**
 **             tication procedure successfully completes  **
 **      reject:    Callback function executed when the authen-**
 **             tication procedure fails or is rejected    **
 **      failure:   Callback function executed whener a lower  **
 **             layer failure occured before the authenti- **
 **             cation procedure comnpletes                **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_proc_authentication (
  void *ctx,
  mme_ue_s1ap_id_t ue_id,
  ksi_t ksi,
  const uint8_t   * const rand,
  const uint8_t   * const autn,
  emm_common_success_callback_t success,
  emm_common_reject_callback_t reject,
  emm_common_ll_failure_callback_t failure)
{
  int                                     rc = RETURNerror;
  authentication_data_t                  *data = NULL;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " EMM-PROC  - Initiate authentication KSI = %d, ctx = %p\n", ue_id, ksi, ctx);
  /*
   * Allocate parameters of the retransmission timer callback
   */
  data = (authentication_data_t *) calloc (1, sizeof (authentication_data_t));

  if (data ) {
    /*
     * Set the UE identifier
     */
    data->ue_id = ue_id;
    /*
     * Reset the retransmission counter
     */
    data->retransmission_count = 0;

    /*
     * Setup ongoing EMM procedure callback functions
     */
    rc = emm_proc_common_initialize (ue_id, success, reject, failure, _authentication_ll_failure, _authentication_non_delivered, _authentication_abort, data);

    if (rc != RETURNok) {
      OAILOG_WARNING (LOG_NAS_EMM, "Failed to initialize EMM callback functions\n");
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
    }

    /*
     * Set the key set identifier
     */
    data->ksi = ksi;

    /*
     * Set the authentication random challenge number
     */
    if (rand) {
        memcpy (data->rand, rand, AUTH_RAND_SIZE);
    }

    /*
     * Set the authentication token
     */
    if (autn) {
        memcpy (data->autn, autn, AUTH_AUTN_SIZE);
    }

    /*
     * Send authentication request message to the UE
     */
    rc = _authentication_request (data);

    if (rc != RETURNerror) {
      /*
       * Notify EMM that common procedure has been initiated
       */
      MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_EMM_MME, NULL, 0, "EMMREG_COMMON_PROC_REQ ue id " MME_UE_S1AP_ID_FMT " (authentication)", ue_id);
      emm_sap_t                               emm_sap = {0};

      emm_sap.primitive = EMMREG_COMMON_PROC_REQ;
      emm_sap.u.emm_reg.ue_id = ue_id;
      emm_sap.u.emm_reg.ctx = ctx;
      rc = emm_sap_send (&emm_sap);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int emm_proc_authentication_failure (
  mme_ue_s1ap_id_t ue_id,
  int emm_cause,
  const_bstring auts)
{
  int                                     rc = RETURNerror;

  authentication_data_t                  *data = (authentication_data_t *) (emm_proc_common_get_args (ue_id));

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Authentication failure (ue_id=" MME_UE_S1AP_ID_FMT ", cause=%d)\n", ue_id, emm_cause);

  // Get the UE context
  emm_data_context_t *emm_ctx = emm_data_context_get (&_emm_data, ue_id);

  if (! emm_ctx) {
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Failed to authenticate the UE\n");
    emm_cause = EMM_CAUSE_ILLEGAL_UE;
    emm_proc_common_clear_args(ue_id);
    data = NULL;
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }


  // Stop timer T3460
  REQUIREMENT_3GPP_24_301(R10_5_4_2_4__3);
  emm_ctx->T3460.id = nas_timer_stop (emm_ctx->T3460.id);
  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3460 (%d) UE " MME_UE_S1AP_ID_FMT "\n", emm_ctx->T3460.id, emm_ctx->ue_id);
  MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3460 stopped UE " MME_UE_S1AP_ID_FMT " ", emm_ctx->ue_id);


  switch (emm_cause) {
  case EMM_CAUSE_SYNCH_FAILURE:
    /*
     * USIM has detected a mismatch in SQN.
     *  Ask for a new vector.
     */
    REQUIREMENT_3GPP_24_301(R10_5_4_2_4__3);
    MSC_LOG_EVENT (MSC_NAS_EMM_MME, "SQN SYNCH_FAILURE ue id " MME_UE_S1AP_ID_FMT " ", ue_id);

    emm_ctx->auth_sync_fail_count += 1;
    if (EMM_AUTHENTICATION_SYNC_FAILURE_MAX > emm_ctx->auth_sync_fail_count) {
      OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC  - USIM has detected a mismatch in SQN Ask for new vector(s)\n");

      REQUIREMENT_3GPP_24_301(R10_5_4_2_7_e__3);
      // Pass back the current rand.
      REQUIREMENT_3GPP_24_301(R10_5_4_2_7_e__2);
      struct tagbstring resync_param;
      resync_param.data = (unsigned char *) calloc(1, RESYNC_PARAM_LENGTH);
      DevAssert(resync_param.data != NULL);
      if (resync_param.data == NULL) {
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
      }

      memcpy (resync_param.data, (emm_ctx->_vector[emm_ctx->_security.vector_index].rand), RAND_LENGTH_OCTETS);
      memcpy ((resync_param.data + RAND_LENGTH_OCTETS), auts->data, AUTS_LENGTH);
      // TODO: Double check this case as there is no identity request being sent.
      nas_itti_auth_info_req(ue_id, emm_ctx->_imsi64, false, &emm_ctx->originating_tai.plmn, MAX_EPS_AUTH_VECTORS,
                             &resync_param);
      emm_ctx_clear_auth_vectors(emm_ctx);
      rc = RETURNok;
      emm_proc_common_clear_args(ue_id);
      data = NULL;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
    } else {
      REQUIREMENT_3GPP_24_301(R10_5_4_2_7_e__NOTE3);
      rc = _authentication_reject(data);
    }
    break;

  case EMM_CAUSE_MAC_FAILURE:
    emm_ctx->auth_sync_fail_count = 0;
    if (!IS_EMM_CTXT_PRESENT_IMSI(emm_ctx)) { // VALID means received in IDENTITY RESPONSE
      REQUIREMENT_3GPP_24_301(R10_5_4_2_7_c__2);
      rc = emm_proc_identification (emm_ctx->ue_id, emm_ctx, EMM_IDENT_TYPE_IMSI,
          _authentication_check_imsi_5_4_2_5__1, _authentication_reject, _authentication_ll_failure);

      if (rc != RETURNok) {
        REQUIREMENT_3GPP_24_301(R10_5_4_2_7_c__NOTE1); // more or less this case...
        // Failed to initiate the identification procedure
        OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT "EMM-PROC  - Failed to initiate identification procedure\n", emm_ctx->ue_id);
        emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
        // Do not accept the UE to attach to the network
        rc = _authentication_reject(data);
      }
    } else {
      REQUIREMENT_3GPP_24_301(R10_5_4_2_5__2);
      emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
      // Do not accept the UE to attach to the network
      rc = _authentication_reject(data);
    }
    break;

  case EMM_CAUSE_NON_EPS_AUTH_UNACCEPTABLE:
    emm_ctx->auth_sync_fail_count = 0;
    REQUIREMENT_3GPP_24_301(R10_5_4_2_7_d__1);
    rc = emm_proc_identification (emm_ctx->ue_id, emm_ctx, EMM_IDENT_TYPE_IMSI,
        _authentication_check_imsi_5_4_2_5__1, _authentication_reject, _authentication_ll_failure);

    if (rc != RETURNok) {
      REQUIREMENT_3GPP_24_301(R10_5_4_2_7_d__NOTE2); // more or less this case...
      // Failed to initiate the identification procedure
      OAILOG_WARNING (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT "EMM-PROC  - Failed to initiate identification procedure\n", emm_ctx->ue_id);
      emm_ctx->emm_cause = EMM_CAUSE_ILLEGAL_UE;
      // Do not allow the UE to attach to the network
      rc = _authentication_reject(data);
      // TODO: Check return value.
      MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_EMM_MME, NULL, 0, "EMMREG_COMMON_PROC_REJ ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
    }
    break;

  default:
    emm_ctx->auth_sync_fail_count = 0;
    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC  - The MME received an unknown EMM CAUSE %d\n", emm_cause);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);

  }
  emm_proc_common_clear_args(ue_id);
  data = NULL;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}


/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_authentication_complete()                            **
 **                                                                        **
 ** Description: Performs the authentication completion procedure executed **
 **      by the network.                                                   **
 **                                                                        **
 **              3GPP TS 24.301, section 5.4.2.4                           **
 **      Upon receiving the AUTHENTICATION RESPONSE message, the           **
 **      MME shall stop timer T3460 and check the correctness of           **
 **      the RES parameter.                                                **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                          **
 **      emm_cause: Authentication failure EMM cause code                  **
 **      res:       Authentication response parameter. or auts             **
 **                 in case of sync failure                                **
 **      Others:    None                                                   **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                                  **
 **      Others:    _emm_data, T3460                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_proc_authentication_complete (
  mme_ue_s1ap_id_t ue_id)
{
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Authentication complete (ue_id=" MME_UE_S1AP_ID_FMT ")\n", ue_id);
  // Get the UE context
  emm_data_context_t                     *emm_ctx = emm_data_context_get (&_emm_data, ue_id);
  authentication_data_t                  *data = (authentication_data_t *) (emm_proc_common_get_args (ue_id));


  DevAssert(emm_ctx != NULL);
  if (!emm_ctx) {
    emm_proc_common_clear_args(ue_id);
    data = NULL;
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Failed to authenticate the UE due to NULL emm_ctx\n");
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }

  // Stop timer T3460
  REQUIREMENT_3GPP_24_301(R10_5_4_2_4__1);
  emm_ctx->T3460.id = nas_timer_stop (emm_ctx->T3460.id);
  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3460 (%d) UE " MME_UE_S1AP_ID_FMT "\n", emm_ctx->T3460.id, emm_ctx->ue_id);
  MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3460 stopped UE " MME_UE_S1AP_ID_FMT " ", emm_ctx->ue_id);

  emm_ctx->auth_sync_fail_count = 0;

  REQUIREMENT_3GPP_24_301(R10_5_4_2_4__2);
  emm_ctx_set_security_eksi(emm_ctx, data->ksi);
  OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC  - Successful authentication of the UE  RESP XRES == XRES UE CONTEXT\n");
  /*
   * Notify EMM that the authentication procedure successfully completed
   */
  MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_EMM_MME, NULL, 0, "EMMREG_COMMON_PROC_CNF ue id "
      MME_UE_S1AP_ID_FMT
      " ", ue_id);
  OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC  - Notify EMM that the authentication procedure successfully completed\n");
  emm_sap.primitive = EMMREG_COMMON_PROC_CNF;
  emm_sap.u.emm_reg.ue_id = ue_id;
  emm_sap.u.emm_reg.ctx = emm_ctx;
  emm_sap.u.emm_reg.u.common.is_attached = emm_ctx->is_attached;

  emm_proc_common_clear_args(ue_id);
  data = NULL;
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/


/*
   --------------------------------------------------------------------------
                Timer handlers
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    _authentication_t3460_handler()                           **
 **                                                                        **
 ** Description: T3460 timeout handler                                     **
 **      Upon T3460 timer expiration, the authentication request   **
 **      message is retransmitted and the timer restarted. When    **
 **      retransmission counter is exceed, the MME shall abort the **
 **      authentication procedure and any ongoing EMM specific     **
 **      procedure and release the NAS signalling connection.      **
 **                                                                        **
 **              3GPP TS 24.301, section 5.4.2.7, case b                   **
 **                                                                        **
 ** Inputs:  args:      handler parameters                         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static void  *_authentication_t3460_handler (void *args)
{
  authentication_data_t                  *data = (authentication_data_t *) (args);
  OAILOG_FUNC_IN (LOG_NAS_EMM);

  if (data) {
    /*
     * Increment the retransmission counter
     */
    REQUIREMENT_3GPP_24_301(R10_5_4_2_7_b);
    // TODO the network shall abort any ongoing EMM specific procedure.

    data->retransmission_count += 1;
    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - T3460 timer expired, retransmission " "counter = %d\n", data->retransmission_count);

    if (data->retransmission_count < AUTHENTICATION_COUNTER_MAX) {
      /*
       * Send authentication request message to the UE
       */
      _authentication_request (data);
    } else {
      /*
       * Abort the authentication procedure
       */
      emm_sap_t                               emm_sap = {0};
      emm_sap.primitive = EMMREG_PROC_ABORT;
      emm_sap.u.emm_reg.ue_id = data->ue_id;
      data->notify_failure = true;
      emm_sap_send (&emm_sap);
      // Clean up MME APP UE context  
      emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
      emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = data->ue_id;
      emm_sap_send (&emm_sap);
    }
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, NULL);
}

/*
   --------------------------------------------------------------------------
                MME specific local functions
   --------------------------------------------------------------------------
*/

int _authentication_check_imsi_5_4_2_5__1 (void * args) {
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  struct emm_data_context_s              *emm_ctx = NULL;
  authentication_data_t                  *data = (authentication_data_t *) (args);

  if (data) {
    emm_ctx = emm_data_context_get (&_emm_data, data->ue_id);
    if (emm_ctx) {
      REQUIREMENT_3GPP_24_301(R10_5_4_2_5__1);
      if (IS_EMM_CTXT_VALID_IMSI(emm_ctx)) { // VALID means received in IDENTITY RESPONSE
        if (emm_ctx->_imsi64 != emm_ctx->saved_imsi64) {
          nas_itti_auth_info_req (emm_ctx->ue_id, emm_ctx->_imsi64, false, &emm_ctx->originating_tai.plmn,
                                  MAX_EPS_AUTH_VECTORS, NULL);
          OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
        }
      }
      rc = _authentication_reject(data);
    }
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _authentication_request()                                 **
 **                                                                        **
 ** Description: Sends AUTHENTICATION REQUEST message and start timer T3460**
 **                                                                        **
 ** Inputs:  args:      handler parameters                         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3460                                      **
 **                                                                        **
 ***************************************************************************/
int
_authentication_request (
  authentication_data_t * data)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNerror;
  struct emm_data_context_s              *emm_ctx = NULL;

  if (data) {
    /*
     * Notify EMM-AS SAP that Authentication Request message has to be sent
     * to the UE
     */
    emm_sap.primitive = EMMAS_SECURITY_REQ;
    emm_sap.u.emm_as.u.security.guti = NULL;
    emm_sap.u.emm_as.u.security.ue_id = data->ue_id;
    emm_sap.u.emm_as.u.security.msg_type = EMM_AS_MSG_TYPE_AUTH;
    emm_sap.u.emm_as.u.security.ksi = data->ksi;
    memcpy(emm_sap.u.emm_as.u.security.rand, data->rand, sizeof (emm_sap.u.emm_as.u.security.rand));
    memcpy(emm_sap.u.emm_as.u.security.autn, data->autn, sizeof (emm_sap.u.emm_as.u.security.autn));
    /*
     * TODO: check for pointer validity
     */
    emm_ctx = emm_data_context_get (&_emm_data, data->ue_id);
    AssertFatal(emm_ctx != NULL, "emm_ctx should not be NULL" MME_UE_S1AP_ID_FMT " ", data->ue_id);
    /*
     * Setup EPS NAS security data
     */
    emm_as_set_security_data (&emm_sap.u.emm_as.u.security.sctx, &emm_ctx->_security, false, true);
    REQUIREMENT_3GPP_24_301(R10_5_4_2_2);
    MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_EMM_MME, NULL, 0, "EMMAS_SECURITY_REQ ue id " MME_UE_S1AP_ID_FMT " ", data->ue_id);
    rc = emm_sap_send (&emm_sap);

    if (rc != RETURNerror) {
      emm_ctx_mark_common_procedure_running(emm_ctx, EMM_CTXT_COMMON_PROC_AUTH);
      if (emm_ctx->T3460.id != NAS_TIMER_INACTIVE_ID) {
        /*
         * Re-start T3460 timer
         */
        emm_ctx->T3460.id = nas_timer_restart (emm_ctx->T3460.id);
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3460 restarted UE " MME_UE_S1AP_ID_FMT " ", data->ue_id);
      } else {
        /*
         * Start T3460 timer
         */
        emm_ctx->T3460.id = nas_timer_start (emm_ctx->T3460.sec, _authentication_t3460_handler, data);
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3460 started UE " MME_UE_S1AP_ID_FMT " ", data->ue_id);
      }

      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Timer T3460 (%d) expires in %ld seconds\n", emm_ctx->T3460.id, emm_ctx->T3460.sec);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _authentication_reject()                                  **
 **                                                                        **
 ** Description: Sends AUTHENTICATION REJECT message                       **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _authentication_reject (void* args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  emm_sap_t                               emm_sap_rej = {0};
  int                                     rc = RETURNerror;
  struct emm_data_context_s              *emm_ctx = NULL;
  authentication_data_t                  *data = (authentication_data_t *) (args);

  if (data) {
    /*
     * Notify EMM-AS SAP that Authentication Reject message has to be sent
     * to the UE
     */
    emm_sap.primitive = EMMAS_SECURITY_REJ;
    emm_sap.u.emm_as.u.security.guti = NULL;
    emm_sap.u.emm_as.u.security.ue_id = data->ue_id;
    emm_sap.u.emm_as.u.security.msg_type = EMM_AS_MSG_TYPE_AUTH;
    emm_ctx = emm_data_context_get (&_emm_data, data->ue_id);

    /*
     * Setup EPS NAS security data
     */
    emm_as_set_security_data (&emm_sap.u.emm_as.u.security.sctx, &emm_ctx->_security, false, true);
    rc = emm_sap_send (&emm_sap);

    /*
     * Notify EMM that the authentication procedure failed
     */
    MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_EMM_MME, NULL, 0, "EMMREG_COMMON_PROC_REJ ue id " MME_UE_S1AP_ID_FMT " ", data->ue_id);
    emm_sap_rej.primitive = EMMREG_COMMON_PROC_REJ;
    emm_sap_rej.u.emm_reg.ue_id = data->ue_id;
    emm_sap_rej.u.emm_reg.ctx = emm_ctx;
    rc = emm_sap_send (&emm_sap_rej);
    
   _clear_emm_ctxt(emm_ctx);

  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _authentication_ll_failure()                                   **
 **                                                                        **
 ** Description: Aborts the authentication procedure currently in progress **
 ** Inputs:  args:      Authentication data to be released         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3460                                      **
 **                                                                        **
 ***************************************************************************/
static int _authentication_ll_failure (void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  if (args) {
    authentication_data_t                  *data = (authentication_data_t *) (args);
    REQUIREMENT_3GPP_24_301(R10_5_4_2_7_a);
    emm_sap_t                               emm_sap = {0};

    emm_sap.primitive = EMMREG_PROC_ABORT;
    emm_sap.u.emm_reg.ue_id = data->ue_id;
    data->notify_failure = true;
    rc = emm_sap_send (&emm_sap);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _authentication_non_delivered()                                   **
 **                                                                        **
 ** Description:  **
 ** Inputs:  args:      Authentication data to be released         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3460                                      **
 **                                                                        **
 ***************************************************************************/
static int _authentication_non_delivered (void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  if (args) {
    authentication_data_t                *data = (authentication_data_t *) (args);
    REQUIREMENT_3GPP_24_301(R10_5_4_2_7_j);
    rc = _authentication_request(data);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}


/****************************************************************************
 **                                                                        **
 ** Name:    _authentication_abort()                                   **
 **                                                                        **
 ** Description: Aborts the authentication procedure currently in progress **
 **                                                                        **
 ** Inputs:  args:      Authentication data to be released         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3460                                      **
 **                                                                        **
 ***************************************************************************/
static int _authentication_abort (void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  struct emm_data_context_s              *emm_ctx = {0};
  authentication_data_t                  *data = (authentication_data_t *) (args);

  if (data) {
    mme_ue_s1ap_id_t                        ue_id = data->ue_id;

    OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - Abort authentication procedure " "(ue_id=" MME_UE_S1AP_ID_FMT ")\n", ue_id);
    emm_ctx = emm_data_context_get (&_emm_data, ue_id);

    if (emm_ctx) {
      emm_ctx_unmark_common_procedure_running(emm_ctx, EMM_CTXT_COMMON_PROC_AUTH);
      /*
       * Stop timer T3460
       */
      if (emm_ctx->T3460.id != NAS_TIMER_INACTIVE_ID) {
        OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3460 (%d)\n", emm_ctx->T3460.id);
        emm_ctx->T3460.id = nas_timer_stop (emm_ctx->T3460.id);
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3460 stopped UE " MME_UE_S1AP_ID_FMT " ", ue_id);
      }
    }

    if (data->notify_failure) {
      /*
       * Notify EMM that the authentication procedure failed
       */
      emm_sap_t                               emm_sap = {0};

      emm_sap.primitive = EMMREG_COMMON_PROC_REJ;
      emm_sap.u.emm_reg.ue_id = ue_id;
      rc = emm_sap_send (&emm_sap);
    }

    /*
     * Release retransmission timer parameters
     * Do it after emm_sap_send
     */
    emm_proc_common_clear_args(ue_id);
    data = NULL;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
