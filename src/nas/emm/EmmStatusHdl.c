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
  Source      Emmstatus.c

  Version     0.1

  Date        2013/06/26

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines the EMM status procedure executed by the Non-Access
        Stratum.

        The purpose of the sending of the EMM STATUS message is to
        report at any time certain error conditions detected upon
        receipt of EMM protocol data. The EMM STATUS message can be
        sent by both the MME and the UE.

*****************************************************************************/

#include "emm_proc.h"
#include "commonDef.h"
#include "log.h"

#include "emm_cause.h"
#include "emmData.h"

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
 ** Name:    emm_proc_status_ind()                                     **
 **                                                                        **
 ** Description: Processes received EMM status message.                    **
 **                                                                        **
 **      3GPP TS 24.301, section 5.7                               **
 **      On receipt of an EMM STATUS message no state transition   **
 **      and no specific action shall be taken. Local actions are  **
 **      possible and are implementation dependent.                **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **          emm_cause: Received EMM cause code                    **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_proc_status_ind (
  mme_ue_s1ap_id_t ue_id,
  int emm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - EMM status procedure requested (cause=%d)", emm_cause);
  OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC  - To be implemented");
  /*
   * TODO
   */
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_proc_status()                                         **
 **                                                                        **
 ** Description: Initiates EMM status procedure.                           **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      emm_cause: EMM cause code to be reported              **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_proc_status (
  mme_ue_s1ap_id_t ue_id,
  int emm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc;
  emm_sap_t                               emm_sap = {0};
  emm_security_context_t                 *sctx = NULL;
  struct emm_data_context_s              *ctx = NULL;

  OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - EMM status procedure requested");
  /*
   * Notity EMM that EMM status indication has to be sent to lower layers
   */
  emm_sap.primitive = EMMAS_STATUS_IND;
  emm_sap.u.emm_as.u.status.emm_cause = emm_cause;
  emm_sap.u.emm_as.u.status.ue_id = ue_id;
  emm_sap.u.emm_as.u.status.guti = NULL;
  ctx = emm_data_context_get (&_emm_data, ue_id);

  if (ctx) {
    sctx = &ctx->_security;
  }

  /*
   * Setup EPS NAS security data
   */
  emm_as_set_security_data (&emm_sap.u.emm_as.u.status.sctx, sctx, false, true);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
