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

/*! \file ue_context_manager.c
  \brief
  \author 
  \company 
  \email 
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include "pgw_lite_paa.h"

int allocate_ue_ipv4_address(const char *imsi, struct in_addr *addr) {
  // Call PGW IP Address allocator 
  return pgw_get_free_ipv4_paa_address (addr); 
}

int release_ue_ipv4_address(const char *imsi, struct in_addr *addr) {
  // Release IP address back to PGW IP Address allocator 
  return pgw_release_free_ipv4_paa_address (addr); 
}

void pgw_ip_address_pool_init(void) {
  pgw_load_pool_ip_addresses ();
  return;
}

