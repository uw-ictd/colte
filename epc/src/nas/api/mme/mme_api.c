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
  Source      mme_api.c

  Version     0.1

  Date        2013/02/28

  Product     NAS stack

  Subsystem   Application Programming Interface

  Author      Frederic Maurel

  Description Implements the API used by the NAS layer running in the MME
        to interact with a Mobility Management Entity

*****************************************************************************/


#include "common_types.h"
#include "log.h"
#include "mme_api.h"
#include "assertions.h"
#include "conversions.h"
#include "sgw_ie_defs.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_config.h"
#include <string.h>             // memcpy

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/
extern mme_app_desc_t                   mme_app_desc;

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* Maximum number of PDN connections the MME may simultaneously support */
#define MME_API_PDN_MAX         10

static mme_api_ip_version_t             _mme_api_ip_capability = MME_API_IPV4V6_ADDR;


/* Subscribed QCI */
#define MME_API_QCI     3

/* Data bit rates */
#define MME_API_BIT_RATE_64K    0x40
#define MME_API_BIT_RATE_128K   0x48
#define MME_API_BIT_RATE_512K   0x78
#define MME_API_BIT_RATE_1024K  0x87

/* Total number of PDN connections (should not exceed MME_API_PDN_MAX) */
static int                              _mme_api_pdn_id = 0;

static tmsi_t                           mme_m_tmsi_generator = 0x00000001;

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    mme_api_get_emm_config()                                  **
 **                                                                        **
 ** Description: Retreives MME configuration data related to EPS mobility  **
 **      management                                                **
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
mme_api_get_emm_config (
  mme_api_emm_config_t * config,
  mme_config_t * mme_config_p)
{
  int                                     i;
  OAILOG_FUNC_IN (LOG_NAS);
  AssertFatal (mme_config_p->served_tai.nb_tai >= 1, "No TAI configured");
  AssertFatal (mme_config_p->gummei.nb >= 1, "No GUMMEI configured");

  config->tai_list.n_tais = 0;
  for (i = 0; i < mme_config_p->served_tai.nb_tai; i++) {
    config->tai_list.tai[i].plmn.mcc_digit1 = (mme_config_p->served_tai.plmn_mcc[i] / 100) % 10;
    config->tai_list.tai[i].plmn.mcc_digit2 = (mme_config_p->served_tai.plmn_mcc[i] / 10) % 10;
    config->tai_list.tai[i].plmn.mcc_digit3 = mme_config_p->served_tai.plmn_mcc[i] % 10;

    if (mme_config_p->served_tai.plmn_mnc_len[0] == 2) {
      config->tai_list.tai[i].plmn.mnc_digit1 = (mme_config_p->served_tai.plmn_mnc[0] / 10) % 10;
      config->tai_list.tai[i].plmn.mnc_digit2 = mme_config_p->served_tai.plmn_mnc[0] % 10;
      config->tai_list.tai[i].plmn.mnc_digit3 = 0xf;
    } else if (mme_config_p->served_tai.plmn_mnc_len[0] == 3) {
      config->tai_list.tai[i].plmn.mnc_digit1 = (mme_config_p->served_tai.plmn_mnc[0] / 100) % 10;
      config->tai_list.tai[i].plmn.mnc_digit2 = (mme_config_p->served_tai.plmn_mnc[0] / 10) % 10;
      config->tai_list.tai[i].plmn.mnc_digit3 = mme_config_p->served_tai.plmn_mnc[0] % 10;
    } else {
      AssertFatal ((mme_config_p->served_tai.plmn_mnc_len[0] >= 2) && (mme_config_p->served_tai.plmn_mnc_len[0] <= 3), "BAD MNC length for GUMMEI");
    }
    config->tai_list.tai[i].tac            = mme_config_p->served_tai.tac[i];
    config->tai_list.n_tais += 1;
  }
  config->tai_list.list_type = mme_config_p->served_tai.list_type;

  config->gummei = mme_config_p->gummei.gummei[0];


  // hardcoded
  config->eps_network_feature_support = EPS_NETWORK_FEATURE_SUPPORT_CS_LCS_LOCATION_SERVICES_VIA_CS_DOMAIN_NOT_SUPPORTED;
  if (mme_config_p->eps_network_feature_support.emergency_bearer_services_in_s1_mode != 0) {
    config->eps_network_feature_support |= EPS_NETWORK_FEATURE_SUPPORT_EMERGENCY_BEARER_SERVICES_IN_S1_MODE_SUPPORTED;
  }
  if (mme_config_p->eps_network_feature_support.ims_voice_over_ps_session_in_s1 != 0) {
    config->eps_network_feature_support |= EPS_NETWORK_FEATURE_SUPPORT_IMS_VOICE_OVER_PS_SESSION_IN_S1_SUPPORTED;
  }
  if (mme_config_p->eps_network_feature_support.location_services_via_epc != 0) {
    config->eps_network_feature_support |= EPS_NETWORK_FEATURE_SUPPORT_LOCATION_SERVICES_VIA_EPC_SUPPORTED;
  }
  if (mme_config_p->eps_network_feature_support.extended_service_request != 0) {
    config->eps_network_feature_support |= EPS_NETWORK_FEATURE_SUPPORT_EXTENDED_SERVICE_REQUEST_SUPPORTED;
  }

  if (mme_config_p->unauthenticated_imsi_supported != 0) {
    config->features |= MME_API_UNAUTHENTICATED_IMSI;
  }

  for (i = 0; i < 8; i++) {
    config->prefered_integrity_algorithm[i] = mme_config_p->nas_config.prefered_integrity_algorithm[i];
    config->prefered_ciphering_algorithm[i] = mme_config_p->nas_config.prefered_ciphering_algorithm[i];
  }
  OAILOG_FUNC_RETURN (LOG_NAS, RETURNok);
}

