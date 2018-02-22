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


/*! \file s6a_peers.c
   \brief Authenticate a new peer connecting to the HSS by checking the database
   \author Sebastien ROUX <sebastien.roux@eurecom.fr>
   \date 2013
   \version 0.1
*/

#include <stdio.h>
#include <string.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"
#include "log.h"

int
s6a_peer_validate (
  struct peer_info *info,
  int *auth,
  int                                     (**cb2) (struct peer_info *))
{
  mysql_mme_identity_t                    mme_identity;

  if (info == NULL) {
    return EINVAL;
  }

  memset (&mme_identity, 0, sizeof (mysql_mme_identity_t));
  /*
   * We received a new connection. Check the database for allowed equipments
   * * * * on EPC
   */
  memcpy (mme_identity.mme_host, info->pi_diamid, info->pi_diamidlen);

  if (hss_mysql_check_epc_equipment (&mme_identity) != 0) {
    /*
     * The MME has not been found in list of known peers -> reject it
     */
    *auth = -1;
    FPRINTF_NOTICE ( "Rejecting %s: either db has no knowledge of this peer " "or sql query failed\n", info->pi_diamid);
  } else {
    *auth = 1;
    /*
     * For now we don't use security
     */
    info->config.pic_flags.sec = PI_SEC_NONE;
    info->config.pic_flags.persist = PI_PRST_NONE;
    FPRINTF_NOTICE ( "Accepting %s peer\n", info->pi_diamid);
  }

  return 0;
}
