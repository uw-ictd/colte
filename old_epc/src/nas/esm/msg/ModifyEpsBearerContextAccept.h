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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ProtocolDiscriminator.h"
#include "EpsBearerIdentity.h"
#include "ProcedureTransactionIdentity.h"
#include "MessageType.h"
#include "ProtocolConfigurationOptions.h"

#ifndef MODIFY_EPS_BEARER_CONTEXT_ACCEPT_H_
#define MODIFY_EPS_BEARER_CONTEXT_ACCEPT_H_

/* Minimum length macro. Formed by minimum length of each mandatory field */
#define MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MINIMUM_LENGTH (0)

/* Maximum length macro. Formed by maximum length of each field */
#define MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MAXIMUM_LENGTH ( \
    PROTOCOL_CONFIGURATION_OPTIONS_MAXIMUM_LENGTH )

/* If an optional value is present and should be encoded, the corresponding
 * Bit mask should be set to 1.
 */
# define MODIFY_EPS_BEARER_CONTEXT_ACCEPT_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<0)

typedef enum modify_eps_bearer_context_accept_iei_tag {
  MODIFY_EPS_BEARER_CONTEXT_ACCEPT_PROTOCOL_CONFIGURATION_OPTIONS_IEI  = 0x27, /* 0x27 = 39 */
} modify_eps_bearer_context_accept_iei;

/*
 * Message name: Modify EPS bearer context accept
 * Description: This message is sent by the UE to the network to acknowledge the modification of an active EPS bearer context. See table 8.3.16.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct modify_eps_bearer_context_accept_msg_tag {
  /* Mandatory fields */
  ProtocolDiscriminator                       protocoldiscriminator:4;
  EpsBearerIdentity                           epsbeareridentity:4;
  ProcedureTransactionIdentity                proceduretransactionidentity;
  MessageType                                 messagetype;
  /* Optional fields */
  uint32_t                                    presencemask;
  ProtocolConfigurationOptions                protocolconfigurationoptions;
} modify_eps_bearer_context_accept_msg;

int decode_modify_eps_bearer_context_accept(modify_eps_bearer_context_accept_msg *modifyepsbearercontextaccept, uint8_t *buffer, uint32_t len);

int encode_modify_eps_bearer_context_accept(modify_eps_bearer_context_accept_msg *modifyepsbearercontextaccept, uint8_t *buffer, uint32_t len);

#endif /* ! defined(MODIFY_EPS_BEARER_CONTEXT_ACCEPT_H_) */

