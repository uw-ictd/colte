#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdproto.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"
#include "bstrlib.h"
#include "log.h"

//   char       imsi[IMSI_BCD_DIGITS_MAX + 1]; // username
  // uint8_t    imsi_length;               // username

int
s6a_generate_cancel_location_req (char *imsi)
{
  struct avp                             *avp_p = NULL;
  struct msg                             *msg_p = NULL;
  struct session                         *sess_p = NULL;
  union avp_value                         value;

  /* SMS TODO: THESE VALS ARE JUST HARD-CODED! */
  bstring dst_host = bfromcstr("mme");
  bstring realm = bfromcstr("OpenAir5G.Alliance");

  /*
   * Create the new update location request message
   */
  CHECK_FCT (fd_msg_new (s6a_cnf.dataobj_s6a_cancel_loc_req, 0, &msg_p));

  /*
   * Create a new session
   */
  CHECK_FCT (fd_sess_new (&sess_p, fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len, (os0_t) "apps6a", 6));
  {
    os0_t                                   sid;
    size_t                                  sidlen;

    CHECK_FCT (fd_sess_getsid (sess_p, &sid, &sidlen));
    CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_session_id, 0, &avp_p));
    value.os.data = sid;
    value.os.len = sidlen;
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_FIRST_CHILD, avp_p));
  }

  CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_auth_session_state, 0, &avp_p));
  /*
   * No State maintained
   */
  value.i32 = 1;
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
  /*
   * Add Origin_Host & Origin_Realm
   */
  CHECK_FCT (fd_msg_add_origin (msg_p, 0));

  /*
   * Destination Host
   */
  {
    bconchar(dst_host, '.');
    bconcat (dst_host, realm);

    CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_destination_host, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(dst_host);
    value.os.len = blength(dst_host);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
    bdestroy(dst_host);
  }
  /*
   * Destination_Realm
   */

  {
    CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_destination_realm, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(realm);
    value.os.len = blength(realm);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
    bdestroy(realm);
  }

  /*
   * Adding the User-Name (IMSI)
   */
/* SMS CLR: LOOK INTO THIS */
  CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_imsi, 0, &avp_p));
  value.os.data = (unsigned char *)imsi;
  value.os.len = strlen (imsi);
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_cancel_type, 0, &avp_p));
/* SMS CLR: WHAT VAL SHOULD THIS ACTUALLY BE?!? AND WHAT FORMAT?!? */
  value.i32 = 2;
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  CHECK_FCT (fd_msg_send (&msg_p, NULL, NULL));

  FPRINTF_ERROR("SMS CLR: Sending S6A Cancel Location Request for imsi=%s\n", imsi);

  return 0;
}

int
s6a_cancel_loc_ans_cb (
  struct msg **msg,
  struct avp *paramavp,
  struct session *sess,
  void *opaque,
  enum disp_action *act)
{
  struct msg                             *ans_p = NULL;
  struct msg                             *qry_p = NULL;
  struct avp                             *avp_p = NULL;
  struct avp_hdr                         *hdr_p = NULL;
  // MessageDef                             *message_p = NULL;
  // s6a_update_location_ans_t              *s6a_update_location_ans_p = NULL;

  ans_p = *msg;

  /*
   * Retrieve the original query associated with the asnwer
   */
  CHECK_FCT (fd_msg_answ_getq (ans_p, &qry_p));
  // message_p = itti_alloc_new_message (TASK_S6A, S6A_UPDATE_LOCATION_ANS);
  // s6a_update_location_ans_p = &message_p->ittiMsg.s6a_update_location_ans;

  /* Get the IMSI out of the answer */
  // CHECK_FCT (fd_msg_search_avp (qry_p, s6a_cnf.dataobj_s6a_user_name, &avp_p));
  // if (avp_p) {
  //   CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));
  //   memcpy (s6a_update_location_ans_p->imsi, hdr_p->avp_value->os.data, hdr_p->avp_value->os.len);
  //   s6a_update_location_ans_p->imsi[hdr_p->avp_value->os.len] = '\0';
  //   s6a_update_location_ans_p->imsi_length = hdr_p->avp_value->os.len;
  //   OAILOG_DEBUG (LOG_S6A, "Received s6a ula for imsi=%*s\n", (int)hdr_p->avp_value->os.len, hdr_p->avp_value->os.data);
  // } else {
  //   DevMessage ("Query has been freed before we received the answer\n");
  // }

  /*
   * Retrieve the result-code
   */
  CHECK_FCT (fd_msg_search_avp (ans_p, s6a_cnf.dataobj_s6a_result_code, &avp_p));
  if (avp_p) {
    CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));
    // s6a_update_location_ans_p->result.present = S6A_RESULT_BASE;
    // s6a_update_location_ans_p->result.choice.base = hdr_p->avp_value->u32;
    // MSC_LOG_TX_MESSAGE (MSC_S6A_MME, MSC_MMEAPP_MME, NULL, 0, "0 S6A_UPDATE_LOCATION_ANS imsi %s %s", s6a_update_location_ans_p->imsi, retcode_2_string (hdr_p->avp_value->u32));

    if (hdr_p->avp_value->u32 != ER_DIAMETER_SUCCESS) {
      FPRINTF_ERROR ("Got error %u:%s\n", hdr_p->avp_value->u32, retcode_2_string (hdr_p->avp_value->u32));
      goto err;
    }
  }
  // } else {
  //   /*
  //    * The result-code is not present, may be it is an experimental result
  //    * * * * avp_p indicating a 3GPP specific failure.
  //    */
  //   CHECK_FCT (fd_msg_search_avp (ans_p, s6a_fd_cnf.dataobj_s6a_experimental_result, &avp_p));

  //   if (avp_p) {
      
  //      * The procedure has failed within the HSS.
  //      * * * * NOTE: contrary to result-code, the experimental-result is a grouped
  //      * * * * AVP and requires parsing its childs to get the code back.
       
  //     s6a_update_location_ans_p->result.present = S6A_RESULT_EXPERIMENTAL;
  //     s6a_parse_experimental_result (avp_p, &s6a_update_location_ans_p->result.choice.experimental);
  //     goto err;
  //   } else {
  //     /*
  //      * Neither result-code nor experimental-result is present ->
  //      * * * * totally incorrect behaviour here.
  //      */
  //     OAILOG_ERROR (LOG_S6A, "Experimental-Result and Result-Code are absent: " "This is not a correct behaviour\n");
  //     goto err;
  //   }
  // }
err:
//   ans_p = NULL;
//   itti_send_msg_to_task (TASK_MME_APP, INSTANCE_DEFAULT, message_p);
//   OAILOG_DEBUG (LOG_S6A, "Sending S6A_UPDATE_LOCATION_ANS to task MME_APP\n");
//   return RETURNok;

  FPRINTF_ERROR("SMS CLR: Received S6A Cancel Location Answer\n");
  return 0;
}
