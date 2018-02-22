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


#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "obj_hashtable.h"
#include "log.h"
#include "msc.h"
#include "3gpp_24.301.h"
#include "common_types.h"
#include "NasSecurityAlgorithms.h"
#include "conversions.h"
#include "emmData.h"
#include "EmmCommon.h"

static mme_ue_s1ap_id_t mme_ue_s1ap_id_generator = 1;

//------------------------------------------------------------------------------
static bool emm_data_context_dump_hash_table_wrapper (
  hash_key_t keyP,
  void *dataP,
  void *parameterP,
  void**resultP);

//------------------------------------------------------------------------------

static hashtable_rc_t
_emm_data_context_hashtable_insert(
    emm_data_t *emm_data,
    struct emm_data_context_s *elm)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;

  if ( IS_EMM_CTXT_PRESENT_IMSI(elm)) {
    h_rc = hashtable_ts_insert (emm_data->ctx_coll_imsi, elm->_imsi64, (void*)(&elm->ue_id));
  } else {
    // This should not happen. Possible UE bug?
    OAILOG_WARNING(LOG_NAS_EMM, "EMM-CTX doesn't contain valid imsi UE id " MME_UE_S1AP_ID_FMT "\n", elm->ue_id);
  }
  return h_rc;
}


mme_ue_s1ap_id_t emm_ctx_get_new_ue_id(emm_data_context_t *ctxt)
{
  mme_ue_s1ap_id_t tmp = 0;
  if (RUN_MODE_TEST == mme_config.run_mode) {
    tmp = __sync_fetch_and_add (&mme_ue_s1ap_id_generator, 1);
  } else {
    tmp = (mme_ue_s1ap_id_t)((uint)((uintptr_t)ctxt) >> 4);// ^ 0xBABA53AB;
  }
  return tmp;
}

//------------------------------------------------------------------------------
inline void emm_ctx_mark_common_procedure_running(emm_data_context_t * const ctxt, const int proc_id)
{
  __sync_fetch_and_or(&ctxt->common_proc_mask, proc_id);
}

inline void emm_ctx_unmark_common_procedure_running(emm_data_context_t * const ctxt, const int proc_id)
{
  __sync_fetch_and_and(&ctxt->common_proc_mask, ~proc_id);
}

inline bool emm_ctx_is_common_procedure_running(emm_data_context_t * const ctxt, const int proc_id)
{
  if (ctxt->common_proc_mask & proc_id) return true;
  return false;
}



inline void emm_ctx_mark_specific_procedure(emm_data_context_t * const ctxt, const int proc_id)
{
  __sync_fetch_and_or(&ctxt->specific_proc_mask, proc_id);
}

inline void emm_ctx_unmark_specific_procedure(emm_data_context_t * const ctxt, const int proc_id)
{
  __sync_fetch_and_and(&ctxt->specific_proc_mask, ~proc_id);
}

inline bool emm_ctx_is_specific_procedure(emm_data_context_t * const ctxt, const int proc_id)
{
  if (ctxt->specific_proc_mask & proc_id) return true;
  return false;
}


//------------------------------------------------------------------------------
inline void emm_ctx_set_attribute_present(emm_data_context_t * const ctxt, const int attribute_bit_pos)
{
  ctxt->member_present_mask |= attribute_bit_pos;
}

inline void emm_ctx_clear_attribute_present(emm_data_context_t * const ctxt, const int attribute_bit_pos)
{
  ctxt->member_present_mask &= ~attribute_bit_pos;
  ctxt->member_valid_mask &= ~attribute_bit_pos;
}

inline void emm_ctx_set_attribute_valid(emm_data_context_t * const ctxt, const int attribute_bit_pos)
{
  ctxt->member_present_mask |= attribute_bit_pos;
  ctxt->member_valid_mask   |= attribute_bit_pos;
}

inline void emm_ctx_clear_attribute_valid(emm_data_context_t * const ctxt, const int attribute_bit_pos)
{
  ctxt->member_valid_mask &= ~attribute_bit_pos;
}

//------------------------------------------------------------------------------
/* Clear GUTI  */
inline void emm_ctx_clear_guti(emm_data_context_t * const ctxt)
{
  clear_guti(&ctxt->_guti);
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_GUTI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " GUTI cleared\n", ctxt->ue_id);
}

/* Set GUTI */
inline void emm_ctx_set_guti(emm_data_context_t * const ctxt, guti_t *guti)
{
  ctxt->_guti = *guti;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_GUTI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set GUTI " GUTI_FMT " (present)\n", ctxt->ue_id, GUTI_ARG(&ctxt->_guti));
}

/* Set GUTI, mark it as valid */
inline void emm_ctx_set_valid_guti(emm_data_context_t * const ctxt, guti_t *guti)
{
  ctxt->_guti = *guti;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_GUTI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set GUTI " GUTI_FMT " (valid)\n", ctxt->ue_id, GUTI_ARG(&ctxt->_guti));
}

//------------------------------------------------------------------------------
/* Clear old GUTI  */
inline void emm_ctx_clear_old_guti(emm_data_context_t * const ctxt)
{
  clear_guti(&ctxt->_old_guti);
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_OLD_GUTI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " old GUTI cleared\n", ctxt->ue_id);
}

/* Set GUTI */
inline void emm_ctx_set_old_guti(emm_data_context_t * const ctxt, guti_t *guti)
{
  ctxt->_old_guti = *guti;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_OLD_GUTI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set old GUTI " GUTI_FMT " (present)\n", ctxt->ue_id, GUTI_ARG(&ctxt->_old_guti));
}

