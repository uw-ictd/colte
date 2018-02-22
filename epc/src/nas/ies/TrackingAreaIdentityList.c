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
#include "TrackingAreaIdentityList.h"

int
decode_tracking_area_identity_list (
  TrackingAreaIdentityList * trackingareaidentitylist,
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
  trackingareaidentitylist->typeoflist = (*(buffer + decoded) >> 5) & 0x3;
  trackingareaidentitylist->numberofelements = *(buffer + decoded) & 0x1f;
  decoded++;
  if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_CONSECUTIVE_TACS == trackingareaidentitylist->typeoflist) {
    trackingareaidentitylist->mccdigit2[0] = (*(buffer + decoded) >> 4) & 0xf;
    trackingareaidentitylist->mccdigit1[0] = *(buffer + decoded) & 0xf;
    decoded++;
    trackingareaidentitylist->mncdigit3[0] = (*(buffer + decoded) >> 4) & 0xf;
    trackingareaidentitylist->mccdigit3[0] = *(buffer + decoded) & 0xf;
    decoded++;
    trackingareaidentitylist->mncdigit2[0] = (*(buffer + decoded) >> 4) & 0xf;
    trackingareaidentitylist->mncdigit1[0] = *(buffer + decoded) & 0xf;
    decoded++;
    //IES_DECODE_U16(trackingareaidentitylist->tac, *(buffer + decoded));
    IES_DECODE_U16 (buffer, decoded, trackingareaidentitylist->tac[0]);
  } else if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS == trackingareaidentitylist->typeoflist) {
    int i;
    trackingareaidentitylist->mccdigit2[0] = (*(buffer + decoded) >> 4) & 0xf;
    trackingareaidentitylist->mccdigit1[0] = *(buffer + decoded) & 0xf;
    decoded++;
    trackingareaidentitylist->mncdigit3[0] = (*(buffer + decoded) >> 4) & 0xf;
    trackingareaidentitylist->mccdigit3[0] = *(buffer + decoded) & 0xf;
    decoded++;
    trackingareaidentitylist->mncdigit2[0] = (*(buffer + decoded) >> 4) & 0xf;
    trackingareaidentitylist->mncdigit1[0] = *(buffer + decoded) & 0xf;
    decoded++;
    for (i=0; i <= trackingareaidentitylist->numberofelements; i++) {
      IES_DECODE_U16 (buffer, decoded, trackingareaidentitylist->tac[i]);
    }
  } else if (TRACKING_AREA_IDENTITY_LIST_MANY_PLMNS == trackingareaidentitylist->typeoflist) {
    int i;
    for (i=0; i <= trackingareaidentitylist->numberofelements; i++) {
      trackingareaidentitylist->mccdigit2[i] = (*(buffer + decoded) >> 4) & 0xf;
      trackingareaidentitylist->mccdigit1[i] = *(buffer + decoded) & 0xf;
      decoded++;
      trackingareaidentitylist->mncdigit3[i] = (*(buffer + decoded) >> 4) & 0xf;
      trackingareaidentitylist->mccdigit3[i] = *(buffer + decoded) & 0xf;
      decoded++;
      trackingareaidentitylist->mncdigit2[i] = (*(buffer + decoded) >> 4) & 0xf;
      trackingareaidentitylist->mncdigit1[i] = *(buffer + decoded) & 0xf;
      decoded++;
      IES_DECODE_U16 (buffer, decoded, trackingareaidentitylist->tac[i]);
    }
  } else {
    OAILOG_DEBUG (LOG_NAS, "Type of TAIL list not handled %d", trackingareaidentitylist->typeoflist);
  }
#if NAS_DEBUG
  dump_tracking_area_identity_list_xml (trackingareaidentitylist, iei);
#endif
  return decoded;
}

