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

#ifndef PDN_TYPE_H_
#define PDN_TYPE_H_
#include <stdint.h>

#define PDN_TYPE_MINIMUM_LENGTH 1
#define PDN_TYPE_MAXIMUM_LENGTH 1

#define PDN_TYPE_IPV4   0b001
#define PDN_TYPE_IPV6   0b010
#define PDN_TYPE_IPV4V6   0b011
#define PDN_TYPE_UNUSED   0b100
typedef uint8_t PdnType;

int encode_pdn_type(PdnType *pdntype, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_pdn_type_xml(PdnType *pdntype, uint8_t iei);

uint8_t encode_u8_pdn_type(PdnType *pdntype);

int decode_pdn_type(PdnType *pdntype, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_u8_pdn_type(PdnType *pdntype, uint8_t iei, uint8_t value, uint32_t len);

#endif /* PDN TYPE_H_ */

