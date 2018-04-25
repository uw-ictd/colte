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
  Source      emm_main.c

  Version     0.1

  Date        2012/10/10

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines the EPS Mobility Management procedure call manager,
        the main entry point for elementary EMM processing.

*****************************************************************************/

#include "3gpp_24.007.h"
#include "emm_main.h"
#include "log.h"
#include "emmData.h"


#include "mme_config.h"
#include "obj_hashtable.h"
#include <string.h>


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
 ** Name:    emm_main_initialize()                                     **
 **                                                                        **
 ** Description: Initializes EMM internal data                             **
 **                                                                        **
 ** Inputs:  None                                                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    _emm_data                                  **
 **                                                                        **
 ***************************************************************************/
void
emm_main_initialize (
  mme_config_t * mme_config_p)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  /*
   * Retreive MME supported configuration data
   */
  memset(&_emm_data.conf, 0, sizeof(_emm_data.conf));
  if (mme_api_get_emm_config (&_emm_data.conf, mme_config_p) != RETURNok) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMM-MAIN  - Failed to get MME configuration data");
  }
  bstring b = bfromcstr("emm_data.ctx_coll_ue_id");
  _emm_data.ctx_coll_ue_id = hashtable_ts_create (mme_config.max_ues, NULL, NULL, b);
  btrunc(b, 0);
  bassigncstr(b, "emm_data.ctx_coll_imsi");
  _emm_data.ctx_coll_imsi  = hashtable_ts_create (mme_config.max_ues, NULL, hash_free_int_func, b);
  btrunc(b, 0);
  bassigncstr(b, "emm_data.ctx_coll_guti");
  _emm_data.ctx_coll_guti  = obj_hashtable_ts_create (mme_config.max_ues, NULL, NULL, hash_free_int_func, b);
  bdestroy(b);
  OAILOG_FUNC_OUT(LOG_NAS_EMM);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_main_cleanup()                                        **
 **                                                                        **
 ** Description: Performs the EPS Mobility Management clean up procedure   **
 **                                                                        **
 ** Inputs:  None                                                      **
 **          Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **          Return:    None                                       **
 **          Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
void
emm_main_cleanup (
  void)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  hashtable_ts_destroy(_emm_data.ctx_coll_ue_id);
  hashtable_ts_destroy(_emm_data.ctx_coll_imsi);
  obj_hashtable_ts_destroy(_emm_data.ctx_coll_guti);
  OAILOG_FUNC_OUT(LOG_NAS_EMM);
}



/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
