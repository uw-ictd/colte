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

/*****************************************************************************
  Source      esm_data_context.c

  Version     0.1

  Date        2016/01/29

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Lionel Gauthier


*****************************************************************************/
#include "3gpp_24.007.h"
#include "emmData.h"
#include "nas_timer.h"
#include "esmData.h"
#include "commonDef.h"
#include "networkDef.h"
#include "log.h"
#include "esm_ebr_context.h"
#include "dynamic_memory_check.h"

 // free allocated structs
 void free_esm_bearer(esm_bearer_t * bearer)
 {
   if (bearer) {
     unsigned int i;
     for (i=0; i < NET_PACKET_FILTER_MAX; i++) {
       free_wrapper((void**) &bearer->tft.pkf[i]);
     }
     free_wrapper((void**) &bearer);
   }
 }


// free allocated structs
void free_esm_pdn(esm_pdn_t * pdn)
{
  if (pdn) {
    bdestroy(pdn->apn);
    unsigned int i;
    for (i=0; i < ESM_DATA_EPS_BEARER_MAX; i++) {
      free_esm_bearer(pdn->bearer[i]);
    }
    free_wrapper((void**) &pdn);
  }
}

// free allocated structs
void free_esm_data_context(esm_data_context_t * esm_data_ctx)
{

  if (esm_data_ctx) {
    unsigned int i;
    for (i=0; i < (ESM_DATA_PDN_MAX+1); i++) {
      free_esm_pdn(esm_data_ctx->pdn[i].data);
    }

    for (i=0; i < (ESM_EBR_DATA_SIZE + 1); i++) {
      free_esm_ebr_context(esm_data_ctx->ebr.context[i]);
    }
    //free_wrapper(esm_data_ctx);
  }
}
