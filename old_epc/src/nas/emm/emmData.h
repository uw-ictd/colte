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
Source      emmData.h

Version     0.1

Date        2012/10/18

Product     NAS stack

Subsystem   EPS Mobility Management

Author      Frederic Maurel

Description Defines internal private data handled by EPS Mobility
        Management sublayer.

*****************************************************************************/
#ifndef FILE_EMMDATA_SEEN
#define FILE_EMMDATA_SEEN

#include "bstrlib.h"
#include "common_defs.h"
#include "obj_hashtable.h"
#include "hashtable.h"
#include "commonDef.h"
#include "networkDef.h"
#include "securityDef.h"

#include "nas_timer.h"

#include "esmData.h"

#include "emm_fsm.h"
#include "mme_api.h"
#include "3gpp_33.401.h"

#include "AdditionalUpdateType.h"
#include "UeNetworkCapability.h"
#include "MsNetworkCapability.h"
#include "DrxParameter.h"
#include "EpsBearerContextStatus.h"
#include "EpsNetworkFeatureSupport.h"


/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/



/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 * EPS NAS security context handled by EPS Mobility Management sublayer in
 * the UE and in the MME
 * --------------------------------------------------------------------------
 */
/* Type of security context */
typedef enum {
  SECURITY_CTX_TYPE_NOT_AVAILABLE = 0,
  SECURITY_CTX_TYPE_PARTIAL_NATIVE,
  SECURITY_CTX_TYPE_FULL_NATIVE,
  SECURITY_CTX_TYPE_MAPPED             // UNUSED
} emm_sc_type_t;

/* EPS NAS security context structure
 * EPS NAS security context: This context consists of K ASME with the associated key set identifier, the UE security
 * capabilities, and the uplink and downlink NAS COUNT values. In particular, separate pairs of NAS COUNT values are
 * used for each EPS NAS security contexts, respectively. The distinction between native and mapped EPS security
 * contexts also applies to EPS NAS security contexts. The EPS NAS security context is called 'full' if it additionally
 * contains the keys K NASint and K NASenc and the identifiers of the selected NAS integrity and encryption algorithms.*/
typedef struct emm_security_context_s {
  emm_sc_type_t sc_type;     /* Type of security context        */
                      /* state of security context is implicit due to its storage location (current/non-current)*/
#define EKSI_MAX_VALUE 6
  ksi_t eksi;           /* NAS key set identifier for E-UTRAN      */
#define EMM_SECURITY_VECTOR_INDEX_INVALID (-1)
  int vector_index;   /* Pointer on vector */
  uint8_t knas_enc[AUTH_KNAS_ENC_SIZE];/* NAS cyphering key               */
  uint8_t knas_int[AUTH_KNAS_INT_SIZE];/* NAS integrity key               */

  struct count_s{
    uint32_t spare:8;
    uint32_t overflow:16;
    uint32_t seq_num:8;
  } dl_count, ul_count;   /* Downlink and uplink count parameters    */
  struct {
    uint8_t eps_encryption;   /* algorithm used for ciphering            */
    uint8_t eps_integrity;    /* algorithm used for integrity protection */
    uint8_t umts_encryption;  /* algorithm used for ciphering            */
    uint8_t umts_integrity;   /* algorithm used for integrity protection */
    uint8_t gprs_encryption;  /* algorithm used for ciphering            */
    bool    umts_present:1;
    bool    gprs_present:1;
  } capability;       /* UE network capability           */
  struct {
    uint8_t encryption:4;   /* algorithm used for ciphering           */
    uint8_t integrity:4;    /* algorithm used for integrity protection */
  } selected_algorithms;       /* MME selected algorithms                */

  // Requirement MME24.301R10_4.4.4.3_2 (DETACH REQUEST (if sent before security has been activated);)
  uint8_t   activated;
} emm_security_context_t;


/*
 * --------------------------------------------------------------------------
 *  EMM internal data handled by EPS Mobility Management sublayer in the MME
 * --------------------------------------------------------------------------
 */
/*
 * Structure of the EMM context established by the network for a particular UE
 * ---------------------------------------------------------------------------
 */
