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

/*! \file mme_app_itti_messaging.h
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/


#ifndef FILE_MME_APP_ITTI_MESSAGING_SEEN
#define FILE_MME_APP_ITTI_MESSAGING_SEEN

void mme_app_itti_ue_context_release(struct ue_mm_context_s *ue_context_p, enum s1cause cause);
int mme_app_notify_s1ap_ue_context_released(const mme_ue_s1ap_id_t   ue_idP);
int mme_app_send_s11_release_access_bearers_req (struct ue_mm_context_s *const ue_mm_context, const pdn_cid_t pdn_index);
int mme_app_send_s11_create_session_req (struct ue_mm_context_s *const ue_mm_context, const pdn_cid_t pdn_cid);

#endif /* FILE_MME_APP_ITTI_MESSAGING_SEEN */