/* Set GUTI, mark it as valid */
inline void emm_ctx_set_valid_old_guti(emm_data_context_t * const ctxt, guti_t *guti)
{
  ctxt->_old_guti = *guti;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_OLD_GUTI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set old GUTI " GUTI_FMT " (valid)\n", ctxt->ue_id, GUTI_ARG(&ctxt->_old_guti));
}

//------------------------------------------------------------------------------
/* Clear IMSI */
inline void emm_ctx_clear_imsi(emm_data_context_t * const ctxt)
{
  clear_imsi(&ctxt->_imsi);
  ctxt->_imsi64 = INVALID_IMSI64;
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_IMSI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared IMSI\n", ctxt->ue_id);
}

/* Set IMSI */
inline void emm_ctx_set_imsi(emm_data_context_t * const ctxt, imsi_t *imsi, const imsi64_t imsi64)
{
  ctxt->_imsi   = *imsi;
  ctxt->_imsi64 = imsi64;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_IMSI);
#if  DEBUG_IS_ON
  char     imsi_str[IMSI_BCD_DIGITS_MAX+1] = {0};
  IMSI64_TO_STRING (ctxt->_imsi64, imsi_str);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set IMSI %s (valid)\n", ctxt->ue_id, imsi_str);
#endif
}

/* Set IMSI, mark it as valid */
inline void emm_ctx_set_valid_imsi(emm_data_context_t * const ctxt, imsi_t *imsi, const imsi64_t imsi64)
{
  ctxt->_imsi   = *imsi;
  ctxt->_imsi64 = imsi64;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_IMSI);
#if  DEBUG_IS_ON
  char     imsi_str[IMSI_BCD_DIGITS_MAX+1] = {0};
  IMSI64_TO_STRING (ctxt->_imsi64, imsi_str);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set IMSI %s (valid)\n", ctxt->ue_id, imsi_str);
#endif
  mme_api_notify_imsi(ctxt->ue_id, imsi64);
}

//------------------------------------------------------------------------------
/* Clear IMEI */
inline void emm_ctx_clear_imei(emm_data_context_t * const ctxt)
{
  clear_imei(&ctxt->_imei);
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_IMEI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " IMEI cleared\n", ctxt->ue_id);
}

/* Set IMEI */
inline void emm_ctx_set_imei(emm_data_context_t * const ctxt, imei_t *imei)
{
  ctxt->_imei = *imei;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_IMEI);
#if  DEBUG_IS_ON
  char     imei_str[16];
  IMEI_TO_STRING (imei, imei_str, 16);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set IMEI %s (present)\n", ctxt->ue_id, imei_str);
#endif
}

/* Set IMEI, mark it as valid */
inline void emm_ctx_set_valid_imei(emm_data_context_t * const ctxt, imei_t *imei)
{
  ctxt->_imei = *imei;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_IMEI);
#if  DEBUG_IS_ON
  char     imei_str[16];
  IMEI_TO_STRING (imei, imei_str, 16);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set IMEI %s (valid)\n", ctxt->ue_id, imei_str);
#endif
}

//------------------------------------------------------------------------------
/* Clear IMEI_SV */
inline void emm_ctx_clear_imeisv(emm_data_context_t * const ctxt)
{
  clear_imeisv(&ctxt->_imeisv);
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_IMEI_SV);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared IMEI_SV \n", ctxt->ue_id);
}

/* Set IMEI_SV */
inline void emm_ctx_set_imeisv(emm_data_context_t * const ctxt, imeisv_t *imeisv)
{
  ctxt->_imeisv = *imeisv;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_IMEI_SV);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set IMEI_SV (present)\n", ctxt->ue_id);
}

/* Set IMEI_SV, mark it as valid */
inline void emm_ctx_set_valid_imeisv(emm_data_context_t * const ctxt, imeisv_t *imeisv)
{
  ctxt->_imeisv = *imeisv;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_IMEI_SV);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set IMEI_SV (valid)\n", ctxt->ue_id);
}

//------------------------------------------------------------------------------
/* Clear last_visited_registered_tai */
inline void emm_ctx_clear_lvr_tai(emm_data_context_t * const ctxt)
{
  clear_tai(&ctxt->_lvr_tai);
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_LVR_TAI);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared last visited registered TAI\n", ctxt->ue_id);
}

/* Set last_visited_registered_tai */
inline void emm_ctx_set_lvr_tai(emm_data_context_t * const ctxt, tai_t *lvr_tai)
{
  ctxt->_lvr_tai = *lvr_tai;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_LVR_TAI);
  //log_message(NULL, OAILOG_LEVEL_DEBUG,    LOG_NAS_EMM, __FILE__, __LINE__,
  //    "ue_id="MME_UE_S1AP_ID_FMT" set last visited registered TAI "TAI_FMT" (present)\n", ctxt->ue_id, TAI_ARG(&ctxt->_lvr_tai));

  //OAILOG_DEBUG (LOG_NAS_EMM, "ue_id="MME_UE_S1AP_ID_FMT" set last visited registered TAI "TAI_FMT" (present)\n", ctxt->ue_id, TAI_ARG(&ctxt->_lvr_tai));
}

