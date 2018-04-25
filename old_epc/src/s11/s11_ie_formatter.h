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


#ifndef FILE_S11_IE_FORMATTER_SEEN
#define FILE_S11_IE_FORMATTER_SEEN

/* Imsi Information Element
 * 3GPP TS.29.274 #8.3
 * NOTE: Imsi is TBCD coded
 * octet 5   | Number digit 2 | Number digit 1   |
 * octet n+4 | Number digit m | Number digit m-1 |
 */
NwRcT s11_imsi_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_imsi_ie_set(NwGtpv2cMsgHandleT *msg, const Imsi_t *imsi);

NwRcT s11_msisdn_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

/* Node Type Information Element
 * 3GPP TS 29.274 #8.34
 * Node type:
 *      * 0 = MME
 *      * 1 = SGSN
 */
NwRcT s11_node_type_ie_get(uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_node_type_ie_set(NwGtpv2cMsgHandleT *msg, const node_type_t *node_type);

/* PDN Type Information Element
 * 3GPP TS 29.274 #8.34
 * PDN type:
 *      * 1 = IPv4
 *      * 2 = IPv6
 *      * 3 = IPv4v6
 */
NwRcT s11_pdn_type_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_pdn_type_ie_set(NwGtpv2cMsgHandleT *msg, const pdn_type_t *pdn_type);

/* RAT type Information Element
 * WARNING: the RAT type used in MME and S/P-GW is not the same as the one
 * for S11 interface defined in 3GPP TS 29.274 #8.17.
 */
NwRcT s11_rat_type_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_rat_type_ie_set(NwGtpv2cMsgHandleT *msg, const rat_type_t *rat_type);

/* EPS Bearer Id Information Element
 * 3GPP TS 29.274 #8.8
 */
NwRcT s11_ebi_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_ebi_ie_set(NwGtpv2cMsgHandleT *msg, const unsigned ebi);

/* Bearer Contexts to Create Information Element as part of Create Session Request
 * 3GPP TS 29.274 Table 7.2.1-2.
 */
NwRcT s11_bearer_context_to_be_created_ie_get (uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t * ieValue, void *arg);

int s11_bearer_context_to_be_created_ie_set (NwGtpv2cMsgHandleT * msg, const bearer_context_to_be_created_t * bearer_context);


int s11_bearer_context_to_be_modified_ie_set (NwGtpv2cMsgHandleT * msg, const bearer_context_to_be_modified_t * bearer_context);

NwRcT s11_bearer_context_to_be_modified_ie_get(uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

/* EPS Bearer Id Information Element
 * 3GPP TS 29.274 #8.8
 * ebi is 4 bits long
 */
int s11_ebi_ie_set(NwGtpv2cMsgHandleT *msg, const unsigned ebi);

NwRcT s11_ebi_ie_get (uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t * ieValue, void *arg);

NwRcT s11_ebi_ie_get_list (uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t * ieValue, void *arg);

/* Cause Information Element */
NwRcT s11_cause_ie_get(uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_cause_ie_set(NwGtpv2cMsgHandleT *msg, const gtp_cause_t  *cause);

/* Bearer Context Created grouped Information Element */
NwRcT s11_bearer_context_created_ie_get(uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_bearer_context_created_ie_set(NwGtpv2cMsgHandleT *msg, const bearer_context_created_t *bearer);

/* Serving Network Information Element
 * 3GPP TS 29.274 #8.18
 */
NwRcT s11_serving_network_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_serving_network_ie_set(
  NwGtpv2cMsgHandleT     *msg,
  const ServingNetwork_t *serving_network);


/* Fully Qualified TEID (F-TEID) Information Element */
int s11_fteid_ie_set (NwGtpv2cMsgHandleT * msg, const FTeid_t * fteid);

NwRcT s11_fteid_ie_get(uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

/* Protocol Configuration Options Information Element */
NwRcT s11_pco_ie_get (uint8_t ieType, uint8_t ieLength,
  uint8_t ieInstance, uint8_t * ieValue, void *arg);

int s11_pco_ie_set (NwGtpv2cMsgHandleT * msg,
  const protocol_configuration_options_t * pco);

/* PDN Address Allocation Information Element */
NwRcT s11_paa_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_paa_ie_set(NwGtpv2cMsgHandleT *msg, const PAA_t *paa);

/* Access Point Name Information Element
 * 3GPP TS 29.274 #8.6
 * NOTE: The APN field is not encoded as a dotted string as commonly used in
 * documentation.
 * The encoding of the APN field follows 3GPP TS 23.003 subclause 9.1
 */
NwRcT s11_apn_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_apn_ie_set(NwGtpv2cMsgHandleT *msg, const char *apn);

NwRcT s11_ambr_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

NwRcT s11_mei_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

NwRcT s11_uli_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

/* APN restrtiction Information Element */
int s11_apn_restriction_ie_set(
  NwGtpv2cMsgHandleT *msg, const uint8_t apn_restriction);

/* Bearer level Qos Information Element
 * 3GPP TS 29.274 #8.15
 */
NwRcT s11_bearer_qos_ie_get (uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t * ieValue, void *arg);

int s11_bearer_qos_ie_set(NwGtpv2cMsgHandleT *msg, const BearerQOS_t *bearer_qos);

/* IP address Information Element
 * 3GPP TS 29.274 #8.9
 */
NwRcT s11_ip_address_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_ip_address_ie_set(NwGtpv2cMsgHandleT     *msg,
                          const gtp_ip_address_t *ip_address);

/* Delay Value Information Element
 * 3GPP TS 29.274 #8.27
 */
NwRcT s11_delay_value_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_delay_value_ie_set(NwGtpv2cMsgHandleT *msg,
                           const DelayValue_t *delay_value);

/* UE Time Zone Information Element
 * 3GPP TS 29.274 #8.44
 */
NwRcT s11_ue_time_zone_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_ue_time_zone_ie_set(NwGtpv2cMsgHandleT *msg,
                            const UETimeZone_t *ue_time_zone);

/* Target Identification Information Element
 * 3GPP TS 29.274 #8.51
 */
NwRcT s11_target_identification_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

/* Bearer Flags Information Element
 * 3GPP TS 29.274 #8.32
 */
NwRcT s11_bearer_flags_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int s11_bearer_flags_ie_set(NwGtpv2cMsgHandleT   *msg,
                            const bearer_flags_t *bearer_flags);

/* Indication Element
 * 3GPP TS 29.274 #8.12
 */
NwRcT s11_indication_flags_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

int
s11_indication_flags_ie_set (
  NwGtpv2cMsgHandleT * msg,
  const indication_flags_t * indication_flags);


/* FQ-CSID Information Element
 * 3GPP TS 29.274 #8.62
 */

NwRcT s11_fqcsid_ie_get(
  uint8_t ieType, uint8_t ieLength, uint8_t ieInstance, uint8_t *ieValue, void *arg);

#endif /* FILE_S11_IE_FORMATTER_SEEN */
