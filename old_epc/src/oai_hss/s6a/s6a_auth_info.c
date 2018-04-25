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


/*! \file s6a_auth_info.c
   \brief Handle an authentication information request procedure and generate the answer
   \author Sebastien ROUX <sebastien.roux@eurecom.fr>
   \date 2013
   \version 0.1
*/

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdproto.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"
#include "auc.h"
#include "access_restriction.h"


#define AUTH_MAX_EUTRAN_VECTORS 6

int
s6a_auth_info_cb (
  struct msg **msg,
  struct avp *paramavp,
  struct session *sess,
  void *opaque,
  enum disp_action *act)
{
  struct msg                             *ans = NULL,
                                         *qry = NULL;
  struct avp                             *avp = NULL,
                                         *failed_avp = NULL;
  struct avp_hdr                         *hdr = NULL;
  union avp_value                         value;

  /*
   * Database queries
   */
  mysql_auth_info_req_t                   auth_info_req;
  mysql_auth_info_resp_t                  auth_info_resp;

  /*
   * Authentication vector
   */
  auc_vector_t                            vector[AUTH_MAX_EUTRAN_VECTORS];
  int                                     ret = 0;
  int                                     result_code = ER_DIAMETER_SUCCESS;
  int                                     experimental = 0;
  uint64_t                                imsi = 0;
  uint32_t                                num_vectors = 0;
  uint8_t                                *sqn = NULL,
    *auts = NULL;

  if (msg == NULL) {
    return EINVAL;
  }

  /*
   * Create answer header
   */
  qry = *msg;
  CHECK_FCT (fd_msg_new_answer_from_req (fd_g_config->cnf_dict, msg, 0));
  ans = *msg;
  /*
   * Retrieving IMSI AVP: User-Name
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_imsi, &avp));

  if (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    if (hdr->avp_value->os.len > IMSI_LENGTH_MAX) {
      result_code = ER_DIAMETER_INVALID_AVP_VALUE;
      goto out;
    }

    sprintf (auth_info_req.imsi, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
    sscanf (auth_info_req.imsi, "%" SCNu64, &imsi);
  } else {
    result_code = ER_DIAMETER_MISSING_AVP;
    goto out;
  }

  /*
   * Retrieving Supported Features AVP. This is an optional AVP.
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_supported_features, &avp));

  if (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
  }

  /*
   * Retrieving the Requested-EUTRAN-Authentication-Info.
   * * * * If this AVP is not present, we have to check for
   * * * * Requested-GERAN-Authentication-Info AVP which will mean that the request
   * * * * comes from RAT other than E-UTRAN, case not handled by this HSS
   * * * * implementation.
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_req_e_utran_auth_info, &avp));

  if (avp) {
    struct avp                             *child_avp;

    /*
     * Walk through childs avp
     */
    CHECK_FCT (fd_msg_browse (avp, MSG_BRW_FIRST_CHILD, &child_avp, NULL));

