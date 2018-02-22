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
  Source      PdnConnectivity.c

  Version     0.1

  Date        2013/01/02

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Frederic Maurel

  Description Defines the PDN connectivity ESM procedure executed by the
        Non-Access Stratum.

        The PDN connectivity procedure is used by the UE to request
        the setup of a default EPS bearer to a PDN.

        The procedure is used either to establish the 1st default
        bearer by including the PDN CONNECTIVITY REQUEST message
        into the initial attach message, or to establish subsequent
        default bearers to additional PDNs in order to allow the UE
        simultaneous access to multiple PDNs by sending the message
        stand-alone.

*****************************************************************************/

#include <stdlib.h>             // malloc, free_wrapper
#include <string.h>             // memset, memcpy, memcmp
#include <ctype.h>              // isprint

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "log.h"
#include "3gpp_24.007.h"
#include "commonDef.h"
#include "esm_proc.h"
#include "esmData.h"
#include "esm_cause.h"
#include "esm_pt.h"
#include "mme_api.h"
#include "emm_sap.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/



/*
   --------------------------------------------------------------------------
    Internal data handled by the PDN connectivity procedure in the MME
   --------------------------------------------------------------------------
*/
/*
   PDN connection handlers
*/
static int _pdn_connectivity_create (
  emm_data_context_t * ctx,
  const int pti,
  const_bstring const apn,
  esm_proc_pdn_type_t pdn_type,
  const_bstring const pdn_addr,
  const int is_emergency);

int _pdn_connectivity_delete (emm_data_context_t * ctx, int pid);


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/


/*
   --------------------------------------------------------------------------
        PDN connectivity procedure executed by the MME
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_pdn_connectivity_request()                       **
 **                                                                        **
 ** Description: Performs PDN connectivity procedure requested by the UE.  **
 **                                                                        **
 **              3GPP TS 24.301, section 6.5.1.3                           **
 **      Upon receipt of the PDN CONNECTIVITY REQUEST message, the **
 **      MME checks if connectivity with the requested PDN can be  **
 **      established. If no requested  APN  is provided  the  MME  **
 **      shall use the default APN as the  requested  APN if the   **
 **      request type is different from "emergency", or the APN    **
 **      configured for emergency bearer services if the request   **
 **      type is "emergency".                                      **
 **      If connectivity with the requested PDN is accepted by the **
 **      network, the MME shall initiate the default EPS bearer    **
 **      context activation procedure.                             **
 **                                                                        **
 ** Inputs:  ue_id:      UE local identifier                        **
 **      pti:       Identifies the PDN connectivity procedure  **
 **             requested by the UE                        **
 **      request_type:  Type of the PDN request                    **
 **      pdn_type:  PDN type value (IPv4, IPv6, IPv4v6)        **
 **      apn:       Requested Access Point Name                **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     apn:       Default Access Point Name                  **
 **      pdn_addr:  Assigned IPv4 address and/or IPv6 suffix   **
 **      esm_qos:   EPS bearer level QoS parameters            **
 **      esm_cause: Cause code returned upon ESM procedure     **
 **             failure                                    **
 **      Return:    The identifier of the PDN connection if    **
 **             successfully created;                      **
 **             RETURNerror otherwise.                     **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_pdn_connectivity_request (
  emm_data_context_t * ctx,
  const int pti,
  const esm_proc_pdn_request_t request_type,
  const_bstring const apn,
  esm_proc_pdn_type_t pdn_type,
  const_bstring const pdn_addr,
  esm_proc_qos_t * esm_qos,
  int *esm_cause)
{
  int                                     rc = RETURNerror;
  int                                     pid = RETURNerror;

  OAILOG_FUNC_IN (LOG_NAS_ESM);
  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - PDN connectivity requested by the UE "
             "(ue_id=" MME_UE_S1AP_ID_FMT ", pti=%d) PDN type = %s, APN = %s pdn addr = %s\n", ctx->ue_id, pti,
             (pdn_type == ESM_PDN_TYPE_IPV4) ? "IPv4" : (pdn_type == ESM_PDN_TYPE_IPV6) ? "IPv6" : "IPv4v6",
             (apn) ? (char *)bdata(apn) : "null",
             (pdn_addr) ? (char *)bdata(pdn_addr) : "null");

  /*
   * Check network IP capabilities
   */
  *esm_cause = ESM_CAUSE_SUCCESS;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - _esm_data.conf.features %08x\n", _esm_data.conf.features);
