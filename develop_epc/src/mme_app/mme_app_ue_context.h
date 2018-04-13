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



/*! \file mme_app_ue_context.h
 *  \brief MME applicative layer
 *  \author Sebastien ROUX, Lionel Gauthier
 *  \date 2013
 *  \version 1.0
 *  \email: lionel.gauthier@eurecom.fr
 *  @defgroup _mme_app_impl_ MME applicative layer
 *  @ingroup _ref_implementation_
 *  @{
 */


#ifndef FILE_MME_APP_UE_CONTEXT_SEEN
#define FILE_MME_APP_UE_CONTEXT_SEEN
#include <stdint.h>
#include <inttypes.h>   /* For sscanf formats */
#include <time.h>       /* to provide time_t */

#include "tree.h"
#include "queue.h"
#include "hashtable.h"
#include "obj_hashtable.h"
#include "bstrlib.h"
#include "common_types.h"
#include "s1ap_messages_types.h"
#include "nas_messages_types.h"
#include "s6a_messages_types.h"
#include "security_types.h"
#include "sgw_ie_defs.h"
#include "emm_data.h"
#include "esm_data.h"



typedef enum {
  ECM_IDLE = 0,
  ECM_CONNECTED,
} ecm_state_t;

#define IMSI_DIGITS_MAX 15

typedef struct {
  uint32_t length;
  char data[IMSI_DIGITS_MAX + 1];
} mme_app_imsi_t;

// TODO: (amar) only used in testing
#define IMSI_FORMAT "s"
#define IMSI_DATA(MME_APP_IMSI) (MME_APP_IMSI.data)

/* Convert the IMSI contained by a char string NULL terminated to uint64_t */

bool mme_app_is_imsi_empty(mme_app_imsi_t const * imsi);
bool mme_app_imsi_compare(mme_app_imsi_t const * imsi_a, mme_app_imsi_t const * imsi_b);
void mme_app_copy_imsi(mme_app_imsi_t * imsi_dst, mme_app_imsi_t const * imsi_src);

void mme_app_string_to_imsi(mme_app_imsi_t * const imsi_dst, char const * const imsi_string_src);
void mme_app_imsi_to_string(char * const imsi_dst, mme_app_imsi_t const * const imsi_src);

uint64_t mme_app_imsi_to_u64 (mme_app_imsi_t imsi_src);
void mme_app_ue_context_uint_to_imsi(uint64_t imsi_src, mme_app_imsi_t *imsi_dst);
void mme_app_convert_imsi_to_imsi_mme (mme_app_imsi_t * imsi_dst, const imsi_t *imsi_src);
mme_ue_s1ap_id_t mme_app_ctx_get_new_ue_id(void);
/*
 * Timer identifier returned when in inactive state (timer is stopped or has
 * failed to be started)
 */
#define MME_APP_TIMER_INACTIVE_ID   (-1)
#define MME_APP_DELTA_T3412_REACHABILITY_TIMER 4 // in minutes 
#define MME_APP_DELTA_REACHABILITY_IMPLICIT_DETACH_TIMER 0 // in minutes 

#define BEARER_STATE_NULL        0
#define BEARER_STATE_SGW_CREATED (1 << 0)
#define BEARER_STATE_MME_CREATED (1 << 1)
#define BEARER_STATE_ENB_CREATED (1 << 2)
#define BEARER_STATE_ACTIVE      (1 << 3)
#define BEARER_STATE_S1_RELEASED (1 << 4)
// EURECOM LG TODO REMOVE OR CLEAN
typedef uint8_t mme_app_bearer_state_t;

#define MME_APP_INITIAL_CONTEXT_SETUP_RSP_TIMER_VALUE 2 // In seconds

/* Timer structure */
struct mme_app_timer_t {
  long id;         /* The timer identifier                 */
  long sec;       /* The timer interval value in seconds  */
};

/** @struct bearer_context_t
 *  @brief Parameters that should be kept for an eps bearer.
 */
