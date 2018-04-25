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


#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "log.h"
#include "msc.h"
#include "common_types.h"
#include "conversions.h"
#include "intertask_interface.h"
#include "enum_string.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_itti_messaging.h"
#include "s1ap_mme.h"
#include "timer.h"
#include "mme_app_statistics.h"


static void _mme_app_handle_s1ap_ue_context_release (const mme_ue_s1ap_id_t mme_ue_s1ap_id,
                                                     const enb_ue_s1ap_id_t enb_ue_s1ap_id,
                                                     uint32_t enb_id,
                                                     enum s1cause cause);


//------------------------------------------------------------------------------
ue_context_t *mme_create_new_ue_context (void)
{
  ue_context_t                           *new_p = calloc (1, sizeof (ue_context_t));
  new_p->mme_ue_s1ap_id = INVALID_MME_UE_S1AP_ID;
  new_p->enb_s1ap_id_key = INVALID_ENB_UE_S1AP_ID_KEY;
  // Initialize timers to INVALID IDs
  new_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
  new_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;

  new_p->ue_radio_cap_length = 0;

  new_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  new_p->ue_context_rel_cause = S1AP_INVALID_CAUSE;

  return new_p;
}

//------------------------------------------------------------------------------
void mme_app_ue_context_free_content (ue_context_t * const ue_context_p)
{
  //  imsi64_t         imsi;
  //  unsigned               imsi_auth:1;
  //  enb_ue_s1ap_id_t       enb_ue_s1ap_id:24;
  //  mme_ue_s1ap_id_t       mme_ue_s1ap_id;
  //  uint32_t               ue_id;
  //  uint8_t                nb_of_vectors;
  //  eutran_vector_t       *vector_list;
  //  eutran_vector_t       *vector_in_use;
  //  unsigned               subscription_known:1;
  //  uint8_t                msisdn[MSISDN_LENGTH+1];
  //  uint8_t                msisdn_length;
  //  mm_state_t             mm_state;
  //  guti_t                 guti;
  //  me_identity_t          me_identity;
  //  ecgi_t                  e_utran_cgi;
  //  time_t                 cell_age;
  //  network_access_mode_t  access_mode;
  //  apn_config_profile_t   apn_profile;
  //  ard_t                  access_restriction_data;
  //  subscriber_status_t    sub_status;
  //  ambr_t                 subscribed_ambr;
  //  ambr_t                 used_ambr;
  //  rau_tau_timer_t        rau_tau_timer;
  // int                    ue_radio_cap_length;
  // teid_t                 mme_s11_teid;
  // teid_t                 sgw_s11_teid;
  // PAA_t                  paa;
  // char                   pending_pdn_connectivity_req_imsi[16];
  // uint8_t                pending_pdn_connectivity_req_imsi_length;
  DevAssert(ue_context_p != NULL);
  bdestroy(ue_context_p->pending_pdn_connectivity_req_apn);
  bdestroy(ue_context_p->pending_pdn_connectivity_req_pdn_addr);
  
  // Stop Mobile reachability timer,if running 
  if (ue_context_p->mobile_reachability_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->mobile_reachability_timer.id)) {

      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Mobile Reachability timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
  }
  // Stop Implicit detach timer,if running 
  if (ue_context_p->implicit_detach_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->implicit_detach_timer.id)) {
      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
  }
  // Stop Initial context setup process guard timer,if running 
  if (ue_context_p->initial_context_setup_rsp_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->initial_context_setup_rsp_timer.id)) {
      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Initial Context Setup Rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
  }
  if (ue_context_p->ue_radio_capabilities) {
    free_wrapper((void**) &(ue_context_p->ue_radio_capabilities));
  }

  ue_context_p->ue_radio_cap_length = 0;

  ue_context_p->ue_context_rel_cause = S1AP_INVALID_CAUSE;


  // int                    pending_pdn_connectivity_req_pti;
  // unsigned               pending_pdn_connectivity_req_ue_id;
  // network_qos_t          pending_pdn_connectivity_req_qos;
  // pco_flat_t             pending_pdn_connectivity_req_pco;
  // DO NOT FREE THE FOLLOWING POINTER, IT IS esm_proc_data_t*
  // void                  *pending_pdn_connectivity_req_proc_data;
  //int                    pending_pdn_connectivity_req_request_type;
  //ebi_t                  default_bearer_id;
  //bearer_context_t       eps_bearers[BEARERS_PER_UE];

}

//------------------------------------------------------------------------------
ue_context_t                           *
mme_ue_context_exists_enb_ue_s1ap_id (
  mme_ue_context_t * const mme_ue_context_p,
  const enb_s1ap_id_key_t enb_key)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  void                                   *id = NULL;
  
  hashtable_ts_get (mme_ue_context_p->enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)enb_key, (void **)&id);
  
  if (HASH_TABLE_OK == h_rc) {
    return mme_ue_context_exists_mme_ue_s1ap_id (mme_ue_context_p, (mme_ue_s1ap_id_t)(uintptr_t) id);
  }
  return NULL;
}

//------------------------------------------------------------------------------
ue_context_t                           *
mme_ue_context_exists_mme_ue_s1ap_id (
  mme_ue_context_t * const mme_ue_context_p,
  const mme_ue_s1ap_id_t mme_ue_s1ap_id)
{
  struct ue_context_s                    *ue_context_p = NULL;

  hashtable_ts_get (mme_ue_context_p->mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)mme_ue_s1ap_id, (void **)&ue_context_p);
  return ue_context_p;

}
//------------------------------------------------------------------------------
struct ue_context_s                    *
mme_ue_context_exists_imsi (
  mme_ue_context_t * const mme_ue_context_p,
  const imsi64_t imsi)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  void                                   *id = NULL;

  h_rc = hashtable_ts_get (mme_ue_context_p->imsi_ue_context_htbl, (const hash_key_t)imsi, (void **)&id);

  if (HASH_TABLE_OK == h_rc) {
    return mme_ue_context_exists_mme_ue_s1ap_id (mme_ue_context_p, (mme_ue_s1ap_id_t)(uintptr_t) id);
  }

  return NULL;
}

//------------------------------------------------------------------------------
struct ue_context_s                    *
mme_ue_context_exists_s11_teid (
  mme_ue_context_t * const mme_ue_context_p,
  const s11_teid_t teid)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  void                                   *id = NULL;

  h_rc = hashtable_ts_get (mme_ue_context_p->tun11_ue_context_htbl, (const hash_key_t)teid, (void **)&id);

  if (HASH_TABLE_OK == h_rc) {
    return mme_ue_context_exists_mme_ue_s1ap_id (mme_ue_context_p, (mme_ue_s1ap_id_t)(uintptr_t) id);
  }
  return NULL;
}

//------------------------------------------------------------------------------
ue_context_t                           *
mme_ue_context_exists_guti (
  mme_ue_context_t * const mme_ue_context_p,
  const guti_t * const guti_p)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  void                                   *id = NULL;

  h_rc = obj_hashtable_ts_get (mme_ue_context_p->guti_ue_context_htbl, (const void *)guti_p, sizeof (*guti_p), (void **)&id);

  if (HASH_TABLE_OK == h_rc) {
    return mme_ue_context_exists_mme_ue_s1ap_id (mme_ue_context_p, (mme_ue_s1ap_id_t)(uintptr_t)id);
  }

  return NULL;
}

