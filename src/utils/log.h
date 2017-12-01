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


#ifndef FILE_LOG_SEEN
#define FILE_LOG_SEEN

#include "gcc_diag.h"
#include <syslog.h>

/* asn1c debug */
extern int asn_debug;
extern int asn1_xer_print;
extern int fd_g_debug_lvl;


#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "bstrlib.h"

#define LOG_CONFIG_STRING_LOGGING                        "LOGGING"
#define LOG_CONFIG_STRING_OUTPUT                         "OUTPUT"
#define LOG_CONFIG_STRING_OUTPUT_THREAD_SAFE             "THREAD_SAFE"
#define LOG_CONFIG_STRING_COLOR                          "COLOR"
#define LOG_CONFIG_STRING_OUTPUT_CONSOLE                 "CONSOLE"
#define LOG_CONFIG_STRING_OUTPUT_SYSLOG                  "SYSLOG"
#define LOG_CONFIG_STRING_GTPV1U_LOG_LEVEL               "GTPV1U_LOG_LEVEL"
#define LOG_CONFIG_STRING_GTPV2C_LOG_LEVEL               "GTPV2C_LOG_LEVEL"
#define LOG_CONFIG_STRING_ITTI_LOG_LEVEL                 "ITTI_LOG_LEVEL"
#define LOG_CONFIG_STRING_MME_APP_LOG_LEVEL              "MME_APP_LOG_LEVEL"
#define LOG_CONFIG_STRING_MSC_LOG_LEVEL                  "MSC_LOG_LEVEL"
#define LOG_CONFIG_STRING_NAS_LOG_LEVEL                  "NAS_LOG_LEVEL"
#define LOG_CONFIG_STRING_S11_LOG_LEVEL                  "S11_LOG_LEVEL"
#define LOG_CONFIG_STRING_S1AP_LOG_LEVEL                 "S1AP_LOG_LEVEL"
#define LOG_CONFIG_STRING_S6A_LOG_LEVEL                  "S6A_LOG_LEVEL"
#define LOG_CONFIG_STRING_SCTP_LOG_LEVEL                 "SCTP_LOG_LEVEL"
#define LOG_CONFIG_STRING_SPGW_APP_LOG_LEVEL             "SPGW_APP_LOG_LEVEL"
#define LOG_CONFIG_STRING_UDP_LOG_LEVEL                  "UDP_LOG_LEVEL"
#define LOG_CONFIG_STRING_UTIL_LOG_LEVEL                 "UTIL_LOG_LEVEL"

typedef enum {
  MIN_LOG_ENV = 0,
  LOG_MME_ENV = MIN_LOG_ENV,
  LOG_MME_GW_ENV,
  LOG_SPGW_ENV,
  MAX_LOG_ENV
} log_env_t;

typedef enum {
  MIN_LOG_LEVEL = 0,
  OAILOG_LEVEL_EMERGENCY = MIN_LOG_ENV,
  OAILOG_LEVEL_ALERT,
  OAILOG_LEVEL_CRITICAL,
  OAILOG_LEVEL_ERROR,
  OAILOG_LEVEL_WARNING,
  OAILOG_LEVEL_NOTICE,
  OAILOG_LEVEL_INFO,
  OAILOG_LEVEL_DEBUG,
  OAILOG_LEVEL_TRACE,
  MAX_LOG_LEVEL
} log_level_t;

typedef enum {
  MIN_LOG_PROTOS = 0,
  LOG_UDP = MIN_LOG_PROTOS,
  LOG_GTPV1U,
  LOG_GTPV2C,
  LOG_SCTP,
  LOG_S1AP,
  LOG_MME_APP,
  LOG_NAS,
  LOG_NAS_EMM,
  LOG_NAS_ESM,
  LOG_SPGW_APP,
  LOG_S11,
  LOG_S6A,
  LOG_UTIL,
  LOG_CONFIG,
  LOG_MSC,
  LOG_ITTI,
  MAX_LOG_PROTOS,
} log_proto_t;

/*! \struct  log_thread_ctxt_t
* \brief Structure containing a thread context.
*/
typedef struct log_thread_ctxt_s {
  int indent;
  pthread_t tid;
} log_thread_ctxt_t;