typedef struct bearer_context_s {
  // EPS Bearer ID: An EPS bearer identity uniquely identifies an EP S bearer for one UE accessing via E-UTRAN
  ebi_t                       ebi;

  // TI Transaction Identifier
  proc_tid_t                  transaction_identifier;

  // S-GW IP address for S1-u: IP address of the S-GW for the S1-u interfaces.
  // S-GW TEID for S1u: Tunnel Endpoint Identifier of the S-GW for the S1-u interface.
  fteid_t                      s_gw_fteid_s1u;            // set by S11 CREATE_SESSION_RESPONSE

  // PDN GW TEID for S5/S8 (user plane): P-GW Tunnel Endpoint Identifier for the S5/S8 interface for the user plane. (Used for S-GW change only).
  // NOTE:
  // The PDN GW TEID is needed in MME context as S-GW relocation is triggered without interaction with the source S-GW, e.g. when a TAU
  // occurs. The Target S-GW requires this Information Element, so it must be stored by the MME.
  // PDN GW IP address for S5/S8 (user plane): P GW IP address for user plane for the S5/S8 interface for the user plane. (Used for S-GW change only).
  // NOTE:
  // The PDN GW IP address for user plane is needed in MME context as S-GW relocation is triggered without interaction with the source S-GW,
  // e.g. when a TAU occurs. The Target S GW requires this Information Element, so it must be stored by the MME.
  fteid_t                      p_gw_fteid_s5_s8_up;

  // EPS bearer QoS: QCI and ARP, optionally: GBR and MBR for GBR bearer
  qci_t                       qci;

  // TFT: Traffic Flow Template. (For PMIP-based S5/S8 only)
  //traffic_flow_template_t          *tft_pmip;

  // extra 23.401 spec members
  pdn_cid_t                         pdn_cx_id;
  mme_app_bearer_state_t            bearer_state;
  esm_ebr_context_t                 esm_ebr_context;
  fteid_t                           enb_fteid_s1u;

  /* QoS for this bearer */
  priority_level_t            priority_level;
  pre_emption_vulnerability_t preemption_vulnerability;
  pre_emption_capability_t    preemption_capability;
} bearer_context_t;



/** @struct subscribed_apn_t
 *  @brief Parameters that should be kept for a subscribed apn by the UE.
 */
