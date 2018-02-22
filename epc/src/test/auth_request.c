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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "conversions.h"
#include "mcc_mnc_itu.h"
#include "common_defs.h"
#include "common_types.h"
#include "3gpp_23.003.h"
#include "secu_defs.h"
#include "security_types.h"
#include "securityDef.h"
#include "s6a_messages_types.h"
#include "assertions.h"


#define VENDOR_3GPP (10415)
#define APP_S6A     (16777251)

/* Errors that fall within the Permanent Failures category shall be used to
 * inform the peer that the request has failed, and should not be attempted
 * again. The Result-Code AVP values defined in Diameter Base Protocol RFC 3588
 * shall be applied. When one of the result codes defined here is included in a
 * response, it shall be inside an Experimental-Result AVP and the Result-Code
 * AVP shall be absent.
 */
#define DIAMETER_ERROR_USER_UNKNOWN             (5001)
#define DIAMETER_ERROR_ROAMING_NOT_ALLOWED      (5004)
#define DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION (5420)
#define DIAMETER_ERROR_RAT_NOT_ALLOWED          (5421)
#define DIAMETER_ERROR_EQUIPMENT_UNKNOWN        (5422)
#define DIAMETER_ERROR_UNKOWN_SERVING_NODE      (5423)

/* Result codes that fall within the transient failures category shall be used
 * to inform a peer that the request could not be satisfied at the time it was
 * received, but may be able to satisfy the request in the future. The
 * Result-Code AVP values defined in Diameter Base Protocol RFC 3588 shall be
 * applied. When one of the result codes defined here is included in a response,
 * it shall be inside an Experimental-Result AVP and the Result-Code AVP shall
 * be absent.
 */
#define DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE (4181)

#define DIAMETER_ERROR_IS_VENDOR(x)                    \
   ((x == DIAMETER_ERROR_USER_UNKNOWN)              || \
    (x == DIAMETER_ERROR_ROAMING_NOT_ALLOWED)       || \
    (x == DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION)  || \
    (x == DIAMETER_ERROR_RAT_NOT_ALLOWED)           || \
    (x == DIAMETER_ERROR_EQUIPMENT_UNKNOWN)         || \
    (x == DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE) || \
    (x == DIAMETER_ERROR_UNKOWN_SERVING_NODE))

typedef struct {
  struct dict_object *dataobj_s6a_vendor;     /* s6a vendor object */
  struct dict_object *dataobj_s6a_app;        /* s6a application object */

  /* Commands */
  struct dict_object *dataobj_s6a_air; /* s6a authentication request */
  struct dict_object *dataobj_s6a_aia; /* s6a authentication answer */
  struct dict_object *dataobj_s6a_ulr; /* s6a update location request */
  struct dict_object *dataobj_s6a_ula; /* s6a update location asnwer */
  struct dict_object *dataobj_s6a_pur; /* s6a purge ue request */
  struct dict_object *dataobj_s6a_pua; /* s6a purge ue answer */
  struct dict_object *dataobj_s6a_clr; /* s6a Cancel Location req */
  struct dict_object *dataobj_s6a_cla; /* s6a Cancel Location ans */

  /* Some standard basic AVPs */
  struct dict_object *dataobj_s6a_destination_host;
  struct dict_object *dataobj_s6a_destination_realm;
  struct dict_object *dataobj_s6a_user_name;
  struct dict_object *dataobj_s6a_session_id;
  struct dict_object *dataobj_s6a_auth_session_state;
  struct dict_object *dataobj_s6a_result_code;
  struct dict_object *dataobj_s6a_experimental_result;

  /* S6A specific AVPs */
  struct dict_object *dataobj_s6a_visited_plmn_id;
  struct dict_object *dataobj_s6a_rat_type;
  struct dict_object *dataobj_s6a_ulr_flags;
  struct dict_object *dataobj_s6a_ula_flags;
  struct dict_object *dataobj_s6a_subscription_data;
  struct dict_object *dataobj_s6a_req_eutran_auth_info;
  struct dict_object *dataobj_s6a_number_of_requested_vectors;
  struct dict_object *dataobj_s6a_immediate_response_pref;
  struct dict_object *dataobj_s6a_authentication_info;
  struct dict_object *dataobj_s6a_re_synchronization_info;
  struct dict_object *dataobj_s6a_service_selection;
  struct dict_object *dataobj_s6a_ue_srvcc_cap;

  /* Handlers */
  struct disp_hdl *aia_hdl;   /* Authentication Information Answer Handle */
  struct disp_hdl *ula_hdl;   /* Update Location Answer Handle */
  struct disp_hdl *pua_hdl;   /* Purge UE Answer Handle */
  struct disp_hdl *clr_hdl;   /* Cancel Location Request Handle */
} s6a_fd_cnf_t;

