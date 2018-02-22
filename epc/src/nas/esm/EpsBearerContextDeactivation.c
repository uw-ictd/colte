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
  Source      EpsBearerContextDeactivation.c

  Version     0.1

  Date        2013/05/22

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Frederic Maurel

  Description Defines the EPS bearer context deactivation ESM procedure
        executed by the Non-Access Stratum.

        The purpose of the EPS bearer context deactivation procedure
        is to deactivate an EPS bearer context or disconnect from a
        PDN by deactivating all EPS bearer contexts to the PDN.
        The EPS bearer context deactivation procedure is initiated
        by the network, and it may be triggered by the UE by means
        of the UE requested bearer resource modification procedure
        or UE requested PDN disconnect procedure.

*****************************************************************************/

#include "3gpp_24.007.h"
#include "esm_proc.h"
#include "commonDef.h"
#include "log.h"

#include "emmData.h"
#include "esmData.h"
#include "esm_cause.h"
#include "esm_ebr.h"
#include "esm_ebr_context.h"

#include "emm_sap.h"
#include "esm_sap.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/



/*
   --------------------------------------------------------------------------
   Internal data handled by the EPS bearer context deactivation procedure
   in the MME
   --------------------------------------------------------------------------
*/
/*
   Timer handlers
*/
static void *_eps_bearer_deactivate_t3495_handler (void *);

/* Maximum value of the deactivate EPS bearer context request
   retransmission counter */
#define EPS_BEARER_DEACTIVATE_COUNTER_MAX   5