typedef struct emm_data_context_s {
  mme_ue_s1ap_id_t ue_id;        /* UE identifier                                  */
  bool             is_dynamic;  /* Dynamically allocated context indicator         */
  bool             is_attached; /* Attachment indicator                            */
  bool             is_emergency;/* Emergency bearer services indicator             */
  bool             is_has_been_attached; /* Attachment indicator                   */

  /*
   * attach_type has type emm_proc_attach_type_t.
   *
   * Here, it is un-typedef'ed as uint8_t to avoid circular dependency issues.
   */
  uint8_t                    attach_type;  /* EPS/Combined/etc. */
  AdditionalUpdateType       additional_update_type;

  uint             num_attach_request;/* Num attach request received               */
  //bool             is_attach_accept_sent ;/* Do we have sent attach accept         */
  //bool             is_attach_reject_sent ;/* Do we have sent attach reject         */
  //bool             is_attach_complete_received ;/* Do we have received attach complete */

  // this bitmask is here because we wanted to avoid modifying the EmmcCommon interface
  uint32_t         common_proc_mask;  /* bitmask, see significance of bits below */
#define           EMM_CTXT_COMMON_PROC_GUTI                      ((uint32_t)1 << 0)
#define           EMM_CTXT_COMMON_PROC_AUTH                      ((uint32_t)1 << 1)
#define           EMM_CTXT_COMMON_PROC_SMC                       ((uint32_t)1 << 2)
#define           EMM_CTXT_COMMON_PROC_IDENT                     ((uint32_t)1 << 3)
#define           EMM_CTXT_COMMON_PROC_INFO                      ((uint32_t)1 << 4)

  uint32_t         specific_proc_mask;  /* bitmask, see significance of bits below */
#define           EMM_CTXT_SPEC_PROC_ATTACH                      ((uint32_t)1 << 0)
#define           EMM_CTXT_SPEC_PROC_ATTACH_ACCEPT_SENT          ((uint32_t)1 << 1)
#define           EMM_CTXT_SPEC_PROC_ATTACH_REJECT_SENT          ((uint32_t)1 << 2)
#define           EMM_CTXT_SPEC_PROC_TAU                         ((uint32_t)1 << 3)
#define           EMM_CTXT_SPEC_PROC_TAU_ACCEPT_SENT             ((uint32_t)1 << 4)
#define           EMM_CTXT_SPEC_PROC_TAU_REJECT_SENT             ((uint32_t)1 << 5)
#define           EMM_CTXT_SPEC_PROC_MME_INITIATED_DETACH_SENT   ((uint32_t)1 << 6)

  uint32_t         member_present_mask; /* bitmask, see significance of bits below */
  uint32_t         member_valid_mask;   /* bitmask, see significance of bits below */
#define           EMM_CTXT_MEMBER_IMSI                           ((uint32_t)1 << 0)
#define           EMM_CTXT_MEMBER_IMEI                           ((uint32_t)1 << 1)
#define           EMM_CTXT_MEMBER_IMEI_SV                        ((uint32_t)1 << 2)
#define           EMM_CTXT_MEMBER_OLD_GUTI                       ((uint32_t)1 << 3)
#define           EMM_CTXT_MEMBER_GUTI                           ((uint32_t)1 << 4)
#define           EMM_CTXT_MEMBER_TAI_LIST                       ((uint32_t)1 << 5)
#define           EMM_CTXT_MEMBER_LVR_TAI                        ((uint32_t)1 << 6)
#define           EMM_CTXT_MEMBER_AUTH_VECTORS                   ((uint32_t)1 << 7)
#define           EMM_CTXT_MEMBER_SECURITY                       ((uint32_t)1 << 8)
#define           EMM_CTXT_MEMBER_NON_CURRENT_SECURITY           ((uint32_t)1 << 9)
#define           EMM_CTXT_MEMBER_UE_NETWORK_CAPABILITY_IE       ((uint32_t)1 << 10)
#define           EMM_CTXT_MEMBER_MS_NETWORK_CAPABILITY_IE       ((uint32_t)1 << 11)
#define           EMM_CTXT_MEMBER_CURRENT_DRX_PARAMETER          ((uint32_t)1 << 12)
#define           EMM_CTXT_MEMBER_PENDING_DRX_PARAMETER          ((uint32_t)1 << 13)
#define           EMM_CTXT_MEMBER_EPS_BEARER_CONTEXT_STATUS      ((uint32_t)1 << 14)

#define           EMM_CTXT_MEMBER_AUTH_VECTOR0                   ((uint32_t)1 << 26)
//#define           EMM_CTXT_MEMBER_AUTH_VECTOR1                 ((uint32_t)1 << 27)  // reserved bit for AUTH VECTOR
//#define           EMM_CTXT_MEMBER_AUTH_VECTOR2                 ((uint32_t)1 << 28)  // reserved bit for AUTH VECTOR
//#define           EMM_CTXT_MEMBER_AUTH_VECTOR3                 ((uint32_t)1 << 29)  // reserved bit for AUTH VECTOR
//#define           EMM_CTXT_MEMBER_AUTH_VECTOR4                 ((uint32_t)1 << 30)  // reserved bit for AUTH VECTOR
//#define           EMM_CTXT_MEMBER_AUTH_VECTOR5                 ((uint32_t)1 << 31)  // reserved bit for AUTH VECTOR

#define           EMM_CTXT_MEMBER_SET_BIT( eMmCtXtMemBeRmAsK, bIt )   do { (eMmCtXtMemBeRmAsK) |= bIt;} while (0)
#define           EMM_CTXT_MEMBER_CLEAR_BIT( eMmCtXtMemBeRmAsK, bIt ) do { (eMmCtXtMemBeRmAsK) &= ~bIt;} while (0)

  imsi_t                   _imsi;        /* The IMSI provided by the UE or the MME, set valid when identification returns IMSI */
  imsi64_t                 _imsi64;      /* The IMSI provided by the UE or the MME, set valid when identification returns IMSI */
  imsi64_t                 saved_imsi64; /* Useful for 5.4.2.7.c */
  imei_t                   _imei;        /* The IMEI provided by the UE                     */
  imeisv_t                 _imeisv;      /* The IMEISV provided by the UE                   */
  //bool                   _guti_is_new; /* The GUTI assigned to the UE is new              */
  guti_t                   _guti;        /* The GUTI assigned to the UE                     */
  guti_t                   _old_guti;    /* The old GUTI (GUTI REALLOCATION)                */
  tai_list_t               _tai_list;    /* TACs the the UE is registered to                */
  tai_t                    _lvr_tai;
  tai_t                    originating_tai;
  bool                     is_guti_based_attach;

  ksi_t                    ue_ksi;       /* Security key set identifier provided by the UE  */
  int                      eea;          /* EPS encryption algorithms supported by the UE   */
  int                      eia;          /* EPS integrity algorithms supported by the UE    */
  int                      ucs2;         /* UCS2 Alphabet*/
  int                      uea;          /* UMTS encryption algorithms supported by the UE  */
  int                      uia;          /* UMTS integrity algorithms supported by the UE   */
  int                      gea;          /* GPRS encryption algorithms supported by the UE  */
  bool                     umts_present; /* For encoding ue network capabilities (variable size)*/
  bool                     gprs_present; /* For encoding ue network capabilities (variable size)*/

  int                      remaining_vectors;
  auth_vector_t            _vector[MAX_EPS_AUTH_VECTORS];/* EPS authentication vector                            */
  emm_security_context_t   _security;                /* Current EPS security context: The security context which has been activated most recently. Note that a current EPS
                                                        security context originating from either a mapped or native EPS security context may exist simultaneously with a native
                                                        non-current EPS security context.*/
#define EMM_AUTHENTICATION_SYNC_FAILURE_MAX   2
  int                      auth_sync_fail_count;     /* counter of successive AUTHENTICATION FAILURE messages from the UE with EMM cause #21 "synch failure" */

  // Requirement MME24.301R10_4.4.2.1_2
  emm_security_context_t   _non_current_security;    /* Non-current EPS security context: A native EPS security context that is not the current one. A non-current EPS
                                                        security context may be stored along with a current EPS security context in the UE and the MME. A non-current EPS
                                                        security context does not contain an EPS AS security context. A non-current EPS security context is either of type 'full
                                                        native' or of type 'partial native'.     */

  int                      emm_cause;    /* EMM failure cause code                          */

  emm_fsm_state_t          _emm_fsm_status;

  struct nas_timer_t       T3450; /* EMM message retransmission timer */
  struct nas_timer_t       T3460; /* Authentication timer         */
  struct nas_timer_t       T3470; /* Identification timer         */

  esm_data_context_t       esm_data_ctx;


  UeNetworkCapability      _ue_network_capability_ie; /* stored TAU Request IE Requirement MME24.301R10_5.5.3.2.4_2*/
  MsNetworkCapability      _ms_network_capability_ie; /* stored TAU Request IE Requirement MME24.301R10_5.5.3.2.4_2*/
  DrxParameter             _current_drx_parameter;            /* stored TAU Request IE Requirement MME24.301R10_5.5.3.2.4_4*/
  DrxParameter             _pending_drx_parameter;            /* stored TAU Request IE Requirement MME24.301R10_5.5.3.2.4_4*/
  EpsBearerContextStatus   _eps_bearer_context_status;/* stored TAU Request IE Requirement MME24.301R10_5.5.3.2.4_5*/
  EpsNetworkFeatureSupport _eps_network_feature_support;


  // TODO: DO BETTER  WITH BELOW
  bstring         esm_msg;      /* ESM message contained within the initial request*/
#  define EMM_CN_SAP_BUFFER_SIZE 4096
  char                             emm_cn_sap_buffer[EMM_CN_SAP_BUFFER_SIZE];


#define           IS_EMM_CTXT_PRESENT_IMSI( eMmCtXtPtR )                  (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_IMSI))
#define           IS_EMM_CTXT_PRESENT_IMEI( eMmCtXtPtR )                  (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_IMEI))
#define           IS_EMM_CTXT_PRESENT_IMEISV( eMmCtXtPtR )                (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_IMEI_SV))
#define           IS_EMM_CTXT_PRESENT_OLD_GUTI( eMmCtXtPtR )              (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_OLD_GUTI))
#define           IS_EMM_CTXT_PRESENT_GUTI( eMmCtXtPtR )                  (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_GUTI))
#define           IS_EMM_CTXT_PRESENT_TAI_LIST( eMmCtXtPtR )              (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_TAI_LIST))
#define           IS_EMM_CTXT_PRESENT_LVR_TAI( eMmCtXtPtR )               (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_LVR_TAI))
#define           IS_EMM_CTXT_PRESENT_AUTH_VECTORS( eMmCtXtPtR )          (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_AUTH_VECTORS))
#define           IS_EMM_CTXT_PRESENT_SECURITY( eMmCtXtPtR )              (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_SECURITY))
#define           IS_EMM_CTXT_PRESENT_NON_CURRENT_SECURITY( eMmCtXtPtR )  (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_NON_CURRENT_SECURITY))
#define           IS_EMM_CTXT_PRESENT_UE_NETWORK_CAPABILITY( eMmCtXtPtR ) (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_UE_NETWORK_CAPABILITY_IE))
#define           IS_EMM_CTXT_PRESENT_MS_NETWORK_CAPABILITY( eMmCtXtPtR ) (!!((eMmCtXtPtR)->member_present_mask & EMM_CTXT_MEMBER_MS_NETWORK_CAPABILITY_IE))

#define           IS_EMM_CTXT_PRESENT_AUTH_VECTOR( eMmCtXtPtR, KsI )      (!!((eMmCtXtPtR)->member_present_mask & ((EMM_CTXT_MEMBER_AUTH_VECTOR0) << KsI)))

#define           IS_EMM_CTXT_VALID_IMSI( eMmCtXtPtR )                    (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_IMSI))
#define           IS_EMM_CTXT_VALID_IMEI( eMmCtXtPtR )                    (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_IMEI))
#define           IS_EMM_CTXT_VALID_IMEISV( eMmCtXtPtR )                  (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_IMEI_SV))
#define           IS_EMM_CTXT_VALID_OLD_GUTI( eMmCtXtPtR )                (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_OLD_GUTI))
#define           IS_EMM_CTXT_VALID_GUTI( eMmCtXtPtR )                    (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_GUTI))
#define           IS_EMM_CTXT_VALID_TAI_LIST( eMmCtXtPtR )                (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_TAI_LIST))
#define           IS_EMM_CTXT_VALID_LVR_TAI( eMmCtXtPtR )                 (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_LVR_TAI))
#define           IS_EMM_CTXT_VALID_AUTH_VECTORS( eMmCtXtPtR )            (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_AUTH_VECTORS))
#define           IS_EMM_CTXT_VALID_SECURITY( eMmCtXtPtR )                (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_SECURITY))
#define           IS_EMM_CTXT_VALID_NON_CURRENT_SECURITY( eMmCtXtPtR )    (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_NON_CURRENT_SECURITY))
#define           IS_EMM_CTXT_VALID_UE_NETWORK_CAPABILITY( eMmCtXtPtR )   (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_UE_NETWORK_CAPABILITY_IE))
#define           IS_EMM_CTXT_VALID_MS_NETWORK_CAPABILITY( eMmCtXtPtR )   (!!((eMmCtXtPtR)->member_valid_mask & EMM_CTXT_MEMBER_MS_NETWORK_CAPABILITY_IE))

#define           IS_EMM_CTXT_VALID_AUTH_VECTOR( eMmCtXtPtR, KsI )        (!!((eMmCtXtPtR)->member_valid_mask & ((EMM_CTXT_MEMBER_AUTH_VECTOR0) << KsI)))

  void *          specific_proc_data;
} emm_data_context_t;



