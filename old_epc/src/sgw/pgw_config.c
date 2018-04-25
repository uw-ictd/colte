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

/*! \file spgw_config.c
  \brief
  \author Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#define PGW
#define PGW_CONFIG_C

#include <errno.h>
#include <string.h>
#include <libconfig.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h>


#include "assertions.h"
#include "dynamic_memory_check.h"
#include "log.h"
#include "intertask_interface.h"
#include "sgw_config.h"
#include "pgw_config.h"

#ifdef LIBCONFIG_LONG
#  define libconfig_int long
#else
#  define libconfig_int int
#endif

#define SYSTEM_CMD_MAX_STR_SIZE 512

//------------------------------------------------------------------------------
static int pgw_system (
  bstring command_pP,
  bool    is_abort_on_errorP,
  const char *const file_nameP,
  const int line_numberP)
{
  int                                     ret = RETURNerror;

  if (command_pP) {
#if DISABLE_EXECUTE_SHELL_COMMAND
    ret = 0;
    OAILOG_WARNING (LOG_SPGW_APP, "Not executing system command: %s\n", bdata(command_pP));
#else
    OAILOG_DEBUG (LOG_SPGW_APP, "system command: %s\n", bdata(command_pP));
    ret = system (bdata(command_pP));

    if (ret != 0) {
      OAILOG_ERROR (LOG_SPGW_APP, "ERROR in system command %s: %d at %s:%u\n", bdata(command_pP), ret, file_nameP, line_numberP);

      if (is_abort_on_errorP) {
        exit (-1);              // may be not exit
      }
    }
#endif
  }
  return ret;
}

//------------------------------------------------------------------------------
void pgw_config_init (pgw_config_t * config_pP)
{
  memset ((char *)config_pP, 0, sizeof (*config_pP));
  pthread_rwlock_init (&config_pP->rw_lock, NULL);
  STAILQ_INIT (&config_pP->ipv4_pool_list);
}

//------------------------------------------------------------------------------
int pgw_config_process (pgw_config_t * config_pP)
{
  bstring                                 system_cmd = NULL;
  struct in_addr                          addr_start, addr_mask;
  uint64_t                                counter64 = 0;
  conf_ipv4_list_elm_t                   *ip4_ref = NULL;

  system_cmd = bformat ("iptables -t mangle -F FORWARD");
  pgw_system (system_cmd, PGW_ABORT_ON_ERROR, __FILE__, __LINE__);
  bdestroy(system_cmd);

  if (config_pP->masquerade_SGI) {
    system_cmd = bformat ("iptables -t nat -F POSTROUTING");
    pgw_system (system_cmd, PGW_ABORT_ON_ERROR, __FILE__, __LINE__);
    bdestroy(system_cmd);
  }

  // Get ipv4 address
  char str[INET_ADDRSTRLEN];
  char str_sgi[INET_ADDRSTRLEN];

  // GET SGi informations
  {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, (const char *)config_pP->ipv4.if_name_SGI->data, IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFADDR, &ifr)) {
      OAILOG_CRITICAL(LOG_SPGW_APP, "Failed to read SGI ip addresses: error %s\n", strerror(errno));
      return RETURNerror;
    }
    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
    if (inet_ntop(AF_INET, (const void *)&ipaddr->sin_addr, str_sgi, INET_ADDRSTRLEN) == NULL) {
      OAILOG_ERROR (LOG_SPGW_APP, "inet_ntop");
      return RETURNerror;
    }
    config_pP->ipv4.SGI = ipaddr->sin_addr.s_addr;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, (const char *)config_pP->ipv4.if_name_SGI->data, IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFMTU, &ifr)) {
      OAILOG_CRITICAL(LOG_SPGW_APP, "Failed to probe SGI MTU: error %s\n", strerror(errno));
      return RETURNerror;
    }
    config_pP->ipv4.mtu_SGI = ifr.ifr_mtu;
    OAILOG_DEBUG (LOG_SPGW_APP, "Found SGI interface MTU=%d\n", config_pP->ipv4.mtu_SGI);
    close(fd);
  }
  // GET S5_S8 informations
  {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, (const char *)config_pP->ipv4.if_name_S5_S8->data, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
    if (inet_ntop(AF_INET, (const void *)&ipaddr->sin_addr, str, INET_ADDRSTRLEN) == NULL) {
      OAILOG_ERROR (LOG_SPGW_APP, "inet_ntop");
      return RETURNerror;
    }
    config_pP->ipv4.S5_S8 = ipaddr->sin_addr.s_addr;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, (const char *)config_pP->ipv4.if_name_S5_S8->data, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFMTU, &ifr);
    config_pP->ipv4.mtu_S5_S8 = ifr.ifr_mtu;
    OAILOG_DEBUG (LOG_SPGW_APP, "Foung S5_S8 interface MTU=%d\n", config_pP->ipv4.mtu_S5_S8);
    close(fd);
  }

  for (int i = 0; i < config_pP->num_ue_pool; i++) {

    memcpy (&addr_start, &config_pP->ue_pool_addr[i], sizeof (struct in_addr));
    memcpy (&addr_mask, &config_pP->ue_pool_addr[i], sizeof (struct in_addr));
    addr_mask.s_addr = addr_mask.s_addr & htonl (0xFFFFFFFF << (32 - config_pP->ue_pool_mask[i]));

    if (memcmp (&addr_start, &addr_mask, sizeof (struct in_addr)) ) {
      AssertFatal (0, "BAD IPV4 ADDR CONFIG/MASK PAIRING %s/%d addr %X mask %X\n",
          inet_ntoa(config_pP->ue_pool_addr[i]), config_pP->ue_pool_mask[i], addr_start.s_addr, addr_mask.s_addr);
    }

    counter64 = 0x00000000FFFFFFFF >> config_pP->ue_pool_mask[i];  // address Prefix_mask/0..0 not valid
    counter64 = counter64 - 2;

    do {
      addr_start.s_addr = addr_start.s_addr + htonl (2);
      ip4_ref = calloc (1, sizeof (conf_ipv4_list_elm_t));
      ip4_ref->addr = addr_start;
      STAILQ_INSERT_TAIL (&config_pP->ipv4_pool_list, ip4_ref, ipv4_entries);
      counter64 = counter64 - 1;
    } while (counter64 > 0);

    //---------------
    if (config_pP->masquerade_SGI) {
      system_cmd = bformat ("iptables -t nat -I POSTROUTING -s %s/%d -o %s  ! --protocol sctp -j SNAT --to-source %s",
          inet_ntoa(config_pP->ue_pool_addr[i]), config_pP->ue_pool_mask[i],
          bdata(config_pP->ipv4.if_name_SGI), str_sgi);
      pgw_system (system_cmd, PGW_ABORT_ON_ERROR, __FILE__, __LINE__);
      bdestroy(system_cmd);
    }

    uint32_t min_mtu = config_pP->ipv4.mtu_SGI;
    // 36 = GTPv1-U min header size
    if ((config_pP->ipv4.mtu_S5_S8 - 36) < min_mtu) {
      min_mtu = config_pP->ipv4.mtu_S5_S8 - 36;
    }
    if (config_pP->ue_tcp_mss_clamp) {
      system_cmd = bformat ("iptables -t mangle -I FORWARD -s %s/%d   -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss %u",
          inet_ntoa(config_pP->ue_pool_addr[i]), config_pP->ue_pool_mask[i], min_mtu - 40);
      pgw_system (system_cmd, PGW_ABORT_ON_ERROR, __FILE__, __LINE__);
      btrunc(system_cmd, 0);

      bassignformat (system_cmd, "iptables -t mangle -I FORWARD -d %s/%d -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss %u",
          inet_ntoa(config_pP->ue_pool_addr[i]), config_pP->ue_pool_mask[i], min_mtu - 40);
      pgw_system (system_cmd, PGW_ABORT_ON_ERROR, __FILE__, __LINE__);
      bdestroy(system_cmd);
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
int pgw_config_parse_file (pgw_config_t * config_pP)
{
  config_t                                cfg = {0};
  config_setting_t                       *setting_pgw = NULL;
  config_setting_t                       *subsetting = NULL;
  config_setting_t                       *sub2setting = NULL;
  char                                   *if_S5_S8 = NULL;
  char                                   *if_SGI = NULL;
  char                                   *masquerade_SGI = NULL;
  char                                   *ue_tcp_mss_clamping = NULL;
  char                                   *default_dns = NULL;
  char                                   *default_dns_sec = NULL;
  const char                             *astring = NULL;
  bstring                                 address = NULL;
  bstring                                 cidr = NULL;
  bstring                                 mask = NULL;
  int                                     num = 0;
  int                                     i = 0;
  unsigned char                           buf_in_addr[sizeof (struct in_addr)];
  struct in_addr                          addr_start;
  bstring                                 system_cmd = NULL;
  libconfig_int                           mtu = 0;
  int                                     prefix_mask = 0;


  config_init (&cfg);

  if (config_pP->config_file) {
    /*
     * Read the file. If there is an error, report it and exit.
     */
    if (!config_read_file (&cfg, bdata(config_pP->config_file))) {
      OAILOG_ERROR (LOG_SPGW_APP, "%s:%d - %s\n", bdata(config_pP->config_file), config_error_line (&cfg), config_error_text (&cfg));
      config_destroy (&cfg);
      AssertFatal (1 == 0, "Failed to parse SP-GW configuration file %s!\n", bdata(config_pP->config_file));
    }
  } else {
    OAILOG_ERROR (LOG_SPGW_APP, "No SP-GW configuration file provided!\n");
    config_destroy (&cfg);
    AssertFatal (0, "No SP-GW configuration file provided!\n");
  }

  OAILOG_INFO (LOG_SPGW_APP, "Parsing configuration file provided %s\n", bdata(config_pP->config_file));

  system_cmd = bfromcstr("");

  setting_pgw = config_lookup (&cfg, PGW_CONFIG_STRING_PGW_CONFIG);

  if (setting_pgw) {
    subsetting = config_setting_get_member (setting_pgw, PGW_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);

    if (subsetting) {
      if ((config_setting_lookup_string (subsetting, PGW_CONFIG_STRING_PGW_INTERFACE_NAME_FOR_S5_S8, (const char **)&if_S5_S8)
           && config_setting_lookup_string (subsetting, PGW_CONFIG_STRING_PGW_INTERFACE_NAME_FOR_SGI, (const char **)&if_SGI)
           && config_setting_lookup_string (subsetting, PGW_CONFIG_STRING_PGW_MASQUERADE_SGI, (const char **)&masquerade_SGI)
           && config_setting_lookup_string (subsetting, PGW_CONFIG_STRING_UE_TCP_MSS_CLAMPING, (const char **)&ue_tcp_mss_clamping)
          )
        ) {
        config_pP->ipv4.if_name_S5_S8 = bfromcstr(if_S5_S8);
        config_pP->ipv4.if_name_SGI = bfromcstr (if_SGI);
        OAILOG_DEBUG (LOG_SPGW_APP, "Parsing configuration file found SGI: on %s\n", bdata(config_pP->ipv4.if_name_SGI));

        if (strcasecmp (masquerade_SGI, "yes") == 0) {
          config_pP->masquerade_SGI = true;
          OAILOG_DEBUG (LOG_SPGW_APP, "Masquerade SGI\n");
        } else {
          config_pP->masquerade_SGI = false;
          OAILOG_DEBUG (LOG_SPGW_APP, "No masquerading for SGI\n");
        }
        if (strcasecmp (ue_tcp_mss_clamping, "yes") == 0) {
          config_pP->ue_tcp_mss_clamp = true;
          OAILOG_DEBUG (LOG_SPGW_APP, "CLAMP TCP MSS\n");
        } else {
          config_pP->ue_tcp_mss_clamp = false;
          OAILOG_DEBUG (LOG_SPGW_APP, "NO CLAMP TCP MSS\n");
        }
      } else {
        OAILOG_WARNING (LOG_SPGW_APP, "CONFIG P-GW / NETWORK INTERFACES parsing failed\n");
      }
    } else {
      OAILOG_WARNING (LOG_SPGW_APP, "CONFIG P-GW / NETWORK INTERFACES not found\n");
    }

    //!!!------------------------------------!!!
    subsetting = config_setting_get_member (setting_pgw, PGW_CONFIG_STRING_IP_ADDRESS_POOL);

    if (subsetting) {
      sub2setting = config_setting_get_member (subsetting, PGW_CONFIG_STRING_IPV4_ADDRESS_LIST);

      if (sub2setting) {
        num = config_setting_length (sub2setting);

        for (i = 0; i < num; i++) {
          astring = config_setting_get_string_elem (sub2setting, i);

          if (astring) {
            cidr = bfromcstr (astring);
            AssertFatal(BSTR_OK == btrimws(cidr), "Error in PGW_CONFIG_STRING_IPV4_ADDRESS_LIST %s", astring);
            struct bstrList *list = bsplit (cidr, PGW_CONFIG_STRING_IPV4_PREFIX_DELIMITER);
            AssertFatal(2 == list->qty, "Bad CIDR address %s", bdata(cidr));

            address = list->entry[0];
            mask    = list->entry[1];

            if (inet_pton (AF_INET, bdata(address), buf_in_addr) == 1) {
              memcpy (&addr_start, buf_in_addr, sizeof (struct in_addr));
              // valid address
              prefix_mask = atoi ((const char *)mask->data);

              if ((prefix_mask >= 2) && (prefix_mask < 32) && (config_pP->num_ue_pool < PGW_NUM_UE_POOL_MAX)) {
                memcpy (&config_pP->ue_pool_addr[config_pP->num_ue_pool], buf_in_addr, sizeof (struct in_addr));
                config_pP->ue_pool_mask[config_pP->num_ue_pool] = prefix_mask;
                config_pP->num_ue_pool += 1;
              } else {
                OAILOG_ERROR (LOG_SPGW_APP, "CONFIG POOL ADDR IPV4: BAD MASQ: %d\n", prefix_mask);
              }
            }
            bstrListDestroy(list);
          }
        }
      } else {
        OAILOG_WARNING (LOG_SPGW_APP, "CONFIG POOL ADDR IPV4: NO IPV4 ADDRESS FOUND\n");
      }

      if (config_setting_lookup_string (setting_pgw, PGW_CONFIG_STRING_DEFAULT_DNS_IPV4_ADDRESS, (const char **)&default_dns)
          && config_setting_lookup_string (setting_pgw, PGW_CONFIG_STRING_DEFAULT_DNS_SEC_IPV4_ADDRESS, (const char **)&default_dns_sec)) {
        config_pP->ipv4.if_name_S5_S8 = bfromcstr (if_S5_S8);
        IPV4_STR_ADDR_TO_INT_NWBO (default_dns, config_pP->ipv4.default_dns, "BAD IPv4 ADDRESS FORMAT FOR DEFAULT DNS !\n");
        IPV4_STR_ADDR_TO_INT_NWBO (default_dns_sec, config_pP->ipv4.default_dns_sec, "BAD IPv4 ADDRESS FORMAT FOR DEFAULT DNS SEC!\n");
        OAILOG_DEBUG (LOG_SPGW_APP, "Parsing configuration file default primary DNS IPv4 address: %x\n", config_pP->ipv4.default_dns);
        OAILOG_DEBUG (LOG_SPGW_APP, "Parsing configuration file default secondary DNS IPv4 address: %x\n", config_pP->ipv4.default_dns_sec);
      } else {
        OAILOG_WARNING (LOG_SPGW_APP, "NO DNS CONFIGURATION FOUND\n");
      }
    }
    if (config_setting_lookup_string (setting_pgw, PGW_CONFIG_STRING_NAS_FORCE_PUSH_PCO, (const char **)&astring)) {
      if (strcasecmp (astring, "yes") == 0) {
        config_pP->force_push_pco = true;
        OAILOG_DEBUG (LOG_SPGW_APP, "Protocol configuration options: push MTU, push DNS, IP address allocation via NAS signalling\n");
      } else {
        config_pP->force_push_pco = false;
      }
    }
    if (config_setting_lookup_int (setting_pgw, PGW_CONFIG_STRING_UE_MTU, &mtu)) {
      config_pP->ue_mtu = mtu;
    } else {
      config_pP->ue_mtu = 1463;
    }
    OAILOG_DEBUG (LOG_SPGW_APP, "UE MTU : %u\n", config_pP->ue_mtu);
  } else {
    OAILOG_WARNING (LOG_SPGW_APP, "CONFIG P-GW not found\n");
  }
  bdestroy(system_cmd);
  config_destroy (&cfg);
  return RETURNok;
}

