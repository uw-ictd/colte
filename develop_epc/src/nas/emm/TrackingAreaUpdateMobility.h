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


/*
 *    R  E  A  D      M  E      B  E  F  O  R  E         D  O  I  N  G
 *
 *    A  N  Y  T  H  I  N  G      E  L  S  E
 *
 *
 *
 *  Due to legal concerns no mobility code concerning Normal TAU can be commit
 *  here in this git project actually called openair-cn.
 *  The contributions to Normal TAU can only be accepted inside the git project
 *  openair-cn-mobility.
 */

int 
emm_recv_tracking_area_update_req_type_normal (
  mme_ue_s1ap_id_t ue_id,
  const tracking_area_update_request_msg * msg,
  int *emm_cause );
