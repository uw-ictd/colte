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

#ifndef FILE_DETACH_REQUEST_SEEN
#define FILE_DETACH_REQUEST_SEEN

#include "SecurityHeaderType.h"
#include "MessageType.h"
#include "DetachType.h"
#include "NasKeySetIdentifier.h"
#include "EpsMobileIdentity.h"
#include "3gpp_23.003.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"

/* Minimum length macro. Formed by minimum length of each mandatory field */
#define DETACH_REQUEST_MINIMUM_LENGTH DETACH_TYPE_MINIMUM_LENGTH

/* SMS NOTE: Below is the old minimum length macro. Now that we're supporting
// the MME->UE detach request message (which is much shorter) we needed to
// change this macro. We could keep them both if we want to re-name and break
// up (i.e. UE and MME as separate message types) but I see no reason to.
 #define DETACH_REQUEST_MINIMUM_LENGTH ( \
     DETACH_TYPE_MINIMUM_LENGTH + \
     NAS_KEY_SET_IDENTIFIER_MINIMUM_LENGTH + \
     EPS_MOBILE_IDENTITY_MINIMUM_LENGTH )
*/
/* Maximum length macro. Formed by maximum length of each field */
#define DETACH_REQUEST_MAXIMUM_LENGTH ( \
    DETACH_TYPE_MAXIMUM_LENGTH + \
    NAS_KEY_SET_IDENTIFIER_MAXIMUM_LENGTH + \
    EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH )

/*
 * Message name: Detach request
 * Description: This message is sent by the UE to request the release of an EMM context. See table 8.2.11.1.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct detach_request_msg_tag {
  /* Mandatory fields */
  eps_protocol_discriminator_t protocoldiscriminator:4;
  security_header_type_t       securityheadertype:4;
  message_type_t               messagetype;
  detach_type_t                   detachtype;
  NasKeySetIdentifier          naskeysetidentifier;
  eps_mobile_identity_t        gutiorimsi;
} detach_request_msg;

int decode_detach_request(detach_request_msg *detachrequest, uint8_t *buffer, uint32_t len);

int encode_detach_request(detach_request_msg *detachrequest, uint8_t *buffer, uint32_t len);

#endif /* ! defined(FILE_DETACH_REQUEST_SEEN) */

