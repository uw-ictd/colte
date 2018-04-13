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

  Source      emm_send.c

  Version     0.1

  Date        2013/01/30

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines functions executed at the EMMAS Service Access
        Point to send EPS Mobility Management messages to the
        Access Stratum sublayer.

*****************************************************************************/
#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "bstrlib.h"

#include "log.h"
#include "assertions.h"
#include "commonDef.h"
#include "common_types.h"
#include "common_defs.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"
#include "3gpp_29.274.h"
#include "3gpp_24.301.h"
#include "emm_msgDef.h"
#include "emm_proc.h"
#include "mme_config.h"
#include "emm_send.h"
#include "emm_data.h"
#include "mme_app_ue_context.h"

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
   Functions executed by both the UE and the MME to send EMM messages
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_status()                                         **
 **                                                                        **
 ** Description: Builds EMM status message                                 **
 **                                                                        **
 **      The EMM status message is sent by the UE or the network   **
 **      at any time to report certain error conditions.           **
 **                                                                        **
 ** Inputs:  emm_cause: EMM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_status (
  const emm_as_status_t * msg,
  emm_status_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Send EMM Status message (cause=%d)\n", msg->emm_cause);
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = EMM_STATUS;
  /*
   * Mandatory - EMM cause
   */
  size += EMM_CAUSE_MAXIMUM_LENGTH;
  emm_msg->emmcause = msg->emm_cause;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_detach_accept()                                  **
 **                                                                        **
 ** Description: Builds Detach Accept message                              **
 **                                                                        **
 **      The Detach Accept message is sent by the UE or the net-   **
 **      work to indicate that the detach procedure has been com-  **
 **      pleted.                                                   **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_detach_accept (
  const emm_as_data_t * msg,
  detach_accept_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Detach Accept message\n");
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = DETACH_ACCEPT;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}


/*
   --------------------------------------------------------------------------
   Functions executed by the MME to send EMM messages to the UE
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_attach_accept()                                  **
 **                                                                        **
 ** Description: Builds Attach Accept message                              **
 **                                                                        **
 **      The Attach Accept message is sent by the network to the   **
 **      UE to indicate that the corresponding attach request has  **
 **      been accepted.                                            **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_attach_accept (
  const emm_as_establish_t * msg,
  attach_accept_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  // Get the UE context
  emm_context_t *emm_ctx = emm_context_get (&_emm_data, msg->ue_id);
  DevAssert(emm_ctx);
  mme_ue_s1ap_id_t ue_id = PARENT_STRUCT(emm_ctx, struct ue_mm_context_s, emm_context)->mme_ue_s1ap_id;
  DevAssert(msg->ue_id == ue_id);

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Attach Accept message\n");
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size = EMM_HEADER_MAXIMUM_LENGTH(%d)\n", size);
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = ATTACH_ACCEPT;
  /*
   * Mandatory - EPS attach result
   */
  size += EPS_ATTACH_RESULT_MAXIMUM_LENGTH;
  OAILOG_INFO (
      LOG_NAS_EMM,
      "EMMAS-SAP - size += EPS_ATTACH_RESULT_MAXIMUM_LENGTH(%d)  (%d)\n",
      EPS_ATTACH_RESULT_MAXIMUM_LENGTH, size);
  switch (emm_ctx->attach_type) {
  case EMM_ATTACH_TYPE_COMBINED_EPS_IMSI:
    OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - Combined EPS/IMSI attach\n");
    /* It is observed that UE/Handest (with usage setting = voice centric and voice domain preference = CS voice only) sends detach after
     * successful attach .UEs with such settings sends attach type = combined EPS and IMSI attach as attach_type in
     * attach request message. At present, EPC does not support interface with MSC /CS domain and it supports only LTE data service, hence it
     * is supposed to send attach_result as EPS-only and add emm_cause = "CS domain not available" for such cases.
     * Ideally in data service only n/w ,UE's usage setting should be set to data centric mode and should send attach type as EPS attach only. 
     * However UE settings may not be in our control. To take care of this as a workaround in this patch we modified MME
     * implementation to set EPS result to Combined attach if attach type is combined attach to prevent such UEs from
     * sending detach so that such UEs can remain attached in the n/w and should be able to get data service from the n/w.
     */
    emm_msg->epsattachresult = EPS_ATTACH_RESULT_EPS_IMSI;
    break;
  case EMM_ATTACH_TYPE_RESERVED:
  default:
    OAILOG_DEBUG (LOG_NAS_EMM,
                  "EMMAS-SAP - Unused attach type defaults to EPS attach\n");
  case EMM_ATTACH_TYPE_EPS:
    emm_msg->epsattachresult = EPS_ATTACH_RESULT_EPS;
    OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - EPS attach\n");
    break;
  case EMM_ATTACH_TYPE_EMERGENCY:  // We should not reach here
    OAILOG_ERROR (LOG_NAS_EMM,
                  "EMMAS-SAP - EPS emergency attach, currently unsupported\n");
    emm_context_unlock(emm_ctx);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);  // TODO: fix once supported
    break;
  }
  /*
   * Mandatory - T3412 value
   */
  size += GPRS_TIMER_IE_MAX_LENGTH;
  // Check whether Periodic TAU timer is disabled 
  if (mme_config.nas_config.t3412_min == 0) {
    emm_msg->t3412value.unit = GPRS_TIMER_UNIT_0S;
    emm_msg->t3412value.timervalue = mme_config.nas_config.t3412_min;
  } else if (mme_config.nas_config.t3412_min <= 31) {
    emm_msg->t3412value.unit = GPRS_TIMER_UNIT_60S;
    emm_msg->t3412value.timervalue = mme_config.nas_config.t3412_min;
  } else {
    emm_msg->t3412value.unit = GPRS_TIMER_UNIT_360S;
    emm_msg->t3412value.timervalue = mme_config.nas_config.t3412_min / 6;
  }
  //emm_msg->t3412value.unit = GPRS_TIMER_UNIT_0S;
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += GPRS_TIMER_IE_MAX_LENGTH(%d)  (%d)\n", GPRS_TIMER_IE_MAX_LENGTH, size);
  /*
   * Mandatory - Tracking area identity list
   */
  size += TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH * msg->tai_list.numberoflists;
  memcpy(&emm_msg->tailist, &msg->tai_list, sizeof(msg->tai_list));
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING_AREA_IDENTITY_LIST_LENGTH(%d*%d)  (%d)\n",
      TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH, emm_msg->tailist.numberoflists, size);
  AssertFatal(emm_msg->tailist.numberoflists <= 16, "Too many TAIs in TAI list");
  for (int p = 0; p < emm_msg->tailist.numberoflists; p++) {
    if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS == emm_msg->tailist.partial_tai_list[p].typeoflist) {
      size = size + (2*emm_msg->tailist.partial_tai_list[p].numberofelements);
      OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING AREA CODE LENGTH(%d*%d)  (%d)\n", 2, emm_msg->tailist.partial_tai_list[p].numberofelements, size);
    } else if (TRACKING_AREA_IDENTITY_LIST_MANY_PLMNS== emm_msg->tailist.partial_tai_list[p].typeoflist) {
      size = size + (5*emm_msg->tailist.partial_tai_list[p].numberofelements);
      OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING AREA CODE LENGTH(%d*%d)  (%d)\n", 5, emm_msg->tailist.partial_tai_list[p].numberofelements, size);
    }
  }
  /*
   * Mandatory - ESM message container
   */
  size += ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH + blength(msg->nas_msg);
  emm_msg->esmmessagecontainer = bstrcpy(msg->nas_msg);
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH(%d)  (%d)\n", ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH, size);

  /*
   * Optional - GUTI
   */
  if (msg->new_guti) {
    size += EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH;
    emm_msg->presencemask |= ATTACH_ACCEPT_GUTI_PRESENT;
    emm_msg->guti.guti.typeofidentity = EPS_MOBILE_IDENTITY_GUTI;
    emm_msg->guti.guti.oddeven = EPS_MOBILE_IDENTITY_EVEN;
    emm_msg->guti.guti.mme_group_id = msg->new_guti->gummei.mme_gid;
    emm_msg->guti.guti.mme_code = msg->new_guti->gummei.mme_code;
    emm_msg->guti.guti.m_tmsi = msg->new_guti->m_tmsi;
    emm_msg->guti.guti.mcc_digit1 = msg->new_guti->gummei.plmn.mcc_digit1;
    emm_msg->guti.guti.mcc_digit2 = msg->new_guti->gummei.plmn.mcc_digit2;
    emm_msg->guti.guti.mcc_digit3 = msg->new_guti->gummei.plmn.mcc_digit3;
    emm_msg->guti.guti.mnc_digit1 = msg->new_guti->gummei.plmn.mnc_digit1;
    emm_msg->guti.guti.mnc_digit2 = msg->new_guti->gummei.plmn.mnc_digit2;
    emm_msg->guti.guti.mnc_digit3 = msg->new_guti->gummei.plmn.mnc_digit3;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH(%d)  (%d)\n", EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH, size);
  }

  /*
   * Optional - T3402
   */
  if (msg->t3402) {
    size += GPRS_TIMER_IE_MAX_LENGTH;
    emm_msg->presencemask |= ATTACH_ACCEPT_T3402_VALUE_PRESENT;
    if (mme_config.nas_config.t3402_min <= 31) {
      emm_msg->t3402value.unit = GPRS_TIMER_UNIT_60S;
      emm_msg->t3402value.timervalue = mme_config.nas_config.t3402_min;
    } else  {
      emm_msg->t3402value.unit = GPRS_TIMER_UNIT_360S;
      emm_msg->t3402value.timervalue = mme_config.nas_config.t3402_min / 6;
    }
  }

  emm_context_unlock(emm_ctx);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}
