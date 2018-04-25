/*
 * Copyright (c) 2015, EURECOM (www.eurecom.fr)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */



#ifndef FILE_COMMON_TYPES_SEEN
#define FILE_COMMON_TYPES_SEEN

#include <stdint.h>
#include "3gpp_23.003.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"
#include "3gpp_24.301.h"
#include "3gpp_33.401.h"
#include "3gpp_36.401.h"
#include "security_types.h"
#include "common_dim.h"

void clear_guti(guti_t * const guti);
void clear_imsi(imsi_t * const imsi);
void clear_imei(imei_t * const imei);
void clear_imeisv(imeisv_t * const imeisv);
void clear_tai(tai_t * const tai);

typedef uint16_t                 sctp_stream_id_t;
typedef uint32_t                 sctp_assoc_id_t;
typedef uint64_t enb_s1ap_id_key_t ;
#define MME_APP_ENB_S1AP_ID_KEY(kEy, eNb_Id, eNb_Ue_S1Ap_Id) do { kEy = (((enb_s1ap_id_key_t)eNb_Id) << 24) | eNb_Ue_S1Ap_Id; } while(0);
#define MME_APP_ENB_S1AP_ID_KEY2ENB_S1AP_ID(kEy) (enb_ue_s1ap_id_t)(((enb_s1ap_id_key_t)kEy) & ENB_UE_S1AP_ID_MASK)
#define MME_APP_ENB_S1AP_ID_KEY_FORMAT "0x%16"PRIX64

#define M_TMSI_BIT_MASK          UINT32_MAX


//------------------------------------------------------------------------------
// UE S1AP IDs

#define INVALID_ENB_UE_S1AP_ID_KEY   0xFFFFFFFFFFFFFFFF
#define ENB_UE_S1AP_ID_MASK      0x00FFFFFF
#define ENB_UE_S1AP_ID_FMT       "0x%06"PRIX32

#define MME_UE_S1AP_ID_FMT       "0x%08"PRIX32


/* INVALID_MME_UE_S1AP_ID 
 * Any value between 0..2^32-1, is allowed/valid as per 3GPP spec 36.413.
 * Here we are conisdering 0 as invalid. Don't allocate 0 and consider this as invalid 
 */
#define INVALID_MME_UE_S1AP_ID   0x0     

//------------------------------------------------------------------------------
// TEIDs
typedef uint32_t                 teid_t;
#define TEID_FMT                "0x%"PRIX32
typedef teid_t                   s11_teid_t;
typedef teid_t                   s1u_teid_t;

//------------------------------------------------------------------------------
// IMSI

typedef uint64_t                 imsi64_t;
#define IMSI_64_FMT              "%"SCNu64
#define INVALID_IMSI64           (imsi64_t)0

//------------------------------------------------------------------------------
// PLMN



/* Checks Mobile Country Code equality */
#define MCCS_ARE_EQUAL(n1, n2)  (((n1).mcc_digit1 == (n2).mcc_digit1) && \
                                 ((n1).mcc_digit2 == (n2).mcc_digit2) && \
                                 ((n1).mcc_digit3 == (n2).mcc_digit3))

/* Checks Mobile Network Code equality */
#define MNCS_ARE_EQUAL(n1, n2)  (((n1).mnc_digit1 == (n2).mnc_digit1) &&  \
                                 ((n1).mnc_digit2 == (n2).mnc_digit2) &&  \
                                 ((n1).mnc_digit3 == (n2).mnc_digit3))

/* Checks PLMNs equality */
#define PLMNS_ARE_EQUAL(p1, p2) ((MCCS_ARE_EQUAL((p1),(p2))) && \
                                 (MNCS_ARE_EQUAL((p1),(p2))))
// MCC digit 2 MCC digit 1 octet 1
// MNC digit 3 MCC digit 3 octet 2
// MNC digit 2 MNC digit 1 octet 3
// The coding of this field is the responsibility of each administration but BCD coding
// shall be used. The MNC shall consist of 2 or 3 digits. If a network operator decides
// to use only two digits in the MNC, bits 5 to 8 of octet 2 shall be coded as "1111".
#define PLMN_FMT "%c%c%c.%c%c%c"
#define PLMN_ARG(PlMn_PtR) \
  (char)((PlMn_PtR)->mcc_digit1+0x30), (char)((PlMn_PtR)->mcc_digit2+0x30), (char)((PlMn_PtR)->mcc_digit3+0x30), \
  (char)((PlMn_PtR)->mnc_digit1+0x30), (char)((PlMn_PtR)->mnc_digit2+0x30), \
  (((PlMn_PtR)->mnc_digit3) == 0x0f) ? ' ':(char)((PlMn_PtR)->mnc_digit3+0x30)

