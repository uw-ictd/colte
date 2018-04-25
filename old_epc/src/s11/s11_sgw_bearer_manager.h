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

#ifndef FILE_S11_SGW_BEARER_MANAGER_SEEN
#define FILE_S11_SGW_BEARER_MANAGER_SEEN

int s11_sgw_handle_modify_bearer_request(
  NwGtpv2cStackHandleT *stack_p,
  NwGtpv2cUlpApiT      *pUlpApi);

int s11_sgw_handle_modify_bearer_response(
  NwGtpv2cStackHandleT    *stack_p,
  itti_s11_modify_bearer_response_t *modify_bearer_response_p);

int
s11_sgw_handle_release_access_bearers_request (
  NwGtpv2cStackHandleT * stack_p,
  NwGtpv2cUlpApiT * pUlpApi);

int s11_sgw_handle_release_access_bearers_response (
  NwGtpv2cStackHandleT * stack_p,
  itti_s11_release_access_bearers_response_t * response_p);

#endif /* FILE_S11_SGW_BEARER_MANAGER_SEEN */
