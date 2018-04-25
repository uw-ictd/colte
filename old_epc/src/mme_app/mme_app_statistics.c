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

#include "intertask_interface.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_statistics.h"

int mme_app_statistics_display (
  void)
{
  OAILOG_DEBUG (LOG_MME_APP, "======================================= STATISTICS ============================================\n\n");
  OAILOG_DEBUG (LOG_MME_APP, "               |   Current Status| Added since last display|  Removed since last display |\n");
  OAILOG_DEBUG (LOG_MME_APP, "Connected eNBs | %10u      |     %10u              |    %10u               |\n",mme_app_desc.nb_enb_connected,
                                          mme_app_desc.nb_enb_connected_since_last_stat,mme_app_desc.nb_enb_released_since_last_stat);
  OAILOG_DEBUG (LOG_MME_APP, "Attached UEs   | %10u      |     %10u              |    %10u               |\n",mme_app_desc.nb_ue_attached,
                                          mme_app_desc.nb_ue_attached_since_last_stat,mme_app_desc.nb_ue_detached_since_last_stat);
  OAILOG_DEBUG (LOG_MME_APP, "Connected UEs  | %10u      |     %10u              |    %10u               |\n",mme_app_desc.nb_ue_connected,
                                          mme_app_desc.nb_ue_connected_since_last_stat,mme_app_desc.nb_ue_disconnected_since_last_stat);
  OAILOG_DEBUG (LOG_MME_APP, "Default Bearers| %10u      |     %10u              |    %10u               |\n",mme_app_desc.nb_default_eps_bearers,
                                          mme_app_desc.nb_eps_bearers_established_since_last_stat,mme_app_desc.nb_eps_bearers_released_since_last_stat);
  OAILOG_DEBUG (LOG_MME_APP, "S1-U Bearers   | %10u      |     %10u              |    %10u               |\n\n",mme_app_desc.nb_s1u_bearers,
                                          mme_app_desc.nb_s1u_bearers_established_since_last_stat,mme_app_desc.nb_s1u_bearers_released_since_last_stat);
  OAILOG_DEBUG (LOG_MME_APP, "======================================= STATISTICS ============================================\n\n");
  
  mme_stats_write_lock (&mme_app_desc);
  
  // resetting stats for next display
  mme_app_desc.nb_enb_connected_since_last_stat = 0;
  mme_app_desc.nb_enb_released_since_last_stat  = 0;
  mme_app_desc.nb_ue_connected_since_last_stat  = 0;
  mme_app_desc.nb_ue_disconnected_since_last_stat  = 0;
  mme_app_desc.nb_s1u_bearers_established_since_last_stat = 0;
  mme_app_desc.nb_s1u_bearers_released_since_last_stat = 0;
  mme_app_desc.nb_eps_bearers_established_since_last_stat = 0;
  mme_app_desc.nb_eps_bearers_released_since_last_stat = 0;
  mme_app_desc.nb_ue_attached_since_last_stat = 0;
  mme_app_desc.nb_ue_detached_since_last_stat = 0;
  
  mme_stats_unlock(&mme_app_desc);

  return 0;
}

/*********************************** Utility Functions to update Statistics**************************************/

// Number of Connected eNBs 
void update_mme_app_stats_connected_enb_add(void)
{
  mme_stats_write_lock (&mme_app_desc);
  (mme_app_desc.nb_enb_connected)++;
  (mme_app_desc.nb_enb_connected_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}
void update_mme_app_stats_connected_enb_sub(void)
{
  mme_stats_write_lock (&mme_app_desc);
  if (mme_app_desc.nb_enb_connected !=0)
    (mme_app_desc.nb_enb_connected)--;
  (mme_app_desc.nb_enb_released_since_last_stat)--;
  mme_stats_unlock(&mme_app_desc);
  return;
}

/*****************************************************/
// Number of Connected UEs
void update_mme_app_stats_connected_ue_add(void)
{
  mme_stats_write_lock (&mme_app_desc);
  (mme_app_desc.nb_ue_connected)++;
  (mme_app_desc.nb_ue_connected_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}
void update_mme_app_stats_connected_ue_sub(void)
{
  mme_stats_write_lock (&mme_app_desc);
  if (mme_app_desc.nb_ue_connected !=0)
    (mme_app_desc.nb_ue_connected)--;
  (mme_app_desc.nb_ue_disconnected_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}

/*****************************************************/
// Number of S1U Bearers 
void update_mme_app_stats_s1u_bearer_add(void)
{
  mme_stats_write_lock (&mme_app_desc);
  (mme_app_desc.nb_s1u_bearers)++;
  (mme_app_desc.nb_s1u_bearers_established_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}
void update_mme_app_stats_s1u_bearer_sub(void)
{
  mme_stats_write_lock (&mme_app_desc);
  if (mme_app_desc.nb_s1u_bearers !=0)
    (mme_app_desc.nb_s1u_bearers)--;
  (mme_app_desc.nb_s1u_bearers_released_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}

/*****************************************************/
// Number of Default EPS Bearers 
void update_mme_app_stats_default_bearer_add(void)
{
  mme_stats_write_lock (&mme_app_desc);
  (mme_app_desc.nb_default_eps_bearers)++;
  (mme_app_desc.nb_eps_bearers_established_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}
void update_mme_app_stats_default_bearer_sub(void)
{
  mme_stats_write_lock (&mme_app_desc);
  if (mme_app_desc.nb_default_eps_bearers !=0)
    (mme_app_desc.nb_default_eps_bearers)--;
  (mme_app_desc.nb_eps_bearers_released_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}

/*****************************************************/
// Number of Attached UEs 
void update_mme_app_stats_attached_ue_add(void)
{
  mme_stats_write_lock (&mme_app_desc);
  (mme_app_desc.nb_ue_attached)++;
  (mme_app_desc.nb_ue_attached_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}
void update_mme_app_stats_attached_ue_sub(void)
{
  mme_stats_write_lock (&mme_app_desc);
  if (mme_app_desc.nb_ue_attached !=0)
    (mme_app_desc.nb_ue_attached)--;
  (mme_app_desc.nb_ue_detached_since_last_stat)++;
  mme_stats_unlock(&mme_app_desc);
  return;
}
/*****************************************************/
