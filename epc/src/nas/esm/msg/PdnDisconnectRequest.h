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

#ifndef PDN_DISCONNECT_REQUEST_H_
#define PDN_DISCONNECT_REQUEST_H_
#include "MessageType.h"
#include "3gpp_23.003.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"

/* Minimum length macro. Formed by minimum length of each mandatory field */
#define PDN_DISCONNECT_REQUEST_MINIMUM_LENGTH ( \
    LINKED_EPS_BEARER_IDENTITY_MINIMUM_LENGTH )

/* Maximum length macro. Formed by maximum length of each field */
#define PDN_DISCONNECT_REQUEST_MAXIMUM_LENGTH ( \
    LINKED_EPS_BEARER_IDENTITY_MAXIMUM_LENGTH + \
    PROTOCOL_CONFIGURATION_OPTIONS_IE_MAX_LENGTH )

/* If an optional value is present and should be encoded, the corresponding
 * Bit mask should be set to 1.
 */
# define PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<0)

typedef enum pdn_disconnect_request_iei_tag {
  PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_IEI  = SM_PROTOCOL_CONFIGURATION_OPTIONS_IEI,
} pdn_disconnect_request_iei;

/*
 * Message name: PDN disconnect request
 * Description: This message is sent by the UE to the network to initiate release of a PDN connection. See table 8.3.22.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct pdn_disconnect_request_msg_tag {
  /* Mandatory fields */
  eps_protocol_discriminator_t                           protocoldiscriminator:4;
  ebi_t                                                  epsbeareridentity:4;
  pti_t                                                  proceduretransactionidentity;
  message_type_t                                         messagetype;
  linked_eps_bearer_identity_t                                linkedepsbeareridentity;
  /* Optional fields */
  uint32_t                                               presencemask;
  protocol_configuration_options_t                       protocolconfigurationoptions;
} pdn_disconnect_request_msg;

int decode_pdn_disconnect_request(pdn_disconnect_request_msg *pdndisconnectrequest, uint8_t *buffer, uint32_t len);

int encode_pdn_disconnect_request(pdn_disconnect_request_msg *pdndisconnectrequest, uint8_t *buffer, uint32_t len);

#endif /* ! defined(PDN_DISCONNECT_REQUEST_H_) */

