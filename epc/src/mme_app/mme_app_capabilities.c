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


/*! \file mme_app_capabilities.c
  \brief
  \author Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <stdlib.h>

#include "intertask_interface.h"
#include "mme_app_defs.h"
#include "assertions.h"
#include "dynamic_memory_check.h"

int
mme_app_handle_s1ap_ue_capabilities_ind (
  const itti_s1ap_ue_cap_ind_t const *s1ap_ue_cap_ind_pP)
{
  ue_context_t *ue_context_p = NULL;

  OAILOG_FUNC_IN (LOG_MME_APP);
  DevAssert (s1ap_ue_cap_ind_pP );

  ue_context_p = mme_ue_context_exists_mme_ue_s1ap_id(
    &mme_app_desc.mme_ue_contexts, s1ap_ue_cap_ind_pP->mme_ue_s1ap_id);
  if (!ue_context_p) {
    OAILOG_ERROR (
      LOG_MME_APP,
      "UE context doesn't exist for enb_ue_s1ap_ue_id " ENB_UE_S1AP_ID_FMT
      " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
      s1ap_ue_cap_ind_pP->enb_ue_s1ap_id, s1ap_ue_cap_ind_pP->mme_ue_s1ap_id);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }

  ue_context_p->ue_radio_cap_length =
    s1ap_ue_cap_ind_pP->radio_capabilities_length;

  if (ue_context_p->ue_radio_cap_length == 0) {
    OAILOG_ERROR (
      LOG_MME_APP,
      "Received UE Radio Capability len = 0 for enb_ue_s1ap_ue_id " ENB_UE_S1AP_ID_FMT
      " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT "\n",
      s1ap_ue_cap_ind_pP->enb_ue_s1ap_id, s1ap_ue_cap_ind_pP->mme_ue_s1ap_id);
    OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNerror);
  }
  DevAssert (s1ap_ue_cap_ind_pP->radio_capabilities);
  ue_context_p->ue_radio_capabilities =
    (uint8_t*)calloc(ue_context_p->ue_radio_cap_length,
                  sizeof *ue_context_p->ue_radio_capabilities);

  if (ue_context_p->ue_radio_capabilities) {
    memcpy(ue_context_p->ue_radio_capabilities,
           s1ap_ue_cap_ind_pP->radio_capabilities,
           ue_context_p->ue_radio_cap_length);
    OAILOG_DEBUG (LOG_MME_APP,
               "UE radio capabilities of length %d received and cached in ue context\n",
               ue_context_p->ue_radio_cap_length);
  }
  free_wrapper((void**) &(s1ap_ue_cap_ind_pP->radio_capabilities));

  OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNok);
}
