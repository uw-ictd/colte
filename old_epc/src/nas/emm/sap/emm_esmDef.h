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
Source      emm_esmDef.h

Version     0.1

Date        2012/10/16

Product     NAS stack

Subsystem   EPS Mobility Management

Author      Frederic Maurel

Description Defines the EMMESM Service Access Point that provides
        interlayer services to the EPS Session Management sublayer
        for service registration and activate/deactivate PDN
        connections.

*****************************************************************************/
#ifndef FILE_EMM_ESMDEF_SEEN
#define FILE_EMM_ESMDEF_SEEN

#include <stdbool.h>
#include "bstrlib.h"
#include "emmData.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/*
 * EMMESM-SAP primitives
 */
typedef enum {
  _EMMESM_START = 100,
  _EMMESM_RELEASE_IND,
  _EMMESM_UNITDATA_REQ,
  _EMMESM_UNITDATA_IND,
  _EMMESM_END
} emm_esm_primitive_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/*
 * EMMESM primitive for connection establishment
 * ---------------------------------------------
 */
typedef struct {
  bool is_emergency;   /* Indicates whether the PDN connection is established
             * for emergency bearer services only       */
  bool is_attached;    /* Indicates whether the UE remains attached to the
             * network                  */
} emm_esm_establish_t;

/*
 * EMMESM primitive for data transfer
 * ----------------------------------
 */
typedef struct {
  bstring          msg;        /* ESM message to be transfered     */
} emm_esm_data_t;

/*
 * ---------------------------------
 * Structure of EMMESM-SAP primitive
 * ---------------------------------
 */
typedef struct {
  emm_esm_primitive_t primitive;
  mme_ue_s1ap_id_t    ue_id;
  emm_data_context_t *ctx;
  union {
    emm_esm_establish_t establish;
    emm_esm_data_t data;
  } u;
  /* TODO: complete emm_esm_t structure definition */
} emm_esm_t;

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif /* FILE_EMM_ESMDEF_SEEN*/