// For each active PDN connection:
typedef struct pdn_context_s {
  context_identifier_t context_identifier; // key

  //APN in Use: The APN currently used. This APN shall be composed of the APN Network
  //            Identifier and the default APN Operator Identifier, as specified in TS 23.003 [9],
  //            clause 9.1.2 (EURECOM: "mnc<MNC>.mcc<MCC>.gprs"). Any received value in the APN OI Replacement field is not applied
  //            here.
  bstring                     apn_in_use;        // an ID for P-GW through which a user can access the Subscribed APN

  // APN Restriction: Denotes the restriction on the combination of types of APN for the APN associated
  //                  with this EPS bearer Context.

  // APN Subscribed: The subscribed APN received from the HSS.
  bstring                     apn_subscribed;

  // PDN Type: IPv4, IPv6 or IPv4v6
  pdn_type_t                  pdn_type;

  // IP Address(es): IPv4 address and/or IPv6 prefix
  //                 NOTE:
  //                 The MME might not have information on the allocated IPv4 address.
  //                 Alternatively, following mobility involving a pre-release 8 SGSN, this
  //                 IPv4 address might not be the one allocated to the UE.
  paa_t              paa;                         // set by S11 CREATE_SESSION_RESPONSE



  // EPS PDN Charging Characteristics: The charging characteristics of this PDN connection, e.g. normal, prepaid, flat-rate
  // and/or hot billing.

  // APN-OI Replacement: APN level APN-OI Replacement which has same role as UE level APN-OI
  // Replacement but with higher priority than UE level APN-OI Replacement. This is
  // an optional parameter. When available, it shall be used to construct the PDN GW
  // FQDN instead of UE level APN-OI Replacement.
  bstring         apn_oi_replacement;

  // SIPTO permissions: Indicates whether the traffic associated with this APN is allowed or prohibited for SIPTO
  // LIPA permissions: Indicates whether the PDN can be accessed via Local IP Access. Possible values
  //                   are: LIPA-prohibited, LIPA-only and LIPA-conditional.

  // VPLMN Address Allowed: Specifies whether the UE is allowed to use the APN in the domain of the HPLMN
  //                        only, or additionally the APN in the domain of the VPLMN.

  // PDN GW Address in Use(control plane): The IP address of the PDN GW currently used for sending control plane signalling.
  ip_address_t                p_gw_address_s5_s8_cp;

  // PDN GW TEID for S5/S8 (control plane): PDN GW Tunnel Endpoint Identifier for the S5/S8 interface for the control plane.
  //                                        (For GTP-based S5/S8 only).
  teid_t                      p_gw_teid_s5_s8_cp;

  // MS Info Change Reporting Action: Need to communicate change in User Location Information to the PDN GW with this EPS bearer Context.

  // CSG Information Reporting Action: Need to communicate change in User CSG Information to the PDN GW with this
  //                                   EPS bearer Context.
  //                                   This field denotes separately whether the MME/SGSN are requested to send
  //                                   changes in User CSG Information for (a) CSG cells, (b) hybrid cells in which the
  //                                   subscriber is a CSG member and (c) hybrid cells in which the subscriber is not a
  //                                   CSG member.

  // EPS subscribed QoS profile: The bearer level QoS parameter values for that APN's default bearer (QCI and
  // ARP) (see clause 4.7.3).
  eps_subscribed_qos_profile_t  default_bearer_eps_subscribed_qos_profile;

  // Subscribed APN-AMBR: The Maximum Aggregated uplink and downlink MBR values to be shared across
  //                      all Non-GBR bearers, which are established for this APN, according to the
  //                      subscription of the user.
  ambr_t                       subscribed_apn_ambr;

  // APN-AMBR: The Maximum Aggregated uplink and downlink MBR values to be shared across
  // all Non-GBR bearers, which are established for this APN, as decided by the PDN GW.
  ambr_t                       p_gw_apn_ambr;

  // PDN GW GRE Key for uplink traffic (user plane): PDN GW assigned GRE Key for the S5/S8 interface for the user plane for uplink traffic. (For PMIP-based S5/S8 only)

  // Default bearer: Identifies the EPS Bearer Id of the default bearer within the given PDN connection.
  ebi_t                       default_ebi;

  int                         bearer_contexts[BEARERS_PER_UE]; // contains bearer indexes in ue_mm_context_t.bearer_contexts[], or -1

  //apn_configuration_t         apn_configuration; // set by S6A UPDATE LOCATION ANSWER
  //bstring                     pgw_id;            // an ID for P-GW through which a user can access the Subscribed APN

  /* S-GW IP address for User-Plane */
  ip_address_t                s_gw_address_s11_s4;
  teid_t                      s_gw_teid_s11_s4;            // set by S11 CREATE_SESSION_RESPONSE

  esm_pdn_t                   esm_data;
  bool                        is_active;
  protocol_configuration_options_t *pco; // temp storage of information waiting for activation of required procedure

} pdn_context_t;


/** @struct ue_mm_context_t
 *  @brief Useful parameters to know in MME application layer. They are set
 * according to 3GPP TS.23.401 #5.7.2
 */
