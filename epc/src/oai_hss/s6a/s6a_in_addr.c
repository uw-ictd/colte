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


#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"
#include "log.h"

/* http://www.iana.org/assignments/address-family-numbers/address-family-numbers.xml*/

/* Perform a conversion between ipv4 in BCD to AVP served-party-ip-address */
int
s6a_add_ipv4_address (
  struct avp *avp,
  const char *ipv4_addr)
{
  struct avp                             *child_avp;
  union avp_value                         value;
  uint8_t                                 ipv4[6];      /* Converted IPv4 address with family */
  in_addr_t                               sin;

  if (ipv4_addr == NULL) {
    return -1;
  }

  /*
   * This is an IPv4 family -> ipv4 buffer should start with 0x0001
   */
  ipv4[0] = 0x00;
  ipv4[1] = 0x01;
  sin = inet_addr (ipv4_addr);

  /*
   * No need to add the address if it is an any address
   */
  if (sin != INADDR_ANY) {
    memcpy (&ipv4[2], &sin, 4);
    CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_served_party_ip_addr, 0, &child_avp));
    value.os.data = ipv4;
    value.os.len = 6;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp, MSG_BRW_LAST_CHILD, child_avp));
    return 0;
  }

  /*
   * No IP address added to AVP
   */
  return -1;
}

/* Perform a conversion between ipv6 in BCD to AVP served-party-ip-address */
int
s6a_add_ipv6_address (
  struct avp *avp,
  const char *ipv6_addr)
{
  struct avp                             *child_avp;
  union avp_value                         value;
  uint8_t                                 ipv6[18];
  struct in6_addr                         sin6;

  if (ipv6_addr == NULL) {
    return -1;
  }

  memset (&sin6, 0, sizeof (struct in6_addr));
  /*
   * This is an IPv6 family -> ipv6 buffer should start with 0x0002
   */
  ipv6[0] = 0x00;
  ipv6[1] = 0x02;

  if (inet_pton (AF_INET6, ipv6_addr, &sin6) == -1) {
    FPRINTF_ERROR ("INET6 address conversion has failed\n");
    return -1;
  }

  /*
   * If the IPV6 address is 0:0:0:0:0:0:0:0 then we don't add it to the
   * * * * served-party ip address and consider the ip address can be dynamically
   * * * * allocated.
   */
  if (!IN6_IS_ADDR_UNSPECIFIED (sin6.s6_addr32)) {
    memcpy (&ipv6[2], &sin6.s6_addr, 16);
    CHECK_FCT (fd_msg_avp_new (s6a_cnf.dataobj_s6a_served_party_ip_addr, 0, &child_avp));
    value.os.data = ipv6;
    value.os.len = 18;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp, MSG_BRW_LAST_CHILD, child_avp));
    return 0;
  }

  return -1;
}
