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

/*! \file esm_information.c
   \brief
   \author  Lionel GAUTHIER
   \date 2017
   \email: lionel.gauthier@eurecom.fr
*/
#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "bstrlib.h"

#include "log.h"
#include "dynamic_memory_check.h"
#include "common_types.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"
#include "3gpp_29.274.h"
#include "mme_app_ue_context.h"
#include "esm_proc.h"
#include "commonDef.h"
#include "emm_data.h"
#include "esm_data.h"
#include "esm_cause.h"
#include "esm_ebr.h"
#include "esm_ebr_context.h"
#include "emm_sap.h"
#include "esm_sap.h"
#include "esm_send.h"
#include "mme_config.h"
#include "mme_app_defs.h"
#include "nas_itti_messaging.h"
/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/


/*
   Timer handlers
*/
static void _esm_information_t3489_handler (void *);

/* Maximum value of the deactivate EPS bearer context request
   retransmission counter */
#define ESM_INFORMATION_COUNTER_MAX   3

static int _esm_information (emm_context_t * ue_context, ebi_t ebi, esm_ebr_timer_data_t * const data);


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/


//------------------------------------------------------------------------------
int esm_proc_esm_information_request (emm_context_t * const ue_context, const pti_t pti)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc;
  mme_ue_s1ap_id_t      ue_id = PARENT_STRUCT(ue_context, struct ue_mm_context_s, emm_context)->mme_ue_s1ap_id;

  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - Initiate ESM information ue_id=" MME_UE_S1AP_ID_FMT ")\n", ue_id);

  ESM_msg                                 esm_msg = {.header = {0}};
  rc = esm_send_esm_information_request (pti, EPS_BEARER_IDENTITY_UNASSIGNED, &esm_msg.esm_information_request);

  if (rc != RETURNerror) {
    /*
     * Encode the returned ESM response message
     */
    char                                    emm_sap_buffer[16]; // very short msg
    int                                     size = esm_msg_encode (&esm_msg, (uint8_t *) emm_sap_buffer, 16);
    bstring                                 msg_req = NULL;
    OAILOG_INFO (LOG_NAS_EMM, "ESM encoded MSG size %d\n", size);
    if (size > 0) {
      msg_req = blk2bstr(emm_sap_buffer, size);
      /*
       * Send esm information request message and
       * start timer T3489
       */
      esm_ebr_timer_data_t   *data = (esm_ebr_timer_data_t *) calloc(1, sizeof(*data));
      data->ctx = ue_context;
      data->ebi = EPS_BEARER_IDENTITY_UNASSIGNED;
      data->msg = msg_req;
      data->ue_id = ue_id;
      rc = _esm_information (ue_context, pti, data);
    }
  }
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}


