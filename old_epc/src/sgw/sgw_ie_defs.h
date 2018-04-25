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

/*! \file sgw_ie_defs.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#ifndef FILE_SGW_IE_DEFS_SEEN
#define FILE_SGW_IE_DEFS_SEEN
#include "common_types.h"
#include "3gpp_24.008.h"


typedef uint8_t  EBI_t;
typedef uint8_t  APNRestriction_t;
typedef uint8_t  DelayValue_t;
typedef uint32_t teid_t;
typedef uint32_t SequenceNumber_t;

/* Only one type of address can be present at the same time
 * This type is applicable to IP address Information Element defined
 * in 3GPP TS 29.274 #8.9
 */
typedef struct {
#define GTP_IP_ADDR_v4  0x0
#define GTP_IP_ADDR_v6  0x1
  unsigned present:1;
  union {
    uint8_t v4[4];
    uint8_t v6[16];
  } address;
} gtp_ip_address_t;

/* 3GPP TS 29.274 Figure 8.12 */

typedef struct indication_flags_s {
  uint8_t daf:1;
  uint8_t dtf:1;
  uint8_t hi:1;
  uint8_t dfi:1;
  uint8_t oi:1;
  uint8_t isrsi:1;
  uint8_t israi:1;
  uint8_t sgwci:1;

  uint8_t sqci:1;
  uint8_t uimsi:1;
  uint8_t cfsi:1;
  uint8_t crsi:1;
  uint8_t p:1;
  uint8_t pt:1;
  uint8_t si:1;
  uint8_t msv:1;

  uint8_t spare1:1;
  uint8_t spare2:1;
  uint8_t spare3:1;
  uint8_t s6af:1;
  uint8_t s4af:1;
  uint8_t mbmdt:1;
  uint8_t israu:1;
  uint8_t ccrsi:1;
} indication_flags_t;

/* Bit mask for octet 7 in indication IE */
// UPDATE RELEASE 10
#define S6AF_FLAG_BIT_POS  4
#define S4AF_FLAG_BIT_POS  3
#define MBMDT_FLAG_BIT_POS 2
#define ISRAU_FLAG_BIT_POS 1
#define CCRSI_FLAG_BIT_POS 0

/* Bit mask for octet 6 in indication IE */
#define SQSI_FLAG_BIT_POS   7
#define UIMSI_FLAG_BIT_POS  6
#define CFSI_FLAG_BIT_POS   5
#define CRSI_FLAG_BIT_POS   4
#define P_FLAG_BIT_POS      3
#define PT_FLAG_BIT_POS     2
#define SI_FLAG_BIT_POS     1
#define MSV_FLAG_BIT_POS    0

/* Bit mask for octet 5 in indication IE */
#define DAF_FLAG_BIT_POS      7
#define DTF_FLAG_BIT_POS      6
#define HI_FLAG_BIT_POS       5
#define DFI_FLAG_BIT_POS      4
#define OI_FLAG_BIT_POS       3
#define ISRSI_FLAG_BIT_POS    2
#define ISRAI_FLAG_BIT_POS    1
#define SGWCI_FLAG_BIT_POS    0

typedef struct {
  pdn_type_t pdn_type;
  uint8_t ipv4_address[4];
  uint8_t ipv6_address[16];
  /* Note in rel.8 the ipv6 prefix length has a fixed value of /64 */
  uint8_t ipv6_prefix_length;
} PAA_t;


#define IMSI(imsi) \
        (imsi)->digit[0], \
        (imsi)->digit[1], \
        (imsi)->digit[2], \
        (imsi)->digit[3], \
        (imsi)->digit[4], \
        (imsi)->digit[5], \
        (imsi)->digit[6], \
        (imsi)->digit[7], \
        (imsi)->digit[8], \
        (imsi)->digit[9], \
        (imsi)->digit[10], \
        (imsi)->digit[11], \
        (imsi)->digit[12], \
        (imsi)->digit[13], \
        (imsi)->digit[14]

