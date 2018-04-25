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

//////////////////////////////////////////////////////////////////////////////
////////////////////// MME/HSS/SPGW AGNOSTIC FUNCTIONS /////////////////////// 
//////////////////////////////////////////////////////////////////////////////

// this function just *SENDS* a message to the enbrains module
int enbrains_send_udp_msg (uint8_t * buffer, uint32_t buffer_len)
{
  // Create and alloc new message
  MessageDef                             *message_p;
  udp_data_req_t                         *udp_data_req_p;

  message_p = itti_alloc_new_message (TASK_ENBRAINS, UDP_DATA_REQ);
  udp_data_req_p = &message_p->ittiMsg.udp_data_req;

  // (address and port hard-coded for now)
  udp_data_req_p->peer_address = LOCALHOST_HEX;
  udp_data_req_p->peer_port = ENBRAINS_PORT;

  udp_data_req_p->buffer = buffer;
  udp_data_req_p->buffer_length = buffer_len;
  return itti_send_msg_to_task (TASK_UDP, INSTANCE_DEFAULT, message_p);
}

// (modeled after s11_mme_init)
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