    while (child_avp) {
      /*
       * Retrieve the header of the child avp
       */
      CHECK_FCT (fd_msg_avp_hdr (child_avp, &hdr));

      switch (hdr->avp_code) {
      case AVP_CODE_NUMBER_OF_REQ_VECTORS:{
          /*
           * We allow only one vector request
           */
          if (hdr->avp_value->u32 > AUTH_MAX_EUTRAN_VECTORS) {
            result_code = ER_DIAMETER_INVALID_AVP_VALUE;
            failed_avp = child_avp;
            goto out;
          }
          num_vectors = hdr->avp_value->u32;
        }
        break;

      case AVP_CODE_IMMEDIATE_RESP_PREF:
        /*
         * We always respond immediately to the request
         */
        break;

      case AVP_CODE_RE_SYNCHRONIZATION_INFO:

        /*
         * The resynchronization-info AVP is present.
         * * * * AUTS = Conc(SQN MS ) || MAC-S
         */
        if (avp) {
          auts = hdr->avp_value->os.data;
        }

        break;

      default:{
          /*
           * This AVP is not expected on s6a interface
           */
          result_code = ER_DIAMETER_AVP_UNSUPPORTED;
          failed_avp = child_avp;
          goto out;
        }
      }

      /*
       * Go to next AVP in the grouped AVP
       */
      CHECK_FCT (fd_msg_browse (child_avp, MSG_BRW_NEXT, &child_avp, NULL));
    }
  } else {
    CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_req_geran_auth_info, &avp));

    if (avp) {
      result_code = DIAMETER_ERROR_RAT_NOT_ALLOWED;
      experimental = 1;
      goto out;
    } else {
      result_code = ER_DIAMETER_INVALID_AVP_VALUE;
      failed_avp = avp;
      goto out;
    }
  }

  /*
   * Retrieving the Visited-PLMN-Id AVP
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_visited_plmn_id, &avp));

  if (avp) {
    /*
     * TODO: check PLMN and allow/reject connectivity depending on roaming
     */
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    if (hdr->avp_value->os.len == 3) {
      if (apply_access_restriction (auth_info_req.imsi, hdr->avp_value->os.data) != 0) {
        /*
         * We found that user is roaming and has no right to do it ->
         * * * * reject the connection
         */
        result_code = DIAMETER_ERROR_ROAMING_NOT_ALLOWED;
        experimental = 1;
        goto out;
      }
    } else {
      result_code = ER_DIAMETER_INVALID_AVP_VALUE;
      goto out;
    }
  } else {
    /*
     * Mandatory AVP, raise an error if not present
     */
    result_code = ER_DIAMETER_MISSING_AVP;
    goto out;
  }

  /*
   * Fetch User data
   */
  if (hss_mysql_auth_info (&auth_info_req, &auth_info_resp) != 0) {
    /*
     * Database query failed...
     */
    result_code = DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE;
    experimental = 1;
    goto out;
  }

  if (auts != NULL) {
    /*
     * Try to derive SQN_MS from previous RAND
     */
    sqn = sqn_ms_derive (auth_info_resp.opc, auth_info_resp.key, auts, auth_info_resp.rand);

    if (sqn != NULL) {
      /*
       * We succeeded to verify SQN_MS...
       */
      /*
       * Pick a new RAND and store SQN_MS + RAND in the HSS
       */
      generate_random (vector[0].rand, RAND_LENGTH);
      hss_mysql_push_rand_sqn (auth_info_req.imsi, vector[0].rand, sqn);
      hss_mysql_increment_sqn (auth_info_req.imsi);
      free (sqn);
    }

    /*
     * Fetch new user data
     */
    if (hss_mysql_auth_info (&auth_info_req, &auth_info_resp) != 0) {
      /*
       * Database query failed...
       */
      result_code = DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE;
      experimental = 1;
      goto out;
    }

    sqn = auth_info_resp.sqn;
    for (int i = 0; i < num_vectors; i++) {
      generate_random (vector[i].rand, RAND_LENGTH);
      generate_vector (auth_info_resp.opc, imsi, auth_info_resp.key, hdr->avp_value->os.data, sqn, &vector[i]);
    }
    hss_mysql_push_rand_sqn (auth_info_req.imsi, vector[num_vectors-1].rand, sqn);
  } else {
    /*
     * Pick a new RAND and store SQN_MS + RAND in the HSS
     */
    for (int i = 0; i < num_vectors; i++) {
      generate_random (vector[i].rand, RAND_LENGTH);
      sqn = auth_info_resp.sqn;
      /*
       * Generate authentication vector
       */
      generate_vector (auth_info_resp.opc, imsi, auth_info_resp.key, hdr->avp_value->os.data, sqn, &vector[i]);
    }
    hss_mysql_push_rand_sqn (auth_info_req.imsi, vector[num_vectors-1].rand, sqn);
  }

  hss_mysql_increment_sqn (auth_info_req.imsi);
  /*
   * We add the vector
   */
  {
    struct avp                             *e_utran_vector,
                                           *child_avp;
    for (int i = 0; i < num_vectors; i++) {
      CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_authentication_info, 0, &avp));
      CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_e_utran_vector, 0, &e_utran_vector));
      CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_rand, 0, &child_avp));
      value.os.data = vector[i].rand;
      value.os.len = RAND_LENGTH_OCTETS;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (e_utran_vector, MSG_BRW_LAST_CHILD, child_avp));
      CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_xres, 0, &child_avp));
      value.os.data = vector[i].xres;
      value.os.len = XRES_LENGTH_OCTETS;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (e_utran_vector, MSG_BRW_LAST_CHILD, child_avp));
      CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_autn, 0, &child_avp));
      value.os.data = vector[i].autn;
      value.os.len = AUTN_LENGTH_OCTETS;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (e_utran_vector, MSG_BRW_LAST_CHILD, child_avp));
      CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_kasme, 0, &child_avp));
      value.os.data = vector[i].kasme;
      value.os.len = KASME_LENGTH_OCTETS;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (e_utran_vector, MSG_BRW_LAST_CHILD, child_avp));
      CHECK_FCT (fd_msg_avp_add (avp, MSG_BRW_LAST_CHILD, e_utran_vector));
      CHECK_FCT (fd_msg_avp_add (ans, MSG_BRW_LAST_CHILD, avp));
    }
  }
out:
  /*
   * Add the Auth-Session-State AVP
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_cnf.dataobj_s6a_auth_session_state, &avp));
  CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
  CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_auth_session_state, 0, &avp));
  CHECK_FCT (fd_msg_avp_setvalue (avp, hdr->avp_value));
  CHECK_FCT (fd_msg_avp_add (ans, MSG_BRW_LAST_CHILD, avp));
  /*
   * Append the result code to the answer
   */
  CHECK_FCT (s6a_add_result_code (ans, failed_avp, result_code, experimental));
  CHECK_FCT (fd_msg_send (msg, NULL, NULL));
  return ret;
}