typedef struct {
  uint8_t digit[IMSI_BCD_DIGITS_MAX+1]; // +1 for '\0` macro sprintf changed in snprintf
  uint8_t length;
} Imsi_t;

typedef struct {
  uint8_t digit[MSISDN_LENGTH];
  uint8_t length;
} Msisdn_t;

#define MEI_IMEI    0x0
#define MEI_IMEISV  0x1

typedef struct {
  uint8_t present;
  union {
    unsigned imei:15;
    unsigned imeisv:16;
  } choice;
} Mei_t;

typedef struct {
  uint8_t  mcc[3];
  uint8_t  mnc[3];
  uint16_t lac;
  uint16_t ci;
} Cgi_t;

typedef struct {
  uint8_t  mcc[3];
  uint8_t  mnc[3];
  uint16_t lac;
  uint16_t sac;
} Sai_t;

typedef struct {
  uint8_t  mcc[3];
  uint8_t  mnc[3];
  uint16_t lac;
  uint16_t rac;
} Rai_t;

typedef struct {
  uint8_t  mcc[3];
  uint8_t  mnc[3];
  uint16_t tac;
} Tai_t;

typedef struct {
  uint8_t  mcc[3];
  uint8_t  mnc[3];
  uint32_t eci;
} Ecgi_t;

typedef struct {
  uint8_t  mcc[3];
  uint8_t  mnc[3];
  uint16_t lac;
} Lai_t;

#define ULI_CGI  0x01
#define ULI_SAI  0x02
#define ULI_RAI  0x04
#define ULI_TAI  0x08
#define ULI_ECGI 0x10
#define ULI_LAI  0x20

typedef struct {
  uint8_t present;
  struct {
    Cgi_t  cgi;
    Sai_t  sai;
    Rai_t  rai;
    Tai_t  tai;
    Ecgi_t ecgi;
    Lai_t  lai;
  } s;
} Uli_t;

typedef struct {
  uint8_t mcc[3];
  uint8_t mnc[3];
} ServingNetwork_t;
/*
typedef enum RatType_e {
    RAT_TYPE_UTRAN = 1,
    RAT_TYPE_GERAN,
    RAT_TYPE_WLAN,
    RAT_TYPE_GAN,
    RAT_TYPE_HSPA_EVOLUTION,
    RAT_TYPE_EUTRAN,
} RatType_t;*/

/* WARNING: not complete... */
typedef enum InterfaceType_e {
  S1_U_ENODEB_GTP_U = 0,
  S1_U_SGW_GTP_U    = 1,
  S12_RNC_GTP_U     = 2,
  S12_SGW_GTP_U     = 3,
  S5_S8_SGW_GTP_U   = 4,
  S5_S8_PGW_GTP_U   = 5,
  S5_S8_SGW_GTP_C   = 6,
  S5_S8_PGW_GTP_C   = 7,
  S11_MME_GTP_C     = 10,
  S11_SGW_GTP_C     = 11,
} InterfaceType_t;

typedef struct {
  unsigned        ipv4:1;
  unsigned        ipv6:1;
  InterfaceType_t interface_type;
  teid_t          teid; ///< TEID or GRE Key
  uint32_t        ipv4_address;
  uint8_t         ipv6_address[16];
} FTeid_t;
/*
 *              typedef struct {
                  pdn_type_t pdn_type;
                  union {
                      uint8_t ipv4_address[4];
                      uint8_t ipv6_address[16];
                  } address;
              } ip_address_t;

 */
