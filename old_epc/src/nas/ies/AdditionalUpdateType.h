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

#ifndef ADDITIONAL_UPDATE_TYPE_H_
#define ADDITIONAL_UPDATE_TYPE_H_
#include <stdint.h>

#define ADDITIONAL_UPDATE_TYPE_MINIMUM_LENGTH 1
#define ADDITIONAL_UPDATE_TYPE_MAXIMUM_LENGTH 1

typedef enum {
  NO_ADDITIONAL_INFORMATION = 0x0,
  SMS_ONLY                  = 0x1,
  MAX                       = 1 << ADDITIONAL_UPDATE_TYPE_MAXIMUM_LENGTH,
  SENTINEL_MAX              = 0xFF
} AdditionalUpdateType;

int encode_additional_update_type(AdditionalUpdateType *additionalupdatetype, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_additional_update_type(AdditionalUpdateType *additionalupdatetype, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_additional_update_type_xml(AdditionalUpdateType *additionalupdatetype, uint8_t iei);

#endif /* ADDITIONAL UPDATE TYPE_H_ */

