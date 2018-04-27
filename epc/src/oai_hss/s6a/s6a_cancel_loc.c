#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdproto.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"

//   char       imsi[IMSI_BCD_DIGITS_MAX + 1]; // username
  // uint8_t    imsi_length;               // username

int
s6a_generate_cancel_location_req (char *imsi)
{
  struct avp                             *avp_p = NULL;
  struct msg                             *msg_p = NULL;
  struct session                         *sess_p = NULL;
  union avp_value                         value;

  /*
   * Create the new update location request message
   */
  CHECK_FCT (fd_msg_new (s6a_fd_cnf.dataobj_s6a_clr, 0, &msg_p));
  /*
   * Create a new session
   */
  CHECK_FCT (fd_sess_new (&sess_p, fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len, (os0_t) "apps6a", 6));
  {
    os0_t                                   sid;
    size_t                                  sidlen;

    CHECK_FCT (fd_sess_getsid (sess_p, &sid, &sidlen));
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_session_id, 0, &avp_p));
    value.os.data = sid;
    value.os.len = sidlen;
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_FIRST_CHILD, avp_p));
  }
  CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_auth_session_state, 0, &avp_p));
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
    bstring                                 host = bstrcpy(mme_config.s6a_config.hss_host_name);

    bconchar(host, '.');
    bconcat (host, mme_config.realm);

    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_destination_host, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(host);
    value.os.len = blength(host);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
    bdestroy_wrapper (&host);
  }
  /*
   * Destination_Realm
   */
  {
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_destination_realm, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(mme_config.realm);
    value.os.len = blength(mme_config.realm);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
  }
  /*
   * Adding the User-Name (IMSI)
   */
/* SMS CLR: LOOK INTO THIS */
  CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_user_name, 0, &avp_p));
  value.os.data = (unsigned char *)imsi;
  value.os.len = strlen (imsi);
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_cancel_type, 0, &avp_p));
/* SMS CLR: WHAT VAL SHOULD THIS ACTUALLY BE?!? AND WHAT FORMAT?!? */
  value.i32 = 1;  
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
  CHECK_FCT (fd_msg_send (&msg_p, NULL, NULL));

  FPRINTF_ERROR("SMS CLR: Sending S6A Cancel Location Request for imsi=%s\n", imsi);

  return 0;
}