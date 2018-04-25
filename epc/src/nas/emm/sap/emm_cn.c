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

  Source      emm_cn.c

  Version     0.1

  Date        2013/12/05

  Product     NAS stack

  Subsystem   EPS Core Network

  Author      Sebastien Roux, Lionel GAUTHIER

  Description

*****************************************************************************/

#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "bstrlib.h"

#include "log.h"
#include "msc.h"
#include "gcc_diag.h"
#include "dynamic_memory_check.h"
#include "assertions.h"
#include "commonDef.h"
#include "common_types.h"
#include "common_defs.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"
#include "3gpp_29.274.h"
#include "mme_app_ue_context.h"
#include "emm_cn.h"
#include "emm_sap.h"
#include "emm_proc.h"
#include "emm_cause.h"

#include "esm_send.h"
#include "esm_proc.h"
#include "esm_cause.h"
#include "assertions.h"
#include "emm_data.h"
#include "esm_sap.h"
#include "3gpp_requirements_24.301.h"
#include "mme_app_defs.h"
#include "mme_app_apn_selection.h"
#include "nas_itti_messaging.h"

extern int emm_cn_wrapper_attach_accept (emm_context_t * emm_context);

static int _emm_cn_authentication_res (emm_cn_auth_res_t * const msg);
static int _emm_cn_authentication_fail (const emm_cn_auth_fail_t * msg);
static int _emm_cn_deregister_ue (const mme_ue_s1ap_id_t ue_id);
static int _emm_cn_pdn_config_res (emm_cn_pdn_config_res_t * msg_pP);
static int _emm_cn_pdn_connectivity_res (emm_cn_pdn_res_t * msg_pP);

/*
   String representation of EMMCN-SAP primitives
*/
static const char                      *_emm_cn_primitive_str[] = {
  "EMM_CN_AUTHENTICATION_PARAM_RES",
  "EMM_CN_AUTHENTICATION_PARAM_FAIL",
  "EMM_CN_DEREGISTER_UE",
  "EMM_CN_PDN_CONFIG_RES",
  "EMM_CN_PDN_CONNECTIVITY_RES",
  "EMM_CN_PDN_CONNECTIVITY_FAIL",
  "EMM_CN_ACTIVATE_DEDICATED_BEARER_REQ",
  "EMMCN_IMPLICIT_DETACH_UE",
  "EMMCN_SMC_PROC_FAIL",
};