//------------------------------------------------------------------------------
void pgw_config_display (pgw_config_t * config_p)
{
  OAILOG_INFO (LOG_SPGW_APP, "==== EURECOM %s v%s ====\n", PACKAGE_NAME, PACKAGE_VERSION);
  OAILOG_INFO (LOG_SPGW_APP, "Configuration:\n");
  OAILOG_INFO (LOG_SPGW_APP, "- File .................................: %s\n", bdata(config_p->config_file));

  OAILOG_INFO (LOG_SPGW_APP, "- S5-S8:\n");
  OAILOG_INFO (LOG_SPGW_APP, "    S5_S8 iface ..........: %s\n", bdata(config_p->ipv4.if_name_S5_S8));
  OAILOG_INFO (LOG_SPGW_APP, "    S5_S8 ip  (read)......: %s\n", inet_ntoa (*((struct in_addr *)&config_p->ipv4.S5_S8)));
  OAILOG_INFO (LOG_SPGW_APP, "    S5_S8 MTU (read)......: %u\n", config_p->ipv4.mtu_S5_S8);
  OAILOG_INFO (LOG_SPGW_APP, "- SGi:\n");
  OAILOG_INFO (LOG_SPGW_APP, "    SGi iface ............: %s\n", bdata(config_p->ipv4.if_name_SGI));
  OAILOG_INFO (LOG_SPGW_APP, "    SGi ip  (read)........: %s\n", inet_ntoa (*((struct in_addr *)&config_p->ipv4.SGI)));
  OAILOG_INFO (LOG_SPGW_APP, "    SGi MTU (read)........: %u\n", config_p->ipv4.mtu_SGI);

  OAILOG_INFO (LOG_SPGW_APP, "- MSS clamping: ..........: %d\n", config_p->ue_tcp_mss_clamp);
  OAILOG_INFO (LOG_SPGW_APP, "- Masquerading: ..........: %d\n", config_p->masquerade_SGI);
  OAILOG_INFO (LOG_SPGW_APP, "- Push PCO: ..............: %d\n", config_p->force_push_pco);
}
