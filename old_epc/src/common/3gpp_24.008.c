/*
 * Copyright (c) 2015, EURECOM (www.eurecom.fr)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include "common_defs.h"
#include "assertions.h"
#include "3gpp_24.008.h"
#include "TLVDecoder.h"
#include "TLVEncoder.h"

void copy_protocol_configuration_options (protocol_configuration_options_t * const pco_dst, const protocol_configuration_options_t * const pco_src)
{
  if ((pco_dst) && (pco_src)) {
    pco_dst->ext = pco_src->ext;
    pco_dst->spare = pco_src->spare;
    pco_dst->configuration_protocol = pco_src->configuration_protocol;
    pco_dst->num_protocol_or_container_id = pco_src->num_protocol_or_container_id;
    AssertFatal(PCO_UNSPEC_MAXIMUM_PROTOCOL_ID_OR_CONTAINER_ID >= pco_dst->num_protocol_or_container_id,
        "Invalid number of protocol_or_container_id %d", pco_dst->num_protocol_or_container_id);
    for (int i = 0; i < pco_src->num_protocol_or_container_id; i++) {
      pco_dst->protocol_or_container_ids[i].id     = pco_src->protocol_or_container_ids[i].id;
      pco_dst->protocol_or_container_ids[i].length = pco_src->protocol_or_container_ids[i].length;
      pco_dst->protocol_or_container_ids[i].contents = bstrcpy(pco_src->protocol_or_container_ids[i].contents);
    }
  }
}

void clear_protocol_configuration_options (protocol_configuration_options_t * const pco)
{
  if (pco) {
    for (int i = 0; i < PCO_UNSPEC_MAXIMUM_PROTOCOL_ID_OR_CONTAINER_ID; i++) {
      if (pco->protocol_or_container_ids[i].contents) {
        bdestroy(pco->protocol_or_container_ids[i].contents);
      }
    }
    memset(pco, 0, sizeof(protocol_configuration_options_t));
  }
}


int
decode_protocol_configuration_options (
    protocol_configuration_options_t * protocolconfigurationoptions,
    const uint8_t * const buffer,
    const uint32_t len)
{
  int                                     decoded = 0;
  int                                     decode_result = 0;

  if (((*(buffer + decoded) >> 7) & 0x1) != 1) {
    return TLV_VALUE_DOESNT_MATCH;
  }

  /*
   * Bits 7 to 4 of octet 3 are spare, read as 0
   */
  if (((*(buffer + decoded) & 0x78) >> 3) != 0) {
    return TLV_VALUE_DOESNT_MATCH;
  }

  protocolconfigurationoptions->configuration_protocol = (*(buffer + decoded) >> 1) & 0x7;
  decoded++;
  protocolconfigurationoptions->num_protocol_or_container_id = 0;

  while (3 <= ((int32_t)len - (int32_t)decoded)) {
    DECODE_U16 (buffer + decoded, protocolconfigurationoptions->protocol_or_container_ids[protocolconfigurationoptions->num_protocol_or_container_id].id, decoded);
    DECODE_U8 (buffer + decoded, protocolconfigurationoptions->protocol_or_container_ids[protocolconfigurationoptions->num_protocol_or_container_id].length, decoded);

    if (0 < protocolconfigurationoptions->protocol_or_container_ids[protocolconfigurationoptions->num_protocol_or_container_id].length) {
      if ((decode_result = decode_bstring (&protocolconfigurationoptions->protocol_or_container_ids[protocolconfigurationoptions->num_protocol_or_container_id].contents,
          protocolconfigurationoptions->protocol_or_container_ids[protocolconfigurationoptions->num_protocol_or_container_id].length,
          buffer + decoded,
          len - decoded)) < 0) {
        return decode_result;
      } else {
        decoded += decode_result;
      }
    } else {
      protocolconfigurationoptions->protocol_or_container_ids[protocolconfigurationoptions->num_protocol_or_container_id].contents = NULL;
    }
    protocolconfigurationoptions->num_protocol_or_container_id += 1;
  }

  return decoded;
}

int
encode_protocol_configuration_options (
    const protocol_configuration_options_t * const protocolconfigurationoptions,
    uint8_t * buffer,
    uint32_t len)
{
  uint8_t                                 num_protocol_or_container_id = 0;
  uint32_t                                encoded = 0;
  int                                     encode_result = 0;

  *(buffer + encoded) = 0x00 | (1 << 7) | (protocolconfigurationoptions->configuration_protocol & 0x7);
  encoded++;

  while (num_protocol_or_container_id < protocolconfigurationoptions->num_protocol_or_container_id) {
    ENCODE_U16 (buffer + encoded, protocolconfigurationoptions->protocol_or_container_ids[num_protocol_or_container_id].id, encoded);
    *(buffer + encoded) = protocolconfigurationoptions->protocol_or_container_ids[num_protocol_or_container_id].length;
    encoded++;

    if ((encode_result = encode_bstring (protocolconfigurationoptions->protocol_or_container_ids[num_protocol_or_container_id].contents,
        buffer + encoded, len - encoded)) < 0)
      return encode_result;
    else
      encoded += encode_result;

    num_protocol_or_container_id += 1;
  }

  return encoded;
}

bstring protocol_configuration_options_to_xml (protocol_configuration_options_t * pco)
{
  bstring                                 bstr = bfromcstr("<Protocol Configuration Options>");
  int                                     i = 0;

  if (pco) {
    bformata(bstr, "    <Configuration protocol>%u</Configuration protocol>\n", pco->configuration_protocol);

    while (i < pco->num_protocol_or_container_id) {
      bformata(bstr, "        <Protocol ID>%u</Protocol ID>\n", pco->protocol_or_container_ids[i].id);
      bformata(bstr, "        <Length of protocol ID>%u</Length of protocol ID>\n", pco->protocol_or_container_ids[i].length);
      bstring b = dump_bstring_xml (pco->protocol_or_container_ids[i].contents);
      bformata(bstr, "        %s", bdata(b));
      bdestroy(b);
      i++;
    }
  }

  bcatcstr(bstr, "</Protocol Configuration Options>\n");
  return bstr;
}
