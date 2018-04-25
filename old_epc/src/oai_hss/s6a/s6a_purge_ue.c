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


/*! \file s6a_purge_ue.c
   \brief Handle a purge UE request and generate the corresponding answer
   \author Sebastien ROUX <sebastien.roux@eurecom.fr>
   \date 2013
   \version 0.1
*/

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdproto.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"

int
s6a_purge_ue_cb (
  struct msg **msg,
  struct avp *paramavp,
  struct session *sess,
  void *opaque,
  enum disp_action *act)
{
  struct msg                             *ans,
                                         *qry;
  struct avp                             *avp,
                                         *failed_avp = NULL;
  struct avp_hdr                         *hdr;
  int                                     ret = 0;
  int                                     result_code = ER_DIAMETER_SUCCESS;
  int                                     experimental = 0;
  uint32_t                                pur_flags = 0;

  /*
   * MySQL requests and asnwer data
   */
  mysql_pu_req_t                          pu_req;
  mysql_pu_ans_t                          pu_ans;

  if (msg == NULL) {
    return EINVAL;
  }

  memset (&pu_req, 0, sizeof (mysql_pu_req_t));
  qry = *msg;
  /*
   * Create the answer
   */
  CHECK_FCT (fd_msg_new_answer_from_req (fd_g_config->cnf_dict, msg, 0));
  ans = *msg;
  /*
   * Retrieving IMSI AVP
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_imsi, &avp));

  if (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    if (hdr->avp_value->os.len > IMSI_LENGTH) {
      result_code = ER_DIAMETER_INVALID_AVP_VALUE;
      goto out;
    }

    sprintf (pu_req.imsi, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  } else {
    result_code = ER_DIAMETER_MISSING_AVP;
    goto out;
  }

  /*
   * Retrieving the PUR-Flags if present
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_pur_flags, &avp));

  if (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
    pur_flags = hdr->avp_value->u32;

    if (FLAG_IS_SET (pur_flags, PUR_UE_PURGED_IN_SGSN)) {
      /*
       * This bit shall not be set by a standalone MME.
       */
      result_code = ER_DIAMETER_INVALID_AVP_VALUE;
      goto out;
    }
  }

  if ((ret = hss_mysql_purge_ue (&pu_req, &pu_ans)) != 0) {
    /*
     * We failed to find the IMSI in the database. Replying to the request
     * * * * with the user unknown cause.
     */
    experimental = 1;
    result_code = DIAMETER_ERROR_USER_UNKNOWN;
    goto out;
  }

  /*
   * Retrieving Origin host AVP
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_origin_host, &avp));

  if (!avp) {
    result_code = ER_DIAMETER_MISSING_AVP;
    goto out;
  }

  /*
   * Retrieve the header from origin host and realm avps
   */
  CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

  if (strncmp (pu_ans.mme_host, (char *)hdr->avp_value->os.data, hdr->avp_value->os.len) != 0) {
    result_code = DIAMETER_ERROR_UNKOWN_SERVING_NODE;
    experimental = 1;
    goto out;
  }

  /*
   * Retrieving Origin realm AVP
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_origin_realm, &avp));

  if (!avp) {
    result_code = ER_DIAMETER_MISSING_AVP;
    goto out;
  }

  CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

  if (strncmp (pu_ans.mme_realm, (char *)hdr->avp_value->os.data, hdr->avp_value->os.len) != 0) {
    result_code = DIAMETER_ERROR_UNKOWN_SERVING_NODE;
    experimental = 1;
    goto out;
  }

out:
  /*
   * Append the result code to the answer
   */
  CHECK_FCT (s6a_add_result_code (ans, failed_avp, result_code, experimental));
  CHECK_FCT (fd_msg_send (msg, NULL, NULL));
  return 0;
}
