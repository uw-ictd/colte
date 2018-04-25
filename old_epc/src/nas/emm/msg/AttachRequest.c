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
#include <string.h>
#include <stdint.h>


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "AttachRequest.h"

int
decode_attach_request (
  attach_request_msg * attach_request,
  uint8_t * buffer,
  uint32_t len)
{
  uint32_t                                decoded = 0;
  int                                     decoded_result = 0;

  // Check if we got a NULL pointer and if buffer length is >= minimum length expected for the message.
  CHECK_PDU_POINTER_AND_LENGTH_DECODER (buffer, ATTACH_REQUEST_MINIMUM_LENGTH, len);

  /*
   * Decoding mandatory fields
   */
  if ((decoded_result = decode_u8_eps_attach_type (&attach_request->epsattachtype, 0, *(buffer + decoded) & 0x0f, len - decoded)) < 0) {
    //         return decoded_result;
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoded_result);
  }

  if ((decoded_result = decode_u8_nas_key_set_identifier (&attach_request->naskeysetidentifier, 0, *(buffer + decoded) >> 4, len - decoded)) < 0) {
    return decoded_result;
  }

  decoded++;

  if ((decoded_result = decode_eps_mobile_identity (&attach_request->oldgutiorimsi, 0, buffer + decoded, len - decoded)) < 0) {
    return decoded_result;
  } else
    decoded += decoded_result;

  if ((decoded_result = decode_ue_network_capability (&attach_request->uenetworkcapability, 0, buffer + decoded, len - decoded)) < 0) {
    return decoded_result;
  } else
    decoded += decoded_result;

  if ((decoded_result = decode_esm_message_container (&attach_request->esmmessagecontainer, 0, buffer + decoded, len - decoded)) < 0) {
    return decoded_result;
  } else
    decoded += decoded_result;

  /*
   * Decoding optional fields
   */
  while (len - decoded > 0) {
    uint8_t                                 ieiDecoded = *(buffer + decoded);

    /*
     * Type | value iei are below 0x80 so just return the first 4 bits
     */
    if (ieiDecoded >= 0x80)
      ieiDecoded = ieiDecoded & 0xf0;

    switch (ieiDecoded) {
    case ATTACH_REQUEST_OLD_PTMSI_SIGNATURE_IEI:
      if ((decoded_result = decode_p_tmsi_signature (&attach_request->oldptmsisignature, ATTACH_REQUEST_OLD_PTMSI_SIGNATURE_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_OLD_PTMSI_SIGNATURE_PRESENT;
      break;

    case ATTACH_REQUEST_ADDITIONAL_GUTI_IEI:
      if ((decoded_result = decode_eps_mobile_identity (&attach_request->additionalguti, ATTACH_REQUEST_ADDITIONAL_GUTI_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_ADDITIONAL_GUTI_PRESENT;
      break;

    case ATTACH_REQUEST_LAST_VISITED_REGISTERED_TAI_IEI:
      if ((decoded_result = decode_tracking_area_identity (&attach_request->lastvisitedregisteredtai, ATTACH_REQUEST_LAST_VISITED_REGISTERED_TAI_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT;
      break;

    case ATTACH_REQUEST_DRX_PARAMETER_IEI:
      if ((decoded_result = decode_drx_parameter (&attach_request->drxparameter, ATTACH_REQUEST_DRX_PARAMETER_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_DRX_PARAMETER_PRESENT;
      break;

    case ATTACH_REQUEST_MS_NETWORK_CAPABILITY_IEI:
      if ((decoded_result = decode_ms_network_capability (&attach_request->msnetworkcapability, ATTACH_REQUEST_MS_NETWORK_CAPABILITY_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_MS_NETWORK_CAPABILITY_PRESENT;
      break;

    case ATTACH_REQUEST_OLD_LOCATION_AREA_IDENTIFICATION_IEI:
      if ((decoded_result = decode_location_area_identification (&attach_request->oldlocationareaidentification, ATTACH_REQUEST_OLD_LOCATION_AREA_IDENTIFICATION_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_OLD_LOCATION_AREA_IDENTIFICATION_PRESENT;
      break;

    case ATTACH_REQUEST_TMSI_STATUS_IEI:
      if ((decoded_result = decode_tmsi_status (&attach_request->tmsistatus, ATTACH_REQUEST_TMSI_STATUS_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_TMSI_STATUS_PRESENT;
      break;

    case ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_2_IEI:
      if ((decoded_result = decode_mobile_station_classmark_2 (&attach_request->mobilestationclassmark2, ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_2_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_2_PRESENT;
      break;

    case ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_3_IEI:
      if ((decoded_result = decode_mobile_station_classmark_3 (&attach_request->mobilestationclassmark3, ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_3_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_3_PRESENT;
      break;

    case ATTACH_REQUEST_SUPPORTED_CODECS_IEI:
      if ((decoded_result = decode_supported_codec_list (&attach_request->supportedcodecs, ATTACH_REQUEST_SUPPORTED_CODECS_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_SUPPORTED_CODECS_PRESENT;
      break;

    case ATTACH_REQUEST_ADDITIONAL_UPDATE_TYPE_IEI:
      if ((decoded_result = decode_additional_update_type (&attach_request->additionalupdatetype, ATTACH_REQUEST_ADDITIONAL_UPDATE_TYPE_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_ADDITIONAL_UPDATE_TYPE_PRESENT;
      break;

    case ATTACH_REQUEST_OLD_GUTI_TYPE_IEI:
      if ((decoded_result = decode_guti_type (&attach_request->oldgutitype, ATTACH_REQUEST_OLD_GUTI_TYPE_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_OLD_GUTI_TYPE_PRESENT;
      break;

    case ATTACH_REQUEST_VOICE_DOMAIN_PREFERENCE_AND_UE_USAGE_SETTING_IEI:
      if ((decoded_result =
           decode_voice_domain_preference_and_ue_usage_setting (&attach_request->voicedomainpreferenceandueusagesetting, ATTACH_REQUEST_VOICE_DOMAIN_PREFERENCE_AND_UE_USAGE_SETTING_IEI, buffer + decoded, len - decoded)) <= 0) {
        return decoded_result;
      }

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      attach_request->presencemask |= ATTACH_REQUEST_VOICE_DOMAIN_PREFERENCE_AND_UE_USAGE_SETTING_PRESENT;
      break;

    case ATTACH_REQUEST_MS_NETWORK_FEATURE_SUPPORT_IEI: 
      if ((decoded_result =
        decode_ms_network_feature_support(&attach_request->msnetworkfeaturesupport,
            ATTACH_REQUEST_MS_NETWORK_FEATURE_SUPPORT_IEI,
            buffer + decoded, len - decoded)) <= 0) {
        //         return decoded_result;
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoded_result);
      }
      
      decoded += decoded_result;
      /* Set corresponding mask to 1 in presencemask */
      attach_request->presencemask |= ATTACH_REQUEST_MS_NETWORK_FEATURE_SUPPORT_PRESENT;
      break;
    default:
      errorCodeDecoder = TLV_UNEXPECTED_IEI;
      {
        return TLV_UNEXPECTED_IEI;
      }
    }
  }

  return decoded;
}

int
encode_attach_request (
  attach_request_msg * attach_request,
  uint8_t * buffer,
  uint32_t len)
{
  int                                     encoded = 0;
  int                                     encode_result = 0;

  /*
   * Checking IEI and pointer
   */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, ATTACH_REQUEST_MINIMUM_LENGTH, len);
  *(buffer + encoded) = ((encode_u8_nas_key_set_identifier (&attach_request->naskeysetidentifier) & 0x0f) << 4) | (encode_u8_eps_attach_type (&attach_request->epsattachtype) & 0x0f);
  encoded++;

  if ((encode_result = encode_eps_mobile_identity (&attach_request->oldgutiorimsi, 0, buffer + encoded, len - encoded)) < 0)    //Return in case of error
    return encode_result;
  else
    encoded += encode_result;

  if ((encode_result = encode_ue_network_capability (&attach_request->uenetworkcapability, 0, buffer + encoded, len - encoded)) < 0)    //Return in case of error
    return encode_result;
  else
    encoded += encode_result;

  if ((encode_result = encode_esm_message_container (attach_request->esmmessagecontainer, 0, buffer + encoded, len - encoded)) < 0)    //Return in case of error
    return encode_result;
  else
    encoded += encode_result;

  if ((attach_request->presencemask & ATTACH_REQUEST_OLD_PTMSI_SIGNATURE_PRESENT)
      == ATTACH_REQUEST_OLD_PTMSI_SIGNATURE_PRESENT) {
    if ((encode_result = encode_p_tmsi_signature (attach_request->oldptmsisignature, ATTACH_REQUEST_OLD_PTMSI_SIGNATURE_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_ADDITIONAL_GUTI_PRESENT)
      == ATTACH_REQUEST_ADDITIONAL_GUTI_PRESENT) {
    if ((encode_result = encode_eps_mobile_identity (&attach_request->additionalguti, ATTACH_REQUEST_ADDITIONAL_GUTI_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT)
      == ATTACH_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT) {
    if ((encode_result = encode_tracking_area_identity (&attach_request->lastvisitedregisteredtai, ATTACH_REQUEST_LAST_VISITED_REGISTERED_TAI_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_DRX_PARAMETER_PRESENT)
      == ATTACH_REQUEST_DRX_PARAMETER_PRESENT) {
    if ((encode_result = encode_drx_parameter (&attach_request->drxparameter, ATTACH_REQUEST_DRX_PARAMETER_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_MS_NETWORK_CAPABILITY_PRESENT)
      == ATTACH_REQUEST_MS_NETWORK_CAPABILITY_PRESENT) {
    if ((encode_result = encode_ms_network_capability (&attach_request->msnetworkcapability, ATTACH_REQUEST_MS_NETWORK_CAPABILITY_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_OLD_LOCATION_AREA_IDENTIFICATION_PRESENT)
      == ATTACH_REQUEST_OLD_LOCATION_AREA_IDENTIFICATION_PRESENT) {
    if ((encode_result = encode_location_area_identification (&attach_request->oldlocationareaidentification, ATTACH_REQUEST_OLD_LOCATION_AREA_IDENTIFICATION_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_TMSI_STATUS_PRESENT)
      == ATTACH_REQUEST_TMSI_STATUS_PRESENT) {
    if ((encode_result = encode_tmsi_status (&attach_request->tmsistatus, ATTACH_REQUEST_TMSI_STATUS_IEI, buffer + encoded, len - encoded))
        < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_2_PRESENT)
      == ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_2_PRESENT) {
    if ((encode_result = encode_mobile_station_classmark_2 (&attach_request->mobilestationclassmark2, ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_2_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_3_PRESENT)
      == ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_3_PRESENT) {
    if ((encode_result = encode_mobile_station_classmark_3 (&attach_request->mobilestationclassmark3, ATTACH_REQUEST_MOBILE_STATION_CLASSMARK_3_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_SUPPORTED_CODECS_PRESENT)
      == ATTACH_REQUEST_SUPPORTED_CODECS_PRESENT) {
    if ((encode_result = encode_supported_codec_list (&attach_request->supportedcodecs, ATTACH_REQUEST_SUPPORTED_CODECS_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_ADDITIONAL_UPDATE_TYPE_PRESENT)
      == ATTACH_REQUEST_ADDITIONAL_UPDATE_TYPE_PRESENT) {
    if ((encode_result = encode_additional_update_type (&attach_request->additionalupdatetype, ATTACH_REQUEST_ADDITIONAL_UPDATE_TYPE_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_OLD_GUTI_TYPE_PRESENT)
      == ATTACH_REQUEST_OLD_GUTI_TYPE_PRESENT) {
    if ((encode_result = encode_guti_type (&attach_request->oldgutitype, ATTACH_REQUEST_OLD_GUTI_TYPE_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_VOICE_DOMAIN_PREFERENCE_AND_UE_USAGE_SETTING_PRESENT)
      == ATTACH_REQUEST_VOICE_DOMAIN_PREFERENCE_AND_UE_USAGE_SETTING_PRESENT) {
    if ((encode_result = encode_voice_domain_preference_and_ue_usage_setting (&attach_request->voicedomainpreferenceandueusagesetting, ATTACH_REQUEST_VOICE_DOMAIN_PREFERENCE_AND_UE_USAGE_SETTING_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  if ((attach_request->presencemask & ATTACH_REQUEST_MS_NETWORK_FEATURE_SUPPORT_PRESENT)
      == ATTACH_REQUEST_MS_NETWORK_FEATURE_SUPPORT_PRESENT) {
    if ((encode_result =
	 encode_ms_network_feature_support(&attach_request->msnetworkfeaturesupport,
					   ATTACH_REQUEST_MS_NETWORK_FEATURE_SUPPORT_IEI, 
					   buffer + encoded, 
					   len -  encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }
  
  return encoded;
}
