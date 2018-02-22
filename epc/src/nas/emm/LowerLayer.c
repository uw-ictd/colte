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
  Source      LowerLayer.c

  Version     0.1

  Date        2012/03/14

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines EMM procedures executed by the Non-Access Stratum
        upon receiving notifications from lower layers so that data
        transfer succeed or failed, or NAS signalling connection is
        released, or ESM unit data has been received from under layer,
        and to request ESM unit data transfer to under layer.

*****************************************************************************/

#include "commonDef.h"
#include "LowerLayer.h"
#include "log.h"

#include "emmData.h"

#include "emm_sap.h"
#include "esm_sap.h"
#include "log.h"

#include <string.h>             // memset

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
   --------------------------------------------------------------------------
            Lower layer notification handlers
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    lowerlayer_success()                                      **
 **                                                                        **
 ** Description: Notify the EPS Mobility Management entity that data have  **
 **      been successfully delivered to the network                **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
lowerlayer_success (
  mme_ue_s1ap_id_t ue_id)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNok;

  emm_sap.primitive = EMMREG_LOWERLAYER_SUCCESS;
  emm_sap.u.emm_reg.ue_id = ue_id;
  emm_sap.u.emm_reg.ctx = NULL;
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    lowerlayer_failure()                                      **
 **                                                                        **
 ** Description: Notify the EPS Mobility Management entity that lower la-  **
 **      yers failed to deliver data to the network                **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
lowerlayer_failure (
  mme_ue_s1ap_id_t ue_id)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNok;

  emm_sap.primitive = EMMREG_LOWERLAYER_FAILURE;
  emm_sap.u.emm_reg.ue_id = ue_id;
  emm_data_context_t                     *emm_ctx = NULL;

  if (ue_id > 0) {
    emm_ctx = emm_data_context_get (&_emm_data, ue_id);
  }

  emm_sap.u.emm_reg.ctx = emm_ctx;
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    lowerlayer_non_delivery_indication()                          **
 **                                                                        **
 ** Description: Notify the EPS Mobility Management entity that lower la-  **
 **      yers failed to deliver data to the network                        **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                         **
 **      Others:    None                                                   **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                                  **
 **      Others:    None                                                   **
 **                                                                        **
 ***************************************************************************/
int
lowerlayer_non_delivery_indication (
  mme_ue_s1ap_id_t ue_id)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNok;

  emm_sap.primitive = EMMREG_LOWERLAYER_NON_DELIVERY;
  emm_sap.u.emm_reg.ue_id = ue_id;
  emm_data_context_t                     *emm_ctx = NULL;

  if (ue_id > 0) {
    emm_ctx = emm_data_context_get (&_emm_data, ue_id);
  }

  emm_sap.u.emm_reg.ctx = emm_ctx;
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    lowerlayer_establish()                                    **
 **                                                                        **
 ** Description: Update the EPS connection management status upon recei-   **
 **      ving indication so that the NAS signalling connection is  **
 **      established                                               **
 **                                                                        **
 ** Inputs:  None                                                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
lowerlayer_establish (
  void)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    lowerlayer_release()                                      **
 **                                                                        **
 ** Description: Notify the EPS Mobility Management entity that NAS signal-**
 **      ling connection has been released                         **
 **                                                                        **
 ** Inputs:  cause:     Release cause                              **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
lowerlayer_release (
  int cause)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNok;

  emm_sap.primitive = EMMREG_LOWERLAYER_RELEASE;
  emm_sap.u.emm_reg.ue_id = 0;
  emm_sap.u.emm_reg.ctx = NULL;
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    lowerlayer_data_ind()                                     **
 **                                                                        **
 ** Description: Notify the EPS Session Management entity that data have   **
 **      been received from lower layers                           **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      data:      Data transfered from lower layers          **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
