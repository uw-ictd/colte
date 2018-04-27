/* SMS CLR */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#include "bstrlib.h"

#include "dynamic_memory_check.h"
#include "hashtable.h"
#include "obj_hashtable.h"
#include "log.h"
#include "msc.h"
#include "assertions.h"
#include "conversions.h"
#include "intertask_interface.h"
#include "common_defs.h"
#include "s6a_defs.h"
#include "s6a_messages_types.h"
#include "mme_config.h"
	
int
s6a_clr_cb (
  struct msg **msg,
  struct avp *paramvp,
  struct session *sess,
  void *opaque,
  enum disp_action *act)
{
  struct msg                             *ans,
                                         *qry;
  struct avp                             *avp;
//                                         *origin_host,
//                                         *origin_realm;
  struct avp                             *failed_avp = NULL;
//  struct avp_hdr                         *origin_host_hdr,
//                                         *origin_realm_hdr;
  struct avp_hdr                         *hdr;
//  union avp_value                         value;
//  int                                     ret = 0;
  int                                     result_code = ER_DIAMETER_SUCCESS;
  int                                     experimental = 0;
//  uint32_t                                ulr_flags = 0;

  if (msg == NULL) {
    return EINVAL;
  }

  OAILOG_DEBUG (LOG_S6A, "SMS CLR: Received Cancel Location Request\n");
  qry = *msg;
  /*
   * Create the answer
   */
  CHECK_FCT (fd_msg_new_answer_from_req (fd_g_config->cnf_dict, msg, 0));
  ans = *msg;

  /*
   * Add the Auth-Session-State AVP
   */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_fd_cnf.dataobj_s6a_auth_session_state, &avp));
  CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
  CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_auth_session_state, 0, &avp));
  CHECK_FCT (fd_msg_avp_setvalue (avp, hdr->avp_value));
  CHECK_FCT (fd_msg_avp_add (ans, MSG_BRW_LAST_CHILD, avp));
  /*
   * Append the result code to the answer
   */
  CHECK_FCT (s6a_add_result_code (ans, failed_avp, result_code, experimental));
  CHECK_FCT (fd_msg_send (msg, NULL, NULL));
  return 0;
}


  /* SMS CLR: The following code parses/validates IMSI - do we need to? */
  /*
   * Retrieving IMSI AVP
   */
  // CHECK_FCT (fd_msg_search_avp (qry, s6a_fd_cnf.dataobj_s6a_imsi, &avp));
  // if (avp) {
  //   CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

  //   if (hdr->avp_value->os.len > IMSI_LENGTH) {
  //     FPRINTF_NOTICE ( "IMSI_LENGTH ER_DIAMETER_INVALID_AVP_VALUE\n");
  //     result_code = ER_DIAMETER_INVALID_AVP_VALUE;
  //     goto out;
  //   }
  //   // 3GPP TS 29.272-910 / 5.2.1.1.3 Detailed behaviour of the HSS
  //   // When receiving an Update Location request the HSS shall check whether the IMSI is known.
  //   // If it is not known, a Result Code of DIAMETER_ERROR_USER_UNKNOWN shall be returned.
  //   // If it is known, but the subscriber has no EPS subscription, the HSS may (as an operator option)
  //   //     return a Result Code of DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION.
  //   // If the Update Location Request is received over the S6a interface, and the subscriber has not
  //   //     any APN configuration, the HSS shall return a Result Code of DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION.
  //   // The HSS shall check whether the RAT type the UE is using  is allowed. If it is not,
  //   //     a Result Code of DIAMETER_ERROR_RAT_NOT_ALLOWED shall be returned.
  //   // ...
  //   sprintf (mysql_push.imsi, "%*s", (int)hdr->avp_value->os.len, (char *)hdr->avp_value->os.data);

  //   if ((ret = hss_mysql_update_loc (mysql_push.imsi, &mysql_ans)) != 0) {
  //     /*
  //      * We failed to find the IMSI in the database. Replying to the request
  //      * * * * with the user unknown cause.
  //      */
  //     experimental = 1;
  //     FPRINTF_NOTICE ( "IMSI %s DIAMETER_ERROR_USER_UNKNOWN\n", mysql_push.imsi);
  //     result_code = DIAMETER_ERROR_USER_UNKNOWN;
  //     goto out;
  //   }
  // } else {
  //   FPRINTF_ERROR ( "Cannot get IMSI AVP which is mandatory\n");
  //   result_code = ER_DIAMETER_MISSING_AVP;
  //   goto out;
  // }

  /*
   * Retrieving Origin host AVP
   */
  // CHECK_FCT (fd_msg_search_avp (qry, s6a_fd_cnf.dataobj_s6a_origin_host, &origin_host));
  // if (!origin_host) {
  //   FPRINTF_ERROR ( "origin_host ER_DIAMETER_MISSING_AVP\n");
  //   result_code = ER_DIAMETER_MISSING_AVP;
  //   goto out;
  // }

  
  //  * Retrieving Origin realm AVP
   
  // CHECK_FCT (fd_msg_search_avp (qry, s6a_fd_cnf.dataobj_s6a_origin_realm, &origin_realm));
  // if (!origin_realm) {
  //   FPRINTF_ERROR ( "origin_realm ER_DIAMETER_MISSING_AVP\n");
  //   result_code = ER_DIAMETER_MISSING_AVP;
  //   goto out;
  // }






