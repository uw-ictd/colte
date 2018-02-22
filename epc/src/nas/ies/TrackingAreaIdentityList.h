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

#ifndef TRACKING_AREA_IDENTITY_LIST_H_
#define TRACKING_AREA_IDENTITY_LIST_H_
#include <stdint.h>

#define TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH 8
#define TRACKING_AREA_IDENTITY_LIST_MAXIMUM_LENGTH 98

typedef struct TrackingAreaIdentityList_tag {
  /* XXX - The only supported type of list is a list of TACs
   * belonging to one PLMN, with consecutive TAC values */
#define TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS 0b00
#define TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_CONSECUTIVE_TACS     0b01
#define TRACKING_AREA_IDENTITY_LIST_MANY_PLMNS                    0b10
  uint8_t  typeoflist:2;
  uint8_t  numberofelements:5;
  uint8_t  mccdigit2[16];
  uint8_t  mccdigit1[16];
  uint8_t  mncdigit3[16];
  uint8_t  mccdigit3[16];
  uint8_t  mncdigit2[16];
  uint8_t  mncdigit1[16];
  uint16_t tac[16];
} TrackingAreaIdentityList;

int encode_tracking_area_identity_list(TrackingAreaIdentityList *trackingareaidentitylist, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_tracking_area_identity_list(TrackingAreaIdentityList *trackingareaidentitylist, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_tracking_area_identity_list_xml(TrackingAreaIdentityList *trackingareaidentitylist, uint8_t iei);

#endif /* TRACKING AREA IDENTITY LIST_H_ */

