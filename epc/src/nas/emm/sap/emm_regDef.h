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

Source      emm_regDef.h

Version     0.1

Date        2012/10/16

Product     NAS stack

Subsystem   EPS Mobility Management

Author      Frederic Maurel

Description Defines the EMMREG Service Access Point that provides
        registration services for location updating and attach/detach
        procedures.

*****************************************************************************/
#ifndef FILE_EMM_REGDEF_SEEN
#define FILE_EMM_REGDEF_SEEN

#include "common_types.h"
#include "commonDef.h"
#include <stdbool.h>

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/*
 * EMMREG-SAP primitives
 */
typedef enum {
  _EMMREG_START = 0,
  _EMMREG_COMMON_PROC_REQ,    /* EMM common procedure requested   */
  _EMMREG_COMMON_PROC_CNF,    /* EMM common procedure successful  */
  _EMMREG_COMMON_PROC_REJ,    /* EMM common procedure failed      */
  _EMMREG_PROC_ABORT,         /* EMM procedure aborted            */
  _EMMREG_ATTACH_CNF,         /* EPS network attach accepted      */
  _EMMREG_ATTACH_REJ,         /* EPS network attach rejected      */
  _EMMREG_DETACH_INIT,        /* Network detach initiated         */
  _EMMREG_DETACH_REQ,         /* Network detach requested         */
  _EMMREG_DETACH_FAILED,      /* Network detach attempt failed    */
  _EMMREG_DETACH_CNF,         /* Network detach accepted          */
  _EMMREG_TAU_REQ,
  _EMMREG_TAU_CNF,
  _EMMREG_TAU_REJ,
  _EMMREG_SERVICE_REQ,
  _EMMREG_SERVICE_CNF,
  _EMMREG_SERVICE_REJ,
  _EMMREG_LOWERLAYER_SUCCESS, /* Data successfully delivered      */
  _EMMREG_LOWERLAYER_FAILURE, /* Lower layer failure indication   */
  _EMMREG_LOWERLAYER_RELEASE, /* NAS signalling connection released   */
  _EMMREG_LOWERLAYER_NON_DELIVERY, /*  remote Lower layer failure indication  */
  _EMMREG_END
} emm_reg_primitive_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/


/*
 * EMMREG primitive for attach procedure
 * -------------------------------------
 */
typedef struct {
  bool is_emergency;   /* true if the UE was attempting to register to
             * the network for emergency services only  */
} emm_reg_attach_t;

/*
 * EMMREG primitive for detach procedure
 * -------------------------------------
 */
typedef struct {
  bool switch_off; /* true if the UE is switched off       */
  int type;       /* Network detach type              */
} emm_reg_detach_t;

/*
 * EMMREG primitive for EMM common procedures
 * ------------------------------------------
 */
typedef struct {
  bool is_attached;    /* UE context attach indicator          */
} emm_reg_common_t;

/*
 * Structure of EMMREG-SAP primitive
 */
typedef struct {
  emm_reg_primitive_t primitive;
  mme_ue_s1ap_id_t    ue_id;
  void               *ctx;

  union {
    emm_reg_attach_t    attach;
    emm_reg_detach_t    detach;
    emm_reg_common_t    common;
  } u;
} emm_reg_t;

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif /* FILE_EMM_REGDEF_SEEN*/