//------------------------------------------------------------------------------
void mme_app_move_context (ue_context_t *dst, ue_context_t *src)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  if ((dst) && (src)) {
    dst->imsi                = src->imsi;
    dst->imsi_auth           = src->imsi_auth;
    //enb_s1ap_id_key
    //enb_ue_s1ap_id
    //mme_ue_s1ap_id
    dst->sctp_assoc_id_key       = src->sctp_assoc_id_key;
    dst->subscription_known      = src->subscription_known;
    memcpy((void *)dst->msisdn, (const void *)src->msisdn, sizeof(src->msisdn));
    dst->msisdn_length           = src->msisdn_length;src->msisdn_length = 0;
    dst->mm_state                = src->mm_state;
    dst->ecm_state               = src->ecm_state;
    dst->is_guti_set             = src->is_guti_set;
    dst->guti                    = src->guti;
    dst->me_identity             = src->me_identity;
    dst->e_utran_cgi             = src->e_utran_cgi;
    dst->cell_age                = src->cell_age;
    dst->access_mode             = src->access_mode;
    dst->apn_profile             = src->apn_profile;
    dst->access_restriction_data = src->access_restriction_data;
    dst->sub_status              = src->sub_status;
    dst->subscribed_ambr         = src->subscribed_ambr;
    dst->used_ambr               = src->used_ambr;
    dst->rau_tau_timer           = src->rau_tau_timer;
    dst->mme_s11_teid            = src->mme_s11_teid;
    dst->sgw_s11_teid            = src->sgw_s11_teid;
    memcpy((void *)dst->pending_pdn_connectivity_req_imsi, (const void *)src->pending_pdn_connectivity_req_imsi, sizeof(src->pending_pdn_connectivity_req_imsi));
    dst->pending_pdn_connectivity_req_imsi_length = src->pending_pdn_connectivity_req_imsi_length;
    dst->pending_pdn_connectivity_req_apn         = src->pending_pdn_connectivity_req_apn;
    src->pending_pdn_connectivity_req_apn         = NULL;

    dst->pending_pdn_connectivity_req_pdn_addr    = src->pending_pdn_connectivity_req_pdn_addr;
    src->pending_pdn_connectivity_req_pdn_addr    = NULL;
    dst->pending_pdn_connectivity_req_pti         = src->pending_pdn_connectivity_req_pti;
    dst->pending_pdn_connectivity_req_ue_id       = src->pending_pdn_connectivity_req_ue_id;
    dst->pending_pdn_connectivity_req_qos         = src->pending_pdn_connectivity_req_qos;
    dst->pending_pdn_connectivity_req_pco         = src->pending_pdn_connectivity_req_pco;
    dst->pending_pdn_connectivity_req_proc_data   = src->pending_pdn_connectivity_req_proc_data;
    src->pending_pdn_connectivity_req_proc_data   = NULL;
    dst->pending_pdn_connectivity_req_request_type= src->pending_pdn_connectivity_req_request_type;
    dst->default_bearer_id       = src->default_bearer_id;
    memcpy((void *)dst->eps_bearers, (const void *)src->eps_bearers, sizeof(bearer_context_t)*BEARERS_PER_UE);
    OAILOG_DEBUG (LOG_MME_APP,
           "mme_app_move_context("ENB_UE_S1AP_ID_FMT " <- " ENB_UE_S1AP_ID_FMT ") done\n",
           dst->enb_ue_s1ap_id, src->enb_ue_s1ap_id);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
// this is detected only while receiving an INITIAL UE message

/***********************************************IMPORTANT*****************************************************
 * We are not using this function. If plan to use this in future then the key insertion and removal within this 
 * function need to modified.
 **********************************************IMPORTANT*****************************************************/
void
mme_ue_context_duplicate_enb_ue_s1ap_id_detected (
  const enb_s1ap_id_key_t enb_key,
  const mme_ue_s1ap_id_t  mme_ue_s1ap_id,
  const bool              is_remove_old)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  void                                   *id = NULL;
  enb_ue_s1ap_id_t                        enb_ue_s1ap_id = 0;
  enb_s1ap_id_key_t                       old_enb_key = 0;

  OAILOG_FUNC_IN (LOG_MME_APP);
  enb_ue_s1ap_id = MME_APP_ENB_S1AP_ID_KEY2ENB_S1AP_ID(enb_key);

  if (INVALID_MME_UE_S1AP_ID == mme_ue_s1ap_id) {
    OAILOG_ERROR (LOG_MME_APP,
        "Error could not associate this enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " with mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
        enb_ue_s1ap_id, mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  h_rc = hashtable_ts_get (mme_app_desc.mme_ue_contexts.mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)mme_ue_s1ap_id,  (void **)&id);
  if (HASH_TABLE_OK == h_rc) {
    old_enb_key = (enb_s1ap_id_key_t)(uintptr_t) id;
    if (old_enb_key != enb_key) {
      if (is_remove_old) {
        ue_context_t                           *old = NULL;
        /* TODO
         * Insert and remove need to be corrected. mme_ue_s1ap_id is used to point to context ptr and
         * enb_ue_s1ap_id_key is used to point to mme_ue_s1ap_id
         */ 
        h_rc = hashtable_ts_remove (mme_app_desc.mme_ue_contexts.mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)mme_ue_s1ap_id, (void **)&id);
        h_rc = hashtable_ts_insert (mme_app_desc.mme_ue_contexts.mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)mme_ue_s1ap_id, (void *)(uintptr_t)enb_key);
        h_rc = hashtable_ts_remove (mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)old_enb_key, (void **)&old);
        if (HASH_TABLE_OK == h_rc) {
          ue_context_t                           *new = NULL;
          h_rc = hashtable_ts_get (mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)enb_key, (void **)&new);
          mme_app_move_context(new, old);
          mme_app_ue_context_free_content(old);
          OAILOG_DEBUG (LOG_MME_APP,
                  "Removed old UE context enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
                  MME_APP_ENB_S1AP_ID_KEY2ENB_S1AP_ID(old_enb_key), mme_ue_s1ap_id);
        }
      } else {
        ue_context_t                           *new = NULL;
        h_rc = hashtable_ts_remove (mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)enb_key, (void **)&new);
        if (HASH_TABLE_OK == h_rc) {
          mme_app_ue_context_free_content(new);
          OAILOG_DEBUG (LOG_MME_APP,
                  "Removed new UE context enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
                  enb_ue_s1ap_id, mme_ue_s1ap_id);
        }
      }
    } else {
      OAILOG_DEBUG (LOG_MME_APP,
          "No duplicated context found enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " with mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
          enb_ue_s1ap_id, mme_ue_s1ap_id);
    }
  } else {
    OAILOG_ERROR (LOG_MME_APP,
            "Error could find this mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
            mme_ue_s1ap_id);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//------------------------------------------------------------------------------
int
mme_ue_context_notified_new_ue_s1ap_id_association (
  const enb_s1ap_id_key_t  enb_key,
  const mme_ue_s1ap_id_t   mme_ue_s1ap_id)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  ue_context_t                           *ue_context_p = NULL;
  enb_ue_s1ap_id_t                        enb_ue_s1ap_id = 0;

  OAILOG_FUNC_IN (LOG_MME_APP);
  
  if (INVALID_MME_UE_S1AP_ID == mme_ue_s1ap_id) {
    OAILOG_ERROR (LOG_MME_APP,
        "Error could not associate this enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " with mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
        enb_ue_s1ap_id, mme_ue_s1ap_id);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }

  ue_context_p = mme_ue_context_exists_enb_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, enb_key);
  if (ue_context_p) {
    if (ue_context_p->enb_s1ap_id_key == enb_key) { // useless
      if (INVALID_MME_UE_S1AP_ID == ue_context_p->mme_ue_s1ap_id) {
        // new insertion of mme_ue_s1ap_id, not a change in the id
        h_rc = hashtable_ts_insert (mme_app_desc.mme_ue_contexts.mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)mme_ue_s1ap_id, (void *)ue_context_p);
        if (HASH_TABLE_OK == h_rc) {
          ue_context_p->mme_ue_s1ap_id = mme_ue_s1ap_id;
          OAILOG_DEBUG (LOG_MME_APP,
              "Associated this enb_ue_s1ap_ue_id " ENB_UE_S1AP_ID_FMT " with mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
              ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id);

          s1ap_notified_new_ue_mme_s1ap_id_association (ue_context_p->sctp_assoc_id_key,ue_context_p-> enb_ue_s1ap_id, mme_ue_s1ap_id);
          OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNok);
        }
      }
    }
  }
  OAILOG_ERROR (LOG_MME_APP,
      "Error could not associate this enb_ue_s1ap_ue_id " ENB_UE_S1AP_ID_FMT " with mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
      enb_ue_s1ap_id, mme_ue_s1ap_id);
  OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
}
//------------------------------------------------------------------------------
void
mme_ue_context_update_coll_keys (
  mme_ue_context_t * const mme_ue_context_p,
  ue_context_t     * const ue_context_p,
  const enb_s1ap_id_key_t  enb_s1ap_id_key,
  const mme_ue_s1ap_id_t   mme_ue_s1ap_id,
  const imsi64_t     imsi,
  const s11_teid_t         mme_s11_teid,
  const guti_t     * const guti_p)  //  never NULL, if none put &ue_context_p->guti
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;
  void                                   *id = NULL;

  OAILOG_FUNC_IN(LOG_MME_APP);

  OAILOG_TRACE (LOG_MME_APP, "Update ue context.old_enb_ue_s1ap_id_key %ld ue context.old_mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " ue context.old_IMSI " IMSI_64_FMT " ue context.old_GUTI "GUTI_FMT"\n",
             ue_context_p->enb_s1ap_id_key, ue_context_p->mme_ue_s1ap_id, ue_context_p->imsi, GUTI_ARG(&ue_context_p->guti));

  OAILOG_TRACE (LOG_MME_APP, "Update ue context %p updated_enb_ue_s1ap_id_key %ld updated_mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " updated_IMSI " IMSI_64_FMT " updated_GUTI " GUTI_FMT "\n",
            ue_context_p, enb_s1ap_id_key, mme_ue_s1ap_id, imsi, GUTI_ARG(guti_p));

  if ((INVALID_ENB_UE_S1AP_ID_KEY != enb_s1ap_id_key) && (ue_context_p->enb_s1ap_id_key != enb_s1ap_id_key)) {
      // new insertion of enb_ue_s1ap_id_key,
      h_rc = hashtable_ts_remove (mme_ue_context_p->enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->enb_s1ap_id_key, (void **)&id);
      h_rc = hashtable_ts_insert (mme_ue_context_p->enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)enb_s1ap_id_key, (void *)(uintptr_t)mme_ue_s1ap_id);

      if (HASH_TABLE_OK != h_rc) {
        OAILOG_ERROR (LOG_MME_APP,
            "Error could not update this ue context %p enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " %s\n",
            ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, hashtable_rc_code2string(h_rc));
      }
      ue_context_p->enb_s1ap_id_key = enb_s1ap_id_key;
    }


  if ((INVALID_MME_UE_S1AP_ID != mme_ue_s1ap_id) && (ue_context_p->mme_ue_s1ap_id != mme_ue_s1ap_id)) {
      // new insertion of mme_ue_s1ap_id, not a change in the id
      h_rc = hashtable_ts_remove (mme_ue_context_p->mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->mme_ue_s1ap_id,  (void **)&ue_context_p);
      h_rc = hashtable_ts_insert (mme_ue_context_p->mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)mme_ue_s1ap_id, (void *)ue_context_p);

      if (HASH_TABLE_OK != h_rc) {
        OAILOG_ERROR (LOG_MME_APP,
            "Error could not update this ue context %p enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " %s\n",
            ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, hashtable_rc_code2string(h_rc));
      }
      ue_context_p->mme_ue_s1ap_id = mme_ue_s1ap_id;

    if (INVALID_IMSI64 != imsi) {
      h_rc = hashtable_ts_remove (mme_ue_context_p->imsi_ue_context_htbl, (const hash_key_t)ue_context_p->imsi, (void **)&id);
      h_rc = hashtable_ts_insert (mme_ue_context_p->imsi_ue_context_htbl, (const hash_key_t)imsi, (void *)(uintptr_t)mme_ue_s1ap_id);
      if (HASH_TABLE_OK != h_rc) {
       OAILOG_ERROR (LOG_MME_APP,
          "Error could not update this ue context %p enb_ue_s1ap_ue_id " ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " imsi " IMSI_64_FMT ": %s\n",
          ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, imsi, hashtable_rc_code2string(h_rc));
    }
      ue_context_p->imsi = imsi;
    }
    h_rc = hashtable_ts_remove (mme_ue_context_p->tun11_ue_context_htbl, (const hash_key_t)ue_context_p->mme_s11_teid, (void **)&id);
    h_rc = hashtable_ts_insert (mme_ue_context_p->tun11_ue_context_htbl, (const hash_key_t)mme_s11_teid, (void *)(uintptr_t)mme_ue_s1ap_id);
    if (HASH_TABLE_OK != h_rc) {
      OAILOG_TRACE (LOG_MME_APP,
          "Error could not update this ue context %p enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " mme_s11_teid " TEID_FMT " : %s\n",
          ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, mme_s11_teid, hashtable_rc_code2string(h_rc));
    }
    ue_context_p->mme_s11_teid = mme_s11_teid;

    if (guti_p)
    {
      h_rc = obj_hashtable_ts_remove (mme_ue_context_p->guti_ue_context_htbl, (const void *const)&ue_context_p->guti, sizeof (ue_context_p->guti), (void **)&id);
      h_rc = obj_hashtable_ts_insert (mme_ue_context_p->guti_ue_context_htbl, (const void *const)guti_p, sizeof (*guti_p), (void *)(uintptr_t)mme_ue_s1ap_id);
      if (HASH_TABLE_OK != h_rc) {
        OAILOG_TRACE (LOG_MME_APP, "Error could not update this ue context %p enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " guti " GUTI_FMT " %s\n",
            ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, GUTI_ARG(guti_p), hashtable_rc_code2string(h_rc));
      }
      ue_context_p->guti = *guti_p;
    }
  }

  if ((ue_context_p->imsi != imsi)
      || (ue_context_p->mme_ue_s1ap_id != mme_ue_s1ap_id)) {
    h_rc = hashtable_ts_remove (mme_ue_context_p->imsi_ue_context_htbl, (const hash_key_t)ue_context_p->imsi, (void **)&id);
    if (INVALID_MME_UE_S1AP_ID != mme_ue_s1ap_id) {
      h_rc = hashtable_ts_insert (mme_ue_context_p->imsi_ue_context_htbl, (const hash_key_t)imsi, (void *)(uintptr_t)mme_ue_s1ap_id);
    } else {
      h_rc = HASH_TABLE_KEY_NOT_EXISTS;
    }
    if (HASH_TABLE_OK != h_rc) {
      OAILOG_TRACE (LOG_MME_APP,
          "Error could not update this ue context %p enb_ue_s1ap_ue_id " ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " imsi " IMSI_64_FMT ": %s\n",
          ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, imsi, hashtable_rc_code2string(h_rc));
    }
    ue_context_p->imsi = imsi;
  }

  if ((ue_context_p->mme_s11_teid != mme_s11_teid)
      || (ue_context_p->mme_ue_s1ap_id != mme_ue_s1ap_id)) {
    h_rc = hashtable_ts_remove (mme_ue_context_p->tun11_ue_context_htbl, (const hash_key_t)ue_context_p->mme_s11_teid, (void **)&id);
    if (INVALID_MME_UE_S1AP_ID != mme_ue_s1ap_id) {
      h_rc = hashtable_ts_insert (mme_ue_context_p->tun11_ue_context_htbl, (const hash_key_t)mme_s11_teid, (void *)(uintptr_t)mme_ue_s1ap_id);
    } else {
      h_rc = HASH_TABLE_KEY_NOT_EXISTS;
    }

    if (HASH_TABLE_OK != h_rc) {
      OAILOG_TRACE (LOG_MME_APP,
          "Error could not update this ue context %p enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " mme_s11_teid " TEID_FMT " : %s\n",
          ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, mme_s11_teid, hashtable_rc_code2string(h_rc));
    }
    ue_context_p->mme_s11_teid = mme_s11_teid;
  }

  if (guti_p) {
    if ((guti_p->gummei.mme_code != ue_context_p->guti.gummei.mme_code)
      || (guti_p->gummei.mme_gid != ue_context_p->guti.gummei.mme_gid)
      || (guti_p->m_tmsi != ue_context_p->guti.m_tmsi)
      || (guti_p->gummei.plmn.mcc_digit1 != ue_context_p->guti.gummei.plmn.mcc_digit1)
      || (guti_p->gummei.plmn.mcc_digit2 != ue_context_p->guti.gummei.plmn.mcc_digit2)
      || (guti_p->gummei.plmn.mcc_digit3 != ue_context_p->guti.gummei.plmn.mcc_digit3)
      || (ue_context_p->mme_ue_s1ap_id != mme_ue_s1ap_id)) {

      // may check guti_p with a kind of instanceof()?
      h_rc = obj_hashtable_ts_remove (mme_ue_context_p->guti_ue_context_htbl, &ue_context_p->guti, sizeof (*guti_p), (void **)&id);
      if (INVALID_MME_UE_S1AP_ID != mme_ue_s1ap_id) {
        h_rc = obj_hashtable_ts_insert (mme_ue_context_p->guti_ue_context_htbl, (const void *const)guti_p, sizeof (*guti_p), (void *)(uintptr_t)mme_ue_s1ap_id);
      } else {
        h_rc = HASH_TABLE_KEY_NOT_EXISTS;
      }

      if (HASH_TABLE_OK != h_rc) {
        OAILOG_TRACE (LOG_MME_APP, "Error could not update this ue context %p enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " guti " GUTI_FMT " %s\n",
            ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, GUTI_ARG(guti_p), hashtable_rc_code2string(h_rc));
      }
      ue_context_p->guti = *guti_p;
    }
  }
  OAILOG_FUNC_OUT(LOG_MME_APP);
}

