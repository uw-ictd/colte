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
#include "mme_config.h"

#include "intertask_interface_init.h"

#include "sctp_primitives_server.h"
#include "udp_primitives_server.h"
#include "s1ap_mme.h"
#include "timer.h"
#include "mme_app_extern.h"
#include "nas_defs.h"
#include "s11_mme.h"

/* FreeDiameter headers for support of S6A interface */
#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>
#include "s6a_defs.h"

// #include "oai_mme.h"
#include "pid_file.h"

#include "enbrains.h"

static void * enbrains_main_intertask_interface (
  void *args_p)
{
  int                                     rc = 0;
  // int                                     nb_events = 0;
  // struct epoll_event                     *events = NULL;

  itti_mark_task_ready (TASK_ENBRAINS);
  OAILOG_START_USE ();
  MSC_START_USE ();

  // OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ENBRAINS_MAIN_INTERTASK_INTERFACE RUNNING!!!\n");
  printf("SMS: ENBRAINS_MAIN_INTERTASK_INTERFACE RUNNING!!!\n");

  while (1) {
    MessageDef                             *received_message_p = NULL;

    itti_receive_msg (TASK_ENBRAINS, &received_message_p);

    if (received_message_p != NULL) {
      OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ENBRAINS MAIN RECEIVED MESSAGE!!!\n");

      switch (ITTI_MSG_ID (received_message_p)) {
      case ENBRAINS_INIT:{
          // udp_init_t                             *udp_init_p = &received_message_p->ittiMsg.udp_init;
          // rc = udp_server_create_socket (udp_init_p->port, udp_init_p->address, ITTI_MSG_ORIGIN_ID (received_message_p));
          OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ENBRAINS MAIN INIT CASE!!!\n");
        }
        break;

      case TERMINATE_MESSAGE:{
          // enbrains_exit();
          itti_exit_task ();
        }
        break;

      default:{
          OAILOG_DEBUG (LOG_ENBRAINS, "Unknown message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
        }
        break;
      }

    // on_error:
      rc = itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
      AssertFatal (rc == EXIT_SUCCESS, "Failed to free memory (%d)!\n", rc);
      received_message_p = NULL;
    }

    // SMS NOTE: MAYBE HAVE TO UNCOMMENT THE LINES BELOW? I THINK THEY'RE FOR MULTI-SOCKETS...
    // nb_events = itti_get_events (TASK_ENBRAINS, &events);
    // if ((nb_events > 0) && (events != NULL)) {
      /*
       * Now handle notifications for other sockets
       */
    //   udp_server_flush_sockets (events, nb_events);
    // }
  }

  return NULL;
}

static int enbrains_main_send_udp_msg (
  uint8_t * buffer,
  uint32_t buffer_len
  // uint32_t peerIpAddr,
  // uint32_t peerPort
  )
{
  // Create and alloc new message
  MessageDef                             *message_p;
  udp_data_req_t                         *udp_data_req_p;
  // int                                     ret = 0;

  message_p = itti_alloc_new_message (TASK_ENBRAINS, UDP_DATA_REQ);
  udp_data_req_p = &message_p->ittiMsg.udp_data_req;

  // (address and port hard-coded for now)
  udp_data_req_p->peer_address = LOCALHOST_HEX;
  udp_data_req_p->peer_port = ENBRAINS_MME_PORT;

  udp_data_req_p->buffer = buffer;
  udp_data_req_p->buffer_length = buffer_len;
  return itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
}

int enbrains_main_init (void)
{
  char *localhost_str = "127.0.0.1";
  uint8_t buffer[5];

  printf("Initializing ENBRAINS main task interface...\n");

  // OAILOG_DEBUG (LOG_ENBRAINS, "Initializing ENBRAINS task interface\n");
  // STAILQ_INIT (&udp_socket_list);

  if (itti_create_task (TASK_ENBRAINS, &enbrains_main_intertask_interface, NULL) < 0) {
    // OAILOG_ERROR (LOG_ENBRAINS, "enbrains pthread_create (%s)\n", strerror (errno));
    return -1;
  }

  // localhost_str = inet_ntoa(0x7F000001);
  // OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ADDRESS STRING = %s\n", localhost_str);
  DevAssert (localhost_str);
  enbrains_send_init_udp (localhost_str, ENBRAINS_PORT);

  // OAILOG_DEBUG (LOG_ENBRAINS, "Initializing ENBRAINS task interface: DONE\n");

  sleep(10);

  buffer[0] = 0;
  buffer[1] = 1;
  buffer[2] = 2;
  buffer[3] = 3;
  buffer[4] = 4;
  enbrains_main_send_udp_msg(buffer, 5);
  printf("...Initialized!\n");

  return 0;
}

// copied from oai_mme.c main
int
main (int argc, char *argv[])
{
  // char *pid_dir;
  // char *pid_file_name;

  CHECK_INIT_RETURN (OAILOG_INIT (LOG_SPGW_ENV, OAILOG_LEVEL_DEBUG, MAX_LOG_PROTOS));
  openlog(NULL, 0, LOG_DAEMON);

  /*
   * Parse the command line for options and set the mme_config accordingly.
   */
  // CHECK_INIT_RETURN (mme_config_parse_opt_line (argc, argv, &mme_config));

  /*
   * Calling each layer init function
   */

  CHECK_INIT_RETURN (itti_init (TASK_MAX, THREAD_MAX, MESSAGES_ID_MAX, tasks_info, messages_info, NULL, NULL));
  MSC_INIT (MSC_MME, THREAD_MAX + TASK_MAX);

  // CHECK_INIT_RETURN (enbrains_app_init (&mme_config));
  OAILOG_DEBUG(LOG_ENBRAINS, "LOG TESTING\n");

  CHECK_INIT_RETURN (udp_init ());
  CHECK_INIT_RETURN (enbrains_main_init());

  printf("ENBRAINS app initialization complete!\n");

  /*
   * Handle signals here
   */

  itti_wait_tasks_end ();
  // pid_file_unlock();
  // free_wrapper((void**) &pid_file_name);
  return 0;
}