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
#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "bstrlib.h"

#include "dynamic_memory_check.h"
#include "log.h"
#include "msc.h"
#include "common_types.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"
#include "3gpp_29.274.h"
#include "common_defs.h"
#include "commonDef.h"
#include "mme_app_ue_context.h"
#include "emm_data.h"
#include "esm_ebr.h"
#include "esm_ebr_context.h"
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
static int                              _esm_ebr_get_available_entry (emm_context_t * emm_context);


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

//------------------------------------------------------------------------------
const char * esm_ebr_state2string(esm_ebr_state esm_ebr_state)
{
  switch (esm_ebr_state) {
    case ESM_EBR_INACTIVE:
      return "ESM_EBR_INACTIVE";
    case ESM_EBR_ACTIVE:
      return "ESM_EBR_ACTIVE";
    case ESM_EBR_INACTIVE_PENDING:
      return "ESM_EBR_INACTIVE_PENDING";
    case ESM_EBR_MODIFY_PENDING:
      return "ESM_EBR_MODIFY_PENDING";
    case ESM_EBR_ACTIVE_PENDING:
      return "ESM_EBR_ACTIVE_PENDING";
    default:
      return "UNKNOWN";
  }
}
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
void esm_ebr_initialize (void)
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
int esm_ebr_assign (emm_context_t * emm_context, ebi_t ebi)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  esm_ebr_context_t                      *ebr_ctx = NULL;
  bearer_context_t                       *bearer_context = NULL;
  int                                     i;

  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);

  if (ebi != ESM_EBI_UNASSIGNED) {
    if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
    } else {
      bearer_context = ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)];
      if (bearer_context ) {
        OAILOG_WARNING (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context already " "assigned (ebi=%d)\n", ebi);
        OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
      }
    }
    /*
     * The specified EPS bearer context is available
     */
    i = EBI_TO_INDEX(ebi);
  } else {
    /*
     * Search for an available EPS bearer identity
     */
    i = _esm_ebr_get_available_entry (emm_context);

    if (i < 0) {
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
    }

    /*
     * An available EPS bearer context is found
     */
    ebi = INDEX_TO_EBI(i);
    bearer_context = (bearer_context_t*) malloc (sizeof (bearer_context_t));
  }

  /*
   * Assign new EPS bearer context
   */

  if (bearer_context == NULL) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
  }
  ebr_ctx = &bearer_context->esm_ebr_context;

  /*
   * Set the EPS bearer identity
   */
  bearer_context->ebi = ebi;

  esm_ebr_context_init(ebr_ctx);

  OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context %d assigned\n", bearer_context->ebi);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, bearer_context->ebi);
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
int esm_ebr_release (emm_context_t * emm_context, ebi_t ebi)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  esm_ebr_context_t                      *ebr_ctx = NULL;
  bearer_context_t                       *bearer_context = NULL;

  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }
  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  /*
   * Get EPS bearer context data
   */
  bearer_context = ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)];

  if ((bearer_context == NULL) || (bearer_context->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }
  ebr_ctx = &bearer_context->esm_ebr_context;

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
    OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Stop retransmission timer %ld\n", ebr_ctx->timer.id);
    esm_ebr_timer_data_t * esm_ebr_timer_data = NULL;
    ebr_ctx->timer.id = nas_timer_stop (ebr_ctx->timer.id, (void**)&esm_ebr_timer_data);
    /*
     * Release the retransmisison timer parameters
     */
    if (esm_ebr_timer_data) {
      if (esm_ebr_timer_data->msg) {
        bdestroy_wrapper (&esm_ebr_timer_data->msg);
      }
      free_wrapper ((void**)&esm_ebr_timer_data);
    }
    MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u stopped", ebr_ctx->timer.id, ebi);
  }

  /*
   * Release EPS bearer context data
   */
  // struct attribute of another struct, no free

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
int esm_ebr_start_timer (emm_context_t * emm_context, ebi_t ebi,
  CLONE_REF const_bstring msg,
  long sec,
  nas_timer_callback_t cb)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  esm_ebr_context_t                      *ebr_ctx = NULL;
  bearer_context_t                       *bearer_context = NULL;


  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - Retransmission timer bad ebi %d\n", ebi);
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  /*
   * Get EPS bearer context data
   */
  bearer_context = ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)];

  if ((bearer_context == NULL) || (bearer_context->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context not assigned\n");
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }
  ebr_ctx = &bearer_context->esm_ebr_context;

  esm_ebr_timer_data_t * esm_ebr_timer_data = NULL;
  if (ebr_ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
    /*
     * Re-start the retransmission timer
     */
    ebr_ctx->timer.id = nas_timer_stop (ebr_ctx->timer.id, (void**)&esm_ebr_timer_data);
    ebr_ctx->timer.id = nas_timer_start (sec, 0 /* usec */, cb, esm_ebr_timer_data);
    MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u restarted", ebr_ctx->timer.id, ebi);
  } else {
    /*
     * Setup the retransmission timer parameters
     */
    esm_ebr_timer_data = (esm_ebr_timer_data_t *) calloc (1, sizeof (esm_ebr_timer_data_t));

    if (esm_ebr_timer_data) {
      /*
       * Set the UE identifier
       */
      esm_ebr_timer_data->ue_id = ue_mm_context->mme_ue_s1ap_id;
      esm_ebr_timer_data->ctx = emm_context;
      /*
       * Set the EPS bearer identity
       */
      esm_ebr_timer_data->ebi = ebi;
      /*
       * Reset the retransmission counter
       */
      esm_ebr_timer_data->count = 0;
      /*
       * Set the ESM message to be re-transmited
       */
      esm_ebr_timer_data->msg = bstrcpy (msg);

      /*
       * Setup the retransmission timer to expire at the given
       * * * * time interval
       */
      ebr_ctx->timer.id = nas_timer_start (sec, 0 /* usec */, cb, esm_ebr_timer_data);
      MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u started", ebr_ctx->timer.id, ebi);
      ebr_ctx->timer.sec = sec;
    }
  }

  if ((esm_ebr_timer_data) && (ebr_ctx->timer.id != NAS_TIMER_INACTIVE_ID)) {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Retransmission timer %ld expires in " "%ld seconds\n", ebr_ctx->timer.id, ebr_ctx->timer.sec);
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
  } else {
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - esm_ebr_timer_data == NULL(%p) or ebr_ctx->timer.id == NAS_TIMER_INACTIVE_ID == -1 (%ld)\n",
        esm_ebr_timer_data, ebr_ctx->timer.id);
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
int esm_ebr_stop_timer (emm_context_t * emm_context, ebi_t ebi)
{
  esm_ebr_context_t                      *ebr_ctx = NULL;
  bearer_context_t                       *bearer_context = NULL;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  /*
   * Get EPS bearer context data
   */
  bearer_context = ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)];

  if ((bearer_context == NULL) || (bearer_context->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  ebr_ctx = &bearer_context->esm_ebr_context;
  /*
   * Stop the retransmission timer if still running
   */
  if (ebr_ctx->timer.id != NAS_TIMER_INACTIVE_ID) {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Stop retransmission timer %ld\n", ebr_ctx->timer.id);
    esm_ebr_timer_data_t * esm_ebr_timer_data = NULL;
    ebr_ctx->timer.id = nas_timer_stop (ebr_ctx->timer.id, (void**)&esm_ebr_timer_data);
    MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 Timer %x ebi %u stopped", ebr_ctx->timer.id, ebi);
    /*
     * Release the retransmisison timer parameters
     */
    if (esm_ebr_timer_data) {
      if (esm_ebr_timer_data->msg) {
        bdestroy_wrapper (&esm_ebr_timer_data->msg);
      }
      free_wrapper ((void**)&esm_ebr_timer_data);
    }
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
ebi_t esm_ebr_get_pending_ebi (emm_context_t * emm_context, esm_ebr_state status)
{
  int                                     i;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  for (i = 0; i < BEARERS_PER_UE; i++) {
    if (ue_mm_context->bearer_contexts[i] == NULL) {
      continue;
    }

    if (ue_mm_context->bearer_contexts[i]->esm_ebr_context.status != status) {
      continue;
    }

    /*
     * EPS bearer context entry found
     */
    break;
  }

  if (i < BEARERS_PER_UE) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, ue_mm_context->bearer_contexts[i]->ebi);
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
  emm_context_t * emm_context,
  ebi_t ebi,
  esm_ebr_state status,
  bool ue_requested)
{
  bearer_context_t                       *bearer_context = NULL;
  esm_ebr_context_t                      *ebr_ctx = 0;
  esm_ebr_state                           old_status = ESM_EBR_INACTIVE;

  OAILOG_FUNC_IN (LOG_NAS_ESM);

  if (emm_context == NULL) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  /*
   * Get EPS bearer context data
   */
  bearer_context = ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)];

  if ((bearer_context == NULL) || (bearer_context->ebi != ebi)) {
    /*
     * EPS bearer context not assigned
     */
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-FSM   - EPS bearer context not assigned \n" "(ebi=%d)", ebi);
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
  }

  ebr_ctx = &bearer_context->esm_ebr_context;
  old_status = ebr_ctx->status;

  if (status < ESM_EBR_STATE_MAX) {
    if (status != old_status) {
      OAILOG_INFO (LOG_NAS_ESM, "ESM-FSM   - Status of EPS bearer context %d changed:" " %s ===> %s\n",
          ebi, _esm_ebr_state_str[old_status], _esm_ebr_state_str[status]);
      MSC_LOG_EVENT (MSC_NAS_ESM_MME, "0 ESM state %s => %s " MME_UE_S1AP_ID_FMT " ",
          _esm_ebr_state_str[old_status], _esm_ebr_state_str[status], ue_mm_context->mme_ue_s1ap_id);
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
  emm_context_t * emm_context,
  ebi_t ebi)
{
  if ((ebi < ESM_EBI_MIN) || (ebi > ESM_EBI_MAX)) {
    return (ESM_EBR_INACTIVE);
  }
  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  bearer_context_t                       *bearer_context = NULL;

  bearer_context = ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)];

  if (bearer_context == NULL) {
    /*
     * EPS bearer context not allocated
     */
    return (ESM_EBR_INACTIVE);
  }

  if (bearer_context->ebi != ebi) {
    /*
     * EPS bearer context not assigned
     */
    return (ESM_EBR_INACTIVE);
  }

  return (bearer_context->esm_ebr_context.status);
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
bool esm_ebr_is_reserved (ebi_t ebi)
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
bool
esm_ebr_is_not_in_use (
  emm_context_t * emm_context,
  ebi_t ebi)
{
  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  return ((ebi == ESM_EBI_UNASSIGNED) || (ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)] == NULL) || (ue_mm_context->bearer_contexts[EBI_TO_INDEX(ebi)]->ebi) != ebi);
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
static int _esm_ebr_get_available_entry (emm_context_t * emm_context)
{
  int                                     i;

  ue_mm_context_t      *ue_mm_context = PARENT_STRUCT(emm_context, struct ue_mm_context_s, emm_context);
  for (i = 0; i < BEARERS_PER_UE; i++) {
    if (!ue_mm_context->bearer_contexts[i]) {
      return i;
    }
  }

  /*
   * No available EBI entry found
   */
  return (-1);
}