/* Set last_visited_registered_tai, mark it as valid */
inline void emm_ctx_set_valid_lvr_tai(emm_data_context_t * const ctxt, tai_t *lvr_tai)
{
  ctxt->_lvr_tai = *lvr_tai;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_LVR_TAI);
  //OAILOG_DEBUG (LOG_NAS_EMM, "ue_id="MME_UE_S1AP_ID_FMT" set last visited registered TAI "TAI_FMT" (valid)\n", ctxt->ue_id, TAI_ARG(&ctxt->_lvr_tai));
}

//------------------------------------------------------------------------------
/* Clear AUTH vectors  */
inline void emm_ctx_clear_auth_vectors(emm_data_context_t * const ctxt)
{
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_AUTH_VECTORS);
  for (int i = 0; i < MAX_EPS_AUTH_VECTORS; i++) {
    memset((void *)&ctxt->_vector[i], 0, sizeof(ctxt->_vector[i]));
    emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_AUTH_VECTOR0+i);
  }
  emm_ctx_clear_security_vector_index(ctxt);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared auth vectors \n", ctxt->ue_id);
}
//------------------------------------------------------------------------------
/* Clear AUTH vector  */
inline void emm_ctx_clear_auth_vector(emm_data_context_t * const ctxt, ksi_t eksi)
{
  AssertFatal(eksi < MAX_EPS_AUTH_VECTORS, "Out of bounds eksi %d", eksi);
  memset((void *)&ctxt->_vector[eksi], 0, sizeof(ctxt->_vector[eksi]));
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_AUTH_VECTOR0+eksi);
  int remaining_vectors = 0;
  for (int i = 0; i < MAX_EPS_AUTH_VECTORS; i++) {
    if (IS_EMM_CTXT_VALID_AUTH_VECTOR(ctxt, i)) {
      remaining_vectors+=1;
    }
  }
  ctxt->remaining_vectors = remaining_vectors;
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared auth vector %u \n", ctxt->ue_id, eksi);
  if (!(remaining_vectors)) {
    emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_AUTH_VECTORS);
    emm_ctx_clear_security_vector_index(ctxt);
    OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared auth vectors\n", ctxt->ue_id);
  }
}
//------------------------------------------------------------------------------
/* Clear security  */
inline void emm_ctx_clear_security(emm_data_context_t * const ctxt)
{
  memset (&ctxt->_security, 0, sizeof (ctxt->_security));
  emm_ctx_set_security_type(ctxt, SECURITY_CTX_TYPE_NOT_AVAILABLE);
  emm_ctx_set_security_eksi(ctxt, KSI_NO_KEY_AVAILABLE);
  emm_ctx_clear_security_vector_index(ctxt);
  ctxt->_security.selected_algorithms.encryption = NAS_SECURITY_ALGORITHMS_EEA0;
  ctxt->_security.selected_algorithms.integrity  = NAS_SECURITY_ALGORITHMS_EIA0;
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_SECURITY);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared security context \n", ctxt->ue_id);
}

//------------------------------------------------------------------------------
inline void emm_ctx_set_security_type(emm_data_context_t * const ctxt, emm_sc_type_t sc_type)
{
  ctxt->_security.sc_type = sc_type;
  OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set security context security type %d\n", ctxt->ue_id, sc_type);
}

//------------------------------------------------------------------------------
inline void emm_ctx_set_security_eksi(emm_data_context_t * const ctxt, ksi_t eksi)
{
  ctxt->_security.eksi = eksi;
  OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set security context eksi %d\n", ctxt->ue_id, eksi);
}

//------------------------------------------------------------------------------
inline void emm_ctx_clear_security_vector_index(emm_data_context_t * const ctxt)
{
  ctxt->_security.vector_index = EMM_SECURITY_VECTOR_INDEX_INVALID;
  OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " clear security context vector index\n", ctxt->ue_id);
}
//------------------------------------------------------------------------------
inline void emm_ctx_set_security_vector_index(emm_data_context_t * const ctxt, int vector_index)
{
  ctxt->_security.vector_index = vector_index;
  OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set security context vector index %d\n", ctxt->ue_id, vector_index);
}


//------------------------------------------------------------------------------
/* Clear non current security  */
inline void emm_ctx_clear_non_current_security(emm_data_context_t * const ctxt)
{
  memset (&ctxt->_non_current_security, 0, sizeof (ctxt->_non_current_security));
  ctxt->_non_current_security.sc_type      = SECURITY_CTX_TYPE_NOT_AVAILABLE;
  ctxt->_non_current_security.eksi         = KSI_NO_KEY_AVAILABLE;
  emm_ctx_clear_non_current_security_vector_index(ctxt);
  ctxt->_non_current_security.selected_algorithms.encryption = NAS_SECURITY_ALGORITHMS_EEA0;
  ctxt->_non_current_security.selected_algorithms.integrity  = NAS_SECURITY_ALGORITHMS_EIA0;
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_NON_CURRENT_SECURITY);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared non current security context \n", ctxt->ue_id);
}
//------------------------------------------------------------------------------
inline void emm_ctx_clear_non_current_security_vector_index(emm_data_context_t * const ctxt)
{
  ctxt->_non_current_security.vector_index = EMM_SECURITY_VECTOR_INDEX_INVALID;
  OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " clear non current security context vector index\n", ctxt->ue_id);
}
//------------------------------------------------------------------------------
inline void emm_ctx_set_non_current_security_vector_index(emm_data_context_t * const ctxt, int vector_index)
{
  ctxt->_non_current_security.vector_index = vector_index;
  OAILOG_TRACE (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set non current security context vector index %d\n", ctxt->ue_id, vector_index);
}

//------------------------------------------------------------------------------
/* Clear UE network capability IE   */
inline void emm_ctx_clear_ue_nw_cap_ie(emm_data_context_t * const ctxt)
{
  memset (&ctxt->_ue_network_capability_ie, 0, sizeof (ctxt->_ue_network_capability_ie));
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_UE_NETWORK_CAPABILITY_IE);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared UE network capability IE\n", ctxt->ue_id);
}

