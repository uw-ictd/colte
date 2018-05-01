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

/*! \file pgw_ue_ip_address_alloc.h
* \brief
* \author 
* \company
* \email: 
*/

#ifndef PGW_UE_IP_ADDRESS_ALLOC_SEEN
#define PGW_UE_IP_ADDRESS_ALLOC_SEEN

int allocate_ue_ipv4_address (const char *imsi, struct in_addr *addr); 
int release_ue_ipv4_address (const char *imsi, struct in_addr *addr);
int ue_get_imsi_from_ipv4(char *imsi, struct in_addr *addr);
void pgw_ip_address_pool_init (void); 

#endif /*PGW_UE_IP_ADDRESS_ALLOC_SEEN */
