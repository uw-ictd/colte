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
  Source      esm_ebr_context.h

  Version     0.1

  Date        2013/05/28

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Frederic Maurel

  Description Defines functions used to handle EPS bearer contexts.

*****************************************************************************/
#include <stdlib.h>             // malloc, free_wrapper
#include <string.h>             // memset

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "log.h"
#include "3gpp_24.007.h"
#include "commonDef.h"
#include "emmData.h"
#include "esm_ebr.h"
#include "esm_ebr_context.h"
#include "emm_sap.h"


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
 ** Name:    esm_ebr_context_create()                                  **
 **                                                                        **
 ** Description: Creates a new EPS bearer context to the PDN with the spe- **
 **      cified PDN connection identifier                          **
 **                                                                        **
 ** Inputs:  ue_id:      UE identifier                              **
 **      pid:       PDN connection identifier                  **
 **      ebi:       EPS bearer identity                        **
 **      is_default:    true if the new bearer is a default EPS    **
 **             bearer context                             **
 **      esm_qos:   EPS bearer level QoS parameters            **
 **      tft:       Traffic flow template parameters           **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    The EPS bearer identity of the default EPS **
 **             bearer associated to the new EPS bearer    **
 **             context if successfully created;           **
 **             UNASSIGN EPS bearer value otherwise.       **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_context_create (
  emm_data_context_t * ctx,
  int pid,
  int ebi,
  int is_default,
  const network_qos_t * qos,
  const network_tft_t * tft)
{
  int                                     bid = 0;
  esm_data_context_t                     *esm_ctx = NULL;
  esm_pdn_t                              *pdn = NULL;

  OAILOG_FUNC_IN (LOG_NAS_ESM);
  esm_ctx = &ctx->esm_data_ctx;

  bid = ESM_DATA_EPS_BEARER_MAX;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-PROC  - Create new %s EPS bearer context (ebi=%d) " "for PDN connection (pid=%d)\n",
      (is_default) ? "default" : "dedicated", ebi, pid);

  if (pid < ESM_DATA_PDN_MAX) {
    if (pid != esm_ctx->pdn[pid].pid) {
      OAILOG_ERROR(LOG_NAS_ESM , "ESM-PROC  - PDN connection identifier %d is " "not valid\n", pid);
    } else if (esm_ctx->pdn[pid].data == NULL) {
      OAILOG_ERROR(LOG_NAS_ESM , "ESM-PROC  - PDN connection %d has not been " "allocated\n", pid);
    }
    /*
     * Check the total number of active EPS bearers
     */
    else if (esm_ctx->n_ebrs > ESM_DATA_EPS_BEARER_TOTAL) {
      OAILOG_WARNING (LOG_NAS_ESM , "ESM-PROC  - The total number of active EPS" "bearers is exceeded\n");
    } else {
      /*
       * Get the PDN connection entry
       */
      pdn = esm_ctx->pdn[pid].data;

      if (is_default) {
        /*
         * Default EPS bearer entry is defined at index 0
         */
        bid = 0;

        if (pdn->bearer[bid] ) {
          OAILOG_ERROR(LOG_NAS_ESM , "ESM-PROC  - A default EPS bearer context " "is already allocated\n");
          OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
        }
      } else {
        /*
         * Search for an available EPS bearer context entry
         */
        for (bid = 1; bid < ESM_DATA_EPS_BEARER_MAX; bid++) {
          if (pdn->bearer[bid] ) {
            continue;
          }

          break;
        }
      }
    }
  }

  if (bid < ESM_DATA_EPS_BEARER_MAX) {
    /*
     * Create new EPS bearer context
     */
    esm_bearer_t                           *ebr = (esm_bearer_t *) malloc (sizeof (esm_bearer_t));

    if (ebr ) {
      memset (ebr, 0, sizeof (esm_bearer_t));
      /*
       * Increment the total number of active EPS bearers
       */
      esm_ctx->n_ebrs += 1;
      /*
       * Increment the number of EPS bearer for this PDN connection
       */
      pdn->n_bearers += 1;
      /*
       * Setup the EPS bearer data
       */
      pdn->bearer[bid] = ebr;
      ebr->bid = bid;
      ebr->ebi = ebi;

      if (qos ) {
        /*
         * EPS bearer level QoS parameters
         */
        ebr->qos = *qos;
      }

      if ((tft ) && (tft->n_pkfs < NET_PACKET_FILTER_MAX)) {
        int                                     i;

        /*
         * Traffic flow template parameters
         */
        for (i = 0; i < tft->n_pkfs; i++) {
          ebr->tft.pkf[i] = (network_pkf_t *) malloc (sizeof (network_pkf_t));

          if (ebr->tft.pkf[i] ) {
            *(ebr->tft.pkf[i]) = *(tft->pkf[i]);
            ebr->tft.n_pkfs += 1;
          }
        }
      }

      if (is_default) {
        /*
         * Set the PDN connection activation indicator
         */
        esm_ctx->pdn[pid].is_active = true;

        /*
         * Update the emergency bearer services indicator
         */
        if (pdn->is_emergency) {
          esm_ctx->is_emergency = true;
        }
      }

      /*
       * Return the EPS bearer identity of the default EPS bearer
       * * * * associated to the new EPS bearer context
       */
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, pdn->bearer[0]->ebi);
    }

    OAILOG_WARNING (LOG_NAS_ESM , "ESM-PROC  - Failed to create new EPS bearer " "context (ebi=%d)\n", ebi);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_ebr_context_release()                                 **
 **                                                                        **
 ** Description: Releases EPS bearer context entry previously allocated    **
 **      to the EPS bearer with the specified EPS bearer identity  **
 **                                                                        **
 ** Inputs:  ue_id:      UE identifier                              **
 **      ebi:       EPS bearer identity                        **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ** Outputs:     pid:       Identifier of the PDN connection entry the **
 **             EPS bearer context belongs to              **
 **      bid:       Identifier of the released EPS bearer con- **
 **             text entry                                 **
 **      Return:    The EPS bearer identity associated to the  **
 **             EPS bearer context if successfully relea-  **
 **             sed; UNASSIGN EPS bearer value otherwise.  **
 **      Others:    _esm_data                                  **
 **                                                                        **
 ***************************************************************************/
