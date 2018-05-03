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
	
#define IMSI_LENGTH_MAX 15

int
s6a_add_result_code (
  struct msg *ans,
  struct avp *failed_avp,
  int result_code,
  int experimental)
{
  struct avp                             *avp;
  union avp_value                         value;

  if (DIAMETER_ERROR_IS_VENDOR (result_code) && experimental != 0) {
    struct avp                             *experimental_result;

    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_experimental_result, 0, &experimental_result));
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_vendor_id, 0, &avp));
    value.u32 = VENDOR_3GPP;
    CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
    CHECK_FCT (fd_msg_avp_add (experimental_result, MSG_BRW_LAST_CHILD, avp));
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_experimental_result_code, 0, &avp));
    value.u32 = result_code;
    CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
    CHECK_FCT (fd_msg_avp_add (experimental_result, MSG_BRW_LAST_CHILD, avp));
    CHECK_FCT (fd_msg_avp_add (ans, MSG_BRW_LAST_CHILD, experimental_result));
    /*
     * Add Origin_Host & Origin_Realm AVPs
     */
    CHECK_FCT (fd_msg_add_origin (ans, 0));
  } else {
    /*
     * This is a code defined in the base protocol: result-code AVP should
     * * * * be used.
     */
    CHECK_FCT (fd_msg_rescode_set (ans, retcode_2_string (result_code), NULL, failed_avp, 1));
  }

  return 0;
}
  
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
  struct avp                             *failed_avp = NULL;
  struct avp_hdr                         *hdr;
  int                                     result_code = ER_DIAMETER_SUCCESS;
  int                                     experimental = 0;

  s6a_cancel_location_req_t              *s6a_cancel_location_req_p = NULL;
  MessageDef                             *message_p = NULL;

  if (msg == NULL) {
    return EINVAL;
  }

  OAILOG_DEBUG (LOG_S6A, "SMS CLR: Received Cancel Location Request\n");
  qry = *msg;

  /* STEP 0: Validate Message? */
  /* (SMS TODO) */

  /* STEP 0.5: Get and check IMSI */
  CHECK_FCT (fd_msg_search_avp (qry, s6a_fd_cnf.dataobj_s6a_user_name, &avp));
  if (!avp) {
    OAILOG_ERROR (LOG_S6A, "Cannot get IMSI AVP which is mandatory\n");
    result_code = ER_DIAMETER_MISSING_AVP;
    // goto out;
    return -1;
  }

  CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
  if (hdr->avp_value->os.len > IMSI_LENGTH_MAX) {
    OAILOG_ERROR (LOG_S6A, "IMSI_LENGTH ER_DIAMETER_INVALID_AVP_VALUE\n");
    result_code = ER_DIAMETER_INVALID_AVP_VALUE;
      // goto out;
    return -1;
  }

  /* STEP 1: Convert IMSI to our format and send it to MME app for processing */
  message_p = itti_alloc_new_message (TASK_S6A, S6A_CANCEL_LOCATION_REQ);
  s6a_cancel_location_req_p = &message_p->ittiMsg.s6a_cancel_location_req;

  memcpy (s6a_cancel_location_req_p->imsi, hdr->avp_value->os.data, hdr->avp_value->os.len);
  s6a_cancel_location_req_p->imsi[hdr->avp_value->os.len] = '\0';
  s6a_cancel_location_req_p->imsi_length = hdr->avp_value->os.len;
  IMSI_STRING_TO_IMSI64 ((char *)s6a_cancel_location_req_p->imsi, &s6a_cancel_location_req_p->imsi64);

  itti_send_msg_to_task (TASK_MME_APP, INSTANCE_DEFAULT, message_p);
  OAILOG_DEBUG (LOG_S6A, "Sending S6A_CANCEL_LOCATION_REQ to task MME_APP\n");

  /* STEP 2: Send a response to the HSS */
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
