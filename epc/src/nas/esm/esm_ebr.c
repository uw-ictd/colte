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
  Source      esm_ebr.c

  Version     0.1

  Date        2013/01/29

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Frederic Maurel

  Description Defines functions used to handle state of EPS bearer contexts
        and manage ESM messages re-transmission.

*****************************************************************************/

#include <stdlib.h>             // malloc, free_wrapper
#include <string.h>             // memcpy

#include "dynamic_memory_check.h"
#include "log.h"
#include "msc.h"
#include "3gpp_24.007.h"
#include "commonDef.h"
#include "emmData.h"
#include "esm_ebr.h"
#include "mme_api.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/


#define ESM_EBR_NB_UE_MAX   (MME_API_NB_UE_MAX + 1)

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* String representation of EPS bearer context status */
static const char                      *_esm_ebr_state_str[ESM_EBR_STATE_MAX] = {
  "BEARER CONTEXT INACTIVE",
  "BEARER CONTEXT ACTIVE",
  "BEARER CONTEXT INACTIVE PENDING",
  "BEARER CONTEXT MODIFY PENDING",
  "BEARER CONTEXT ACTIVE PENDING"
};



/*
   ----------------------
   User notification data
   ----------------------
*/


/* Returns the index of the next available entry in the list of EPS bearer
   context data */
static int                              _esm_ebr_get_available_entry (
  emm_data_context_t * ctx);


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_initialize()                                      **
 **                                                                        **
 ** Description: Initialize EPS bearer context data                        **
 **                                                                        **
 ** Inputs:  cb:        User notification callback                 **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
