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
  Source      PdnDisconnect.c

  Version     0.1

  Date        2013/05/15

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Frederic Maurel

  Description Defines the PDN disconnect ESM procedure executed by the
        Non-Access Stratum.

        The PDN disconnect procedure is used by the UE to request
        disconnection from one PDN.

        All EPS bearer contexts established towards this PDN, inclu-
        ding the default EPS bearer context, are released.

*****************************************************************************/

#include "3gpp_24.007.h"
#include "esm_proc.h"
#include "commonDef.h"
#include "log.h"

#include "esmData.h"
#include "esm_cause.h"
#include "esm_pt.h"
#include "emm_sap.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

extern int _pdn_connectivity_delete (emm_data_context_t * ctx, int pid);

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/


/*
   --------------------------------------------------------------------------
    Internal data handled by the PDN disconnect procedure in the MME
   --------------------------------------------------------------------------
*/
/*
   PDN disconnection handlers
*/
static int _pdn_disconnect_get_pid (esm_data_context_t * ctx, int pti);


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/



/*
   --------------------------------------------------------------------------
          PDN disconnect procedure executed by the MME
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_pdn_disconnect_request()                         **
 **                                                                        **
 ** Description: Performs PDN disconnect procedure requested by the UE.    **
 **                                                                        **
 **              3GPP TS 24.301, section 6.5.2.3                           **
 **      Upon receipt of the PDN DISCONNECT REQUEST message, if it **
 **      is accepted by the network, the MME shall initiate the    **
 **      bearer context deactivation procedure.                    **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      pti:       Identifies the PDN disconnect procedure    **
 **             requested by the UE                        **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     esm_cause: Cause code returned upon ESM procedure     **
 **             failure                                    **
 **      Return:    The identifier of the PDN connection to be **
 **             released, if it exists;                    **
 **             RETURNerror otherwise.                     **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_pdn_disconnect_request (
  emm_data_context_t * ctx,
  int pti,
  int *esm_cause)
{
  int                                     pid = RETURNerror;

  OAILOG_FUNC_IN (LOG_NAS_ESM);
  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - PDN disconnect requested by the UE " "(ue_id=" MME_UE_S1AP_ID_FMT ", pti=%d)\n", ctx->ue_id, pti);

  /*
   * Get UE's ESM context
   */
  if (ctx->esm_data_ctx.n_pdns > 1) {
    /*
     * Get the identifier of the PDN connection entry assigned to the
     * * * * procedure transaction identity
     */
    pid = _pdn_disconnect_get_pid (&ctx->esm_data_ctx, pti);

    if (pid < 0) {
      OAILOG_ERROR (LOG_NAS_ESM, "ESM-PROC  - No PDN connection found (pti=%d)\n", pti);
      *esm_cause = ESM_CAUSE_PROTOCOL_ERROR;
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
    }
  } else {
    /*
     * Attempt to disconnect from the last PDN disconnection
     * * * * is not allowed
     */
    *esm_cause = ESM_CAUSE_LAST_PDN_DISCONNECTION_NOT_ALLOWED;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, pid);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_pdn_disconnect_accept()                          **
 **                                                                        **
 ** Description: Performs PDN disconnect procedure accepted by the UE.     **
 **                                                                        **
 **              3GPP TS 24.301, section 6.5.2.3                           **
 **      On reception of DEACTIVATE EPS BEARER CONTEXT ACCEPT mes- **
 **      sage from the UE, the MME releases all the resources re-  **
 **      served for the PDN in the network.                        **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      pid:       Identifier of the PDN connection to be     **
 **             released                                   **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     esm_cause: Cause code returned upon ESM procedure     **
 **             failure                                    **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_pdn_disconnect_accept (
  emm_data_context_t * ctx,
  int pid,
  int *esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - PDN disconnect accepted by the UE " "(ue_id=" MME_UE_S1AP_ID_FMT ", pid=%d)\n", ctx->ue_id, pid);
  /*
   * Release the connectivity with the requested PDN
   */
  int                                     rc = mme_api_unsubscribe (NULL);

  if (rc != RETURNerror) {
    /*
     * Delete the PDN connection entry
     */
    int                                     pti = _pdn_connectivity_delete (ctx, pid);

    if (pti != ESM_PT_UNASSIGNED) {
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
    }
  }

  *esm_cause = ESM_CAUSE_PROTOCOL_ERROR;
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_pdn_disconnect_reject()                          **
 **                                                                        **
 ** Description: Performs PDN disconnect procedure not accepted by the     **
 **      network.                                                  **
 **                                                                        **
 **              3GPP TS 24.301, section 6.5.2.4                           **
 **      Upon receipt of the PDN DISCONNECT REQUEST message, if it **
 **      is not accepted by the network, the MME shall send a PDN  **
 **      DISCONNECT REJECT message to the UE.                      **
 **                                                                        **
 ** Inputs:  is_standalone: Not used - Always true                     **
 **      ue_id:      UE lower layer identifier                  **
 **      ebi:       Not used                                   **
 **      msg:       Encoded PDN disconnect reject message to   **
 **             be sent                                    **
 **      ue_triggered:  Not used                                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_pdn_disconnect_reject (
  const bool is_standalone,
  emm_data_context_t * ctx,
  int ebi,
  bstring msg,
  const bool ue_triggered)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc;
  emm_sap_t                               emm_sap = {0};

  OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - PDN disconnect not accepted by the network " "(ue_id=" MME_UE_S1AP_ID_FMT ")\n", ctx->ue_id);
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

/*
   --------------------------------------------------------------------------
                Timer handlers
   --------------------------------------------------------------------------
*/


/*
  ---------------------------------------------------------------------------
                PDN disconnection handlers
  ---------------------------------------------------------------------------
*/


/****************************************************************************
 **                                                                        **
 ** Name:    _pdn_disconnect_get_pid()                                 **
 **                                                                        **
 ** Description: Returns the identifier of the PDN connection to which the **
 **      given procedure transaction identity has been assigned    **
 **      to establish connectivity to the specified UE             **
 **                                                                        **
 ** Inputs:  ue_id:      UE local identifier                        **
 **      pti:       The procedure transaction identity         **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The identifier of the PDN connection if    **
 **             found in the list; -1 otherwise.           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int
_pdn_disconnect_get_pid (
  esm_data_context_t * ctx,
  int pti)
{
  int                                     i = ESM_DATA_PDN_MAX;

  if (ctx ) {
    for (i = 0; i < ESM_DATA_PDN_MAX; i++) {
      if ((ctx->pdn[i].pid != -1) && (ctx->pdn[i].data )) {
        if (ctx->pdn[i].data->pti != pti) {
          continue;
        }

        /*
         * PDN entry found
         */
        break;
      }
    }
  }

  /*
   * Return the identifier of the PDN connection
   */
  return (ctx->pdn[i].pid);
}
