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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "conversion.h"
#include "access_restriction.h"
#include "log.h"

/* TODO: identification of MCC and MNC within an IMSI should be done according
   to a table that maps MCC to 2 or 3 digits expected...
   By default we will assume there is only 2 digits for our use.
*/

/* Split a PLMN formed of <MCC><MNC> to mcc and mnc.
   In case MNC is formed of only two digits a 0 is inserted at the most significant
   digit.
   When PLMN is represented using european convention it contains only two digits,
   while three digits are used in North American Standard.
   Encoding of PLMN is defined in ITU E.212.
   @param plmn string either 5 or 6 digits resulting of the concatenation of
   MCC and MNC.
*/
int
split_plmn (
  uint8_t plmn[3],
  uint8_t mcc[3],
  uint8_t mnc[3])
{
  if (plmn == NULL) {
    return -1;
  }

  mcc[0] = plmn[0] & 0x0F;
  mcc[1] = (plmn[0] & 0xF0) >> 4;
  mcc[2] = plmn[1] & 0x0F;

  if ((plmn[1] & 0xF0) == 0xF0) {
    /*
     * Only 2 digits case
     */
    mnc[1] = plmn[2] & 0x0F;
    mnc[2] = (plmn[2] & 0xF0) >> 4;
    mnc[0] = 0;
  } else {
    mnc[0] = plmn[2] & 0x0F;
    mnc[1] = (plmn[2] & 0xF0) >> 4;
    mnc[2] = ((plmn[1] & 0xF0) >> 4);
  }

  return 0;
}

/* Apply restriction (if any) to current 'visited' PLMN for this user.
   Criterias are based on ODB (operator determined barring), visited PLMN
   and user PLMN (obtain from IMSI).
   @param imsi is the user identity formed of MCC.MNC.MSIN (14 or 15 digits long)
   @param vplmn is the plmn of the cell the UE is currently attached to
*/
#define FORMAT_MCC(mCC) (mCC[0] * 100 + mCC[1] * 10 + mCC[2])
#define FORMAT_MNC(mNC) (mNC[0] * 100 + mNC[1] * 10 + mNC[2])

int
apply_access_restriction (
  char *imsi,
  uint8_t * vplmn)
{
  uint8_t                                 vmcc[3],
                                          vmnc[3];
  uint8_t                                 hmcc[3],
                                          hmnc[3];
  uint8_t                                 imsi_hex[15];

  if (bcd_to_hex (imsi_hex, imsi, strlen (imsi)) != 0) {
    FPRINTF_ERROR ( "Failed to convert imsi %s to hex representation\n", imsi);
    return -1;
  }

  /*
   * There is a problem while converting the PLMN...
   */
  if (split_plmn (vplmn, vmcc, vmnc) != 0) {
    FPRINTF_ERROR ( "Fail to convert vplmn %02x%02x%02x to mcc/mnc for imsi %s\n", vplmn[0], vplmn[1], vplmn[2], imsi);
    return -1;
  }

  FPRINTF_ERROR ( "Converted %02x%02x%02x to plmn %u.%u\n", vplmn[0], vplmn[1], vplmn[2], FORMAT_MCC (vmcc), FORMAT_MNC (vmnc));
  /*
   * MCC is always 3 digits
   */
  memcpy (hmcc, &imsi_hex[0], 3);

  if (memcmp (vmcc, hmcc, 3) != 0) {
    FPRINTF_ERROR ( "Only France MCC is handled for now, got imsi plmn %u.%u for a visited plmn %u.%u\n", FORMAT_MCC (hmcc), FORMAT_MNC (hmnc), FORMAT_MCC (vmcc), FORMAT_MNC (vmnc));
    /*
     * Reject the association
     */
    return -1;
  }

  /*
   * In France MNC is composed of 2 digits and thus imsi by 14 digit
   */
  hmnc[0] = 0;
  memcpy (&hmnc[1], &imsi_hex[3], 2);

  if ((memcmp (vmcc, hmcc, 3) != 0) && (memcmp (vmnc, hmnc, 3) != 0)) {
    FPRINTF_ERROR ( "UE is roaming from %u.%u to %u.%u which is not allowed" " by the ODB\n", FORMAT_MCC (hmcc), FORMAT_MNC (hmnc), FORMAT_MCC (vmcc), FORMAT_MNC (vmnc));
    return -1;
  }

  /*
   * User has successfully passed all the checking -> accept the association
   */
  return 0;
}