/* Set UE network capability IE */
inline void emm_ctx_set_ue_nw_cap_ie(emm_data_context_t * const ctxt, UeNetworkCapability *ue_nw_cap_ie)
{
  ctxt->_ue_network_capability_ie = *ue_nw_cap_ie;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_UE_NETWORK_CAPABILITY_IE);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set UE network capability IE (present)\n", ctxt->ue_id);
}

/* Set UE network capability IE, mark it as valid */
inline void emm_ctx_set_valid_ue_nw_cap_ie(emm_data_context_t * const ctxt, UeNetworkCapability *ue_nw_cap_ie)
{
  ctxt->_ue_network_capability_ie = *ue_nw_cap_ie;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_UE_NETWORK_CAPABILITY_IE);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set UE network capability IE (valid)\n", ctxt->ue_id);
}


//------------------------------------------------------------------------------
/* Clear MS network capability IE   */
inline void emm_ctx_clear_ms_nw_cap(emm_data_context_t * const ctxt)
{
  memset (&ctxt->_ms_network_capability_ie, 0, sizeof (ctxt->_ue_network_capability_ie));
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_MS_NETWORK_CAPABILITY_IE);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared MS network capability IE\n", ctxt->ue_id);
}

//------------------------------------------------------------------------------
/* Clear current DRX parameter   */
inline void emm_ctx_clear_current_drx_parameter(emm_data_context_t * const ctxt)
{
  memset (&ctxt->_current_drx_parameter, 0, sizeof (ctxt->_current_drx_parameter));
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_CURRENT_DRX_PARAMETER);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared current DRX parameter\n", ctxt->ue_id);
}

/* Set current DRX parameter */
inline void emm_ctx_set_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx)
{
  ctxt->_current_drx_parameter = *drx;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_CURRENT_DRX_PARAMETER);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set current DRX parameter (present)\n", ctxt->ue_id);
}

/* Set current DRX parameter, mark it as valid */
inline void emm_ctx_set_valid_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx)
{
  ctxt->_current_drx_parameter = *drx;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_CURRENT_DRX_PARAMETER);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set current DRX parameter (valid)\n", ctxt->ue_id);
}

//------------------------------------------------------------------------------
/* Clear non current DRX parameter   */
inline void emm_ctx_clear_pending_current_drx_parameter(emm_data_context_t * const ctxt)
{
  memset (&ctxt->_pending_drx_parameter, 0, sizeof (ctxt->_pending_drx_parameter));
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_PENDING_DRX_PARAMETER);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared pending DRX parameter\n", ctxt->ue_id);
}

/* Set current DRX parameter */
inline void emm_ctx_set_pending_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx)
{
  ctxt->_pending_drx_parameter = *drx;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_PENDING_DRX_PARAMETER);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set pending DRX parameter (present)\n", ctxt->ue_id);
}

/* Set current DRX parameter, mark it as valid */
inline void emm_ctx_set_valid_pending_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx)
{
  ctxt->_pending_drx_parameter = *drx;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_PENDING_DRX_PARAMETER);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set pending DRX parameter (valid)\n", ctxt->ue_id);
}

//------------------------------------------------------------------------------
/* Clear EPS bearer context status   */
inline void emm_ctx_clear_eps_bearer_context_status(emm_data_context_t * const ctxt)
{
  memset (&ctxt->_eps_bearer_context_status, 0, sizeof (ctxt->_eps_bearer_context_status));
  emm_ctx_clear_attribute_present(ctxt, EMM_CTXT_MEMBER_EPS_BEARER_CONTEXT_STATUS);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " cleared EPS bearer context status\n", ctxt->ue_id);
}

/* Set current DRX parameter */
inline void emm_ctx_set_eps_bearer_context_status(emm_data_context_t * const ctxt, EpsBearerContextStatus *status)
{
  ctxt->_eps_bearer_context_status = *status;
  emm_ctx_set_attribute_present(ctxt, EMM_CTXT_MEMBER_EPS_BEARER_CONTEXT_STATUS);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set EPS bearer context status (present)\n", ctxt->ue_id);
}

/* Set current DRX parameter, mark it as valid */
inline void emm_ctx_set_valid_eps_bearer_context_status(emm_data_context_t * const ctxt, EpsBearerContextStatus *status)
{
  ctxt->_eps_bearer_context_status = *status;
  emm_ctx_set_attribute_valid(ctxt, EMM_CTXT_MEMBER_EPS_BEARER_CONTEXT_STATUS);
  OAILOG_DEBUG (LOG_NAS_EMM, "ue_id=" MME_UE_S1AP_ID_FMT " set EPS bearer context status (valid)\n", ctxt->ue_id);
}



//------------------------------------------------------------------------------
struct emm_data_context_s              *
emm_data_context_get (
  emm_data_t * emm_data,
  const mme_ue_s1ap_id_t ue_id)
{
  struct emm_data_context_s              *emm_data_context_p = NULL;

  DevAssert (emm_data );
  if (INVALID_MME_UE_S1AP_ID != ue_id) {
    hashtable_ts_get (emm_data->ctx_coll_ue_id, (const hash_key_t)(ue_id), (void **)&emm_data_context_p);
    OAILOG_INFO (LOG_NAS_EMM, "EMM-CTX - get UE id " MME_UE_S1AP_ID_FMT " context %p\n", ue_id, emm_data_context_p);
  }
  return emm_data_context_p;
}

