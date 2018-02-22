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
Source      emm_proc.h

Version     0.1

Date        2012/10/16

Product     NAS stack

Subsystem   EPS Mobility Management

Author      Frederic Maurel

Description Defines the EPS Mobility Management procedures executed at
        the EMM Service Access Points.

*****************************************************************************/
#ifndef FILE_EMM_PROC_SEEN
#define FILE_EMM_PROC_SEEN

#include "commonDef.h"
#include "bstrlib.h"

#include "EmmCommon.h"
#include "emmData.h"

#include "nas_message.h" //nas_message_decode_status_t

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* Type of network attachment */
typedef enum {
  EMM_ATTACH_TYPE_EPS = 0,
  EMM_ATTACH_TYPE_COMBINED_EPS_IMSI,
  EMM_ATTACH_TYPE_EMERGENCY,
  EMM_ATTACH_TYPE_RESERVED,
} emm_proc_attach_type_t;

/* Type of network detach */
typedef enum {
  EMM_DETACH_TYPE_EPS = 0,
  EMM_DETACH_TYPE_IMSI,
  EMM_DETACH_TYPE_EPS_IMSI,
  EMM_DETACH_TYPE_REATTACH,
  EMM_DETACH_TYPE_NOT_REATTACH,
  EMM_DETACH_TYPE_RESERVED,
} emm_proc_detach_type_t;

/* Type of requested identity */
typedef enum {
  EMM_IDENT_TYPE_NOT_AVAILABLE = 0,
  EMM_IDENT_TYPE_IMSI,
  EMM_IDENT_TYPE_IMEI,
  EMM_IDENT_TYPE_IMEISV,
  EMM_IDENT_TYPE_TMSI
} emm_proc_identity_type_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
 *---------------------------------------------------------------------------
 *              EMM status procedure
 *---------------------------------------------------------------------------
 */
int emm_proc_status_ind(mme_ue_s1ap_id_t ue_id, int emm_cause);
int emm_proc_status(mme_ue_s1ap_id_t ue_id, int emm_cause);

/*
 *---------------------------------------------------------------------------
 *              Lower layer procedure
 *---------------------------------------------------------------------------
 */


/*
 *---------------------------------------------------------------------------
 *              UE's Idle mode procedure
 *---------------------------------------------------------------------------
 */


/*
 * --------------------------------------------------------------------------
 *              Attach procedure
 * --------------------------------------------------------------------------
 */


int emm_proc_attach_request(mme_ue_s1ap_id_t ue_id,
                            emm_proc_attach_type_t type,
                            AdditionalUpdateType additional_update_type,
                            bool is_native_ksi, ksi_t ksi,
                            bool is_native_guti, guti_t *guti,
                            imsi_t *imsi,
                            imei_t *imei, tai_t *last_visited_registered_tai,
                            const tai_t              * const originating_tai,
                            const ecgi_t             * const originating_ecgi,
                            const int eea, const int eia, const int ucs2, const int uea, const int uia, const int gea,
                            const bool umts_present, const bool gprs_present, const_bstring esm_msg,
                            const nas_message_decode_status_t  * const decode_status);

int emm_proc_attach_reject(mme_ue_s1ap_id_t ue_id, int emm_cause);
int emm_proc_attach_complete(mme_ue_s1ap_id_t ue_id, const_bstring esm_msg);

int emm_proc_tracking_area_update_request (
        const mme_ue_s1ap_id_t ue_id,
        const tracking_area_update_request_msg *msg,
        int *emm_cause,
        const nas_message_decode_status_t *decode_status);

int emm_proc_tracking_area_update_reject (
        const mme_ue_s1ap_id_t ue_id,
        const int emm_cause);

int emm_proc_service_reject (const mme_ue_s1ap_id_t ue_id, const int emm_cause);
/*
 * --------------------------------------------------------------------------
 *              Detach procedure
 * --------------------------------------------------------------------------
 */

int emm_proc_detach(mme_ue_s1ap_id_t ue_id, emm_proc_detach_type_t type);
int emm_proc_detach_request(mme_ue_s1ap_id_t ue_id, emm_proc_detach_type_t type,
                            int switch_off, ksi_t native_ksi, ksi_t ksi, guti_t *guti, imsi_t *imsi,
                            imei_t *imei);

/*
 * --------------------------------------------------------------------------
 *              Identification procedure
 * --------------------------------------------------------------------------
 */
int emm_proc_identification(mme_ue_s1ap_id_t                    ue_id,
                            emm_data_context_t            *emm_ctx,
                            emm_proc_identity_type_t       type,
                            emm_common_success_callback_t  success,
                            emm_common_reject_callback_t   reject,
                            emm_common_failure_callback_t  failure);
int emm_proc_identification_complete(const mme_ue_s1ap_id_t ue_id,
                            imsi_t   * const imsi,
                            imei_t   * const imei,
                            imeisv_t * const imeisv,
                            uint32_t * const tmsi);

/*
 * --------------------------------------------------------------------------
 *              Authentication procedure
 * --------------------------------------------------------------------------
 */

int emm_proc_authentication(void *ctx, mme_ue_s1ap_id_t ue_id, ksi_t ksi,
                            const uint8_t   * const rand,
                            const uint8_t   * const autn,
                            emm_common_success_callback_t success,
                            emm_common_reject_callback_t reject,
                            emm_common_failure_callback_t failure); // warning failure unused

int emm_proc_authentication_failure (mme_ue_s1ap_id_t ue_id, int emm_cause,
                                     const_bstring auts);

int emm_proc_authentication_complete(mme_ue_s1ap_id_t ue_id);

int emm_attach_security(void *args);

/*
 * --------------------------------------------------------------------------
 *          Security mode control procedure
 * --------------------------------------------------------------------------
 */

int emm_proc_security_mode_control(const mme_ue_s1ap_id_t ue_id, ksi_t ksi,
    const int eea, int eia,const int ucs2, const int uea, const int uia, const int gea,
    const bool umts_present, const bool gprs_present,
                                   emm_common_success_callback_t success,
                                   emm_common_reject_callback_t reject,
                                   emm_common_failure_callback_t failure);
int emm_proc_security_mode_complete(mme_ue_s1ap_id_t ue_id);
int emm_proc_security_mode_reject(mme_ue_s1ap_id_t ue_id);

void
_clear_emm_ctxt(emm_data_context_t *emm_ctx); 
/*
 *---------------------------------------------------------------------------
 *             Network indication handlers
 *---------------------------------------------------------------------------
 */


#endif /* FILE_EMM_PROC_SEEN*/
