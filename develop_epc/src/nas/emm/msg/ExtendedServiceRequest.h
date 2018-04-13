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

#ifndef FILE_EXTENDED_SERVICE_REQUEST_SEEN
#define FILE_EXTENDED_SERVICE_REQUEST_SEEN

#include "SecurityHeaderType.h"
#include "MessageType.h"
#include "ServiceType.h"
#include "NasKeySetIdentifier.h"
#include "CsfbResponse.h"
#include "3gpp_23.003.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"


/* Minimum length macro. Formed by minimum length of each mandatory field */
#define EXTENDED_SERVICE_REQUEST_MINIMUM_LENGTH ( \
    SERVICE_TYPE_MINIMUM_LENGTH + \
    NAS_KEY_SET_IDENTIFIER_MINIMUM_LENGTH + \
    MOBILE_IDENTITY_IE_MIN_LENGTH )

/* Maximum length macro. Formed by maximum length of each field */
#define EXTENDED_SERVICE_REQUEST_MAXIMUM_LENGTH ( \
    SERVICE_TYPE_MAXIMUM_LENGTH + \
    NAS_KEY_SET_IDENTIFIER_MAXIMUM_LENGTH + \
    MOBILE_IDENTITY_IE_MAX_LENGTH + \
    CSFB_RESPONSE_MAXIMUM_LENGTH )


/*
 * Message name: Extended service request
 * Description: This message is sent by the UE to the network to initiate a CS fallback call or respond to a mobile terminated CS fallback request from the network. See table 8.2.15.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct extended_service_request_msg_tag {
  /* Mandatory fields */
  eps_protocol_discriminator_t          protocoldiscriminator:4;
  security_header_type_t                securityheadertype:4;
  message_type_t                        messagetype;
  service_type_t                        servicetype;
  NasKeySetIdentifier                   naskeysetidentifier;
  mobile_identity_t                     mtmsi;
  /* Optional fields */
  uint32_t                              presencemask;
  csfb_response_t                       csfbresponse;
} extended_service_request_msg;

int decode_extended_service_request(extended_service_request_msg *extendedservicerequest, uint8_t *buffer, uint32_t len);

int encode_extended_service_request(extended_service_request_msg *extendedservicerequest, uint8_t *buffer, uint32_t len);

#endif /* ! defined(FILE_EXTENDED_SERVICE_REQUEST_SEEN) */