#define FTEID_T_2_IP_ADDRESS_T(fte_p,ip_p) \
do { \
    if (fte_p->ipv4) { \
        ip_p->pdn_type = IPv4; \
        ip_p->address.ipv4_address[0] = (uint8_t)(fte_p->ipv4_address & 0x000000FF);         \
        ip_p->address.ipv4_address[1] = (uint8_t)((fte_p->ipv4_address & 0x0000FF00) >> 8);  \
        ip_p->address.ipv4_address[2] = (uint8_t)((fte_p->ipv4_address & 0x00FF0000) >> 16); \
        ip_p->address.ipv4_address[3] = (uint8_t)((fte_p->ipv4_address & 0xFF000000) >> 24); \
    } \
    if (fte_p->ipv6) { \
        if (fte_p->ipv4) { \
            ip_p->pdn_type = IPv4_AND_v6; \
        } else { \
            ip_p->pdn_type = IPv6; \
        } \
        memcpy(ip_p->address.ipv6_address, fte_p->ipv6_address, 16); \
    } \
} while (0)

typedef enum {
  TARGET_ID_RNC_ID       = 0,
  TARGET_ID_MACRO_ENB_ID = 1,
  TARGET_ID_CELL_ID      = 2,
  TARGET_ID_HOME_ENB_ID  = 3
                           /* Other values are spare */
} target_type_t;

typedef struct {
  uint16_t lac;
  uint8_t  rac;

  /* Length of RNC Id can be 2 bytes if length of element is 8
   * or 4 bytes long if length is 10.
   */
  uint32_t rnc_id;
} rnc_id_t;

typedef struct {
  unsigned enb_id:20;
  uint16_t tac;
} macro_enb_id_t;

typedef struct {
  unsigned enb_id:28;
  uint16_t tac;
} home_enb_id_t;

typedef struct {
  /* Common part */
  uint8_t target_type;

  uint8_t  mcc[3];
  uint8_t  mnc[3];
  union {
    rnc_id_t       rnc_id;
    macro_enb_id_t macro_enb_id;
    home_enb_id_t  home_enb_id;
  } target_id;
} target_identification_t;

typedef enum SelectionMode_e {
  MS_O_N_P_APN_S_V    = 0,    ///< MS or network provided APN, subscribed verified
  MS_P_APN_S_N_V      = 1,    ///< MS provided APN, subscription not verified
  N_P_APN_S_N_V       = 2,    ///< Network provided APN, subscription not verified
} SelectionMode_t;

typedef struct {
  uint32_t uplink_ambr;
  uint32_t downlink_ambr;
} AMBR_t;

typedef enum node_id_type_e {
  GLOBAL_UNICAST_IPv4 = 0,
  GLOBAL_UNICAST_IPv6 = 1,
  TYPE_EXOTIC         = 2, ///< (MCC * 1000 + MNC) << 12 + Integer value assigned to MME by operator
} node_id_type_t;

typedef struct {
  node_id_type_t node_id_type;
  uint16_t       csid;          ///< Connection Set Identifier
  union {
    uint32_t   unicast_ipv4;
    uint8_t    unicast_ipv6[16];
    struct {
      uint16_t mcc;
      uint16_t mnc;
      uint16_t operator_specific_id;
    } exotic;
  } node_id;
} FQ_CSID_t;

typedef struct {
  uint8_t  time_zone;
  unsigned daylight_saving_time:2;
} UETimeZone_t;

typedef enum AccessMode_e {
  CLOSED_MODE = 0,
  HYBRID_MODE = 1,
} AccessMode_t;

typedef struct {
  uint8_t  mcc[3];
  uint8_t  mnc[3];
  uint32_t csg_id;
  AccessMode_t access_mode;
  unsigned lcsg:1;
  unsigned cmi:1;
} UCI_t;

typedef struct {
  /* PPC (Prohibit Payload Compression):
   * This flag is used to determine whether an SGSN should attempt to
   * compress the payload of user data when the users asks for it
   * to be compressed (PPC = 0), or not (PPC = 1).
   */
  unsigned ppc:1;

  /* VB (Voice Bearer):
   * This flag is used to indicate a voice bearer when doing PS-to-CS
   * SRVCC handover.
   */
  unsigned vb:1;
} bearer_flags_t;

