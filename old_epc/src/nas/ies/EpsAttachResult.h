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


#ifndef EPS_ATTACH_RESULT_H_
#define EPS_ATTACH_RESULT_H_
#include <stdint.h>

#define EPS_ATTACH_RESULT_MINIMUM_LENGTH 1
#define EPS_ATTACH_RESULT_MAXIMUM_LENGTH 1

#define EPS_ATTACH_RESULT_EPS   0b001
#define EPS_ATTACH_RESULT_EPS_IMSI  0b010
typedef uint8_t EpsAttachResult;

int encode_eps_attach_result(EpsAttachResult *epsattachresult, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_eps_attach_result_xml(EpsAttachResult *epsattachresult, uint8_t iei);

uint8_t encode_u8_eps_attach_result(EpsAttachResult *epsattachresult);

int decode_eps_attach_result(EpsAttachResult *epsattachresult, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_u8_eps_attach_result(EpsAttachResult *epsattachresult, uint8_t iei, uint8_t value, uint32_t len);

#endif /* EPS ATTACH RESULT_H_ */

