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


#ifndef FILE_S1AP_MME_TA_SEEN
#define FILE_S1AP_MME_TA_SEEN

enum {
  TA_LIST_UNKNOWN_TAC = -2,
  TA_LIST_UNKNOWN_PLMN = -1,
  TA_LIST_RET_OK = 0,
  TA_LIST_NO_MATCH = 0x1,
  TA_LIST_AT_LEAST_ONE_MATCH = 0x2,
  TA_LIST_COMPLETE_MATCH = 0x3,
};

int s1ap_mme_compare_ta_lists(S1ap_SupportedTAs_t *ta_list);

#endif /* FILE_S1AP_MME_TA_SEEN */
