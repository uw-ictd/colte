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

#ifndef PROTOCOL_CONFIGURATION_OPTIONS_H_
#define PROTOCOL_CONFIGURATION_OPTIONS_H_
#include <stdint.h>
#include "3gpp_24.008.h"

#define PROTOCOL_CONFIGURATION_OPTIONS_MINIMUM_LENGTH PCO_MIN_LENGTH
#define PROTOCOL_CONFIGURATION_OPTIONS_MAXIMUM_LENGTH PCO_MAX_LENGTH


typedef enum ProtocolConfigurationOptionsList_ids_tag {
  PCO_UNKNOWN                         = 0,
  PCO_P_CSCF_IPV6_ADDRESS_REQ         = 1,
  PCO_IM_CN_SUBSYSTEM_SIGNALING_FLAG  = 2,
  PCO_DNS_SERVER_IPV6_ADDRESS_REQ     = 3,
  PCO_NOT_SUPPORTED                   = 4,
  PCO_MS_SUPPORTED_OF_NETWORK_REQUESTED_BEARER_CONTROL_INDICATOR = 5,
  PCO_RESERVED                        = 6,
  /* TODO: complete me */
} ProtocolConfigurationOptionsList_ids;


typedef protocol_configuration_options_t ProtocolConfigurationOptions;

int encode_ProtocolConfigurationOptions(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_ProtocolConfigurationOptions(ProtocolConfigurationOptions *protocolconfigurationoptions, const uint8_t iei, const uint8_t * const buffer, const uint32_t len);

void dump_ProtocolConfigurationOptions_xml(ProtocolConfigurationOptions *protocolconfigurationoptions, uint8_t iei);

#endif /* PROTOCOL CONFIGURATION OPTIONS_H_ */

