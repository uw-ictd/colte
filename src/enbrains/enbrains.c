#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#include "enbrains.h"

static int enbrains_connected = 0;

// SMS NOTE: USE udp_intertask_interface as an example/model here!!!
static void * enbrains_intertask_interface (
  void *args_p)
{
  int                                     rc = 0;
  int                                     nb_events = 0;
  struct epoll_event                     *events = NULL;

  itti_mark_task_ready (TASK_ENBRAINS);
  OAILOG_START_USE ();
  MSC_START_USE ();

  OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ENBRAINS_INTERTASK_INTERFACE RUNNING!!!\n");

  while (1) {
    MessageDef                             *received_message_p = NULL;

    itti_receive_msg (TASK_ENBRAINS, &received_message_p);

    if (received_message_p != NULL) {
      OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ENBRAINS RECEIVED MESSAGE!!!\n");

      switch (ITTI_MSG_ID (received_message_p)) {
      case ENBRAINS_INIT:{
          // udp_init_t                             *udp_init_p = &received_message_p->ittiMsg.udp_init;
          // rc = udp_server_create_socket (udp_init_p->port, udp_init_p->address, ITTI_MSG_ORIGIN_ID (received_message_p));
          OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ENBRAINS_INIT CASE!!!\n");
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

    on_error:
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

// INSPIRED BY S11_MME_INIT
int enbrains_send_init_udp (char *address, uint16_t port_number)
{
  MessageDef *message_p = itti_alloc_new_message (TASK_ENBRAINS, UDP_INIT);
  if (message_p == NULL) {
    return RETURNerror;
  }

  message_p->ittiMsg.udp_init.port = port_number;
  message_p->ittiMsg.udp_init.address = address;
  OAILOG_DEBUG (LOG_ENBRAINS, "Tx UDP_INIT IP addr %s:%d\n", message_p->ittiMsg.udp_init.address, message_p->ittiMsg.udp_init.port);
  return itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
}

int enbrains_init (void)
{
  char *localhost_str = "127.0.0.1";

  OAILOG_DEBUG (LOG_ENBRAINS, "Initializing ENBRAINS task interface\n");
  // STAILQ_INIT (&udp_socket_list);

  if (itti_create_task (TASK_ENBRAINS, &enbrains_intertask_interface, NULL) < 0) {
    OAILOG_ERROR (LOG_ENBRAINS, "enbrains pthread_create (%s)\n", strerror (errno));
    return -1;
  }

  // localhost_str = inet_ntoa(0x7F000001);
  OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ADDRESS STRING = %s\n", localhost_str);
  DevAssert (localhost_str);
  enbrains_send_init_udp (localhost_str, ENBRAINS_MME_PORT);

  OAILOG_DEBUG (LOG_ENBRAINS, "Initializing ENBRAINS task interface: DONE\n");
  return 0;
}

// this function just *SENDS* a message to the enbrains module, so it's mme/hss/spgw agnostic
static int enbrains_send_udp_msg (uint8_t * buffer, uint32_t buffer_len)
{
  // Create and alloc new message
  MessageDef                             *message_p;
  udp_data_req_t                         *udp_data_req_p;
  int                                     ret = 0;

  message_p = itti_alloc_new_message (TASK_ENBRAINS, UDP_DATA_REQ);
  udp_data_req_p = &message_p->ittiMsg.udp_data_req;

  // (address and port hard-coded for now)
  udp_data_req_p->peer_address = LOCALHOST_HEX;
  udp_data_req_p->peer_port = ENBRAINS_PORT;

  udp_data_req_p->buffer = buffer;
  udp_data_req_p->buffer_length = buffer_len;
  return itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
}
