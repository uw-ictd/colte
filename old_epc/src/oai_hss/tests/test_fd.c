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

#include "config.h"

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "test_utils.h"
#include "test_fd.h"

extern int                              fd_ext_add (
  char *filename,
  char *conffile);

void
s6a_fd_init (
  void)
{
  struct peer_info                        peer;

  fd_g_debug_lvl = NONE;
  memset (&peer, 0, sizeof (struct peer_info));
  peer.pi_diamid = "hss.test.fr";
  peer.pi_diamidlen = strlen (peer.pi_diamid);
  /*
   * Only SCTP
   */
  peer.config.pic_flags.pro4 = PI_P4_SCTP;
  peer.config.pic_flags.sec = PI_SEC_NONE;
  peer.config.pic_flags.exp = PI_EXP_NONE;
  peer.config.pic_port = 18678;

  if (fd_core_initialize () != 0) {
    fail ("fd_core_initialize failed");
  }

  if (fd_core_start () != 0) {
    fail ("fd_core_start failed");
  }

  if (fd_core_parseconf ("../../conf/hss_fd.conf") != 0) {
    fail ("fd_core_waitstartcomplete failed");
  }

  if (fd_core_waitstartcomplete () != 0) {
    fail ("fd_core_waitstartcomplete failed");
  }
  //     if (fd_peer_add(&peer, NULL, NULL, NULL) != 0) {
  //         fail("fd_peer_add failed");
  //     }
}

void
s6a_fd_stop (
  void)
{
  if (fd_core_shutdown () != 0) {
    fail ("fd_core_shutdown failed");
  }

  if (fd_core_wait_shutdown_complete () != 0) {
    fail ("fd_core_shutdown failed");
  }
}
