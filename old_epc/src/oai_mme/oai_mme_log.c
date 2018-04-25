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


#include "oai_mme.h"
#include "log.h"

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

int
oai_mme_log_specific (
  int log_level)
{
  if (log_level == 1) {
    asn_debug = 0;
    asn1_xer_print = 1;
    fd_g_debug_lvl = INFO;
  } else if (log_level == 2) {
    asn_debug = 1;
    asn1_xer_print = 1;
    fd_g_debug_lvl = ANNOYING;
  } else {
    asn1_xer_print = 0;
    asn_debug = 0;
    fd_g_debug_lvl = NONE;
  }

  return 0;
}