static int _eps_bearer_deactivate (emm_data_context_t * ctx, int ebi, STOLEN_REF bstring *msg);
static int _eps_bearer_release (emm_data_context_t * ctx, int ebi, int *pid, int *bid);


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
   --------------------------------------------------------------------------
    EPS bearer context deactivation procedure executed by the MME
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_eps_bearer_context_deactivate()                  **
 **                                                                        **
 ** Description: Locally releases the EPS bearer context identified by the **
 **      given EPS bearer identity, without peer-to-peer signal-   **
 **      ling between the UE and the MME, or checks whether an EPS **
 **      bearer context with specified EPS bearer identity has     **
 **      been activated for the given UE.                          **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      is local:  true if the EPS bearer context has to be   **
 **             locally released without peer-to-peer si-  **
 **             gnalling between the UE and the MME        **
 **      ebi:       EPS bearer identity of the EPS bearer con- **
 **             text to be deactivated                     **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     pid:       Identifier of the PDN connection the EPS   **
 **             bearer belongs to                          **
 **      bid:       Identifier of the released EPS bearer con- **
 **             text entry                                 **
 **      esm_cause: Cause code returned upon ESM procedure     **
 **             failure                                    **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_eps_bearer_context_deactivate (
  emm_data_context_t * ctx,
  bool is_local,
  int ebi,
  int *pid,
  int *bid,
  int *esm_cause)
{
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_NAS_ESM);
  if (!((ctx) && (ctx->esm_data_ctx.pdn[0].data) && (ctx->esm_data_ctx.pdn[0].data->bearer[0]))) {
    /* At present only one PDN connection and one bearer within that PDN connection is supported 
     * So using index 0 for pid and bid.
     */
    OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - EPS bearer context deactivation: No Valid context\n");
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
  }

  if (is_local) {
    // Get the valid EPS bearer Id from the ESM context and set the ebi value accordingly 
    ebi = ctx->esm_data_ctx.pdn[0].data->bearer[0]->ebi;
    OAILOG_DEBUG (LOG_NAS_ESM, "ESM-PROC  - EPS bearer context deactivation " "(ue_id=" MME_UE_S1AP_ID_FMT ", ebi=%d)\n", ctx->ue_id, ebi);
    if (ebi != ESM_SAP_ALL_EBI) {
      /*
       * Locally release the specified EPS bearer context
       */
      rc = _eps_bearer_release (ctx, ebi, pid, bid);
    } else if (ctx ) {
      /*
       * Locally release all the EPS bearer contexts
       */
      *bid = 0;

      for (*pid = 0; *pid < ESM_DATA_PDN_MAX; (*pid)++) {
        if (ctx->esm_data_ctx.pdn[*pid].data) {
          rc = _eps_bearer_release (ctx, ESM_EBI_UNASSIGNED, pid, bid);

          if (rc != RETURNok) {
            break;
          }
        }
      }
    }

    OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
  }

  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - EPS bearer context deactivation " "(ue_id=" MME_UE_S1AP_ID_FMT ", ebi=%d)\n", ctx->ue_id, ebi);

  if ((ctx ) && (*pid < ESM_DATA_PDN_MAX)) {
    if (ctx->esm_data_ctx.pdn[*pid].pid != *pid) {
      OAILOG_ERROR (LOG_NAS_ESM, "ESM-PROC  - PDN connection identifier %d " "is not valid\n", *pid);
      *esm_cause = ESM_CAUSE_PROTOCOL_ERROR;
    } else if (ctx->esm_data_ctx.pdn[*pid].data == NULL) {
      OAILOG_ERROR (LOG_NAS_ESM, "ESM-PROC  - PDN connection %d has not been " "allocated\n", *pid);
      *esm_cause = ESM_CAUSE_PROTOCOL_ERROR;
    } else if (!ctx->esm_data_ctx.pdn[*pid].is_active) {
      OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - PDN connection %d is not active\n", *pid);
      *esm_cause = ESM_CAUSE_PROTOCOL_ERROR;
    } else {
      int                                     i;
      esm_pdn_t                              *pdn = ctx->esm_data_ctx.pdn[*pid].data;

      *esm_cause = ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY;

      for (i = 0; i < pdn->n_bearers; i++) {
        if (pdn->bearer[i]->ebi != ebi) {
          continue;
        }

        /*
         * The EPS bearer context to be released is valid
         */
        rc = RETURNok;
      }
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_eps_bearer_context_deactivate_request()          **
 **                                                                        **
 ** Description: Initiates the EPS bearer context deactivation procedure   **
 **                                                                        **
 **      3GPP TS 24.301, section 6.4.4.2                           **
 **      If a NAS signalling connection exists, the MME initiates  **
 **      the EPS bearer context deactivation procedure by sending  **
 **      a DEACTIVATE EPS BEARER CONTEXT REQUEST message to the    **
 **      UE, starting timer T3495 and entering state BEARER CON-   **
 **      TEXT INACTIVE PENDING.                                    **
 **                                                                        **
 ** Inputs:  is_standalone: Not used - Always true                     **
 **      ue_id:      UE lower layer identifier                  **
 **      ebi:       EPS bearer identity                        **
 **      msg:       Encoded ESM message to be sent             **
 **      ue_triggered:  true if the EPS bearer context procedure   **
 **             was triggered by the UE (not used)         **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_eps_bearer_context_deactivate_request (
  bool is_standalone,
  emm_data_context_t * ctx,
  int ebi,
  bstring msg,
  bool ue_triggered)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc;

  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - Initiate EPS bearer context deactivation " "(ue_id=" MME_UE_S1AP_ID_FMT ", ebi=%d)\n", ctx->ue_id, ebi);
  /*
   * Send deactivate EPS bearer context request message and
   * * * * start timer T3495
   */
  rc = _eps_bearer_deactivate (ctx, ebi, &msg);

  if (rc != RETURNerror) {
    /*
     * Set the EPS bearer context state to ACTIVE PENDING
     */
    rc = esm_ebr_set_status (ctx, ebi, ESM_EBR_INACTIVE_PENDING, ue_triggered);

    if (rc != RETURNok) {
      /*
       * The EPS bearer context was already in ACTIVE state
       */
      OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - EBI %d was already INACTIVE PENDING\n", ebi);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_eps_bearer_context_deactivate_accept()           **
 **                                                                        **
 ** Description: Performs EPS bearer context deactivation procedure accep- **
 **      ted by the UE.                                            **
 **                                                                        **
 **      3GPP TS 24.301, section 6.4.4.3                           **
 **      Upon receipt of the DEACTIVATE EPS BEARER CONTEXT ACCEPT  **
 **      message, the MME shall enter the state BEARER CONTEXT     **
 **      INACTIVE and stop the timer T3495.                        **
 **                                                                        **
 ** Inputs:  ue_id:      UE local identifier                        **
 **      ebi:       EPS bearer identity                        **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     esm_cause: Cause code returned upon ESM procedure     **
 **             failure                                    **
 **      Return:    The identifier of the PDN connection to be **
 **             released, if it exists;                    **
 **             RETURNerror otherwise.                     **
 **      Others:    T3495                                      **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_eps_bearer_context_deactivate_accept (
  emm_data_context_t * ctx,
  int ebi,
  int *esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc;
  int                                     pid = RETURNerror;

  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - EPS bearer context deactivation " "accepted by the UE (ue_id=" MME_UE_S1AP_ID_FMT ", ebi=%d)\n", ctx->ue_id, ebi);
  /*
   * Stop T3495 timer if running
   */
  rc = esm_ebr_stop_timer (ctx, ebi);

  if (rc != RETURNerror) {
    int                                     bid;

    /*
     * Release the EPS bearer context
     */
    rc = _eps_bearer_release (ctx, ebi, &pid, &bid);

    if (rc != RETURNok) {
      /*
       * Failed to release the EPS bearer context
       */
      *esm_cause = ESM_CAUSE_PROTOCOL_ERROR;
      pid = RETURNerror;
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, pid);
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
 ** Name:    _eps_bearer_deactivate_t3495_handler()                    **
 **                                                                        **
 ** Description: T3495 timeout handler                                     **
 **                                                                        **
 **              3GPP TS 24.301, section 6.4.4.5, case a                   **
 **      On the first expiry of the timer T3495, the MME shall re- **
 **      send the DEACTIVATE EPS BEARER CONTEXT REQUEST and shall  **
 **      reset and restart timer T3495. This retransmission is     **
 **      repeated four times, i.e. on the fifth expiry of timer    **
 **      T3495, the MME shall abort the procedure and deactivate   **
 **      the EPS bearer context locally.                           **
 **                                                                        **
 ** Inputs:  args:      handler parameters                         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static void                            *
_eps_bearer_deactivate_t3495_handler (
  void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc;

  /*
   * Get retransmission timer parameters data
   */
  esm_ebr_timer_data_t                   *data = (esm_ebr_timer_data_t *) (args);

  /*
   * Increment the retransmission counter
   */
  data->count += 1;
  OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - T3495 timer expired (ue_id=" MME_UE_S1AP_ID_FMT ", ebi=%d), " "retransmission counter = %d\n",
      data->ue_id, data->ebi, data->count);

  if (data->count < EPS_BEARER_DEACTIVATE_COUNTER_MAX) {
    /*
     * Re-send deactivate EPS bearer context request message to the UE
     */
    bstring b = bstrcpy(data->msg);
    rc = _eps_bearer_deactivate (data->ctx, data->ebi, &b);
  } else {
    /*
     * The maximum number of deactivate EPS bearer context request
     * message retransmission has exceed
     */
    int                                     pid,
                                            bid;

    /*
     * Deactivate the EPS bearer context locally without peer-to-peer
     * * * * signalling between the UE and the MME
     */
    rc = _eps_bearer_release (data->ctx, data->ebi, &pid, &bid);

    if (rc != RETURNerror) {
      /*
       * Stop timer T3495
       */
      rc = esm_ebr_stop_timer (data->ctx, data->ebi);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, NULL);
}

/*
   --------------------------------------------------------------------------
                MME specific local functions
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    _eps_bearer_deactivate()                                  **
 **                                                                        **
 ** Description: Sends DEACTIVATE EPS BEREAR CONTEXT REQUEST message and   **
 **      starts timer T3495                                        **
 **                                                                        **
 ** Inputs:  ue_id:      UE local identifier                        **
 **      ebi:       EPS bearer identity                        **
 **      msg:       Encoded ESM message to be sent             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3495                                      **
 **                                                                        **
 ***************************************************************************/
static int
_eps_bearer_deactivate (
  emm_data_context_t * ctx,
  int ebi,
  STOLEN_REF bstring *msg)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc;

  /*
   * Notify EMM that a deactivate EPS bearer context request message
   * has to be sent to the UE
   */
  emm_esm_data_t                         *emm_esm = &emm_sap.u.emm_esm.u.data;

  emm_sap.primitive = EMMESM_UNITDATA_REQ;
  emm_sap.u.emm_esm.ue_id = ctx->ue_id;
  emm_sap.u.emm_esm.ctx = ctx;
  emm_esm->msg = *msg;
  rc = emm_sap_send (&emm_sap);

  if (rc != RETURNerror) {
    /*
     * Start T3495 retransmission timer
     */
    rc = esm_ebr_start_timer (ctx, ebi, *msg, T3495_DEFAULT_VALUE, _eps_bearer_deactivate_t3495_handler);
  }
  *msg = NULL;
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _eps_bearer_release()                                     **
 **                                                                        **
 ** Description: Releases the EPS bearer context identified by the given   **
 **      EPS bearer identity and enters state INACTIVE.            **
 **                                                                        **
 ** Inputs:  ue_id:      UE local identifier                        **
 **      ebi:       EPS bearer identity                        **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     pid:       Identifier of the PDN connection the EPS   **
 **             bearer belongs to                          **
 **      bid:       Identifier of the released EPS bearer con- **
 **             text entry                                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int
_eps_bearer_release (
  emm_data_context_t * ctx,
  int ebi,
  int *pid,
  int *bid)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc = RETURNok;

  /*
   * Release the EPS bearer context entry
   */
  ebi = esm_ebr_context_release (ctx, ebi, pid, bid);

  if (ebi == ESM_EBI_UNASSIGNED) {
    OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - Failed to release EPS bearer context\n");
  } else {
    /*
     * Set the EPS bearer context state to INACTIVE
     */
    rc = esm_ebr_set_status (ctx, ebi, ESM_EBR_INACTIVE, false);

    if (rc != RETURNok) {
      /*
       * The EPS bearer context was already in INACTIVE state
       */
      OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - EBI %d was already INACTIVE\n", ebi);
    } 
    /*
     * Release EPS bearer data
     */
    rc = esm_ebr_release (ctx, ebi);

    if (rc != RETURNok) {
      OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - Failed to release EPS bearer data\n");
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}