void
esm_ebr_initialize (
  void
  )
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  OAILOG_FUNC_OUT (LOG_NAS_ESM);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_assign()                                          **
 **                                                                        **
 ** Description: Assigns a new EPS bearer context                          **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      ebi:       Identity of the new EPS bearer context     **
 **      cid:       Identifier of the PDN context the EPS bea- **
 **             rer context is associated to               **
 **      default_ebr    true if the new bearer context is associa- **
 **             ted to a default EPS bearer                **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The identity of the new EPS bearer context **
 **             if successfully assigned;                  **
 **             the not assigned EBI (0) otherwise.        **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_assign (
  emm_data_context_t * ctx,
  int ebi)
{
  esm_ebr_context_t                      *ebr_ctx = NULL;
  int                                     i;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  if (ebi != ESM_EBI_UNASSIGNED) {
    if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
    } else {
      ebr_ctx = ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN];
      if (ebr_ctx) {
        OAILOG_WARNING (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context already " "assigned (ebi=%d)\n", ebi);
        OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
      }
    }   
    /*
     * The specified EPS bearer context is available
     */
    i = ebi - ESM_EBI_MIN;
  } else {
    /*
     * Search for an available EPS bearer identity
     */
    i = _esm_ebr_get_available_entry (ctx);

    if (i < 0) {
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
    }

    /*
     * An available EPS bearer context is found
     */
    ebi = i + ESM_EBI_MIN;
  }

  /*
   * Assign new EPS bearer context
   */
  ebr_ctx = (esm_ebr_context_t *) malloc (sizeof (esm_ebr_context_t));

  if (ebr_ctx == NULL) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
  }
  ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN] = ebr_ctx;

  /*
   * Store the index of the next available EPS bearer identity
   */
  ctx->esm_data_ctx.ebr.index = i + 1;
  /*
   * Set the EPS bearer identity
   */
  ebr_ctx->ebi = ebi;
  /*
   * Set the EPS bearer context status to INACTIVE
   */
  ebr_ctx->status = ESM_EBR_INACTIVE;
  /*
   * Disable the retransmission timer
   */
  ebr_ctx->timer.id = NAS_TIMER_INACTIVE_ID;
  /*
   * Setup retransmission timer parameters
   */
  ebr_ctx->args = NULL;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context %d assigned\n", ebi);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, ebr_ctx->ebi);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_release()                                         **
 **                                                                        **
 ** Description: Release the given EPS bearer identity                     **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer context to  **
 **             be released                                **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok if the EPS bearer context has     **
 **             been successfully released;                **
 **             RETURNerror otherwise.                     **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_release (
  emm_data_context_t * ctx,
  int ebi)
{
  esm_ebr_context_t                      *ebr_ctx;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }
  /*
   * Get EPS bearer context data
   */
  ebr_ctx = ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN];

  if ((ebr_ctx == NULL) || (ebr_ctx->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  /*
   * Do not release active EPS bearer context
   */
  if (ebr_ctx->status != ESM_EBR_INACTIVE) {
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context is not INACTIVE\n");
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  /*
   * Stop the retransmission timer if still running
   */
  if (ebr_ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Stop retransmission timer %d\n", ebr_ctx->timer.id);
    ebr_ctx->timer.id = nas_timer_stop (ebr_ctx->timer.id);
    MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u stopped", ebr_ctx->timer.id, ebi);
  }

  /*
   * Release the retransmisison timer parameters
   */
  if (ebr_ctx->args) {
    if (ebr_ctx->args->msg) {
      bdestroy (ebr_ctx->args->msg);
    }

    free_wrapper ((void**) &ebr_ctx->args);
    ebr_ctx->args = NULL;
  }

  /*
   * Release EPS bearer context data
   */
  free_wrapper ((void**) &ebr_ctx);
  ebr_ctx = NULL;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context %d released\n", ebi);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_start_timer()                                     **
 **                                                                        **
 ** Description: Start the timer of the specified EPS bearer context to    **
 **      expire after a given time interval. Timer expiration will **
 **      schedule execution of the callback function where stored  **
 **      ESM message should be re-transmit.                        **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      msg:       The encoded ESM message to be stored       **
 **      sec:       The value of the time interval in seconds  **
 **      cb:        Function executed upon timer expiration    **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_start_timer (
  emm_data_context_t * ctx,
  int ebi,
  CLONE_REF const_bstring msg,
  long sec,
  nas_timer_callback_t cb)
{
  esm_ebr_context_t                      *ebr_ctx = NULL;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - Retransmission timer bad ebi %d\n", ebi);
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  /*
   * Get EPS bearer context data
   */
  ebr_ctx = ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN];

  if ((ebr_ctx == NULL) || (ebr_ctx->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context not assigned\n");
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  if (ebr_ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
    if (ebr_ctx->args) {
      /*
       * Re-start the retransmission timer
       */
      ebr_ctx->timer.id = nas_timer_restart (ebr_ctx->timer.id);
      MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u restarted", ebr_ctx->timer.id, ebi);
    }
  } else {
    /*
     * Setup the retransmission timer parameters
     */
    ebr_ctx->args = (esm_ebr_timer_data_t *) malloc (sizeof (esm_ebr_timer_data_t));

    if (ebr_ctx->args) {
      /*
       * Set the UE identifier
       */
      ebr_ctx->args->ue_id = ctx->ue_id;
      /*
       * Set the EPS bearer identity
       */
      ebr_ctx->args->ebi = ebi;
      /*
       * Reset the retransmission counter
       */
      ebr_ctx->args->count = 0;
      /*
       * Set the ESM message to be re-transmited
       */
      ebr_ctx->args->msg = bstrcpy (msg);

      /*
       * Setup the retransmission timer to expire at the given
       * * * * time interval
       */
      ebr_ctx->timer.id = nas_timer_start (sec, cb, ebr_ctx->args);
      MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u started", ebr_ctx->timer.id, ebi);
      ebr_ctx->timer.sec = sec;
    }
  }

  if ((ebr_ctx->args ) && (ebr_ctx->timer.id != NAS_TIMER_INACTIVE_ID)) {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Retransmission timer %d expires in " "%ld seconds\n", ebr_ctx->timer.id, ebr_ctx->timer.sec);
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
  } else {
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - ebr_ctx->args == NULL(%p) or ebr_ctx->timer.id == NAS_TIMER_INACTIVE_ID == -1 (%d)\n", ebr_ctx->args, ebr_ctx->timer.id);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_stop_timer()                                      **
 **                                                                        **
 ** Description: Stop the timer previously started for the given EPS bea-  **
 **      rer context                                               **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_stop_timer (
  emm_data_context_t * ctx,
  int ebi)
{
  esm_ebr_context_t                      *ebr_ctx = NULL;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  /*
   * Get EPS bearer context data
   */
  ebr_ctx = ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN];

  if ((ebr_ctx == NULL) || (ebr_ctx->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  /*
   * Stop the retransmission timer if still running
   */
  if (ebr_ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Stop retransmission timer %d\n", ebr_ctx->timer.id);
    ebr_ctx->timer.id = nas_timer_stop (ebr_ctx->timer.id);
    MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u stopped", ebr_ctx->timer.id, ebi);
  }

  /*
   * Release the retransmisison timer parameters
   */
  if (ebr_ctx->args) {
    if (ebr_ctx->args->msg) {
      bdestroy (ebr_ctx->args->msg);
    }

    free_wrapper ((void**) &ebr_ctx->args);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_get_pending_ebi()                                 **
 **                                                                        **
 ** Description: Returns the EPS bearer identity assigned to the first EPS **
 **      bearer context entry which is pending in the given state  **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      status:    The EPS bearer context status              **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The EPS bearer identity of the EPS bearer  **
 **             context entry if it exists;                **
 **             the not assigned EBI (0) otherwise.        **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_get_pending_ebi (
  emm_data_context_t * ctx,
  esm_ebr_state status)
{
  int                                     i;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  for (i = 0; i < ESM_EBR_DATA_SIZE; i++) {
    if (ctx->esm_data_ctx.ebr.context[i] == NULL) {
      continue;
    }

    if (ctx->esm_data_ctx.ebr.context[i]->status != status) {
      continue;
    }

    /*
     * EPS bearer context entry found
     */
    break;
  }

  if (i < ESM_EBR_DATA_SIZE) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, ctx->esm_data_ctx.ebr.context[i]->ebi);
  }

  /*
   * EPS bearer context entry not found
   */
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_set_status()                                      **
 **                                                                        **
 ** Description: Set the status of the specified EPS bearer context to the **
 **      given state                                               **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      status:    The new EPS bearer context status          **
 **      ue_requested:  true/false if the modification of the EPS  **
 **             bearer context status was requested by the **
 **             UE/network                                 **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_set_status (
  emm_data_context_t * ctx,
  int ebi,
  esm_ebr_state status,
  int ue_requested)
{
  esm_ebr_context_t                      *ebr_ctx = 0;
  esm_ebr_state                           old_status = ESM_EBR_INACTIVE;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  if (ctx == NULL) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  /*
   * Get EPS bearer context data
   */
  ebr_ctx = ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN];

  if ((ebr_ctx == NULL) || (ebr_ctx->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context not assigned \n" "(ebi=%d)", ebi);
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  old_status = ebr_ctx->status;

  if (status < ESM_EBR_STATE_MAX) {
    if (status != old_status) {
      OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Status of EPS bearer context %d changed:" " %s ===> %s\n",
          ebi, _esm_ebr_state_str[old_status], _esm_ebr_state_str[status]);
      MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 ESM state %s => %s " MME_UE_S1AP_ID_FMT " ",
          _esm_ebr_state_str[old_status], _esm_ebr_state_str[status], ctx->ue_id);
      ebr_ctx->status = status;
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
    } else {
      OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Status of EPS bearer context %d unchanged:" " %s \n",
          ebi, _esm_ebr_state_str[status]);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_get_status()                                      **
 **                                                                        **
 ** Description: Get the current status value of the specified EPS bearer  **
 **      context                                                   **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The current value of the EPS bearer con-   **
 **             text status                                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
esm_ebr_state
esm_ebr_get_status (
  emm_data_context_t * ctx,
  int ebi)
{
  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    return (ESM_EBR_INACTIVE);
  }

  if (ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN] == NULL) {
    /*
     * EPS bearer context not allocated
     */
    return (ESM_EBR_INACTIVE);
  }

  if (ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN]->ebi != ebi) {
    /*
     * EPS bearer context not assigned
     */
    return (ESM_EBR_INACTIVE);
  }

  return (ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN]->status);
}


/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_is_reserved()                                     **
 **                                                                        **
 ** Description: Check whether the given EPS bearer identity is a reserved **
 **      value                                                     **
 **                                                                        **
 ** Inputs:  ebi:       The identity of the EPS bearer             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    true, false                                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_is_reserved (
  int ebi)
{
  return ((ebi != ESM_EBI_UNASSIGNED) && (ebi < ESM_EBI_MIN));
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_is_not_in_use()                                   **
 **                                                                        **
 ** Description: Check whether the given EPS bearer identity does not      **
 **      match an assigned EBI value currently in use              **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      ebi:       The identity of the EPS bearer             **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    true, false                                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_is_not_in_use (
  emm_data_context_t * ctx,
  int ebi)
{
  return ((ebi == ESM_EBI_UNASSIGNED) || (ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN] == NULL) || (ctx->esm_data_ctx.ebr.context[ebi - ESM_EBI_MIN]->ebi) != ebi);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    _esm_ebr_get_available_entry()                            **
 **                                                                        **
 ** Description: Returns the index of the next available entry in the list **
 **      of EPS bearer context data                                **
 **                                                                        **
 ** Inputs:  ue_id:      Lower layers UE identifier                 **
 **      Others:    _esm_ebr_data                              **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The index of the next available EPS bearer **
 **             context data entry; -1 if no any entry is  **
 **             available.                                 **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int
_esm_ebr_get_available_entry (
  emm_data_context_t * ctx)
{
  int                                     i;

  for (i = ctx->esm_data_ctx.ebr.index; i < ESM_EBR_DATA_SIZE; i++) {
    if (ctx->esm_data_ctx.ebr.context[i] ) {
      continue;
    }

    return i;
  }

  for (i = 0; i < ctx->esm_data_ctx.ebr.index; i++) {
    if (ctx->esm_data_ctx.ebr.context[i] ) {
      continue;
    }

    return i;
  }

  /*
   * No available EBI entry found
   */
  return (-1);
}