int
encode_tracking_area_identity_list (
  TrackingAreaIdentityList * trackingareaidentitylist,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  uint8_t                                *lenPtr;
  uint32_t                                encoded = 0;

  /*
   * Checking IEI and pointer
   */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, TRACKING_AREA_IDENTITY_LIST_MINIMUM_LENGTH, len);
#if NAS_DEBUG
  dump_tracking_area_identity_list_xml (trackingareaidentitylist, iei);
#endif

  if (iei > 0) {
    *buffer = iei;
    encoded++;
  }

  lenPtr = (buffer + encoded);
  encoded++;

  *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->typeoflist & 0x3) << 5) | (trackingareaidentitylist->numberofelements & 0x1f);
  encoded++;

  if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_CONSECUTIVE_TACS == trackingareaidentitylist->typeoflist) {
    *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mccdigit2[0] & 0xf) << 4) | (trackingareaidentitylist->mccdigit1[0] & 0xf);
    encoded++;
    *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mncdigit3[0] & 0xf) << 4) | (trackingareaidentitylist->mccdigit3[0] & 0xf);
    encoded++;
    *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mncdigit2[0] & 0xf) << 4) | (trackingareaidentitylist->mncdigit1[0] & 0xf);
    encoded++;
    IES_ENCODE_U16 (buffer, encoded, trackingareaidentitylist->tac[0]);
  } else if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS == trackingareaidentitylist->typeoflist) {
    int i;
    *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mccdigit2[0] & 0xf) << 4) | (trackingareaidentitylist->mccdigit1[0] & 0xf);
    encoded++;
    *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mncdigit3[0] & 0xf) << 4) | (trackingareaidentitylist->mccdigit3[0] & 0xf);
    encoded++;
    *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mncdigit2[0] & 0xf) << 4) | (trackingareaidentitylist->mncdigit1[0] & 0xf);
    encoded++;
    for (i=0; i <= trackingareaidentitylist->numberofelements; i++) {
      IES_ENCODE_U16 (buffer, encoded, trackingareaidentitylist->tac[i]);
    }
  } else if (TRACKING_AREA_IDENTITY_LIST_MANY_PLMNS == trackingareaidentitylist->typeoflist) {
    int i;
    for (i=0; i <= trackingareaidentitylist->numberofelements; i++) {
      *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mccdigit2[i] & 0xf) << 4) | (trackingareaidentitylist->mccdigit1[i] & 0xf);
      encoded++;
      *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mncdigit3[i] & 0xf) << 4) | (trackingareaidentitylist->mccdigit3[i] & 0xf);
      encoded++;
      *(buffer + encoded) = 0x00 | ((trackingareaidentitylist->mncdigit2[i] & 0xf) << 4) | (trackingareaidentitylist->mncdigit1[i] & 0xf);
      encoded++;
      IES_ENCODE_U16 (buffer, encoded, trackingareaidentitylist->tac[i]);
    }
  } else {
    OAILOG_DEBUG (LOG_NAS, "Type of TAIL list not handled %d", trackingareaidentitylist->typeoflist);
  }
  *lenPtr = encoded - 1 - ((iei > 0) ? 1 : 0);
  return encoded;
}

