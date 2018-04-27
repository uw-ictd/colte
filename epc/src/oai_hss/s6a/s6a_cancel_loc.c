#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdproto.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"
#include "bstrlib.h"
#include "log.h"

#define CHECK_FD_FCT(fCT)  DevAssert(fCT == 0);

//   char       imsi[IMSI_BCD_DIGITS_MAX + 1]; // username
  // uint8_t    imsi_length;               // username

int
s6a_generate_cancel_location_req (char *imsi)
{
  struct avp                             *avp_p = NULL;
  struct msg                             *msg_p = NULL;
  struct session                         *sess_p = NULL;
  union avp_value                         value;

  /* SMS CLR TODO: ALL THIS IS JUST HARD-CODED!!! */
  /* SMS CLR TODO: should this value actually be "colte"??? Possible bug??? */
  bstring dst_host = bfromcstr("mme");
  // bstring src_host = bfromcstr("hss");
  bstring realm = bfromcstr("OpenAir5G.Alliance");

  /* SMS CLR: These specific values copied/mimic'ed from s6a_dict.c since not included.
   * They directly correspond to all the values that lead with s6a_fd_cnf. */
  struct dict_object *dataobj_s6a_clr;
  struct dict_object *dataobj_s6a_session_id;
  struct dict_object *dataobj_s6a_auth_session_state;
  struct dict_object *dataobj_s6a_destination_host;
  struct dict_object *dataobj_s6a_destination_realm;
  struct dict_object *dataobj_s6a_user_name;
  struct dict_object *dataobj_s6a_cancel_type;

  CHECK_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Cancel-Location-Request", &dataobj_s6a_clr, ENOENT));
  CHECK_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Session-Id", &dataobj_s6a_session_id, ENOENT));
  CHECK_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Auth-Session-State", &dataobj_s6a_auth_session_state, ENOENT));
  CHECK_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Host", &dataobj_s6a_destination_host, ENOENT));
  CHECK_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Realm", &dataobj_s6a_destination_realm, ENOENT));
  CHECK_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "User-Name", &dataobj_s6a_user_name, ENOENT));
  CHECK_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Cancellation-Type", &dataobj_s6a_cancel_type, ENOENT));

  /*
   * Create the new update location request message
   */
  CHECK_FCT (fd_msg_new (dataobj_s6a_clr, 0, &msg_p));

  /*
   * Create a new session
   */
  CHECK_FCT (fd_sess_new (&sess_p, fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len, (os0_t) "apps6a", 6));
  {
    os0_t                                   sid;
    size_t                                  sidlen;

    CHECK_FCT (fd_sess_getsid (sess_p, &sid, &sidlen));
    CHECK_FCT (fd_msg_avp_new (dataobj_s6a_session_id, 0, &avp_p));
    value.os.data = sid;
    value.os.len = sidlen;
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_FIRST_CHILD, avp_p));
  }
  CHECK_FCT (fd_msg_avp_new (dataobj_s6a_auth_session_state, 0, &avp_p));
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

    CHECK_FCT (fd_msg_avp_new (dataobj_s6a_destination_host, 0, &avp_p));
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
    CHECK_FCT (fd_msg_avp_new (dataobj_s6a_destination_realm, 0, &avp_p));
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
  CHECK_FCT (fd_msg_avp_new (dataobj_s6a_user_name, 0, &avp_p));
  value.os.data = (unsigned char *)imsi;
  value.os.len = strlen (imsi);
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  CHECK_FCT (fd_msg_avp_new (dataobj_s6a_cancel_type, 0, &avp_p));
/* SMS CLR: WHAT VAL SHOULD THIS ACTUALLY BE?!? AND WHAT FORMAT?!? */
  value.i32 = 2;
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));
  CHECK_FCT (fd_msg_send (&msg_p, NULL, NULL));

  FPRINTF_ERROR("SMS CLR: Sending S6A Cancel Location Request for imsi=%s\n", imsi);

  return 0;
}