#define PRE_EMPTION_CAPABILITY_ENABLED  (0x0)
#define PRE_EMPTION_CAPABILITY_DISABLED (0x1)
#define PRE_EMPTION_VULNERABILITY_ENABLED  (0x0)
#define PRE_EMPTION_VULNERABILITY_DISABLED (0x1)

typedef struct {
  /* PCI (Pre-emption Capability)
   * The following values are defined:
   * - PRE-EMPTION_CAPABILITY_ENABLED (0)
   *    This value indicates that the service data flow or bearer is allowed
   *    to get resources that were already assigned to another service data
   *    flow or bearer with a lower priority level.
   * - PRE-EMPTION_CAPABILITY_DISABLED (1)
   *    This value indicates that the service data flow or bearer is not
   *    allowed to get resources that were already assigned to another service
   *    data flow or bearer with a lower priority level.
   * Default value: PRE-EMPTION_CAPABILITY_DISABLED
   */
  unsigned pci:1;
  /* PL (Priority Level): defined in 3GPP TS.29.212 #5.3.45
   * Values 1 to 15 are defined, with value 1 as the highest level of priority.
   * Values 1 to 8 should only be assigned for services that are authorized to
   * receive prioritized treatment within an operator domain. Values 9 to 15
   * may be assigned to resources that are authorized by the home network and
   * thus applicable when a UE is roaming.
   */
  unsigned pl:4;
  /* PVI (Pre-emption Vulnerability): defined in 3GPP TS.29.212 #5.3.47
   * Defines whether a service data flow can lose the resources assigned to it
   * in order to admit a service data flow with higher priority level.
   * The following values are defined:
   * - PRE-EMPTION_VULNERABILITY_ENABLED (0)
   *   This value indicates that the resources assigned to the service data
   *   flow or bearer can be pre-empted and allocated to a service data flow
   *   or bearer with a higher priority level.
   * - PRE-EMPTION_VULNERABILITY_DISABLED (1)
   *   This value indicates that the resources assigned to the service data
   *   flow or bearer shall not be pre-empted and allocated to a service data
   *   flow or bearer with a higher priority level.
   * Default value: EMPTION_VULNERABILITY_ENABLED
   */
  unsigned pvi:1;
  uint8_t  qci;
  ambr_t   gbr;           ///< Guaranteed bit rate
  ambr_t   mbr;           ///< Maximum bit rate
} BearerQOS_t;

/* TFT operation Code */
typedef enum {
  /* 0 = spare */

  CREATE_NEW_TFT                          = 0x1,
  DELETE_EXISTING_TFT                     = 0x2,
  ADD_PACKET_FILTERS_TO_EXISTING_TFT      = 0x3,
  REPLACE_PACKET_FILTERS_IN_EXISTING_TFT  = 0x4,
  DELETE_PACKET_FILTERS_FROM_EXISTING_TFT = 0x5,
  NO_TFT_OPERATION                        = 0x6,

  TFT_OPERATION_CODE_MAX

  /* Other Values Reserved */
} tft_operation_code_t;

/* Defined in 3GPP TS 24.008 Table 10.5.162 */
typedef enum {
  PACKET_FILTER_PRE_REL_7         = 0,
  PACKET_FILTER_DIRECTION_DL_ONLY = 1,
  PACKET_FILTER_DIRECTION_UL_ONLY = 2,
  PACKET_FILTER_BIDIRECTIONAL     = 3,
} packet_filter_direction_t;

/* The Traffic Flow Template is specified in 3GPP TS 24.008 #10.5.6.12
 */