//   struct msg                             *ans_p = NULL;
//   struct msg                             *qry_p = NULL;
//   struct avp                             *avp_p = NULL;
//   struct avp_hdr                         *hdr_p = NULL;
//   MessageDef                             *message_p = NULL;
//   s6a_update_location_ans_t              *s6a_update_location_ans_p = NULL;

//   DevAssert (msg_pP );
//   ans_p = *msg_pP;
//   /*
//    * Retrieve the original query associated with the asnwer
//    */
//   CHECK_FCT (fd_msg_answ_getq (ans_p, &qry_p));
//   DevAssert (qry_p );
//   message_p = itti_alloc_new_message (TASK_S6A, S6A_UPDATE_LOCATION_ANS);
//   s6a_update_location_ans_p = &message_p->ittiMsg.s6a_update_location_ans;
//   CHECK_FCT (fd_msg_search_avp (qry_p, s6a_fd_cnf.dataobj_s6a_user_name, &avp_p));

//   if (avp_p) {
//     CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));
//     memcpy (s6a_update_location_ans_p->imsi, hdr_p->avp_value->os.data, hdr_p->avp_value->os.len);
//     s6a_update_location_ans_p->imsi[hdr_p->avp_value->os.len] = '\0';
//     s6a_update_location_ans_p->imsi_length = hdr_p->avp_value->os.len;
//     OAILOG_DEBUG (LOG_S6A, "Received s6a ula for imsi=%*s\n", (int)hdr_p->avp_value->os.len, hdr_p->avp_value->os.data);
//   } else {
//     DevMessage ("Query has been freed before we received the answer\n");
//   }

//   /*
//    * Retrieve the result-code
//    */
//   CHECK_FCT (fd_msg_search_avp (ans_p, s6a_fd_cnf.dataobj_s6a_result_code, &avp_p));

//   if (avp_p) {
//     CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));
//     s6a_update_location_ans_p->result.present = S6A_RESULT_BASE;
//     s6a_update_location_ans_p->result.choice.base = hdr_p->avp_value->u32;
//     MSC_LOG_TX_MESSAGE (MSC_S6A_MME, MSC_MMEAPP_MME, NULL, 0, "0 S6A_UPDATE_LOCATION_ANS imsi %s %s", s6a_update_location_ans_p->imsi, retcode_2_string (hdr_p->avp_value->u32));

