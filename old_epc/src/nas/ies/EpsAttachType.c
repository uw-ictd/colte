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
#include "EpsAttachType.h"

int
decode_eps_attach_type (
  EpsAttachType * epsattachtype,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  int                                     decoded = 0;

  CHECK_PDU_POINTER_AND_LENGTH_DECODER (buffer, EPS_ATTACH_TYPE_MINIMUM_LENGTH, len);

  if (iei > 0) {
    CHECK_IEI_DECODER ((*buffer & 0xf0), iei);
  }

  *epsattachtype = *buffer & 0x7;
  decoded++;
  return decoded;
}

int
decode_u8_eps_attach_type (
  EpsAttachType * epsattachtype,
  uint8_t iei,
  uint8_t value,
  uint32_t len)
{
  int                                     decoded = 0;
  uint8_t                                *buffer = &value;

  *epsattachtype = *buffer & 0x7;
  decoded++;
  return decoded;
}

int
encode_eps_attach_type (
  EpsAttachType * epsattachtype,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  uint8_t                                 encoded = 0;

  /*
   * Checking length and pointer
   */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, EPS_ATTACH_TYPE_MINIMUM_LENGTH, len);
  *(buffer + encoded) = 0x00 | (iei & 0xf0) | (*epsattachtype & 0x7);
  encoded++;
  return encoded;
}

uint8_t
encode_u8_eps_attach_type (
  EpsAttachType * epsattachtype)
{
  uint8_t                                 bufferReturn;
  uint8_t                                *buffer = &bufferReturn;
  uint8_t                                 encoded = 0;
  uint8_t                                 iei = 0;

  *(buffer + encoded) = 0x00 | (iei & 0xf0) | (*epsattachtype & 0x7);
  encoded++;
  return bufferReturn;
}

void
dump_eps_attach_type_xml (
  EpsAttachType * epsattachtype,
  uint8_t iei)
{
  OAILOG_DEBUG (LOG_NAS, "<Eps Attach Type>\n");

  if (iei > 0)
    /*
     * Don't display IEI if = 0
     */
    OAILOG_DEBUG (LOG_NAS, "    <IEI>0x%X</IEI>\n", iei);

  OAILOG_DEBUG (LOG_NAS, "    <EPS attach type value>%u</EPS attach type value>\n", *epsattachtype);
  OAILOG_DEBUG (LOG_NAS, "</Eps Attach Type>\n");
}
