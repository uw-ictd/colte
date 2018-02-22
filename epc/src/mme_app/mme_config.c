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


#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <libconfig.h>

#include <arpa/inet.h>          /* To provide inet_addr */

#include "assertions.h"
#include "dynamic_memory_check.h"
#include "log.h"
#include "intertask_interface.h"
#include "spgw_config.h"

mme_config_t                            mme_config = {.rw_lock = PTHREAD_RWLOCK_INITIALIZER, 0};

//------------------------------------------------------------------------------
int mme_config_find_mnc_length (
  const char mcc_digit1P,
  const char mcc_digit2P,
  const char mcc_digit3P,
  const char mnc_digit1P,
  const char mnc_digit2P,
  const char mnc_digit3P)
{
  uint16_t                                mcc = 100 * mcc_digit1P + 10 * mcc_digit2P + mcc_digit3P;
  uint16_t                                mnc3 = 100 * mnc_digit1P + 10 * mnc_digit2P + mnc_digit3P;
  uint16_t                                mnc2 = 10 * mnc_digit1P + mnc_digit2P;
  int                                     plmn_index = 0;

  AssertFatal ((mcc_digit1P >= 0) && (mcc_digit1P <= 9)
               && (mcc_digit2P >= 0) && (mcc_digit2P <= 9)
               && (mcc_digit3P >= 0) && (mcc_digit3P <= 9), "BAD MCC PARAMETER (%d%d%d)!\n", mcc_digit1P, mcc_digit2P, mcc_digit3P);
  AssertFatal ((mnc_digit2P >= 0) && (mnc_digit2P <= 9)
               && (mnc_digit1P >= 0) && (mnc_digit1P <= 9), "BAD MNC PARAMETER (%d.%d.%d)!\n", mnc_digit1P, mnc_digit2P, mnc_digit3P);

  while (plmn_index < mme_config.served_tai.nb_tai) {
    if (mme_config.served_tai.plmn_mcc[plmn_index] == mcc) {
      if ((mme_config.served_tai.plmn_mnc[plmn_index] == mnc2) && (mme_config.served_tai.plmn_mnc_len[plmn_index] == 2)) {
        return 2;
      } else if ((mme_config.served_tai.plmn_mnc[plmn_index] == mnc3) && (mme_config.served_tai.plmn_mnc_len[plmn_index] == 3)) {
        return 3;
      }
    }

    plmn_index += 1;
  }

  return 0;
}


//------------------------------------------------------------------------------
static void mme_config_init (mme_config_t * config_pP)
{
  memset(&mme_config, 0, sizeof(mme_config));
  pthread_rwlock_init (&config_pP->rw_lock, NULL);
  config_pP->log_config.output             = NULL;
  config_pP->log_config.is_output_thread_safe = false;
  config_pP->log_config.color              = false;
  config_pP->log_config.udp_log_level      = MAX_LOG_LEVEL; // Means invalid
  config_pP->log_config.gtpv1u_log_level   = MAX_LOG_LEVEL; // will not overwrite existing log levels if MME and S-GW bundled in same executable
  config_pP->log_config.gtpv2c_log_level   = MAX_LOG_LEVEL;
  config_pP->log_config.sctp_log_level     = MAX_LOG_LEVEL;
  config_pP->log_config.s1ap_log_level     = MAX_LOG_LEVEL;
  config_pP->log_config.nas_log_level      = MAX_LOG_LEVEL;
  config_pP->log_config.mme_app_log_level  = MAX_LOG_LEVEL;
  config_pP->log_config.spgw_app_log_level = MAX_LOG_LEVEL;
  config_pP->log_config.s11_log_level      = MAX_LOG_LEVEL;
  config_pP->log_config.s6a_log_level      = MAX_LOG_LEVEL;
  config_pP->log_config.util_log_level     = MAX_LOG_LEVEL;
  config_pP->log_config.msc_log_level      = MAX_LOG_LEVEL;
  config_pP->log_config.itti_log_level     = MAX_LOG_LEVEL;

  config_pP->log_config.asn1_verbosity_level = 0;
  config_pP->config_file = NULL;
  config_pP->max_enbs = 2;
  config_pP->max_ues = 2;
  config_pP->unauthenticated_imsi_supported = 0;
  /*
   * EPS network feature support
   */
  config_pP->eps_network_feature_support.emergency_bearer_services_in_s1_mode = 0;
  config_pP->eps_network_feature_support.extended_service_request = 0;
  config_pP->eps_network_feature_support.ims_voice_over_ps_session_in_s1 = 0;
  config_pP->eps_network_feature_support.location_services_via_epc = 0;

  config_pP->s1ap_config.port_number = S1AP_PORT_NUMBER;
  /*
   * IP configuration
   */
  config_pP->ipv4.if_name_s1_mme = NULL;
  config_pP->ipv4.s1_mme = 0;
  config_pP->ipv4.if_name_s11 = NULL;
  config_pP->ipv4.s11 = 0;
  config_pP->ipv4.port_s11 = 2123;
  config_pP->ipv4.sgw_s11 = 0;
  config_pP->s6a_config.conf_file = bfromcstr(S6A_CONF_FILE);
  config_pP->itti_config.queue_size = ITTI_QUEUE_MAX_ELEMENTS;
  config_pP->itti_config.log_file = NULL;
  config_pP->sctp_config.in_streams = SCTP_IN_STREAMS;
  config_pP->sctp_config.out_streams = SCTP_OUT_STREAMS;
  config_pP->relative_capacity = RELATIVE_CAPACITY;
  config_pP->mme_statistic_timer = MME_STATISTIC_TIMER_S;
  config_pP->gummei.nb = 1;
  config_pP->gummei.gummei[0].mme_code = MMEC;
  config_pP->gummei.gummei[0].mme_gid = MMEGID;
  config_pP->gummei.gummei[0].plmn.mcc_digit1 = 0;
  config_pP->gummei.gummei[0].plmn.mcc_digit2 = 0;
  config_pP->gummei.gummei[0].plmn.mcc_digit3 = 1;
  config_pP->gummei.gummei[0].plmn.mcc_digit1 = 0;
  config_pP->gummei.gummei[0].plmn.mcc_digit2 = 1;
  config_pP->gummei.gummei[0].plmn.mcc_digit3 = 0x0F;

  /*
   * Set the TAI
   */
  config_pP->served_tai.nb_tai = 1;
  config_pP->served_tai.plmn_mcc = calloc (1, sizeof (*config_pP->served_tai.plmn_mcc));
  config_pP->served_tai.plmn_mnc = calloc (1, sizeof (*config_pP->served_tai.plmn_mnc));
  config_pP->served_tai.plmn_mnc_len = calloc (1, sizeof (*config_pP->served_tai.plmn_mnc_len));
  config_pP->served_tai.tac = calloc (1, sizeof (*config_pP->served_tai.tac));
  config_pP->served_tai.plmn_mcc[0] = PLMN_MCC;
  config_pP->served_tai.plmn_mnc[0] = PLMN_MNC;
  config_pP->served_tai.plmn_mnc_len[0] = PLMN_MNC_LEN;
  config_pP->served_tai.tac[0] = PLMN_TAC;
  config_pP->s1ap_config.outcome_drop_timer_sec = S1AP_OUTCOME_TIMER_DEFAULT;
}


