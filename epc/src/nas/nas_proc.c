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
  Source      nas_proc.c

  Version     0.1

  Date        2012/09/20

  Product     NAS stack

  Subsystem   NAS main process

  Author      Frederic Maurel, Lionel GAUTHIER

  Description NAS procedure call manager

*****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#include "bstrlib.h"

#include "log.h"
#include "msc.h"
#include "assertions.h"
#include "conversions.h"
#include "nas_proc.h"
#include "emm_main.h"
#include "emm_sap.h"
#include "esm_main.h"
#include "esm_sap.h"
#include "msc.h"
#include "s6a_defs.h"
#include "dynamic_memory_check.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

static nas_cause_t s6a_error_2_nas_cause (uint32_t s6a_error,int experimental);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    nas_proc_initialize()                                     **
 **                                                                        **
 ** Description:                                                           **
 **                                                                        **
 ** Inputs:  None                                                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
void
nas_proc_initialize (
  mme_config_t * mme_config_p)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  /*
   * Initialize the EMM procedure manager
   */
  emm_main_initialize (mme_config_p);
  /*
   * Initialize the ESM procedure manager
   */
  esm_main_initialize ();
  OAILOG_FUNC_OUT (LOG_NAS_EMM);
}


/****************************************************************************
 **                                                                        **
 ** Name:    nas_proc_cleanup()                                        **
 **                                                                        **
 ** Description: Performs clean up procedure before the system is shutdown **
 **                                                                        **
 ** Inputs:  None                                                      **
 **          Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **          Return:    None                                       **
 **          Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
void
nas_proc_cleanup (
  void)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  /*
   * Perform the EPS Mobility Manager's clean up procedure
   */
  emm_main_cleanup ();
  /*
   * Perform the EPS Session Manager's clean up procedure
   */
  esm_main_cleanup ();
  OAILOG_FUNC_OUT (LOG_NAS_EMM);
}

/*
   --------------------------------------------------------------------------
            NAS procedures triggered by the user
   --------------------------------------------------------------------------
*/


/****************************************************************************
 **                                                                        **
 ** Name:    nas_proc_establish_ind()                                  **
 **                                                                        **
 ** Description: Processes the NAS signalling connection establishment     **
 **      indication message received from the network              **
 **                                                                        **
 ** Inputs:  ueid:      UE identifier                              **
 **      tac:       The code of the tracking area the initia-  **
 **             ting UE belongs to                         **
 **      data:      The initial NAS message transfered within  **
 **             the message                                **
 **      len:       The length of the initial NAS message      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
nas_proc_establish_ind (
  const mme_ue_s1ap_id_t ue_id,
  const bool is_mm_ctx_new,
  const tai_t originating_tai,
  const ecgi_t ecgi,
  const as_cause_t as_cause,
  const s_tmsi_t s_tmsi,
  STOLEN_REF bstring *msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;

  if (msg) {
    emm_sap_t                               emm_sap = {0};

    /*
     * Notify the EMM procedure call manager that NAS signalling
     * connection establishment indication message has been received
     * from the Access-Stratum sublayer
     */

    emm_sap.primitive = EMMAS_ESTABLISH_REQ;
    emm_sap.u.emm_as.u.establish.ue_id              = ue_id;
    emm_sap.u.emm_as.u.establish.is_initial         = true;
    emm_sap.u.emm_as.u.establish.is_mm_ctx_new      = is_mm_ctx_new;

    emm_sap.u.emm_as.u.establish.nas_msg            = *msg;
    *msg = NULL;
    emm_sap.u.emm_as.u.establish.tai                = &originating_tai;
    //emm_sap.u.emm_as.u.establish.plmn_id            = &originating_tai.plmn;
    //emm_sap.u.emm_as.u.establish.tac                = originating_tai.tac;
    emm_sap.u.emm_as.u.establish.ecgi               = ecgi;

    MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMAS_ESTABLISH_REQ ue id " MME_UE_S1AP_ID_FMT " tai:  plmn %c%c%c.%c%c%c tac %u",
        ue_id,
        (char)(originating_tai.mcc_digit1 + 0x30), (char)(originating_tai.mcc_digit2 + 0x30), (char)(originating_tai.mcc_digit3 + 0x30),
        (char)(originating_tai.mnc_digit1 + 0x30), (char)(originating_tai.mnc_digit2 + 0x30),
        (9 < originating_tai.mnc_digit3) ? ' ': (char)(originating_tai.mnc_digit3 + 0x30),
            originating_tai.tac);
    rc = emm_sap_send (&emm_sap);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    nas_proc_dl_transfer_cnf()                                **
 **                                                                        **
 ** Description: Processes the downlink data transfer confirm message re-  **
 **      ceived from the network while NAS message has been succes-**
 **      sfully delivered to the NAS sublayer on the receiver side.**
 **                                                                        **
 ** Inputs:  ueid:      UE identifier                              **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
