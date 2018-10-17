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
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>

#include "hss_config.h"
#include "db_proto.h"
#include "s6a_proto.h"
#include "auc.h"
#include "pid_file.h"

hss_config_t                            hss_config;

#define SPENCER_COMMAND_REMOVE_IMSI 0
#define SPENCER_COMMAND_ADD_IMSI 1
typedef struct spencer_msg {
  uint8_t command;
  char    imsi[16];
} spencer_msg_t;

void add_back_imsi(char *imsi) {
  printf("SMS ERROR: add_back_imsi not yet written! got imsi %s\n", imsi);
}

int spencer_listening_server(void) {
  int sockfd; /* socket */
  int portno = 62880; /* port to listen on */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
  struct sockaddr_in serveraddr; /* server's addr */
  spencer_msg_t msg;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("ERROR opening socket:");
    exit(1);
  }

  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  serveraddr.sin_port = htons((unsigned short)portno);

  if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
    perror("ERROR on binding:");
    exit(1);
  }

  while (1) {
    bzero((char *)&msg, sizeof(spencer_msg_t));
    n = recv(sockfd, (char *)&msg, sizeof(spencer_msg_t), 0);
    if (n < 0) {
      perror("ERROR in recvfrom:");
      exit(1);
    }

    if (msg.command == SPENCER_COMMAND_REMOVE_IMSI) {
      printf("SMS CLR: RECEIVED COMMAND TO REMOVE IMSI %s\n", msg.imsi);
      s6a_generate_cancel_location_req(msg.imsi);
    } else if (msg.command == SPENCER_COMMAND_ADD_IMSI) {
      add_back_imsi(msg.imsi);
    }
  }
}

int
main (
  int argc,
  char *argv[])
{
  char   *pid_file_name = NULL;

  pid_file_name = get_exe_basename();

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
    free(pid_file_name);
    exit (-EDEADLK);
  }
#else
  if (! is_pid_file_lock_success(pid_file_name)) {
    free(pid_file_name);
    exit (-EDEADLK);
  }
#endif


  memset (&hss_config, 0, sizeof (hss_config_t));

  if (hss_config_init (argc, argv, &hss_config) != 0) {
    return -1;
  }

  if (hss_mysql_connect (&hss_config) != 0) {
    return -1;
  }

  random_init ();

  // if (hss_config.valid_op) {
  //   hss_mysql_check_opc_keys ((uint8_t *) hss_config.operator_key_bin);
  // }

  s6a_init (&hss_config);

  // while (1) {
    /*
     * TODO: handle signals here
     */
    // sleep (1);
  // }
  // SMS NOTE: Past code just infinite-looped sleep() above. New code calls
  // a network socket listener that just listens for my message passing.
  spencer_listening_server();

  pid_file_unlock();
  free(pid_file_name);
  return 0;
}
