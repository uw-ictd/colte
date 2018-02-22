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
  Source      EmmDeregisteredInitiated.c

  Version     0.1

  Date        2012/10/03

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Implements the EPS Mobility Management procedures executed
        when the EMM-SAP is in EMM-DEREGISTERED-INITIATED state.

        In EMM-DEREGISTERED-INITIATED state, the UE has requested
        release of the EMM context by starting the detach or combined
        detach procedure and is waiting for a response from the MME.
        The MME has started a detach procedure and is waiting for a
        response from the UE.

*****************************************************************************/

#include "common_defs.h"
#include "emm_fsm.h"
#include "commonDef.h"
#include "log.h"

#include "emm_proc.h"

#include <assert.h>

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    EmmDeregisteredInitiated()                                **
 **                                                                        **
 ** Description: Handles the behaviour of the UE and the MME while the     **
 **      EMM-SAP is in EMM-DEREGISTERED-INITIATED state.           **
 **                                                                        **
 ** Inputs:  evt:       The received EMM-SAP event                 **
 **      Others:    emm_fsm_status                             **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    emm_fsm_status                             **
 **                                                                        **
 ***************************************************************************/
int
EmmDeregisteredInitiated (
  const emm_reg_t * evt)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;

  assert (emm_fsm_get_status (evt->ue_id, evt->ctx) == EMM_DEREGISTERED_INITIATED);

  switch (evt->primitive) {

    case _EMMREG_DETACH_CNF:
      rc = emm_fsm_set_status (evt->ue_id, evt->ctx, EMM_DEREGISTERED);
      break;

    case _EMMREG_LOWERLAYER_FAILURE:
      /*
       * Transmission failure occurred before the EMM common
       * procedure being completed
       */
      rc = emm_fsm_set_status (evt->ue_id, evt->ctx, EMM_DEREGISTERED);
      break;

    case _EMMREG_LOWERLAYER_NON_DELIVERY:
      rc = emm_fsm_set_status (evt->ue_id, evt->ctx, EMM_DEREGISTERED);
      break;
    
    case _EMMREG_DETACH_REQ:
      rc = emm_fsm_set_status (evt->ue_id, evt->ctx, EMM_DEREGISTERED);
      break;

    default:
      OAILOG_ERROR (LOG_NAS_EMM, "EMM-FSM   - Primitive is not valid (%d)\n", evt->primitive);
      break;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
