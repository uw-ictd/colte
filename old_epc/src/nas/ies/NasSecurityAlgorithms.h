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

#ifndef NAS_SECURITY_ALGORITHMS_H_
#define NAS_SECURITY_ALGORITHMS_H_
#include <stdint.h>

#define NAS_SECURITY_ALGORITHMS_MINIMUM_LENGTH 1
#define NAS_SECURITY_ALGORITHMS_MAXIMUM_LENGTH 2

typedef struct NasSecurityAlgorithms_tag {
#define NAS_SECURITY_ALGORITHMS_EEA0  0b000
#define NAS_SECURITY_ALGORITHMS_EEA1  0b001
#define NAS_SECURITY_ALGORITHMS_EEA2  0b010
#define NAS_SECURITY_ALGORITHMS_EEA3  0b011
#define NAS_SECURITY_ALGORITHMS_EEA4  0b100
#define NAS_SECURITY_ALGORITHMS_EEA5  0b101
#define NAS_SECURITY_ALGORITHMS_EEA6  0b110
#define NAS_SECURITY_ALGORITHMS_EEA7  0b111
  uint8_t  typeofcipheringalgorithm:3;
#define NAS_SECURITY_ALGORITHMS_EIA0  0b000
#define NAS_SECURITY_ALGORITHMS_EIA1  0b001
#define NAS_SECURITY_ALGORITHMS_EIA2  0b010
#define NAS_SECURITY_ALGORITHMS_EIA3  0b011
#define NAS_SECURITY_ALGORITHMS_EIA4  0b100
#define NAS_SECURITY_ALGORITHMS_EIA5  0b101
#define NAS_SECURITY_ALGORITHMS_EIA6  0b110
#define NAS_SECURITY_ALGORITHMS_EIA7  0b111
  uint8_t  typeofintegrityalgorithm:3;
} NasSecurityAlgorithms;

int encode_nas_security_algorithms(NasSecurityAlgorithms *nassecurityalgorithms, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_nas_security_algorithms_xml(NasSecurityAlgorithms *nassecurityalgorithms, uint8_t iei);

int decode_nas_security_algorithms(NasSecurityAlgorithms *nassecurityalgorithms, uint8_t iei, uint8_t *buffer, uint32_t len);

#endif /* NAS SECURITY ALGORITHMS_H_ */