/*! \struct  log_queue_item_t
* \brief Structure containing a string to be logged.
* This structure is pushed in thread safe queues by thread producers of logs.
* This structure is then popped by a dedicated thread that will write the string
* in the opened stream ( file, tcp, stdout)
*/
typedef struct log_queue_item_s {
  int32_t                                 log_level; /*!< \brief log level for syslog. */
  bstring                                 bstr;      /*!< \brief string containing the message. */
} log_queue_item_t;

/*! \struct  log_config_t
* \brief Structure containing the dynamically configurable parameters of the Logging facilities.
* This structure is filled by configuration facilities when parsing a configuration file.
*/
typedef struct log_config_s {
  bstring       output;             /*!< \brief Where logs go, choice in { "CONSOLE", "`path to file`", "`IPv4@`:`TCP port num`"} . */
  bool          is_output_thread_safe; /*!< \brief Is final string goes in a thread safe buffer of is flushed without care . */
  log_level_t   udp_log_level;      /*!< \brief UDP ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   gtpv1u_log_level;   /*!< \brief GTPv1-U ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   gtpv2c_log_level;   /*!< \brief GTPv2-C ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   sctp_log_level;     /*!< \brief SCTP ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   s1ap_log_level;     /*!< \brief S1AP ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   nas_log_level;      /*!< \brief NAS ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   mme_app_log_level;  /*!< \brief MME-APP ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   spgw_app_log_level; /*!< \brief SP-GW ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   s11_log_level;      /*!< \brief S11 ITTI task log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   s6a_log_level;      /*!< \brief S6a layer log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   util_log_level;     /*!< \brief Misc utilities log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   msc_log_level;      /*!< \brief MSC utility log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  log_level_t   itti_log_level;     /*!< \brief ITTI layer log level starting from OAILOG_LEVEL_EMERGENCY up to MAX_LOG_LEVEL (no log) */
  uint8_t       asn1_verbosity_level; /*!< \brief related to asn1c generated code for S1AP verbosity level */
  bool          color;              /*!< \brief use of ANSI styling codes or no */
} log_config_t;

# if LOG_OAI

void log_connect_to_server(void);
void log_set_config(const log_config_t * const config);
const char * log_level_int2str(const log_level_t log_level);
log_level_t log_level_str2int(const char * const log_level_str);

int log_init(
  const log_env_t envP,
  const log_level_t default_log_levelP,
  const int max_threadsP);

void log_itti_connect(void);
void log_start_use(void);
void log_flush_messages(void) __attribute__ ((hot));
void log_exit(void);

void log_stream_hex(
  const log_level_t log_levelP,
  const log_proto_t protoP,
  const char *const source_fileP,
  const unsigned int line_numP,
  const char *const messageP,
  const char *const streamP,
  const size_t sizeP);

void log_stream_hex_array(
  const log_level_t log_levelP,
  const log_proto_t protoP,
  const char *const source_fileP,
  const unsigned int line_numP,
  const char *const messageP,
  const char *const streamP,
  const size_t sizeP);

void log_message_add (
  log_queue_item_t * contextP,
  char *format,
  ...) __attribute__ ((format (printf, 2, 3)));

void log_message_finish (
  log_queue_item_t * contextP);

void log_message_start (
  log_thread_ctxt_t * const thread_ctxtP,
  const log_level_t log_levelP,
  const log_proto_t protoP,
  log_queue_item_t ** contextP, // Out parameter
  const char *const source_fileP,
  const unsigned int line_numP,
  char *format,
  ...) __attribute__ ((format (printf, 7, 8)));

void
log_func (
  bool  is_entering,
  const log_proto_t protoP,
  const char *const source_fileP,
  const unsigned int line_numP,
  const char *const function);

void
log_func_return (
  const log_proto_t protoP,
  const char *const source_fileP,
  const unsigned int line_numP,
  const char *const functionP,
  const long return_codeP);

void log_message (
      log_thread_ctxt_t * const thread_ctxtP,
      const log_level_t log_levelP,
      const log_proto_t protoP,
      const char *const source_fileP,
      const unsigned int line_numP,
      char *format,
      ...) __attribute__ ((format (printf, 6, 7)));

int log_get_start_time_sec (void);