extern s6a_fd_cnf_t s6a_fd_cnf;

#define ULR_SINGLE_REGISTRATION_IND      (1U)
#define ULR_S6A_S6D_INDICATOR            (1U << 1)
#define ULR_SKIP_SUBSCRIBER_DATA         (1U << 2)
#define ULR_GPRS_SUBSCRIPTION_DATA_IND   (1U << 3)
#define ULR_NODE_TYPE_IND                (1U << 4)
#define ULR_INITIAL_ATTACH_IND           (1U << 5)
#define ULR_PS_LCS_SUPPORTED_BY_UE       (1U << 6)

#define ULA_SEPARATION_IND          (1U)

#define FLAG_IS_SET(x, flag)   \
    ((x) & (flag))

#define FLAGS_SET(x, flags) \
    ((x) |= (flags))

#define FLAGS_CLEAR(x, flags)   \
    ((x) = (x) & ~(flags))

/* IANA defined IP address type */
#define IANA_IPV4   (0x1)
#define IANA_IPV6   (0x2)

#define AVP_CODE_VENDOR_ID                         (266)
#define AVP_CODE_EXPERIMENTAL_RESULT               (297)
#define AVP_CODE_EXPERIMENTAL_RESULT_CODE          (298)
#define AVP_CODE_SERVICE_SELECTION                 (493)
#define AVP_CODE_BANDWIDTH_UL                      (516)
#define AVP_CODE_BANDWIDTH_DL                      (515)
#define AVP_CODE_MSISDN                            (701)
#define AVP_CODE_SERVED_PARTY_IP_ADDRESS           (848)
#define AVP_CODE_QCI                               (1028)
#define AVP_CODE_ALLOCATION_RETENTION_PRIORITY     (1034)
#define AVP_CODE_PRIORITY_LEVEL                    (1046)
#define AVP_CODE_PRE_EMPTION_CAPABILITY            (1047)
#define AVP_CODE_PRE_EMPTION_VULNERABILITY         (1048)
#define AVP_CODE_SUBSCRIPTION_DATA                 (1400)
#define AVP_CODE_AUTHENTICATION_INFO               (1413)
#define AVP_CODE_E_UTRAN_VECTOR                    (1414)
#define AVP_CODE_NETWORK_ACCESS_MODE               (1417)
#define AVP_CODE_CONTEXT_IDENTIFIER                (1423)
#define AVP_CODE_SUBSCRIBER_STATUS                 (1424)
#define AVP_CODE_ACCESS_RESTRICTION_DATA           (1426)
#define AVP_CODE_ALL_APN_CONFIG_INC_IND            (1428)
#define AVP_CODE_APN_CONFIGURATION_PROFILE         (1429)
#define AVP_CODE_APN_CONFIGURATION                 (1430)
#define AVP_CODE_EPS_SUBSCRIBED_QOS_PROFILE        (1431)
#define AVP_CODE_AMBR                              (1435)
#define AVP_CODE_RAND                              (1447)
#define AVP_CODE_XRES                              (1448)
#define AVP_CODE_AUTN                              (1449)
#define AVP_CODE_KASME                             (1450)
#define AVP_CODE_PDN_TYPE                          (1456)
#define AVP_CODE_SUBSCRIBED_PERIODIC_RAU_TAU_TIMER (1619)


s6a_fd_cnf_t                            s6a_fd_cnf;
char                                    *gfd_config_file = NULL;
char                                    *gfileout        = NULL;
char                                    *ghss_hostname   = NULL;
char                                    *grealm          = NULL;
uint8_t                                  gintegrity      = 0;
uint8_t                                  gencryption     = 0;
uint8_t                                  gul_nas_count   = 0;


#define CHECK_FD_FCT(fCT)   AssertFatal(fCT == 0, "freeDiameter call failed!\n")

//------------------------------------------------------------------------------
void s6a_peer_connected_cb(struct peer_info *info, void *arg);
int s6a_fd_init_dict_objs(void);
int s6a_parse_subscription_data(struct avp *avp_subscription_data,
                                subscription_data_t *subscription_data);
int s6a_parse_experimental_result(struct avp *avp, s6a_experimental_result_t *ptr);
char *experimental_retcode_2_string(uint32_t ret_code);
char *retcode_2_string(uint32_t ret_code);
int aia_cb (
  struct msg **msg,
  struct avp *paramavp,
  struct session *sess,
  void *opaque,
  enum disp_action *act);