/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_attach_accept_dl_nas()                               **
 **                                                                        **
 ** Description: Builds Attach Accept message to be sent is S1AP:DL NAS Tx **
 **                                                                        **
 **      The Attach Accept message is sent by the network to the           **
 **      UE to indicate that the corresponding attach request has          **
 **      been accepted.                                                    **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process                 **
 **      Others:    None                                                   **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                     **
 **      Return:    The size of the EMM message                            **
 **      Others:    None                                                   **
 **                                                                        **
 ***************************************************************************/
int
emm_send_attach_accept_dl_nas (
  const emm_as_data_t * msg,
  attach_accept_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  // Get the UE context
  emm_context_t *emm_ctx = emm_context_get (&_emm_data, msg->ue_id);
  DevAssert(emm_ctx);
  mme_ue_s1ap_id_t ue_id = PARENT_STRUCT(emm_ctx, struct ue_mm_context_s, emm_context)->mme_ue_s1ap_id;
  DevAssert(msg->ue_id == ue_id);

  OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - Send Attach Accept message\n");
  OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - size = EMM_HEADER_MAXIMUM_LENGTH(%d)\n", size);
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = ATTACH_ACCEPT;
  /*
   * Mandatory - EPS attach result
   */
  size += EPS_ATTACH_RESULT_MAXIMUM_LENGTH;
  OAILOG_INFO (
      LOG_NAS_EMM,
      "EMMAS-SAP - size += EPS_ATTACH_RESULT_MAXIMUM_LENGTH(%d)  (%d)\n",
      EPS_ATTACH_RESULT_MAXIMUM_LENGTH, size);
  switch (emm_ctx->attach_type) {
  case EMM_ATTACH_TYPE_COMBINED_EPS_IMSI:
    OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - Combined EPS/IMSI attach\n");
    /* It is observed that UE/Handest (with usage setting = voice centric and voice domain preference = CS voice only) sends detach after
     * successful attach. UEs with such settings sends attach type = combined EPS and IMSI attach as attach_type in
     * attach request message. At present, EPC does not support interface with MSC /CS domain and it supports only LTE data service, hence it
     * is supposed to send attach_result as EPS-only and add emm_cause = "CS domain not available" for such cases. 
     * Ideally in data service only n/w ,UE's usage setting should be set to data centric mode and should send attach type as EPS attach only. 
     * However UE settings may not be in our control. To take care of this as a workaround in this patch we modified MME
     * implementation to set EPS result to Combined attach if attach type is combined attach to prevent such UEs from
     * sending detach so that such UEs can remain attached in the n/w and should be able to get data service from the n/w.
     */
    emm_msg->epsattachresult = EPS_ATTACH_RESULT_EPS_IMSI;
    break;
  case EMM_ATTACH_TYPE_RESERVED:
  default:
    OAILOG_DEBUG (LOG_NAS_EMM,
                  "EMMAS-SAP - Unused attach type defaults to EPS attach\n");
  case EMM_ATTACH_TYPE_EPS:
    emm_msg->epsattachresult = EPS_ATTACH_RESULT_EPS;
    OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - EPS attach\n");
    break;
  case EMM_ATTACH_TYPE_EMERGENCY:  // We should not reach here
    OAILOG_ERROR (LOG_NAS_EMM,
                  "EMMAS-SAP - EPS emergency attach, currently unsupported\n");
    emm_context_unlock(emm_ctx);
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);  // TODO: fix once supported
    break;
  }
  /*
   * Mandatory - T3412 value
   */
  size += GPRS_TIMER_IE_MAX_LENGTH;
  // Check whether Periodic TAU timer is disabled 
  if (mme_config.nas_config.t3412_min == 0) {
    emm_msg->t3412value.unit = GPRS_TIMER_UNIT_0S;
    emm_msg->t3412value.timervalue = mme_config.nas_config.t3412_min;
  } else if (mme_config.nas_config.t3412_min <= 31) {
    emm_msg->t3412value.unit = GPRS_TIMER_UNIT_60S;
    emm_msg->t3412value.timervalue = mme_config.nas_config.t3412_min;
  } else {
    emm_msg->t3412value.unit = GPRS_TIMER_UNIT_360S;
    emm_msg->t3412value.timervalue = mme_config.nas_config.t3412_min / 6;
  }
  //emm_msg->t3412value.unit = GPRS_TIMER_UNIT_0S;
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += GPRS_TIMER_MAXIMUM_LENGTH(%d)  (%d)\n", GPRS_TIMER_IE_MAX_LENGTH, size);
  /*
   * Mandatory - Tracking area identity list
   */
  size += TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH * msg->tai_list.numberoflists;
  memcpy(&emm_msg->tailist, &msg->tai_list, sizeof(msg->tai_list));
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING_AREA_IDENTITY_LIST_LENGTH(%d*%d)  (%d)\n",
      TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH, emm_msg->tailist.numberoflists, size);
  AssertFatal(emm_msg->tailist.numberoflists <= 16, "Too many TAIs in TAI list");
  for (int p = 0; p < emm_msg->tailist.numberoflists; p++) {
    if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS == emm_msg->tailist.partial_tai_list[p].typeoflist) {
      size = size + (2*emm_msg->tailist.partial_tai_list[p].numberofelements);
      OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING AREA CODE LENGTH(%d*%d)  (%d)\n", 2, emm_msg->tailist.partial_tai_list[p].numberofelements, size);
    } else if (TRACKING_AREA_IDENTITY_LIST_MANY_PLMNS== emm_msg->tailist.partial_tai_list[p].typeoflist) {
      size = size + (5*emm_msg->tailist.partial_tai_list[p].numberofelements);
      OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING AREA CODE LENGTH(%d*%d)  (%d)\n", 5, emm_msg->tailist.partial_tai_list[p].numberofelements, size);
    }
  }
  /*
   * Mandatory - ESM message container
   */
  size += ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH + blength(msg->nas_msg);
  emm_msg->esmmessagecontainer = bstrcpy(msg->nas_msg);
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH(%d)  (%d)\n", ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH, size);

  /*
   * Optional - GUTI
   */
  if (msg->new_guti) {
    size += EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH;
    emm_msg->presencemask |= ATTACH_ACCEPT_GUTI_PRESENT;
    emm_msg->guti.guti.typeofidentity = EPS_MOBILE_IDENTITY_GUTI;
    emm_msg->guti.guti.oddeven = EPS_MOBILE_IDENTITY_EVEN;
    emm_msg->guti.guti.mme_group_id = msg->new_guti->gummei.mme_gid;
    emm_msg->guti.guti.mme_code = msg->new_guti->gummei.mme_code;
    emm_msg->guti.guti.m_tmsi = msg->new_guti->m_tmsi;
    emm_msg->guti.guti.mcc_digit1 = msg->new_guti->gummei.plmn.mcc_digit1;
    emm_msg->guti.guti.mcc_digit2 = msg->new_guti->gummei.plmn.mcc_digit2;
    emm_msg->guti.guti.mcc_digit3 = msg->new_guti->gummei.plmn.mcc_digit3;
    emm_msg->guti.guti.mnc_digit1 = msg->new_guti->gummei.plmn.mnc_digit1;
    emm_msg->guti.guti.mnc_digit2 = msg->new_guti->gummei.plmn.mnc_digit2;
    emm_msg->guti.guti.mnc_digit3 = msg->new_guti->gummei.plmn.mnc_digit3;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH(%d)  (%d)\n", EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH, size);
  }

  emm_context_unlock(emm_ctx);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_attach_reject()                                  **
 **                                                                        **
 ** Description: Builds Attach Reject message                              **
 **                                                                        **
 **      The Attach Reject message is sent by the network to the   **
 **      UE to indicate that the corresponding attach request has  **
 **      been rejected.                                            **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_attach_reject (
  const emm_as_establish_t * msg,
  attach_reject_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Attach Reject message (cause=%d)\n", msg->emm_cause);
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = ATTACH_REJECT;
  /*
   * Mandatory - EMM cause
   */
  size += EMM_CAUSE_MAXIMUM_LENGTH;
  emm_msg->emmcause = msg->emm_cause;

  /*
   * Optional - ESM message container
   */
  if (msg->nas_msg) {
    size += ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH + blength(msg->nas_msg) + 1; // Adding 1 byte since ESM Container is optional IE in Attach Reject
    emm_msg->presencemask |= ATTACH_REJECT_ESM_MESSAGE_CONTAINER_PRESENT;
    emm_msg->esmmessagecontainer = msg->nas_msg;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:        emm_send_tracking_area_update_accept()                    **
 **                                                                        **
 ** Description: Builds Tracking Area Update Accept message                **
 **                                                                        **
 **              The Tracking Area Update Accept message is sent by the    **
 **              network to the UE to indicate that the corresponding      **
 **              tracking area update has been accepted.                   **
 **              This function is used to send TAU Accept message together **
 **              with Initial context setup request message to establish   **
 **              radio bearers as well.                                    **
 **                                                                        **
 ** Inputs:      msg:           The EMMAS-SAP primitive to process         **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:       The EMM message to be sent                 **
 **              Return:        The size of the EMM message                **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_tracking_area_update_accept (
  const emm_as_establish_t * msg,
  tracking_area_update_accept_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Tracking Area Update Accept message (cause=%d)\n", msg->emm_cause);
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = TRACKING_AREA_UPDATE_ACCEPT;
  /*
   * Mandatory - EMM cause
   */
  size += EPS_UPDATE_RESULT_MAXIMUM_LENGTH;
  emm_msg->epsupdateresult = msg->eps_update_result;
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += EPS_UPDATE_RESULT_MAXIMUM_LENGTH(%d)  (%d)\n", EPS_UPDATE_RESULT_MAXIMUM_LENGTH, size);

  // Optional - GPRS Timer T3412
  if ((msg->t3412) && (*msg->t3412)) {
    size += GPRS_TIMER_IE_MAX_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_T3412_VALUE_PRESENT;
    if (*msg->t3412 <= 31) {
      emm_msg->t3412value.unit = GPRS_TIMER_UNIT_60S;
      emm_msg->t3412value.timervalue = *msg->t3412;
    } else  {
      emm_msg->t3412value.unit = GPRS_TIMER_UNIT_360S;
      emm_msg->t3412value.timervalue = *msg->t3412 / 6;
    }
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "GPRS_TIMER_IE_MAX_LENGTH(%d)  (%d)\n", GPRS_TIMER_IE_MAX_LENGTH, size);
  }
  // Optional - GUTI
  if (msg->new_guti) {
    size += EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH;
    emm_msg->presencemask |= ATTACH_ACCEPT_GUTI_PRESENT;
    emm_msg->guti.guti.typeofidentity = EPS_MOBILE_IDENTITY_GUTI;
    emm_msg->guti.guti.oddeven = EPS_MOBILE_IDENTITY_EVEN;
    emm_msg->guti.guti.mme_group_id = msg->guti->gummei.mme_gid;
    emm_msg->guti.guti.mme_code = msg->guti->gummei.mme_code;
    emm_msg->guti.guti.m_tmsi = msg->guti->m_tmsi;
    emm_msg->guti.guti.mcc_digit1 = msg->guti->gummei.plmn.mcc_digit1;
    emm_msg->guti.guti.mcc_digit2 = msg->guti->gummei.plmn.mcc_digit2;
    emm_msg->guti.guti.mcc_digit3 = msg->guti->gummei.plmn.mcc_digit3;
    emm_msg->guti.guti.mnc_digit1 = msg->guti->gummei.plmn.mnc_digit1;
    emm_msg->guti.guti.mnc_digit2 = msg->guti->gummei.plmn.mnc_digit2;
    emm_msg->guti.guti.mnc_digit3 = msg->guti->gummei.plmn.mnc_digit3;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH(%d)  (%d)\n", EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH, size);
  }
  /* Optional - TAI list
   * This IE may be included to assign a TAI list to a UE.
   */
  if (msg->tai_list.numberoflists > 0) {
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_TAI_LIST_PRESENT;

    size += TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH * msg->tai_list.numberoflists;
    memcpy(&emm_msg->tailist, &msg->tai_list, sizeof(msg->tai_list));
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING_AREA_IDENTITY_LIST_LENGTH(%d*%d)  (%d)\n",
        TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH, emm_msg->tailist.numberoflists, size);
    AssertFatal(emm_msg->tailist.numberoflists <= 16, "Too many TAIs in TAI list");
    for (int p = 0; p < emm_msg->tailist.numberoflists; p++) {
      if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS == emm_msg->tailist.partial_tai_list[p].typeoflist) {
        size = size + (2*emm_msg->tailist.partial_tai_list[p].numberofelements);
        OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING AREA CODE LENGTH(%d*%d)  (%d)\n", 2, emm_msg->tailist.partial_tai_list[p].numberofelements, size);
      } else if (TRACKING_AREA_IDENTITY_LIST_MANY_PLMNS == emm_msg->tailist.partial_tai_list[p].typeoflist) {
        size = size + (5*emm_msg->tailist.partial_tai_list[p].numberofelements);
        OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "TRACKING AREA CODE LENGTH(%d*%d)  (%d)\n", 5, emm_msg->tailist.partial_tai_list[p].numberofelements, size);
      }
    }
  }
  // Optional - EPS Bearer context status
  if (msg->eps_bearer_context_status) {
    size += EPS_BEARER_CONTEXT_STATUS_MAXIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_EPS_BEARER_CONTEXT_STATUS_PRESENT;
    emm_msg->epsbearercontextstatus = *(msg->eps_bearer_context_status);
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "EPS_BEARER_CONTEXT_STATUS_MAXIMUM_LENGTH(%d)  (%d)\n", EPS_BEARER_CONTEXT_STATUS_MAXIMUM_LENGTH, size);
  }
  /* Useless actually, Optional - Location Area Identification
  if (msg->location_area_identification) {
    size += LOCATION_AREA_IDENTIFICATION_MAXIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_LOCATION_AREA_IDENTIFICATION_PRESENT;
    emm_msg->locationareaidentification = ;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "LOCATION_AREA_IDENTIFICATION_MAXIMUM_LENGTH(%d)  (%d)", LOCATION_AREA_IDENTIFICATION_MAXIMUM_LENGTH, size);
  }*/
  /* Useless actually, Optional - Mobile Identity
  if (msg->mobile_identity) {
    size += MOBILE_IDENTITY_MINIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_MS_IDENTITY_PRESENT;
    emm_msg->msidentity = ;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "MOBILE_IDENTITY_MINIMUM_LENGTH(%d)  (%d)", MOBILE_IDENTITY_MINIMUM_LENGTH, size);
  }*/
  // Optional - EMM cause
  if (msg->emm_cause) {
    size += EMM_CAUSE_MAXIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_EMM_CAUSE_PRESENT;
    emm_msg->emmcause = msg->emm_cause;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "MOBILE_IDENTITY_MINIMUM_LENGTH(%d)  (%d)\n", EMM_CAUSE_MAXIMUM_LENGTH, size);
  }
  // Optional - GPRS Timer T3402
  if (msg->t3402) {
    size += GPRS_TIMER_IE_MAX_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_T3402_VALUE_PRESENT;
    if (*msg->t3402 <= 31) {
      emm_msg->t3402value.unit = GPRS_TIMER_UNIT_60S;
      emm_msg->t3402value.timervalue = *msg->t3402;
    } else  {
      emm_msg->t3402value.unit = GPRS_TIMER_UNIT_360S;
      emm_msg->t3402value.timervalue = *msg->t3402 / 6;
    }
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "GPRS_TIMER_IE_MAX_LENGTH(%d)  (%d)\n", GPRS_TIMER_IE_MAX_LENGTH, size);
  }
  // Optional - GPRS Timer T3423
  if (msg->t3423) {
    size += GPRS_TIMER_IE_MAX_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_T3423_VALUE_PRESENT;
    if (*msg->t3423 <= 31) {
      emm_msg->t3423value.unit = GPRS_TIMER_UNIT_60S;
      emm_msg->t3423value.timervalue = *msg->t3423;
    } else  {
      emm_msg->t3423value.unit = GPRS_TIMER_UNIT_360S;
      emm_msg->t3423value.timervalue = *msg->t3423 / 6;
    }
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "GPRS_TIMER_IE_MAX_LENGTH(%d)  (%d)\n", GPRS_TIMER_IE_MAX_LENGTH, size);
  }
  // Useless actually, Optional - Equivalent PLMNs
  /*if (msg->equivalent_plmns) {
    size += PLMN_LIST_MINIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_EQUIVALENT_PLMNS_PRESENT;
    emm_msg->equivalentplmns.       = msg->;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "PLMN_LIST_MINIMUM_LENGTH(%d)  (%d)", PLMN_LIST_MINIMUM_LENGTH, size);
  }*/
  /* Useless actually, Optional - Emergency number list
  if (msg->emergency_number_list) {
    size += EMERGENCY_NUMBER_LIST_MINIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_EMERGENCY_NUMBER_LIST_PRESENT;
    emm_msg->emergencynumberlist.       = msg->;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "EMERGENCY_NUMBER_LIST_MINIMUM_LENGTH(%d)  (%d)", EMERGENCY_NUMBER_LIST_MINIMUM_LENGTH, size);
  }*/
  // Optional - EPS network feature support
  if (msg->eps_network_feature_support) {
    size += EPS_NETWORK_FEATURE_SUPPORT_MAXIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_EPS_NETWORK_FEATURE_SUPPORT_PRESENT;
    emm_msg->epsnetworkfeaturesupport       = *msg->eps_network_feature_support;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "EPS_NETWORK_FEATURE_SUPPORT_MAXIMUM_LENGTH(%d)  (%d)\n", EPS_NETWORK_FEATURE_SUPPORT_MAXIMUM_LENGTH, size);
  }
  /* Useless actually, Optional - Additional update result
  if (msg->additional_update_result) {
    size += ADDITIONAL_UPDATE_RESULT_MAXIMUM_LENGTH;
    emm_msg->presencemask |= TRACKING_AREA_UPDATE_ACCEPT_ADDITIONAL_UPDATE_RESULT_PRESENT;
    emm_msg->additionalupdateresult.       = msg->;
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - size += " "ADDITIONAL_UPDATE_RESULT_MAXIMUM_LENGTH(%d)  (%d)", ADDITIONAL_UPDATE_RESULT_MAXIMUM_LENGTH, size);
  }*/
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}
/****************************************************************************
 **                                                                        **
 ** Name:        emm_send_tracking_area_update_accept_dl_nas()             **
 **                                                                        **
 ** Description: Builds Tracking Area Update Accept message                **
 **                                                                        **
 **              The Tracking Area Update Accept message is sent by the    **
 **              network to the UE to indicate that the corresponding      **
 **              tracking area update has been accepted.                   **
 **              This function is used to send TAU Accept message via      **
 **              S1AP DL NAS Transport message.                            **
 **                                                                        **
 ** Inputs:      msg:           The EMMAS-SAP primitive to process         **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:       The EMM message to be sent                 **
 **              Return:        The size of the EMM message                **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/

int
emm_send_tracking_area_update_accept_dl_nas (
  const emm_as_data_t * msg,
  tracking_area_update_accept_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = TRACKING_AREA_UPDATE_ACCEPT;
  /*
   * Mandatory - EMM cause
   */
  size += EPS_UPDATE_RESULT_MAXIMUM_LENGTH;
  emm_msg->epsupdateresult = EPS_UPDATE_RESULT_TA_UPDATED;
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Sending DL NAS - TAU Accept\n");
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);

}

