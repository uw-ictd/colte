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



#ifndef DETACH_TYPE_H_
#define DETACH_TYPE_H_
#include <stdint.h>

#define DETACH_TYPE_MINIMUM_LENGTH 1
#define DETACH_TYPE_MAXIMUM_LENGTH 1

typedef struct DetachType_tag {
#define DETACH_TYPE_NORMAL_DETACH 0
#define DETACH_TYPE_SWITCH_OFF    1
  uint8_t  switchoff:1;
#define DETACH_TYPE_EPS     0b001
#define DETACH_TYPE_IMSI    0b010
#define DETACH_TYPE_EPS_IMSI    0b011
#define DETACH_TYPE_RESERVED_1    0b110
#define DETACH_TYPE_RESERVED_2    0b111
  uint8_t  typeofdetach:3;
} DetachType;

int encode_detach_type(DetachType *detachtype, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_detach_type_xml(DetachType *detachtype, uint8_t iei);

uint8_t encode_u8_detach_type(DetachType *detachtype);

int decode_detach_type(DetachType *detachtype, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_u8_detach_type(DetachType *detachtype, uint8_t iei, uint8_t value, uint32_t len);

#endif /* DETACH TYPE_H_ */

