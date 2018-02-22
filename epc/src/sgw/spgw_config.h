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

/*! \file spgw_config.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#ifndef FILE_SPGW_CONFIG_SEEN
#define FILE_SPGW_CONFIG_SEEN
#include "sgw_config.h"
#include "pgw_config.h"


typedef struct spgw_config_s {
  sgw_config_t sgw_config;
  pgw_config_t pgw_config;
  bstring      config_file;
} spgw_config_t;

#ifndef SGW
extern spgw_config_t spgw_config;
#endif

int spgw_system (
  bstring command_pP,
  bool is_abort_on_errorP,
  const char *const file_nameP,
  const int line_numberP);

int spgw_config_parse_opt_line (
  int argc,
  char *argv[],
  spgw_config_t * spgw_config_p);

#endif /* FILE_SPGW_CONFIG_SEEN */
