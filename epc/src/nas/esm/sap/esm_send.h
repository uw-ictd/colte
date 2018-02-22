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
Source      esm_send.h

Version     0.1

Date        2013/02/11

Product     NAS stack

Subsystem   EPS Session Management

Author      Frederic Maurel

Description Defines functions executed at the ESM Service Access
        Point to send EPS Session Management messages to the
        EPS Mobility Management sublayer.

*****************************************************************************/
#ifndef __ESM_SEND_H__
#define __ESM_SEND_H__

#include "common_defs.h"
#include "EsmStatus.h"

#include "PdnConnectivityReject.h"
#include "PdnDisconnectReject.h"
#include "BearerResourceAllocationReject.h"
#include "BearerResourceModificationReject.h"

#include "ActivateDefaultEpsBearerContextRequest.h"
#include "ActivateDedicatedEpsBearerContextRequest.h"
#include "ModifyEpsBearerContextRequest.h"
#include "DeactivateEpsBearerContextRequest.h"

#include "EsmInformationRequest.h"

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
 * Functions executed by both the UE and the MME to send ESM messages
 * --------------------------------------------------------------------------
 */
int esm_send_status(int pti, int ebi, esm_status_msg *msg, int esm_cause);


/*
 * --------------------------------------------------------------------------
 * Functions executed by the MME to send ESM message to the UE
 * --------------------------------------------------------------------------
 */
/*
 * Transaction related messages
 * ----------------------------
 */
int esm_send_pdn_connectivity_reject(int pti, pdn_connectivity_reject_msg *msg,
                                     int esm_cause);

int esm_send_pdn_disconnect_reject(int pti, pdn_disconnect_reject_msg *msg,
                                   int esm_cause);

/*
 * Messages related to EPS bearer contexts
 * ---------------------------------------
 */
int esm_send_activate_default_eps_bearer_context_request(int pti, int ebi,
    activate_default_eps_bearer_context_request_msg *msg, bstring apn,
    const ProtocolConfigurationOptions *pco, int pdn_type, bstring pdn_addr,
    const EpsQualityOfService *qos, int esm_cause);

int esm_send_activate_dedicated_eps_bearer_context_request(int pti, int ebi,
    activate_dedicated_eps_bearer_context_request_msg *msg, int linked_ebi,
    const EpsQualityOfService *qos, PacketFilters *pkfs, int n_pkfs);

int esm_send_deactivate_eps_bearer_context_request(int pti, int ebi,
    deactivate_eps_bearer_context_request_msg *msg, int esm_cause);

#endif /* __ESM_SEND_H__*/