lowerlayer_data_ind (
  mme_ue_s1ap_id_t ue_id,
  const_bstring    data)
{
  esm_sap_t                               esm_sap = {0};
  int                                     rc = RETURNok;
  emm_data_context_t                     *emm_ctx = NULL;

  OAILOG_FUNC_IN (LOG_NAS_EMM);

  if (ue_id > 0) {
    emm_ctx = emm_data_context_get (&_emm_data, ue_id);
  }
  esm_sap.primitive = ESM_UNITDATA_IND;
  esm_sap.is_standalone = true;
  esm_sap.ue_id = ue_id;
  esm_sap.ctx = emm_ctx;
  esm_sap.recv = data;
  data = NULL;
  rc = esm_sap_send (&esm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    lowerlayer_data_req()                                     **
 **                                                                        **
 ** Description: Notify the EPS Mobility Management entity that data have  **
 **      to be transfered to lower layers                          **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **          data:      Data to be transfered to lower layers      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
lowerlayer_data_req (
  mme_ue_s1ap_id_t ue_id,
  bstring data)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;
  emm_sap_t                               emm_sap = {0};
  emm_security_context_t                 *sctx = NULL;
  struct emm_data_context_s              *ctx = NULL;

  emm_sap.primitive = EMMAS_DATA_REQ;
  emm_sap.u.emm_as.u.data.guti = NULL;
  emm_sap.u.emm_as.u.data.ue_id = ue_id;

  if (ue_id > 0) {
    ctx = emm_data_context_get (&_emm_data, ue_id);
  }

  if (ctx) {
    sctx = &ctx->_security;
  }

  emm_sap.u.emm_as.u.data.nas_info = 0;
  emm_sap.u.emm_as.u.data.nas_msg = data;
  data = NULL;
  /*
   * Setup EPS NAS security data
   */
  emm_as_set_security_data (&emm_sap.u.emm_as.u.data.sctx, sctx, false, true);
  rc = emm_sap_send (&emm_sap);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/*
   --------------------------------------------------------------------------
                EMM procedure handlers
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    emm_as_set_security_data()                                    **
 **                                                                        **
 ** Description: Setup security data according to the given EPS security   **
 **      context when data transfer to lower layers is requested   **
 **                                                                        **
 ** Inputs:  args:      EPS security context currently in use      **
 **      is_new:    Indicates whether a new security context   **
 **             has just been taken into use               **
 **      is_ciphered:   Indicates whether the NAS message has to   **
 **             be sent ciphered                           **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     data:      EPS NAS security data to be setup          **
 **      Return:    None                                       **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
void
emm_as_set_security_data (
  emm_as_security_data_t * data,
  const void *args,
  bool is_new,
  bool is_ciphered)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  const emm_security_context_t           *context = (emm_security_context_t *) (args);

  memset (data, 0, sizeof (emm_as_security_data_t));

  if (context && ((context->sc_type == SECURITY_CTX_TYPE_FULL_NATIVE) || (context->sc_type == SECURITY_CTX_TYPE_MAPPED))) {
    /*
     * 3GPP TS 24.301, sections 5.4.3.3 and 5.4.3.4
     * * * * Once a valid EPS security context exists and has been taken
     * * * * into use, UE and MME shall cipher and integrity protect all
     * * * * NAS signalling messages with the selected NAS ciphering and
     * * * * NAS integrity algorithms
     */
    OAILOG_INFO (LOG_NAS_EMM, "EPS security context exists is new %u KSI %u SQN %u count %u\n",
        is_new, context->eksi, context->ul_count.seq_num, *(uint32_t *) (&context->ul_count));
    OAILOG_STREAM_HEX (OAILOG_LEVEL_DEBUG, LOG_NAS_EMM, "knas_int:", context->knas_int, AUTH_KNAS_INT_SIZE);
    OAILOG_STREAM_HEX (OAILOG_LEVEL_DEBUG, LOG_NAS_EMM, "knas_enc:", context->knas_enc, AUTH_KNAS_ENC_SIZE);
    data->is_new = is_new;
    data->ksi = context->eksi;
    data->sqn = context->dl_count.seq_num; //TODO AT check whether we need to increment this by one or it is already incremented after sending last NAS mssage. 
    data->count = 0x00000000 | ((context->dl_count.overflow & 0x0000FFFF) << 8) | (context->dl_count.seq_num & 0x000000FF);
    /*
     * NAS integrity and cyphering keys may not be available if the
     * * * * current security context is a partial EPS security context
     * * * * and not a full native EPS security context
     */
    data->is_knas_int_present = true;
    memcpy(data->knas_int, context->knas_int, sizeof(data->knas_int));

    if (is_ciphered) {
      /*
       * 3GPP TS 24.301, sections 4.4.5
       * * * * When the UE establishes a new NAS signalling connection,
       * * * * it shall send initial NAS messages integrity protected
       * * * * and unciphered
       */
      /*
       * 3GPP TS 24.301, section 5.4.3.2
       * * * * The MME shall send the SECURITY MODE COMMAND message integrity
       * * * * protected and unciphered
       */
      OAILOG_WARNING (LOG_NAS_EMM, "EPS security context exists knas_enc\n");
      data->is_knas_enc_present = true;
      memcpy(data->knas_enc, context->knas_enc, sizeof(data->knas_enc));
    }
  } else {
    OAILOG_DEBUG (LOG_NAS_EMM, "NO Valid Security Context Available\n");
    /*
     * No valid EPS security context exists
     */
    data->ksi = KSI_NO_KEY_AVAILABLE;
  }

  OAILOG_FUNC_OUT (LOG_NAS_EMM);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