void
dump_tracking_area_identity_list_xml (
  TrackingAreaIdentityList * trackingareaidentitylist,
  uint8_t iei)
{
  OAILOG_DEBUG (LOG_NAS, "<Tracking Area Identity List>\n");

  if (iei > 0)
    /*
     * Don't display IEI if = 0
     */
    OAILOG_DEBUG (LOG_NAS, "    <IEI>0x%X</IEI>\n", iei);

  OAILOG_DEBUG (LOG_NAS, "    <Type of list>%u</Type of list>\n", trackingareaidentitylist->typeoflist);
  /*
   * LW: number of elements is coded as N-1 (0 -> 1 element, 1 -> 2 elements...),
   * * * *  see 3GPP TS 24.301, section 9.9.3.33.1
   */
  OAILOG_DEBUG (LOG_NAS, "    <Number of elements>%u</Number of elements>\n", trackingareaidentitylist->numberofelements + 1);
  if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_CONSECUTIVE_TACS == trackingareaidentitylist->typeoflist) {
    OAILOG_DEBUG (LOG_NAS, "    <MCC digit 2>%u</MCC digit 2>\n", trackingareaidentitylist->mccdigit2[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MCC digit 1>%u</MCC digit 1>\n", trackingareaidentitylist->mccdigit1[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MNC digit 3>%u</MNC digit 3>\n", trackingareaidentitylist->mncdigit3[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MCC digit 3>%u</MCC digit 3>\n", trackingareaidentitylist->mccdigit3[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MNC digit 2>%u</MNC digit 2>\n", trackingareaidentitylist->mncdigit2[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MNC digit 1>%u</MNC digit 1>\n", trackingareaidentitylist->mncdigit1[0]);
    OAILOG_DEBUG (LOG_NAS, "    <TAC>0x%.4x</TAC>\n", trackingareaidentitylist->tac[0]);
  } else  if (TRACKING_AREA_IDENTITY_LIST_ONE_PLMN_NON_CONSECUTIVE_TACS == trackingareaidentitylist->typeoflist) {
    int i;
    OAILOG_DEBUG (LOG_NAS, "    <MCC digit 2>%u</MCC digit 2>\n", trackingareaidentitylist->mccdigit2[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MCC digit 1>%u</MCC digit 1>\n", trackingareaidentitylist->mccdigit1[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MNC digit 3>%u</MNC digit 3>\n", trackingareaidentitylist->mncdigit3[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MCC digit 3>%u</MCC digit 3>\n", trackingareaidentitylist->mccdigit3[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MNC digit 2>%u</MNC digit 2>\n", trackingareaidentitylist->mncdigit2[0]);
    OAILOG_DEBUG (LOG_NAS, "    <MNC digit 1>%u</MNC digit 1>\n", trackingareaidentitylist->mncdigit1[0]);
    for (i=0; i <= trackingareaidentitylist->numberofelements; i++) {
      OAILOG_DEBUG (LOG_NAS, "    <TAC>0x%.4x</TAC>\n", trackingareaidentitylist->tac[i]);
    }
  } else if (TRACKING_AREA_IDENTITY_LIST_MANY_PLMNS == trackingareaidentitylist->typeoflist) {
    int i;
    for (i=0; i <= trackingareaidentitylist->numberofelements; i++) {
      OAILOG_DEBUG (LOG_NAS, "    <MCC digit 2>%u</MCC digit 2>\n", trackingareaidentitylist->mccdigit2[i]);
      OAILOG_DEBUG (LOG_NAS, "    <MCC digit 1>%u</MCC digit 1>\n", trackingareaidentitylist->mccdigit1[i]);
      OAILOG_DEBUG (LOG_NAS, "    <MNC digit 3>%u</MNC digit 3>\n", trackingareaidentitylist->mncdigit3[i]);
      OAILOG_DEBUG (LOG_NAS, "    <MCC digit 3>%u</MCC digit 3>\n", trackingareaidentitylist->mccdigit3[i]);
      OAILOG_DEBUG (LOG_NAS, "    <MNC digit 2>%u</MNC digit 2>\n", trackingareaidentitylist->mncdigit2[i]);
      OAILOG_DEBUG (LOG_NAS, "    <MNC digit 1>%u</MNC digit 1>\n", trackingareaidentitylist->mncdigit1[i]);
      OAILOG_DEBUG (LOG_NAS, "    <TAC>0x%.4x</TAC>\n", trackingareaidentitylist->tac[i]);
    }
  } else {
    OAILOG_DEBUG (LOG_NAS, "Type of TAIL list not handled %d", trackingareaidentitylist->typeoflist);
  }
  OAILOG_DEBUG (LOG_NAS, "</Tracking Area Identity List>\n");
}
