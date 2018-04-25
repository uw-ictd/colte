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

/*****************************************************************************
      Eurecom OpenAirInterface 3
      Copyright(c) 2012 Eurecom

Source    user_parser.h

Version   0.1

Date    2012/10/09

Product   User simulator

Subsystem Command line parser

Author    Frederic Maurel

Description Command line parser of the user simulator process

*****************************************************************************/

#ifndef __USER_PARSER_H__
#define __USER_PARSER__H__

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* The default remote hostname the User Simulator must connect to */
#define USER_PARSER_DEFAULT_REMOTE_HOSTNAME "localhost"

/* The default port number used for the internet service delivered by the
 * remote hostname */
#define USER_PARSER_DEFAULT_REMOTE_PORT_NUMBER  "10000"

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

void user_parser_print_usage(void);
int user_parser_get_options(int argc, const char** argv);

int user_parser_get_nb_options(void);
const char* user_parser_get_host(void);
const char* user_parser_get_port(void);
const char* user_parser_get_devpath(void);
const char* user_parser_get_devattr(void);

#endif /* __USER_PARSER_H__*/
