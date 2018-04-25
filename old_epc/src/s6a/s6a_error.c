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


#include <stdint.h>

#include "assertions.h"
#include "log.h"
#include "common_defs.h"
#include "common_types.h"
#include "s6a_defs.h"

int
s6a_parse_experimental_result (
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
      OAILOG_ERROR (LOG_S6A, "Got experimental error %u:%s\n", hdr->avp_value->u32, experimental_retcode_2_string (hdr->avp_value->u32));

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
  
  case ER_DIAMETER_AUTHORIZATION_REJECTED:
    return "DIAMETER_AUTHORIZATION_REJECTED";

  default:
    break;
  }

  return "DIAMETER_AVP_UNSUPPORTED";
}
