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

#ifndef EPS_UPDATE_TYPE_H_
#define EPS_UPDATE_TYPE_H_
#include <stdint.h>

#define EPS_UPDATE_TYPE_MINIMUM_LENGTH 1
#define EPS_UPDATE_TYPE_MAXIMUM_LENGTH 1

#define EPS_UPDATE_TYPE_TA_UPDATING                              0
#define EPS_UPDATE_TYPE_COMBINED_TA_LA_UPDATING                  1
#define EPS_UPDATE_TYPE_COMBINED_TA_LA_UPDATING_WITH_IMSI_ATTACH 2
#define EPS_UPDATE_TYPE_PERIODIC_UPDATING                        3


typedef struct EpsUpdateType_tag {
  uint8_t  activeflag:1;
  uint8_t  epsupdatetypevalue:3;
} EpsUpdateType;

int encode_eps_update_type(EpsUpdateType *epsupdatetype, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_eps_update_type_xml(EpsUpdateType *epsupdatetype, uint8_t iei);

uint8_t encode_u8_eps_update_type(EpsUpdateType *epsupdatetype);

int decode_eps_update_type(EpsUpdateType *epsupdatetype, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_u8_eps_update_type(EpsUpdateType *epsupdatetype, uint8_t iei, uint8_t value, uint32_t len);

#endif /* EPS UPDATE TYPE_H_ */

