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
#include "LinkedEpsBearerIdentity.h"
#include "TrafficFlowAggregateDescription.h"
#include "EpsQualityOfService.h"
#include "EsmCause.h"
#include "ProtocolConfigurationOptions.h"

#ifndef BEARER_RESOURCE_MODIFICATION_REQUEST_H_
#define BEARER_RESOURCE_MODIFICATION_REQUEST_H_

/* Minimum length macro. Formed by minimum length of each mandatory field */
#define BEARER_RESOURCE_MODIFICATION_REQUEST_MINIMUM_LENGTH ( \
    TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_MINIMUM_LENGTH )

/* Maximum length macro. Formed by maximum length of each field */
#define BEARER_RESOURCE_MODIFICATION_REQUEST_MAXIMUM_LENGTH ( \
    TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_MAXIMUM_LENGTH + \
    EPS_QUALITY_OF_SERVICE_MAXIMUM_LENGTH + \
    ESM_CAUSE_MAXIMUM_LENGTH + \
    PROTOCOL_CONFIGURATION_OPTIONS_MAXIMUM_LENGTH )

/* If an optional value is present and should be encoded, the corresponding
 * Bit mask should be set to 1.
 */
# define BEARER_RESOURCE_MODIFICATION_REQUEST_REQUIRED_TRAFFIC_FLOW_QOS_PRESENT      (1<<0)
# define BEARER_RESOURCE_MODIFICATION_REQUEST_ESM_CAUSE_PRESENT                      (1<<1)
# define BEARER_RESOURCE_MODIFICATION_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<2)

typedef enum bearer_resource_modification_request_iei_tag {
  BEARER_RESOURCE_MODIFICATION_REQUEST_REQUIRED_TRAFFIC_FLOW_QOS_IEI       = 0x5B, /* 0x5B = 91 */
  BEARER_RESOURCE_MODIFICATION_REQUEST_ESM_CAUSE_IEI                       = 0x58, /* 0x58 = 88 */
  BEARER_RESOURCE_MODIFICATION_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_IEI  = 0x27, /* 0x27 = 39 */
} bearer_resource_modification_request_iei;

/*
 * Message name: Bearer resource modification request
 * Description: This message is sent by the UE to the network to request the modification of a dedicated bearer resource. See table 8.3.10.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct bearer_resource_modification_request_msg_tag {
  /* Mandatory fields */
  ProtocolDiscriminator                            protocoldiscriminator:4;
  EpsBearerIdentity                                epsbeareridentity:4;
  ProcedureTransactionIdentity                     proceduretransactionidentity;
  MessageType                                      messagetype;
  LinkedEpsBearerIdentity                          epsbeareridentityforpacketfilter;
  TrafficFlowAggregateDescription                  trafficflowaggregate;
  /* Optional fields */
  uint32_t                                         presencemask;
  EpsQualityOfService                              requiredtrafficflowqos;
  EsmCause                                         esmcause;
  ProtocolConfigurationOptions                     protocolconfigurationoptions;
} bearer_resource_modification_request_msg;

int decode_bearer_resource_modification_request(bearer_resource_modification_request_msg *bearerresourcemodificationrequest, uint8_t *buffer, uint32_t len);

int encode_bearer_resource_modification_request(bearer_resource_modification_request_msg *bearerresourcemodificationrequest, uint8_t *buffer, uint32_t len);

#endif /* ! defined(BEARER_RESOURCE_MODIFICATION_REQUEST_H_) */

