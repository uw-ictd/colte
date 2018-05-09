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

/*! \file pgw_lite_paa.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#ifndef FILE_PGW_LITE_PAA_SEEN
#define FILE_PGW_LITE_PAA_SEEN

#include "sgw_config.h"

void pgw_load_pool_ip_addresses       (void);
int pgw_get_free_ipv4_paa_address     (struct in_addr * const addr_P, const char *imsi);
int pgw_release_free_ipv4_paa_address (const struct in_addr * const addr_P);
int pgw_get_imsi_from_ipv4		      (struct in_addr * const addr_P, char *imsi);

// sgw_db_connector.c
int spgw_mysql_connect (const sgw_config_t * sgw_config_p);
void spgw_mysql_disconnect (void);
int spgw_get_imsi_from_ip (struct in_addr *ip, char *imsi);
int spgw_get_ip_from_imsi (struct in_addr *ip, const char *imsi);

#endif