//------------------------------------------------------------------------------
// callback for freeDiameter logs
void oai_fd_logger(int loglevel, const char * format, va_list args)
{
#define FD_LOG_MAX_MESSAGE_LENGTH 8192
  char       buffer[FD_LOG_MAX_MESSAGE_LENGTH];
  int        rv = 0;

  rv = vsnprintf (buffer, 8192, format, args);
  if ((0 > rv) || ((FD_LOG_MAX_MESSAGE_LENGTH) < rv)) {
    return;
  }
  fprintf (stdout, "%s\n", buffer);
}

//------------------------------------------------------------------------------
int
fd_init_dict_objs (
  void)
{
  struct disp_when                        when;
  vendor_id_t                             vendor_3gpp = VENDOR_3GPP;
  application_id_t                        app_s6a = APP_S6A;

  /*
   * Pre-loading vendor object
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_VENDOR, VENDOR_BY_ID, (void *)&vendor_3gpp, &s6a_fd_cnf.dataobj_s6a_vendor, ENOENT));
  /*
   * Pre-loading application object
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_APPLICATION, APPLICATION_BY_ID, (void *)&app_s6a, &s6a_fd_cnf.dataobj_s6a_app, ENOENT));
  /*
   * Pre-loading commands objects
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Authentication-Information-Request", &s6a_fd_cnf.dataobj_s6a_air, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Authentication-Information-Answer", &s6a_fd_cnf.dataobj_s6a_aia, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Update-Location-Request", &s6a_fd_cnf.dataobj_s6a_ulr, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Update-Location-Answer", &s6a_fd_cnf.dataobj_s6a_ula, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Purge-UE-Request", &s6a_fd_cnf.dataobj_s6a_pur, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Purge-UE-Answer", &s6a_fd_cnf.dataobj_s6a_pua, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Cancel-Location-Request", &s6a_fd_cnf.dataobj_s6a_clr, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Cancel-Location-Answer", &s6a_fd_cnf.dataobj_s6a_cla, ENOENT));
  /*
   * Pre-loading base avps
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Host", &s6a_fd_cnf.dataobj_s6a_destination_host, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Realm", &s6a_fd_cnf.dataobj_s6a_destination_realm, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "User-Name", &s6a_fd_cnf.dataobj_s6a_user_name, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Session-Id", &s6a_fd_cnf.dataobj_s6a_session_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Auth-Session-State", &s6a_fd_cnf.dataobj_s6a_auth_session_state, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Result-Code", &s6a_fd_cnf.dataobj_s6a_result_code, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Experimental-Result", &s6a_fd_cnf.dataobj_s6a_experimental_result, ENOENT));
  /*
   * Pre-loading S6A specifics AVPs
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Visited-PLMN-Id", &s6a_fd_cnf.dataobj_s6a_visited_plmn_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "RAT-Type", &s6a_fd_cnf.dataobj_s6a_rat_type, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "ULR-Flags", &s6a_fd_cnf.dataobj_s6a_ulr_flags, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "ULA-Flags", &s6a_fd_cnf.dataobj_s6a_ula_flags, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Subscription-Data", &s6a_fd_cnf.dataobj_s6a_subscription_data, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Requested-EUTRAN-Authentication-Info", &s6a_fd_cnf.dataobj_s6a_req_eutran_auth_info, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Number-Of-Requested-Vectors", &s6a_fd_cnf.dataobj_s6a_number_of_requested_vectors, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Immediate-Response-Preferred", &s6a_fd_cnf.dataobj_s6a_immediate_response_pref, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Authentication-Info", &s6a_fd_cnf.dataobj_s6a_authentication_info, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Re-Synchronization-Info", &s6a_fd_cnf.dataobj_s6a_re_synchronization_info, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Service-Selection", &s6a_fd_cnf.dataobj_s6a_service_selection, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "UE-SRVCC-Capability", &s6a_fd_cnf.dataobj_s6a_ue_srvcc_cap, ENOENT));
  /*
   * Register callbacks
   */
  memset (&when, 0, sizeof (when));
  when.command = s6a_fd_cnf.dataobj_s6a_aia;
  when.app = s6a_fd_cnf.dataobj_s6a_app;
  /*
   * Register the callback for Authentication Information Answer S6A Application
   */
  CHECK_FD_FCT (fd_disp_register (aia_cb, DISP_HOW_CC, &when, NULL, &s6a_fd_cnf.aia_hdl));
  DevAssert (s6a_fd_cnf.aia_hdl );
  /*
   * Advertise the support for the test application in the peer
   */
  CHECK_FD_FCT (fd_disp_app_support (s6a_fd_cnf.dataobj_s6a_app, s6a_fd_cnf.dataobj_s6a_vendor, 1, 0));
  return RETURNok;
}
//------------------------------------------------------------------------------
int auth_request_init (char * s6a_conf_file)
{
  int                                     ret;

  fprintf (stdout, "Initializing S6a interface\n");

  memset (&s6a_fd_cnf, 0, sizeof (s6a_fd_cnf_t));


  /*
   * Initializing freeDiameter logger
   */
  ret = fd_log_handler_register(oai_fd_logger);
  if (ret) {
    fprintf (stderr, "An error occurred during freeDiameter log handler registration: %d\n", ret);
    return ret;
  } else {
    fprintf (stdout, "Initializing freeDiameter log handler done\n");
  }

  /*
   * Initializing freeDiameter core
   */
  fprintf (stdout, "Initializing freeDiameter core...\n");
  ret = fd_core_initialize ();
  if (ret) {
    fprintf (stderr, "An error occurred during freeDiameter core library initialization: %d\n", ret);
    return ret;
  } else {
    fprintf (stdout, "Initializing freeDiameter core done\n");
  }


  fprintf (stdout, "Default ext path: %s\n", DEFAULT_EXTENSIONS_PATH);


  ret = fd_core_parseconf (s6a_conf_file);
  if (ret) {
    fprintf (stderr, "An error occurred during fd_core_parseconf file : %s.\n", s6a_conf_file);
    return ret;
  } else {
    fprintf (stdout, "fd_core_parseconf done\n");
  }

  /*
   * Starting freeDiameter core
   */
  ret = fd_core_start ();
  if (ret) {
    fprintf (stderr, "An error occurred during freeDiameter core library start\n");
    return ret;
  } else {
    fprintf (stdout, "fd_core_start done\n");
  }



  ret = fd_core_waitstartcomplete ();
  if (ret) {
    fprintf (stderr, "An error occurred during fd_core_waitstartcomplete.\n");
    return ret;
  } else {
    fprintf (stdout, "fd_core_waitstartcomplete done\n");
  }

  ret = fd_init_dict_objs ();
  if (ret) {
    fprintf (stderr, "An error occurred during fd_init_dict_objs.\n");
    return ret;
  } else {
    fprintf (stdout, "fd_init_dict_objs done\n");
  }

  /*
   * Trying to connect to peers
   */
  // No done in freeDiameter config file
  //CHECK_FCT (s6a_fd_new_peer ());

  fprintf (stdout, "Initializing S6a interface: DONE\n");

  return RETURNok;
}

