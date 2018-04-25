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


#ifndef FILE_ATTACH_COMPLETE_SEEN
#define FILE_ATTACH_COMPLETE_SEEN
#include <stdint.h>

#include "ProtocolDiscriminator.h"
#include "SecurityHeaderType.h"
#include "MessageType.h"
#include "EsmMessageContainer.h"

/* Minimum length macro. Formed by minimum length of each mandatory field */
#define ATTACH_COMPLETE_MINIMUM_LENGTH ( \
    ESM_MESSAGE_CONTAINER_MINIMUM_LENGTH )

/* Maximum length macro. Formed by maximum length of each field */
#define ATTACH_COMPLETE_MAXIMUM_LENGTH ( \
    ESM_MESSAGE_CONTAINER_MAXIMUM_LENGTH )


/*
 * Message name: Attach complete
 * Description: This message is sent by the UE to the network in response to an ATTACH ACCEPT message. See table 8.2.2.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct attach_complete_msg_tag {
  /* Mandatory fields */
  ProtocolDiscriminator         protocoldiscriminator:4;
  SecurityHeaderType            securityheadertype:4;
  MessageType                   messagetype;
  EsmMessageContainer           esmmessagecontainer;
} attach_complete_msg;

int decode_attach_complete(attach_complete_msg *attachcomplete, uint8_t *buffer, uint32_t len);

int encode_attach_complete(attach_complete_msg *attachcomplete, uint8_t *buffer, uint32_t len);

#endif /* ! defined(FILE_ATTACH_COMPLETE_SEEN) */