#    define OAILOG_SET_CONFIG                                           log_set_config
#    define OAILOG_LEVEL_STR2INT                                        log_level_str2int
#    define OAILOG_LEVEL_INT2STR                                        log_level_int2str
#    define OAILOG_INIT                                                 log_init
#    define OAILOG_START_USE                                            log_start_use
#    define OAILOG_ITTI_CONNECT                                         log_itti_connect
#    define OAILOG_EXIT()                                               log_exit()
#    define OAILOG_SPEC(pRoTo, ...)                                     do { log_message(NULL, OAILOG_LEVEL_NOTICE,   pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0)/*!< \brief 3GPP trace on specifications */
#    define OAILOG_EMERGENCY(pRoTo, ...)                                do { log_message(NULL, OAILOG_LEVEL_EMERGENCY,pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0)/*!< \brief system is unusable */
#    define OAILOG_ALERT(pRoTo, ...)                                    do { log_message(NULL, OAILOG_LEVEL_ALERT,    pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief action must be taken immediately */
#    define OAILOG_CRITICAL(pRoTo, ...)                                 do { log_message(NULL, OAILOG_LEVEL_CRITICAL, pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief critical conditions */
#    define OAILOG_ERROR(pRoTo, ...)                                    do { log_message(NULL, OAILOG_LEVEL_ERROR,    pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief error conditions */
#    define OAILOG_WARNING(pRoTo, ...)                                  do { log_message(NULL, OAILOG_LEVEL_WARNING,  pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief warning conditions */
#    define OAILOG_NOTICE(pRoTo, ...)                                   do { log_message(NULL, OAILOG_LEVEL_NOTICE,   pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief normal but significant condition */
#    define OAILOG_INFO(pRoTo, ...)                                     do { log_message(NULL, OAILOG_LEVEL_INFO,     pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief informational */
#    define OAILOG_MESSAGE_START(lOgLeVeL, pRoTo, cOnTeXt, ...)         do { log_message_start(NULL, lOgLeVeL, pRoTo, cOnTeXt, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief when need to log only 1 message with many char messages, ex formating a dumped struct */
#    define OAILOG_MESSAGE_ADD(cOnTeXt, ...)                            do { log_message_add(cOnTeXt, ##__VA_ARGS__); } while(0) /*!< \brief can be called as many times as needed after OAILOG_MESSAGE_START() */
#    define OAILOG_MESSAGE_FINISH(cOnTeXt)                              do { log_message_finish(cOnTeXt); } while(0) /*!< \brief Send the message built by OAILOG_MESSAGE_START() n*LOG_MESSAGE_ADD() (n=0..N) */
#    define OAILOG_STREAM_HEX(lOgLeVeL, pRoTo, mEsSaGe, sTrEaM, sIzE)   do { \
                                                                   OAI_GCC_DIAG_OFF(pointer-sign); \
                                                                   log_stream_hex(lOgLeVeL, pRoTo, __FILE__, __LINE__, mEsSaGe, sTrEaM, sIzE);\
                                                                   OAI_GCC_DIAG_ON(pointer-sign); \
                                                                 } while(0); /*!< \brief trace buffer content */
#    if DEBUG_IS_ON
#      define OAILOG_DEBUG(pRoTo, ...)                                  do { log_message(NULL, OAILOG_LEVEL_DEBUG,    pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief debug informations */
#      if TRACE_IS_ON
#        define OAILOG_EXTERNAL(lOgLeVeL, pRoTo, ...)                   do { log_message(NULL, lOgLeVeL       ,    pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0)
#        define OAILOG_TRACE(pRoTo, ...)                                do { log_message(NULL, OAILOG_LEVEL_TRACE,    pRoTo, __FILE__, __LINE__, ##__VA_ARGS__); } while(0) /*!< \brief most detailled informations, struct dumps */
#        define OAILOG_FUNC_IN(pRoTo)                                   do { log_func(true, pRoTo, __FILE__, __LINE__, __FUNCTION__); } while(0) /*!< \brief informational */
#        define OAILOG_FUNC_OUT(pRoTo)                                  do { log_func(false, pRoTo, __FILE__, __LINE__, __FUNCTION__); return;} while(0) /*!< \brief informational */
#        define OAILOG_FUNC_RETURN(pRoTo, rEtUrNcOdE)                   do { log_func_return(pRoTo, __FILE__, __LINE__, __FUNCTION__, (long)rEtUrNcOdE); return rEtUrNcOdE;} while(0) /*!< \brief informational */
#        define OAILOG_STREAM_HEX_ARRAY(pRoTo, mEsSaGe, sTrEaM, sIzE)       do { log_stream_hex_array(OAILOG_LEVEL_TRACE, pRoTo, __FILE__, __LINE__, mEsSaGe, sTrEaM, sIzE); } while(0) /*!< \brief trace buffer content with indexes */
#      endif
#    endif
#  else
#    define OAILOG_SPEC(...)
#    define OAILOG_SET_CONFIG(a)
#    define OAILOG_LEVEL_STR2INT(a)                                     OAILOG_LEVEL_EMERGENCY
#    define OAILOG_LEVEL_INT2STR(a)                                     "EMERGENCY"
#    define OAILOG_INIT(a,b,c)                                          0
#    define OAILOG_START_USE()
#    define OAILOG_ITTI_CONNECT()
#    define OAILOG_EXIT()
#    define OAILOG_EMERGENCY(...)
#    define OAILOG_ALERT(...)
#    define OAILOG_CRITICAL(...)
#    define OAILOG_ERROR(...)
#    define OAILOG_WARNING(...)
#    define OAILOG_NOTICE(...)
#    define OAILOG_INFO(...)
#    define OAILOG_MESSAGE_START(...)
#    define OAILOG_MESSAGE_ADD(...)
#    define OAILOG_MESSAGE_FINISH(cOnTeXt)
#  endif

