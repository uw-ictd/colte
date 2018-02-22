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


#ifndef FILE_NAS_ITTI_MESSAGING_SEEN
#define FILE_NAS_ITTI_MESSAGING_SEEN
#include <stdint.h>

#include "bstrlib.h"
#include "assertions.h"
#include "log.h"
#include "msc.h"
#include "intertask_interface.h"
#include "3gpp_24.301.h"
#include "esm_proc.h"

int nas_itti_plain_msg(
  const char          *buffer,
  const nas_message_t *msg,
  const size_t         lengthP,
  const bool           is_down_link);

int nas_itti_protected_msg(
  const char          *buffer,
  const nas_message_t *msg,
  const size_t         lengthP,
  const bool           is_down_link);


int nas_itti_dl_data_req(
  const mme_ue_s1ap_id_t ue_idP,
  bstring                nas_msgP,
  nas_error_code_t transaction_status);

void nas_itti_pdn_connectivity_req(
  int                     ptiP,
  unsigned int            ue_idP,
  const imsi_t           *const imsi_pP,
  esm_proc_data_t        *proc_data_pP,
  esm_proc_pdn_request_t  request_typeP);

void nas_itti_auth_info_req(
  const uint32_t        ue_idP,
  const imsi64_t        imsi64_P,
  const bool            is_initial_reqP,
  plmn_t        * const visited_plmnP,
  const uint8_t         num_vectorsP,
  const_bstring   const auts_pP);

void nas_itti_establish_rej(
  const uint32_t      ue_idP,
  const imsi_t *const imsi_pP
  , uint8_t           initial_reqP);

void nas_itti_establish_cnf(
  const uint32_t         ue_idP,
  const nas_error_code_t error_codeP,
  bstring                msgP,
  const uint16_t         selected_encryption_algorithmP,
  const uint16_t         selected_integrity_algorithmP);

void nas_itti_detach_req(
  const uint32_t      ue_idP);


#endif /* FILE_NAS_ITTI_MESSAGING_SEEN */
