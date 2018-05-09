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

#include "3gpp_23.003.h"
#include "sgw_config.h"
#include "pgw_lite_paa.h"
#include "db_proto.h"

database_t                             *db_desc;

int spgw_mysql_connect (const sgw_config_t * sgw_config_p)
{
  const int                               mysql_reconnect_val = 1;

  if ((sgw_config_p->mysql_server == NULL) || (sgw_config_p->mysql_user == NULL) || (sgw_config_p->mysql_password == NULL) || (sgw_config_p->mysql_database == NULL)) {
    printf( "An empty name is not allowed\n");
    return EINVAL;
  }

  printf("Initializing db layer\n");
  db_desc = malloc (sizeof (database_t));

  if (db_desc == NULL) {
    printf("An error occured on MALLOC\n");
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
    printf("An error occured while connecting to db: %s\n", mysql_error (db_desc->db_conn));
    mysql_thread_end();
    return -1;
  }

  /*
   * Set the multi statement ON
   */
  mysql_set_server_option (db_desc->db_conn, MYSQL_OPTION_MULTI_STATEMENTS_ON);
  printf("Initializing db layer: DONE\n");
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
  printf("Query: %s\n", query);
  pthread_mutex_lock (&db_desc->db_cs_mutex);

  if (mysql_query (db_desc->db_conn, query)) {
    pthread_mutex_unlock (&db_desc->db_cs_mutex);
    printf("Query execution failed: %s\n", mysql_error (db_desc->db_conn));
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

int spgw_get_ip_from_imsi (struct in_addr *ip, const char *imsi)
{
  MYSQL_RES                              *res;
  MYSQL_ROW                               row;
  char                                    query[1000];

  if (db_desc->db_conn == NULL) {
    return EINVAL;
  }

  char imsi_str[16];
  imsi_t *im = (imsi_t *)imsi;
  sprintf(imsi_str, "%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u",
    	im->u.num.digit1,im->u.num.digit2,im->u.num.digit3,im->u.num.digit4,im->u.num.digit5,
	im->u.num.digit6,im->u.num.digit7,im->u.num.digit8,im->u.num.digit9,im->u.num.digit10,
	im->u.num.digit11,im->u.num.digit12,im->u.num.digit13,im->u.num.digit14,im->u.num.digit15);

  sprintf (query, "SELECT `ip` FROM static_ips WHERE imsi=%s ", imsi_str);
  printf("Query: %s\n", query);
  pthread_mutex_lock (&db_desc->db_cs_mutex);

  if (mysql_query (db_desc->db_conn, query)) {
    pthread_mutex_unlock (&db_desc->db_cs_mutex);
    printf("Query execution failed: %s\n", mysql_error (db_desc->db_conn));
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
