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
#include <stdint.h>


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "ApnAggregateMaximumBitRate.h"

int
decode_apn_aggregate_maximum_bit_rate (
  ApnAggregateMaximumBitRate * apnaggregatemaximumbitrate,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  int                                     decoded = 0;
  uint8_t                                 ielen = 0;

  if (iei > 0) {
    CHECK_IEI_DECODER (iei, *buffer);
    decoded++;
  }

  ielen = *(buffer + decoded);
  decoded++;
  CHECK_LENGTH_DECODER (len - decoded, ielen);
  apnaggregatemaximumbitrate->apnambrfordownlink = *(buffer + decoded);
  decoded++;
  apnaggregatemaximumbitrate->apnambrforuplink = *(buffer + decoded);
  decoded++;

  if (ielen >= 4) {
    apnaggregatemaximumbitrate->apnambrfordownlink_extended = *(buffer + decoded);
    decoded++;
    apnaggregatemaximumbitrate->apnambrforuplink_extended = *(buffer + decoded);
    decoded++;

    if (ielen >= 6) {
      apnaggregatemaximumbitrate->apnambrfordownlink_extended2 = *(buffer + decoded);
      decoded++;
      apnaggregatemaximumbitrate->apnambrforuplink_extended2 = *(buffer + decoded);
      decoded++;
    }
  }
  return decoded;
}

int
encode_apn_aggregate_maximum_bit_rate (
  ApnAggregateMaximumBitRate * apnaggregatemaximumbitrate,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  uint8_t                                *lenPtr;
  uint32_t                                encoded = 0;

  /*
   * Checking IEI and pointer
   */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, APN_AGGREGATE_MAXIMUM_BIT_RATE_MINIMUM_LENGTH, len);

  if (iei > 0) {
    *buffer = iei;
    encoded++;
  }

  lenPtr = (buffer + encoded);
  encoded++;
  *(buffer + encoded) = apnaggregatemaximumbitrate->apnambrfordownlink;
  encoded++;
  *(buffer + encoded) = apnaggregatemaximumbitrate->apnambrforuplink;
  encoded++;

  if (apnaggregatemaximumbitrate->extensions & APN_AGGREGATE_MAXIMUM_BIT_RATE_MAXIMUM_EXTENSION_PRESENT) {
    *(buffer + encoded) = apnaggregatemaximumbitrate->apnambrfordownlink_extended;
    encoded++;
    *(buffer + encoded) = apnaggregatemaximumbitrate->apnambrforuplink_extended;
    encoded++;

    if (apnaggregatemaximumbitrate->extensions & APN_AGGREGATE_MAXIMUM_BIT_RATE_MAXIMUM_EXTENSION2_PRESENT) {
      *(buffer + encoded) = apnaggregatemaximumbitrate->apnambrfordownlink_extended2;
      encoded++;
      *(buffer + encoded) = apnaggregatemaximumbitrate->apnambrforuplink_extended2;
      encoded++;
    }
  }

  *lenPtr = encoded - 1 - ((iei > 0) ? 1 : 0);
  return encoded;
}

void
dump_apn_aggregate_maximum_bit_rate_xml (
  ApnAggregateMaximumBitRate * apnaggregatemaximumbitrate,
  uint8_t iei)
{
  OAILOG_DEBUG (LOG_NAS, "<Apn Aggregate Maximum Bit Rate>\n");

  if (iei > 0)
    /*
     * Don't display IEI if = 0
     */
    OAILOG_DEBUG (LOG_NAS, "    <IEI>0x%X</IEI>\n", iei);

  OAILOG_DEBUG (LOG_NAS, "    <APN AMBR for downlink>%u</APN AMBR for downlink>\n", apnaggregatemaximumbitrate->apnambrfordownlink);
  OAILOG_DEBUG (LOG_NAS, "    <APN AMBR for uplink>%u</APN AMBR for uplink>\n", apnaggregatemaximumbitrate->apnambrforuplink);

  if (apnaggregatemaximumbitrate->extensions & APN_AGGREGATE_MAXIMUM_BIT_RATE_MAXIMUM_EXTENSION_PRESENT) {
    OAILOG_DEBUG (LOG_NAS, "    <APN AMBR extended for downlink>%u</APN AMBR for downlink>\n", apnaggregatemaximumbitrate->apnambrfordownlink_extended);
    OAILOG_DEBUG (LOG_NAS, "    <APN AMBR extended for uplink>%u</APN AMBR for uplink>\n", apnaggregatemaximumbitrate->apnambrforuplink_extended);

    if (apnaggregatemaximumbitrate->extensions & APN_AGGREGATE_MAXIMUM_BIT_RATE_MAXIMUM_EXTENSION2_PRESENT) {
      OAILOG_DEBUG (LOG_NAS, "    <APN AMBR extended2 for downlink>%u</APN AMBR for downlink>\n", apnaggregatemaximumbitrate->apnambrfordownlink_extended);
      OAILOG_DEBUG (LOG_NAS, "    <APN AMBR extended2 for uplink>%u</APN AMBR for uplink>\n", apnaggregatemaximumbitrate->apnambrforuplink_extended);
    }
  }

  OAILOG_DEBUG (LOG_NAS, "</Apn Aggregate Maximum Bit Rate>\n");
}