//------------------------------------------------------------------------------
void auth_request_exit(void)
{
  int    rv = RETURNok;
  /* Initialize shutdown of the framework */
  rv = fd_core_shutdown();
  if (rv) {
    fprintf (stderr, "An error occurred during fd_core_shutdown().\n");
  }

  /* Wait for the shutdown to be complete -- this should always be called after fd_core_shutdown */
  rv = fd_core_wait_shutdown_complete();
  if (rv) {
    fprintf (stderr, "An error occurred during fd_core_wait_shutdown_complete().\n");
  }
}

//------------------------------------------------------------------------------
int
parse_experimental_result (
  struct avp *avp,
  s6a_experimental_result_t * ptr)
{
  struct avp_hdr                         *hdr;
  struct avp                             *child_avp = NULL;

  if (!avp) {
    return EINVAL;
  }

  CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
  DevAssert (hdr->avp_code == AVP_CODE_EXPERIMENTAL_RESULT);
  CHECK_FCT (fd_msg_browse (avp, MSG_BRW_FIRST_CHILD, &child_avp, NULL));

  while (child_avp) {
    CHECK_FCT (fd_msg_avp_hdr (child_avp, &hdr));

    switch (hdr->avp_code) {
    case AVP_CODE_EXPERIMENTAL_RESULT_CODE:
      fprintf (stderr, "Got experimental error %u:%s\n", hdr->avp_value->u32, experimental_retcode_2_string (hdr->avp_value->u32));

      if (ptr) {
        *ptr = (s6a_experimental_result_t) hdr->avp_value->u32;
      }

      break;

    case AVP_CODE_VENDOR_ID:
      DevCheck (hdr->avp_value->u32 == 10415, hdr->avp_value->u32, AVP_CODE_VENDOR_ID, 10415);
      break;

    default:
      return RETURNerror;
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (child_avp, MSG_BRW_NEXT, &child_avp, NULL));
  }

  return RETURNok;
}

//------------------------------------------------------------------------------
char                                   *
experimental_retcode_2_string (
  uint32_t ret_code)
{
  switch (ret_code) {
    /*
     * Experimental-Result-Codes
     */
  case DIAMETER_ERROR_USER_UNKNOWN:
    return "DIAMETER_ERROR_USER_UNKNOWN";

  case DIAMETER_ERROR_ROAMING_NOT_ALLOWED:
    return "DIAMETER_ERROR_ROAMING_NOT_ALLOWED";

  case DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION:
    return "DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION";

  case DIAMETER_ERROR_RAT_NOT_ALLOWED:
    return "DIAMETER_ERROR_RAT_NOT_ALLOWED";

  case DIAMETER_ERROR_EQUIPMENT_UNKNOWN:
    return "DIAMETER_ERROR_EQUIPMENT_UNKNOWN";

  case DIAMETER_ERROR_UNKOWN_SERVING_NODE:
    return "DIAMETER_ERROR_UNKOWN_SERVING_NODE";

  case DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE:
    return "DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE";

  default:
    break;
  }

  return "DIAMETER_AVP_UNSUPPORTED";
}