//------------------------------------------------------------------------------
static int _emm_cn_authentication_res (emm_cn_auth_res_t * const msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_context_t                          *emm_ctx = NULL;
  int                                     rc = RETURNerror;

  /*
   * We received security vector from HSS. Try to setup security with UE
   */
  ue_mm_context_t *ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, msg->ue_id);

  if (ue_mm_context) {
    emm_ctx = &ue_mm_context->emm_context;
    nas_auth_info_proc_t * auth_info_proc = get_nas_cn_procedure_auth_info(emm_ctx);

    if (auth_info_proc) {
      for (int i = 0; i < msg->nb_vectors; i++) {
        auth_info_proc->vector[i] = msg->vector[i];
        msg->vector[i] = NULL;
      }
      auth_info_proc->nb_vectors = msg->nb_vectors;
      rc = (*auth_info_proc->success_notif)(emm_ctx);
    } else {
      OAILOG_ERROR (LOG_NAS_EMM, "EMM-PROC  - " "Failed to find Auth_info procedure associated to UE id " MME_UE_S1AP_ID_FMT "...\n", msg->ue_id);
    }
    unlock_ue_contexts(ue_mm_context);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_cn_authentication_fail (const emm_cn_auth_fail_t * msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_context_t                          *emm_ctx = NULL;
  int                                     rc = RETURNerror;

  /*
   * We received security vector from HSS. Try to setup security with UE
   */
  ue_mm_context_t *ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, msg->ue_id);

  if (ue_mm_context) {
    emm_ctx = &ue_mm_context->emm_context;
    nas_auth_info_proc_t * auth_info_proc = get_nas_cn_procedure_auth_info(emm_ctx);

    if (auth_info_proc) {
      auth_info_proc->nas_cause = msg->cause;
      rc = (*auth_info_proc->failure_notif)(emm_ctx);
    } else {
      OAILOG_ERROR (LOG_NAS_EMM, "EMM-PROC  - " "Failed to find Auth_info procedure associated to UE id " MME_UE_S1AP_ID_FMT "...\n", msg->ue_id);
    }
    unlock_ue_contexts(ue_mm_context);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_cn_smc_fail (const emm_cn_smc_fail_t * msg)
{
  int                                     rc = RETURNerror;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  rc = emm_proc_attach_reject (msg->ue_id, msg->emm_cause);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_cn_deregister_ue (const mme_ue_s1ap_id_t ue_id)
{
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_WARNING (LOG_NAS_EMM, "EMM-PROC  - " "TODO deregister UE " MME_UE_S1AP_ID_FMT ", following procedure is a test\n", ue_id);
  emm_detach_request_ies_t * params = calloc(1, sizeof(*params));
  params->type         = EMM_DETACH_TYPE_EPS;
  params->switch_off   = false;
  params->is_native_sc = false;
  params->ksi          = 0;
  emm_proc_detach_request (ue_id, params);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_cn_pdn_config_res (emm_cn_pdn_config_res_t * msg_pP)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  struct emm_context_s                   *emm_ctx = NULL;
  esm_cause_t                             esm_cause = ESM_CAUSE_SUCCESS;
  pdn_cid_t                               pdn_cid = 0;
  ebi_t                                   new_ebi = 0;
  bool                                    is_pdn_connectivity = false;

  ue_mm_context_t *ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, msg_pP->ue_id);

  if (ue_mm_context) {
    emm_ctx = &ue_mm_context->emm_context;
  }

  if (emm_ctx == NULL) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMMCN-SAP  - " "Failed to find UE associated to id " MME_UE_S1AP_ID_FMT "...\n", msg_pP->ue_id);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }

  //----------------------------------------------------------------------------
  // PDN selection here
  // Because NAS knows APN selected by UE if any
  // default APN selection
  struct apn_configuration_s* apn_config = mme_app_select_apn(ue_mm_context, emm_ctx->esm_ctx.esm_proc_data->apn);

  if (!apn_config) {
    /*
     * Unfortunately we didn't find our default APN...
     */
    OAILOG_INFO (LOG_NAS_ESM, "No suitable APN found ue_id=" MME_UE_S1AP_ID_FMT ")\n",ue_mm_context->mme_ue_s1ap_id);
    return RETURNerror;
  }

  // search for an already set PDN context
  for (pdn_cid = 0; pdn_cid < MAX_APN_PER_UE; pdn_cid++) {
    if ((ue_mm_context->pdn_contexts[pdn_cid]) && (ue_mm_context->pdn_contexts[pdn_cid]->context_identifier == apn_config->context_identifier)) {
      is_pdn_connectivity = true;
      break;
    }
  }

  if (pdn_cid >= MAX_APN_PER_UE) {
    /*
     * Search for an available PDN connection entry
     */
    for (pdn_cid = 0; pdn_cid < MAX_APN_PER_UE; pdn_cid++) {
      if (!ue_mm_context->pdn_contexts[pdn_cid]) break;
    }
  }
  if (pdn_cid < MAX_APN_PER_UE) {

    /*
     * Execute the PDN connectivity procedure requested by the UE
     */
    emm_ctx->esm_ctx.esm_proc_data->pdn_cid = pdn_cid;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.qci       = apn_config->subscribed_qos.qci;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.pci       = apn_config->subscribed_qos.allocation_retention_priority.pre_emp_capability;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.pl        = apn_config->subscribed_qos.allocation_retention_priority.priority_level;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.pvi       = apn_config->subscribed_qos.allocation_retention_priority.pre_emp_vulnerability;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.gbr.br_ul = 0;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.gbr.br_dl = 0;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.mbr.br_ul = 0;
    emm_ctx->esm_ctx.esm_proc_data->bearer_qos.mbr.br_dl = 0;
// TODO  "Better to throw emm_ctx->esm_ctx.esm_proc_data as a parameter or as a hidden parameter ?"
    rc = esm_proc_pdn_connectivity_request (emm_ctx,
        emm_ctx->esm_ctx.esm_proc_data->pti,
        emm_ctx->esm_ctx.esm_proc_data->pdn_cid,
        apn_config->context_identifier,
        emm_ctx->esm_ctx.esm_proc_data->request_type,
        emm_ctx->esm_ctx.esm_proc_data->apn,
        emm_ctx->esm_ctx.esm_proc_data->pdn_type,
        emm_ctx->esm_ctx.esm_proc_data->pdn_addr,
        &emm_ctx->esm_ctx.esm_proc_data->bearer_qos,
        (emm_ctx->esm_ctx.esm_proc_data->pco.num_protocol_or_container_id ) ? &emm_ctx->esm_ctx.esm_proc_data->pco:NULL,
        &esm_cause);

    if (rc != RETURNerror) {
      /*
       * Create local default EPS bearer context
       */
      if ((!is_pdn_connectivity) || ((is_pdn_connectivity) && (EPS_BEARER_IDENTITY_UNASSIGNED == ue_mm_context->pdn_contexts[pdn_cid]->default_ebi))) {
        rc = esm_proc_default_eps_bearer_context (emm_ctx, emm_ctx->esm_ctx.esm_proc_data->pti, pdn_cid, &new_ebi, emm_ctx->esm_ctx.esm_proc_data->bearer_qos.qci, &esm_cause);
      }

      if (rc != RETURNerror) {
        esm_cause = ESM_CAUSE_SUCCESS;
      }
    } else {
      unlock_ue_contexts(ue_mm_context);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
    }
    if (!is_pdn_connectivity) {
      nas_itti_pdn_connectivity_req (emm_ctx->esm_ctx.esm_proc_data->pti, msg_pP->ue_id, pdn_cid, &emm_ctx->_imsi,
        emm_ctx->esm_ctx.esm_proc_data, emm_ctx->esm_ctx.esm_proc_data->request_type);
    } else {

    }

    unlock_ue_contexts(ue_mm_context);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
  }
  unlock_ue_contexts(ue_mm_context);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
}

//------------------------------------------------------------------------------
static int _emm_cn_implicit_detach_ue (const uint32_t ue_id)
{
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_DEBUG (LOG_NAS_EMM, "EMM-PROC Implicit Detach UE" MME_UE_S1AP_ID_FMT "\n", ue_id);
  emm_detach_request_ies_t  params = {0};
  //params.decode_status
  //params.guti = NULL;
  //params.imei = NULL;
  //params.imsi = NULL;
  params.is_native_sc = true;
  params.ksi = 0;
  params.switch_off = true;
  params.type = EMM_DETACH_TYPE_EPS;

  emm_proc_detach_request (ue_id, &params);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_cn_pdn_connectivity_res (emm_cn_pdn_res_t * msg_pP)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  struct emm_context_s                   *emm_ctx = NULL;
  esm_proc_pdn_type_t                     esm_pdn_type = ESM_PDN_TYPE_IPV4;
  ESM_msg                                 esm_msg = {.header = {0}};
  EpsQualityOfService                     qos = {0};
  bstring                                 rsp = NULL;
  bool                                    is_standalone = false;    // warning hardcoded
  bool                                    triggered_by_ue = true;  // warning hardcoded

  ue_mm_context_t *ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, msg_pP->ue_id);

  if (ue_mm_context) {
    emm_ctx = &ue_mm_context->emm_context;
  }

  if (emm_ctx == NULL) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMMCN-SAP  - " "Failed to find UE associated to id " MME_UE_S1AP_ID_FMT "...\n", msg_pP->ue_id);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }

  memset (&esm_msg, 0, sizeof (ESM_msg));

  switch (msg_pP->pdn_type) {
  case IPv4:
    OAILOG_INFO (LOG_NAS_EMM, "EMM  -  esm_pdn_type = ESM_PDN_TYPE_IPV4\n");
    esm_pdn_type = ESM_PDN_TYPE_IPV4;
    break;

  case IPv6:
    OAILOG_INFO (LOG_NAS_EMM, "EMM  -  esm_pdn_type = ESM_PDN_TYPE_IPV6\n");
    esm_pdn_type = ESM_PDN_TYPE_IPV6;
    break;

  case IPv4_AND_v6:
    OAILOG_INFO (LOG_NAS_EMM, "EMM  -  esm_pdn_type = ESM_PDN_TYPE_IPV4V6\n");
    esm_pdn_type = ESM_PDN_TYPE_IPV4V6;
    break;

  default:
    OAILOG_INFO (LOG_NAS_EMM, "EMM  -  esm_pdn_type = ESM_PDN_TYPE_IPV4 (forced to default)\n");
    esm_pdn_type = ESM_PDN_TYPE_IPV4;
  }

  OAILOG_INFO (LOG_NAS_EMM, "EMM  -  qci       = %u \n", msg_pP->qci);
  OAILOG_INFO (LOG_NAS_EMM, "EMM  -  qos.qci   = %u \n", msg_pP->qos.qci);
  OAILOG_INFO (LOG_NAS_EMM, "EMM  -  qos.mbrUL = %u \n", msg_pP->qos.mbrUL);
  OAILOG_INFO (LOG_NAS_EMM, "EMM  -  qos.mbrDL = %u \n", msg_pP->qos.mbrDL);
  OAILOG_INFO (LOG_NAS_EMM, "EMM  -  qos.gbrUL = %u \n", msg_pP->qos.gbrUL);
  OAILOG_INFO (LOG_NAS_EMM, "EMM  -  qos.gbrDL = %u \n", msg_pP->qos.gbrDL);
  qos.bitRatesPresent = 0;
  qos.bitRatesExtPresent = 0;
//#pragma message "Some work to do here about qos"
  qos.qci = msg_pP->qci;
  qos.bitRates.maxBitRateForUL = 0;     //msg_pP->qos.mbrUL;
  qos.bitRates.maxBitRateForDL = 0;     //msg_pP->qos.mbrDL;
  qos.bitRates.guarBitRateForUL = 0;    //msg_pP->qos.gbrUL;
  qos.bitRates.guarBitRateForDL = 0;    //msg_pP->qos.gbrDL;
  qos.bitRatesExt.maxBitRateForUL = 0;
  qos.bitRatesExt.maxBitRateForDL = 0;
  qos.bitRatesExt.guarBitRateForUL = 0;
  qos.bitRatesExt.guarBitRateForDL = 0;

  int def_bearer_index = EBI_TO_INDEX(msg_pP->ebi);
  pdn_cid_t pdn_cid = ue_mm_context->bearer_contexts[def_bearer_index]->pdn_cx_id;

  /*
   * Return default EPS bearer context request message
   */
  rc = esm_send_activate_default_eps_bearer_context_request (msg_pP->pti, msg_pP->ebi,      //msg_pP->ebi,
                                                             &esm_msg.activate_default_eps_bearer_context_request,
                                                             ue_mm_context->pdn_contexts[pdn_cid]->apn_subscribed,
                                                             &msg_pP->pco,
                                                             esm_pdn_type, msg_pP->pdn_addr,
                                                             &qos, ESM_CAUSE_SUCCESS);
  clear_protocol_configuration_options(&msg_pP->pco);
  if (rc != RETURNerror) {
    /*
     * Encode the returned ESM response message
     */
    char                                    emm_cn_sap_buffer[EMM_CN_SAP_BUFFER_SIZE];
    int                                     size = esm_msg_encode (&esm_msg, (uint8_t *) emm_cn_sap_buffer,
                                                                   EMM_CN_SAP_BUFFER_SIZE);

    OAILOG_INFO (LOG_NAS_EMM, "ESM encoded MSG size %d\n", size);

    if (size > 0) {
      rsp = blk2bstr(emm_cn_sap_buffer, size);
    }

    /*
     * Complete the relevant ESM procedure
     */
    rc = esm_proc_default_eps_bearer_context_request (is_standalone, emm_ctx, msg_pP->ebi,        //0, //ESM_EBI_UNASSIGNED, //msg->ebi,
                                                      &rsp, triggered_by_ue);

    if (rc != RETURNok) {
      /*
       * Return indication that ESM procedure failed
       */
      unlock_ue_contexts(ue_mm_context);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
    }
  } else {
    OAILOG_INFO (LOG_NAS_EMM, "ESM send activate_default_eps_bearer_context_request failed\n");
  }

  /*************************************************************************/
  /*
   * END OF CODE THAT WAS IN esm_sap.c/_esm_sap_recv()
   */
  /*************************************************************************/
  nas_emm_attach_proc_t                  *attach_proc = get_nas_specific_procedure_attach(emm_ctx);

  if (attach_proc) {
    /*
     * Setup the ESM message container
     */
    attach_proc->esm_msg_out = rsp;

    /*
     * Send attach accept message to the UE
     */
    rc = emm_cn_wrapper_attach_accept (emm_ctx);

    if (rc != RETURNerror) {
      if (IS_EMM_CTXT_PRESENT_OLD_GUTI(emm_ctx) &&
          (memcmp(&emm_ctx->_old_guti, &emm_ctx->_guti, sizeof(emm_ctx->_guti)))) {
        /*
         * Implicit GUTI reallocation;
         * Notify EMM that common procedure has been initiated
         * LG: TODO check this, seems very suspicious
         */
        emm_sap_t                               emm_sap = {0};

        emm_sap.primitive = EMMREG_COMMON_PROC_REQ;
        emm_sap.u.emm_reg.ue_id = msg_pP->ue_id;
        emm_sap.u.emm_reg.ctx  = emm_ctx;

        MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMREG_COMMON_PROC_REQ ue id " MME_UE_S1AP_ID_FMT " ", msg_pP->ue_id);

        rc = emm_sap_send (&emm_sap);
      }
    }
  }
  unlock_ue_contexts(ue_mm_context);

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_cn_pdn_connectivity_fail (const emm_cn_pdn_fail_t * msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;
  struct emm_context_s                   *emm_ctx_p = NULL;
  ESM_msg                                 esm_msg = {.header = {0}};
  int                                     esm_cause; 
  emm_ctx_p = emm_context_get (&_emm_data, msg->ue_id);
  if (emm_ctx_p == NULL) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMMCN-SAP  - " "Failed to find UE associated to id " MME_UE_S1AP_ID_FMT "...\n", msg->ue_id);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }
  memset (&esm_msg, 0, sizeof (ESM_msg));
  
  // Map S11 cause to ESM cause
  switch (msg->cause) {
    case CAUSE_CONTEXT_NOT_FOUND:
      esm_cause = ESM_CAUSE_REQUEST_REJECTED_BY_GW; 
      break;
    case CAUSE_INVALID_MESSAGE_FORMAT:
      esm_cause = ESM_CAUSE_REQUEST_REJECTED_BY_GW; 
      break;
    case CAUSE_SERVICE_NOT_SUPPORTED:                  
      esm_cause = ESM_CAUSE_SERVICE_OPTION_NOT_SUPPORTED;
      break;
    case CAUSE_SYSTEM_FAILURE:                        
      esm_cause = ESM_CAUSE_NETWORK_FAILURE; 
      break;
    case CAUSE_NO_RESOURCES_AVAILABLE:           
      esm_cause = ESM_CAUSE_INSUFFICIENT_RESOURCES; 
      break;
    case CAUSE_ALL_DYNAMIC_ADDRESSES_OCCUPIED:  
      esm_cause = ESM_CAUSE_INSUFFICIENT_RESOURCES;
      break;
    default: 
      esm_cause = ESM_CAUSE_REQUEST_REJECTED_BY_GW; 
      break;
  }

  rc = esm_send_pdn_connectivity_reject (msg->pti, &esm_msg.pdn_connectivity_reject, esm_cause);
  /*
   * Encode the returned ESM response message
   */
  uint8_t                             emm_cn_sap_buffer[EMM_CN_SAP_BUFFER_SIZE];
  int size = esm_msg_encode (&esm_msg, emm_cn_sap_buffer, EMM_CN_SAP_BUFFER_SIZE);
  OAILOG_INFO (LOG_NAS_EMM, "ESM encoded MSG size %d\n", size);

  if (size > 0) {
    nas_emm_attach_proc_t  *attach_proc = get_nas_specific_procedure_attach(emm_ctx_p);
    /*
     * Setup the ESM message container
     */
    attach_proc->esm_msg_out = blk2bstr(emm_cn_sap_buffer, size);
    rc = emm_proc_attach_reject (msg->ue_id, EMM_CAUSE_ESM_FAILURE);
  }
  emm_context_unlock(emm_ctx_p);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
static int _emm_cn_activate_dedicated_bearer_req (emm_cn_activate_dedicated_bearer_req_t * msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;
  // forward to ESM
  esm_sap_t                               esm_sap = {0};

  ue_mm_context_t *ue_mm_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, msg->ue_id);

  esm_sap.primitive = ESM_DEDICATED_EPS_BEARER_CONTEXT_ACTIVATE_REQ;
  esm_sap.ctx           = &ue_mm_context->emm_context;
  esm_sap.is_standalone = true;
  esm_sap.ue_id         = msg->ue_id;
  esm_sap.data.eps_dedicated_bearer_context_activate.cid         = msg->cid;
  esm_sap.data.eps_dedicated_bearer_context_activate.ebi         = msg->ebi;
  esm_sap.data.eps_dedicated_bearer_context_activate.linked_ebi  = msg->linked_ebi;
  esm_sap.data.eps_dedicated_bearer_context_activate.tft         = msg->tft;
  esm_sap.data.eps_dedicated_bearer_context_activate.qci         = msg->bearer_qos.qci;
  esm_sap.data.eps_dedicated_bearer_context_activate.gbr_ul      = msg->bearer_qos.gbr.br_ul;
  esm_sap.data.eps_dedicated_bearer_context_activate.gbr_dl      = msg->bearer_qos.gbr.br_dl;
  esm_sap.data.eps_dedicated_bearer_context_activate.mbr_ul      = msg->bearer_qos.mbr.br_ul;
  esm_sap.data.eps_dedicated_bearer_context_activate.mbr_dl      = msg->bearer_qos.mbr.br_dl;
  // stole ref if any
  msg->tft = NULL;
  esm_sap.data.eps_dedicated_bearer_context_activate.pco         = msg->pco;
  // stole ref if any
  msg->pco = NULL;

  MSC_LOG_TX_MESSAGE (MSC_NAS_EMM_MME, MSC_NAS_ESM_MME, NULL, 0, "0 ESM_DEDICATED_EPS_BEARER_CONTEXT_ACTIVATE_REQ ue id " MME_UE_S1AP_ID_FMT " ebi %u",
      esm_sap.ue_id,esm_sap.data.eps_dedicated_bearer_context_activate.ebi);

  rc = esm_sap_send (&esm_sap);

  unlock_ue_contexts(ue_mm_context);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int emm_cn_send (const emm_cn_t * msg)
{
  int                                     rc = RETURNerror;
  emm_cn_primitive_t                      primitive = msg->primitive;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMMCN-SAP - Received primitive %s (%d)\n", _emm_cn_primitive_str[primitive - _EMMCN_START - 1], primitive);

  switch (primitive) {
  case _EMMCN_AUTHENTICATION_PARAM_RES:
    rc = _emm_cn_authentication_res (msg->u.auth_res);
    break;

  case _EMMCN_AUTHENTICATION_PARAM_FAIL:
    rc = _emm_cn_authentication_fail (msg->u.auth_fail);
    break;

  case EMMCN_DEREGISTER_UE:
    rc = _emm_cn_deregister_ue (msg->u.deregister.ue_id);
    break;

  case EMMCN_PDN_CONFIG_RES:
    rc = _emm_cn_pdn_config_res (msg->u.emm_cn_pdn_config_res);
    break;

  case EMMCN_PDN_CONNECTIVITY_RES:
    rc = _emm_cn_pdn_connectivity_res (msg->u.emm_cn_pdn_res);
    break;

  case EMMCN_PDN_CONNECTIVITY_FAIL:
    rc = _emm_cn_pdn_connectivity_fail (msg->u.emm_cn_pdn_fail);
    break;
  
  case EMMCN_ACTIVATE_DEDICATED_BEARER_REQ:
    rc = _emm_cn_activate_dedicated_bearer_req (msg->u.activate_dedicated_bearer_req);
    break;

  case EMMCN_IMPLICIT_DETACH_UE:
    rc = _emm_cn_implicit_detach_ue (msg->u.emm_cn_implicit_detach.ue_id);
    break;

  case EMMCN_SMC_PROC_FAIL:
    rc = _emm_cn_smc_fail (msg->u.smc_fail);
    break;

  default:
    /*
     * Other primitives are forwarded to the Access Stratum
     */
    rc = RETURNerror;
    break;
  }

  if (rc != RETURNok) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMMCN-SAP - Failed to process primitive %s (%d)\n", _emm_cn_primitive_str[primitive - _EMMCN_START - 1], primitive);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}
