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
  Source      EsmStatus.c

  Version     0.1

  Date        2013/06/17

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Frederic Maurel

  Description Defines the ESM status procedure executed by the Non-Access
        Stratum.

        ESM status procedure can be related to an EPS bearer context
        or to a procedure transaction.

        The purpose of the sending of the ESM STATUS message is to
        report at any time certain error conditions detected upon
        receipt of ESM protocol data. The ESM STATUS message can be
        sent by both the MME and the UE.

*****************************************************************************/

#include "3gpp_24.007.h"
#include "esm_proc.h"
#include "commonDef.h"
#include "log.h"

#include "esm_cause.h"

#include "emm_sap.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_status_ind()                                     **
 **                                                                        **
 ** Description: Processes received ESM status message.                    **
 **                                                                        **
 **      3GPP TS 24.301, section 6.7                               **
 **      Upon receiving ESM Status message the UE/MME shall take   **
 **      different actions depending on the received ESM cause     **
 **      value.                                                    **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      esm_cause: Received ESM cause code                    **
 **             failure                                    **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     esm_cause: Cause code returned upon ESM procedure     **
 **             failure                                    **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_status_ind (
  emm_data_context_t * ctx,
  int pti,
  int ebi,
  int *esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc;

  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - ESM status procedure requested (cause=%d)\n", *esm_cause);
  OAILOG_DEBUG (LOG_NAS_ESM, "ESM-PROC  - To be implemented\n");

  switch (*esm_cause) {
  case ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY:
    /*
     * Abort any ongoing ESM procedure related to the received EPS
     * bearer identity, stop any related timer, and deactivate the
     * corresponding EPS bearer context locally
     */
    /*
     * TODO
     */
    rc = RETURNok;
    break;

  case ESM_CAUSE_INVALID_PTI_VALUE:
    /*
     * Abort any ongoing ESM procedure related to the received PTI
     * value and stop any related timer
     */
    /*
     * TODO
     */
    rc = RETURNok;
    break;

  case ESM_CAUSE_MESSAGE_TYPE_NOT_IMPLEMENTED:
    /*
     * Abort any ongoing ESM procedure related to the PTI or
     * EPS bearer identity and stop any related timer
     */
    /*
     * TODO
     */
    rc = RETURNok;
    break;

  default:
    /*
     * No state transition and no specific action shall be taken;
     * local actions are possible
     */
    /*
     * TODO
     */
    rc = RETURNok;
    break;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_status()                                         **
 **                                                                        **
 ** Description: Initiates ESM status procedure.                           **
 **                                                                        **
 ** Inputs:  is_standalone: Not used - Always true                     **
 **      ue_id:      UE lower layer identifier                  **
 **      ebi:       Not used                                   **
 **      msg:       Encoded ESM status message to be sent      **
 **      ue_triggered:  Not used                                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_status (
  bool is_standalone,
  emm_data_context_t * ctx,
  int ebi,
  bstring msg,
  bool ue_triggered)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc;
  emm_sap_t                               emm_sap = {0};

  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - ESM status procedure requested\n");
  /*
   * Notity EMM that ESM PDU has to be forwarded to lower layers
   */
  emm_sap.primitive = EMMESM_UNITDATA_REQ;
  emm_sap.u.emm_esm.ue_id = ctx->ue_id;
  emm_sap.u.emm_esm.ctx = ctx;
  emm_sap.u.emm_esm.u.data.msg = msg;
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