#  if !defined(OAILOG_DEBUG)
#    define OAILOG_DEBUG(...)                                           {void;}
#  endif
#  if !defined(OAILOG_TRACE)
#    define OAILOG_TRACE(pRoTo, ...)                                    (void)(pRoTo)
#  endif
#  if !defined(OAILOG_EXTERNAL)
#    define OAILOG_EXTERNAL(lOgLeVeL, pRoTo, ...)                       (void)(lOgLeVeL), (void)(pRoTo)
#  endif
#  if !defined(OAILOG_FUNC_IN)
#    define OAILOG_FUNC_IN(pRoTo)                                       (void)(pRoTo)
#  endif
#  if !defined(OAILOG_FUNC_OUT)
#    define OAILOG_FUNC_OUT(pRoTo)                                      do{ return;} while 0
#  endif
#  if !defined(OAILOG_FUNC_RETURN)
#    define OAILOG_FUNC_RETURN(pRoTo, rEtUrNcOdE)                       do{ return rEtUrNcOdE;} while 0
#  endif
#  if !defined(OAILOG_STREAM_HEX)
#    define OAILOG_STREAM_HEX(...)                                      {void;}
#  endif
#  if !defined(OAILOG_STREAM_HEX_ARRAY)
#    define OAILOG_STREAM_HEX_ARRAY(...)                                {void;}
#  endif

#  if DAEMONIZE
#    define OAI_FPRINTF_ERR(...)                                     do {syslog (LOG_ERR,   ##__VA_ARGS__);} while(0)
#    define OAI_FPRINTF_INFO(...)                                    do {syslog (LOG_INFO , ##__VA_ARGS__);} while(0)
#    define OAI_VFPRINTF_ERR(...)                                    do {vsyslog (LOG_ERR , ##__VA_ARGS__);} while(0)
#    define OAI_VFPRINTF_INFO(...)                                   do {vsyslog (LOG_INFO , ##__VA_ARGS__);} while(0)
#    if EMIT_ASN_DEBUG_EXTERN
#      define ASN_DEBUG(...)                                         do {vsyslog (LOG_ERR , ##__VA_ARGS__);} while(0)
#    endif
#  else
#    define OAI_FPRINTF_ERR(...)                                     do {fprintf (stderr,   ##__VA_ARGS__);} while(0)
#    define OAI_FPRINTF_INFO(...)                                    do {fprintf (stdout,   ##__VA_ARGS__);} while(0)
#    define OAI_VFPRINTF_ERR(...)                                    do {vfprintf (stderr , ##__VA_ARGS__);} while(0)
#    define OAI_VFPRINTF_INFO(...)                                   do {vfprintf (stderr , ##__VA_ARGS__);} while(0)
#    if EMIT_ASN_DEBUG_EXTERN
#      define ASN_DEBUG(...)                                         do {vfprintf (stderr , ##__VA_ARGS__);} while(0)
#    endif
#  endif
#endif /* FILE_LOG_SEEN */