typedef struct {
  /* The TFT operation code "No TFT operation" shall be used
   * if a parameters list is included but no packet filter
   * list is included in the traffic flow template information
   * element.
   */
  tft_operation_code_t tft_operation_code;

  /* The E bit indicates if a parameters list is included
   * in the TFT IE and it is encoded as follows:
   * - 0 parameters list is not included
   * - 1 parameters list is included
   */
  unsigned             e_bit:1;

  /* For the "delete existing TFT" operation and for the "no TFT
   * operation", the number of packet filters shall be coded
   * as 0. For all other operations, the number of packet filters
   * shall be greater than 0 and less than or equal to 15.
   */
  uint8_t              number_of_packet_filters;

  /* TODO: add packet filter list as defined in 3GPP TS 29.274 Table 10.5.162 */
} tft_t;

typedef enum node_type_e {
  NODE_TYPE_MME  = 0,
  NODE_TYPE_SGSN = 1
} node_type_t;


/* Cause as defined in 3GPP TS 29.274 #8.4 */
typedef enum SGWCause_e {
  /* Request / Initial message */
  LOCAL_DETACH                    = 2,
  COMPLETE_DETACH                 = 3,
  RAT_CHANGE_3GPP_TO_NON_3GPP     = 4,  ///< RAT changed from 3GPP to Non-3GPP
  ISR_DEACTIVATION                = 5,
  ERROR_IND_FROM_RNC_ENB_SGSN     = 6,
  IMSI_DETACH_ONLY                = 7,
  /* Acceptance in a Response/Triggered message */
  REQUEST_ACCEPTED                = 16,
  REQUEST_ACCEPTED_PARTIALLY      = 17,
  NEW_PDN_TYPE_NW_PREF            = 18, ///< New PDN type due to network preference
  NEW_PDN_TYPE_SAB_ONLY           = 19, ///< New PDN type due to single address bearer only
  /* Rejection in a Response triggered message. */
  CONTEXT_NOT_FOUND               = 64,
  INVALID_MESSAGE_FORMAT          = 65,
  INVALID_LENGTH                  = 67,
  SERVICE_NOT_SUPPORTED           = 68,
  MANDATORY_IE_INCORRECT          = 69,
  MANDATORY_IE_MISSING            = 70,
  SYSTEM_FAILURE                  = 72,
  NO_RESOURCES_AVAILABLE          = 73,
  SEMANTIC_ERROR_IN_TFT           = 74,
  SYNTACTIC_ERROR_IN_TFT          = 75,
  SEMANTIC_ERRORS_IN_PF           = 76,
  SYNTACTIC_ERRORS_IN_PF          = 77,
  MISSING_OR_UNKNOWN_APN          = 78,
  GRE_KEY_NOT_FOUND               = 80,
  RELOCATION_FAILURE              = 81,
  DENIED_IN_RAT                   = 82,
  ALL_DYNAMIC_IP_ADD_OCCUPIED     = 84,
  UE_NOT_RESPONDING               = 87,
  UE_REFUSES                      = 88,
  SERVICE_DENIED                  = 89,
  UNABLE_TO_PAGE_UE               = 90,
  NO_MEMORY_AVAILABLE             = 91,
  REQUEST_REJECTED                = 94,
  DATA_FORWARDING_NOT_SUPPORTED   = 106,
  INVALID_REPLY_FROM_REMOTE_PEER  = 107,
  FALLBACK_TO_GTPV1               = 108,
  INVALID_PEER                    = 109,
  TEMP_REJECT_HO_IN_PROGRESS      = 110, ///< Temporarily rejected due to handover procedure in progress
  REJECTED_FOR_PMIPv6_REASON      = 112, ///< Request rejected for a PMIPv6 reason (see 3GPP TS 29.275 [26]).
  M_PDN_APN_NOT_ALLOWED           = 116, ///< Multiple PDN connections for a given APN not allowed.
  SGW_CAUSE_MAX
} SGWCause_t;

typedef struct {
  uint8_t  cause_value;
  uint8_t  pce:1;
  uint8_t  bce:1;
  uint8_t  cs:1;

  uint8_t  offending_ie_type;
  uint16_t offending_ie_length;
  uint8_t  offending_ie_instance;
} gtp_cause_t;