/****************************************************************************
 **                                                                        **
 ** Name:        emm_send_tracking_area_update_reject()                    **
 **                                                                        **
 ** Description: Builds Tracking Area Update Reject message                **
 **                                                                        **
 **              The Tracking Area Update Reject message is sent by the    **
 **              network to the UE to indicate that the corresponding      **
 **              tracking area update has been rejected.                   **
 **                                                                        **
 ** Inputs:      msg:           The EMMAS-SAP primitive to process         **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:       The EMM message to be sent                 **
 **              Return:        The size of the EMM message                **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_tracking_area_update_reject (
  const emm_as_establish_t * msg,
  tracking_area_update_reject_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Tracking Area Update Reject message (cause=%d)\n", msg->emm_cause);
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = TRACKING_AREA_UPDATE_REJECT;
  /*
   * Mandatory - EMM cause
   */
  size += EMM_CAUSE_MAXIMUM_LENGTH;
  emm_msg->emmcause = msg->emm_cause;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:        emm_send_service_reject()                                 **
 **                                                                        **
 ** Description: Builds Service Reject message                             **
 **                                                                        **
 **              The Tracking Area Update Reject message is sent by the    **
 **              network to the UE to indicate that the corresponding      **
 **              tracking area update has been rejected.                   **
 **                                                                        **
 ** Inputs:      msg:           The EMMAS-SAP primitive to process         **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:       The EMM message to be sent                 **
 **              Return:        The size of the EMM message                **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_service_reject (
  const emm_as_establish_t * msg,
  service_reject_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Service Reject message (cause=%d)\n", msg->emm_cause);
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = SERVICE_REJECT;
  /*
   * Mandatory - EMM cause
   */
  size += EMM_CAUSE_MAXIMUM_LENGTH;
  emm_msg->emmcause = msg->emm_cause;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_identity_request()                               **
 **                                                                        **
 ** Description: Builds Identity Request message                           **
 **                                                                        **
 **      The Identity Request message is sent by the network to    **
 **      the UE to request the UE to provide the specified identi- **
 **      ty.                                                       **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_identity_request (
  const emm_as_security_t * msg,
  identity_request_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Identity Request message\n");
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = IDENTITY_REQUEST;
  /*
   * Mandatory - Identity type 2
   */
  size += IDENTITY_TYPE_2_IE_MAX_LENGTH;

  if (msg->ident_type == IDENTITY_TYPE_2_IMSI) {
    emm_msg->identitytype = IDENTITY_TYPE_2_IMSI;
  } else if (msg->ident_type == IDENTITY_TYPE_2_TMSI) {
    emm_msg->identitytype = IDENTITY_TYPE_2_TMSI;
  } else if (msg->ident_type == IDENTITY_TYPE_2_IMEI) {
    emm_msg->identitytype = IDENTITY_TYPE_2_IMEI;
  } else {
    /*
     * All other values are interpreted as "IMSI"
     */
    emm_msg->identitytype = IDENTITY_TYPE_2_IMSI;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_authentication_request()                         **
 **                                                                        **
 ** Description: Builds Authentication Request message                     **
 **                                                                        **
 **      The Authentication Request message is sent by the network **
 **      to the UE to initiate authentication of the UE identity.  **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_authentication_request (
  const emm_as_security_t * msg,
  authentication_request_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Authentication Request message\n");
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = AUTHENTICATION_REQUEST;
  /*
   * Mandatory - NAS key set identifier
   */
  size += NAS_KEY_SET_IDENTIFIER_MAXIMUM_LENGTH;
  emm_msg->naskeysetidentifierasme.tsc = NAS_KEY_SET_IDENTIFIER_NATIVE;
  emm_msg->naskeysetidentifierasme.naskeysetidentifier = msg->ksi;
  /*
   * Mandatory - Authentication parameter RAND
   */
  size += AUTHENTICATION_PARAMETER_RAND_IE_MAX_LENGTH;
  emm_msg->authenticationparameterrand = blk2bstr((const void *)msg->rand, AUTH_RAND_SIZE);
  if (!emm_msg->authenticationparameterrand) {
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
  }
  /*
   * Mandatory - Authentication parameter AUTN
   */
  size += AUTHENTICATION_PARAMETER_AUTN_IE_MAX_LENGTH;
  emm_msg->authenticationparameterautn = blk2bstr((const void *)msg->autn, AUTH_AUTN_SIZE);
  if (!emm_msg->authenticationparameterautn) {
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_authentication_reject()                          **
 **                                                                        **
 ** Description: Builds Authentication Reject message                      **
 **                                                                        **
 **      The Authentication Reject message is sent by the network  **
 **      to the UE to indicate that the authentication procedure   **
 **      has failed and that the UE shall abort all activities.    **
 **                                                                        **
 ** Inputs:  None                                                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_authentication_reject (
  authentication_reject_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Authentication Reject message\n");
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = AUTHENTICATION_REJECT;
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_send_security_mode_command()                          **
 **                                                                        **
 ** Description: Builds Security Mode Command message                      **
 **                                                                        **
 **      The Security Mode Command message is sent by the network  **
 **      to the UE to establish NAS signalling security.           **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_msg:   The EMM message to be sent                 **
 **      Return:    The size of the EMM message                **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
emm_send_security_mode_command (
  const emm_as_security_t * msg,
  security_mode_command_msg * emm_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = EMM_HEADER_MAXIMUM_LENGTH;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send Security Mode Command message\n");
  /*
   * Mandatory - Message type
   */
  emm_msg->messagetype = SECURITY_MODE_COMMAND;
  /*
   * Selected NAS security algorithms
   */
  size += NAS_SECURITY_ALGORITHMS_MAXIMUM_LENGTH;
  emm_msg->selectednassecurityalgorithms.typeofcipheringalgorithm = msg->selected_eea;
  emm_msg->selectednassecurityalgorithms.typeofintegrityalgorithm = msg->selected_eia;
  /*
   * NAS key set identifier
   */
  size += NAS_KEY_SET_IDENTIFIER_MAXIMUM_LENGTH;
  emm_msg->naskeysetidentifier.tsc = NAS_KEY_SET_IDENTIFIER_NATIVE;
  emm_msg->naskeysetidentifier.naskeysetidentifier = msg->ksi;
  /*
   * Replayed UE security capabilities
   */
  size += UE_SECURITY_CAPABILITY_MAXIMUM_LENGTH;
  emm_msg->replayeduesecuritycapabilities.eea = msg->eea;
  emm_msg->replayeduesecuritycapabilities.eia = msg->eia;
  emm_msg->replayeduesecuritycapabilities.umts_present = msg->umts_present;
  emm_msg->replayeduesecuritycapabilities.gprs_present = msg->gprs_present;
  emm_msg->replayeduesecuritycapabilities.uea = msg->uea;
  emm_msg->replayeduesecuritycapabilities.uia = msg->uia;
  emm_msg->replayeduesecuritycapabilities.gea = msg->gea;
  emm_msg->presencemask = 0;

  if (msg->imeisv_request) {
    size += IMEISV_REQUEST_IE_MAX_LENGTH;
    emm_msg->presencemask |= SECURITY_MODE_COMMAND_IMEISV_REQUEST_PRESENT;
    emm_msg->imeisvrequest = IMEISV_REQUESTED;
    OAILOG_INFO (LOG_NAS_EMM, "imeisvrequest                               %d\n", emm_msg->imeisvrequest);
  }
  OAILOG_INFO (LOG_NAS_EMM, "replayeduesecuritycapabilities.gprs_present %d\n", emm_msg->replayeduesecuritycapabilities.gprs_present);
  OAILOG_INFO (LOG_NAS_EMM, "replayeduesecuritycapabilities.gea          %d\n", emm_msg->replayeduesecuritycapabilities.gea);

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, size);
}


/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