//     if (hdr_p->avp_value->u32 != ER_DIAMETER_SUCCESS) {
//       OAILOG_ERROR (LOG_S6A, "Got error %u:%s\n", hdr_p->avp_value->u32, retcode_2_string (hdr_p->avp_value->u32));
//       goto err;
//     }
//   } else {
//     /*
//      * The result-code is not present, may be it is an experimental result
//      * * * * avp_p indicating a 3GPP specific failure.
//      */
//     CHECK_FCT (fd_msg_search_avp (ans_p, s6a_fd_cnf.dataobj_s6a_experimental_result, &avp_p));

//     if (avp_p) {
//       /*
//        * The procedure has failed within the HSS.
//        * * * * NOTE: contrary to result-code, the experimental-result is a grouped
//        * * * * AVP and requires parsing its childs to get the code back.
//        */
//       s6a_update_location_ans_p->result.present = S6A_RESULT_EXPERIMENTAL;
//       s6a_parse_experimental_result (avp_p, &s6a_update_location_ans_p->result.choice.experimental);
//       goto err;
//     } else {
//       /*
//        * Neither result-code nor experimental-result is present ->
//        * * * * totally incorrect behaviour here.
//        */
//       OAILOG_ERROR (LOG_S6A, "Experimental-Result and Result-Code are absent: " "This is not a correct behaviour\n");
//       goto err;
//     }
//   }

//   /*
//    * Retrieving the ULA flags
//    */
//   CHECK_FCT (fd_msg_search_avp (ans_p, s6a_fd_cnf.dataobj_s6a_ula_flags, &avp_p));

//   if (avp_p) {
//     CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));

//     /*
//      * This bit, when set, indicates that the HSS stores SGSN number
//      * * * * and MME number in separate memory. A Rel-8 HSS shall set
//      * * * * the bit.
//      */
//     if (!FLAG_IS_SET (hdr_p->avp_value->u32, ULA_SEPARATION_IND)) {
//       OAILOG_ERROR (LOG_S6A, "ULA-Flags does not indicate the HSS is post Rel.8: " "This behaviour is not compliant\n");
//       goto err;
//     }
//   } else {
//     /*
//      * ULA-Flags is absent while the error code indicates DIAMETER_SUCCESS:
//      * * * * this is not a compliant behaviour...
//      * * * * TODO: handle this case.
//      */
//     OAILOG_ERROR (LOG_S6A, "ULA-Flags AVP is absent while result code indicates " "DIAMETER_SUCCESS\n");
//     goto err;
//   }

//   CHECK_FCT (fd_msg_search_avp (ans_p, s6a_fd_cnf.dataobj_s6a_subscription_data, &avp_p));

//   if (avp_p) {
//     CHECK_FCT (s6a_parse_subscription_data (avp_p, &s6a_update_location_ans_p->subscription_data));
//     // LG COMMENTED THIS (2014/04/01)-> DevParam(0, 0, 0);
//   }

// err:
//   ans_p = NULL;
//   itti_send_msg_to_task (TASK_MME_APP, INSTANCE_DEFAULT, message_p);
//   OAILOG_DEBUG (LOG_S6A, "Sending S6A_UPDATE_LOCATION_ANS to task MME_APP\n");
//   return RETURNok;
// }

int
s6a_generate_cancel_location (
  s6a_cancel_location_ans_t * ulr_pP)
{
  OAILOG_DEBUG (LOG_S6A, "SMS CLR: s6a_generate_cancel_location\n");
  return 0;
}  
//   struct avp                             *avp_p = NULL;
//   struct msg                             *msg_p = NULL;
//   struct session                         *sess_p = NULL;
//   union avp_value                         value;

//   DevAssert (ulr_pP );
//   /*
//    * Create the new update location request message
//    */
//   CHECK_FCT (fd_msg_new (s6a_fd_cnf.dataobj_s6a_ulr, 0, &msg_p));
//   /*
//    * Create a new session
//    */
//   CHECK_FCT (fd_sess_new (&sess_p, fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len, (os0_t) "apps6a", 6));
//   {
//     os0_t                                   sid;
//     size_t                                  sidlen;

