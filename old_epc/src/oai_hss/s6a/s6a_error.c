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


#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"

char                                   *
experimental_retcode_2_string (
  int ret_code)
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
  int ret_code)
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
