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
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#include "bstrlib.h"

#include "log.h"
#include "intertask_interface.h"
#include "mme_config.h"
#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "assertions.h"
#include "common_types.h"
#include "common_defs.h"
#include "dynamic_memory_check.h"

int
mme_app_handle_s1ap_ue_capabilities_ind (
  const itti_s1ap_ue_cap_ind_t const *s1ap_ue_cap_ind_pP)
{
  ue_mm_context_t *ue_context_p = NULL;

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

  if (ue_context_p->ue_radio_capability) {
    bdestroy_wrapper(&ue_context_p->ue_radio_capability);
  }

  // Allocate the radio capabilities memory. Note that this takes care of the
  // length = 0 case for us quite nicely.
  ue_context_p->ue_radio_capability = blk2bstr(s1ap_ue_cap_ind_pP->radio_capabilities, s1ap_ue_cap_ind_pP->radio_capabilities_length);

  OAILOG_DEBUG (LOG_MME_APP,
               "UE radio capabilities of length %d found and cached\n",
               ue_context_p->ue_radio_capability->slen);

  unlock_ue_contexts(ue_context_p);
  OAILOG_FUNC_RETURN (LOG_MME_APP, RETURNok);
}