int
esm_ebr_context_release (
  emm_data_context_t * ctx,
  int ebi,
  int *pid,
  int *bid)
{
  int                                     found = false;
  esm_pdn_t                              *pdn = NULL;
  esm_data_context_t                     *esm_ctx;

  OAILOG_FUNC_IN (LOG_NAS_ESM);
  esm_ctx = &ctx->esm_data_ctx;

  if (ebi != ESM_EBI_UNASSIGNED) {
    /*
     * The identity of the EPS bearer to released is given;
     * Release the EPS bearer context entry that match the specified EPS
     * bearer identity
     */

    /*
     * Search for active PDN connection
     */
    for (*pid = 0; *pid < ESM_DATA_PDN_MAX; (*pid)++) {
      if (!esm_ctx->pdn[*pid].data) {
        continue;
      }

      /*
       * An active PDN connection is found
       */
      if (esm_ctx->pdn[*pid].data ) {
        pdn = esm_ctx->pdn[*pid].data;

        /*
         * Search for the specified EPS bearer context entry
         */
        for (*bid = 0; *bid < pdn->n_bearers; (*bid)++) {
          if (pdn->bearer[*bid] ) {
            if (pdn->bearer[*bid]->ebi != ebi) {
              continue;
            }

            /*
             * The EPS bearer context entry is found
             */
            found = true;
            break;
          }
        }
      }

      if (found) {
        break;
      }
    }
  } else {
    /*
     * The identity of the EPS bearer to released is not given;
     * Release the EPS bearer context entry allocated with the EPS
     * bearer context identifier (bid) to establish connectivity to
     * the PDN identified by the PDN connection identifier (pid).
     * Default EPS bearer to a given PDN is always identified by the
     * first EPS bearer context entry at index bid = 0
     */
    if (*pid < ESM_DATA_PDN_MAX) {
      if (*pid != esm_ctx->pdn[*pid].pid) {
        OAILOG_ERROR(LOG_NAS_ESM , "ESM-PROC  - PDN connection identifier %d " "is not valid\n", *pid);
      } else if (!esm_ctx->pdn[*pid].is_active) {
        OAILOG_WARNING (LOG_NAS_ESM , "ESM-PROC  - PDN connection %d is not active\n", *pid);
      } else if (esm_ctx->pdn[*pid].data == NULL) {
        OAILOG_ERROR(LOG_NAS_ESM , "ESM-PROC  - PDN connection %d has not been " "allocated\n", *pid);
      } else {
        pdn = esm_ctx->pdn[*pid].data;

        if (pdn->bearer[*bid] ) {
          ebi = pdn->bearer[*bid]->ebi;
          found = true;
        }
      }
    }
  }

  if (found) {
    int                                     i,
                                            j;

    /*
     * Delete the specified EPS bearer context entry
     */
    if (pdn->bearer[*bid]->bid != *bid) {
      OAILOG_ERROR(LOG_NAS_ESM , "ESM-PROC  - EPS bearer identifier %d is " "not valid\n", *bid);
      OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
    }

    OAILOG_WARNING (LOG_NAS_ESM, "ESM-PROC  - Release EPS bearer context " "(ebi=%d)\n", ebi);

    /*
     * Delete the TFT
     */
    for (i = 0; i < pdn->bearer[*bid]->tft.n_pkfs; i++) {
      free_wrapper ((void**) &pdn->bearer[*bid]->tft.pkf[i]);
    }

    /*
     * Release the specified EPS bearer data
     */
    free_wrapper ((void**) &pdn->bearer[*bid]);
    pdn->bearer[*bid] = NULL;
    /*
     * Decrement the number of EPS bearer context allocated
     * * * * to the PDN connection
     */
    pdn->n_bearers -= 1;
    /*
     * Decrement the total number of active EPS bearers
     */
    esm_ctx->n_ebrs -= 1;

    if (*bid == 0) {
      /*
       * 3GPP TS 24.301, section 6.4.4.3, 6.4.4.6
       * * * * If the EPS bearer identity is that of the default bearer to a
       * * * * PDN, the UE shall delete all EPS bearer contexts associated to
       * * * * that PDN connection.
       */
      for (i = 1; pdn->n_bearers > 0; i++) {
        if (pdn->bearer[i]) {
          OAILOG_WARNING (LOG_NAS_ESM , "ESM-PROC  - Release EPS bearer context " "(ebi=%d)\n", pdn->bearer[i]->ebi);

          /*
           * Delete the TFT
           */
          for (j = 0; j < pdn->bearer[i]->tft.n_pkfs; j++) {
            free_wrapper ((void**) &pdn->bearer[i]->tft.pkf[j]);
          }

          /*
           * Set the EPS bearer context state to INACTIVE
           */
          (void)esm_ebr_set_status (ctx, pdn->bearer[i]->ebi, ESM_EBR_INACTIVE, true);
          /*
           * Release EPS bearer data
           */
          (void)esm_ebr_release (ctx, pdn->bearer[i]->ebi);
          // esm_ebr_release()
          /*
           * Release dedicated EPS bearer data
           */
          free_wrapper ((void**) &pdn->bearer[i]);
          pdn->bearer[i] = NULL;
          /*
           * Decrement the number of EPS bearer context allocated
           * * * * to the PDN connection
           */
          pdn->n_bearers -= 1;
          /*
           * Decrement the total number of active EPS bearers
           */
          esm_ctx->n_ebrs -= 1;
        }
      }

      /*
       * Reset the PDN connection activation indicator
       */
      esm_ctx->pdn[*pid].is_active = false;

      /*
       * Update the emergency bearer services indicator
       */
      if (pdn->is_emergency) {
        esm_ctx->is_emergency = false;
      }
    }

    if (esm_ctx->n_ebrs == 0) {
      /*
       * TODO: Release the PDN connection and marked the UE as inactive
       * * * * in the network for EPS services (is_attached = false)
       */
    }

    OAILOG_FUNC_RETURN (LOG_NAS_ESM, ebi);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_ESM, ESM_EBI_UNASSIGNED);
}


// free structs only
void free_esm_ebr_context(esm_ebr_context_t * ctx)
{
  if (ctx) {
    if (ctx->args) {
      bdestroy(ctx->args->msg);
      // do not free ctx->args->ctx
    }
    free_wrapper((void**) &ctx);
  }
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