//------------------------------------------------------------------------------
void mme_ue_context_dump_coll_keys(void)
{
  bstring tmp = bfromcstr(" ");
  btrunc(tmp, 0);

  hashtable_ts_dump_content (mme_app_desc.mme_ue_contexts.imsi_ue_context_htbl, tmp);
  OAILOG_TRACE (LOG_MME_APP,"imsi_ue_context_htbl %s\n", bdata(tmp));

  btrunc(tmp, 0);
  hashtable_ts_dump_content (mme_app_desc.mme_ue_contexts.tun11_ue_context_htbl, tmp);
  OAILOG_TRACE (LOG_MME_APP,"tun11_ue_context_htbl %s\n", bdata(tmp));

  btrunc(tmp, 0);
  hashtable_ts_dump_content (mme_app_desc.mme_ue_contexts.mme_ue_s1ap_id_ue_context_htbl, tmp);
  OAILOG_TRACE (LOG_MME_APP,"mme_ue_s1ap_id_ue_context_htbl %s\n", bdata(tmp));

  btrunc(tmp, 0);
  hashtable_ts_dump_content (mme_app_desc.mme_ue_contexts.enb_ue_s1ap_id_ue_context_htbl, tmp);
  OAILOG_TRACE (LOG_MME_APP,"enb_ue_s1ap_id_ue_context_htbl %s\n", bdata(tmp));

  btrunc(tmp, 0);
  obj_hashtable_ts_dump_content (mme_app_desc.mme_ue_contexts.guti_ue_context_htbl, tmp);
  OAILOG_TRACE (LOG_MME_APP,"guti_ue_context_htbl %s", bdata(tmp));
}