//------------------------------------------------------------------------------
struct emm_data_context_s              *
emm_data_context_get_by_imsi (
  emm_data_t * emm_data,
  imsi64_t     imsi64)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  mme_ue_s1ap_id_t                       *emm_ue_id_p = NULL;

  DevAssert (emm_data );

  h_rc = hashtable_ts_get (emm_data->ctx_coll_imsi, (const hash_key_t)imsi64, (void **)&emm_ue_id_p);

  if (HASH_TABLE_OK == h_rc) {
    struct emm_data_context_s * tmp = emm_data_context_get (emm_data, (const hash_key_t)*emm_ue_id_p);
#if DEBUG_IS_ON
    if ((tmp)) {
      OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - get UE id " MME_UE_S1AP_ID_FMT " context %p by imsi " IMSI_64_FMT "\n", tmp->ue_id, tmp, imsi64);
    }
#endif
    return tmp;
  }
  return NULL;
}


//------------------------------------------------------------------------------
struct emm_data_context_s              *
emm_data_context_get_by_guti (
  emm_data_t * emm_data,
  guti_t * guti)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  mme_ue_s1ap_id_t                        *emm_ue_id_p = NULL;

  DevAssert (emm_data );

  if ( guti) {

    h_rc = obj_hashtable_ts_get (emm_data->ctx_coll_guti, (const void *)guti, sizeof (*guti), (void **) &emm_ue_id_p);

    if (HASH_TABLE_OK == h_rc) {
      struct emm_data_context_s * tmp = emm_data_context_get (emm_data, *emm_ue_id_p);
#if DEBUG_IS_ON
      if ((tmp)) {
        OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - get UE id " MME_UE_S1AP_ID_FMT " context %p by guti " GUTI_FMT "\n", tmp->ue_id, tmp, GUTI_ARG(guti));
      }
#endif
      return tmp;
    }
  }
  return NULL;
}


//------------------------------------------------------------------------------
struct emm_data_context_s              *
emm_data_context_remove (
  emm_data_t * emm_data,
  struct emm_data_context_s *elm)
{
  struct emm_data_context_s              *emm_data_context_p = NULL;
  mme_ue_s1ap_id_t                       *emm_ue_id          = NULL;

  OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Remove in context %p UE id " MME_UE_S1AP_ID_FMT "\n", elm, elm->ue_id);

  if ( IS_EMM_CTXT_PRESENT_GUTI(elm)) {
    obj_hashtable_ts_remove(emm_data->ctx_coll_guti, (const void *) &elm->_guti, sizeof(elm->_guti),
                            (void **) &emm_ue_id);
    if (emm_ue_id) {
      // The GUTI is only inserted as part of attach complete, so it might be NULL.
      OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Remove in ctx_coll_guti context %p UE id "
          MME_UE_S1AP_ID_FMT " guti " " " GUTI_FMT "\n", elm, (mme_ue_s1ap_id_t) (*emm_ue_id), GUTI_ARG(&elm->_guti));
    }
    emm_ctx_clear_guti(elm);
  }
  
  if ( IS_EMM_CTXT_PRESENT_IMSI(elm)) {
    imsi64_t imsi64 = INVALID_IMSI64;
    IMSI_TO_IMSI64(&elm->_imsi,imsi64);
    hashtable_ts_remove (emm_data->ctx_coll_imsi, (const hash_key_t)imsi64, (void **)&emm_ue_id);

    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Remove in ctx_coll_imsi context %p UE id " MME_UE_S1AP_ID_FMT " imsi " IMSI_64_FMT "\n",
                  elm, (mme_ue_s1ap_id_t)((uintptr_t)emm_ue_id), imsi64);
    emm_ctx_clear_imsi(elm);
  }

  hashtable_ts_remove (emm_data->ctx_coll_ue_id, (const hash_key_t)(elm->ue_id), (void **)&emm_data_context_p);
  return emm_data_context_p;
}
//------------------------------------------------------------------------------
void 
emm_data_context_remove_mobile_ids (
  emm_data_t * emm_data, struct emm_data_context_s *elm)
{
  mme_ue_s1ap_id_t                       *emm_ue_id          = NULL;

  OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Remove in context %p UE id " MME_UE_S1AP_ID_FMT "\n", elm, elm->ue_id);

  if ( IS_EMM_CTXT_PRESENT_GUTI(elm)) {
    obj_hashtable_ts_remove(emm_data->ctx_coll_guti, (const void *) &elm->_guti, sizeof(elm->_guti),
                            (void **) &emm_ue_id);

    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Remove in ctx_coll_guti context %p UE id " MME_UE_S1AP_ID_FMT " guti " " "
        GUTI_FMT "\n", elm, (mme_ue_s1ap_id_t) (*emm_ue_id), GUTI_ARG(&elm->_guti));
  }
  
  emm_ctx_clear_guti(elm);

  if ( IS_EMM_CTXT_PRESENT_IMSI(elm)) {
    imsi64_t imsi64 = INVALID_IMSI64;
    IMSI_TO_IMSI64(&elm->_imsi,imsi64);
    hashtable_ts_remove (emm_data->ctx_coll_imsi, (const hash_key_t)imsi64, (void **)&emm_ue_id);

    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Remove in ctx_coll_imsi context %p UE id " MME_UE_S1AP_ID_FMT " imsi " IMSI_64_FMT "\n",
        elm, (mme_ue_s1ap_id_t)((uintptr_t)emm_ue_id), imsi64);
  }
  emm_ctx_clear_imsi(elm);
  return;
}