//------------------------------------------------------------------------------
static int mme_config_parse_file (mme_config_t * config_pP)
{
  config_t                                cfg = {0};
  config_setting_t                       *setting_mme = NULL;
  config_setting_t                       *setting = NULL;
  config_setting_t                       *subsetting = NULL;
  config_setting_t                       *sub2setting = NULL;
  int                                     aint = 0;
  int                                     i = 0,n = 0,
                                          stop_index = 0,
                                          num = 0;
  const char                             *astring = NULL;
  const char                             *tac = NULL;
  const char                             *mcc = NULL;
  const char                             *mnc = NULL;
  char                                   *if_name_s1_mme = NULL;
  char                                   *s1_mme = NULL;
  char                                   *if_name_s11 = NULL;
  char                                   *s11 = NULL;
  char                                   *sgw_ip_address_for_s11 = NULL;
  bool                                    swap = false;
  bstring                                 address = NULL;
  bstring                                 cidr = NULL;
  bstring                                 mask = NULL;
  struct in_addr                          in_addr_var = {0};

  config_init (&cfg);

  if (config_pP->config_file != NULL) {
    /*
     * Read the file. If there is an error, report it and exit.
     */
    if (!config_read_file (&cfg, bdata(config_pP->config_file))) {
      OAILOG_ERROR (LOG_CONFIG, ": %s:%d - %s\n", bdata(config_pP->config_file), config_error_line (&cfg), config_error_text (&cfg));
      config_destroy (&cfg);
      AssertFatal (1 == 0, "Failed to parse MME configuration file %s!\n", bdata(config_pP->config_file));
    }
  } else {
    OAILOG_ERROR (LOG_CONFIG, " No MME configuration file provided!\n");
    config_destroy (&cfg);
    AssertFatal (0, "No MME configuration file provided!\n");
  }

  setting_mme = config_lookup (&cfg, MME_CONFIG_STRING_MME_CONFIG);

  if (setting_mme != NULL) {
    // LOGGING setting
    setting = config_setting_get_member (setting_mme, LOG_CONFIG_STRING_LOGGING);


    if (setting != NULL) {
      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_OUTPUT, (const char **)&astring)) {
        if (astring != NULL) {
          if (config_pP->log_config.output) {
            bassigncstr(config_pP->log_config.output , astring);
          } else {
            config_pP->log_config.output = bfromcstr(astring);
          }
        }
      }

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_OUTPUT_THREAD_SAFE, (const char **)&astring)) {
        if (astring != NULL) {
          if (strcasecmp (astring, "yes") == 0) {
            config_pP->log_config.is_output_thread_safe = true;
          } else {
            config_pP->log_config.is_output_thread_safe = false;
          }
        }
      }

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_COLOR, (const char **)&astring)) {
        if (0 == strcasecmp("true", astring)) config_pP->log_config.color = true;
        else config_pP->log_config.color = false;
      }

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_SCTP_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.sctp_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_S1AP_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.s1ap_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_NAS_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.nas_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_MME_APP_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.mme_app_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_S6A_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.s6a_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_GTPV2C_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.gtpv2c_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_UDP_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.udp_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_S11_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.s11_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_UTIL_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.util_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_MSC_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.msc_log_level = OAILOG_LEVEL_STR2INT (astring);

      if (config_setting_lookup_string (setting, LOG_CONFIG_STRING_ITTI_LOG_LEVEL, (const char **)&astring))
        config_pP->log_config.itti_log_level = OAILOG_LEVEL_STR2INT (astring);

      if ((config_setting_lookup_string (setting_mme, MME_CONFIG_STRING_ASN1_VERBOSITY, (const char **)&astring))) {
        if (strcasecmp (astring, MME_CONFIG_STRING_ASN1_VERBOSITY_NONE) == 0)
          config_pP->log_config.asn1_verbosity_level = 0;
        else if (strcasecmp (astring, MME_CONFIG_STRING_ASN1_VERBOSITY_ANNOYING) == 0)
          config_pP->log_config.asn1_verbosity_level = 2;
        else if (strcasecmp (astring, MME_CONFIG_STRING_ASN1_VERBOSITY_INFO) == 0)
          config_pP->log_config.asn1_verbosity_level = 1;
        else
          config_pP->log_config.asn1_verbosity_level = 0;
      }
    }

    // GENERAL MME SETTINGS
    if ((config_setting_lookup_string (setting_mme, MME_CONFIG_STRING_RUN_MODE, (const char **)&astring))) {
      if (strcasecmp (astring, MME_CONFIG_STRING_RUN_MODE_TEST) == 0)
        config_pP->run_mode = RUN_MODE_TEST;
      else
        config_pP->run_mode = RUN_MODE_OTHER;
    }

    if ((config_setting_lookup_string (setting_mme, MME_CONFIG_STRING_REALM, (const char **)&astring))) {
      config_pP->realm = bfromcstr (astring);
    }

    if ((config_setting_lookup_string (setting_mme,
                                       MME_CONFIG_STRING_PID_DIRECTORY,
                                       (const char **)&astring))) {
      config_pP->pid_dir = bfromcstr (astring);
    }

    if ((config_setting_lookup_int (setting_mme, MME_CONFIG_STRING_MAXENB, &aint))) {
      config_pP->max_enbs = (uint32_t) aint;
    }

    if ((config_setting_lookup_int (setting_mme, MME_CONFIG_STRING_MAXUE, &aint))) {
      config_pP->max_ues = (uint32_t) aint;
    }

    if ((config_setting_lookup_int (setting_mme, MME_CONFIG_STRING_RELATIVE_CAPACITY, &aint))) {
      config_pP->relative_capacity = (uint8_t) aint;
    }

    if ((config_setting_lookup_int (setting_mme, MME_CONFIG_STRING_STATISTIC_TIMER, &aint))) {
      config_pP->mme_statistic_timer = (uint32_t) aint;
    }

    if ((config_setting_lookup_string (setting_mme, EPS_NETWORK_FEATURE_SUPPORT_EMERGENCY_BEARER_SERVICES_IN_S1_MODE, (const char **)&astring))) {
      if (strcasecmp (astring, "yes") == 0)
        config_pP->eps_network_feature_support.emergency_bearer_services_in_s1_mode = 1;
      else
        config_pP->eps_network_feature_support.emergency_bearer_services_in_s1_mode = 0;
    }
    if ((config_setting_lookup_string (setting_mme, EPS_NETWORK_FEATURE_SUPPORT_EXTENDED_SERVICE_REQUEST, (const char **)&astring))) {
      if (strcasecmp (astring, "yes") == 0)
        config_pP->eps_network_feature_support.extended_service_request = 1;
      else
        config_pP->eps_network_feature_support.extended_service_request = 0;
    }
    if ((config_setting_lookup_string (setting_mme, EPS_NETWORK_FEATURE_SUPPORT_IMS_VOICE_OVER_PS_SESSION_IN_S1, (const char **)&astring))) {
      if (strcasecmp (astring, "yes") == 0)
        config_pP->eps_network_feature_support.ims_voice_over_ps_session_in_s1 = 1;
      else
        config_pP->eps_network_feature_support.ims_voice_over_ps_session_in_s1 = 0;
    }
    if ((config_setting_lookup_string (setting_mme, EPS_NETWORK_FEATURE_SUPPORT_LOCATION_SERVICES_VIA_EPC, (const char **)&astring))) {
      if (strcasecmp (astring, "yes") == 0)
        config_pP->eps_network_feature_support.location_services_via_epc = 1;
      else
        config_pP->eps_network_feature_support.location_services_via_epc = 0;
    }

    if ((config_setting_lookup_string (setting_mme, MME_CONFIG_STRING_UNAUTHENTICATED_IMSI_SUPPORTED, (const char **)&astring))) {
      if (strcasecmp (astring, "yes") == 0)
        config_pP->unauthenticated_imsi_supported = 1;
      else
        config_pP->unauthenticated_imsi_supported = 0;
    }

    // ITTI SETTING
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_INTERTASK_INTERFACE_CONFIG);

    if (setting != NULL) {
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_INTERTASK_INTERFACE_QUEUE_SIZE, &aint))) {
        config_pP->itti_config.queue_size = (uint32_t) aint;
      }
    }
    // S6A SETTING
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_S6A_CONFIG);

    if (setting != NULL) {
      if ((config_setting_lookup_string (setting, MME_CONFIG_STRING_S6A_CONF_FILE_PATH, (const char **)&astring))) {
        if (astring != NULL) {
          if (config_pP->s6a_config.conf_file) {
            bassigncstr(config_pP->s6a_config.conf_file , astring);
          } else {
            config_pP->s6a_config.conf_file = bfromcstr(astring);
          }
        }
      }

      if ((config_setting_lookup_string (setting, MME_CONFIG_STRING_S6A_HSS_HOSTNAME, (const char **)&astring))) {
        if (astring != NULL) {
          if (config_pP->s6a_config.hss_host_name) {
            bassigncstr(config_pP->s6a_config.hss_host_name , astring);
          } else {
            config_pP->s6a_config.hss_host_name = bfromcstr(astring);
          }
        } else
          AssertFatal (1 == 0, "You have to provide a valid HSS hostname %s=...\n", MME_CONFIG_STRING_S6A_HSS_HOSTNAME);
      }
    }
    // SCTP SETTING
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_SCTP_CONFIG);

    if (setting != NULL) {
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_SCTP_INSTREAMS, &aint))) {
        config_pP->sctp_config.in_streams = (uint16_t) aint;
      }

      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_SCTP_OUTSTREAMS, &aint))) {
        config_pP->sctp_config.out_streams = (uint16_t) aint;
      }
    }
    // S1AP SETTING
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_S1AP_CONFIG);

    if (setting != NULL) {
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_S1AP_OUTCOME_TIMER, &aint))) {
        config_pP->s1ap_config.outcome_drop_timer_sec = (uint8_t) aint;
      }

      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_S1AP_PORT, &aint))) {
        config_pP->s1ap_config.port_number = (uint16_t) aint;
      }
    }
    // TAI list setting
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_TAI_LIST);
    if (setting != NULL) {
      num = config_setting_length (setting);

      if (config_pP->served_tai.nb_tai != num) {
        if (config_pP->served_tai.plmn_mcc != NULL)
          free_wrapper ((void**) &config_pP->served_tai.plmn_mcc);

        if (config_pP->served_tai.plmn_mnc != NULL)
          free_wrapper ((void**) &config_pP->served_tai.plmn_mnc);

        if (config_pP->served_tai.plmn_mnc_len != NULL)
          free_wrapper ((void**) &config_pP->served_tai.plmn_mnc_len);

        if (config_pP->served_tai.tac != NULL)
          free_wrapper ((void**) &config_pP->served_tai.tac);

        config_pP->served_tai.plmn_mcc = calloc (num, sizeof (*config_pP->served_tai.plmn_mcc));
        config_pP->served_tai.plmn_mnc = calloc (num, sizeof (*config_pP->served_tai.plmn_mnc));
        config_pP->served_tai.plmn_mnc_len = calloc (num, sizeof (*config_pP->served_tai.plmn_mnc_len));
        config_pP->served_tai.tac = calloc (num, sizeof (*config_pP->served_tai.tac));
      }

      config_pP->served_tai.nb_tai = num;
      AssertFatal(16 >= num , "Too many TAIs configured %d", num);

      for (i = 0; i < num; i++) {
        sub2setting = config_setting_get_elem (setting, i);

        if (sub2setting != NULL) {
          if ((config_setting_lookup_string (sub2setting, MME_CONFIG_STRING_MCC, &mcc))) {
            config_pP->served_tai.plmn_mcc[i] = (uint16_t) atoi (mcc);
          }

          if ((config_setting_lookup_string (sub2setting, MME_CONFIG_STRING_MNC, &mnc))) {
            config_pP->served_tai.plmn_mnc[i] = (uint16_t) atoi (mnc);
            config_pP->served_tai.plmn_mnc_len[i] = strlen (mnc);
            AssertFatal ((config_pP->served_tai.plmn_mnc_len[i] == 2) || (config_pP->served_tai.plmn_mnc_len[i] == 3),
                "Bad MNC length %u, must be 2 or 3", config_pP->served_tai.plmn_mnc_len[i]);
          }

          if ((config_setting_lookup_string (sub2setting, MME_CONFIG_STRING_TAC, &tac))) {
            config_pP->served_tai.tac[i] = (uint16_t) atoi (tac);
            AssertFatal(TAC_IS_VALID(config_pP->served_tai.tac[i]), "Invalid TAC value "TAC_FMT, config_pP->served_tai.tac[i]);
          }
        }
      }
      // sort TAI list
      n = config_pP->served_tai.nb_tai;
      do {
        stop_index = 0;
        for (i = 1; i < n; i++) {
          swap = false;
          if (config_pP->served_tai.plmn_mcc[i-1] > config_pP->served_tai.plmn_mcc[i]) {
            swap = true;
          } else if (config_pP->served_tai.plmn_mcc[i-1] == config_pP->served_tai.plmn_mcc[i]) {
            if (config_pP->served_tai.plmn_mnc[i-1] > config_pP->served_tai.plmn_mnc[i]) {
              swap = true;
            } else  if (config_pP->served_tai.plmn_mnc[i-1] == config_pP->served_tai.plmn_mnc[i]) {
              if (config_pP->served_tai.tac[i-1] > config_pP->served_tai.tac[i]) {
                swap = true;
              }
            }
          }
          if (true == swap) {
            uint16_t swap16;
            swap16 = config_pP->served_tai.plmn_mcc[i-1];
            config_pP->served_tai.plmn_mcc[i-1] = config_pP->served_tai.plmn_mcc[i];
            config_pP->served_tai.plmn_mcc[i]   = swap16;

            swap16 = config_pP->served_tai.plmn_mnc[i-1];
            config_pP->served_tai.plmn_mnc[i-1] = config_pP->served_tai.plmn_mnc[i];
            config_pP->served_tai.plmn_mnc[i]   = swap16;

            swap16 = config_pP->served_tai.tac[i-1];
            config_pP->served_tai.tac[i-1] = config_pP->served_tai.tac[i];
            config_pP->served_tai.tac[i]   = swap16;

            stop_index = i;
          }
        }
        n = stop_index;
      } while (0 != n);
      // helper for determination of list type (global view), we could make sublists with different types, but keep things simple for now
      config_pP->served_tai.list_type = TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_CONSECUTIVE_TACS;
      for (i = 1; i < config_pP->served_tai.nb_tai; i++) {
        if ((config_pP->served_tai.plmn_mcc[i] != config_pP->served_tai.plmn_mcc[0]) ||
            (config_pP->served_tai.plmn_mnc[i] != config_pP->served_tai.plmn_mnc[0])){
          config_pP->served_tai.list_type = TRACKING_AREA_IDENTITY_LIST_TYPE_MANY_PLMNS;
          break;
        } else if ((config_pP->served_tai.plmn_mcc[i] != config_pP->served_tai.plmn_mcc[i-1]) ||
                   (config_pP->served_tai.plmn_mnc[i] != config_pP->served_tai.plmn_mnc[i-1])) {
          config_pP->served_tai.list_type = TRACKING_AREA_IDENTITY_LIST_TYPE_MANY_PLMNS;
          break;
        }
        if (config_pP->served_tai.tac[i] != (config_pP->served_tai.tac[i-1] + 1)) {
          config_pP->served_tai.list_type = TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_NON_CONSECUTIVE_TACS;
        }
      }
    }

    // GUMMEI SETTING
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_GUMMEI_LIST);
    config_pP->gummei.nb = 0;
    if (setting != NULL) {
      num = config_setting_length (setting);
      AssertFatal(num == 1, "Only one GUMMEI supported for this version of MME");
      for (i = 0; i < num; i++) {
        sub2setting = config_setting_get_elem (setting, i);

        if (sub2setting != NULL) {
          if ((config_setting_lookup_string (sub2setting, MME_CONFIG_STRING_MCC, &mcc))) {
            AssertFatal( 3 == strlen(mcc), "Bad MCC length, it must be 3 digit ex: 001");
            char c[2] = { mcc[0], 0};
            config_pP->gummei.gummei[i].plmn.mcc_digit1 = (uint8_t) atoi (c);
            c[0] = mcc[1];
            config_pP->gummei.gummei[i].plmn.mcc_digit2 = (uint8_t) atoi (c);
            c[0] = mcc[2];
            config_pP->gummei.gummei[i].plmn.mcc_digit3 = (uint8_t) atoi (c);
          }

          if ((config_setting_lookup_string (sub2setting, MME_CONFIG_STRING_MNC, &mnc))) {
            AssertFatal( (3 == strlen(mnc)) || (2 == strlen(mnc)) , "Bad MCC length, it must be 3 digit ex: 001");
            char c[2] = { mnc[0], 0};
            config_pP->gummei.gummei[i].plmn.mnc_digit1 = (uint8_t) atoi (c);
            c[0] = mnc[1];
            config_pP->gummei.gummei[i].plmn.mnc_digit2 = (uint8_t) atoi (c);
            if (3 == strlen(mnc)) {
              c[0] = mnc[2];
              config_pP->gummei.gummei[i].plmn.mnc_digit3 = (uint8_t) atoi (c);
            } else {
              config_pP->gummei.gummei[i].plmn.mnc_digit3 = 0x0F;
            }
          }

          if ((config_setting_lookup_string (sub2setting, MME_CONFIG_STRING_MME_GID, &mnc))) {
            config_pP->gummei.gummei[i].mme_gid = (uint16_t) atoi (mnc);
          }
          if ((config_setting_lookup_string (sub2setting, MME_CONFIG_STRING_MME_CODE, &mnc))) {
            config_pP->gummei.gummei[i].mme_code = (uint8_t) atoi (mnc);
          }
          config_pP->gummei.nb += 1;
        }
      }
    }
    // NETWORK INTERFACE SETTING
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);

    if (setting != NULL) {
      if ((config_setting_lookup_string (setting, MME_CONFIG_STRING_INTERFACE_NAME_FOR_S1_MME, (const char **)&if_name_s1_mme)
           && config_setting_lookup_string (setting, MME_CONFIG_STRING_IPV4_ADDRESS_FOR_S1_MME, (const char **)&s1_mme)
           && config_setting_lookup_string (setting, MME_CONFIG_STRING_INTERFACE_NAME_FOR_S11_MME, (const char **)&if_name_s11)
           && config_setting_lookup_string (setting, MME_CONFIG_STRING_IPV4_ADDRESS_FOR_S11_MME, (const char **)&s11)
           && config_setting_lookup_int (setting, MME_CONFIG_STRING_MME_PORT_FOR_S11, &aint)
          )
        ) {
        config_pP->ipv4.port_s11 = (uint16_t)aint;

        config_pP->ipv4.if_name_s1_mme = bfromcstr(if_name_s1_mme);
        cidr = bfromcstr (s1_mme);
        struct bstrList *list = bsplit (cidr, '/');
        AssertFatal(2 == list->qty, "Bad CIDR address %s", bdata(cidr));
        address = list->entry[0];
        mask    = list->entry[1];
        IPV4_STR_ADDR_TO_INT_NWBO (bdata(address), config_pP->ipv4.s1_mme, "BAD IP ADDRESS FORMAT FOR S1-MME !\n");
        config_pP->ipv4.netmask_s1_mme = atoi ((const char*)mask->data);
        bstrListDestroy(list);
        in_addr_var.s_addr = config_pP->ipv4.s1_mme;
        OAILOG_INFO (LOG_MME_APP, "Parsing configuration file found S1-MME: %s/%d on %s\n",
                       inet_ntoa (in_addr_var), config_pP->ipv4.netmask_s1_mme, bdata(config_pP->ipv4.if_name_s1_mme));

        config_pP->ipv4.if_name_s11 = bfromcstr(if_name_s11);
        cidr = bfromcstr (s11);
        list = bsplit (cidr, '/');
        AssertFatal(2 == list->qty, "Bad CIDR address %s", bdata(cidr));
        address = list->entry[0];
        mask    = list->entry[1];
        IPV4_STR_ADDR_TO_INT_NWBO (bdata(address), config_pP->ipv4.s11, "BAD IP ADDRESS FORMAT FOR S11 !\n");
        config_pP->ipv4.netmask_s11 = atoi ((const char*)mask->data);
        bstrListDestroy(list);
        in_addr_var.s_addr = config_pP->ipv4.s11;
        OAILOG_INFO (LOG_MME_APP, "Parsing configuration file found S11: %s/%d on %s\n",
                       inet_ntoa (in_addr_var), config_pP->ipv4.netmask_s11, bdata(config_pP->ipv4.if_name_s11));
      }
    }
    // NAS SETTING
    setting = config_setting_get_member (setting_mme, MME_CONFIG_STRING_NAS_CONFIG);

    if (setting != NULL) {
      subsetting = config_setting_get_member (setting, MME_CONFIG_STRING_NAS_SUPPORTED_INTEGRITY_ALGORITHM_LIST);

      if (subsetting != NULL) {
        num = config_setting_length (subsetting);

        if (num <= 8) {
          for (i = 0; i < num; i++) {
            astring = config_setting_get_string_elem (subsetting, i);

            if (strcmp ("EIA0", astring) == 0)
              config_pP->nas_config.prefered_integrity_algorithm[i] = EIA0_ALG_ID;
            else if (strcmp ("EIA1", astring) == 0)
              config_pP->nas_config.prefered_integrity_algorithm[i] = EIA1_128_ALG_ID;
            else if (strcmp ("EIA2", astring) == 0)
              config_pP->nas_config.prefered_integrity_algorithm[i] = EIA2_128_ALG_ID;
            else
              config_pP->nas_config.prefered_integrity_algorithm[i] = EIA0_ALG_ID;
          }

          for (i = num; i < 8; i++) {
            config_pP->nas_config.prefered_integrity_algorithm[i] = EIA0_ALG_ID;
          }
        }
      }

      subsetting = config_setting_get_member (setting, MME_CONFIG_STRING_NAS_SUPPORTED_CIPHERING_ALGORITHM_LIST);

      if (subsetting != NULL) {
        num = config_setting_length (subsetting);

        if (num <= 8) {
          for (i = 0; i < num; i++) {
            astring = config_setting_get_string_elem (subsetting, i);

            if (strcmp ("EEA0", astring) == 0)
              config_pP->nas_config.prefered_ciphering_algorithm[i] = EEA0_ALG_ID;
            else if (strcmp ("EEA1", astring) == 0)
              config_pP->nas_config.prefered_ciphering_algorithm[i] = EEA1_128_ALG_ID;
            else if (strcmp ("EEA2", astring) == 0)
              config_pP->nas_config.prefered_ciphering_algorithm[i] = EEA2_128_ALG_ID;
            else
              config_pP->nas_config.prefered_ciphering_algorithm[i] = EEA0_ALG_ID;
          }

          for (i = num; i < 8; i++) {
            config_pP->nas_config.prefered_ciphering_algorithm[i] = EEA0_ALG_ID;
          }
        }
      }
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_NAS_T3402_TIMER, &aint))) {
        config_pP->nas_config.t3402_min = (uint8_t) aint;
      }
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_NAS_T3412_TIMER, &aint))) {
        config_pP->nas_config.t3412_min = (uint8_t) aint;
      }
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_NAS_T3485_TIMER, &aint))) {
        config_pP->nas_config.t3485_sec = (uint8_t) aint;
      }
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_NAS_T3486_TIMER, &aint))) {
        config_pP->nas_config.t3486_sec = (uint8_t) aint;
      }
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_NAS_T3489_TIMER, &aint))) {
        config_pP->nas_config.t3489_sec = (uint8_t) aint;
      }
      if ((config_setting_lookup_int (setting, MME_CONFIG_STRING_NAS_T3495_TIMER, &aint))) {
        config_pP->nas_config.t3495_sec = (uint8_t) aint;
      }
    }
  }

  setting = config_lookup (&cfg, SGW_CONFIG_STRING_SGW_CONFIG);

  if (setting != NULL) {
    if ((config_setting_lookup_string (setting, SGW_CONFIG_STRING_SGW_IPV4_ADDRESS_FOR_S11, (const char **)&sgw_ip_address_for_s11)
        )
      ) {

      cidr = bfromcstr (sgw_ip_address_for_s11);
      struct bstrList *list = bsplit (cidr, '/');
      AssertFatal(2 == list->qty, "Bad CIDR address %s", bdata(cidr));
      address = list->entry[0];
      IPV4_STR_ADDR_TO_INT_NWBO (bdata(address), config_pP->ipv4.sgw_s11, "BAD IP ADDRESS FORMAT FOR SGW S11 !\n");
      bstrListDestroy(list);
      in_addr_var.s_addr = config_pP->ipv4.sgw_s11;
      OAILOG_INFO (LOG_SPGW_APP, "Parsing configuration file found S-GW S11: %s\n", inet_ntoa (in_addr_var));
    }
  }

  OAILOG_SET_CONFIG(&config_pP->log_config);
  config_destroy (&cfg);
  return 0;
}


