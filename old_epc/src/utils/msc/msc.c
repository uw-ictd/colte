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

/*! \file msc.c
   \brief Message chart generator logging utility (generated files to processed by a script to produce a mscgen input file for generating a sequence diagram document)
   \author  Lionel GAUTHIER
   \date 2015
   \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>

#include "liblfds611.h"
#include "intertask_interface.h"
#include "timer.h"

#include "msc.h"
#include "assertions.h"
#include "dynamic_memory_check.h"
#include "log.h"

//-------------------------------
#define MSC_MAX_QUEUE_ELEMENTS    1024
#define MSC_MAX_PROTO_NAME_LENGTH 16
#define MSC_MAX_MESSAGE_LENGTH    512

//-------------------------------

FILE                                   *g_msc_fd = NULL;
char                                    g_msc_proto2str[MAX_MSC_PROTOS][MSC_MAX_PROTO_NAME_LENGTH];
int                                     g_msc_start_time_second = 0;



typedef unsigned long                   msc_message_number_t;
typedef struct msc_queue_item_s {
  char                                   *message_str;
  uint32_t                                message_str_size;
  uint8_t                                *message_bin;
  uint32_t                                message_bin_size;
} msc_queue_item_t;

msc_message_number_t                    g_message_number = 0;
struct lfds611_queue_state             *g_msc_message_queue_p = NULL;
struct lfds611_stack_state             *g_msc_memory_stack_p = NULL;

//------------------------------------------------------------------------------
void *msc_task (void *args_p)
{
  MessageDef                             *received_message_p = NULL;
  long                                    timer_id = 0;
  int                                     rc = 0;

  itti_mark_task_ready (TASK_MSC);
  msc_start_use ();
  timer_setup (0,               // seconds
               50000,           // usec
               TASK_MSC, INSTANCE_DEFAULT, TIMER_PERIODIC, NULL, &timer_id);

  while (1) {
    itti_receive_msg (TASK_MSC, &received_message_p);

    if (received_message_p != NULL) {

      switch (ITTI_MSG_ID (received_message_p)) {
      case TIMER_HAS_EXPIRED:{
          msc_flush_messages ();
        }
        break;

      case TERMINATE_MESSAGE:{
          timer_remove (timer_id);
          msc_end ();
          itti_exit_task ();
        }
        break;

      case MESSAGE_TEST:{
        }
        break;

      default:{
        }
        break;
      }
      // Freeing the memory allocated from the memory pool
      rc = itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
      AssertFatal (rc == EXIT_SUCCESS, "Failed to free memory (%d)!\n", rc);
      received_message_p = NULL;
    }
  }

  fprintf (stderr, "Task MSC exiting\n");
  return NULL;
}


//------------------------------------------------------------------------------
static void msc_get_elapsed_time_since_start(struct timeval * const elapsed_time)
{
  // no thread safe but do not matter a lot
  gettimeofday(elapsed_time, NULL);
  // no timersub call for fastest operations
  elapsed_time->tv_sec = elapsed_time->tv_sec - g_msc_start_time_second;
}

//------------------------------------------------------------------------------
int
msc_init (
  const msc_env_t envP,
  const int max_threadsP)
{
  int                                     i = 0;
  int                                     rv = 0;
  void                                   *pointer_p = NULL;
  char                                    msc_filename[256];

#if LOG_OAI
  // take same time reference as log
  g_msc_start_time_second = log_get_start_time_sec();
#else
  struct timeval                          start_time = {.tv_sec=0, .tv_usec=0};
  rv = gettimeofday(&start_time, NULL);
  g_msc_start_time_second = start_time.tv_sec;
#endif

  fprintf (stderr, "Initializing MSC logs\n");
  rv = snprintf (msc_filename, 256, "/tmp/openair.msc.%u.log", envP);   // TODO NAME

  if ((0 >= rv) || (256 < rv)) {
    fprintf (stderr, "Error in MSC log file name");
  }

  g_msc_fd = fopen (msc_filename, "w");
  AssertFatal (g_msc_fd != NULL, "Could not open MSC log file %s : %s", msc_filename, strerror (errno));
  rv = lfds611_stack_new (&g_msc_memory_stack_p, (lfds611_atom_t) max_threadsP + 2);

  if (0 >= rv) {
    AssertFatal (0, "lfds611_stack_new failed!\n");
  }

  rv = lfds611_queue_new (&g_msc_message_queue_p, (lfds611_atom_t) MSC_MAX_QUEUE_ELEMENTS);
  AssertFatal (rv, "lfds611_queue_new failed!\n");
  AssertFatal (g_msc_message_queue_p != NULL, "g_msc_message_queue_p is NULL!\n");
  msc_start_use ();

  for (i = 0; i < max_threadsP * 30; i++) {
    pointer_p = malloc (MSC_MAX_MESSAGE_LENGTH);
    AssertFatal (pointer_p, "malloc failed!\n");
    rv = lfds611_stack_guaranteed_push (g_msc_memory_stack_p, pointer_p);
    AssertFatal (rv, "lfds611_stack_guaranteed_push failed for item %u\n", i);
  }

  for (i = MIN_MSC_PROTOS; i < MAX_MSC_PROTOS; i++) {
    switch (i) {
    case MSC_NAS_UE:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_UE");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      break;


    case MSC_S1AP_ENB:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "S1AP_ENB");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_GTPU_ENB:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "GTPU_ENB");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }
      break;

    case MSC_GTPU_SGW:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "GTPU_SGW");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_SP_GW)) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_S1AP_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "S1AP_MME");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_MMEAPP_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "MME_APP");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_NAS_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_MME");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME) || (envP == MSC_E_UTRAN)) {
        msc_log_declare_proto (i);
      }
      break;

    case MSC_NAS_EMM_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_EMM");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_NAS_ESM_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_ESM");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_SP_GWAPP_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "SP_GW_MME");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if (envP == MSC_MME_GW) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_S11_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "S11_MME");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if (envP == MSC_MME) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_S6A_MME:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "S6A");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_SGW:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "SGW");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if (envP == MSC_MME) {
        msc_log_declare_proto (i);
      }

      break;

    case MSC_HSS:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "HSS");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }

      if ((envP == MSC_MME_GW) || (envP == MSC_MME)) {
        msc_log_declare_proto (i);
      }

      break;

    default:
      rv = snprintf (&g_msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "UNKNOWN");

      if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {
        g_msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH - 1] = 0;
      }
    }
  }

  rv = itti_create_task (TASK_MSC, msc_task, NULL);
  AssertFatal (rv == 0, "Create task for MSC failed!\n");
  fprintf (stderr, "Initializing MSC logs Done\n");
  return 0;
}

//------------------------------------------------------------------------------
void msc_start_use (void)
{
  lfds611_queue_use (g_msc_message_queue_p);
  lfds611_stack_use (g_msc_memory_stack_p);
}


//------------------------------------------------------------------------------
void msc_flush_messages (void)
{
  int                                     rv = 0;
  msc_queue_item_t                       *item_p = NULL;

  while ((rv = lfds611_queue_dequeue (g_msc_message_queue_p, (void **)&item_p)) == 1) {
    if (NULL != item_p->message_str) {
      fputs (item_p->message_str, g_msc_fd);
      // TODO BIN DATA
      rv = lfds611_stack_guaranteed_push (g_msc_memory_stack_p, item_p->message_str);
    }
    // TODO free_wrapper BIN DATA
    free_wrapper ((void**) &item_p);
  }

  fflush (g_msc_fd);
}


//------------------------------------------------------------------------------
void msc_end (void)
{
  int                                     rv = 0;

  if (NULL != g_msc_fd) {
    msc_flush_messages ();
    rv = fflush (g_msc_fd);

    if (rv != 0) {
      fprintf (stderr, "Error while flushing stream of MSC log file: %s", strerror (errno));
    }

    rv = fclose (g_msc_fd);

    if (rv != 0) {
      fprintf (stderr, "Error while closing MSC log file: %s", strerror (errno));
    }
  }
}

//------------------------------------------------------------------------------
void msc_log_declare_proto (const msc_proto_t protoP)
{
  int                                     rv = 0;
  msc_queue_item_t                       *new_item_p = NULL;
  char                                   *char_message_p = NULL;

  if ((MIN_MSC_PROTOS <= protoP) && (MAX_MSC_PROTOS > protoP)) {
    // may be build a memory pool for that also ?
    new_item_p = malloc (sizeof (msc_queue_item_t));

    if (NULL != new_item_p) {
      rv = lfds611_stack_pop (g_msc_memory_stack_p, (void **)&char_message_p);

      if (0 == rv) {
        msc_flush_messages ();
        rv = lfds611_stack_pop (g_msc_memory_stack_p, (void **)&char_message_p);
      }

      if (1 == rv) {
        rv = snprintf (char_message_p, MSC_MAX_MESSAGE_LENGTH, "%" PRIu64 " [PROTO] %d %s\n", __sync_fetch_and_add (&g_message_number, 1), protoP, &g_msc_proto2str[protoP][0]);

        if (0 > rv) {
          fprintf (stderr, "Error while declaring new protocol in MSC: %s", strerror (errno));
        }

        new_item_p->message_str = char_message_p;
        new_item_p->message_str_size = rv;
        new_item_p->message_bin = NULL;
        new_item_p->message_bin_size = 0;
        rv = lfds611_queue_enqueue (g_msc_message_queue_p, new_item_p);

        if (0 == rv) {
          rv = lfds611_queue_guaranteed_enqueue (g_msc_message_queue_p, new_item_p);

          if (0 == rv) {
            fprintf (stderr, "Error while lfds611_queue_guaranteed_enqueue message %s in MSC", char_message_p);
            rv = lfds611_stack_guaranteed_push (g_msc_memory_stack_p, char_message_p);
            free_wrapper ((void**) &new_item_p);
          }
        }

        return;
      } else {
        fprintf (stderr, "Error while lfds611_stack_pop()\n");
      }

      free_wrapper ((void**) &new_item_p);
    } else {
      fprintf (stderr, "Error while malloc in MSC");
    }
  }
}

//------------------------------------------------------------------------------
void msc_log_event (const msc_proto_t protoP, char *format, ...)
{
  va_list                                 args;
  int                                     rv = 0;
  int                                     rv2 = 0;
  msc_queue_item_t                       *new_item_p = NULL;
  char                                   *char_message_p = NULL;

  if ((MIN_MSC_PROTOS > protoP) || (MAX_MSC_PROTOS <= protoP)) {
    return;
  }

  new_item_p = malloc (sizeof (msc_queue_item_t));

  if (NULL != new_item_p) {
    rv = lfds611_stack_pop (g_msc_memory_stack_p, (void **)&char_message_p);

    if (0 == rv) {
      msc_flush_messages ();
      rv = lfds611_stack_pop (g_msc_memory_stack_p, (void **)&char_message_p);
    }

    if (1 == rv) {
      struct timeval elapsed_time;
      msc_get_elapsed_time_since_start(&elapsed_time);
      rv = snprintf (char_message_p, MSC_MAX_MESSAGE_LENGTH, "%" PRIu64 " [EVENT] %d %04ld:%06ld",
          __sync_fetch_and_add (&g_message_number, 1), protoP, elapsed_time.tv_sec, elapsed_time.tv_usec);

      if ((0 > rv) || (MSC_MAX_MESSAGE_LENGTH < rv)) {
        fprintf (stderr, "Error while logging MSC event : %s", &g_msc_proto2str[protoP][0]);
        goto error_event;
      }

      va_start (args, format);
      rv2 = vsnprintf (&char_message_p[rv], MSC_MAX_MESSAGE_LENGTH - rv, format, args);
      va_end (args);

      if ((0 > rv2) || ((MSC_MAX_MESSAGE_LENGTH - rv) < rv2)) {
        fprintf (stderr, "Error while logging MSC event : %s", &g_msc_proto2str[protoP][0]);
        goto error_event;
      }

      rv += rv2;
      rv2 = snprintf (&char_message_p[rv], MSC_MAX_MESSAGE_LENGTH - rv, "\n");

      if ((0 > rv2) || ((MSC_MAX_MESSAGE_LENGTH - rv) < rv2)) {
        fprintf (stderr, "Error while logging MSC event : %s", &g_msc_proto2str[protoP][0]);
        goto error_event;
      }

      rv += rv2;
      new_item_p->message_str = char_message_p;
      new_item_p->message_str_size = rv;
      new_item_p->message_bin = NULL;
      new_item_p->message_bin_size = 0;
      rv = lfds611_queue_enqueue (g_msc_message_queue_p, new_item_p);

      if (0 == rv) {
        fprintf (stderr, "Error while lfds611_queue_guaranteed_enqueue message %s in MSC", char_message_p);
        rv = lfds611_stack_guaranteed_push (g_msc_memory_stack_p, char_message_p);
        free_wrapper ((void**) &new_item_p);
      }
    } else {
      free_wrapper ((void**) &new_item_p);
      fprintf (stderr, "Error while lfds611_stack_pop()\n");
    }
  }

  return;
error_event:
  rv = lfds611_stack_guaranteed_push (g_msc_memory_stack_p, char_message_p);
  free_wrapper ((void**) &new_item_p);
}

//------------------------------------------------------------------------------
void
msc_log_message (
  const char *const message_operationP,
  const msc_proto_t proto1P,
  const msc_proto_t proto2P,
  const uint8_t * const bytesP,
  const unsigned int num_bytes,
  char *format,
  ...)
{
  va_list                                 args;
  uint64_t                                mac = 0;      // TO DO mac on bytesP param
  int                                     rv = 0;
  int                                     rv2 = 0;
  msc_queue_item_t                       *new_item_p = NULL;
  char                                   *char_message_p = NULL;

  if ((MIN_MSC_PROTOS > proto1P) || (MAX_MSC_PROTOS <= proto1P) || (MIN_MSC_PROTOS > proto2P) || (MAX_MSC_PROTOS <= proto2P)) {
    return;
  }

  new_item_p = malloc (sizeof (msc_queue_item_t));

  if (NULL != new_item_p) {
    rv = lfds611_stack_pop (g_msc_memory_stack_p, (void **)&char_message_p);

    if (0 == rv) {
      msc_flush_messages ();
      rv = lfds611_stack_pop (g_msc_memory_stack_p, (void **)&char_message_p);
    }

    if (1 == rv) {
      struct timeval elapsed_time;
      msc_get_elapsed_time_since_start(&elapsed_time);
      rv = snprintf (char_message_p, MSC_MAX_MESSAGE_LENGTH, "%" PRIu64 " [MESSAGE] %d %s %d %" PRIu64 " %04ld:%06ld",
          __sync_fetch_and_add (&g_message_number, 1), proto1P, message_operationP, proto2P, mac, elapsed_time.tv_sec, elapsed_time.tv_usec);

      if ((0 > rv) || (MSC_MAX_MESSAGE_LENGTH < rv)) {
        AssertFatal (0, "Error while logging MSC message : %s/%s", &g_msc_proto2str[proto1P][0], &g_msc_proto2str[proto2P][0]);
        goto error_event;
      }

      va_start (args, format);
      rv2 = vsnprintf (&char_message_p[rv], MSC_MAX_MESSAGE_LENGTH - rv, format, args);
      va_end (args);

      if ((0 > rv2) || ((MSC_MAX_MESSAGE_LENGTH - rv) < rv2)) {
        AssertFatal (0, "Error while logging MSC message : %s/%s", &g_msc_proto2str[proto1P][0], &g_msc_proto2str[proto2P][0]);
        goto error_event;
      }

      rv += rv2;
      rv2 = snprintf (&char_message_p[rv], MSC_MAX_MESSAGE_LENGTH - rv, "\n");

      if ((0 > rv2) || ((MSC_MAX_MESSAGE_LENGTH - rv) < rv2)) {
        AssertFatal (0, "Error while logging MSC message : %s/%s", &g_msc_proto2str[proto1P][0], &g_msc_proto2str[proto2P][0]);
        goto error_event;
      }

      rv += rv2;
      new_item_p->message_str = char_message_p;
      new_item_p->message_str_size = rv;
      new_item_p->message_bin = NULL;   // TO DO
      new_item_p->message_bin_size = 0; // TO DO
      rv = lfds611_queue_enqueue (g_msc_message_queue_p, new_item_p);

      if (0 == rv) {
        fprintf (stderr, "Error while lfds611_queue_guaranteed_enqueue message %s in MSC", char_message_p);
        rv = lfds611_stack_guaranteed_push (g_msc_memory_stack_p, char_message_p);
        free_wrapper ((void**) &new_item_p);
      }
    } else {
      free_wrapper ((void**) &new_item_p);
      fprintf (stderr, "Error while lfds611_stack_pop()\n");
      msc_flush_messages ();
    }
  }

  return;
error_event:
  rv = lfds611_stack_guaranteed_push (g_msc_memory_stack_p, char_message_p);
  free_wrapper ((void**) &new_item_p);
}