//------------------------------------------------------------------------------
char                                   *
retcode_2_string (
  uint32_t ret_code)
{
  switch (ret_code) {
  case ER_DIAMETER_SUCCESS:
    return "DIAMETER_SUCCESS";

  case ER_DIAMETER_MISSING_AVP:
    return "DIAMETER_MISSING_AVP";

  case ER_DIAMETER_INVALID_AVP_VALUE:
    return "DIAMETER_INVALID_AVP_VALUE";

  default:
    break;
  }

  return "DIAMETER_AVP_UNSUPPORTED";
}

//------------------------------------------------------------------------------
int
parse_rand (
  struct avp_hdr *hdr,
  uint8_t * rand_p)
{
  int                                     ret = 0;

  DevCheck (hdr->avp_value->os.len == RAND_LENGTH_OCTETS, RAND_LENGTH_OCTETS, hdr->avp_value->os.len, 0);
  DevAssert (rand_p );
  STRING_TO_RAND (hdr->avp_value->os.data, rand_p, ret);
  return ret;
}

//------------------------------------------------------------------------------
int
parse_xres (
  struct avp_hdr *hdr,
  res_t * xres)
{
  int                                     ret = 0;

  DevCheck (hdr->avp_value->os.len >= XRES_LENGTH_MIN && hdr->avp_value->os.len <= XRES_LENGTH_MAX, XRES_LENGTH_MIN, XRES_LENGTH_MAX, hdr->avp_value->os.len);
  DevAssert (xres );
  STRING_TO_XRES (hdr->avp_value->os.data, hdr->avp_value->os.len, xres, ret);
  return ret;
}

//------------------------------------------------------------------------------
int
parse_autn (
  struct avp_hdr *hdr,
  uint8_t * autn)
{
  int                                     ret = 0;

  DevCheck (hdr->avp_value->os.len == AUTN_LENGTH_OCTETS, AUTN_LENGTH_OCTETS, hdr->avp_value->os.len, 0);
  DevAssert (autn );
  STRING_TO_AUTN (hdr->avp_value->os.data, autn, ret);
  return ret;
}

//------------------------------------------------------------------------------
int
parse_kasme (
  struct avp_hdr *hdr,
  uint8_t * kasme)
{
  int                                     ret = 0;

  DevCheck (hdr->avp_value->os.len == KASME_LENGTH_OCTETS, KASME_LENGTH_OCTETS, hdr->avp_value->os.len, 0);
  DevAssert (kasme );
  STRING_TO_KASME (hdr->avp_value->os.data, kasme, ret);
  return ret;
}

