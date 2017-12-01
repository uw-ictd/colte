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


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <error.h>

#include <mysql/mysql.h>

#include "hss_config.h"
#include "db_proto.h"
#include "log.h"

int
hss_mysql_query_mmeidentity (
  const int id_mme_identity,
  mysql_mme_identity_t * mme_identity_p)
{
  MYSQL_RES                              *res;
  MYSQL_ROW                               row;
  char                                    query[1000];

  if ((db_desc->db_conn == NULL) || (mme_identity_p == NULL)) {
    return EINVAL;
  }

  memset (mme_identity_p, 0, sizeof (mysql_mme_identity_t));
  sprintf (query, "SELECT mmehost,mmerealm FROM mmeidentity WHERE " "mmeidentity.idmmeidentity='%d' ", id_mme_identity);
  FPRINTF_DEBUG ("Query: %s\n", query);
  pthread_mutex_lock (&db_desc->db_cs_mutex);

  if (mysql_query (db_desc->db_conn, query)) {
    pthread_mutex_unlock (&db_desc->db_cs_mutex);
    FPRINTF_ERROR ("Query execution failed: %s\n", mysql_error (db_desc->db_conn));
    return EINVAL;
  }

  res = mysql_store_result (db_desc->db_conn);
  pthread_mutex_unlock (&db_desc->db_cs_mutex);

  if ( res == NULL )
    return EINVAL;

  if ((row = mysql_fetch_row (res)) != NULL) {
    if (row[0] != NULL) {
      memcpy (mme_identity_p->mme_host, row[0], strlen (row[0]));
    } else {
      mme_identity_p->mme_host[0] = '\0';
    }

    if (row[1] != NULL) {
      memcpy (mme_identity_p->mme_realm, row[1], strlen (row[1]));
    } else {
      mme_identity_p->mme_realm[0] = '\0';
    }

    mysql_free_result (res);
    return 0;
  }

  mysql_free_result (res);
  return EINVAL;
}

int
hss_mysql_check_epc_equipment (
  mysql_mme_identity_t * mme_identity_p)
{
  MYSQL_RES                              *res;
  MYSQL_ROW                               row;
  char                                    query[1000];

  if ((db_desc->db_conn == NULL) || (mme_identity_p == NULL)) {
    return EINVAL;
  }

  sprintf (query, "SELECT idmmeidentity FROM mmeidentity WHERE mmeidentity.mmehost='%s' ", mme_identity_p->mme_host);
  FPRINTF_DEBUG ("Query: %s\n", query);
  pthread_mutex_lock (&db_desc->db_cs_mutex);

  if (mysql_query (db_desc->db_conn, query)) {
    pthread_mutex_unlock (&db_desc->db_cs_mutex);
    FPRINTF_ERROR ("Query execution failed: %s\n", mysql_error (db_desc->db_conn));
    return EINVAL;
  }

  res = mysql_store_result (db_desc->db_conn);
  pthread_mutex_unlock (&db_desc->db_cs_mutex);

  if ( res == NULL )
    return EINVAL;

  if ((row = mysql_fetch_row (res)) != NULL) {
    mysql_free_result (res);
    return 0;
  }

  mysql_free_result (res);
  return EINVAL;
}
