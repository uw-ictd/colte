#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <inttypes.h>

#include <mysql/mysql.h>

#include "sgw_config.h"
#include "db_proto.h"
#include "log.h"
#include "s6a_proto.h"

database_t                             *db_desc;

int spgw_mysql_connect (const sgw_config_t * sgw_config_p)
{
  const int                               mysql_reconnect_val = 1;

  if ((sgw_config_p->mysql_server == NULL) || (sgw_config_p->mysql_user == NULL) || (sgw_config_p->mysql_password == NULL) || (sgw_config_p->mysql_database == NULL)) {
    FPRINTF_ERROR ( "An empty name is not allowed\n");
    return EINVAL;
  }

  FPRINTF_DEBUG ("Initializing db layer\n");
  db_desc = malloc (sizeof (database_t));

  if (db_desc == NULL) {
    FPRINTF_DEBUG ("An error occured on MALLOC\n");
    return errno;
  }

  pthread_mutex_init (&db_desc->db_cs_mutex, NULL);
  /*
   * Copy database configuration from static sgw config
   */
  db_desc->server = strdup (sgw_config_p->mysql_server);
  db_desc->user = strdup (sgw_config_p->mysql_user);
  db_desc->password = strdup (sgw_config_p->mysql_password);
  db_desc->database = strdup (sgw_config_p->mysql_database);
  /*
   * Init mySQL client
   */
  db_desc->db_conn = mysql_init (NULL);
  mysql_options (db_desc->db_conn, MYSQL_OPT_RECONNECT, &mysql_reconnect_val);

  /*
   * Try to connect to database
   */
  if (!mysql_real_connect (db_desc->db_conn, db_desc->server, db_desc->user, db_desc->password, db_desc->database, 0, NULL, 0)) {
    FPRINTF_ERROR ("An error occured while connecting to db: %s\n", mysql_error (db_desc->db_conn));
    mysql_thread_end();
    return -1;
  }

  /*
   * Set the multi statement ON
   */
  mysql_set_server_option (db_desc->db_conn, MYSQL_OPTION_MULTI_STATEMENTS_ON);
  FPRINTF_DEBUG ("Initializing db layer: DONE\n");
  return 0;
}

void spgw_mysql_disconnect (void)
{
  mysql_close (db_desc->db_conn);
  mysql_thread_end();
}

int spgw_get_imsi_from_ip (struct in_addr *ip, char *imsi)
{
  MYSQL_RES                              *res;
  MYSQL_ROW                               row;
  char                                    query[1000];

  if (db_desc->db_conn == NULL) {
    return EINVAL;
  }

  char ip_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, ip, ip_str, INET_ADDRSTRLEN);

  sprintf (query, "SELECT `imsi` FROM static_ips WHERE ip=%s ", ip_str);
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
    strncpy(imsi, row[0], 16);
    return 0;
  }

  mysql_free_result (res);
  return EINVAL;
}

int spgw_get_ip_from_imsi (struct in_addr *ip, char *imsi)
{
  MYSQL_RES                              *res;
  MYSQL_ROW                               row;
  char                                    query[1000];

  if (db_desc->db_conn == NULL) {
    return EINVAL;
  }

  sprintf (query, "SELECT `ip` FROM static_ips WHERE imsi=%s ", imsi);
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
  	char ip_str[INET_ADDRSTRLEN];
    strncpy(ip_str, row[0], INET_ADDRSTRLEN);
    inet_pton(AF_INET, ip_str, ip);
    return 0;
  }

  mysql_free_result (res);
  return EINVAL;
}