/* Checks PLMN validity !?! */
#define PLMN_IS_VALID(plmn) (((plmn).mcc_digit1 &    \
                              (plmn).mcc_digit2 &    \
                              (plmn).mcc_digit3) != 0x0F)
//------------------------------------------------------------------------------
// TAI
#define TAI_LIST_MAX_SIZE 16
typedef struct tai_list_s {
  uint8_t list_type;
  uint8_t n_tais;
  tai_t  tai[TAI_LIST_MAX_SIZE];
}tai_list_t;


/* Checks TAIs equality */
#define TAIS_ARE_EQUAL(t1, t2)  ((PLMNS_ARE_EQUAL((t1).plmn,(t2).plmn)) && \
                                 ((t1).tac == (t2).tac))
#define TAC_FMT "0x%04X"
#define TAI_FMT PLMN_FMT"-"TAC_FMT
#define TAI_ARG(tAi_PtR) \
  PLMN_ARG(&(tAi_PtR)->plmn),\
  (tAi_PtR)->tac

/* Checks TAC validity */
#define TAC_IS_VALID(tac)   (((tac) != INVALID_TAC_0000) && ((tac) != INVALID_TAC_FFFE))

/* Checks TAI validity */
#define TAI_IS_VALID(tai)   (PLMN_IS_VALID((tai).plmn) &&   \
                             TAC_IS_VALID((tai).tac))


//------------------------------------------------------------------------------
// GUTI
#define GUTI_FMT PLMN_FMT"|%04x|%02x|%08x"
#define GUTI_ARG(GuTi_PtR) \
  PLMN_ARG(&(GuTi_PtR)->gummei.plmn), \
  (GuTi_PtR)->gummei.mme_gid,\
  (GuTi_PtR)->gummei.mme_code,\
  (GuTi_PtR)->m_tmsi
#define MSISDN_LENGTH      (15)
#define IMEI_DIGITS_MAX    (15)
#define IMEISV_DIGITS_MAX  (16)
#define APN_MAX_LENGTH     (100)
#define PRIORITY_LEVEL_MAX (15)
#define PRIORITY_LEVEL_MIN (1)
#define BEARERS_PER_UE     (11)
#define MAX_APN_PER_UE     (5)

//------------------------------------------------------------------------------
typedef uint8_t       ksi_t;
#define KSI_NO_KEY_AVAILABLE     0x07



typedef uint8_t     AcT_t;      /* Access Technology    */


typedef enum {
  RAT_WLAN           = 0,
  RAT_VIRTUAL        = 1,
  RAT_UTRAN          = 1000,
  RAT_GERAN          = 1001,
  RAT_GAN            = 1002,
  RAT_HSPA_EVOLUTION = 1003,
  RAT_EUTRAN         = 1004,
  RAT_CDMA2000_1X    = 2000,
  RAT_HRPD           = 2001,
  RAT_UMB            = 2002,
  RAT_EHRPD          = 2003,
} rat_type_t;

#define NUMBER_OF_RAT_TYPE 11

typedef enum {
  SS_SERVICE_GRANTED = 0,
  SS_OPERATOR_DETERMINED_BARRING = 1,
  SS_MAX,
} subscriber_status_t;

typedef enum {
  NAM_PACKET_AND_CIRCUIT = 0,
  NAM_RESERVED           = 1,
  NAM_ONLY_PACKET        = 2,
  NAM_MAX,
} network_access_mode_t;

typedef uint64_t bitrate_t;

typedef char*    APN_t;
typedef uint8_t  APNRestriction_t;
typedef uint8_t  DelayValue_t;
typedef uint8_t  priority_level_t;
typedef uint32_t SequenceNumber_t;
typedef uint32_t access_restriction_t;
typedef uint32_t context_identifier_t;
typedef uint32_t rau_tau_timer_t;

//typedef uint32_t in_addr_t; is network byte order

typedef uint32_t ard_t;
#define ARD_UTRAN_NOT_ALLOWED               (1U)
#define ARD_GERAN_NOT_ALLOWED               (1U << 1)
#define ARD_GAN_NOT_ALLOWED                 (1U << 2)
#define ARD_I_HSDPA_EVO_NOT_ALLOWED         (1U << 3)
#define ARD_E_UTRAN_NOT_ALLOWED             (1U << 4)
#define ARD_HO_TO_NON_3GPP_NOT_ALLOWED      (1U << 5)
#define ARD_MAX                             (1U << 6)



typedef union {
  uint8_t imei[IMEI_DIGITS_MAX-1]; // -1 =  remove CD/SD digit
  uint8_t imeisv[IMEISV_DIGITS_MAX];
} me_identity_t;

typedef struct {
  bitrate_t br_ul;
  bitrate_t br_dl;
} ambr_t;