static inline int
parse_e_utran_vector (
  struct avp *avp_vector,
  eutran_vector_t * vector)
{
  int                                     ret = 0x0f;
  struct avp                             *avp;
  struct avp_hdr                         *hdr;

  CHECK_FCT (fd_msg_avp_hdr (avp_vector, &hdr));
  DevCheck (hdr->avp_code == AVP_CODE_E_UTRAN_VECTOR, hdr->avp_code, AVP_CODE_E_UTRAN_VECTOR, 0);
  CHECK_FCT (fd_msg_browse (avp_vector, MSG_BRW_FIRST_CHILD, &avp, NULL));

  while (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    switch (hdr->avp_code) {
    case AVP_CODE_RAND:
      CHECK_FCT (parse_rand (hdr, vector->rand));
      ret &= ~0x01;
      break;

    case AVP_CODE_XRES:
      CHECK_FCT (parse_xres (hdr, &vector->xres));
      ret &= ~0x02;
      break;

    case AVP_CODE_AUTN:
      CHECK_FCT (parse_autn (hdr, vector->autn));
      ret &= ~0x04;
      break;

    case AVP_CODE_KASME:
      CHECK_FCT (parse_kasme (hdr, vector->kasme));
      ret &= ~0x08;
      break;

    default:
      /*
       * Unexpected AVP
       */
      fprintf (stderr, "Unexpected AVP with code %d\n", hdr->avp_code);
      return RETURNerror;
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (avp, MSG_BRW_NEXT, &avp, NULL));
  }

  if (ret) {
    fprintf (stderr, "Missing AVP for E-UTRAN vector: %c%c%c%c\n", ret & 0x01 ? 'R' : '-', ret & 0x02 ? 'X' : '-', ret & 0x04 ? 'A' : '-', ret & 0x08 ? 'K' : '-');
    return RETURNerror;
  }

  return RETURNok;
}
//------------------------------------------------------------------------------
int
parse_authentication_info_avp (
  struct avp *avp_auth_info,
  authentication_info_t * authentication_info)
{
  struct avp                             *avp;
  struct avp_hdr                         *hdr;

  CHECK_FCT (fd_msg_avp_hdr (avp_auth_info, &hdr));
  DevCheck (hdr->avp_code == AVP_CODE_AUTHENTICATION_INFO, hdr->avp_code, AVP_CODE_AUTHENTICATION_INFO, 0);
  authentication_info->nb_of_vectors = 0;
  CHECK_FCT (fd_msg_browse (avp_auth_info, MSG_BRW_FIRST_CHILD, &avp, NULL));

  while (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    switch (hdr->avp_code) {
    case AVP_CODE_E_UTRAN_VECTOR:{
        DevAssert (authentication_info->nb_of_vectors == 0);
        CHECK_FCT (parse_e_utran_vector (avp, &authentication_info->eutran_vector[0]));
        authentication_info->nb_of_vectors++;
      }
      break;

    default:
      /*
       * We should only receive E-UTRAN-Vectors
       */
      fprintf ( stderr, "Unexpected AVP with code %d\n", hdr->avp_code);
      return RETURNerror;
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (avp, MSG_BRW_NEXT, &avp, NULL));
  }

  return RETURNok;
}

//------------------------------------------------------------------------------
void auth_write_results(char *fileout, authentication_info_t *ai)
{
  FILE *fp = NULL;

  fp = fopen(fileout, "w+");
  for (int i = 0; i < ai->nb_of_vectors; i++) {
    fprintf(fp, "RAND[%u]=\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n", i, DISPLAY_128BITS(ai->eutran_vector[i].rand));
    fprintf(fp, "XRES[%u]=\"", i);
    for (int j = 0; j < ai->eutran_vector[i].xres.size; j++) {
      fprintf(fp, "%02x", ai->eutran_vector[i].xres.data[j]);
    }
    fprintf(fp, "\"\n");
    fprintf(fp, "AUTN[%u]=\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n", i, DISPLAY_128BITS(ai->eutran_vector[i].autn));
    fprintf(fp, "KASME[%u]=\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n",
        i, KASME_DISPLAY_1(ai->eutran_vector[i].kasme), KASME_DISPLAY_2(ai->eutran_vector[i].kasme));

    uint8_t kenb[AUTH_KENB_SIZE] = {0};
    derive_keNB (ai->eutran_vector[i].kasme, gul_nas_count, kenb);
    fprintf(fp, "KENB[%u]=\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n",
        i, KASME_DISPLAY_1(kenb), KASME_DISPLAY_2(kenb));

    uint8_t knas_int[AUTH_KNAS_INT_SIZE] = {0};
    uint8_t knas_enc[AUTH_KNAS_ENC_SIZE] = {0};

    derive_key_nas (NAS_INT_ALG, gintegrity,  ai->eutran_vector[i].kasme, knas_int);
    derive_key_nas (NAS_ENC_ALG, gencryption, ai->eutran_vector[i].kasme, knas_enc);
    fprintf(fp, "KNAS_INT[%u]=\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n", i, DISPLAY_128BITS(knas_int));
    fprintf(fp, "KNAS_ENC[%u]=\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n", i, DISPLAY_128BITS(knas_enc));
  }

  fflush(fp);
  fclose(fp);
}
//------------------------------------------------------------------------------
int
aia_cb (
  struct msg **msg,
  struct avp *paramavp,
  struct session *sess,
  void *opaque,
  enum disp_action *act)
{
  struct msg                             *ans = NULL;
  struct msg                             *qry = NULL;
  struct avp                             *avp = NULL;
  struct avp_hdr                         *hdr = NULL;
  s6a_auth_info_ans_t                     s6a_auth_info_ans = {.imsi = {0}, 0};
  s6a_auth_info_ans_t                    *s6a_auth_info_ans_p = &s6a_auth_info_ans;
  int                                     skip_auth_res = 0;

  DevAssert (msg );
  ans = *msg;
  /*
   * Retrieve the original query associated with the asnwer
   */
  CHECK_FCT (fd_msg_answ_getq (ans, &qry));
  DevAssert (qry );
  fprintf ( stdout, "Received S6A Authentication Information Answer (AIA)\n");
  CHECK_FCT (fd_msg_search_avp (qry, s6a_fd_cnf.dataobj_s6a_user_name, &avp));

