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

  Source      emm_recv.c

  Version     0.1

  Date        2013/01/30

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines functions executed at the EMMAS Service Access
        Point upon receiving EPS Mobility Management messages
        from the Access Stratum sublayer.

*****************************************************************************/
#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "bstrlib.h"

#include "common_types.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"
#include "3gpp_29.274.h"
#include "emm_recv.h"

#include "commonDef.h"
#include "common_defs.h"
#include "log.h"
#include "emm_msgDef.h"
#include "emm_cause.h"
#include "emm_proc.h"
#include "mme_config.h"
#include "3gpp_requirements_24.301.h"
#include "mme_app_defs.h"
#include "emm_sap.h"

extern mme_app_desc_t                          mme_app_desc;

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
static int
_emm_initiate_default_bearer_re_establishment (
  emm_context_t * emm_ctx);
/*
   --------------------------------------------------------------------------
   Functions executed by both the UE and the MME upon receiving EMM messages
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_status()                                         **
 **                                                                        **
 ** Description: Processes EMM status message                              **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **          msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_status (
  mme_ue_s1ap_id_t ue_id,
  emm_status_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t * status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received EMM Status message (cause=%d)\n", msg->emmcause);
  /*
   * Message checking
   */
  *emm_cause = EMM_CAUSE_SUCCESS;
  /*
   * Message processing
   */
  rc = emm_proc_status_ind (ue_id, msg->emmcause);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/*
   --------------------------------------------------------------------------
   Functions executed by the MME upon receiving EMM message from the UE
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_attach_request()                                 **
 **                                                                        **
 ** Description: Processes Attach Request message                          **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_attach_request (
  const mme_ue_s1ap_id_t           ue_id,
  const tai_t              * const originating_tai,
  const ecgi_t             * const originating_ecgi,
  attach_request_msg       * const msg,
  const bool                       is_initial,
  const bool                       is_mm_ctx_new,
  int * const emm_cause,
  const nas_message_decode_status_t  * decode_status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Attach Request message\n");
  /*
   * Message checking
   */
  if (msg->uenetworkcapability.spare != 0b000) {
    /*
     * Spare bits shall be coded as zero
     */
    *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
    REQUIREMENT_3GPP_24_301(R10_5_5_1_2_7_b__4);
    OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - [%08x] - Non zero spare bits is suspicious\n", ue_id);
  }

  /*
   * Handle message checking error
   */
  if (*emm_cause != EMM_CAUSE_SUCCESS) {
    /*
     * Requirement MME24.301R10_5.5.1.2.7_b Protocol error
     */
    rc = emm_proc_attach_reject (ue_id, *emm_cause);
    *emm_cause = EMM_CAUSE_SUCCESS;
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
  }

  emm_attach_request_ies_t * params = calloc(1, sizeof(*params));
  /*
   * Message processing
   */
  /*
   * Get the EPS attach type
   */
  params->type = EMM_ATTACH_TYPE_RESERVED;
  if (msg->epsattachtype == EPS_ATTACH_TYPE_EPS) {
    params->type = EMM_ATTACH_TYPE_EPS;
  } else if (msg->epsattachtype == EPS_ATTACH_TYPE_COMBINED_EPS_IMSI) {
    params->type = EMM_ATTACH_TYPE_COMBINED_EPS_IMSI;
  } else if (msg->epsattachtype == EPS_ATTACH_TYPE_EMERGENCY) {
    params->type = EMM_ATTACH_TYPE_EMERGENCY;
  } else if (msg->epsattachtype == EPS_ATTACH_TYPE_RESERVED) {
    params->type = EMM_ATTACH_TYPE_RESERVED;
  } else {
    REQUIREMENT_3GPP_24_301(R10_9_9_3_11__1);
    params->type = EMM_ATTACH_TYPE_EPS;
  }

  /*
   * Get the EPS mobile identity
   */

  if (msg->oldgutiorimsi.guti.typeofidentity == EPS_MOBILE_IDENTITY_GUTI) {
    /*
     * Get the GUTI
     */
    params->guti = calloc(1, sizeof(guti_t));
    params->guti->gummei.plmn.mcc_digit1 = msg->oldgutiorimsi.guti.mcc_digit1;
    params->guti->gummei.plmn.mcc_digit2 = msg->oldgutiorimsi.guti.mcc_digit2;
    params->guti->gummei.plmn.mcc_digit3 = msg->oldgutiorimsi.guti.mcc_digit3;
    params->guti->gummei.plmn.mnc_digit1 = msg->oldgutiorimsi.guti.mnc_digit1;
    params->guti->gummei.plmn.mnc_digit2 = msg->oldgutiorimsi.guti.mnc_digit2;
    params->guti->gummei.plmn.mnc_digit3 = msg->oldgutiorimsi.guti.mnc_digit3;
    params->guti->gummei.mme_gid = msg->oldgutiorimsi.guti.mme_group_id;
    params->guti->gummei.mme_code = msg->oldgutiorimsi.guti.mme_code;
    params->guti->m_tmsi = msg->oldgutiorimsi.guti.m_tmsi;
  } else if (msg->oldgutiorimsi.imsi.typeofidentity == EPS_MOBILE_IDENTITY_IMSI) {
    /*
     * Get the IMSI
     */
    params->imsi = calloc(1, sizeof(imsi_t));
    params->imsi->u.num.digit1 = msg->oldgutiorimsi.imsi.identity_digit1;
    params->imsi->u.num.digit2 = msg->oldgutiorimsi.imsi.identity_digit2;
    params->imsi->u.num.digit3 = msg->oldgutiorimsi.imsi.identity_digit3;
    params->imsi->u.num.digit4 = msg->oldgutiorimsi.imsi.identity_digit4;
    params->imsi->u.num.digit5 = msg->oldgutiorimsi.imsi.identity_digit5;
    params->imsi->u.num.digit6 = msg->oldgutiorimsi.imsi.identity_digit6;
    params->imsi->u.num.digit7 = msg->oldgutiorimsi.imsi.identity_digit7;
    params->imsi->u.num.digit8 = msg->oldgutiorimsi.imsi.identity_digit8;
    params->imsi->u.num.digit9 = msg->oldgutiorimsi.imsi.identity_digit9;
    params->imsi->u.num.digit10 = msg->oldgutiorimsi.imsi.identity_digit10;
    params->imsi->u.num.digit11 = msg->oldgutiorimsi.imsi.identity_digit11;
    params->imsi->u.num.digit12 = msg->oldgutiorimsi.imsi.identity_digit12;
    params->imsi->u.num.digit13 = msg->oldgutiorimsi.imsi.identity_digit13;
    params->imsi->u.num.digit14 = msg->oldgutiorimsi.imsi.identity_digit14;
    params->imsi->u.num.digit15 = msg->oldgutiorimsi.imsi.identity_digit15;
    params->imsi->u.num.parity  = 0x0f;
    for (int index=0; index < IMSI_BCD8_SIZE; index++) {
      if (params->imsi->u.value[index] == 0xff) {
        params->imsi->length = index;
        break;
      } else  if ((params->imsi->u.value[index] & 0x0f) == 0x0f) {
        params->imsi->length = index + 1;
        break;
      }
    }
  } else if (msg->oldgutiorimsi.imei.typeofidentity == EPS_MOBILE_IDENTITY_IMEI) {
    /*
     * Get the IMEI
     */
    params->imei = calloc(1, sizeof(imei_t));
    params->imei->u.num.tac1 = msg->oldgutiorimsi.imei.identity_digit1;
    params->imei->u.num.tac2 = msg->oldgutiorimsi.imei.identity_digit2;
    params->imei->u.num.tac3 = msg->oldgutiorimsi.imei.identity_digit3;
    params->imei->u.num.tac4 = msg->oldgutiorimsi.imei.identity_digit4;
    params->imei->u.num.tac5 = msg->oldgutiorimsi.imei.identity_digit5;
    params->imei->u.num.tac6 = msg->oldgutiorimsi.imei.identity_digit6;
    params->imei->u.num.tac7 = msg->oldgutiorimsi.imei.identity_digit7;
    params->imei->u.num.tac8 = msg->oldgutiorimsi.imei.identity_digit8;
    params->imei->u.num.snr1 = msg->oldgutiorimsi.imei.identity_digit9;
    params->imei->u.num.snr2 = msg->oldgutiorimsi.imei.identity_digit10;
    params->imei->u.num.snr3 = msg->oldgutiorimsi.imei.identity_digit11;
    params->imei->u.num.snr4 = msg->oldgutiorimsi.imei.identity_digit12;
    params->imei->u.num.snr5 = msg->oldgutiorimsi.imei.identity_digit13;
    params->imei->u.num.snr6 = msg->oldgutiorimsi.imei.identity_digit14;
    params->imei->u.num.cdsd = msg->oldgutiorimsi.imei.identity_digit15;
    params->imei->u.num.parity = msg->oldgutiorimsi.imei.oddeven;
  }

  /*
   * TODO: Get the UE network capabilities
   */
  /*
   * Get the Last visited registered TAI
   */


  if (msg->presencemask & ATTACH_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT) {
    params->last_visited_registered_tai = calloc(1, sizeof(tai_t));
    params->last_visited_registered_tai->mcc_digit1 = msg->lastvisitedregisteredtai.mcc_digit1;
    params->last_visited_registered_tai->mcc_digit2 = msg->lastvisitedregisteredtai.mcc_digit2;
    params->last_visited_registered_tai->mcc_digit3 = msg->lastvisitedregisteredtai.mcc_digit3;
    params->last_visited_registered_tai->mnc_digit1 = msg->lastvisitedregisteredtai.mnc_digit1;
    params->last_visited_registered_tai->mnc_digit2 = msg->lastvisitedregisteredtai.mnc_digit2;
    params->last_visited_registered_tai->mnc_digit3 = msg->lastvisitedregisteredtai.mnc_digit3;
    params->last_visited_registered_tai->tac = msg->lastvisitedregisteredtai.tac;
  }
  if (msg->presencemask & ATTACH_REQUEST_DRX_PARAMETER_PRESENT) {
    params->drx_parameter = calloc(1, sizeof(drx_parameter_t));
    memcpy(params->drx_parameter, &msg->drxparameter, sizeof(drx_parameter_t));
  }

  params->is_initial     = is_initial;
  params->is_native_sc   = (msg->naskeysetidentifier.tsc != NAS_KEY_SET_IDENTIFIER_MAPPED);
  params->ksi            = msg->naskeysetidentifier.naskeysetidentifier;
  params->is_native_guti = (msg->oldgutitype != GUTI_MAPPED);
  if (originating_tai) {
    params->originating_tai = calloc(1, sizeof(tai_t));
    memcpy(params->originating_tai, originating_tai, sizeof(tai_t));
  }
  if (originating_ecgi) {
    params->originating_ecgi = calloc(1, sizeof(ecgi_t));
    memcpy(params->originating_ecgi, originating_ecgi, sizeof(ecgi_t));
  }
  memcpy(&params->ue_network_capability, &msg->uenetworkcapability, sizeof(ue_network_capability_t));

  if (msg->presencemask & ATTACH_REQUEST_MS_NETWORK_CAPABILITY_PRESENT) {
    params->ms_network_capability = calloc(1, sizeof(ms_network_capability_t));
    memcpy(params->ms_network_capability, &msg->msnetworkcapability, sizeof(ms_network_capability_t));
  }
  params->esm_msg = msg->esmmessagecontainer;
  msg->esmmessagecontainer = NULL;

  params->decode_status = *decode_status;

  /*
   * Execute the requested UE attach procedure
   */
  rc = emm_proc_attach_request (ue_id, is_mm_ctx_new, params);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_attach_complete()                                **
 **                                                                        **
 ** Description: Processes Attach Complete message                         **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_attach_complete (
  mme_ue_s1ap_id_t ue_id,
  const attach_complete_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t * status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Attach Complete message\n");
  /*
   * Execute the attach procedure completion
   */
  rc = emm_proc_attach_complete (ue_id, msg->esmmessagecontainer, *emm_cause, *status);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_detach_request()                                 **
 **                                                                        **
 ** Description: Processes Detach Request message                          **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_detach_request (
  mme_ue_s1ap_id_t                   ue_id,
  const detach_request_msg          *msg,
  const bool                         is_initial,
  int                               *emm_cause,
  const nas_message_decode_status_t *status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc  = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Detach Request message\n");
  /*
   * Message processing
   */
  emm_detach_request_ies_t * params = calloc(1, sizeof(*params));
  /*
   * Get the detach type
   */
  params->type = EMM_DETACH_TYPE_RESERVED;

  if (msg->detachtype.typeofdetach == DETACH_TYPE_EPS) {
    params->type = EMM_DETACH_TYPE_EPS;
  } else if (msg->detachtype.typeofdetach == DETACH_TYPE_IMSI) {
    params->type = EMM_DETACH_TYPE_IMSI;
  } else if (msg->detachtype.typeofdetach == DETACH_TYPE_EPS_IMSI) {
    params->type = EMM_DETACH_TYPE_EPS_IMSI;
  } else if (msg->detachtype.typeofdetach == DETACH_TYPE_RESERVED_1) {
    params->type = EMM_DETACH_TYPE_RESERVED;
  } else if (msg->detachtype.typeofdetach == DETACH_TYPE_RESERVED_2) {
    params->type = EMM_DETACH_TYPE_RESERVED;
  } else {
    /*
     * All other values are interpreted as "combined EPS/IMSI detach"
     */
    REQUIREMENT_3GPP_24_301(R10_9_9_3_7_1__1);
    params->type = DETACH_TYPE_EPS_IMSI;
  }

  /*
   * Get the EPS mobile identity
   */

  if (msg->gutiorimsi.guti.typeofidentity == EPS_MOBILE_IDENTITY_GUTI) {
    /*
     * Get the GUTI
     */
    params->guti = calloc(1, sizeof(guti_t));
    params->guti->gummei.plmn.mcc_digit1 = msg->gutiorimsi.guti.mcc_digit1;
    params->guti->gummei.plmn.mcc_digit2 = msg->gutiorimsi.guti.mcc_digit2;
    params->guti->gummei.plmn.mcc_digit3 = msg->gutiorimsi.guti.mcc_digit3;
    params->guti->gummei.plmn.mnc_digit1 = msg->gutiorimsi.guti.mnc_digit1;
    params->guti->gummei.plmn.mnc_digit2 = msg->gutiorimsi.guti.mnc_digit2;
    params->guti->gummei.plmn.mnc_digit3 = msg->gutiorimsi.guti.mnc_digit3;
    params->guti->gummei.mme_gid = msg->gutiorimsi.guti.mme_group_id;
    params->guti->gummei.mme_code = msg->gutiorimsi.guti.mme_code;
    params->guti->m_tmsi = msg->gutiorimsi.guti.m_tmsi;
  } else if (msg->gutiorimsi.imsi.typeofidentity == EPS_MOBILE_IDENTITY_IMSI) {
    /*
     * Get the IMSI
     */
    params->imsi = calloc(1, sizeof(imsi_t));
    params->imsi->u.num.digit1 = msg->gutiorimsi.imsi.identity_digit1;
    params->imsi->u.num.digit2 = msg->gutiorimsi.imsi.identity_digit2;
    params->imsi->u.num.digit3 = msg->gutiorimsi.imsi.identity_digit3;
    params->imsi->u.num.digit4 = msg->gutiorimsi.imsi.identity_digit4;
    params->imsi->u.num.digit5 = msg->gutiorimsi.imsi.identity_digit5;
    params->imsi->u.num.digit6 = msg->gutiorimsi.imsi.identity_digit6;
    params->imsi->u.num.digit7 = msg->gutiorimsi.imsi.identity_digit7;
    params->imsi->u.num.digit8 = msg->gutiorimsi.imsi.identity_digit8;
    params->imsi->u.num.digit9 = msg->gutiorimsi.imsi.identity_digit9;
    params->imsi->u.num.digit10 = msg->gutiorimsi.imsi.identity_digit10;
    params->imsi->u.num.digit11 = msg->gutiorimsi.imsi.identity_digit11;
    params->imsi->u.num.digit12 = msg->gutiorimsi.imsi.identity_digit12;
    params->imsi->u.num.digit13 = msg->gutiorimsi.imsi.identity_digit13;
    params->imsi->u.num.digit14 = msg->gutiorimsi.imsi.identity_digit14;
    params->imsi->u.num.digit15 = msg->gutiorimsi.imsi.identity_digit15;
    params->imsi->u.num.parity  = 0x0f;
    for (int index=0; index < IMSI_BCD8_SIZE; index++) {
      if (params->imsi->u.value[index] == 0xff) {
        params->imsi->length = index;
        break;
      } else  if ((params->imsi->u.value[index] & 0x0f) == 0x0f) {
        params->imsi->length = index + 1;
        break;
      }
    }
  } else if (msg->gutiorimsi.imei.typeofidentity == EPS_MOBILE_IDENTITY_IMEI) {
    /*
     * Get the IMEI
     */
    params->imei = calloc(1, sizeof(imei_t));
    params->imei->u.num.tac1 = msg->gutiorimsi.imei.identity_digit1;
    params->imei->u.num.tac2 = msg->gutiorimsi.imei.identity_digit2;
    params->imei->u.num.tac3 = msg->gutiorimsi.imei.identity_digit3;
    params->imei->u.num.tac4 = msg->gutiorimsi.imei.identity_digit4;
    params->imei->u.num.tac5 = msg->gutiorimsi.imei.identity_digit5;
    params->imei->u.num.tac6 = msg->gutiorimsi.imei.identity_digit6;
    params->imei->u.num.tac7 = msg->gutiorimsi.imei.identity_digit7;
    params->imei->u.num.tac8 = msg->gutiorimsi.imei.identity_digit8;
    params->imei->u.num.snr1 = msg->gutiorimsi.imei.identity_digit9;
    params->imei->u.num.snr2 = msg->gutiorimsi.imei.identity_digit10;
    params->imei->u.num.snr3 = msg->gutiorimsi.imei.identity_digit11;
    params->imei->u.num.snr4 = msg->gutiorimsi.imei.identity_digit12;
    params->imei->u.num.snr5 = msg->gutiorimsi.imei.identity_digit13;
    params->imei->u.num.snr6 = msg->gutiorimsi.imei.identity_digit14;
    params->imei->u.num.cdsd = msg->gutiorimsi.imei.identity_digit15;
    params->imei->u.num.parity = msg->gutiorimsi.imei.oddeven;
  }

  params->switch_off   = (msg->detachtype.switchoff != DETACH_TYPE_NORMAL_DETACH);
  params->is_native_sc = (msg->naskeysetidentifier.tsc != NAS_KEY_SET_IDENTIFIER_MAPPED);
  params->ksi          = msg->naskeysetidentifier.naskeysetidentifier;
  params->decode_status= *status;
  /*
   * Execute the UE initiated detach procedure completion by the network
   */
  rc = emm_proc_detach_request (ue_id, params);
  *emm_cause = RETURNok == rc ? EMM_CAUSE_SUCCESS : EMM_CAUSE_PROTOCOL_ERROR;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:        emm_recv_tracking_area_update_request()                   **
 **                                                                        **
 ** Description: Processes Tracking Area Update Request message            **
 **                                                                        **
 ** Inputs:      ue_id:          UE lower layer identifier                  **
 **              msg:           The received EMM message                   **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     emm_cause:     EMM cause code                             **
 **              Return:        RETURNok, RETURNerror                      **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_tracking_area_update_request (
  const mme_ue_s1ap_id_t ue_id,
  tracking_area_update_request_msg * const msg,
  const bool is_initial,
  int * const emm_cause,
  const nas_message_decode_status_t  * const decode_status)
{
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Tracking Area Update Request message, Security context %s Integrity protected %s MAC matched %s Ciphered %s\n",
      (decode_status->security_context_available)?"yes":"no",
      (decode_status->integrity_protected_message)?"yes":"no",
      (decode_status->mac_matched)?"yes":"no",
      (decode_status->ciphered_message)?"yes":"no");
  /* Basic Periodic TAU Request handling is supported. Only mandatory IEs are supported
   * TODO - Add support for re-auth during TAU , Implicit GUTI Re-allocation & TAU Complete,
   * TAU due to change in TAs, optional IEs 
   */

  emm_tau_request_ies_t *ies = calloc(1, sizeof(*ies));
  ies->is_initial = is_initial;
  // Mandatory fields
  ies->eps_update_type = msg->epsupdatetype;
  ies->is_native_sc   = (msg->naskeysetidentifier.tsc != NAS_KEY_SET_IDENTIFIER_MAPPED);
  ies->ksi            = msg->naskeysetidentifier.naskeysetidentifier;

  ies->old_guti.gummei.plmn.mcc_digit1 = msg->oldguti.guti.mcc_digit1;
  ies->old_guti.gummei.plmn.mcc_digit2 = msg->oldguti.guti.mcc_digit2;
  ies->old_guti.gummei.plmn.mcc_digit3 = msg->oldguti.guti.mcc_digit3;
  ies->old_guti.gummei.plmn.mnc_digit1 = msg->oldguti.guti.mnc_digit1;
  ies->old_guti.gummei.plmn.mnc_digit2 = msg->oldguti.guti.mnc_digit2;
  ies->old_guti.gummei.plmn.mnc_digit3 = msg->oldguti.guti.mnc_digit3;
  ies->old_guti.gummei.mme_gid         = msg->oldguti.guti.mme_group_id;
  ies->old_guti.gummei.mme_code        = msg->oldguti.guti.mme_code;
  ies->old_guti.m_tmsi                 = msg->oldguti.guti.m_tmsi;

  // Optional fields
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_NONCURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER_PRESENT) {
    ies->is_native_non_current_sc   = (msg->noncurrentnativenaskeysetidentifier.tsc != NAS_KEY_SET_IDENTIFIER_MAPPED);
    ies->non_current_ksi            = msg->noncurrentnativenaskeysetidentifier.naskeysetidentifier;
  }

  // NOT TODO additional_guti, useless
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_UE_NETWORK_CAPABILITY_PRESENT) {
    ies->ue_network_capability = calloc(1, sizeof(*ies->ue_network_capability));
    memcpy(ies->ue_network_capability, &msg->uenetworkcapability, sizeof(*ies->ue_network_capability));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT) {
    ies->last_visited_registered_tai = calloc(1, sizeof(*ies->last_visited_registered_tai));
    memcpy(ies->last_visited_registered_tai, &msg->lastvisitedregisteredtai, sizeof(*ies->last_visited_registered_tai));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_DRX_PARAMETER_PRESENT) {
    ies->drx_parameter = calloc(1, sizeof(*ies->drx_parameter));
    memcpy(ies->drx_parameter, &msg->drxparameter, sizeof(*ies->drx_parameter));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_UE_RADIO_CAPABILITY_INFORMATION_UPDATE_NEEDED_PRESENT) {
    ies->is_ue_radio_capability_information_update_needed = (msg->ueradiocapabilityinformationupdateneeded) ? true:false;
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_EPS_BEARER_CONTEXT_STATUS_PRESENT) {
    ies->eps_bearer_context_status = calloc(1, sizeof(*ies->eps_bearer_context_status));
    memcpy(ies->eps_bearer_context_status, &msg->epsbearercontextstatus, sizeof(*ies->eps_bearer_context_status));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_MS_NETWORK_CAPABILITY_PRESENT) {
    ies->ms_network_capability = calloc(1, sizeof(*ies->ms_network_capability));
    memcpy(ies->ms_network_capability, &msg->msnetworkcapability, sizeof(*ies->ms_network_capability));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_TMSI_STATUS_PRESENT) {
    ies->tmsi_status = calloc(1, sizeof(*ies->tmsi_status));
    memcpy(ies->tmsi_status, &msg->tmsistatus, sizeof(*ies->tmsi_status));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_MOBILE_STATION_CLASSMARK_2_PRESENT) {
    ies->mobile_station_classmark2 = calloc(1, sizeof(*ies->mobile_station_classmark2));
    memcpy(ies->mobile_station_classmark2, &msg->mobilestationclassmark2, sizeof(*ies->mobile_station_classmark2));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_MOBILE_STATION_CLASSMARK_3_PRESENT) {
    ies->mobile_station_classmark3 = calloc(1, sizeof(*ies->mobile_station_classmark3));
    memcpy(ies->mobile_station_classmark3, &msg->mobilestationclassmark3, sizeof(*ies->mobile_station_classmark3));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_SUPPORTED_CODECS_PRESENT) {
    ies->supported_codecs = calloc(1, sizeof(*ies->supported_codecs));
    memcpy(ies->supported_codecs, &msg->supportedcodecs, sizeof(*ies->supported_codecs));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_ADDITIONAL_UPDATE_TYPE_PRESENT) {
    ies->additional_updatetype = calloc(1, sizeof(*ies->additional_updatetype));
    memcpy(ies->additional_updatetype, &msg->additionalupdatetype, sizeof(*ies->additional_updatetype));
  }
  if (msg->presencemask & TRACKING_AREA_UPDATE_REQUEST_OLD_GUTI_TYPE_PRESENT) {
    ies->old_guti_type = calloc(1, sizeof(*ies->old_guti_type));
    memcpy(ies->old_guti_type, &msg->oldgutitype, sizeof(*ies->old_guti_type));
  }

  ies->decode_status = *decode_status;
  rc = emm_proc_tracking_area_update_request(ue_id, ies, emm_cause);

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:        emm_recv_service_request()                                **
 **                                                                        **
 ** Description: Processes Service Request message                         **
 **                                                                        **
 ** Inputs:      ue_id:          UE lower layer identifier                  **
 **              msg:           The received EMM message                   **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     emm_cause:     EMM cause code                             **
 **              Return:        RETURNok, RETURNerror                      **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_service_request (
  mme_ue_s1ap_id_t ue_id,
  const service_request_msg * msg,
  const bool is_initial,
  int *emm_cause,
  const nas_message_decode_status_t  * decode_status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM); 
  int                                     rc = RETURNok;
  emm_context_t * emm_ctx = NULL;
  *emm_cause = EMM_CAUSE_PROTOCOL_ERROR; 

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Service Request message, Security context %s Integrity protected %s MAC matched %s Ciphered %s\n",
      (decode_status->security_context_available)?"yes":"no",
      (decode_status->integrity_protected_message)?"yes":"no",
      (decode_status->mac_matched)?"yes":"no",
      (decode_status->ciphered_message)?"yes":"no");
  
  // Get emm_ctx 
  emm_ctx = emm_context_get (&_emm_data,ue_id);
  /*
   * Do following: 
   * 1. Re-establish UE specfic S1 signaling connection and S1-U tunnel for default bearer.
   * note - At present, only default bearer is supported 
   * 2. Move UE ECM state to Connected 
   * 3. Stop Mobile reachability time and Implicit Deatch timer ( if running)
   */
  rc = _emm_initiate_default_bearer_re_establishment(emm_ctx);
  if (rc == RETURNok) { 
    *emm_cause = EMM_CAUSE_SUCCESS;
  }
  emm_context_unlock(emm_ctx);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_identity_response()                              **
 **                                                                        **
 ** Description: Processes Identity Response message                       **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_identity_response (
  mme_ue_s1ap_id_t ue_id,
  identity_response_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t * status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Identity Response message\n");
  /*
   * Message processing
   */
  /*
   * Get the mobile identity
   */
  imsi_t                                  imsi = {0},
                                         *p_imsi = NULL;
  imei_t                                  imei = {0},
                                         *p_imei = NULL;
  imeisv_t                                imeisv = {0},
                                         *p_imeisv = NULL;
  tmsi_t                                  tmsi = 0,
                                         *p_tmsi = NULL;

  if (msg->mobileidentity.imsi.typeofidentity == MOBILE_IDENTITY_IMSI) {
    /*
     * Get the IMSI
     */
    p_imsi = &imsi;
    imsi.u.num.digit1 = msg->mobileidentity.imsi.digit1;
    imsi.u.num.digit2 = msg->mobileidentity.imsi.digit2;
    imsi.u.num.digit3 = msg->mobileidentity.imsi.digit3;
    imsi.u.num.digit4 = msg->mobileidentity.imsi.digit4;
    imsi.u.num.digit5 = msg->mobileidentity.imsi.digit5;
    imsi.u.num.digit6 = msg->mobileidentity.imsi.digit6;
    imsi.u.num.digit7 = msg->mobileidentity.imsi.digit7;
    imsi.u.num.digit8 = msg->mobileidentity.imsi.digit8;
    imsi.u.num.digit9 = msg->mobileidentity.imsi.digit9;
    imsi.u.num.digit10 = msg->mobileidentity.imsi.digit10;
    imsi.u.num.digit11 = msg->mobileidentity.imsi.digit11;
    imsi.u.num.digit12 = msg->mobileidentity.imsi.digit12;
    imsi.u.num.digit13 = msg->mobileidentity.imsi.digit13;
    imsi.u.num.digit14 = msg->mobileidentity.imsi.digit14;
    imsi.u.num.digit15 = msg->mobileidentity.imsi.digit15;
    imsi.u.num.parity  = 0x0f;

    for (int index=0; index < IMSI_BCD8_SIZE; index++) {
      if (imsi.u.value[index] == 0xff) {
        imsi.length = index;
        break;
      } else  if ((imsi.u.value[index] & 0x0f) == 0x0f) {
        imsi.length = index + 1;
        break;
      }
    }
  } else if (msg->mobileidentity.imei.typeofidentity == MOBILE_IDENTITY_IMEI) {
    /*
     * Get the IMEI
     */
    p_imei = &imei;
    imei.u.num.tac1 = msg->mobileidentity.imei.tac1;
    imei.u.num.tac2 = msg->mobileidentity.imei.tac2;
    imei.u.num.tac3 = msg->mobileidentity.imei.tac3;
    imei.u.num.tac4 = msg->mobileidentity.imei.tac4;
    imei.u.num.tac5 = msg->mobileidentity.imei.tac5;
    imei.u.num.tac6 = msg->mobileidentity.imei.tac6;
    imei.u.num.tac7 = msg->mobileidentity.imei.tac7;
    imei.u.num.tac8 = msg->mobileidentity.imei.tac8;
    imei.u.num.snr1 = msg->mobileidentity.imei.snr1;
    imei.u.num.snr2 = msg->mobileidentity.imei.snr2;
    imei.u.num.snr3 = msg->mobileidentity.imei.snr3;
    imei.u.num.snr4 = msg->mobileidentity.imei.snr4;
    imei.u.num.snr5 = msg->mobileidentity.imei.snr5;
    imei.u.num.snr6 = msg->mobileidentity.imei.snr6;
    imei.u.num.cdsd = msg->mobileidentity.imei.cdsd;
    imei.u.num.parity = msg->mobileidentity.imei.oddeven;
  } else if (msg->mobileidentity.imeisv.typeofidentity == MOBILE_IDENTITY_IMEISV) {
    /*
     * Get the IMEISV
     */
    p_imeisv = &imeisv;
    imeisv.u.num.tac1 = msg->mobileidentity.imeisv.tac1;
    imeisv.u.num.tac2 = msg->mobileidentity.imeisv.tac2;
    imeisv.u.num.tac3 = msg->mobileidentity.imeisv.tac3;
    imeisv.u.num.tac4 = msg->mobileidentity.imeisv.tac4;
    imeisv.u.num.tac5 = msg->mobileidentity.imeisv.tac5;
    imeisv.u.num.tac6 = msg->mobileidentity.imeisv.tac6;
    imeisv.u.num.tac7 = msg->mobileidentity.imeisv.tac7;
    imeisv.u.num.tac8 = msg->mobileidentity.imeisv.tac8;
    imeisv.u.num.snr1 = msg->mobileidentity.imeisv.snr1;
    imeisv.u.num.snr2 = msg->mobileidentity.imeisv.snr2;
    imeisv.u.num.snr3 = msg->mobileidentity.imeisv.snr3;
    imeisv.u.num.snr4 = msg->mobileidentity.imeisv.snr4;
    imeisv.u.num.snr5 = msg->mobileidentity.imeisv.snr5;
    imeisv.u.num.snr6 = msg->mobileidentity.imeisv.snr6;
    imeisv.u.num.svn1 = msg->mobileidentity.imeisv.svn1;
    imeisv.u.num.svn2 = msg->mobileidentity.imeisv.svn2;
    imeisv.u.num.parity = msg->mobileidentity.imeisv.oddeven;
  } else if (msg->mobileidentity.tmsi.typeofidentity == MOBILE_IDENTITY_TMSI) {
    /*
     * Get the TMSI
     */
    p_tmsi = &tmsi;
    tmsi  = ((tmsi_t)msg->mobileidentity.tmsi.tmsi[0]) << 24;
    tmsi |= (((tmsi_t)msg->mobileidentity.tmsi.tmsi[1]) << 16);
    tmsi |= (((tmsi_t)msg->mobileidentity.tmsi.tmsi[2]) << 8);
    tmsi |= ((tmsi_t)msg->mobileidentity.tmsi.tmsi[3]);
  }

  /*
   * Execute the identification completion procedure
   */
  rc = emm_proc_identification_complete (ue_id, p_imsi, p_imei, p_imeisv, (uint32_t *) (p_tmsi));
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_authentication_response()                        **
 **                                                                        **
 ** Description: Processes Authentication Response message                 **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_authentication_response (
  mme_ue_s1ap_id_t ue_id,
  authentication_response_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t * status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Authentication Response message\n");

  /*
   * Message checking
   */
  if ((NULL == msg->authenticationresponseparameter) || (!blength(msg->authenticationresponseparameter))) {
    /*
     * RES parameter shall not be null
     */
    *emm_cause = EMM_CAUSE_INVALID_MANDATORY_INFO;
  }

  /*
   * Handle message checking error
   */
  if (*emm_cause != EMM_CAUSE_SUCCESS) {
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
  }

  /*
   * Message processing
   */
  /*
   * Execute the authentication completion procedure
   */
  rc = emm_proc_authentication_complete (ue_id, EMM_CAUSE_SUCCESS, msg->authenticationresponseparameter);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_authentication_failure()                         **
 **                                                                        **
 ** Description: Processes Authentication Failure message                  **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_authentication_failure (
  mme_ue_s1ap_id_t ue_id,
  authentication_failure_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t * status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Authentication Failure message\n");

  /*
   * Message checking
   */
  if (msg->emmcause == EMM_CAUSE_SUCCESS) {
    *emm_cause = EMM_CAUSE_INVALID_MANDATORY_INFO;
  } else if ((msg->emmcause == EMM_CAUSE_SYNCH_FAILURE) && !(msg->presencemask & AUTHENTICATION_FAILURE_AUTHENTICATION_FAILURE_PARAMETER_PRESENT)) {
    /*
     * AUTS parameter shall be present in case of "synch failure"
     */
    *emm_cause = EMM_CAUSE_INVALID_MANDATORY_INFO;
  }

  /*
   * Handle message checking error
   */
  if (*emm_cause != EMM_CAUSE_SUCCESS) {
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
  }

  /*
   * Message processing
   */
  /*
   * Execute the authentication completion procedure
   */
  rc = emm_proc_authentication_failure (ue_id, msg->emmcause, msg->authenticationfailureparameter);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_security_mode_complete()                         **
 **                                                                        **
 ** Description: Processes Security Mode Complete message                  **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_security_mode_complete (
  mme_ue_s1ap_id_t ue_id,
  security_mode_complete_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t * status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Security Mode Complete message\n");
  /*
   * Message processing
   */
  /*
   * Execute the NAS security mode control completion procedure
   */
  if (msg->presencemask & SECURITY_MODE_COMMAND_IMEISV_REQUEST_PRESENT) {
    rc = emm_proc_security_mode_complete (ue_id, &msg->imeisv.imeisv);
  } else {
    rc = emm_proc_security_mode_complete (ue_id, NULL);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_recv_security_mode_reject()                           **
 **                                                                        **
 ** Description: Processes Security Mode Reject message                    **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The received EMM message                   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_recv_security_mode_reject (
  mme_ue_s1ap_id_t ue_id,
  security_mode_reject_msg * msg,
  int *emm_cause,
  const nas_message_decode_status_t * status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received Security Mode Reject message " "(cause=%d)\n", msg->emmcause);

  /*
   * Message checking
   */
  if (msg->emmcause == EMM_CAUSE_SUCCESS) {
    *emm_cause = EMM_CAUSE_INVALID_MANDATORY_INFO;
  }

  /*
   * Handle message checking error
   */
  if (*emm_cause != EMM_CAUSE_SUCCESS) {
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
  }

  /*
   * Message processing
   */
  /*
   * Execute the NAS security mode commend not accepted by the UE
   */
  rc = emm_proc_security_mode_reject (ue_id);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

//-------------------------------------------------------------------------------------
static int
_emm_initiate_default_bearer_re_establishment (
  emm_context_t * emm_ctx)
{
  /*
   * This function is used to trigger initial context setup request towards eNB via S1AP module as part of serivce request handling.
   * This inturn triggers re-establishment of "data radio bearer" and "S1-U bearer" between UE & eNB and eNB & EPC respectively.
   */

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  emm_sap_t                               emm_sap = {0};
  int                                     rc = RETURNerror;
  if (emm_ctx) {
    emm_sap.primitive = EMMAS_ESTABLISH_CNF;
    emm_sap.u.emm_as.u.establish.ue_id = PARENT_STRUCT(emm_ctx, struct ue_mm_context_s, emm_context)->mme_ue_s1ap_id;
    emm_sap.u.emm_as.u.establish.nas_info = EMM_AS_NAS_INFO_NONE;
    emm_sap.u.emm_as.u.establish.encryption = emm_ctx->_security.selected_algorithms.encryption;
    emm_sap.u.emm_as.u.establish.integrity = emm_ctx->_security.selected_algorithms.integrity;
    emm_sap.u.emm_as.u.establish.nas_msg = NULL;
    emm_sap.u.emm_as.u.establish.eps_id.guti = &emm_ctx->_guti;
    rc = emm_sap_send (&emm_sap);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