typedef struct ue_mm_context_s {
  pthread_mutex_t recmutex;  // mutex on the ue_mm_context_t + emm_context_s + esm_context_t

  /* Basic identifier for ue. IMSI is encoded on maximum of 15 digits of 4 bits,
   * so usage of an unsigned integer on 64 bits is necessary.
   */
#define IMSI_UNAUTHENTICATED  (0x0)
#define IMSI_AUTHENTICATED    (0x1)
  /* Indicator to show the IMSI authentication state */
  unsigned               imsi_auth:1;                 // set by nas_auth_resp_t

  bstring                msisdn;                    // The basic MSISDN of the UE. The presence is dictated by its storage in the HSS.
                                                    // set by S6A UPDATE LOCATION ANSWER

  enum s1cause           ue_context_rel_cause;

  // Globally Unique Temporary Identity can be found in emm_nas_context
  //bool                   is_guti_set;                 // is GUTI has been set
  //guti_t                 guti;                        // Globally Unique Temporary Identity. guti.gummei.plmn set by nas_auth_param_req_t

  mm_state_t             mm_state;
  ecm_state_t            ecm_state;

  // read by S6A UPDATE LOCATION REQUEST
  // was me_identity_t // Mobile Equipment Identity – (e.g. IMEI/IMEISV) Software Version Number not set/read except read by display utility
  //imei_t                   _imei;        /* The IMEI provided by the UE     can be found in emm_nas_context                */
  //imeisv_t                 _imeisv;      /* The IMEISV provided by the UE   can be found in emm_nas_context                */


  tai_list_t             tail_list; // Current Tracking area list

  tai_t                  tai_last_tau ; // TAI of the TA in which the last Tracking Area Update was initiated.

  /* Last known cell identity */
  ecgi_t                  e_utran_cgi;                 // Last known E-UTRAN cell, set by nas_attach_req_t
  // read for S11 CREATE_SESSION_REQUEST
  /* Time when the cell identity was acquired */
  time_t                 cell_age;                    // Time elapsed since the last E-UTRAN Cell Global Identity was acquired. set by nas_auth_param_req_t

  /* TODO: add csg_id */
  /* TODO: add csg_membership */
  /* TODO Access mode: Access mode of last known ECGI when the UE was active */

  // Authentication Vector Temporary authentication and key agreement data that enables an MME to
  // engage in AKA with a particular user. An EPS Authentication Vector consists of four elements:
  // a) network challenge RAND,
  // b) an expected response XRES,
  // c) Key K ASME ,
  // d) a network authentication token AUTN.

  network_access_mode_t  access_mode;                  // set by S6A UPDATE LOCATION ANSWER

  /* TODO: add ue radio cap, ms classmarks, supported codecs */

  // UE Network Capability  // UE network capabilities including security algorithms and key derivation functions supported by the UE

  // MS Network Capability  // For a GERAN and/or UTRAN capable UE, this contains information needed by the SGSN.

  /* TODO: add DRX parameter */
  // UE Specific DRX Parameters   // UE specific DRX parameters for A/Gb mode, Iu mode and S1-mode

  // Selected NAS Algorithm       // Selected NAS security algorithm
  // eKSI                         // Key Set Identifier for the main key K ASME . Also indicates whether the UE is using
                                  // security keys derived from UTRAN or E-UTRAN security association.

  apn_config_profile_t   apn_profile;                  // set by S6A UPDATE LOCATION ANSWER
  subscriber_status_t    sub_status;                   // set by S6A UPDATE LOCATION ANSWER
  ambr_t                 subscribed_ambr;              // set by S6A UPDATE LOCATION ANSWER

  // K ASME                       // Main key for E-UTRAN key hierarchy based on CK, IK and Serving network identity

  // NAS Keys and COUNT           // K NASint , K_ NASenc , and NAS COUNT parameter.

  // Selected CN operator id      // Selected core network operator identity (to support network sharing as defined in TS 23.251 [24]).

  // Recovery                     // Indicates if the HSS is performing database recovery.

  ard_t                  access_restriction_data;      // The access restriction subscription information. set by S6A UPDATE LOCATION ANSWER

  // ODB for PS parameters        // Indicates that the status of the operator determined barring for packet oriented services.

  // APN-OI Replacement           // Indicates the domain name to replace the APN-OI when constructing the PDN GW
                                  // FQDN upon which to perform a DNS resolution. This replacement applies for all
                                  // the APNs in the subscriber's profile. See TS 23.003 [9] clause 9.1.2 for more
                                  // information on the format of domain names that are allowed in this field.
  bstring      apn_oi_replacement; // example: "province1.mnc012.mcc345.gprs"

  // MME IP address for S11       // MME IP address for the S11 interface (used by S-GW)
  // LOCATED IN mme_config_t.ipv4.s11

  // MME TEID for S11             // MME Tunnel Endpoint Identifier for S11 interface.
  // LOCATED IN THIS.subscribed_apns[MAX_APN_PER_UE].mme_teid_s11
  teid_t                      mme_teid_s11;                // set by mme_app_send_s11_create_session_req

  // S-GW IP address for S11/S4   // S-GW IP address for the S11 and S4 interfaces
  // LOCATED IN THIS.subscribed_apns[MAX_APN_PER_UE].s_gw_address_s11_s4

  // S-GW TEID for S11/S4         // S-GW Tunnel Endpoint Identifier for the S11 and S4 interfaces.
  // LOCATED IN THIS.subscribed_apns[MAX_APN_PER_UE].s_gw_teid_s11_s4

  // SGSN IP address for S3       // SGSN IP address for the S3 interface (used if ISR is activated for the GERAN and/or UTRAN capable UE)

  // SGSN TEID for S3             // SGSN Tunnel Endpoint Identifier for S3 interface (used if ISR is activated for the E-UTRAN capable UE)

  // eNodeB Address in Use for S1-MME // The IP address of the eNodeB currently used for S1-MME.
  // implicit with use of SCTP through the use of sctp_assoc_id_key
  sctp_assoc_id_t        sctp_assoc_id_key; // link with eNB id

  // eNB UE S1AP ID,  Unique identity of the UE within eNodeB.
  enb_ue_s1ap_id_t       enb_ue_s1ap_id:24;
  enb_s1ap_id_key_t      enb_s1ap_id_key; // key uniq among all connected eNBs

  // MME UE S1AP ID, Unique identity of the UE within MME.
  mme_ue_s1ap_id_t       mme_ue_s1ap_id;


  // Subscribed UE-AMBR: The Maximum Aggregated uplink and downlink MBR values to be shared across all Non-GBR bearers according to the subscription of the user.
  ambr_t                 suscribed_ue_ambr;
  // UE-AMBR: The currently used Maximum Aggregated uplink and downlink MBR values to be shared across all Non-GBR bearers.
  ambr_t                 used_ue_ambr;
  // EPS Subscribed Charging Characteristics: The charging characteristics for the MS e.g. normal, prepaid, flat rate and/or hot billing.
  // Subscribed RFSP Index: An index to specific RRM configuration in the E-UTRAN that is received from the HSS.
  // RFSP Index in Use: An index to specific RRM configuration in the E-UTRAN that is currently in use.
  // Trace reference: Identifies a record or a collection of records for a particular trace.
  // Trace type: Indicates the type of trace
  // Trigger id: Identifies the entity that initiated the trace
  // OMC identity: Identifies the OMC that shall receive the trace record(s).
  // URRP-MME: URRP-MME indicating that the HSS has requested the MME to notify the HSS regarding UE reachability at the MME.
  // CSG Subscription Data: The CSG Subscription Data is a list of CSG IDs for the visiting PLMN and for each
  //   CSG ID optionally an associated expiration date which indicates the point in time when the subscription to the CSG ID
  //   expires; an absent expiration date indicates unlimited subscription. For a CSG ID that can be used to access specific PDNs via Local IP Access, the
  //   CSG ID entry includes the corresponding APN(s).
  // LIPA Allowed: Specifies whether the UE is allowed to use LIPA in this PLMN.

  // Subscribed Periodic RAU/TAU Timer: Indicates a subscribed Periodic RAU/TAU Timer value.
  rau_tau_timer_t        rau_tau_timer;               // set by S6A UPDATE LOCATION ANSWER

  // MPS CS priority: Indicates that the UE is subscribed to the eMLPP or 1x RTT priority service in the CS domain.

  // MPS EPS priority: Indicates that the UE is subscribed to MPS in the EPS domain.

  // For each active PDN connection:
  pdn_context_t         *pdn_contexts[MAX_APN_PER_UE]; // index is of type pdn_cid_t
  int                    nb_active_pdn_contexts;




  // Not in spec members
  emm_context_t          emm_context;
  bearer_context_t      *bearer_contexts[BEARERS_PER_UE];

  apn_config_profile_t   apn_config_profile;                  // set by S6A UPDATE LOCATION ANSWER
  /* Store the radio capabilities as received in S1AP UE capability indication
   * message.
   */
  bstring                 ue_radio_capability;



  
  // Mobile Reachability Timer-Start when UE moves to idle state. Stop when UE moves to connected state
  struct mme_app_timer_t       mobile_reachability_timer; 
  // Implicit Detach Timer-Start at the expiry of Mobile Reachability timer. Stop when UE moves to connected state
  struct mme_app_timer_t       implicit_detach_timer; 
  // Initial Context Setup Procedure Guard timer 
  struct mme_app_timer_t       initial_context_setup_rsp_timer; 

#define SUBSCRIPTION_UNKNOWN    false
#define SUBSCRIPTION_KNOWN      true
  bool                   subscription_known;        // set by S6A UPDATE LOCATION ANSWER
  ambr_t                 used_ambr;
  subscriber_status_t    subscriber_status;        // set by S6A UPDATE LOCATION ANSWER
  network_access_mode_t  network_access_mode;       // set by S6A UPDATE LOCATION ANSWER
  LIST_HEAD(s11_procedures_s, mme_app_s11_proc_s) *s11_procedures;
} ue_mm_context_t;