  if (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
    sprintf (s6a_auth_info_ans_p->imsi, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  } else {
    DevMessage ("Query has been freed before we received the answer\n");
  }

  /*
   * Retrieve the result-code
   */
  CHECK_FCT (fd_msg_search_avp (ans, s6a_fd_cnf.dataobj_s6a_result_code, &avp));

  if (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));
    s6a_auth_info_ans_p->result.present = S6A_RESULT_BASE;
    s6a_auth_info_ans_p->result.choice.base = hdr->avp_value->u32;

    if (hdr->avp_value->u32 != ER_DIAMETER_SUCCESS) {
      fprintf ( stderr, "Got error %u:%s\n", hdr->avp_value->u32, retcode_2_string (hdr->avp_value->u32));
      goto err;
    } else {
      fprintf ( stdout, "Received S6A Result code %u:%s\n", s6a_auth_info_ans_p->result.choice.base, retcode_2_string (s6a_auth_info_ans_p->result.choice.base));
    }
  } else {
    /*
     * The result-code is not present, may be it is an experimental result
     * * * * avp indicating a 3GPP specific failure.
     */
    CHECK_FCT (fd_msg_search_avp (ans, s6a_fd_cnf.dataobj_s6a_experimental_result, &avp));

    if (avp) {
      /*
       * The procedure has failed within the HSS.
       * * * * NOTE: contrary to result-code, the experimental-result is a grouped
       * * * * AVP and requires parsing its childs to get the code back.
       */
      s6a_auth_info_ans_p->result.present = S6A_RESULT_EXPERIMENTAL;
      parse_experimental_result (avp, &s6a_auth_info_ans_p->result.choice.experimental);
      skip_auth_res = 1;
    } else {
      /*
       * Neither result-code nor experimental-result is present ->
       * * * * totally incorrect behaviour here.
       */
      fprintf ( stderr, "Experimental-Result and Result-Code are absent: " "This is not a correct behaviour\n");
      goto err;
    }
  }

  if (skip_auth_res == 0) {
    CHECK_FCT (fd_msg_search_avp (ans, s6a_fd_cnf.dataobj_s6a_authentication_info, &avp));

    if (avp) {
      CHECK_FCT (parse_authentication_info_avp (avp, &s6a_auth_info_ans_p->auth_info));
    } else {
      DevMessage ("We requested E-UTRAN vectors with an immediate response...\n");
    }
  }

  auth_write_results(gfileout, &s6a_auth_info_ans_p->auth_info);
  auth_request_exit();
  exit(0);
err:
  return RETURNok;
}

