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

/*! \file udp_primitives_server.c
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/

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

#include "assertions.h"
#include "dynamic_memory_check.h"
#include "queue.h"
#include "log.h"
#include "msc.h"
#include "conversions.h"
#include "intertask_interface.h"
#include "udp_primitives_server.h"


struct udp_socket_desc_s {
  uint8_t                                 buffer[4096];
  int                                     sd;   /* Socket descriptor to use */

  pthread_t                               listener_thread;      /* Thread affected to recv */

  char                                   *local_address;        /* Local ipv4 address to use */
  uint16_t                                local_port;   /* Local port to use */

  task_id_t                               task_id;      /* Task who has requested the new endpoint */

                                          STAILQ_ENTRY (
  udp_socket_desc_s)                      entries;
};

static void udp_exit(void);

static
STAILQ_HEAD (
  udp_socket_list_s,
  udp_socket_desc_s) udp_socket_list;
     static pthread_mutex_t                  udp_socket_list_mutex = PTHREAD_MUTEX_INITIALIZER;


static void                             udp_server_receive_and_process (
  struct udp_socket_desc_s *udp_sock_pP);


/* @brief Retrieve the descriptor associated with the task_id
*/
static
struct udp_socket_desc_s               *
udp_server_get_socket_desc (
  task_id_t task_id)
{
  struct udp_socket_desc_s               *udp_sock_p = NULL;

  OAILOG_DEBUG (LOG_UDP, "Looking for task %d\n", task_id);
  STAILQ_FOREACH (udp_sock_p, &udp_socket_list, entries) {
    if (udp_sock_p->task_id == task_id) {
      OAILOG_DEBUG (LOG_UDP, "Found matching task desc\n");
      break;
    }
  }
  return udp_sock_p;
}

static
struct udp_socket_desc_s               *
udp_server_get_socket_desc_by_sd (
  int sdP)
{
  struct udp_socket_desc_s               *udp_sock_p = NULL;

  OAILOG_DEBUG (LOG_UDP, "Looking for sd %d\n", sdP);
  STAILQ_FOREACH (udp_sock_p, &udp_socket_list, entries) {
    if (udp_sock_p->sd == sdP) {
      OAILOG_DEBUG (LOG_UDP, "Found matching task desc\n");
      break;
    }
  }
  return udp_sock_p;
}

static
  int
