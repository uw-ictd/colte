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

Source      emm_fsm.h

Version     0.1

Date        2012/10/03

Product     NAS stack

Subsystem   EPS Mobility Management

Author      Frederic Maurel

Description Defines the EPS Mobility Management procedures executed at
        the EMMREG Service Access Point.

*****************************************************************************/
#ifndef FILE_EMM_FSM_SEEN
#define FILE_EMM_FSM_SEEN

#include "emm_regDef.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/*
 * States of the EPS Mobility Management sublayer
 * ----------------------------------------------
 * The EMM protocol of the UE and the network is described by means of two
 * different state machines.
 */
typedef enum {
  EMM_STATE_MIN = 0,
  EMM_INVALID = EMM_STATE_MIN,
  EMM_DEREGISTERED,
  EMM_REGISTERED,
  EMM_DEREGISTERED_INITIATED,
  EMM_COMMON_PROCEDURE_INITIATED,
  EMM_STATE_MAX
} emm_fsm_state_t;

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

void emm_fsm_initialize(void);

//LG 2015-03-13 struct emm_data_context_t;
int emm_fsm_set_status(mme_ue_s1ap_id_t ueid, void *ctx, emm_fsm_state_t status);
emm_fsm_state_t emm_fsm_get_status(mme_ue_s1ap_id_t ueid, void *ctx);

int emm_fsm_process(const emm_reg_t *evt);

#endif /* FILE_EMM_FSM_SEEN*/