/*
 * Structure of the EMM data
 * -------------------------
 */
typedef struct emm_data_s {
  /*
   * MME configuration
   * -----------------
   */
  mme_api_emm_config_t conf;
  /*
   * EMM contexts
   * ------------
   */
  hash_table_ts_t    *ctx_coll_ue_id; // key is emm ue id, data is struct emm_data_context_s
  hash_table_ts_t    *ctx_coll_imsi;  // key is imsi_t, data is emm ue id (unsigned int)
  obj_hash_table_t   *ctx_coll_guti;  // key is guti, data is emm ue id (unsigned int)
} emm_data_t;

mme_ue_s1ap_id_t emm_ctx_get_new_ue_id(emm_data_context_t *ctxt) __attribute__((nonnull));

void emm_ctx_mark_common_procedure_running(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_unmark_common_procedure_running(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));
bool emm_ctx_is_common_procedure_running(emm_data_context_t * const ctxt, const int proc_id) __attribute__ ((nonnull)) __attribute__ ((flatten));


void emm_ctx_mark_specific_procedure(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_unmark_specific_procedure(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));
bool emm_ctx_is_specific_procedure(emm_data_context_t * const ctxt, const int proc_id) __attribute__ ((nonnull)) __attribute__ ((flatten));


void emm_ctx_set_attribute_present(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_clear_attribute_present(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_set_attribute_valid(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_clear_attribute_valid(emm_data_context_t * const ctxt, const int attribute_bit_pos) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_guti(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_guti(emm_data_context_t * const ctxt, guti_t *guti) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_valid_guti(emm_data_context_t * const ctxt, guti_t *guti) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_old_guti(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_old_guti(emm_data_context_t * const ctxt, guti_t *guti) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_valid_old_guti(emm_data_context_t * const ctxt, guti_t *guti) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_imsi(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_imsi(emm_data_context_t * const ctxt, imsi_t *imsi, const imsi64_t imsi64) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_valid_imsi(emm_data_context_t * const ctxt, imsi_t *imsi, const imsi64_t imsi64) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_imei(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_imei(emm_data_context_t * const ctxt, imei_t *imei) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_valid_imei(emm_data_context_t * const ctxt, imei_t *imei) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_imeisv(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_imeisv(emm_data_context_t * const ctxt, imeisv_t *imeisv) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_valid_imeisv(emm_data_context_t * const ctxt, imeisv_t *imeisv) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_lvr_tai(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_lvr_tai(emm_data_context_t * const ctxt, tai_t *lvr_tai) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_valid_lvr_tai(emm_data_context_t * const ctxt, tai_t *lvr_tai) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_auth_vectors(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_clear_auth_vector(emm_data_context_t * const ctxt, ksi_t eksi) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_clear_security(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_security_type(emm_data_context_t * const ctxt, emm_sc_type_t sc_type) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_security_eksi(emm_data_context_t * const ctxt, ksi_t eksi) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_clear_security_vector_index(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_set_security_vector_index(emm_data_context_t * const ctxt, int vector_index) __attribute__ ((nonnull)) __attribute__ ((flatten));

void emm_ctx_clear_non_current_security(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) __attribute__ ((flatten));
void emm_ctx_clear_non_current_security_vector_index(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) ;
void emm_ctx_set_non_current_security_vector_index(emm_data_context_t * const ctxt, int vector_index)__attribute__ ((nonnull)) ;

void emm_ctx_clear_ue_nw_cap_ie(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) ;
void emm_ctx_set_ue_nw_cap_ie(emm_data_context_t * const ctxt, UeNetworkCapability *ue_nw_cap_ie) __attribute__ ((nonnull)) ;
void emm_ctx_set_valid_ue_nw_cap_ie(emm_data_context_t * const ctxt, UeNetworkCapability *ue_nw_cap_ie) __attribute__ ((nonnull)) ;

void emm_ctx_clear_ms_nw_cap(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) ;

void emm_ctx_clear_current_drx_parameter(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) ;
void emm_ctx_set_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx) __attribute__ ((nonnull)) ;
void emm_ctx_set_valid_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx) __attribute__ ((nonnull)) ;

void emm_ctx_clear_pending_current_drx_parameter(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) ;
void emm_ctx_set_pending_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx) __attribute__ ((nonnull)) ;
void emm_ctx_set_valid_pending_current_drx_parameter(emm_data_context_t * const ctxt, DrxParameter *drx) __attribute__ ((nonnull)) ;

void emm_ctx_clear_eps_bearer_context_status(emm_data_context_t * const ctxt) __attribute__ ((nonnull)) ;
void emm_ctx_set_eps_bearer_context_status(emm_data_context_t * const ctxt, EpsBearerContextStatus *status) __attribute__ ((nonnull)) ;
void emm_ctx_set_valid_eps_bearer_context_status(emm_data_context_t * const ctxt, EpsBearerContextStatus *status) __attribute__ ((nonnull)) ;



struct emm_data_context_s *emm_data_context_get(
  emm_data_t *emm_data, const mme_ue_s1ap_id_t ueid) __attribute__ ((nonnull)) ;

struct emm_data_context_s *emm_data_context_get_by_imsi (
  emm_data_t * emm_data, imsi64_t imsi64) __attribute__ ((nonnull)) ;

struct emm_data_context_s *emm_data_context_get_by_guti(
  emm_data_t *emm_data, guti_t *guti) __attribute__ ((nonnull)) ;

struct emm_data_context_s *emm_data_context_remove(
  emm_data_t *_emm_data, struct emm_data_context_s *elm) __attribute__ ((nonnull)) ;

void emm_data_context_remove_mobile_ids(
  emm_data_t *_emm_data, struct emm_data_context_s *elm) __attribute__ ((nonnull)) ;

int  emm_data_context_add(emm_data_t *emm_data, struct emm_data_context_s *elm) __attribute__ ((nonnull)) ;
int  emm_data_context_add_guti (emm_data_t * emm_data, struct emm_data_context_s *elm) __attribute__ ((nonnull)) ;
int  emm_data_context_add_old_guti (emm_data_t * emm_data, struct emm_data_context_s *elm) __attribute__ ((nonnull)) ;
int  emm_data_context_add_imsi (emm_data_t * emm_data, struct emm_data_context_s *elm) __attribute__ (
(nonnull)) ;
int emm_data_context_upsert_imsi (emm_data_t * emm_data, struct emm_data_context_s *elm) __attribute__((nonnull));

void emm_data_context_silently_reset_procedures (struct emm_data_context_s *emm_ctx) __attribute__ ((nonnull)) ;
void emm_data_context_stop_all_timers (struct emm_data_context_s *emm_ctx) __attribute__ ((nonnull)) ;
void free_emm_data_context(struct emm_data_context_s * const emm_ctx) __attribute__ ((nonnull)) ;
void emm_data_context_dump(const struct emm_data_context_s * const elm_pP) __attribute__ ((nonnull)) ;

void emm_data_context_dump_all(void);


/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 *      EPS mobility management data (used within EMM only)
 * --------------------------------------------------------------------------
 */
emm_data_t _emm_data;



/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif /* FILE_EMMDATA_SEEN*/