//------------------------------------------------------------------------------
int
mme_insert_ue_context (
  mme_ue_context_t * const mme_ue_context_p,
  const struct ue_context_s *const ue_context_p)
{
  hashtable_rc_t                          h_rc = HASH_TABLE_OK;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (mme_ue_context_p );
  DevAssert (ue_context_p );


  // filled ENB UE S1AP ID
  h_rc = hashtable_ts_is_key_exists (mme_ue_context_p->enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->enb_s1ap_id_key);
  if (HASH_TABLE_OK == h_rc) {
    OAILOG_DEBUG (LOG_MME_APP, "This ue context %p already exists enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT "\n",
        ue_context_p, ue_context_p->enb_ue_s1ap_id);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }
  h_rc = hashtable_ts_insert (mme_ue_context_p->enb_ue_s1ap_id_ue_context_htbl,
                             (const hash_key_t)ue_context_p->enb_s1ap_id_key,
                              (void *)((uintptr_t)ue_context_p->mme_ue_s1ap_id));

  if (HASH_TABLE_OK != h_rc) {
    OAILOG_DEBUG (LOG_MME_APP, "Error could not register this ue context %p enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT " ue_id 0x%x\n",
        ue_context_p, ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }

  if (INVALID_MME_UE_S1AP_ID != ue_context_p->mme_ue_s1ap_id) {
    h_rc = hashtable_ts_is_key_exists (mme_ue_context_p->mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->mme_ue_s1ap_id);

    if (HASH_TABLE_OK == h_rc) {
      OAILOG_DEBUG (LOG_MME_APP, "This ue context %p already exists mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
          ue_context_p, ue_context_p->mme_ue_s1ap_id);
      OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
    }

    h_rc = hashtable_ts_insert (mme_ue_context_p->mme_ue_s1ap_id_ue_context_htbl,
                                (const hash_key_t)ue_context_p->mme_ue_s1ap_id,
                                (void *)ue_context_p);

    if (HASH_TABLE_OK != h_rc) {
      OAILOG_DEBUG (LOG_MME_APP, "Error could not register this ue context %p mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
          ue_context_p, ue_context_p->mme_ue_s1ap_id);
      OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
    }

    // filled IMSI
    if (ue_context_p->imsi) {
      h_rc = hashtable_ts_insert (mme_ue_context_p->imsi_ue_context_htbl,
                                  (const hash_key_t)ue_context_p->imsi,
                                  (void *)((uintptr_t)ue_context_p->mme_ue_s1ap_id));

      if (HASH_TABLE_OK != h_rc) {
        OAILOG_DEBUG (LOG_MME_APP, "Error could not register this ue context %p mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " imsi %" SCNu64 "\n",
            ue_context_p, ue_context_p->mme_ue_s1ap_id, ue_context_p->imsi);
        OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
      }
    }

    // filled S11 tun id
    if (ue_context_p->mme_s11_teid) {
      h_rc = hashtable_ts_insert (mme_ue_context_p->tun11_ue_context_htbl,
                                 (const hash_key_t)ue_context_p->mme_s11_teid,
                                 (void *)((uintptr_t)ue_context_p->mme_ue_s1ap_id));

      if (HASH_TABLE_OK != h_rc) {
        OAILOG_DEBUG (LOG_MME_APP, "Error could not register this ue context %p mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " mme_s11_teid " TEID_FMT "\n",
            ue_context_p, ue_context_p->mme_ue_s1ap_id, ue_context_p->mme_s11_teid);
        OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
      }
    }

    // filled guti
    if ((0 != ue_context_p->guti.gummei.mme_code) || (0 != ue_context_p->guti.gummei.mme_gid) || (0 != ue_context_p->guti.m_tmsi) || (0 != ue_context_p->guti.gummei.plmn.mcc_digit1) ||     // MCC 000 does not exist in ITU table
        (0 != ue_context_p->guti.gummei.plmn.mcc_digit2)
        || (0 != ue_context_p->guti.gummei.plmn.mcc_digit3)) {

      h_rc = obj_hashtable_ts_insert (mme_ue_context_p->guti_ue_context_htbl,
                                     (const void *const)&ue_context_p->guti,
                                     sizeof (ue_context_p->guti),
                                     (void *)((uintptr_t)ue_context_p->mme_ue_s1ap_id));

      if (HASH_TABLE_OK != h_rc) {
        OAILOG_DEBUG (LOG_MME_APP, "Error could not register this ue context %p mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " guti "GUTI_FMT"\n",
                ue_context_p, ue_context_p->mme_ue_s1ap_id, GUTI_ARG(&ue_context_p->guti));
        OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
      }
    }
  }
  OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNok);
}
//------------------------------------------------------------------------------
void mme_notify_ue_context_released (
    mme_ue_context_t * const mme_ue_context_p,
    struct ue_context_s *ue_context_p)
{
  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (mme_ue_context_p);
  DevAssert (ue_context_p);
  // TODO HERE free resources

  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//------------------------------------------------------------------------------
void mme_remove_ue_context (
  mme_ue_context_t * const mme_ue_context_p,
  struct ue_context_s *ue_context_p)
{
  unsigned int                           *id = NULL;
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (mme_ue_context_p);
  DevAssert (ue_context_p);
  
