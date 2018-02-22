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
Source      esm_ebr.h

Version     0.1

Date        2013/01/29

Product     NAS stack

Subsystem   EPS Session Management

Author      Frederic Maurel

Description Defines functions used to handle state of EPS bearer contexts
        and manage ESM messages re-transmission.

*****************************************************************************/
#ifndef __ESM_EBR_H__
#define __ESM_EBR_H__

#include "bstrlib.h"
#include "nas_timer.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* Unassigned EPS bearer identity value */
#define ESM_EBI_UNASSIGNED  (EPS_BEARER_IDENTITY_UNASSIGNED)

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

int esm_ebr_is_reserved(int ebi);

void esm_ebr_initialize(void);
int esm_ebr_assign(emm_data_context_t *ctx, int ebi);
int esm_ebr_release(emm_data_context_t *ctx, int ebi);

int esm_ebr_start_timer(emm_data_context_t *ctx, int ebi, CLONE_REF const_bstring msg,
                        long sec, nas_timer_callback_t cb);
int esm_ebr_stop_timer(emm_data_context_t *ctx, int ebi);

int esm_ebr_get_pending_ebi(emm_data_context_t *ctx, esm_ebr_state status);

int esm_ebr_set_status(emm_data_context_t *ctx, int ebi, esm_ebr_state status,
                       int ue_requested);
esm_ebr_state esm_ebr_get_status(emm_data_context_t *ctx, int ebi);

int esm_ebr_is_not_in_use(emm_data_context_t *ctx, int ebi);

#endif /* __ESM_EBR_H__*/
