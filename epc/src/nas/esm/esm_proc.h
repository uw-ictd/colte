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
Source      esm_proc.h

Version     0.1

Date        2013/01/02

Product     NAS stack

Subsystem   EPS Session Management

Author      Frederic Maurel

Description Defines the EPS Session Management procedures executed at
        the ESM Service Access Points.

*****************************************************************************/
#ifndef __ESM_PROC_H__
#define __ESM_PROC_H__

#include <stdbool.h>
#include "bstrlib.h"
#include "networkDef.h"
#include "emmData.h"
#include "ProtocolConfigurationOptions.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/*
 * ESM retransmission timers
 * -------------------------
 */

/* Type of PDN address */
typedef enum {
  ESM_PDN_TYPE_IPV4 = NET_PDN_TYPE_IPV4,
  ESM_PDN_TYPE_IPV6 = NET_PDN_TYPE_IPV6,
  ESM_PDN_TYPE_IPV4V6 = NET_PDN_TYPE_IPV4V6
} esm_proc_pdn_type_t;

/* Type of PDN request */
typedef enum {
  ESM_PDN_REQUEST_INITIAL = 1,
  ESM_PDN_REQUEST_HANDOVER,
  ESM_PDN_REQUEST_EMERGENCY
} esm_proc_pdn_request_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/*
 * Type of the ESM procedure callback executed when requested by the UE
 * or initiated by the network
 */
typedef int (*esm_proc_procedure_t) (bool, emm_data_context_t *, int, bstring, bool);

/* EPS bearer level QoS parameters */
typedef network_qos_t esm_proc_qos_t;

/* Traffic Flow Template for packet filtering */
typedef network_tft_t esm_proc_tft_t;

typedef ProtocolConfigurationOptions esm_proc_pco_t;

/* PDN connection and EPS bearer context data */
typedef struct {
  bstring             apn;
  esm_proc_pdn_type_t pdn_type;
  bstring             pdn_addr;
  esm_proc_qos_t      qos;
  esm_proc_tft_t      tft;
  esm_proc_pco_t      pco;
} esm_proc_data_t;

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
 * --------------------------------------------------------------------------
 *              ESM status procedure
 * --------------------------------------------------------------------------
 */


int esm_proc_status_ind(emm_data_context_t *ctx, const int pti, int ebi, int *esm_cause);
int esm_proc_status(bool is_standalone, emm_data_context_t *ctx, const int pti,
    bstring msg, bool sent_by_ue);


/*
 * --------------------------------------------------------------------------
 *          PDN connectivity procedure
 * --------------------------------------------------------------------------
 */

int esm_proc_pdn_connectivity_request(emm_data_context_t *ctx, const int pti,
                                     const  esm_proc_pdn_request_t request_type,
                                     const_bstring const apn, esm_proc_pdn_type_t pdn_type,
                                     const_bstring const pdn_addr, esm_proc_qos_t *esm_qos,
                                     int *esm_cause);

int esm_proc_pdn_connectivity_reject(bool is_standalone, emm_data_context_t *ctx,
                                     int ebi, bstring msg, bool ue_triggered);
int esm_proc_pdn_connectivity_failure(emm_data_context_t *ctx, int pid);

/*
 * --------------------------------------------------------------------------
 *              PDN disconnect procedure
 * --------------------------------------------------------------------------
 */

int esm_proc_pdn_disconnect_request(emm_data_context_t *ctx, const int pti, int *esm_cause);

int esm_proc_pdn_disconnect_accept(emm_data_context_t *ctx, int pid, int *esm_cause);
int esm_proc_pdn_disconnect_reject(const bool is_standalone, emm_data_context_t *ctx,
                                   int ebi, bstring msg, const bool ue_triggered);

/*
 * --------------------------------------------------------------------------
 *      Default EPS bearer context activation procedure
 * --------------------------------------------------------------------------
 */
int esm_proc_default_eps_bearer_context(emm_data_context_t *ctx, int pid,
                                        unsigned int *ebi, const esm_proc_qos_t *esm_qos, int *esm_cause);
int esm_proc_default_eps_bearer_context_request(bool is_standalone,
    emm_data_context_t *ctx, int ebi, STOLEN_REF bstring *msg, bool ue_triggered);
int esm_proc_default_eps_bearer_context_failure(emm_data_context_t *ctx);

int esm_proc_default_eps_bearer_context_accept(emm_data_context_t *ctx, int ebi,
    int *esm_cause);
int esm_proc_default_eps_bearer_context_reject(emm_data_context_t *ctx, int ebi,
    int *esm_cause);


/*
 * --------------------------------------------------------------------------
 *      Dedicated EPS bearer context activation procedure
 * --------------------------------------------------------------------------
 */
int esm_proc_dedicated_eps_bearer_context(emm_data_context_t *ctx, int pid,
    unsigned int *ebi, unsigned int *default_ebi, const esm_proc_qos_t *qos,
    const esm_proc_tft_t *tft, int *esm_cause);
int esm_proc_dedicated_eps_bearer_context_request(bool is_standalone,
    emm_data_context_t *ctx, int ebi, STOLEN_REF bstring *msg, bool ue_triggered);

int esm_proc_dedicated_eps_bearer_context_accept(emm_data_context_t *ctx, int ebi,
    int *esm_cause);
int esm_proc_dedicated_eps_bearer_context_reject(emm_data_context_t *ctx, int ebi,
    int *esm_cause);


/*
 * --------------------------------------------------------------------------
 *      EPS bearer context deactivation procedure
 * --------------------------------------------------------------------------
 */
int esm_proc_eps_bearer_context_deactivate(emm_data_context_t *ctx, bool is_local,
    int ebi, int *pid, int *bid,
    int *esm_cause);
int esm_proc_eps_bearer_context_deactivate_request(bool is_standalone,
    emm_data_context_t *ctx, int ebi, bstring msg, bool ue_triggered);
int esm_proc_eps_bearer_context_deactivate_accept(emm_data_context_t *ctx, int ebi,
    int *esm_cause);



#endif /* __ESM_PROC_H__*/
