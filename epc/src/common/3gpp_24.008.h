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

#ifndef FILE_3GPP_24_008_SEEN
#define FILE_3GPP_24_008_SEEN
#include <stdint.h>
#include "bstrlib.h"

//------------------------------------------------------------------------------
// 10.5.5.12 MS network capability
//------------------------------------------------------------------------------
//9.9.3.34 UE network capability
#define MS_NETWORK_CAPABILITY_MINIMUM_LENGTH 3
#define MS_NETWORK_CAPABILITY_MAXIMUM_LENGTH 10

typedef struct ms_network_capability_s {
#define MS_NETWORK_CAPABILITY_GEA1                               0b10000000
  uint8_t  gea1:1;
#define MS_NETWORK_CAPABILITY_SM_CAP_VIA_DEDICATED_CHANNELS      0b01000000
  uint8_t  smdc:1;
#define MS_NETWORK_CAPABILITY_SM_CAP_VIA_GPRS_CHANNELS           0b00100000
  uint8_t  smgc:1;
#define MS_NETWORK_CAPABILITY_UCS2_SUPPORT                       0b00010000
  uint8_t  ucs2:1;
#define MS_NETWORK_CAPABILITY_SS_SCREENING_INDICATOR             0b00001100
  uint8_t  sssi:2;
#define MS_NETWORK_CAPABILITY_SOLSA                              0b00000010
  uint8_t  solsa:1;
#define MS_NETWORK_CAPABILITY_REVISION_LEVEL_INDICATOR           0b00000001
  uint8_t  revli:1;

#define MS_NETWORK_CAPABILITY_PFC_FEATURE_MODE                   0b10000000
  uint8_t  pfc:1;
#define MS_NETWORK_CAPABILITY_GEA2                               0b01000000
#define MS_NETWORK_CAPABILITY_GEA3                               0b00100000
#define MS_NETWORK_CAPABILITY_GEA4                               0b00010000
#define MS_NETWORK_CAPABILITY_GEA5                               0b00001000
#define MS_NETWORK_CAPABILITY_GEA6                               0b00000100
#define MS_NETWORK_CAPABILITY_GEA7                               0b00000010
  uint8_t  egea:6;
#define MS_NETWORK_CAPABILITY_LCS_VA                             0b00000001
  uint8_t  lcs:1;

#define MS_NETWORK_CAPABILITY_PS_INTER_RAT_HO_GERAN_TO_UTRAN_IU  0b10000000
  uint8_t  ps_ho_utran:1;
#define MS_NETWORK_CAPABILITY_PS_INTER_RAT_HO_GERAN_TO_EUTRAN_S1 0b01000000
  uint8_t  ps_ho_eutran:1;
#define MS_NETWORK_CAPABILITY_EMM_COMBINED_PROCEDURE             0b00100000
  uint8_t  emm_cpc:1;
#define MS_NETWORK_CAPABILITY_ISR                                0b00010000
  uint8_t  isr:1;
#define MS_NETWORK_CAPABILITY_SRVCC                              0b00001000
  uint8_t  srvcc:1;
#define MS_NETWORK_CAPABILITY_EPC                                0b00000100
  uint8_t  epc_cap:1;
#define MS_NETWORK_CAPABILITY_NOTIFICATION                       0b00000010
  uint8_t  nf_cap:1;
#define MS_NETWORK_CAPABILITY_GERAN_NETWORK_SHARING              0b00000001
  uint8_t  geran_ns:1;
} ms_network_capability_t;


//------------------------------------------------------------------------------
// 10.5.6.3 Protocol configuration options
//------------------------------------------------------------------------------
#define PCO_MIN_LENGTH                                               3
#define PCO_MAX_LENGTH                                               253
#define PCO_CONFIGURATION_PROTOCOL_PPP_FOR_USE_WITH_IP_PDP_TYPE_OR_IP_PDN_TYPE  0b000

// Protocol identifiers defined in RFC 3232
#define PCO_PI_LCP                                                      (0xC021)
#define PCO_PI_PAP                                                      (0xC023)
#define PCO_PI_CHAP                                                     (0xC223)
#define PCO_PI_IPCP                                                     (0x8021)