//------------------------------------------------------------------------------
static void mme_config_display (mme_config_t * config_pP)
{
  int                                     j;

  OAILOG_INFO (LOG_CONFIG, "==== EURECOM %s v%s ====\n", PACKAGE_NAME, PACKAGE_VERSION);
  OAILOG_INFO (LOG_CONFIG, "Configuration:\n");
  OAILOG_INFO (LOG_CONFIG, "- File .................................: %s\n", bdata(config_pP->config_file));
  OAILOG_INFO (LOG_CONFIG, "- Realm ................................: %s\n", bdata(config_pP->realm));
  OAILOG_INFO (LOG_CONFIG, "- Run mode .............................: %s\n", (RUN_MODE_TEST == config_pP->run_mode) ? "TEST":"NORMAL");
  OAILOG_INFO (LOG_CONFIG, "- Max eNBs .............................: %u\n", config_pP->max_enbs);
  OAILOG_INFO (LOG_CONFIG, "- Max UEs ..............................: %u\n", config_pP->max_ues);
  OAILOG_INFO (LOG_CONFIG, "- IMS voice over PS session in S1 ......: %s\n", config_pP->eps_network_feature_support.ims_voice_over_ps_session_in_s1 == 0 ? "false" : "true");
  OAILOG_INFO (LOG_CONFIG, "- Emergency bearer services in S1 mode .: %s\n", config_pP->eps_network_feature_support.emergency_bearer_services_in_s1_mode == 0 ? "false" : "true");
  OAILOG_INFO (LOG_CONFIG, "- Location services via epc ............: %s\n", config_pP->eps_network_feature_support.location_services_via_epc == 0 ? "false" : "true");
  OAILOG_INFO (LOG_CONFIG, "- Extended service request .............: %s\n", config_pP->eps_network_feature_support.extended_service_request == 0 ? "false" : "true");
  OAILOG_INFO (LOG_CONFIG, "- Unauth IMSI support ..................: %s\n", config_pP->unauthenticated_imsi_supported == 0 ? "false" : "true");
  OAILOG_INFO (LOG_CONFIG, "- Relative capa ........................: %u\n", config_pP->relative_capacity);
  OAILOG_INFO (LOG_CONFIG, "- Statistics timer .....................: %u (seconds)\n\n", config_pP->mme_statistic_timer);
  OAILOG_INFO (LOG_CONFIG, "- S1-MME:\n");
  OAILOG_INFO (LOG_CONFIG, "    port number ......: %d\n", config_pP->s1ap_config.port_number);
  OAILOG_INFO (LOG_CONFIG, "- IP:\n");
  OAILOG_INFO (LOG_CONFIG, "    s1-MME iface .....: %s\n", bdata(config_pP->ipv4.if_name_s1_mme));
  OAILOG_INFO (LOG_CONFIG, "    s1-MME ip ........: %s\n", inet_ntoa (*((struct in_addr *)&config_pP->ipv4.s1_mme)));
  OAILOG_INFO (LOG_CONFIG, "    s11 MME iface ....: %s\n", bdata(config_pP->ipv4.if_name_s11));
  OAILOG_INFO (LOG_CONFIG, "    s11 MME port .....: %d\n", config_pP->ipv4.port_s11);
  OAILOG_INFO (LOG_CONFIG, "    s11 MME ip .......: %s\n", inet_ntoa (*((struct in_addr *)&config_pP->ipv4.s11)));
  OAILOG_INFO (LOG_CONFIG, "- ITTI:\n");
  OAILOG_INFO (LOG_CONFIG, "    queue size .......: %u (bytes)\n", config_pP->itti_config.queue_size);
  OAILOG_INFO (LOG_CONFIG, "    log file .........: %s\n", bdata(config_pP->itti_config.log_file));
  OAILOG_INFO (LOG_CONFIG, "- SCTP:\n");
  OAILOG_INFO (LOG_CONFIG, "    in streams .......: %u\n", config_pP->sctp_config.in_streams);
  OAILOG_INFO (LOG_CONFIG, "    out streams ......: %u\n", config_pP->sctp_config.out_streams);
  OAILOG_INFO (LOG_CONFIG, "- GUMMEIs (PLMN|MMEGI|MMEC):\n");
  for (j = 0; j < config_pP->gummei.nb; j++) {
    OAILOG_INFO (LOG_CONFIG, "            " PLMN_FMT "|%u|%u \n",
        PLMN_ARG(&config_pP->gummei.gummei[j].plmn), config_pP->gummei.gummei[j].mme_gid, config_pP->gummei.gummei[j].mme_code);
  }
  OAILOG_INFO (LOG_CONFIG, "- TAIs : (mcc.mnc:tac)\n");
  switch (config_pP->served_tai.list_type) {
  case TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_CONSECUTIVE_TACS:
    OAILOG_INFO (LOG_CONFIG, "- TAI list type one PLMN consecutive TACs\n");
    break;
  case TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_NON_CONSECUTIVE_TACS:
    OAILOG_INFO (LOG_CONFIG, "- TAI list type one PLMN non consecutive TACs\n");
    break;
  case TRACKING_AREA_IDENTITY_LIST_TYPE_MANY_PLMNS:
    OAILOG_INFO (LOG_CONFIG, "- TAI list type multiple PLMNs\n");
    break;
  }
  for (j = 0; j < config_pP->served_tai.nb_tai; j++) {
    if (config_pP->served_tai.plmn_mnc_len[j] == 2) {
      OAILOG_INFO (LOG_CONFIG, "            %3u.%3u:%u\n",
          config_pP->served_tai.plmn_mcc[j], config_pP->served_tai.plmn_mnc[j], config_pP->served_tai.tac[j]);
    } else {
      OAILOG_INFO (LOG_CONFIG, "            %3u.%03u:%u\n",
          config_pP->served_tai.plmn_mcc[j], config_pP->served_tai.plmn_mnc[j], config_pP->served_tai.tac[j]);
    }
  }

  OAILOG_INFO (LOG_CONFIG, "- S6A:\n");
  OAILOG_INFO (LOG_CONFIG, "    conf file ........: %s\n", bdata(config_pP->s6a_config.conf_file));
  OAILOG_INFO (LOG_CONFIG, "- Logging:\n");
  OAILOG_INFO (LOG_CONFIG, "    Output ..............: %s\n", bdata(config_pP->log_config.output));
  OAILOG_INFO (LOG_CONFIG, "    Output thread safe ..: %s\n", (config_pP->log_config.is_output_thread_safe) ? "true":"false");
  OAILOG_INFO (LOG_CONFIG, "    UDP log level........: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.udp_log_level));
  OAILOG_INFO (LOG_CONFIG, "    GTPV1-U log level....: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.gtpv1u_log_level));
  OAILOG_INFO (LOG_CONFIG, "    GTPV2-C log level....: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.gtpv2c_log_level));
  OAILOG_INFO (LOG_CONFIG, "    SCTP log level.......: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.sctp_log_level));
  OAILOG_INFO (LOG_CONFIG, "    S1AP log level.......: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.s1ap_log_level));
  OAILOG_INFO (LOG_CONFIG, "    ASN1 Verbosity level : %d\n", config_pP->log_config.asn1_verbosity_level);
  OAILOG_INFO (LOG_CONFIG, "    NAS log level........: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.nas_log_level));
  OAILOG_INFO (LOG_CONFIG, "    MME_APP log level....: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.mme_app_log_level));
  OAILOG_INFO (LOG_CONFIG, "    S/P-GW APP log level.: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.spgw_app_log_level));
  OAILOG_INFO (LOG_CONFIG, "    S11 log level........: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.s11_log_level));
  OAILOG_INFO (LOG_CONFIG, "    S6a log level........: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.s6a_log_level));
  OAILOG_INFO (LOG_CONFIG, "    UTIL log level.......: %s\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.util_log_level));
  OAILOG_INFO (LOG_CONFIG, "    MSC log level........: %s (MeSsage Chart)\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.msc_log_level));
  OAILOG_INFO (LOG_CONFIG, "    ITTI log level.......: %s (InTer-Task Interface)\n", OAILOG_LEVEL_INT2STR(config_pP->log_config.itti_log_level));
}

//------------------------------------------------------------------------------
static void usage (char *target)
{
  OAILOG_INFO (LOG_CONFIG, "==== EURECOM %s version: %s ====\n", PACKAGE_NAME, PACKAGE_VERSION);
  OAILOG_INFO (LOG_CONFIG, "Please report any bug to: %s\n", PACKAGE_BUGREPORT);
  OAILOG_INFO (LOG_CONFIG, "Usage: %s [options]\n", target);
  OAILOG_INFO (LOG_CONFIG, "Available options:\n");
  OAILOG_INFO (LOG_CONFIG, "-h      Print this help and return\n");
  OAILOG_INFO (LOG_CONFIG, "-c<path>\n");
  OAILOG_INFO (LOG_CONFIG, "        Set the configuration file for mme\n");
  OAILOG_INFO (LOG_CONFIG, "        See template in UTILS/CONF\n");
  OAILOG_INFO (LOG_CONFIG, "-K<file>\n");
  OAILOG_INFO (LOG_CONFIG, "        Output intertask messages to provided file\n");
  OAILOG_INFO (LOG_CONFIG, "-V      Print %s version and return\n", PACKAGE_NAME);
  OAILOG_INFO (LOG_CONFIG, "-v[1-2] Debug level:\n");
  OAILOG_INFO (LOG_CONFIG, "            1 -> ASN1 XER printf on and ASN1 debug off\n");
  OAILOG_INFO (LOG_CONFIG, "            2 -> ASN1 XER printf on and ASN1 debug on\n");
}

//------------------------------------------------------------------------------
int
mme_config_parse_opt_line (
  int argc,
  char *argv[],
  mme_config_t * config_pP)
{
  int                                     c;

  mme_config_init (config_pP);

  /*
   * Parsing command line
   */
  while ((c = getopt (argc, argv, "c:hi:K:v:V")) != -1) {
    switch (c) {
    case 'c':{
        /*
         * Store the given configuration file. If no file is given,
         * * * * then the default values will be used.
         */
        config_pP->config_file = blk2bstr(optarg, strlen(optarg));
        OAILOG_DEBUG (LOG_CONFIG, "%s mme_config.config_file %s\n", __FUNCTION__, bdata(config_pP->config_file));
      }
      break;

    case 'v':{
        config_pP->log_config.asn1_verbosity_level = atoi (optarg);
      }
      break;

    case 'V':{
        OAILOG_DEBUG (LOG_CONFIG, "==== EURECOM %s v%s ====" "Please report any bug to: %s\n", PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_BUGREPORT);
      }
      break;

    case 'K':
      config_pP->itti_config.log_file = blk2bstr (optarg, strlen(optarg));;
      OAILOG_DEBUG (LOG_CONFIG, "%s mme_config.itti_config.log_file %s\n", __FUNCTION__, bdata(config_pP->itti_config.log_file));
      break;

    case 'h':                  /* Fall through */
    default:
      usage (argv[0]);
      exit (0);
    }
  }

  /*
   * Parse the configuration file using libconfig
   */
  if (!config_pP->config_file) {
    config_pP->config_file = bfromcstr("/usr/local/etc/oai/mme.conf");
  }
  if (mme_config_parse_file (config_pP) != 0) {
    return -1;
  }

  /*
   * Display the configuration
   */
  mme_config_display (config_pP);
  return 0;
}
