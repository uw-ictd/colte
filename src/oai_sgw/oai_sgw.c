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

/*! \file oai_sgw.c
  \brief
  \author Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>

#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "dynamic_memory_check.h"
#include "assertions.h"
#include "log.h"
#include "msc.h"
#include "intertask_interface_init.h"
#include "spgw_config.h"
#include "udp_primitives_server.h"
#include "s11_sgw.h"
#include "sgw_defs.h"
#include "gtpv1u_sgw_defs.h"
#include "oai_sgw.h"
#include "pid_file.h"
#include "timer.h"


int
main (
  int argc,
  char *argv[])
{
  char   *pid_file_name = NULL;

  // Currently hard-coded value. TODO: add as config option.
  pid_file_name = get_exe_absolute_path("/var/run");

#if DAEMONIZE
  pid_t pid, sid; // Our process ID and Session ID

  // Fork off the parent process
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  // If we got a good PID, then we can exit the parent process.
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  // Change the file mode mask
  umask(0);

  // Create a new SID for the child process
  sid = setsid();
  if (sid < 0) {
    exit(EXIT_FAILURE); // Log the failure
  }

  // Change the current working directory
  if ((chdir("/")) < 0) {
    // Log the failure
    exit(EXIT_FAILURE);
  }

  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  openlog(NULL, 0, LOG_DAEMON);

  if (! is_pid_file_lock_success(pid_file_name)) {
    closelog();
    free_wrapper((void **) &pid_file_name);
    exit (-EDEADLK);
  }
#else
  if (! is_pid_file_lock_success(pid_file_name)) {
    free_wrapper((void**) &pid_file_name);
    exit (-EDEADLK);
  }
#endif


  CHECK_INIT_RETURN (OAILOG_INIT (LOG_SPGW_ENV, OAILOG_LEVEL_NOTICE, MAX_LOG_PROTOS));
  /*
   * Parse the command line for options and set the mme_config accordingly.
   */
  CHECK_INIT_RETURN (spgw_config_parse_opt_line (argc, argv, &spgw_config));
  /*
   * Calling each layer init function
   */
  CHECK_INIT_RETURN (itti_init (TASK_MAX, THREAD_MAX, MESSAGES_ID_MAX, tasks_info, messages_info,
#if ENABLE_ITTI_ANALYZER
          messages_definition_xml,
#else
          NULL,
#endif
          NULL));
  MSC_INIT (MSC_SP_GW, THREAD_MAX + TASK_MAX);
  CHECK_INIT_RETURN (udp_init ());
  CHECK_INIT_RETURN (s11_sgw_init (&spgw_config.sgw_config));
  //CHECK_INIT_RETURN (gtpv1u_init (&spgw_config));
  CHECK_INIT_RETURN (sgw_init (&spgw_config));
  /*
   * Handle signals here
   */
  itti_wait_tasks_end ();
  pid_file_unlock();
  free_wrapper((void**) &pid_file_name);
  return 0;
}