typedef struct {
  uint8_t     eps_bearer_id;    ///< EBI,  Mandatory CSR
  BearerQOS_t bearer_level_qos;
  tft_t       tft;              ///< Bearer TFT, Optional CSR, This IE may be included on the S4/S11 and S5/S8 interfaces.
} bearer_to_create_t;

//-----------------
typedef struct bearer_context_to_be_created_s {
  uint8_t      eps_bearer_id;       ///< EBI,  Mandatory CSR
  tft_t        tft;                 ///< Bearer TFT, Optional CSR, This IE may be included on the S4/S11 and S5/S8 interfaces.
  FTeid_t      s1u_enb_fteid;       ///< S1-U eNodeB F-TEID, Conditional CSR, This IE shall be included on the S11 interface for X2-based handover with SGW relocation.
  FTeid_t      s4u_sgsn_fteid;      ///< S4-U SGSN F-TEID, Conditional CSR, This IE shall be included on the S4 interface if the S4-U interface is used.
  FTeid_t      s5_s8_u_sgw_fteid;   ///< S5/S8-U SGW F-TEID, Conditional CSR, This IE shall be included on the S5/S8 interface for an "eUTRAN Initial Attach",
                                    ///  a "PDP Context Activation" or a "UE Requested PDN Connectivity".
  FTeid_t      s5_s8_u_pgw_fteid;   ///< S5/S8-U PGW F-TEID, Conditional CSR, This IE shall be included on the S4 and S11 interfaces for the TAU/RAU/Handover
                                    /// cases when the GTP-based S5/S8 is used.
  FTeid_t      s12_rnc_fteid;       ///< S12 RNC F-TEID, Conditional Optional CSR, This IE shall be included on the S4 interface if the S12
                                    /// interface is used in the Enhanced serving RNS relocation with SGW relocation procedure.
  FTeid_t      s2b_u_epdg_fteid;    ///< S2b-U ePDG F-TEID, Conditional CSR, This IE shall be included on the S2b interface for an Attach
                                    /// with GTP on S2b, a UE initiated Connectivity to Additional PDN with GTP on S2b and a Handover to Untrusted Non-
                                    /// 3GPP IP Access with GTP on S2b.
  /* This parameter is received only if the QoS parameters have been modified */
  BearerQOS_t  bearer_level_qos;    ///< Bearer QoS, Mandatory CSR
} bearer_context_to_be_created_t;

typedef struct bearer_contexts_to_be_created_s {
#define MSG_CREATE_SESSION_REQUEST_MAX_BEARER_CONTEXTS   11
uint8_t num_bearer_context;
bearer_context_to_be_created_t bearer_contexts[MSG_CREATE_SESSION_REQUEST_MAX_BEARER_CONTEXTS];    ///< Bearer Contexts to be created
///< Several IEs with the same type and instance value shall be
///< included on the S4/S11 and S5/S8 interfaces as necessary
///< to represent a list of Bearers. One single IE shall be
///< included on the S2b interface.
///< One bearer shall be included for an E-UTRAN Initial
///< Attach, a PDP Context Activation, a UE requested PDN
///< Connectivity, an Attach with GTP on S2b, a UE initiated
///< Connectivity to Additional PDN with GTP on S2b and a
///< Handover to Untrusted Non-3GPP IP Access with GTP on
///< S2b.
///< One or more bearers shall be included for a
///< Handover/TAU/RAU with an SGW change.
} bearer_contexts_to_be_created_t;