//     CHECK_FCT (fd_sess_getsid (sess_p, &sid, &sidlen));
//     CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_session_id, 0, &avp_p));
//     value.os.data = sid;
//     value.os.len = sidlen;
//     CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//     CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_FIRST_CHILD, avp_p));
//   }
//   CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_auth_session_state, 0, &avp_p));
//   /*
//    * No State maintained
//    */
//   value.i32 = 1;
//   CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//   CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
//   /*
//    * Add Origin_Host & Origin_Realm
//    */
//   CHECK_FCT (fd_msg_add_origin (msg_p, 0));
//   mme_config_read_lock (&mme_config);
//   /*
//    * Destination Host
//    */
//   {
//     bstring                                 host = bstrcpy(mme_config.s6a_config.hss_host_name);

//     bconchar(host, '.');
//     bconcat (host, mme_config.realm);

//     CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_destination_host, 0, &avp_p));
//     value.os.data = (unsigned char *)bdata(host);
//     value.os.len = blength(host);
//     CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//     CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
//     bdestroy_wrapper (&host);
//   }
//   /*
//    * Destination_Realm
//    */
//   {
//     CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_destination_realm, 0, &avp_p));
//     value.os.data = (unsigned char *)bdata(mme_config.realm);
//     value.os.len = blength(mme_config.realm);
//     CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//     CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
//   }
//   mme_config_unlock (&mme_config);
//   /*
//    * Adding the User-Name (IMSI)
//    */
//   CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_user_name, 0, &avp_p));
//   value.os.data = (unsigned char *)ulr_pP->imsi;
//   value.os.len = strlen (ulr_pP->imsi);
//   CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//   CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
//   /*
//    * Adding the visited plmn id
//    */
//   {
//     uint8_t                                 plmn[3];

//     CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_visited_plmn_id, 0, &avp_p));
//     PLMN_T_TO_TBCD (ulr_pP->visited_plmn,
//                     plmn,
//                     mme_config_find_mnc_length (ulr_pP->visited_plmn.mcc_digit1, ulr_pP->visited_plmn.mcc_digit2, ulr_pP->visited_plmn.mcc_digit3, ulr_pP->visited_plmn.mnc_digit1, ulr_pP->visited_plmn.mnc_digit2, ulr_pP->visited_plmn.mnc_digit3)
//       );
//     value.os.data = plmn;
//     value.os.len = 3;
//     CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//     CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
//   }
//   /*
//    * Adding the RAT-Type
//    */
//   CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_rat_type, 0, &avp_p));
//   DevCheck (ulr_pP->rat_type == RAT_EUTRAN, ulr_pP->rat_type, 0, 0);
//   value.u32 = ulr_pP->rat_type;
//   CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//   CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
//   /*
//    * Adding ULR-Flags
//    */
//   CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_ulr_flags, 0, &avp_p));
//   value.u32 = 0;
//   /*
//    * Identify the ULR as coming from S6A interface (i.e. from MME)
//    */
//   FLAGS_SET (value.u32, ULR_S6A_S6D_INDICATOR);

//   /*
//    * Set the ulr-flags as indicated by upper layer
//    */
//   if (ulr_pP->skip_subscriber_data) {
//     FLAGS_SET (value.u32, ULR_SKIP_SUBSCRIBER_DATA);
//   }

//   if (ulr_pP->initial_attach) {
//     FLAGS_SET (value.u32, ULR_INITIAL_ATTACH_IND);
//   }

//   CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
//   CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
//   CHECK_FCT (fd_msg_send (&msg_p, NULL, NULL));
//   OAILOG_DEBUG (LOG_S6A, "Sending s6a ulr for imsi=%s\n", ulr_pP->imsi);
//   return RETURNok;
// }