// TODO
void mme_api_duplicate_enb_ue_s1ap_id_detected (
    const enb_s1ap_id_key_t enb_s1ap_id_key,
    const mme_ue_s1ap_id_t mme_ue_s1ap_id,
    const bool             is_remove_old)
{
  mme_ue_context_duplicate_enb_ue_s1ap_id_detected(enb_s1ap_id_key, mme_ue_s1ap_id, is_remove_old);
}



/****************************************************************************
 **                                                                        **
 ** Name:    mme_api_get_config()                                      **
 **                                                                        **
 ** Description: Retreives MME configuration data related to EPS session   **
 **      management                                                **
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
mme_api_get_esm_config (
  mme_api_esm_config_t * config)
{
  OAILOG_FUNC_IN (LOG_NAS);

  if (_mme_api_ip_capability == MME_API_IPV4_ADDR) {
    config->features = MME_API_IPV4;
  } else if (_mme_api_ip_capability == MME_API_IPV6_ADDR) {
    config->features = MME_API_IPV6;
  } else if (_mme_api_ip_capability == MME_API_IPV4V6_ADDR) {
    config->features = MME_API_IPV4 | MME_API_IPV6;
  } else {
    config->features = 0;
  }

  OAILOG_FUNC_RETURN (LOG_NAS, RETURNok);
}


/*
 *
 *  Name:    mme_api_notify_imsi()
 *
 *  Description: Notify the MME of the IMSI of a UE.
 *
 *  Inputs:
 *         ueid:      nas_ue id
 *         imsi64:    IMSI
 *  Return:    RETURNok, RETURNerror
 *
 */
int
mme_api_notify_imsi (
  const mme_ue_s1ap_id_t id,
  const imsi64_t imsi64)
{
  ue_context_t                           *ue_context = NULL;

  OAILOG_FUNC_IN (LOG_NAS);
  ue_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, id);


  if ( ue_context) {
    mme_ue_context_update_coll_keys (&mme_app_desc.mme_ue_contexts,
        ue_context,
        ue_context->enb_s1ap_id_key,
        id,
        imsi64,
        ue_context->mme_s11_teid,
        &ue_context->guti);
    OAILOG_FUNC_RETURN (LOG_NAS, RETURNok);
  }

  OAILOG_FUNC_RETURN (LOG_NAS, RETURNerror);
}

/*
 *
 *  Name:    mme_api_notify_new_guti()
 *
 *  Description: Notify the MME of a generated GUTI for a UE(not spec).
 *
 *  Inputs:
 *         ueid:      nas_ue id
 *         guti:      EPS Globally Unique Temporary UE Identity
 *  Return:    RETURNok, RETURNerror
 *
 */