//------------------------------------------------------------------------------
int
emm_data_context_add (
  emm_data_t * emm_data,
  struct emm_data_context_s *elm)
{
  hashtable_rc_t                          h_rc;

  h_rc = hashtable_ts_insert (emm_data->ctx_coll_ue_id, (const hash_key_t)(elm->ue_id), elm);

  if (HASH_TABLE_OK == h_rc) {
    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Add in context %p UE id " MME_UE_S1AP_ID_FMT "\n", elm, elm->ue_id);

    if ( IS_EMM_CTXT_PRESENT_GUTI(elm)) {
      h_rc = obj_hashtable_ts_insert (emm_data->ctx_coll_guti, (const void *const)(&elm->_guti), sizeof (elm->_guti),
                                      &elm->ue_id);

      if (HASH_TABLE_OK == h_rc) {
        OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with GUTI "GUTI_FMT"\n", elm->ue_id, GUTI_ARG(&elm->_guti));
      } else {
        OAILOG_ERROR (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with GUTI "GUTI_FMT" Failed %s\n", elm->ue_id, GUTI_ARG(&elm->_guti), hashtable_rc_code2string (h_rc));
        return RETURNerror;
      }
    }
    if ( IS_EMM_CTXT_PRESENT_IMSI(elm)) {
      imsi64_t imsi64 = INVALID_IMSI64;
      IMSI_TO_IMSI64(&elm->_imsi,imsi64);
      h_rc = hashtable_ts_insert (emm_data->ctx_coll_imsi, imsi64, (void*)&elm->ue_id);

      if (HASH_TABLE_OK == h_rc) {
        OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with IMSI "IMSI_64_FMT"\n", elm->ue_id, imsi64);
      } else {
        OAILOG_ERROR (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with IMSI "IMSI_64_FMT" Failed %s\n", elm->ue_id, imsi64, hashtable_rc_code2string (h_rc));
        return RETURNerror;
      }
    }
    return RETURNok;
  } else {
    OAILOG_ERROR (LOG_NAS_EMM, "EMM-CTX - Add in context %p UE id " MME_UE_S1AP_ID_FMT " Failed %s\n", elm, elm->ue_id, hashtable_rc_code2string (h_rc));
    return RETURNerror;
  }
}
//------------------------------------------------------------------------------
int
emm_data_context_add_guti (
  emm_data_t * emm_data,
  struct emm_data_context_s *elm)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;

  if ( IS_EMM_CTXT_PRESENT_GUTI(elm)) {
    h_rc = obj_hashtable_ts_insert (emm_data->ctx_coll_guti, (const void *const)(&elm->_guti), sizeof (elm->_guti),
                                    &elm->ue_id);

    if (HASH_TABLE_OK == h_rc) {
      OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with GUTI "GUTI_FMT"\n", elm->ue_id, GUTI_ARG(&elm->_guti));
    } else {
      OAILOG_ERROR (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with GUTI "GUTI_FMT" Failed %s\n", elm->ue_id, GUTI_ARG(&elm->_guti), hashtable_rc_code2string (h_rc));
      return RETURNerror;
    }
  }
  return RETURNok;
}
//------------------------------------------------------------------------------
int
emm_data_context_add_old_guti (
  emm_data_t * emm_data,
  struct emm_data_context_s *elm)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;

  if ( IS_EMM_CTXT_PRESENT_OLD_GUTI(elm)) {
    h_rc = obj_hashtable_ts_insert (emm_data->ctx_coll_guti, (const void *const)(&elm->_old_guti),
                                    sizeof(elm->_old_guti), &elm->ue_id);

    if (HASH_TABLE_OK == h_rc) {
      OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with old GUTI "GUTI_FMT"\n", elm->ue_id, GUTI_ARG(&elm->_old_guti));
    } else {
      OAILOG_ERROR (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with old GUTI "GUTI_FMT" Failed %s\n", elm->ue_id, GUTI_ARG(&elm->_old_guti), hashtable_rc_code2string (h_rc));
      return RETURNerror;
    }
  }
  return RETURNok;
}

//------------------------------------------------------------------------------

int
emm_data_context_add_imsi (
  emm_data_t * emm_data,
  struct emm_data_context_s *elm) {
  hashtable_rc_t h_rc = HASH_TABLE_OK;

  h_rc = _emm_data_context_hashtable_insert(emm_data, elm);

  if (HASH_TABLE_OK == h_rc) {
    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with IMSI " IMSI_64_FMT "\n",
                  elm->ue_id, elm->_imsi64);
    return RETURNok;
  } else {
    OAILOG_ERROR (LOG_NAS_EMM, "EMM-CTX - Add in context UE id " MME_UE_S1AP_ID_FMT " with IMSI " IMSI_64_FMT
        " Failed %s\n", elm->ue_id, elm->_imsi64, hashtable_rc_code2string(h_rc));
    return RETURNerror;
  }
}

//------------------------------------------------------------------------------

int
emm_data_context_upsert_imsi (
    emm_data_t * emm_data,
    struct emm_data_context_s *elm)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;

  h_rc = _emm_data_context_hashtable_insert(emm_data, elm);

