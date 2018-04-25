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


#include "3gpp_24.007.h"
#include "3gpp_24.301.h"
#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "PdnDisconnectRequest.h"

int
decode_pdn_disconnect_request (
  pdn_disconnect_request_msg * pdn_disconnect_request,
  uint8_t * buffer,
  uint32_t len)
{
  uint32_t                                decoded = 0;
  int                                     decoded_result = 0;

  // Check if we got a NULL pointer and if buffer length is >= minimum length expected for the message.
  CHECK_PDU_POINTER_AND_LENGTH_DECODER (buffer, PDN_DISCONNECT_REQUEST_MINIMUM_LENGTH, len);

  /*
   * Decoding mandatory fields
   */
  if ((decoded_result = decode_u8_linked_eps_bearer_identity (&pdn_disconnect_request->linkedepsbeareridentity, 0, *(buffer + decoded) & 0x0f, len - decoded)) < 0)
    return decoded_result;

  decoded++;

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
    case PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_IEI:
      if ((decoded_result = decode_ProtocolConfigurationOptions (&pdn_disconnect_request->protocolconfigurationoptions, PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_IEI, buffer + decoded, len - decoded)) <= 0)
        return decoded_result;

      decoded += decoded_result;
      /*
       * Set corresponding mask to 1 in presencemask
       */
      pdn_disconnect_request->presencemask |= PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
      break;

    default:
      errorCodeDecoder = TLV_UNEXPECTED_IEI;
      return TLV_UNEXPECTED_IEI;
    }
  }

  return decoded;
}

int
encode_pdn_disconnect_request (
  pdn_disconnect_request_msg * pdn_disconnect_request,
  uint8_t * buffer,
  uint32_t len)
{
  int                                     encoded = 0;
  int                                     encode_result = 0;

  /*
   * Checking IEI and pointer
   */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, PDN_DISCONNECT_REQUEST_MINIMUM_LENGTH, len);
  *(buffer + encoded) = (encode_u8_linked_eps_bearer_identity (&pdn_disconnect_request->linkedepsbeareridentity) & 0x0f);
  encoded++;

  if ((pdn_disconnect_request->presencemask & PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
      == PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT) {
    if ((encode_result = encode_ProtocolConfigurationOptions (&pdn_disconnect_request->protocolconfigurationoptions, PDN_DISCONNECT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_IEI, buffer + encoded, len - encoded)) < 0)
      // Return in case of error
      return encode_result;
    else
      encoded += encode_result;
  }

  return encoded;
}