/* CONTAINER IDENTIFIER MS to network direction:*/
#define PCO_CI_P_CSCF_IPV6_ADDRESS_REQUEST                              (0x0001)
#define PCO_CI_DNS_SERVER_IPV6_ADDRESS_REQUEST                          (0x0003)
// NOT SUPPORTED                                                        (0x0004)
#define PCO_CI_MS_SUPPORT_OF_NETWORK_REQUESTED_BEARER_CONTROL_INDICATOR (0x0005)
// RESERVED                                                             (0x0006)
#define PCO_CI_DSMIPV6_HOME_AGENT_ADDRESS_REQUEST                       (0x0007)
#define PCO_CI_DSMIPV6_HOME_NETWORK_PREFIX_REQUEST                      (0x0008)
#define PCO_CI_DSMIPV6_IPV4_HOME_AGENT_ADDRESS_REQUEST                  (0x0009)
#define PCO_CI_IP_ADDRESS_ALLOCATION_VIA_NAS_SIGNALLING                 (0x000A)
#define PCO_CI_IPV4_ADDRESS_ALLOCATION_VIA_DHCPV4                       (0x000B)
#define PCO_CI_P_CSCF_IPV4_ADDRESS_REQUEST                              (0x000C)
#define PCO_CI_DNS_SERVER_IPV4_ADDRESS_REQUEST                          (0x000D)
#define PCO_CI_MSISDN_REQUEST                                           (0x000E)
#define PCO_CI_IFOM_SUPPORT_REQUEST                                     (0x000F)
#define PCO_CI_IPV4_LINK_MTU_REQUEST                                    (0x0010)
// RESERVED                                                             (0xFF00..FFFF)

/* CONTAINER IDENTIFIER Network to MS direction:*/
#define PCO_CI_P_CSCF_IPV6_ADDRESS                                      (0x0001)
#define PCO_CI_DNS_SERVER_IPV6_ADDRESS                                  (0x0003)
#define PCO_CI_POLICY_CONTROL_REJECTION_CODE                            (0x0004)
#define PCO_CI_SELECTED_BEARER_CONTROL_MODE                             (0x0005)
// RESERVED                                                             (0x0006)
#define PCO_CI_DSMIPV6_HOME_AGENT_ADDRESS                               (0x0007)
#define PCO_CI_DSMIPV6_HOME_NETWORK_PREFIX                              (0x0008)
#define PCO_CI_DSMIPV6_IPV4_HOME_AGENT_ADDRESS                          (0x0009)
// RESERVED                                                             (0x000A)
// RESERVED                                                             (0x000B)
#define PCO_CI_P_CSCF_IPV4_ADDRESS                                      (0x000C)
#define PCO_CI_DNS_SERVER_IPV4_ADDRESS                                  (0x000D)
#define PCO_CI_MSISDN                                                   (0x000E)
#define PCO_CI_IFOM_SUPPORT                                             (0x000F)
#define PCO_CI_IPV4_LINK_MTU                                            (0x0010)
// RESERVED                                                             (0xFF00..FFFF)

/* Both directions:*/
#define PCO_CI_IM_CN_SUBSYSTEM_SIGNALING_FLAG                           (0x0002)

typedef struct pco_protocol_or_container_id_s {
  uint16_t    id;
  uint8_t     length;
  bstring     contents;
} pco_protocol_or_container_id_t;

typedef struct protocol_configuration_options_s {
  uint8_t     ext:1;
  uint8_t     spare:4;
  uint8_t     configuration_protocol:3;
  uint8_t     num_protocol_or_container_id;
  // arbitrary value, can be greater than defined (250/3)
# define PCO_UNSPEC_MAXIMUM_PROTOCOL_ID_OR_CONTAINER_ID 8
  pco_protocol_or_container_id_t protocol_or_container_ids[PCO_UNSPEC_MAXIMUM_PROTOCOL_ID_OR_CONTAINER_ID];
} protocol_configuration_options_t;

void copy_protocol_configuration_options (protocol_configuration_options_t * const pco_dst, const protocol_configuration_options_t * const pco_src);
void clear_protocol_configuration_options (protocol_configuration_options_t * const pco);

int decode_protocol_configuration_options (
    protocol_configuration_options_t * protocolconfigurationoptions,
    const uint8_t * const buffer,
    const uint32_t len);

int
encode_protocol_configuration_options (
    const protocol_configuration_options_t * const protocolconfigurationoptions,
    uint8_t * buffer,
    uint32_t len);

bstring protocol_configuration_options_to_xml (protocol_configuration_options_t * pco);


#endif /* FILE_3GPP_24_008_SEEN */