int
mme_api_notify_new_guti (
  const mme_ue_s1ap_id_t id,
  guti_t * const guti)
{
  ue_context_t                           *ue_context = NULL;

  OAILOG_FUNC_IN (LOG_NAS);
  ue_context = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, id);


  if ( ue_context) {
    ue_context->is_guti_set = true;
    mme_ue_context_update_coll_keys (&mme_app_desc.mme_ue_contexts,
        ue_context,
        ue_context->enb_s1ap_id_key,
        id,
        ue_context->imsi,
        ue_context->mme_s11_teid,
        guti);
    OAILOG_FUNC_RETURN (LOG_NAS, RETURNok);
  }

  OAILOG_FUNC_RETURN (LOG_NAS, RETURNerror);
}

/*
 *
 *  Name:    mme_api_notify_end_ue_s1ap_id_changed()
 *
 *  Description: Notify the MME of a change in ue id (reconnection).
 *
 *  Inputs:
 *         old_ueid:      old nas_ue id
 *         new_ueid:      new nas_ue id
 *         mme_ue_s1ap_id:   nas ue id
 *  Return:    RETURNok, RETURNerror
 *
 */
int
mme_api_notified_new_ue_s1ap_id_association (
    const enb_s1ap_id_key_t  enb_ue_s1ap_id_key,
    const uint32_t         enb_id,
    const mme_ue_s1ap_id_t mme_ue_s1ap_id)
{
  int                                     ret = RETURNok;
  OAILOG_FUNC_IN (LOG_NAS);
  ret = mme_ue_context_notified_new_ue_s1ap_id_association(enb_ue_s1ap_id_key, mme_ue_s1ap_id);
  OAILOG_FUNC_RETURN (LOG_NAS, ret);
}