//------------------------------------------------------------------------------
int esm_proc_esm_information_response (emm_context_t * ue_context, pti_t pti, const_bstring const apn, const protocol_configuration_options_t * const pco, esm_cause_t * const esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc = RETURNok;

  /*
   * Stop T3489 timer if running
   */
  nas_stop_T3489(&ue_context->esm_ctx);

  if (apn) {
    if (ue_context->esm_ctx.esm_proc_data->apn) {
      bdestroy_wrapper(&ue_context->esm_ctx.esm_proc_data->apn);
    }
    ue_context->esm_ctx.esm_proc_data->apn = bstrcpy(apn);
  }

  if ((pco) && (pco->num_protocol_or_container_id)) {
    if (ue_context->esm_ctx.esm_proc_data->pco.num_protocol_or_container_id) {
      clear_protocol_configuration_options(&ue_context->esm_ctx.esm_proc_data->pco);
    }
    copy_protocol_configuration_options(&ue_context->esm_ctx.esm_proc_data->pco, pco);
  }

  *esm_cause = ESM_CAUSE_SUCCESS;

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
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
 ** Name:    _esm_information_t3489_handler()                    **
 **                                                                        **
 ** Description: T3489 timeout handler                                     **
 **                                                                        **
 **              3GPP TS 24.301, section 6.4.4.5, case a                   **
 **      On the first expiry of the timer T3489, the MME shall re- **
 **      send the DEACTIVATE EPS BEARER CONTEXT REQUEST and shall  **
 **      reset and restart timer T3489. This retransmission is     **
 **      repeated four times, i.e. on the fifth expiry of timer    **
 **      T3489, the MME shall abort the procedure and deactivate   **
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
static void _esm_information_t3489_handler (void *args)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);

  /*
   * Get retransmission timer parameters data
   */
  esm_ebr_timer_data_t                   *esm_ebr_timer_data = (esm_ebr_timer_data_t *) (args);

  if (esm_ebr_timer_data) {
    /*
     * Increment the retransmission counter
     */
    esm_ebr_timer_data->count += 1;
    OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - T3489 timer expired (ue_id=" MME_UE_S1AP_ID_FMT "), " "retransmission counter = %d\n",
        esm_ebr_timer_data->ue_id, esm_ebr_timer_data->count);

    if (esm_ebr_timer_data->count < ESM_INFORMATION_COUNTER_MAX) {
      /*
       * Re-send deactivate EPS bearer context request message to the UE
       */
      _esm_information (esm_ebr_timer_data->ctx, esm_ebr_timer_data->ebi, esm_ebr_timer_data);
    } else {
      /*
       * The maximum number of deactivate EPS bearer context request
       * message retransmission has exceed
       */
      // TODO call something like _emm_cn_pdn_connectivity_fail (emm_cn_pdn_fail) #ESM information not received
      /*
       * Stop timer T3489
       */
      esm_ebr_timer_data->ctx->esm_ctx.T3489.id = NAS_TIMER_INACTIVE_ID;
      /*
       * Re-start T3489 timer
       */
      bdestroy_wrapper(&esm_ebr_timer_data->msg);
      free_wrapper((void**)esm_ebr_timer_data);
    }
  }

  OAILOG_FUNC_OUT (LOG_NAS_ESM);
}

/*
   --------------------------------------------------------------------------
                MME specific local functions
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    _esm_information()                                  **
 **                                                                        **
 ** Description: Sends DEACTIVATE EPS BEREAR CONTEXT REQUEST message and   **
 **      starts timer T3489                                        **
 **                                                                        **
 ** Inputs:  ue_id:      UE local identifier                        **
 **      ebi:       EPS bearer identity                        **
 **      msg:       Encoded ESM message to be sent             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    T3489                                      **
 **                                                                        **
 ***************************************************************************/
static int
_esm_information (
  emm_context_t * ue_context,
  ebi_t ebi,
  esm_ebr_timer_data_t * const data)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc;
  mme_ue_s1ap_id_t                        ue_id = PARENT_STRUCT(ue_context, struct ue_mm_context_s, emm_context)->mme_ue_s1ap_id;

  /*
   * Notify EMM that a deactivate EPS bearer context request message
   * has to be sent to the UE
   */
  emm_esm_data_t                         *emm_esm = &emm_sap.u.emm_esm.u.data;

  emm_sap.primitive = EMMESM_UNITDATA_REQ;
  emm_sap.u.emm_esm.ue_id = ue_id;
  emm_sap.u.emm_esm.ctx = ue_context;
  emm_esm->msg = bstrcpy(data->msg);

  MSC_LOG_TX_MESSAGE (MSC_NAS_ESM_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMESM_UNITDATA_REQ (ESM_INFORMATION_REQUEST) ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
  rc = emm_sap_send (&emm_sap);

  if (rc != RETURNerror) {
    nas_stop_T3489(&ue_context->esm_ctx);
    /*
     * Start T3489 timer
     */
    ue_context->esm_ctx.T3489.id = nas_timer_start (ue_context->esm_ctx.T3489.sec, 0 /*usec*/,_esm_information_t3489_handler, data);
    MSC_LOG_EVENT (MSC_NAS_EMM_MME, "T3489 started UE " MME_UE_S1AP_ID_FMT " ", ue_id);

    OAILOG_INFO (LOG_NAS_EMM, "UE " MME_UE_S1AP_ID_FMT "Timer T3489 (%lx) expires in %ld seconds\n",
        ue_id, ue_context->esm_ctx.T3489.id, ue_context->esm_ctx.T3489.sec);
  }else {
    bdestroy_wrapper(&data->msg);
    free_wrapper((void**)data);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}