  if (HASH_TABLE_OK == h_rc || HASH_TABLE_INSERT_OVERWRITTEN_DATA == h_rc) {
    OAILOG_DEBUG (LOG_NAS_EMM, "EMM-CTX - Upsert in context UE id " MME_UE_S1AP_ID_FMT " with IMSI "IMSI_64_FMT"\n",
                  elm->ue_id, elm->_imsi64);
    return RETURNok;
  } else {
    OAILOG_ERROR (LOG_NAS_EMM, "EMM-CTX - Upsert in context UE id " MME_UE_S1AP_ID_FMT " with IMSI "IMSI_64_FMT" "
        "Failed %s\n", elm->ue_id, elm->_imsi64, hashtable_rc_code2string (h_rc));
    return RETURNerror;
  }
}

//------------------------------------------------------------------------------

void emm_data_context_stop_all_timers (struct emm_data_context_s *emm_ctx)
{
  if (emm_ctx ) {
    /*
     * Stop timer T3450
     */
    if (emm_ctx->T3450.id != NAS_TIMER_INACTIVE_ID) {
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3450 (%d)\n", emm_ctx->T3450.id);
      emm_ctx->T3450.id = nas_timer_stop (emm_ctx->T3450.id);
      MSC_LOG_EVENT (MSC_NAS_EMM_MME, "0 T3450 stopped UE " MME_UE_S1AP_ID_FMT " ", emm_ctx->ue_id);
    }

    /*
     * Stop timer T3460
     */
    if (emm_ctx->T3460.id != NAS_TIMER_INACTIVE_ID) {
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3460 (%d)\n", emm_ctx->T3460.id);
      emm_ctx->T3460.id = nas_timer_stop (emm_ctx->T3460.id);
      MSC_LOG_EVENT (MSC_NAS_EMM_MME, "0 T3460 stopped UE " MME_UE_S1AP_ID_FMT " ", emm_ctx->ue_id);
    }

    /*
     * Stop timer T3470
     */
    if (emm_ctx->T3470.id != NAS_TIMER_INACTIVE_ID) {
      OAILOG_INFO (LOG_NAS_EMM, "EMM-PROC  - Stop timer T3470 (%d)\n", emm_ctx->T3460.id);
      emm_ctx->T3470.id = nas_timer_stop (emm_ctx->T3470.id);
      MSC_LOG_EVENT (MSC_NAS_EMM_MME, "0 T3470 stopped UE " MME_UE_S1AP_ID_FMT " ", emm_ctx->ue_id);
    }
  }
}

//------------------------------------------------------------------------------
void emm_data_context_silently_reset_procedures(
    struct emm_data_context_s * const emm_ctx)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  if (emm_ctx) {
    emm_data_context_stop_all_timers(emm_ctx);
    emm_common_cleanup_by_ueid(emm_ctx->ue_id);
  }
  OAILOG_FUNC_OUT (LOG_NAS_EMM);
}
//------------------------------------------------------------------------------
void free_emm_data_context(
    struct emm_data_context_s * const emm_ctx)
{
  if (emm_ctx ) {
    if (emm_ctx->esm_msg) {
      bdestroy (emm_ctx->esm_msg);
    }

    emm_data_context_stop_all_timers(emm_ctx);

    free_esm_data_context(&emm_ctx->esm_data_ctx);
    free_wrapper((void**) &emm_ctx);
  }
}