typedef uint32_t ipv4_nbo_t;

typedef enum {
  IPv4 = 0,
  IPv6 = 1,
  IPv4_AND_v6 = 2,
  IPv4_OR_v6 = 3,
  IP_MAX,
} pdn_type_t;

typedef struct {
  pdn_type_t pdn_type;
  struct {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } address;
} ip_address_t;

typedef enum {
  QCI_1 = 1,
  QCI_2 = 2,
  QCI_3 = 3,
  QCI_4 = 4,
  QCI_5 = 5,
  QCI_6 = 6,
  QCI_7 = 7,
  QCI_8 = 8,
  QCI_9 = 9,
  /* Values from 128 to 254 are operator specific.
   * Other are reserved.
   */
  QCI_MAX,
} qci_t;

typedef enum {
  PRE_EMPTION_CAPABILITY_ENABLED  = 0,
  PRE_EMPTION_CAPABILITY_DISABLED = 1,
  PRE_EMPTION_CAPABILITY_MAX,
} pre_emp_capability_t;

typedef enum {
  PRE_EMPTION_VULNERABILITY_ENABLED  = 0,
  PRE_EMPTION_VULNERABILITY_DISABLED = 1,
  PRE_EMPTION_VULNERABILITY_MAX,
} pre_emp_vulnerability_t;

typedef struct {
  priority_level_t priority_level;
  pre_emp_vulnerability_t pre_emp_vulnerability;
  pre_emp_capability_t    pre_emp_capability;
} allocation_retention_priority_t;

typedef struct eps_subscribed_qos_profile_s {
  qci_t qci;
  allocation_retention_priority_t allocation_retention_priority;
} eps_subscribed_qos_profile_t;

typedef struct apn_configuration_s {
  context_identifier_t context_identifier;

  /* Each APN configuration can have 0, 1, or 2 ip address:
   * - 0 means subscribed is dynamically allocated by P-GW depending on the
   * pdn_type
   * - 1 Only one type of IP address is returned by HSS
   * - 2 IPv4 and IPv6 address are returned by HSS and are statically
   * allocated
   */
  uint8_t nb_ip_address;
  ip_address_t ip_address[2];

  pdn_type_t  pdn_type;
  char        service_selection[APN_MAX_LENGTH];
  int         service_selection_length;
  eps_subscribed_qos_profile_t subscribed_qos;
  ambr_t ambr;
} apn_configuration_t;

typedef enum {
  ALL_APN_CONFIGURATIONS_INCLUDED = 0,
  MODIFIED_ADDED_APN_CONFIGURATIONS_INCLUDED = 1,
  ALL_APN_MAX,
} all_apn_conf_ind_t;

typedef struct {
  context_identifier_t context_identifier;
  all_apn_conf_ind_t   all_apn_conf_ind;
  /* Number of APNs provided */
  uint8_t nb_apns;
  /* List of APNs configuration 1 to n elements */
  struct apn_configuration_s apn_configuration[MAX_APN_PER_UE];
} apn_config_profile_t;

typedef struct {
  subscriber_status_t   subscriber_status;
  char                  msisdn[MSISDN_LENGTH + 1];
  uint8_t               msisdn_length;
  network_access_mode_t access_mode;
  access_restriction_t  access_restriction;
  ambr_t                subscribed_ambr;
  apn_config_profile_t  apn_config_profile;
  rau_tau_timer_t       rau_tau_timer;
} subscription_data_t;

typedef struct authentication_info_s {
  uint8_t         nb_of_vectors;
  eutran_vector_t eutran_vector[MAX_EPS_AUTH_VECTORS];
} authentication_info_t;

typedef enum {
  DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE = 4181,
  DIAMETER_ERROR_USER_UNKNOWN              = 5001,
  DIAMETER_ERROR_ROAMING_NOT_ALLOWED       = 5004,
  DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION  = 5420,
  DIAMETER_ERROR_RAT_NOT_ALLOWED           = 5421,
  DIAMETER_ERROR_EQUIPMENT_UNKNOWN         = 5422,
  DIAMETER_ERROR_UNKOWN_SERVING_NODE       = 5423,
} s6a_experimental_result_t;

typedef enum {
  DIAMETER_SUCCESS = 2001,
} s6a_base_result_t;

typedef struct {
#define S6A_RESULT_BASE         0x0
#define S6A_RESULT_EXPERIMENTAL 0x1
  unsigned present:1;

  union {
    /* Experimental result as defined in 3GPP TS 29.272 */
    s6a_experimental_result_t experimental;
    /* Diameter basics results as defined in RFC 3588 */
    s6a_base_result_t         base;
  } choice;
} s6a_result_t;

#include "commonDef.h"

#endif /* FILE_COMMON_TYPES_SEEN */