//-----------------
typedef struct bearer_context_created_s {
  uint8_t      eps_bearer_id;       ///< EPS Bearer ID
  SGWCause_t   cause;

  /* This parameter is used on S11 interface only */
  FTeid_t      s1u_sgw_fteid;       ///< S1-U SGW F-TEID

  /* This parameter is used on S4 interface only */
  FTeid_t      s4u_sgw_fteid;       ///< S4-U SGW F-TEID

  /* This parameter is used on S11 and S5/S8 interface only for a
   * GTP-based S5/S8 interface and during:
   * - E-UTRAN Inintial attch
   * - PDP Context Activation
   * - UE requested PDN connectivity
   */
  FTeid_t      s5_s8_u_pgw_fteid;   ///< S4-U SGW F-TEID

  /* This parameter is used on S4 interface only and when S12 interface is used */
  FTeid_t      s12_sgw_fteid;       ///< S12 SGW F-TEID

  /* This parameter is received only if the QoS parameters have been modified */
  BearerQOS_t *bearer_level_qos;

  tft_t        tft;                 ///< Bearer TFT
} bearer_context_created_t;

typedef struct bearer_contexts_created_s {
  uint8_t num_bearer_context;
  bearer_context_created_t bearer_contexts[MSG_CREATE_SESSION_REQUEST_MAX_BEARER_CONTEXTS];
} bearer_contexts_created_t;

//-----------------
typedef struct bearer_context_modified_s {
  uint8_t    eps_bearer_id;   ///< EPS Bearer ID
  SGWCause_t cause;
  FTeid_t    s1u_sgw_fteid;   ///< Sender F-TEID for user plane
} bearer_context_modified_t;

typedef struct bearer_contexts_modified_s {
#define MSG_MODIFY_BEARER_RESPONSE_MAX_BEARER_CONTEXTS   11
  uint8_t num_bearer_context;
  bearer_context_modified_t bearer_contexts[MSG_MODIFY_BEARER_RESPONSE_MAX_BEARER_CONTEXTS];
} bearer_contexts_modified_t;

//-----------------
typedef struct bearer_context_marked_for_removal_s {
  uint8_t    eps_bearer_id;   ///< EPS bearer ID
  SGWCause_t cause;
} bearer_context_marked_for_removal_t;

typedef struct bearer_contexts_marked_for_removal_s {
  uint8_t num_bearer_context;
  bearer_context_marked_for_removal_t bearer_contexts[MSG_MODIFY_BEARER_RESPONSE_MAX_BEARER_CONTEXTS];
} bearer_contexts_marked_for_removal_t;

//-----------------
typedef struct bearer_context_to_be_modified_s {
  uint8_t eps_bearer_id;      ///< EPS Bearer ID
  FTeid_t s1_eNB_fteid;       ///< S1 eNodeB F-TEID
} bearer_context_to_be_modified_t;

typedef struct bearer_contexts_to_be_modified_s {
#define MSG_MODIFY_BEARER_REQUEST_MAX_BEARER_CONTEXTS   11
  uint8_t num_bearer_context;
  bearer_context_to_be_modified_t bearer_contexts[MSG_MODIFY_BEARER_REQUEST_MAX_BEARER_CONTEXTS];
} bearer_contexts_to_be_modified_t;
//-----------------

typedef struct bearer_context_to_be_removed_s {
  uint8_t eps_bearer_id;      ///< EPS Bearer ID, Mandatory
  FTeid_t s4u_sgsn_fteid;     ///< S4-U SGSN F-TEID, Conditional , redundant
} bearer_context_to_be_removed_t; // Within Create Session Request, Modify Bearer Request, Modify Access Bearers Request


typedef struct bearer_contexts_to_be_removed_s {
  uint8_t num_bearer_context;
  bearer_context_to_be_removed_t bearer_contexts[MSG_CREATE_SESSION_REQUEST_MAX_BEARER_CONTEXTS];
} bearer_contexts_to_be_removed_t;

typedef struct ebi_list_s {
  uint32_t   num_ebi;
  #define RELEASE_ACCESS_BEARER_MAX_BEARERS   8
  EBI_t      ebis[RELEASE_ACCESS_BEARER_MAX_BEARERS]  ;
} ebi_list_t;



#endif  /* FILE_SGW_IE_DEFS_SEEN */