//#pragma message  "Uncomment code about _esm_data.conf.features & (MME_API_IPV4 | MME_API_IPV6) later"
#if ORIGINAL_CODE

  switch (_esm_data.conf.features & (MME_API_IPV4 | MME_API_IPV6)) {
  case (MME_API_IPV4 | MME_API_IPV6):

    /*
     * The network supports both IPv4 and IPv6 connection
     */
    if ((pdn_type == ESM_PDN_TYPE_IPV4V6) && (_esm_data.conf.features & MME_API_SINGLE_ADDR_BEARERS)) {
      /*
       * The network supports single IP version bearers only
       */
      *esm_cause = ESM_CAUSE_SINGLE_ADDRESS_BEARERS_ONLY_ALLOWED;
    }

    rc = RETURNok;
    break;

  case MME_API_IPV6:
    /*
     * The network supports connection to IPv6 only
     */
    *esm_cause = ESM_CAUSE_PDN_TYPE_IPV6_ONLY_ALLOWED;

    if (pdn_type != ESM_PDN_TYPE_IPV4) {
      rc = RETURNok;
    }

    break;

  case MME_API_IPV4:
    /*
     * The network supports connection to IPv4 only
     */
    *esm_cause = ESM_CAUSE_PDN_TYPE_IPV4_ONLY_ALLOWED;

    if (pdn_type != ESM_PDN_TYPE_IPV6) {
      rc = RETURNok;
    }

    break;

  default:
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-PROC  - _esm_data.conf.features incorrect value (no IPV4 or IPV6 ) %X\n", _esm_data.conf.features);
  }

#else
  rc = RETURNok;
