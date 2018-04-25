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

#  include <stdarg.h>

#  if DAEMONIZE
#    include <syslog.h>
#    define FPRINTF_ERROR(...)                                   do {syslog (LOG_ERR,     ##__VA_ARGS__);} while(0)
#    define FPRINTF_NOTICE(...)                                  do {syslog (LOG_NOTICE , ##__VA_ARGS__);} while(0)
#    define FPRINTF_INFO(...)                                    do {syslog (LOG_INFO ,   ##__VA_ARGS__);} while(0)
#    define FPRINTF_DEBUG(...)                                   do {syslog (LOG_DEBUG ,  ##__VA_ARGS__);} while(0)
#    define VFPRINTF_ERR(...)                                    do {vsyslog (LOG_ERR ,   ##__VA_ARGS__);} while(0)
#    define VFPRINTF_INFO(...)                                   do {vsyslog (LOG_INFO ,  ##__VA_ARGS__);} while(0)
#    define VFPRINTF_DEBUG(...)                                  do {vsyslog (LOG_DEBUG , ##__VA_ARGS__);} while(0)
#  else
#    define FPRINTF_ERROR(...)                                   do {fprintf (stderr,   ##__VA_ARGS__);} while(0)
#    define FPRINTF_NOTICE(...)                                  do {fprintf (stdout,   ##__VA_ARGS__);} while(0)
#    define FPRINTF_INFO(...)                                    do {fprintf (stdout,   ##__VA_ARGS__);} while(0)
#    define FPRINTF_DEBUG(...)                                   do {fprintf (stdout,   ##__VA_ARGS__);} while(0)
#    define VFPRINTF_ERROR(...)                                  do {vfprintf (stderr , ##__VA_ARGS__);} while(0)
#    define VFPRINTF_INFO(...)                                   do {vfprintf (stdout , ##__VA_ARGS__);} while(0)
#    define VFPRINTF_DEBUG(...)                                  do {vfprintf (stdout , ##__VA_ARGS__);} while(0)
#  endif
#endif /* FILE_LOG_SEEN */