  // IMSI 
  if (ue_context_p->imsi) {
    hash_rc = hashtable_ts_remove (mme_ue_context_p->imsi_ue_context_htbl, (const hash_key_t)ue_context_p->imsi, (void **)&id);
    if (HASH_TABLE_OK != hash_rc)
      OAILOG_DEBUG(LOG_MME_APP, "UE context enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT ", IMSI %" SCNu64 "  not in IMSI collection",
          ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, ue_context_p->imsi);
  }
  
  // eNB UE S1P UE ID
  hash_rc = hashtable_ts_remove (mme_ue_context_p->enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->enb_s1ap_id_key, (void **)&id);
  if (HASH_TABLE_OK != hash_rc)
    OAILOG_DEBUG(LOG_MME_APP, "UE context enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT ", ENB_UE_S1AP_ID not ENB_UE_S1AP_ID collection",
      ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id);
  
  // filled S11 tun id
  if (ue_context_p->mme_s11_teid) {
    hash_rc = hashtable_ts_remove (mme_ue_context_p->tun11_ue_context_htbl, (const hash_key_t)ue_context_p->mme_s11_teid, (void **)&id);
    if (HASH_TABLE_OK != hash_rc)
      OAILOG_DEBUG(LOG_MME_APP, "UE context enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT ", MME S11 TEID  " TEID_FMT "  not in S11 collection",
          ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id, ue_context_p->mme_s11_teid);
  }
  // filled guti
  if ((ue_context_p->guti.gummei.mme_code) || (ue_context_p->guti.gummei.mme_gid) || (ue_context_p->guti.m_tmsi) ||
      (ue_context_p->guti.gummei.plmn.mcc_digit1) || (ue_context_p->guti.gummei.plmn.mcc_digit2) || (ue_context_p->guti.gummei.plmn.mcc_digit3)) { // MCC 000 does not exist in ITU table
    hash_rc = obj_hashtable_ts_remove (mme_ue_context_p->guti_ue_context_htbl, (const void *const)&ue_context_p->guti, sizeof (ue_context_p->guti), (void **)&id);
    if (HASH_TABLE_OK != hash_rc)
      OAILOG_DEBUG(LOG_MME_APP, "UE context enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT ", GUTI  not in GUTI collection",
          ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id);
  }
  
  // filled NAS UE ID/ MME UE S1AP ID
  if (INVALID_MME_UE_S1AP_ID != ue_context_p->mme_ue_s1ap_id) {
    hash_rc = hashtable_ts_remove (mme_ue_context_p->mme_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->mme_ue_s1ap_id, (void **)&ue_context_p);
    if (HASH_TABLE_OK != hash_rc)
      OAILOG_DEBUG(LOG_MME_APP, "UE context enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT ", mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " not in MME UE S1AP ID collection",
          ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id);
  }