//------------------------------------------------------------------------------
void
emm_data_context_dump (
  const struct emm_data_context_s * const elm_pP)
{
  if (elm_pP ) {
    char                                    imsi_str[16];
    int                                     k = 0,
                                            size = 0,
                                            remaining_size = 0;
    char                                    key_string[KASME_LENGTH_OCTETS * 2];

    OAILOG_INFO (LOG_NAS_EMM, "EMM-CTX: ue id:           " MME_UE_S1AP_ID_FMT " (UE identifier)\n", elm_pP->ue_id);
    OAILOG_INFO (LOG_NAS_EMM, "         is_dynamic:       %u      (Dynamically allocated context indicator)\n", elm_pP->is_dynamic);
    OAILOG_INFO (LOG_NAS_EMM, "         is_attached:      %u      (Attachment indicator)\n", elm_pP->is_attached);
    OAILOG_INFO (LOG_NAS_EMM, "         is_emergency:     %u      (Emergency bearer services indicator)\n", elm_pP->is_emergency);
    IMSI_TO_STRING (&elm_pP->_imsi, imsi_str, 16);
    OAILOG_INFO (LOG_NAS_EMM, "         imsi:             %s      (The IMSI provided by the UE or the MME)\n", imsi_str);
    OAILOG_INFO (LOG_NAS_EMM, "         imei:             TODO    (The IMEI provided by the UE)\n");
    OAILOG_INFO (LOG_NAS_EMM, "         imeisv:           TODO    (The IMEISV provided by the UE)\n");
    OAILOG_INFO (LOG_NAS_EMM, "         guti:             "GUTI_FMT"      (The GUTI assigned to the UE)\n", GUTI_ARG(&elm_pP->_guti));
    OAILOG_INFO (LOG_NAS_EMM, "         old_guti:         "GUTI_FMT"      (The old GUTI)\n", GUTI_ARG(&elm_pP->_old_guti));
    for (k=0; k < elm_pP->_tai_list.n_tais; k++) {
      OAILOG_INFO (LOG_NAS_EMM, "         tai:              "TAI_FMT"   (Tracking area identity the UE is registered to)\n",
        TAI_ARG(&elm_pP->_tai_list.tai[k]));
    }
    OAILOG_INFO (LOG_NAS_EMM, "         eksi:             %u      (Security key set identifier)\n", elm_pP->_security.eksi);
    OAILOG_INFO (LOG_NAS_EMM, "         auth_vector:              (EPS authentication vector)\n");
    OAILOG_INFO (LOG_NAS_EMM, "             kasme: " KASME_FORMAT "" KASME_FORMAT "\n",
                               KASME_DISPLAY_1 (elm_pP->_vector[elm_pP->_security.eksi].kasme),
                               KASME_DISPLAY_2 (elm_pP->_vector[elm_pP->_security.eksi].kasme));
    OAILOG_INFO (LOG_NAS_EMM, "             rand:  " RAND_FORMAT "\n", RAND_DISPLAY (elm_pP->_vector[elm_pP->_security.eksi].rand));
    OAILOG_INFO (LOG_NAS_EMM, "             autn:  " AUTN_FORMAT "\n", AUTN_DISPLAY (elm_pP->_vector[elm_pP->_security.eksi].autn));

    for (k = 0; k < XRES_LENGTH_MAX; k++) {
      sprintf (&key_string[k * 3], "%02x,", elm_pP->_vector[elm_pP->_security.eksi].xres[k]);
    }

    key_string[k * 3 - 1] = '\0';
    OAILOG_INFO (LOG_NAS_EMM, "             xres:  %s\n", key_string);

    if (IS_EMM_CTXT_PRESENT_SECURITY(elm_pP)) {
      OAILOG_INFO (LOG_NAS_EMM, "         security context:          (Current EPS NAS security context)\n");
      OAILOG_INFO (LOG_NAS_EMM, "             type:  %s              (Type of security context)\n",
          (elm_pP->_security.sc_type == SECURITY_CTX_TYPE_NOT_AVAILABLE)  ? "NOT_AVAILABLE" :
          (elm_pP->_security.sc_type == SECURITY_CTX_TYPE_PARTIAL_NATIVE) ? "PARTIAL_NATIVE" :
          (elm_pP->_security.sc_type == SECURITY_CTX_TYPE_FULL_NATIVE)    ? "FULL_NATIVE" :  "MAPPED");
      OAILOG_INFO (LOG_NAS_EMM, "             eksi:  %u              (NAS key set identifier for E-UTRAN)\n", elm_pP->_security.eksi);

      if (SECURITY_CTX_TYPE_PARTIAL_NATIVE <= elm_pP->_security.sc_type) {
        OAILOG_INFO (LOG_NAS_EMM, "             dl_count.overflow: %u\n", elm_pP->_security.dl_count.overflow);
        OAILOG_INFO (LOG_NAS_EMM, "             dl_count.seq_num:  %u\n", elm_pP->_security.dl_count.seq_num);
        OAILOG_INFO (LOG_NAS_EMM, "             ul_count.overflow: %u\n", elm_pP->_security.ul_count.overflow);
        OAILOG_INFO (LOG_NAS_EMM, "             ul_count.seq_num:  %u\n", elm_pP->_security.ul_count.seq_num);

        if (SECURITY_CTX_TYPE_FULL_NATIVE <= elm_pP->_security.sc_type) {
          size = 0;
          remaining_size = KASME_LENGTH_OCTETS * 2;

          for (k = 0; k < AUTH_KNAS_ENC_SIZE; k++) {
            size += snprintf (&key_string[size], remaining_size, "0x%x ", elm_pP->_security.knas_enc[k]);
            remaining_size -= size;
          }

          OAILOG_INFO (LOG_NAS_EMM, "             knas_enc: %s     (NAS cyphering key)\n", key_string);

          size = 0;
          remaining_size = KASME_LENGTH_OCTETS * 2;

          for (k = 0; k < AUTH_KNAS_INT_SIZE; k++) {
            size += snprintf (&key_string[size], remaining_size, "0x%x ", elm_pP->_security.knas_int[k]);
            remaining_size -= size;
          }


          OAILOG_INFO (LOG_NAS_EMM, "             knas_int: %s     (NAS integrity key)\n", key_string);
          OAILOG_INFO (LOG_NAS_EMM, "             TODO  capability");
          OAILOG_INFO (LOG_NAS_EMM, "             selected_algorithms.encryption:  %x\n", elm_pP->_security.selected_algorithms.encryption);
          OAILOG_INFO (LOG_NAS_EMM, "             selected_algorithms.integrity:   %x\n", elm_pP->_security.selected_algorithms.integrity);
        }
      }
    } else {
      OAILOG_INFO (LOG_NAS_EMM, "         No security context\n");
    }

    OAILOG_INFO (LOG_NAS_EMM, "         _emm_fsm_status     %u\n", elm_pP->_emm_fsm_status);
    OAILOG_INFO (LOG_NAS_EMM, "         TODO  esm_data_ctx\n");
  }
}

//------------------------------------------------------------------------------
static bool
emm_data_context_dump_hash_table_wrapper(
    const hash_key_t keyP,
    void *const dataP,
    __attribute__((unused)) void *parameterP,
    __attribute__((unused)) void **resultP)
{
  emm_data_context_dump (dataP);
  return false; // otherwise dump stop
}

//------------------------------------------------------------------------------
void
emm_data_context_dump_all (
  void)
{
  OAILOG_INFO (LOG_NAS_EMM, "EMM-CTX - Dump all contexts:\n");
  hashtable_ts_apply_callback_on_elements (_emm_data.ctx_coll_ue_id, emm_data_context_dump_hash_table_wrapper, NULL, NULL);
}
