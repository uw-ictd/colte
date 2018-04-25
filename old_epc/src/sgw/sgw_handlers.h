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

/*! \file sgw_handlers.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#ifndef FILE_SGW_HANDLERS_SEEN
#define FILE_SGW_HANDLERS_SEEN

int sgw_handle_create_session_request(const itti_s11_create_session_request_t * const session_req_p);
int sgw_handle_sgi_endpoint_created  (itti_sgi_create_end_point_response_t   * const resp_p);
int sgw_handle_sgi_endpoint_updated  (const itti_sgi_update_end_point_response_t   * const resp_p);
int sgw_handle_gtpv1uCreateTunnelResp(const Gtpv1uCreateTunnelResp  * const endpoint_created_p);
int sgw_handle_gtpv1uUpdateTunnelResp(const Gtpv1uUpdateTunnelResp  * const endpoint_updated_p);
int sgw_handle_gtpv1uDeleteTunnelResp(const Gtpv1uDeleteTunnelResp  * const endpoint_deleted_p);
int sgw_handle_modify_bearer_request (const itti_s11_modify_bearer_request_t  * const modify_bearer_p);
int sgw_handle_delete_session_request(const itti_s11_delete_session_request_t * const delete_session_p);
int sgw_handle_release_access_bearers_request(const itti_s11_release_access_bearers_request_t * const release_access_bearers_req_pP);
#endif /* FILE_SGW_HANDLERS_SEEN */