nas_proc_dl_transfer_cnf (
  const uint32_t ue_id,
  const nas_error_code_t status,
  bstring * STOLEN_REF nas_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNok;

  /*
   * Notify the EMM procedure call manager that downlink NAS message
   * has been successfully delivered to the NAS sublayer on the
   * receiver side
   */
  emm_sap.primitive = EMMAS_DATA_IND;
  if (AS_SUCCESS == status) {
    emm_sap.u.emm_as.u.data.delivered = EMM_AS_DATA_DELIVERED_TRUE;
    MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMAS_DATA_IND (DATA_DELIVERED) ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
  } else {
    emm_sap.u.emm_as.u.data.delivered = EMM_AS_DATA_DELIVERED_LOWER_LAYER_FAILURE;
    MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMAS_DATA_IND (LL_FAIL) ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
  }
  emm_sap.u.emm_as.u.data.ue_id = ue_id;
  if (*nas_msg) {
    emm_sap.u.emm_as.u.data.nas_msg   = *nas_msg;
    *nas_msg = NULL;
  }
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    nas_proc_dl_transfer_rej()                                **
 **                                                                        **
 ** Description: Processes the downlink data transfer confirm message re-  **
 **      ceived from the network while NAS message has not been    **
 **      delivered to the NAS sublayer on the receiver side.       **
 **                                                                        **
 ** Inputs:  ueid:      UE identifier                              **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
nas_proc_dl_transfer_rej (
  const uint32_t ue_id,
  const nas_error_code_t status,
  bstring * STOLEN_REF nas_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNok;

  /*
   * Notify the EMM procedure call manager that transmission
   * failure of downlink NAS message indication has been received
   * from lower layers
   */
  emm_sap.primitive = EMMAS_DATA_IND;
  emm_sap.u.emm_as.u.data.ue_id = ue_id;
  if (AS_SUCCESS == status) {
    emm_sap.u.emm_as.u.data.delivered = EMM_AS_DATA_DELIVERED_TRUE;
    MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMAS_DATA_IND (DELIVERED) ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
  } else if (AS_NON_DELIVERED_DUE_HO == status) {
    emm_sap.u.emm_as.u.data.delivered = EMM_AS_DATA_DELIVERED_LOWER_LAYER_NON_DELIVERY_INDICATION_DUE_TO_HO;
    MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMAS_DATA_IND (NON_DELIVERED_HO) ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
  } else {
    emm_sap.u.emm_as.u.data.delivered = EMM_AS_DATA_DELIVERED_LOWER_LAYER_FAILURE;
    MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMAS_DATA_IND (LL_FAIL) ue id " MME_UE_S1AP_ID_FMT " ", ue_id);
  }
  emm_sap.u.emm_as.u.data.delivered = status;
  emm_sap.u.emm_as.u.data.nas_msg   = NULL;
  if (*nas_msg) {
    emm_sap.u.emm_as.u.data.nas_msg   = *nas_msg;
    *nas_msg = NULL;
  }
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    nas_proc_ul_transfer_ind()                                **
 **                                                                        **
 ** Description: Processes uplink data transfer indication message recei-  **
 **      ved from the network                                      **
 **                                                                        **
 ** Inputs:  ueid:      UE identifier                              **
 **      data:      The transfered NAS message                 **
 **      len:       The length of the NAS message              **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
nas_proc_ul_transfer_ind (
  const mme_ue_s1ap_id_t ue_id,
  const tai_t originating_tai,
  const ecgi_t cgi,
  STOLEN_REF bstring *msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;


  if (msg) {
    emm_sap_t                               emm_sap = {0};

    /*
     * Notify the EMM procedure call manager that data transfer
     * indication has been received from the Access-Stratum sublayer
     */
    emm_sap.primitive = EMMAS_DATA_IND;
    emm_sap.u.emm_as.u.data.ue_id     = ue_id;
    emm_sap.u.emm_as.u.data.delivered = true;
    emm_sap.u.emm_as.u.data.nas_msg   = *msg;
    *msg = NULL;
    emm_sap.u.emm_as.u.data.tai       = &originating_tai;
    //emm_sap.u.emm_as.u.data.plmn_id   = &originating_tai.plmn;
    //emm_sap.u.emm_as.u.data.tac       = originating_tai.tac;
    emm_sap.u.emm_as.u.data.ecgi      = cgi;
    MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMAS_DATA_IND (UL_TRANSFER) ue id " MME_UE_S1AP_ID_FMT " len %u tai:  plmn %c%c%c.%c%c%c tac %u",
        ue_id, blength(*msg),
        (char)(originating_tai.mcc_digit1 + 0x30), (char)(originating_tai.mcc_digit2 + 0x30), (char)(originating_tai.mcc_digit3 + 0x30),
        (char)(originating_tai.mnc_digit1 + 0x30), (char)(originating_tai.mnc_digit2 + 0x30),
        (9 < originating_tai.mnc_digit3) ? ' ': (char)(originating_tai.mnc_digit3 + 0x30),
            originating_tai.tac);
    rc = emm_sap_send (&emm_sap);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_authentication_info_answer (
    s6a_auth_info_ans_t * aia)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  imsi64_t                                imsi64  = INVALID_IMSI64;
  int                                     rc      = RETURNerror;
  emm_context_t                          *ctxt    = NULL;
  ue_mm_context_t                        *ue_mm_context = NULL;

  DevAssert (aia);
  IMSI_STRING_TO_IMSI64 ((char *)aia->imsi, &imsi64);

  OAILOG_DEBUG (LOG_NAS_EMM, "Handling imsi " IMSI_64_FMT "\n", imsi64);

  ue_mm_context = mme_ue_context_exists_imsi (&mme_app_desc.mme_ue_contexts, imsi64);
  if (ue_mm_context) {
    ctxt = &ue_mm_context->emm_context;
  }

  if (!(ctxt)) {
    OAILOG_ERROR (LOG_NAS_EMM, "That's embarrassing as we don't know this IMSI\n");
    MSC_LOG_EVENT (MSC_MMEAPP_MME, "0 S6A_AUTH_INFO_ANS Unknown imsi " IMSI_64_FMT, imsi64);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
  }

  if ((aia->result.present == S6A_RESULT_BASE)
      && (aia->result.choice.base == DIAMETER_SUCCESS)) {
    /*
     * Check that list is not empty and contain at most MAX_EPS_AUTH_VECTORS elements
     */
    DevCheck(aia->auth_info.nb_of_vectors <= MAX_EPS_AUTH_VECTORS, aia->auth_info.nb_of_vectors, MAX_EPS_AUTH_VECTORS, 0);
    DevCheck(aia->auth_info.nb_of_vectors > 0, aia->auth_info.nb_of_vectors, 1, 0);

    OAILOG_DEBUG (LOG_NAS_EMM, "INFORMING NAS ABOUT AUTH RESP SUCCESS got %u vector(s)\n", aia->auth_info.nb_of_vectors);
    rc = nas_proc_auth_param_res (ue_mm_context->mme_ue_s1ap_id, aia->auth_info.nb_of_vectors, aia->auth_info.eutran_vector);
  } else {
    OAILOG_ERROR (LOG_NAS_EMM, "INFORMING NAS ABOUT AUTH RESP ERROR CODE\n");
    MSC_LOG_EVENT (MSC_MMEAPP_MME, "0 S6A_AUTH_INFO_ANS S6A Failure imsi " IMSI_64_FMT, imsi64);

    /*
     * Inform NAS layer with the right failure
     */
    if (aia->result.present == S6A_RESULT_BASE) {
      rc = nas_proc_auth_param_fail (ue_mm_context->mme_ue_s1ap_id, s6a_error_2_nas_cause (aia->result.choice.base, 0));
    } else {
      rc = nas_proc_auth_param_fail (ue_mm_context->mme_ue_s1ap_id, s6a_error_2_nas_cause (aia->result.choice.experimental, 1));
    }
  }

  unlock_ue_contexts(ue_mm_context);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_auth_param_res (
    mme_ue_s1ap_id_t ue_id,
    uint8_t          nb_vectors,
    eutran_vector_t *vectors)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};
  emm_cn_auth_res_t                       emm_cn_auth_res = {0};


  emm_cn_auth_res.ue_id      = ue_id;
  emm_cn_auth_res.nb_vectors = nb_vectors;
  for (int i = 0; i < nb_vectors; i++) {
    emm_cn_auth_res.vector[i] = &vectors[i];
  }

  emm_sap.primitive = EMMCN_AUTHENTICATION_PARAM_RES;
  emm_sap.u.emm_cn.u.auth_res = &emm_cn_auth_res;
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMCN_AUTHENTICATION_PARAM_RES ue_id " MME_UE_S1AP_ID_FMT " ", ue_id);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_auth_param_fail (
    mme_ue_s1ap_id_t    ue_id,
    nas_cause_t         cause)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};
  emm_cn_auth_fail_t                      emm_cn_auth_fail = {0};

  emm_cn_auth_fail.cause = cause;
  emm_cn_auth_fail.ue_id = ue_id;

  emm_sap.primitive = EMMCN_AUTHENTICATION_PARAM_FAIL;
  emm_sap.u.emm_cn.u.auth_fail = &emm_cn_auth_fail;
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMCN_AUTHENTICATION_PARAM_FAIL ue_id " MME_UE_S1AP_ID_FMT " ", ue_id);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_deregister_ue (
    mme_ue_s1ap_id_t ue_id)
{
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMCN_DEREGISTER_UE ue_id " MME_UE_S1AP_ID_FMT " ", ue_id);
  emm_sap.primitive = EMMCN_DEREGISTER_UE;
  emm_sap.u.emm_cn.u.deregister.ue_id = ue_id;
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_pdn_config_res (
  emm_cn_pdn_config_res_t * emm_cn_pdn_config_res)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};

  emm_sap.primitive = EMMCN_PDN_CONFIG_RES;
  emm_sap.u.emm_cn.u.emm_cn_pdn_config_res = emm_cn_pdn_config_res;
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMCN_PDN_CONFIG_RES ue_id " MME_UE_S1AP_ID_FMT " ", emm_cn_pdn_config_res->ue_id);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_pdn_connectivity_res (
  emm_cn_pdn_res_t * emm_cn_pdn_res)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};

  emm_sap.primitive = EMMCN_PDN_CONNECTIVITY_RES;
  emm_sap.u.emm_cn.u.emm_cn_pdn_res = emm_cn_pdn_res;
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMCN_PDN_CONNECTIVITY_RES ue_id " MME_UE_S1AP_ID_FMT " ", emm_cn_pdn_res->ue_id);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_pdn_connectivity_fail (
  emm_cn_pdn_fail_t * emm_cn_pdn_fail)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};

  emm_sap.primitive = EMMCN_PDN_CONNECTIVITY_FAIL;
  emm_sap.u.emm_cn.u.emm_cn_pdn_fail = emm_cn_pdn_fail;
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMCN_PDN_CONNECTIVITY_FAIL ue_id " MME_UE_S1AP_ID_FMT " ", emm_cn_pdn_fail->ue_id);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int nas_proc_create_dedicated_bearer(emm_cn_activate_dedicated_bearer_req_t * emm_cn_activate)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};
  emm_sap.primitive = _EMMCN_ACTIVATE_DEDICATED_BEARER_REQ;
  emm_sap.u.emm_cn.u.activate_dedicated_bearer_req = emm_cn_activate;
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMM_CN_ACTIVATE_DEDICATED_BEARER_REQ " MME_UE_S1AP_ID_FMT " ", emm_cn_activate->ue_id);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//------------------------------------------------------------------------------
int
nas_proc_implicit_detach_ue_ind (
  mme_ue_s1ap_id_t ue_id)
{
  int                                     rc = RETURNerror;
  emm_sap_t                               emm_sap = {0};

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap.primitive = EMMCN_IMPLICIT_DETACH_UE;
  emm_sap.u.emm_cn.u.emm_cn_implicit_detach.ue_id = ue_id;
  MSC_LOG_TX_MESSAGE (MSC_NAS_MME, MSC_NAS_EMM_MME, NULL, 0, "0 EMMCN_IMPLICIT_DETACH_UE " MME_UE_S1AP_ID_FMT " ", ue_id);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
static nas_cause_t
s6a_error_2_nas_cause (
  uint32_t s6a_error,
  int experimental)
{
  if (experimental == 0) {
    /*
     * Base protocol errors
     */
    switch (s6a_error) {
      /*
       * 3002
       */
    case ER_DIAMETER_UNABLE_TO_DELIVER:        /* Fall through */

      /*
       * 3003
       */
    case ER_DIAMETER_REALM_NOT_SERVED: /* Fall through */

      /*
       * 5003
       */
    case ER_DIAMETER_AUTHORIZATION_REJECTED:
      return NAS_CAUSE_IMSI_UNKNOWN_IN_HSS;

      /*
       * 5012
       */
    case ER_DIAMETER_UNABLE_TO_COMPLY: /* Fall through */

      /*
       * 5004
       */
    case ER_DIAMETER_INVALID_AVP_VALUE:        /* Fall through */

      /*
       * Any other permanent errors from the diameter base protocol
       */
    default:
      break;
    }
  } else {
    switch (s6a_error) {
      /*
       * 4181
       */
    case DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE:
      return NAS_CAUSE_ILLEGAL_UE;

      /*
       * 5001
       */
    case DIAMETER_ERROR_USER_UNKNOWN:
      return NAS_CAUSE_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED;

      /*
       * TODO: distinguish GPRS_DATA_SUBSCRIPTION
       */
      /*
       * 5420
       */
    case DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION:
      return NAS_CAUSE_NO_SUITABLE_CELLS_IN_TRACKING_AREA;

      /*
       * 5421
       */
    case DIAMETER_ERROR_RAT_NOT_ALLOWED:
      /*
       * One of the following parameter can be sent depending on
       * operator preference:
       * ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA
       * TRACKING_AREA_NOT_ALLOWED
       * NO_SUITABLE_CELLS_IN_TRACKING_AREA
       */
      return NAS_CAUSE_TRACKING_AREA_NOT_ALLOWED;

      /*
       * 5004 without error diagnostic
       */
    case DIAMETER_ERROR_ROAMING_NOT_ALLOWED:
      return NAS_CAUSE_PLMN_NOT_ALLOWED;

      /*
       * TODO: 5004 with error diagnostic of ODB_HPLMN_APN or
       * ODB_VPLMN_APN
       */
      /*
       * TODO: 5004 with error diagnostic of ODB_ALL_APN
       */
    default:
      break;
    }
  }

  return NAS_CAUSE_NETWORK_FAILURE;
}

