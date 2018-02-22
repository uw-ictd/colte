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

static int enbrains_mme_connected = 0;

static int enbrains_mme_event_processor(eb_msg_t *ebm) {

  switch(ebm->code) {
    case EB_CODE_HELLO: {
      OAILOG_DEBUG(LOG_ENBRAINS, "Received HELLO message from Enbrains module, connected.\n");
      enbrains_mme_connected = 1;
      break;
    }

    case EB_CODE_GOODBYE: {
      OAILOG_DEBUG(LOG_ENBRAINS, "Received GOODBYE message from Enbrains module, disconnected.\n");
      enbrains_mme_connected = 0;
      break;
    }
  }

  return 0;
}


// SMS NOTE: used udp_intertask_interface as an example/model here!!!
static void * enbrains_mme_intertask_interface (
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

      switch (ITTI_MSG_ID (received_message_p)) {
      case UDP_DATA_IND: {
        udp_data_ind_t *udp_data_ind;
        udp_data_ind = &received_message_p->ittiMsg.udp_data_ind;

        // (validation)
        if (udp_data_ind->peer_port != ENBRAINS_PORT) {
          OAILOG_DEBUG(LOG_ENBRAINS, "SMS ENBRAINS: Received a message from the wrong port! %d\n", udp_data_ind->peer_port);
          break;
        }

        // call the event/signal processor
        eb_msg_t *ebm = (eb_msg_t *) udp_data_ind;
        enbrains_mme_event_processor(ebm);

        break;
      }

      case TERMINATE_MESSAGE: {
        // enbrains_exit();
        itti_exit_task ();
        }
        break;

      default: {
        OAILOG_DEBUG (LOG_ENBRAINS, "Unknown message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
        }
        break;
      }

    on_error:
      rc = itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
      AssertFatal (rc == EXIT_SUCCESS, "Failed to free memory (%d)!\n", rc);
      received_message_p = NULL;
    }
  }
  return NULL;
}

int enbrains_mme_init (void)
{
  char *localhost_str = "127.0.0.1";

  OAILOG_DEBUG (LOG_ENBRAINS, "Initializing ENBRAINS task interface...\n");
  // STAILQ_INIT (&udp_socket_list);

  if (itti_create_task (TASK_ENBRAINS, &enbrains_mme_intertask_interface, NULL) < 0) {
    OAILOG_ERROR (LOG_ENBRAINS, "enbrains pthread_create (%s)\n", strerror (errno));
    return -1;
  }

  // localhost_str = inet_ntoa(0x7F000001);
  // OAILOG_DEBUG (LOG_ENBRAINS, "SMS: ADDRESS STRING = %s\n", localhost_str);
  DevAssert (localhost_str);
  enbrains_send_init_udp (localhost_str, ENBRAINS_MME_PORT);

  OAILOG_DEBUG (LOG_ENBRAINS, "Initializing ENBRAINS task interface: DONE\n");
  return 0;
}

