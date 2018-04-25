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
#include "log.h"

int                                     errorCodeEncoder = 0;

int encode_bstring (
  const_bstring const str,
  uint8_t * const buffer,
  const uint32_t buflen)
{
  if (str ) {
    if (blength(str) > 0) {
      CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, blength(str), buflen);
      memcpy ((void *)buffer, (void *)str->data, blength(str));
      return blength(str);
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