udp_server_create_socket (
  int port,
  char *address,
  task_id_t task_id)
{
  struct sockaddr_in                      addr;
  int                                     sd;
  struct udp_socket_desc_s               *socket_desc_p = NULL;

  OAILOG_DEBUG (LOG_UDP, "Creating new listen socket on address " IPV4_ADDR " and port %u\n", IPV4_ADDR_FORMAT (inet_addr (address)), port);

  /*
   * Create UDP socket
   */
  if ((sd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    /*
     * Socket creation has failed...
     */
    OAILOG_ERROR (LOG_UDP, "Socket creation failed (%s)\n", strerror (errno));
    return sd;
  }

  memset (&addr, 0, sizeof (struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  addr.sin_addr.s_addr = inet_addr (address);

  if (bind (sd, (struct sockaddr *)&addr, sizeof (struct sockaddr_in)) < 0) {
    /*
     * Bind failed
     */
    OAILOG_ERROR (LOG_UDP, "Socket bind failed (%s) for address " IPV4_ADDR " and port %u\n", strerror (errno), IPV4_ADDR_FORMAT (inet_addr (address)), port);
    close (sd);
    return -1;
  }

  /*
   * Add the socket to list of fd monitored by ITTI
   */
  /*
   * Mark the socket as non-blocking
   */
  if (fcntl (sd, F_SETFL, O_NONBLOCK) < 0) {
    OAILOG_ERROR (LOG_UDP, "fcntl F_SETFL O_NONBLOCK failed: %s\n", strerror (errno));
    close (sd);
    return -1;
  }

  socket_desc_p = calloc (1, sizeof (struct udp_socket_desc_s));
  DevAssert (socket_desc_p != NULL);
  socket_desc_p->sd = sd;
  socket_desc_p->local_address = address;
  socket_desc_p->local_port = port;
  socket_desc_p->task_id = task_id;
  OAILOG_DEBUG (LOG_UDP, "Inserting new descriptor for task %d, sd %d\n", socket_desc_p->task_id, socket_desc_p->sd);
  pthread_mutex_lock (&udp_socket_list_mutex);
  STAILQ_INSERT_TAIL (&udp_socket_list, socket_desc_p, entries);
  pthread_mutex_unlock (&udp_socket_list_mutex);
  itti_subscribe_event_fd (TASK_UDP, sd);
  return sd;
}

static void
udp_server_flush_sockets (
  struct epoll_event *events,
  int nb_events)
{
  int                                     event;
  struct udp_socket_desc_s               *udp_sock_p = NULL;

  OAILOG_DEBUG (LOG_UDP, "Received %d events\n", nb_events);

  for (event = 0; event < nb_events; event++) {
    if (events[event].events != 0) {
      /*
       * If the event has not been yet been processed (not an itti message)
       */
      pthread_mutex_lock (&udp_socket_list_mutex);
      udp_sock_p = udp_server_get_socket_desc_by_sd (events[event].data.fd);

      if (udp_sock_p != NULL) {
        udp_server_receive_and_process (udp_sock_p);
      } else {
        OAILOG_ERROR (LOG_UDP, "Failed to retrieve the udp socket descriptor %d", events[event].data.fd);
      }

      pthread_mutex_unlock (&udp_socket_list_mutex);
    }
  }
}

static void
udp_server_receive_and_process (
  struct udp_socket_desc_s *udp_sock_pP)
{
  OAILOG_DEBUG (LOG_UDP, "Inserting new descriptor for task %d, sd %d\n", udp_sock_pP->task_id, udp_sock_pP->sd);
  {
    int                                     bytes_received = 0;
    socklen_t                               from_len;
    struct sockaddr_in                      addr;

    from_len = (socklen_t) sizeof (struct sockaddr_in);

    if ((bytes_received = recvfrom (udp_sock_pP->sd, udp_sock_pP->buffer, sizeof (udp_sock_pP->buffer), 0, (struct sockaddr *)&addr, &from_len)) <= 0) {
      OAILOG_ERROR (LOG_UDP, "Recvfrom failed %s\n", strerror (errno));
      //break;
    } else {
      MessageDef                             *message_p = NULL;
      udp_data_ind_t                         *udp_data_ind_p;
      uint8_t                                *forwarded_buffer = NULL;

      AssertFatal (sizeof (udp_sock_pP->buffer) >= bytes_received, "UDP BUFFER OVERFLOW");
      forwarded_buffer = itti_malloc (TASK_UDP, udp_sock_pP->task_id, bytes_received);
      DevAssert (forwarded_buffer != NULL);
      memcpy (forwarded_buffer, udp_sock_pP->buffer, bytes_received);
      message_p = itti_alloc_new_message (TASK_UDP, UDP_DATA_IND);
      DevAssert (message_p != NULL);
      udp_data_ind_p = &message_p->ittiMsg.udp_data_ind;
      udp_data_ind_p->buffer = forwarded_buffer;
      udp_data_ind_p->buffer_length = bytes_received;
      udp_data_ind_p->peer_port = htons (addr.sin_port);
      udp_data_ind_p->peer_address = addr.sin_addr.s_addr;
      OAILOG_DEBUG (LOG_UDP, "Msg of length %d received from %s:%u\n", bytes_received, inet_ntoa (addr.sin_addr), ntohs (addr.sin_port));

      if (itti_send_msg_to_task (udp_sock_pP->task_id, INSTANCE_DEFAULT, message_p) < 0) {
        OAILOG_DEBUG (LOG_UDP, "Failed to send message %d to task %d\n", UDP_DATA_IND, udp_sock_pP->task_id);
        //break;
      }
    }
  }
  //close(udp_sock_pP->sd);
  //udp_sock_pP->sd = -1;
  //pthread_mutex_lock(&udp_socket_list_mutex);
  //STAILQ_REMOVE(&udp_socket_list, udp_sock_pP, udp_socket_desc_s, entries);
  //pthread_mutex_unlock(&udp_socket_list_mutex);
  //return NULL;
}


static void                            *
udp_intertask_interface (
  void *args_p)
{
  int                                     rc = 0;
  int                                     nb_events = 0;
  struct epoll_event                     *events = NULL;

  itti_mark_task_ready (TASK_UDP);
  OAILOG_START_USE ();
  MSC_START_USE ();

  while (1) {
    MessageDef                             *received_message_p = NULL;

    itti_receive_msg (TASK_UDP, &received_message_p);

    if (received_message_p != NULL) {
      switch (ITTI_MSG_ID (received_message_p)) {
      case UDP_INIT:{
          udp_init_t                             *udp_init_p = &received_message_p->ittiMsg.udp_init;
          rc = udp_server_create_socket (udp_init_p->port, udp_init_p->address, ITTI_MSG_ORIGIN_ID (received_message_p));
        }
        break;

      case UDP_DATA_REQ:{
          int                                     udp_sd = -1;
          ssize_t                                 bytes_written;
          struct udp_socket_desc_s               *udp_sock_p = NULL;
          udp_data_req_t                         *udp_data_req_p;
          struct sockaddr_in                      peer_addr;

          udp_data_req_p = &received_message_p->ittiMsg.udp_data_req;
          //UDP_DEBUG("-- UDP_DATA_REQ -----------------------------------------------------\n%s :\n",
          //        __FUNCTION__);
          //udp_print_hex_octets(&udp_data_req_p->buffer[udp_data_req_p->buffer_offset],
          //        udp_data_req_p->buffer_length);
          memset (&peer_addr, 0, sizeof (struct sockaddr_in));
          peer_addr.sin_family = AF_INET;
          peer_addr.sin_port = htons (udp_data_req_p->peer_port);
          peer_addr.sin_addr.s_addr = udp_data_req_p->peer_address;
          pthread_mutex_lock (&udp_socket_list_mutex);
          udp_sock_p = udp_server_get_socket_desc (ITTI_MSG_ORIGIN_ID (received_message_p));

          if (udp_sock_p == NULL) {
            OAILOG_ERROR (LOG_UDP, "Failed to retrieve the udp socket descriptor " "associated with task %d\n", ITTI_MSG_ORIGIN_ID (received_message_p));
            pthread_mutex_unlock (&udp_socket_list_mutex);
            // no free udp_data_req_p->buffer, statically allocated
            goto on_error;
          }

          udp_sd = udp_sock_p->sd;
          pthread_mutex_unlock (&udp_socket_list_mutex);
          OAILOG_DEBUG (LOG_UDP, "[%d] Sending message of size %u to " IPV4_ADDR " and port %u\n", udp_sd, udp_data_req_p->buffer_length, IPV4_ADDR_FORMAT (udp_data_req_p->peer_address), udp_data_req_p->peer_port);
          bytes_written = sendto (udp_sd, &udp_data_req_p->buffer[udp_data_req_p->buffer_offset], udp_data_req_p->buffer_length, 0, (struct sockaddr *)&peer_addr, sizeof (struct sockaddr_in));
          // no free udp_data_req_p->buffer, statically allocated

          if (bytes_written != udp_data_req_p->buffer_length) {
            OAILOG_ERROR (LOG_UDP, "There was an error while writing to socket " "(%d:%s)\n", errno, strerror (errno));
          }
        }
        break;

      case TERMINATE_MESSAGE:{
          udp_exit();
          itti_exit_task ();
        }
        break;

      case MESSAGE_TEST:{
        }
        break;

      default:{
          OAILOG_DEBUG (LOG_UDP, "Unkwnon message ID %d:%s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
        }
        break;
      }

    on_error:
      rc = itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
      AssertFatal (rc == EXIT_SUCCESS, "Failed to free memory (%d)!\n", rc);
      received_message_p = NULL;
    }

    nb_events = itti_get_events (TASK_UDP, &events);

    if ((nb_events > 0) && (events != NULL)) {
      /*
       * Now handle notifications for other sockets
       */
      udp_server_flush_sockets (events, nb_events);
    }
  }

  return NULL;
}

int udp_init (void)
{
  OAILOG_DEBUG (LOG_UDP, "Initializing UDP task interface\n");
  STAILQ_INIT (&udp_socket_list);

  if (itti_create_task (TASK_UDP, &udp_intertask_interface, NULL) < 0) {
    OAILOG_ERROR (LOG_UDP, "udp pthread_create (%s)\n", strerror (errno));
    return -1;
  }

  OAILOG_DEBUG (LOG_UDP, "Initializing UDP task interface: DONE\n");
  return 0;
}

void udp_exit(void) {
  struct udp_socket_desc_s               *udp_sock_p = NULL;
  while (!STAILQ_EMPTY(&udp_socket_list)) {
    udp_sock_p = STAILQ_FIRST(&udp_socket_list);
    STAILQ_REMOVE_HEAD(&udp_socket_list, entries);
    free_wrapper((void**) &udp_sock_p);
  }
}