#endif

  if (rc != RETURNerror) {
    int                                     is_emergency = (request_type == ESM_PDN_REQUEST_EMERGENCY);

#if ORIGINAL_CODE
    mme_api_ip_version_t                    mme_pdn_index;
    mme_api_qos_t                           qos;

    switch (pdn_type) {
    case ESM_PDN_TYPE_IPV4:
      mme_pdn_index = MME_API_IPV4_ADDR;
      break;

    case ESM_PDN_TYPE_IPV6:
      mme_pdn_index = MME_API_IPV6_ADDR;
      break;

    case ESM_PDN_TYPE_IPV4V6:
    default:
      mme_pdn_index = MME_API_IPV4V6_ADDR;
      break;
    }

    /*
     * Check if connectivity with the requested PDN can be established
     */
    rc = mme_api_subscribe (apn, mme_pdn_index, pdn_addr, is_emergency, &qos);

    if (rc != RETURNok) {
      OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - Connectivity to the requested PDN " "cannot be established\n");
      *esm_cause = ESM_CAUSE_REQUEST_REJECTED_UNSPECIFIED;
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
    }
#endif
    /*
     * Create new PDN connection
     */
    pid = _pdn_connectivity_create (ctx, pti, apn, pdn_type, pdn_addr, is_emergency);
#if ORIGINAL_CODE

    /*
     * Setup ESM QoS parameters
     */
    if (esm_qos) {
      esm_qos->gbrUL = qos.gbr[MME_API_UPLINK];
      esm_qos->gbrDL = qos.gbr[MME_API_DOWNLINK];
      esm_qos->mbrUL = qos.mbr[MME_API_UPLINK];
      esm_qos->mbrDL = qos.mbr[MME_API_DOWNLINK];
      esm_qos->qci = qos.qci;
    }
#endif

    if (pid < 0) {
      OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - Failed to create PDN connection\n");
      *esm_cause = ESM_CAUSE_INSUFFICIENT_RESOURCES;
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, pid);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_proc_pdn_connectivity_reject()                        **
 **                                                                        **
 ** Description: Performs PDN connectivity procedure not accepted by the   **
 **      network.                                                  **
 **                                                                        **
 **              3GPP TS 24.301, section 6.5.1.4                           **
 **      If connectivity with the requested PDN cannot be accepted **
 **      by the network, the MME shall send a PDN CONNECTIVITY RE- **
 **      JECT message to the UE.                                   **
 **                                                                        **
 ** Inputs:  is_standalone: Indicates whether the PDN connectivity     **
 **             procedure was initiated as part of the at- **
 **             tach procedure                             **
 **      ue_id:      UE lower layer identifier                  **
 **      ebi:       Not used                                   **
 **      msg:       Encoded PDN connectivity reject message to **
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
esm_proc_pdn_connectivity_reject (
  bool is_standalone,
  emm_data_context_t * ctx,
  int ebi,
  bstring msg,
  bool ue_triggered)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  int                                     rc = RETURNerror;

  OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - PDN connectivity not accepted by the " "network (ue_id=" MME_UE_S1AP_ID_FMT ")\n", ctx->ue_id);

  if (is_standalone) {
    emm_sap_t                               emm_sap = {0};

    /*
     * Notity EMM that ESM PDU has to be forwarded to lower layers
     */
    emm_sap.primitive = EMMESM_UNITDATA_REQ;
    emm_sap.u.emm_esm.ctx = ctx;
    emm_sap.u.emm_esm.u.data.msg = msg;
    rc = emm_sap_send (&emm_sap);
  }

  /*
   * If the PDN connectivity procedure initiated as part of the initial
   * * * * attach procedure has failed, an error is returned to notify EMM that
   * * * * the ESM sublayer did not accept UE requested PDN connectivity
   */
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:        esm_proc_pdn_connectivity_failure()                       **
 **                                                                        **
 ** Description: Performs PDN connectivity procedure upon receiving noti-  **
 **              fication from the EPS Mobility Management sublayer that   **
 **              EMM procedure that initiated PDN connectivity activation  **
 **              locally failed.                                           **
 **                                                                        **
 **              The MME releases the PDN connection entry allocated when  **
 **              the PDN connectivity procedure was requested by the UE.   **
 **                                                                        **
 **         Inputs:  ue_id:      UE local identifier                        **
 **                  pid:       Identifier of the PDN connection to be     **
 **                             released                                   **
 **                  Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **                  Return:    RETURNok, RETURNerror                      **
 **                  Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_proc_pdn_connectivity_failure (
  emm_data_context_t * ctx,
  int pid)
{
  int                                     pti;

  OAILOG_FUNC_IN (LOG_NAS_ESM);
  OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - PDN connectivity failure (ue_id=" MME_UE_S1AP_ID_FMT ", pid=%d)\n", ctx->ue_id, pid);
  /*
   * Delete the PDN connection entry
   */
  pti = _pdn_connectivity_delete (ctx, pid);

  if (pti != ESM_PT_UNASSIGNED) {
    OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNerror);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/


/*
  ---------------------------------------------------------------------------
                PDN connection handlers
  ---------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:        _pdn_connectivity_create()                                **
 **                                                                        **
 ** Description: Creates a new PDN connection entry for the specified UE   **
 **                                                                        **
 ** Inputs:          ue_id:      UE local identifier                        **
 **                  ctx:       UE context                                 **
 **                  pti:       Procedure transaction identity             **
 **                  apn:       Access Point Name of the PDN connection    **
 **                  pdn_type:  PDN type (IPv4, IPv6, IPv4v6)              **
 **                  pdn_addr:  Network allocated PDN IPv4 or IPv6 address **
 **              is_emergency:  true if the PDN connection has to be esta- **
 **                             blished for emergency bearer services      **
 **                  Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **                  Return:    The identifier of the PDN connection if    **
 **                             successfully created; -1 otherwise.        **
 **                  Others:    _esm_data                                  **
 **                                                                        **
 ***************************************************************************/
static int
_pdn_connectivity_create (
  emm_data_context_t * ctx,
  const int pti,
  const_bstring const apn,
  esm_proc_pdn_type_t pdn_type,
  const_bstring const pdn_addr,
  const int is_emergency)
{
  int                                     pid = ESM_DATA_PDN_MAX;

  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - Create new PDN connection "
             "(pti=%d) APN = %s, IP address = %s (ue_id=" MME_UE_S1AP_ID_FMT ")\n", pti, bdata(apn),
             (pdn_type == ESM_PDN_TYPE_IPV4) ? esm_data_get_ipv4_addr (pdn_addr) : (pdn_type == ESM_PDN_TYPE_IPV6) ? esm_data_get_ipv6_addr (pdn_addr) : esm_data_get_ipv4v6_addr (pdn_addr), ctx->ue_id);

  /*
   * Search for an available PDN connection entry
   */
  for (pid = 0; pid < ESM_DATA_PDN_MAX; pid++) {
    if (ctx->esm_data_ctx.pdn[pid].data ) {
      continue;
    }

    break;
  }

  if (pid < ESM_DATA_PDN_MAX) {
    /*
     * Create new PDN connection
     */
    esm_pdn_t                              *pdn = (esm_pdn_t *) malloc (sizeof (esm_pdn_t));

    if (pdn ) {
      memset (pdn, 0, sizeof (esm_pdn_t));
      /*
       * Increment the number of PDN connections
       */
      ctx->esm_data_ctx.n_pdns += 1;
      /*
       * Set the PDN connection identifier
       */
      ctx->esm_data_ctx.pdn[pid].pid = pid;
      /*
       * Reset the PDN connection active indicator
       */
      ctx->esm_data_ctx.pdn[pid].is_active = false;
      /*
       * Setup the PDN connection data
       */
      ctx->esm_data_ctx.pdn[pid].data = pdn;
      /*
       * Set the procedure transaction identity
       */
      pdn->pti = pti;
      /*
       * Set the emergency bearer services indicator
       */
      pdn->is_emergency = is_emergency;

      /*
       * Setup the Access Point Name
       */
      if (apn) {
        pdn->apn = bstrcpy(apn);
      }

      /*
       * Setup the IP address allocated by the network
       */
      if (pdn_addr) {
        int length = ((blength(pdn_addr) < ESM_DATA_IP_ADDRESS_SIZE) ? blength(pdn_addr) : ESM_DATA_IP_ADDRESS_SIZE);

        memcpy (pdn->ip_addr, pdn_addr->data, length);
        pdn->type = pdn_type;
      }

      /*
       * Return the identifier of the new PDN connection
       */
      return (ctx->esm_data_ctx.pdn[pid].pid);
    }

    OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - Failed to create new PDN connection " "(pid=%d)\n", pid);
  }

  return (-1);
}

/****************************************************************************
 **                                                                        **
 ** Name:        _pdn_connectivity_delete()                                **
 **                                                                        **
 ** Description: Deletes PDN connection to the specified UE associated to  **
 **              PDN connection entry with given identifier                **
 **                                                                        **
 ** Inputs:          ue_id:      UE local identifier                        **
 **                  pid:       Identifier of the PDN connection to be     **
 **                             released                                   **
 **                  Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **                  Return:    The identity of the procedure transaction  **
 **                             assigned to the PDN connection when suc-   **
 **                             cessfully released;                        **
 **                             UNASSIGNED value otherwise.                **
 **                  Others:    _esm_data                                  **
 **                                                                        **
 ***************************************************************************/
int
_pdn_connectivity_delete (
  emm_data_context_t * ctx,
  int pid)
{
  int                                     pti = ESM_PT_UNASSIGNED;

  if (ctx == NULL || pid >= ESM_DATA_PDN_MAX) {
    return pti;
  }
  if (ctx->esm_data_ctx.pdn[pid].data == NULL) {
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-PROC  - PDN connection identifier is not valid\n");
    return pti;
  }
  if (pid != ctx->esm_data_ctx.pdn[pid].pid) {
    OAILOG_ERROR (LOG_NAS_ESM, "ESM-PROC  - PDN connection identifier is not valid\n");
    return pti;
  } 
  if (ctx->esm_data_ctx.pdn[pid].is_active) {
      OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - PDN connection is active\n");
  } 
  /*
   * Get the identity of the procedure transaction that created
   *  the PDN connection
   */
  pti = ctx->esm_data_ctx.pdn[pid].data->pti;
  
  /*
   * Decrement the number of PDN connections
   */
  ctx->esm_data_ctx.n_pdns -= 1;
  
  /*
   * Set the PDN connection as available
   */
  
  ctx->esm_data_ctx.pdn[pid].pid = -1;
    
  ctx->esm_data_ctx.pdn[pid].is_active = false;
    
  /*
   * Release allocated PDN connection data
   */
  if (ctx->esm_data_ctx.pdn[pid].data->apn) {
    bdestroy (ctx->esm_data_ctx.pdn[pid].data->apn);
  }

  free_wrapper ((void**) &ctx->esm_data_ctx.pdn[pid].data);
  ctx->esm_data_ctx.pdn[pid].data = NULL;
  OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - PDN connection %d released\n", pid);

  /*
   * Return the procedure transaction identity
   */
  return (pti);
}