  mme_app_ue_context_free_content(ue_context_p);
  free_wrapper ((void**) &ue_context_p);
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
void mme_ue_context_update_ue_sig_connection_state (
  mme_ue_context_t * const mme_ue_context_p,
  struct ue_context_s *ue_context_p,
  ecm_state_t new_ecm_state)
{
  // Function is used to update UE's Signaling Connection State 
  hashtable_rc_t                          hash_rc = HASH_TABLE_OK;
  unsigned int                           *id = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (mme_ue_context_p);
  DevAssert (ue_context_p);
  if (new_ecm_state == ECM_IDLE)
  {
    hash_rc = hashtable_ts_remove (mme_ue_context_p->enb_ue_s1ap_id_ue_context_htbl, (const hash_key_t)ue_context_p->enb_s1ap_id_key, (void **)&id);
    if (HASH_TABLE_OK != hash_rc) 
    {
      OAILOG_DEBUG(LOG_MME_APP, "UE context enb_ue_s1ap_ue_id_key %ld mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT ", ENB_UE_S1AP_ID_KEY could not be found",
                                  ue_context_p->enb_s1ap_id_key, ue_context_p->mme_ue_s1ap_id);
    }
    ue_context_p->enb_s1ap_id_key = INVALID_ENB_UE_S1AP_ID_KEY;

    OAILOG_DEBUG (LOG_MME_APP, "MME_APP: UE Connection State changed to IDLE. mme_ue_s1ap_id = %d\n", ue_context_p->mme_ue_s1ap_id);
    
    if (mme_config.nas_config.t3412_min > 0) {
      // Start Mobile reachability timer only if peroidic TAU timer is not disabled 
      if (timer_setup (ue_context_p->mobile_reachability_timer.sec, 0, TASK_MME_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT, (void *)&(ue_context_p->mme_ue_s1ap_id), &(ue_context_p->mobile_reachability_timer.id)) < 0) {
        OAILOG_ERROR (LOG_MME_APP, "Failed to start Mobile Reachability timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
        ue_context_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
      } else {
        OAILOG_DEBUG (LOG_MME_APP, "Started Mobile Reachability timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
      }
    }
    if (ue_context_p->ecm_state == ECM_CONNECTED) {
      ue_context_p->ecm_state       = ECM_IDLE;
      // Update Stats
      update_mme_app_stats_connected_ue_sub();
    }

  }else if ((ue_context_p->ecm_state == ECM_IDLE) && (new_ecm_state == ECM_CONNECTED))
  {
    ue_context_p->ecm_state = ECM_CONNECTED;

    OAILOG_DEBUG (LOG_MME_APP, "MME_APP: UE Connection State changed to CONNECTED.enb_ue_s1ap_id = %d, mme_ue_s1ap_id = %d\n", ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id);
    
    // Stop Mobile reachability timer,if running 
    if (ue_context_p->mobile_reachability_timer.id != MME_APP_TIMER_INACTIVE_ID)
    {
      if (timer_remove(ue_context_p->mobile_reachability_timer.id)) {

        OAILOG_ERROR (LOG_MME_APP, "Failed to stop Mobile Reachability timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
      } 
      ue_context_p->mobile_reachability_timer.id = MME_APP_TIMER_INACTIVE_ID;
    }
    // Stop Implicit detach timer,if running 
    if (ue_context_p->implicit_detach_timer.id != MME_APP_TIMER_INACTIVE_ID)
    {
      if (timer_remove(ue_context_p->implicit_detach_timer.id)) {
        OAILOG_ERROR (LOG_MME_APP, "Failed to stop Implicit Detach timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
      } 
      ue_context_p->implicit_detach_timer.id = MME_APP_TIMER_INACTIVE_ID;
    }
    // Update Stats
    update_mme_app_stats_connected_ue_add();
  }
  return;
}
//------------------------------------------------------------------------------
bool
mme_app_dump_ue_context (
  const hash_key_t keyP,
  void *const ue_context_pP,
  void *unused_param_pP,
  void** unused_result_pP)
//------------------------------------------------------------------------------
{
  struct ue_context_s                    *const context_p = (struct ue_context_s *)ue_context_pP;
  uint8_t                                 j = 0;

  OAILOG_DEBUG (LOG_MME_APP, "-----------------------UE context %p --------------------\n", ue_context_pP);
  if (context_p) {
    OAILOG_DEBUG (LOG_MME_APP, "    - IMSI ...........: " IMSI_64_FMT "\n", context_p->imsi);
    OAILOG_DEBUG (LOG_MME_APP, "                        |  m_tmsi  | mmec | mmegid | mcc | mnc |\n");
    OAILOG_DEBUG (LOG_MME_APP, "    - GUTI............: | %08x |  %02x  |  %04x  | %03u | %03u |\n", context_p->guti.m_tmsi, context_p->guti.gummei.mme_code, context_p->guti.gummei.mme_gid,
                 /*
                  * TODO check if two or three digits MNC...
                  */
        context_p->guti.gummei.plmn.mcc_digit3 * 100 +
        context_p->guti.gummei.plmn.mcc_digit2 * 10 + context_p->guti.gummei.plmn.mcc_digit1,
        context_p->guti.gummei.plmn.mnc_digit3 * 100 + context_p->guti.gummei.plmn.mnc_digit2 * 10 + context_p->guti.gummei.plmn.mnc_digit1);
    OAILOG_DEBUG (LOG_MME_APP, "    - Authenticated ..: %s\n", (context_p->imsi_auth == IMSI_UNAUTHENTICATED) ? "FALSE" : "TRUE");
    OAILOG_DEBUG (LOG_MME_APP, "    - eNB UE s1ap ID .: %08x\n", context_p->enb_ue_s1ap_id);
    OAILOG_DEBUG (LOG_MME_APP, "    - MME UE s1ap ID .: %08x\n", context_p->mme_ue_s1ap_id);
    OAILOG_DEBUG (LOG_MME_APP, "    - MME S11 TEID ...: %08x\n", context_p->mme_s11_teid);
    OAILOG_DEBUG (LOG_MME_APP, "    - SGW S11 TEID ...: %08x\n", context_p->sgw_s11_teid);
    OAILOG_DEBUG (LOG_MME_APP, "                        | mcc | mnc | cell identity |\n");
    OAILOG_DEBUG (LOG_MME_APP, "    - E-UTRAN CGI ....: | %03u | %03u | %05x.%02x    |\n",
                 context_p->e_utran_cgi.plmn.mcc_digit3 * 100 +
                 context_p->e_utran_cgi.plmn.mcc_digit2 * 10 +
                 context_p->e_utran_cgi.plmn.mcc_digit1,
                 context_p->e_utran_cgi.plmn.mnc_digit3 * 100 + context_p->e_utran_cgi.plmn.mnc_digit2 * 10 + context_p->e_utran_cgi.plmn.mnc_digit1,
                 context_p->e_utran_cgi.cell_identity.enb_id, context_p->e_utran_cgi.cell_identity.cell_id);
    /*
     * Ctime return a \n in the string
     */
    OAILOG_DEBUG (LOG_MME_APP, "    - Last acquired ..: %s", ctime (&context_p->cell_age));

    /*
     * Display UE info only if we know them
     */
    if (SUBSCRIPTION_KNOWN == context_p->subscription_known) {
      OAILOG_DEBUG (LOG_MME_APP, "    - Status .........: %s\n", (context_p->sub_status == SS_SERVICE_GRANTED) ? "Granted" : "Barred");
#define DISPLAY_BIT_MASK_PRESENT(mASK)   \
    ((context_p->access_restriction_data & mASK) ? 'X' : 'O')
      OAILOG_DEBUG (LOG_MME_APP, "    (O = allowed, X = !O) |UTRAN|GERAN|GAN|HSDPA EVO|E_UTRAN|HO TO NO 3GPP|\n");
      OAILOG_DEBUG (LOG_MME_APP,
          "    - Access restriction  |  %c  |  %c  | %c |    %c    |   %c   |      %c      |\n",
          DISPLAY_BIT_MASK_PRESENT (ARD_UTRAN_NOT_ALLOWED),
          DISPLAY_BIT_MASK_PRESENT (ARD_GERAN_NOT_ALLOWED),
          DISPLAY_BIT_MASK_PRESENT (ARD_GAN_NOT_ALLOWED), DISPLAY_BIT_MASK_PRESENT (ARD_I_HSDPA_EVO_NOT_ALLOWED), DISPLAY_BIT_MASK_PRESENT (ARD_E_UTRAN_NOT_ALLOWED), DISPLAY_BIT_MASK_PRESENT (ARD_HO_TO_NON_3GPP_NOT_ALLOWED));
      OAILOG_DEBUG (LOG_MME_APP, "    - Access Mode ....: %s\n", ACCESS_MODE_TO_STRING (context_p->access_mode));
      OAILOG_DEBUG (LOG_MME_APP, "    - MSISDN .........: %-*s\n", MSISDN_LENGTH, context_p->msisdn);
      OAILOG_DEBUG (LOG_MME_APP, "    - RAU/TAU timer ..: %u\n", context_p->rau_tau_timer);
      OAILOG_DEBUG (LOG_MME_APP, "    - IMEISV .........: %*s\n", IMEISV_DIGITS_MAX, context_p->me_identity.imeisv);
      OAILOG_DEBUG (LOG_MME_APP, "    - AMBR (bits/s)     ( Downlink |  Uplink  )\n");
      OAILOG_DEBUG (LOG_MME_APP, "        Subscribed ...: (%010" PRIu64 "|%010" PRIu64 ")\n", context_p->subscribed_ambr.br_dl, context_p->subscribed_ambr.br_ul);
      OAILOG_DEBUG (LOG_MME_APP, "        Allocated ....: (%010" PRIu64 "|%010" PRIu64 ")\n", context_p->used_ambr.br_dl, context_p->used_ambr.br_ul);

      OAILOG_DEBUG (LOG_MME_APP, "    - PDN List:\n");

      for (j = 0; j < context_p->apn_profile.nb_apns; j++) {
        struct apn_configuration_s             *apn_config_p;

        apn_config_p = &context_p->apn_profile.apn_configuration[j];
        /*
         * Default APN ?
         */
        OAILOG_DEBUG (LOG_MME_APP, "        - Default APN ...: %s\n", (apn_config_p->context_identifier == context_p->apn_profile.context_identifier)
                     ? "TRUE" : "FALSE");
        OAILOG_DEBUG (LOG_MME_APP, "        - APN ...........: %s\n", apn_config_p->service_selection);
        OAILOG_DEBUG (LOG_MME_APP, "        - AMBR (bits/s) ( Downlink |  Uplink  )\n");
        OAILOG_DEBUG (LOG_MME_APP, "                        (%010" PRIu64 "|%010" PRIu64 ")\n", apn_config_p->ambr.br_dl, apn_config_p->ambr.br_ul);
        OAILOG_DEBUG (LOG_MME_APP, "        - PDN type ......: %s\n", PDN_TYPE_TO_STRING (apn_config_p->pdn_type));
        OAILOG_DEBUG (LOG_MME_APP, "        - QOS\n");
        OAILOG_DEBUG (LOG_MME_APP, "            QCI .........: %u\n", apn_config_p->subscribed_qos.qci);
        OAILOG_DEBUG (LOG_MME_APP, "            Prio level ..: %u\n", apn_config_p->subscribed_qos.allocation_retention_priority.priority_level);
        OAILOG_DEBUG (LOG_MME_APP, "            Pre-emp vul .: %s\n", (apn_config_p->subscribed_qos.allocation_retention_priority.pre_emp_vulnerability == PRE_EMPTION_VULNERABILITY_ENABLED) ? "ENABLED" : "DISABLED");
        OAILOG_DEBUG (LOG_MME_APP, "            Pre-emp cap .: %s\n", (apn_config_p->subscribed_qos.allocation_retention_priority.pre_emp_capability == PRE_EMPTION_CAPABILITY_ENABLED) ? "ENABLED" : "DISABLED");

        if (apn_config_p->nb_ip_address == 0) {
          OAILOG_DEBUG (LOG_MME_APP, "            IP addr .....: Dynamic allocation\n");
        } else {
          int                                     i;

          OAILOG_DEBUG (LOG_MME_APP, "            IP addresses :\n");

          for (i = 0; i < apn_config_p->nb_ip_address; i++) {
            if (apn_config_p->ip_address[i].pdn_type == IPv4) {
              OAILOG_DEBUG (LOG_MME_APP, "                           [" IPV4_ADDR "]\n", IPV4_ADDR_DISPLAY_8 (apn_config_p->ip_address[i].address.ipv4_address));
            } else {
              char                                    ipv6[40];

              inet_ntop (AF_INET6, apn_config_p->ip_address[i].address.ipv6_address, ipv6, 40);
              OAILOG_DEBUG (LOG_MME_APP, "                           [%s]\n", ipv6);
            }
          }
        }
        OAILOG_DEBUG (LOG_MME_APP, "\n");
      }
      OAILOG_DEBUG (LOG_MME_APP, "    - Bearer List:\n");

      for (j = 0; j < BEARERS_PER_UE; j++) {
        bearer_context_t                       *bearer_context_p;

        bearer_context_p = &context_p->eps_bearers[j];

        if (bearer_context_p->s_gw_teid != 0) {
          OAILOG_DEBUG (LOG_MME_APP, "        Bearer id .......: %02u\n", j);
          OAILOG_DEBUG (LOG_MME_APP, "        S-GW TEID (UP)...: %08x\n", bearer_context_p->s_gw_teid);
          OAILOG_DEBUG (LOG_MME_APP, "        P-GW TEID (UP)...: %08x\n", bearer_context_p->p_gw_teid);
          OAILOG_DEBUG (LOG_MME_APP, "        QCI .............: %u\n", bearer_context_p->qci);
          OAILOG_DEBUG (LOG_MME_APP, "        Priority level ..: %u\n", bearer_context_p->prio_level);
          OAILOG_DEBUG (LOG_MME_APP, "        Pre-emp vul .....: %s\n", (bearer_context_p->pre_emp_vulnerability == PRE_EMPTION_VULNERABILITY_ENABLED) ? "ENABLED" : "DISABLED");
          OAILOG_DEBUG (LOG_MME_APP, "        Pre-emp cap .....: %s\n", (bearer_context_p->pre_emp_capability == PRE_EMPTION_CAPABILITY_ENABLED) ? "ENABLED" : "DISABLED");
        }
      }
    }
    OAILOG_DEBUG (LOG_MME_APP, "---------------------------------------------------------\n");
    return false;
  }
  OAILOG_DEBUG (LOG_MME_APP, "---------------------------------------------------------\n");
  return true;
}


//------------------------------------------------------------------------------
void
mme_app_dump_ue_contexts (
  const mme_ue_context_t * const mme_ue_context_p)
//------------------------------------------------------------------------------
{
  hashtable_ts_apply_callback_on_elements (mme_ue_context_p->mme_ue_s1ap_id_ue_context_htbl, mme_app_dump_ue_context, NULL, NULL);
}


void
mme_app_handle_s1ap_ue_context_release_req (
  const itti_s1ap_ue_context_release_req_t const *s1ap_ue_context_release_req)
//------------------------------------------------------------------------------
{
  _mme_app_handle_s1ap_ue_context_release(s1ap_ue_context_release_req->mme_ue_s1ap_id,
                                          s1ap_ue_context_release_req->enb_ue_s1ap_id,
                                          s1ap_ue_context_release_req->enb_id,
                                          S1AP_RADIO_EUTRAN_GENERATED_REASON);
}

void
mme_app_handle_enb_deregister_ind(const itti_s1ap_eNB_deregistered_ind_t const * eNB_deregistered_ind) {
  for (int i = 0; i < eNB_deregistered_ind->nb_ue_to_deregister; i++) {
    _mme_app_handle_s1ap_ue_context_release(eNB_deregistered_ind->mme_ue_s1ap_id[i],
                                            eNB_deregistered_ind->enb_ue_s1ap_id[i],
                                            eNB_deregistered_ind->enb_id,
                                            S1AP_SCTP_SHUTDOWN_OR_RESET);
  }
}

/*
   From GPP TS 23.401 version 11.11.0 Release 11, section 5.3.5 S1 release procedure, point 6:
   The MME deletes any eNodeB related information ("eNodeB Address in Use for S1-MME" and "eNB UE S1AP
   ID") from the UE's MME context, but, retains the rest of the UE's MME context including the S-GW's S1-U
   configuration information (address and TEIDs). All non-GBR EPS bearers established for the UE are preserved
   in the MME and in the Serving GW.
   If the cause of S1 release is because of User is inactivity, Inter-RAT Redirection, the MME shall preserve the
   GBR bearers. If the cause of S1 release is because of CS Fallback triggered, further details about bearer handling
   are described in TS 23.272 [58]. Otherwise, e.g. Radio Connection With UE Lost, S1 signalling connection lost,
   eNodeB failure the MME shall trigger the MME Initiated Dedicated Bearer Deactivation procedure
   (clause 5.4.4.2) for the GBR bearer(s) of the UE after the S1 Release procedure is completed.
*/
//------------------------------------------------------------------------------
void
mme_app_handle_s1ap_ue_context_release_complete (
  const itti_s1ap_ue_context_release_complete_t const
  *s1ap_ue_context_release_complete)
//------------------------------------------------------------------------------
{
  struct ue_context_s                    *ue_context_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id (&mme_app_desc.mme_ue_contexts, s1ap_ue_context_release_complete->mme_ue_s1ap_id);

  if (!ue_context_p) {
    MSC_LOG_EVENT (MSC_MMEAPP_MME, "0 S1AP_UE_CONTEXT_RELEASE_COMPLETE Unknown mme_ue_s1ap_id 0x%06" PRIX32 " ", s1ap_ue_context_release_complete->mme_ue_s1ap_id);
    OAILOG_ERROR (LOG_MME_APP, "UE context doesn't exist for enb_ue_s1ap_ue_id "ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
        s1ap_ue_context_release_complete->enb_ue_s1ap_id, s1ap_ue_context_release_complete->mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }

  mme_notify_ue_context_released (&mme_app_desc.mme_ue_contexts, ue_context_p);

  if (ue_context_p->mm_state == UE_UNREGISTERED) {
    if ((ue_context_p->mme_s11_teid == 0) && (ue_context_p->sgw_s11_teid == 0)) {
      // No Session
      OAILOG_DEBUG (LOG_MME_APP, "Deleting UE context associated in MME for mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n ",
                  s1ap_ue_context_release_complete->mme_ue_s1ap_id);
      mme_remove_ue_context(&mme_app_desc.mme_ue_contexts, ue_context_p);
      update_mme_app_stats_connected_ue_sub();
    } else { 
      // Send a DELETE_SESSION_REQUEST message to the SGW
      mme_app_send_delete_session_request (ue_context_p);
      // Move the UE to Idle state
      mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context_p,ECM_IDLE);
    }
  }  
  else 
  {
    // Update keys and ECM state 
    mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context_p,ECM_IDLE);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}

//-------------------------------------------------------------------------------------------------------
void mme_ue_context_update_ue_emm_state (
  mme_ue_s1ap_id_t       mme_ue_s1ap_id, mm_state_t  new_mm_state)
{
  // Function is used to update UE's mobility management State- Registered/Un-Registered 

  struct ue_context_s                    *ue_context_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id(&mme_app_desc.mme_ue_contexts, mme_ue_s1ap_id);
  if (ue_context_p == NULL) {
    OAILOG_CRITICAL (LOG_MME_APP, "**** Abnormal- UE context is null.****\n");
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  if ((ue_context_p->mm_state == UE_UNREGISTERED) && (new_mm_state == UE_REGISTERED))
  {
    ue_context_p->mm_state = new_mm_state;
    
    // Update Stats
    update_mme_app_stats_attached_ue_add();
  } else if ((ue_context_p->mm_state == UE_REGISTERED) && (new_mm_state == UE_UNREGISTERED))
  {
    ue_context_p->mm_state = new_mm_state;
    
    // Update Stats
    update_mme_app_stats_attached_ue_sub();
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}


//------------------------------------------------------------------------------
static void
_mme_app_handle_s1ap_ue_context_release (const mme_ue_s1ap_id_t mme_ue_s1ap_id,
                                         const enb_ue_s1ap_id_t enb_ue_s1ap_id,
                                         uint32_t  enb_id,           
                                         enum s1cause cause)
//------------------------------------------------------------------------------
{
  struct ue_context_s                    *ue_context_p = NULL;
  enb_s1ap_id_key_t                       enb_s1ap_id_key = INVALID_ENB_UE_S1AP_ID_KEY;
  MessageDef *message_p;

  OAILOG_FUNC_IN (LOG_MME_APP);
  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id(&mme_app_desc.mme_ue_contexts, mme_ue_s1ap_id);
  if (!ue_context_p) {
    /* 
     * Use enb_ue_s1ap_id_key to get the UE context - In case MME APP could not update S1AP with valid mme_ue_s1ap_id
     * before context release is triggered from s1ap.
     */
    MME_APP_ENB_S1AP_ID_KEY(enb_s1ap_id_key, enb_id, enb_ue_s1ap_id);
    ue_context_p = mme_ue_context_exists_enb_ue_s1ap_id(&mme_app_desc.mme_ue_contexts, enb_s1ap_id_key);

    OAILOG_WARNING (LOG_MME_APP, "Invalid mme_ue_s1ap_ue_id " 
      MME_UE_S1AP_ID_FMT " received from S1AP. Using enb_s1ap_id_key %ld to get the context \n", mme_ue_s1ap_id, enb_s1ap_id_key);
  }
  if (!ue_context_p) {
    OAILOG_ERROR (LOG_MME_APP, " UE Context Release Req: UE context doesn't exist for enb_ue_s1ap_ue_id "
        ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n", enb_ue_s1ap_id, mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  // Set the UE context release cause in UE context. This is used while constructing UE Context Release Command
  ue_context_p->ue_context_rel_cause = cause;

  if (ue_context_p->ecm_state == ECM_IDLE) {
    // This case could happen during sctp reset, before the UE could move to ECM_CONNECTED
    // calling below function to set the enb_s1ap_id_key to invalid
    if (ue_context_p->ue_context_rel_cause == S1AP_SCTP_SHUTDOWN_OR_RESET) {
      mme_ue_context_update_ue_sig_connection_state (&mme_app_desc.mme_ue_contexts, ue_context_p, ECM_IDLE);
      mme_app_itti_ue_context_release(ue_context_p, ue_context_p->ue_context_rel_cause);
      OAILOG_WARNING (LOG_MME_APP, "UE Conetext Release Reqeust:Cause SCTP RESET/SHUTDOWN. UE state: IDLE. mme_ue_s1ap_id = %d, enb_ue_s1ap_id = %d Action -- Handle the message\n ", 
                                  ue_context_p->mme_ue_s1ap_id, ue_context_p->enb_ue_s1ap_id);
    }
    OAILOG_ERROR (LOG_MME_APP, "ERROR: UE Context Release Request: UE state : IDLE. enb_ue_s1ap_ue_id " 
    ENB_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " Action--- Ignore the message\n", ue_context_p->enb_ue_s1ap_id, ue_context_p->mme_ue_s1ap_id);
    OAILOG_FUNC_OUT (LOG_MME_APP);
  }
  // Stop Initial context setup process guard timer,if running 
  if (ue_context_p->initial_context_setup_rsp_timer.id != MME_APP_TIMER_INACTIVE_ID) {
    if (timer_remove(ue_context_p->initial_context_setup_rsp_timer.id)) {
      OAILOG_ERROR (LOG_MME_APP, "Failed to stop Initial Context Setup Rsp timer for UE id  %d \n", ue_context_p->mme_ue_s1ap_id);
    } 
    ue_context_p->initial_context_setup_rsp_timer.id = MME_APP_TIMER_INACTIVE_ID;
    // Setting UE context release cause as Initial context setup failure
    ue_context_p->ue_context_rel_cause = S1AP_INITIAL_CONTEXT_SETUP_FAILED;
  }
  if (ue_context_p->mm_state == UE_UNREGISTERED) {
    // Initiate Implicit Detach for the UE
    message_p = itti_alloc_new_message (TASK_MME_APP, NAS_IMPLICIT_DETACH_UE_IND);
    DevAssert (message_p != NULL);
    message_p->ittiMsg.nas_implicit_detach_ue_ind.ue_id = ue_context_p->mme_ue_s1ap_id;
    itti_send_msg_to_task (TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
  } else {
    // release S1-U tunnel mapping in S_GW for all the active bearers for the UE
    mme_app_send_s11_release_access_bearers_req (ue_context_p);
  }
  OAILOG_FUNC_OUT (LOG_MME_APP);
}