//------------------------------------------------------------------------------
int
generate_authentication_info_req (
  s6a_auth_info_req_t * air_p)
{
  struct avp                             *avp;
  struct msg                             *msg;
  struct session                         *sess;
  union avp_value                         value;

  DevAssert (air_p );
  /*
   * Create the new update location request message
   */
  CHECK_FCT (fd_msg_new (s6a_fd_cnf.dataobj_s6a_air, 0, &msg));
  /*
   * Create a new session
   */
  CHECK_FCT (fd_sess_new (&sess, fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len, (os0_t) "apps6a", 6));
  {
    os0_t                                   sid;
    size_t                                  sidlen;

    CHECK_FCT (fd_sess_getsid (sess, &sid, &sidlen));
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_session_id, 0, &avp));
    value.os.data = sid;
    value.os.len = sidlen;
    CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
    CHECK_FCT (fd_msg_avp_add (msg, MSG_BRW_FIRST_CHILD, avp));
  }
  CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_auth_session_state, 0, &avp));
  /*
   * No State maintained
   */
  value.i32 = 1;
  CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg, MSG_BRW_LAST_CHILD, avp));
  /*
   * Add Origin_Host & Origin_Realm
   */
  CHECK_FCT (fd_msg_add_origin (msg, 0));
  /*
   * Destination Host
   */
  {
    char                                    host[100];
    size_t                                  hostlen;

    memset (host, 0, 100);
    strcat (host, ghss_hostname);
    strcat (host, ".");
    strcat (host, grealm);
    hostlen = strlen (host);
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_destination_host, 0, &avp));
    value.os.data = (unsigned char *)host;
    value.os.len = hostlen;
    CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
    CHECK_FCT (fd_msg_avp_add (msg, MSG_BRW_LAST_CHILD, avp));
  }
  /*
   * Destination_Realm
   */
  {
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_destination_realm, 0, &avp));
    value.os.data = (unsigned char *)grealm;
    value.os.len = strlen (grealm);
    CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
    CHECK_FCT (fd_msg_avp_add (msg, MSG_BRW_LAST_CHILD, avp));
  }
  /*
   * Adding the User-Name (IMSI)
   */
  CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_user_name, 0, &avp));
  value.os.data = (unsigned char *)air_p->imsi;
  value.os.len = strlen (air_p->imsi);
  CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg, MSG_BRW_LAST_CHILD, avp));
  /*
   * Adding the visited plmn id
   */
  {
    uint8_t                                 plmn[3] = { 0x00, 0x00, 0x00 };     //{ 0x02, 0xF8, 0x29 };
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_visited_plmn_id, 0, &avp));
    PLMN_T_TO_TBCD (air_p->visited_plmn,
                    plmn,
                    find_mnc_length (
                        air_p->visited_plmn.mcc_digit1, air_p->visited_plmn.mcc_digit2, air_p->visited_plmn.mcc_digit3,
                        air_p->visited_plmn.mnc_digit1, air_p->visited_plmn.mnc_digit2, air_p->visited_plmn.mnc_digit3)
      );
    value.os.data = plmn;
    value.os.len = 3;
    CHECK_FCT (fd_msg_avp_setvalue (avp, &value));
    CHECK_FCT (fd_msg_avp_add (msg, MSG_BRW_LAST_CHILD, avp));
    fprintf ( stdout, "%s plmn: %02X%02X%02X\n", __FUNCTION__, plmn[0], plmn[1], plmn[2]);
    fprintf ( stdout, "%s visited_plmn: %02X%02X%02X\n", __FUNCTION__, value.os.data[0], value.os.data[1], value.os.data[2]);
  }
  /*
   * Adding the requested E-UTRAN authentication info AVP
   */
  {
    struct avp                             *child_avp;

    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_req_eutran_auth_info, 0, &avp));
    /*
     * Add the number of requested vectors
     */
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_number_of_requested_vectors, 0, &child_avp));
    value.u32 = air_p->nb_of_vectors;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp, MSG_BRW_LAST_CHILD, child_avp));
    /*
     * We want to use the vectors immediately in HSS so we have to add
     * * * * the Immediate-Response-Preferred AVP.
     * * * * Value of this AVP is not significant.
     */
    CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_immediate_response_pref, 0, &child_avp));
    value.u32 = 0;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp, MSG_BRW_LAST_CHILD, child_avp));

    /*
     * Re-synchronization information containing the AUTS computed at USIM
     */
    if (air_p->re_synchronization) {
      CHECK_FCT (fd_msg_avp_new (s6a_fd_cnf.dataobj_s6a_re_synchronization_info, 0, &child_avp));
      value.os.len = AUTS_LENGTH;
      value.os.data = air_p->resync_param;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    CHECK_FCT (fd_msg_avp_add (msg, MSG_BRW_LAST_CHILD, avp));
  }
  CHECK_FCT (fd_msg_send (&msg, NULL, NULL));
  return RETURNok;
}

//------------------------------------------------------------------------------
int
config_parse_opt_line (
  int argc,
  char *argv[])
{
  int                                     c;

  /*
   * Parsing command line
   */
  while ((c = getopt (argc, argv, "c:h:o:r")) != -1) {
    switch (c) {
      case 'c':{
          gfd_config_file = strdup(optarg);
          fprintf ( stdout, "%s freeDiameter config file %s\n", __FUNCTION__, gfd_config_file);
        }
        break;

      case 'h':{
          ghss_hostname = strdup(optarg);
          fprintf ( stdout, "%s HSS hostname %s\n", __FUNCTION__, ghss_hostname);
        }
        break;

      case 'o':{
          gfileout = strdup(optarg);
          fprintf ( stdout, "%s result file %s\n", __FUNCTION__, gfileout);
        }
        break;

      case 'r':{
          grealm = strdup(optarg);
          fprintf ( stdout, "%s realm %s\n", __FUNCTION__, grealm);
        }
        break;

      default:
        fprintf ( stdout, "%s realm %s\n", __FUNCTION__, grealm);
        exit (0);
    }
  }

  /*
   * Parse the configuration file using libconfig
   */
  AssertFatal ((gfd_config_file), "Mandatory argument -c freeDiameter config file");
  AssertFatal ((gfileout), "Mandatory argument -o Result output file");
  AssertFatal ((ghss_hostname), "Mandatory argument -h HSS hostname");
  AssertFatal ((grealm), "Mandatory argument -r realm ");
  return 0;
}
//------------------------------------------------------------------------------
int main (int argc, char** argv)
{
  s6a_auth_info_req_t  air           = {.resync_param = {0}, .imsi = {0}, .visited_plmn = {0}, 0};

  config_parse_opt_line(argc, argv);
  auth_request_init(gfd_config_file);
  generate_authentication_info_req (&air);
  sleep(5);
  // no response ? then exit
  auth_request_exit();
  return 0;
}
