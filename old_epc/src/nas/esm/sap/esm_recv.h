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
Source      esm_recv.h

Version     0.1

Date        2013/02/06

Product     NAS stack

Subsystem   EPS Session Management

Author      Frederic Maurel

Description Defines functions executed at the ESM Service Access
        Point upon receiving EPS Session Management messages
        from the EPS Mobility Management sublayer.

*****************************************************************************/
#ifndef __ESM_RECV_H__
#define __ESM_RECV_H__

#include "EsmStatus.h"
#include "emmData.h"


#include "PdnConnectivityRequest.h"
#include "PdnDisconnectRequest.h"
#include "BearerResourceAllocationRequest.h"
#include "BearerResourceModificationRequest.h"

#include "ActivateDefaultEpsBearerContextAccept.h"
#include "ActivateDefaultEpsBearerContextReject.h"
#include "ActivateDedicatedEpsBearerContextAccept.h"
#include "ActivateDedicatedEpsBearerContextReject.h"
#include "ModifyEpsBearerContextAccept.h"
#include "ModifyEpsBearerContextReject.h"
#include "DeactivateEpsBearerContextAccept.h"

#include "EsmInformationResponse.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 * Functions executed by both the UE and the MME upon receiving ESM messages
 * --------------------------------------------------------------------------
 */

int esm_recv_status(emm_data_context_t *ctx, int pti, int ebi,
                    const esm_status_msg *msg);

/*
 * --------------------------------------------------------------------------
 * Functions executed by the MME upon receiving ESM message from the UE
 * --------------------------------------------------------------------------
 */
/*
 * Transaction related messages
 * ----------------------------
 */
int esm_recv_pdn_connectivity_request(emm_data_context_t *ctx, int pti, int ebi,
                                      const pdn_connectivity_request_msg *msg, unsigned int *new_ebi, void *data);

int esm_recv_pdn_disconnect_request(emm_data_context_t *ctx, int pti, int ebi,
                                    const pdn_disconnect_request_msg *msg,
                                    unsigned int *linked_ebi);

/*
 * Messages related to EPS bearer contexts
 * ---------------------------------------
 */
int esm_recv_activate_default_eps_bearer_context_accept(emm_data_context_t *ctx,
    int pti, int ebi, const activate_default_eps_bearer_context_accept_msg *msg);

int esm_recv_activate_default_eps_bearer_context_reject(emm_data_context_t *ctx,
    int pti, int ebi, const activate_default_eps_bearer_context_reject_msg *msg);

int esm_recv_activate_dedicated_eps_bearer_context_accept(emm_data_context_t *ctx,
    int pti, int ebi, const activate_dedicated_eps_bearer_context_accept_msg *msg);

int esm_recv_activate_dedicated_eps_bearer_context_reject(emm_data_context_t *ctx,
    int pti, int ebi, const activate_dedicated_eps_bearer_context_reject_msg *msg);

int esm_recv_deactivate_eps_bearer_context_accept(emm_data_context_t *ctx, int pti,
    int ebi, const deactivate_eps_bearer_context_accept_msg *msg);

#endif /* __ESM_RECV_H__*/
