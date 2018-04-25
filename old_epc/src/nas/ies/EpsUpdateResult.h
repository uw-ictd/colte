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

#ifndef EPS_UPDATE_RESULT_H_
#define EPS_UPDATE_RESULT_H_
#include <stdint.h>

#define EPS_UPDATE_RESULT_MINIMUM_LENGTH 1
#define EPS_UPDATE_RESULT_MAXIMUM_LENGTH 1

#define EPS_UPDATE_RESULT_TA_UPDATED                               0
#define EPS_UPDATE_RESULT_COMBINED_TA_LA_UPDATED                   1
#define EPS_UPDATE_RESULT_TA_UPDATED_AND_ISR_ACTIVATED             4
#define EPS_UPDATE_RESULT_COMBINED_TA_LA_UPDATED_AND_ISR_ACTIVATED 5


typedef uint8_t EpsUpdateResult;

int encode_eps_update_result(EpsUpdateResult *epsupdateresult, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_eps_update_result_xml(EpsUpdateResult *epsupdateresult, uint8_t iei);

uint8_t encode_u8_eps_update_result(EpsUpdateResult *epsupdateresult);

int decode_eps_update_result(EpsUpdateResult *epsupdateresult, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_u8_eps_update_result(EpsUpdateResult *epsupdateresult, uint8_t iei, uint8_t value, uint32_t len);

#endif /* EPS UPDATE RESULT_H_ */