typedef struct mme_ue_context_s {
  uint32_t               nb_ue_managed;
  uint32_t               nb_ue_idle;

  uint32_t               nb_bearers_managed;

  uint32_t               nb_ue_since_last_stat;
  uint32_t               nb_bearers_since_last_stat;

  hash_table_uint64_ts_t  *imsi_ue_context_htbl; // data is mme_ue_s1ap_id_t
  hash_table_uint64_ts_t  *tun11_ue_context_htbl;// data is mme_ue_s1ap_id_t
  hash_table_ts_t         *mme_ue_s1ap_id_ue_context_htbl;
  hash_table_uint64_ts_t  *enb_ue_s1ap_id_ue_context_htbl;
  obj_hash_table_uint64_t *guti_ue_context_htbl;// data is mme_ue_s1ap_id_t
} mme_ue_context_t;


/** \brief Retrieve an UE context by selecting the provided IMSI
 * \param imsi Imsi to find in UE map
 * @returns an UE context matching the IMSI or NULL if the context doesn't exists
 **/
ue_mm_context_t *mme_ue_context_exists_imsi(mme_ue_context_t * const mme_ue_context,
    const imsi64_t imsi);

/** \brief Retrieve an UE context by selecting the provided S11 teid
 * \param teid The tunnel endpoint identifier used between MME and S-GW
 * @returns an UE context matching the teid or NULL if the context doesn't exists
 **/
