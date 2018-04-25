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
#include <stdbool.h>

#include "bstrlib.h"

#include "log.h"
#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "UeNetworkCapability.h"


//------------------------------------------------------------------------------
int decode_ue_network_capability (
  ue_network_capability_t * uenetworkcapability,
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

  DECODE_U8 (buffer + decoded, ielen, decoded);
  memset (uenetworkcapability, 0, sizeof (ue_network_capability_t));
  OAILOG_TRACE (LOG_NAS_EMM, "decode_ue_network_capability len = %d\n", ielen);
  CHECK_LENGTH_DECODER (len - decoded, ielen);
  uenetworkcapability->eea = *(buffer + decoded);
  decoded++;
  uenetworkcapability->eia = *(buffer + decoded);
  decoded++;

  /*
   * Parts below not mandatory and may not be present
   */
  if (ielen > 2) {
    uenetworkcapability->uea = *(buffer + decoded);
    decoded++;

    if (ielen > 3) {
      uenetworkcapability->ucs2 = (*(buffer + decoded) >> 7) & 0x1;
      uenetworkcapability->uia = *(buffer + decoded) & 0x7f;
      decoded++;
      uenetworkcapability->umts_present = 1;
      OAILOG_TRACE (LOG_NAS_EMM, "uenetworkcapability decoded UMTS\n");

      if (ielen > 4) {
        uenetworkcapability->spare = (*(buffer + decoded) >> 5) & 0x7;
        uenetworkcapability->csfb = (*(buffer + decoded) >> 4) & 0x1;
        uenetworkcapability->lpp = (*(buffer + decoded) >> 3) & 0x1;
        uenetworkcapability->lcs = (*(buffer + decoded) >> 2) & 0x1;
        uenetworkcapability->srvcc = (*(buffer + decoded) >> 1) & 0x1;
        uenetworkcapability->nf = *(buffer + decoded) & 0x1;
        decoded++;
        uenetworkcapability->misc_present = 1;
        OAILOG_TRACE (LOG_NAS_EMM, "uenetworkcapability decoded misc flags\n");
      }
    }
  }

  OAILOG_TRACE (LOG_NAS_EMM, "uenetworkcapability decoded=%u\n", decoded);

  if ((ielen + 2) != decoded) {
    decoded = ielen + 1 + (iei > 0 ? 1 : 0) /* Size of header for this IE */ ;
    OAILOG_TRACE (LOG_NAS_EMM, "uenetworkcapability then decoded=%u\n", decoded);
  }
  return decoded;
}

//------------------------------------------------------------------------------
int encode_ue_network_capability (
  ue_network_capability_t * uenetworkcapability,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  uint8_t                                *lenPtr;
  uint32_t                                encoded = 0;

  /*
   * Checking IEI and pointer
   */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, UE_NETWORK_CAPABILITY_MINIMUM_LENGTH, len);

  if (iei > 0) {
    *buffer = iei;
    encoded++;
  }

  lenPtr = (buffer + encoded);
  encoded++;
  *(buffer + encoded) = uenetworkcapability->eea;
  encoded++;
  *(buffer + encoded) = uenetworkcapability->eia;
  encoded++;
  OAILOG_TRACE (LOG_NAS_EMM, "uenetworkcapability encoded EPS %u\n", encoded);

  if (uenetworkcapability->umts_present) {
    *(buffer + encoded) = uenetworkcapability->uea;
    encoded++;
    *(buffer + encoded) = 0x00 | ((uenetworkcapability->ucs2 & 0x1) << 7) | (uenetworkcapability->uia & 0x7f);
    encoded++;
    OAILOG_TRACE (LOG_NAS_EMM, "uenetworkcapability encoded UMTS %u\n", encoded);
  }

  if (uenetworkcapability->misc_present) {
    *(buffer + encoded) =  ((uenetworkcapability->spare & 0x7) << 5) | // spare coded as zero
        ((uenetworkcapability->csfb  & 0x1) << 4) |
        ((uenetworkcapability->lpp   & 0x1) << 3) |
        ((uenetworkcapability->lcs   & 0x1) << 2) |
        ((uenetworkcapability->srvcc & 0x1) << 1) |
        (uenetworkcapability->nf     & 0x1);
    encoded++;
    OAILOG_TRACE (LOG_NAS_EMM, "uenetworkcapability encoded misc flags %u\n", encoded);
  }

  *lenPtr = encoded - 1 - ((iei > 0) ? 1 : 0);
  return encoded;
}