/****************************************************************************
 **                                                                        **
 ** Name:    mme_api_new_guti()                                        **
 **                                                                        **
 ** Description: Requests the MME to assign a new GUTI to the UE identi-   **
 **      fied by the given IMSI.                                   **
 **                                                                        **
 ** Description: Requests the MME to assign a new GUTI to the UE identi-   **
 **      fied by the given IMSI and returns the list of consecu-   **
 **      tive tracking areas the UE is registered to.              **
 **                                                                        **
 ** Inputs:  imsi:      International Mobile Subscriber Identity   **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     guti:      The new assigned GUTI                      **
 **      tai_list:       TAIs belonging to the PLMN                                **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
mme_api_new_guti (
  const imsi_t * const imsi,
  const guti_t * const old_guti,
  guti_t * const guti,
  const tai_t      * const originating_tai,
  tai_list_t * const tai_list)
{
  ue_context_t                           *ue_context = NULL;
  imsi64_t                                mme_imsi = 0;

  OAILOG_FUNC_IN (LOG_NAS);
  IMSI_TO_IMSI64 (imsi, mme_imsi);
  ue_context = mme_ue_context_exists_imsi (&mme_app_desc.mme_ue_contexts, mme_imsi);

  if (ue_context) {
    guti->gummei.mme_gid         = _emm_data.conf.gummei.mme_gid;
    guti->gummei.mme_code        = _emm_data.conf.gummei.mme_code;
    guti->gummei.plmn.mcc_digit1 = _emm_data.conf.gummei.plmn.mcc_digit1;
    guti->gummei.plmn.mcc_digit2 = _emm_data.conf.gummei.plmn.mcc_digit2;
    guti->gummei.plmn.mcc_digit3 = _emm_data.conf.gummei.plmn.mcc_digit3;
    guti->gummei.plmn.mnc_digit1 = _emm_data.conf.gummei.plmn.mnc_digit1;
    guti->gummei.plmn.mnc_digit2 = _emm_data.conf.gummei.plmn.mnc_digit2;
    guti->gummei.plmn.mnc_digit3 = _emm_data.conf.gummei.plmn.mnc_digit3;
    if (RUN_MODE_TEST == mme_config.run_mode) {
      guti->m_tmsi = __sync_fetch_and_add (&mme_m_tmsi_generator, 0x00000001);
    } else {
      guti->m_tmsi                 = (tmsi_t)(uintptr_t)ue_context;
    }
    if (guti->m_tmsi == INVALID_M_TMSI) {
      OAILOG_FUNC_RETURN (LOG_NAS, RETURNerror);
    }
    mme_api_notify_new_guti(ue_context->mme_ue_s1ap_id, guti);
  } else {
    OAILOG_FUNC_RETURN (LOG_NAS, RETURNerror);
  }

  int            i,j;
  tac_t   tac  = INVALID_TAC_FFFE;
  bool    consecutive_tacs = true;
  j = 0;
  for (i=0; i < _emm_data.conf.tai_list.n_tais; i++) {
    if ((_emm_data.conf.tai_list.tai[i].plmn.mcc_digit1 == guti->gummei.plmn.mcc_digit1) &&
        (_emm_data.conf.tai_list.tai[i].plmn.mcc_digit2 == guti->gummei.plmn.mcc_digit2) &&
        (_emm_data.conf.tai_list.tai[i].plmn.mcc_digit3 == guti->gummei.plmn.mcc_digit3) &&
        (_emm_data.conf.tai_list.tai[i].plmn.mnc_digit1 == guti->gummei.plmn.mnc_digit1) &&
        (_emm_data.conf.tai_list.tai[i].plmn.mnc_digit2 == guti->gummei.plmn.mnc_digit2) &&
        (_emm_data.conf.tai_list.tai[i].plmn.mnc_digit3 == guti->gummei.plmn.mnc_digit3) ) {

      tai_list->tai[j].plmn = guti->gummei.plmn;
      // _emm_data.conf.tai_list is sorted
      tai_list->tai[j].tac            = _emm_data.conf.tai_list.tai[i].tac;
      if (INVALID_TAC_FFFE == tac)  {
        tac = _emm_data.conf.tai_list.tai[i].tac;
      } else {
        if ((tac+1) == _emm_data.conf.tai_list.tai[i].tac) {
          tac = tac + 1;
        } else {
          consecutive_tacs = false;
        }
      }

      j += 1;
    }
  }
  tai_list->n_tais = j;

  if (consecutive_tacs) {
    tai_list->list_type = TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_CONSECUTIVE_TACS;
  } else {
    tai_list->list_type = TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS;
  }
  OAILOG_INFO (LOG_NAS, "UE " MME_UE_S1AP_ID_FMT "  Got GUTI " GUTI_FMT "\n", ue_context->mme_ue_s1ap_id, GUTI_ARG(guti));
  OAILOG_FUNC_RETURN (LOG_NAS, RETURNok);

}

/****************************************************************************
 **                                                                        **
 ** Name:        mme_api_subscribe()                                       **
 **                                                                        **
 ** Description: Requests the MME to check whether connectivity with the   **
 **              requested PDN can be established using the specified APN. **
 **              If accepted the MME returns PDN subscription context con- **
 **              taining EPS subscribed QoS profile, the default APN if    **
 **              required and UE's IPv4 address and/or the IPv6 prefix.    **
 **                                                                        **
 ** Inputs:  apn:               If not NULL, Access Point Name of the PDN  **
 **                             to connect to                              **
 **              is_emergency:  true if the PDN connectivity is requested  **
 **                             for emergency bearer services              **
 **                  Others:    None                                       **
 **                                                                        **
 ** Outputs:         apn:       If NULL, default APN or APN configured for **
 **                             emergency bearer services                  **
 **                  pdn_addr:  PDN connection IPv4 address or IPv6 inter- **
 **                             face identifier to be used to build the    **
 **                             IPv6 link local address                    **
 **                  qos:       EPS subscribed QoS profile                 **
 **                  Return:    RETURNok, RETURNerror                      **
 **                  Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
mme_api_subscribe (
  bstring * apn,
  mme_api_ip_version_t mme_pdn_index,
  bstring * pdn_addr,
  int is_emergency,
  mme_api_qos_t * qos)
{
  int                                     rc = RETURNok;

  OAILOG_FUNC_IN (LOG_NAS);
  OAILOG_FUNC_RETURN (LOG_NAS, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:        mme_api_unsubscribe()                                     **
 **                                                                        **
 ** Description: Requests the MME to release connectivity with the reques- **
 **              ted PDN using the specified APN.                          **
 **                                                                        **
 ** Inputs:  apn:               Access Point Name of the PDN to disconnect **
 **                             from                                       **
 **                  Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **                  Return:    RETURNok, RETURNerror                      **
 **                  Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
mme_api_unsubscribe ( bstring apn)
{
  OAILOG_FUNC_IN (LOG_NAS);
  int                                     rc = RETURNok;

  /*
   * Decrement the total number of PDN connections
   */
  _mme_api_pdn_id -= 1;
  OAILOG_FUNC_RETURN (LOG_NAS, rc);
}