ue_mm_context_t *mme_ue_context_exists_s11_teid(mme_ue_context_t * const mme_ue_context,
    const s11_teid_t teid);

/** \brief Retrieve an UE context by selecting the provided mme_ue_s1ap_id
 * \param mme_ue_s1ap_id The UE id identifier used in S1AP MME (and NAS)
 * @returns an UE context matching the mme_ue_s1ap_id or NULL if the context doesn't exists
 **/
ue_mm_context_t *mme_ue_context_exists_mme_ue_s1ap_id(mme_ue_context_t * const mme_ue_context,
    const mme_ue_s1ap_id_t mme_ue_s1ap_id);

/** \brief Retrieve an UE context by selecting the provided enb_ue_s1ap_id
 * \param enb_ue_s1ap_id The UE id identifier used in S1AP MME
 * @returns an UE context matching the enb_ue_s1ap_id or NULL if the context doesn't exists
 **/
ue_mm_context_t *mme_ue_context_exists_enb_ue_s1ap_id (
  mme_ue_context_t * const mme_ue_context_p,
  const enb_s1ap_id_key_t enb_key);

/** \brief Retrieve an UE context by selecting the provided guti
 * \param guti The GUTI used by the UE
 * @returns an UE context matching the guti or NULL if the context doesn't exists
 **/
ue_mm_context_t *mme_ue_context_exists_guti(mme_ue_context_t * const mme_ue_context,
    const guti_t * const guti);

/** \brief Move the content of a context to another context
 * \param dst            The destination context
 * \param src            The source context
 **/
void mme_app_move_context (ue_mm_context_t *dst, ue_mm_context_t *src);

/** \brief Notify the MME_APP that a duplicated ue_context_t exist (both share the same mme_ue_s1ap_id)
 * \param enb_key        The UE id identifier used in S1AP and MME_APP (agregated with a enb_id)
 * \param mme_ue_s1ap_id The UE id identifier used in MME_APP and NAS
 * \param is_remove_old  Remove old UE context or new UE context ?
 **/
void
mme_ue_context_duplicate_enb_ue_s1ap_id_detected (
  const enb_s1ap_id_key_t enb_key,
  const mme_ue_s1ap_id_t  mme_ue_s1ap_id,
  const bool              is_remove_old);

