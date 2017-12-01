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
#include <string.h>


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "UeNetworkCapability.h"
#include "log.h"

int
decode_ue_network_capability (
  UeNetworkCapability * uenetworkcapability,
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
  memset (uenetworkcapability, 0, sizeof (UeNetworkCapability));
  OAILOG_INFO (LOG_NAS_EMM, "decode_ue_network_capability len = %d\n", ielen);
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
      OAILOG_INFO (LOG_NAS_EMM, "uenetworkcapability decoded UMTS\n");

      if (ielen > 4) {
        uenetworkcapability->spare = (*(buffer + decoded) >> 5) & 0x7;
        uenetworkcapability->csfb = (*(buffer + decoded) >> 4) & 0x1;
        uenetworkcapability->lpp = (*(buffer + decoded) >> 3) & 0x1;
        uenetworkcapability->lcs = (*(buffer + decoded) >> 2) & 0x1;
        uenetworkcapability->srvcc = (*(buffer + decoded) >> 1) & 0x1;
        uenetworkcapability->nf = *(buffer + decoded) & 0x1;
        decoded++;
        uenetworkcapability->misc_present = 1;
        OAILOG_INFO (LOG_NAS_EMM, "uenetworkcapability decoded misc flags\n");
      }
    }
  }

  OAILOG_INFO (LOG_NAS_EMM, "uenetworkcapability decoded=%u\n", decoded);

  if ((ielen + 2) != decoded) {
    decoded = ielen + 1 + (iei > 0 ? 1 : 0) /* Size of header for this IE */ ;
    OAILOG_INFO (LOG_NAS_EMM, "uenetworkcapability then decoded=%u\n", decoded);
  }
#if NAS_DEBUG
  dump_ue_network_capability_xml (uenetworkcapability, iei);
#endif
  return decoded;
}

int
encode_ue_network_capability (
  UeNetworkCapability * uenetworkcapability,
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
#if NAS_DEBUG
  dump_ue_network_capability_xml (uenetworkcapability, iei);
#endif

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
  OAILOG_INFO (LOG_NAS_EMM, "uenetworkcapability encoded EPS %u\n", encoded);

  if (uenetworkcapability->umts_present) {
    *(buffer + encoded) = uenetworkcapability->uea;
    encoded++;
    *(buffer + encoded) = 0x00 | ((uenetworkcapability->ucs2 & 0x1) << 7) | (uenetworkcapability->uia & 0x7f);
    encoded++;
    OAILOG_INFO (LOG_NAS_EMM, "uenetworkcapability encoded UMTS %u\n", encoded);
  }

  if (uenetworkcapability->misc_present) {
    *(buffer + encoded) =  ((uenetworkcapability->spare & 0x7) << 5) | // spare coded as zero
        ((uenetworkcapability->csfb  & 0x1) << 4) |
        ((uenetworkcapability->lpp   & 0x1) << 3) |
        ((uenetworkcapability->lcs   & 0x1) << 2) |
        ((uenetworkcapability->srvcc & 0x1) << 1) |
        (uenetworkcapability->nf     & 0x1);
    encoded++;
    OAILOG_INFO (LOG_NAS_EMM, "uenetworkcapability encoded misc flags %u\n", encoded);
  }

  *lenPtr = encoded - 1 - ((iei > 0) ? 1 : 0);
  return encoded;
}

void
dump_ue_network_capability_xml (
  UeNetworkCapability * uenetworkcapability,
  uint8_t iei)
{
  OAILOG_DEBUG (LOG_NAS, "<Ue Network Capability>\n");

  if (iei > 0)
    /*
     * Don't display IEI if = 0
     */
    OAILOG_DEBUG (LOG_NAS, "    <IEI>0x%X</IEI>\n", iei);

  OAILOG_DEBUG (LOG_NAS, "    <EEA>%02x</EEA>\n", uenetworkcapability->eea);
  OAILOG_DEBUG (LOG_NAS, "    <EIA>%02x</EIA>\n", uenetworkcapability->eia);
  OAILOG_DEBUG (LOG_NAS, "    <UEA>%02x</UEA>\n", uenetworkcapability->uea);
  if (uenetworkcapability->umts_present) {
    OAILOG_DEBUG (LOG_NAS, "    <UCS2>%u</UCS2>\n", uenetworkcapability->ucs2);
    OAILOG_DEBUG (LOG_NAS, "    <UIA>%u</UIA>\n", uenetworkcapability->uia);
  }
  if (uenetworkcapability->misc_present) {
    OAILOG_DEBUG (LOG_NAS, "    <SPARE>%u</SPARE>\n", uenetworkcapability->spare);
    OAILOG_DEBUG (LOG_NAS, "    <CSFB>%u</CSFB>\n", uenetworkcapability->csfb);
    OAILOG_DEBUG (LOG_NAS, "    <LPP>%u</LPP>\n", uenetworkcapability->lpp);
    OAILOG_DEBUG (LOG_NAS, "    <LCS>%u</LCS>\n", uenetworkcapability->lcs);
    OAILOG_DEBUG (LOG_NAS, "    <SRVCC>%u</SRVCC>\n", uenetworkcapability->srvcc);
    OAILOG_DEBUG (LOG_NAS, "    <NF>%u<NF/>\n", uenetworkcapability->nf);
  }
  OAILOG_DEBUG (LOG_NAS, "</Ue Network Capability>\n");
}