/** \brief Create the association between mme_ue_s1ap_id and an UE context (enb_ue_s1ap_id key)
 * \param enb_key        The UE id identifier used in S1AP and MME_APP (agregated with a enb_id)
 * \param mme_ue_s1ap_id The UE id identifier used in MME_APP and NAS
 * @returns RETURNerror or RETURNok
 **/
int
mme_ue_context_notified_new_ue_s1ap_id_association (
    const enb_s1ap_id_key_t  enb_key,
    const mme_ue_s1ap_id_t   mme_ue_s1ap_id);

/** \brief Update an UE context by selecting the provided guti
 * \param mme_ue_context_p The MME context
 * \param ue_context_p The UE context
 * \param enb_s1ap_id_key The eNB UE id identifier
 * \param mme_ue_s1ap_id The UE id identifier used in S1AP MME (and NAS)
 * \param imsi
 * \param mme_s11_teid The tunnel endpoint identifier used between MME and S-GW
 * \param nas_ue_id The UE id identifier used in S1AP MME and NAS
 * \param guti_p The GUTI used by the UE
 **/
void mme_ue_context_update_coll_keys(
    mme_ue_context_t * const mme_ue_context_p,
    ue_mm_context_t     * const ue_context_p,
    const enb_s1ap_id_key_t  enb_s1ap_id_key,
    const mme_ue_s1ap_id_t   mme_ue_s1ap_id,
    const imsi64_t     imsi,
    const s11_teid_t         mme_s11_teid,
    const guti_t     * const guti_p);

/** \brief dump MME associative collections
 **/

void mme_ue_context_dump_coll_keys(void);

/** \brief Insert a new UE context in the tree of known UEs.
 * At least the IMSI should be known to insert the context in the tree.
 * \param ue_context_p The UE context to insert
 * @returns 0 in case of success, -1 otherwise
 **/
int mme_insert_ue_context(mme_ue_context_t * const mme_ue_context,
                         const struct ue_mm_context_s * const ue_context_p);


/** \brief TODO WORK HERE Remove UE context unnecessary information.
 *  mark it as released. It is necessary to keep track of the association (s_tmsi (guti), mme_ue_s1ap_id)
 * \param ue_context_p The UE context to remove
 **/
void mme_notify_ue_context_released (
    mme_ue_context_t * const mme_ue_context_p,
    struct ue_mm_context_s *ue_context_p);

/** \brief Remove a UE context of the tree of known UEs.
 * \param ue_context_p The UE context to remove
 **/
void mme_remove_ue_context(mme_ue_context_t * const mme_ue_context,
		                   struct ue_mm_context_s * const ue_context_p);


int lock_ue_contexts(ue_mm_context_t * const ue_context_p);

int unlock_ue_contexts(ue_mm_context_t * const ue_context_p);

/** \brief Allocate memory for a new UE context
 * @returns Pointer to the new structure, NULL if allocation failed
 **/
ue_mm_context_t *mme_create_new_ue_context(void);

void mme_app_free_pdn_connection (pdn_context_t ** const pdn_connection);

void mme_app_ue_context_free_content (ue_mm_context_t * const mme_ue_context_p);

/** \brief Dump the UE contexts present in the tree
 **/
void mme_app_dump_ue_contexts(const mme_ue_context_t * const mme_ue_context);


void mme_app_handle_s1ap_ue_context_release_req(const itti_s1ap_ue_context_release_req_t const *s1ap_ue_context_release_req);

bearer_context_t* mme_app_get_bearer_context(ue_mm_context_t  * const ue_context, const ebi_t ebi);

void mme_app_handle_enb_deregister_ind(const itti_s1ap_eNB_deregistered_ind_t const* eNB_deregistered_ind);

bearer_context_t* mme_app_get_bearer_context_by_state(ue_mm_context_t * const ue_context, const pdn_cid_t cid, const mme_app_bearer_state_t state);

ebi_t mme_app_get_free_bearer_id(ue_mm_context_t * const ue_context);

void mme_app_free_bearer_context(bearer_context_t ** bc);

void mme_app_send_delete_session_request (struct ue_mm_context_s * const ue_context_p, const ebi_t ebi, const pdn_cid_t cid);

#endif /* FILE_MME_APP_UE_CONTEXT_SEEN */

/* @} */
