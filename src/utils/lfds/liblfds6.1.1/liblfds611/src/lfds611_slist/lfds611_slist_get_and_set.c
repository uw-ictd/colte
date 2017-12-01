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

#include "lfds611_slist_internal.h"/****************************************************************************/intlfds611_slist_get_user_data_from_element (  struct lfds611_slist_element *se,  void **user_data) {  int  rv = 1;  assert (se != NULL);  assert (user_data != NULL);  LFDS611_BARRIER_LOAD;  *user_data = (void *)se->user_data_and_flags[LFDS611_SLIST_USER_DATA];  if ((lfds611_atom_t) se->user_data_and_flags[LFDS611_SLIST_FLAGS] & LFDS611_SLIST_FLAG_DELETED)    rv = 0;  return (rv);}/****************************************************************************/intlfds611_slist_set_user_data_in_element (  struct lfds611_slist_element *se,  void *user_data) {  LFDS611_ALIGN (LFDS611_ALIGN_DOUBLE_POINTER) void  *user_data_and_flags[2],  *new_user_data_and_flags[2];  int  rv = 1;  assert (se != NULL);        // TRD : user_data can be NULL  LFDS611_BARRIER_LOAD;  user_data_and_flags[LFDS611_SLIST_USER_DATA] = se->user_data_and_flags[LFDS611_SLIST_USER_DATA];  user_data_and_flags[LFDS611_SLIST_FLAGS] = se->user_data_and_flags[LFDS611_SLIST_FLAGS];  new_user_data_and_flags[LFDS611_SLIST_USER_DATA] = user_data;  do {    new_user_data_and_flags[LFDS611_SLIST_FLAGS] = user_data_and_flags[LFDS611_SLIST_FLAGS];  } while (!((lfds611_atom_t) user_data_and_flags[LFDS611_SLIST_FLAGS] & LFDS611_SLIST_FLAG_DELETED) and           0 == lfds611_abstraction_dcas ((volatile lfds611_atom_t *)se->user_data_and_flags, (lfds611_atom_t *) new_user_data_and_flags, (lfds611_atom_t *) user_data_and_flags));  if ((lfds611_atom_t) user_data_and_flags[LFDS611_SLIST_FLAGS] & LFDS611_SLIST_FLAG_DELETED)    rv = 0;  LFDS611_BARRIER_STORE;  return (rv);}/****************************************************************************/struct lfds611_slist_element           *lfds611_slist_get_head (  struct lfds611_slist_state *ss,  struct lfds611_slist_element **se) {  assert (ss != NULL);  assert (se != NULL);  LFDS611_BARRIER_LOAD;  *se = (struct lfds611_slist_element *)ss->head;  lfds611_slist_internal_move_to_first_undeleted_element (se);  return (*se);}/****************************************************************************/struct lfds611_slist_element           *lfds611_slist_get_next (  struct lfds611_slist_element *se,  struct lfds611_slist_element **next_se) {  assert (se != NULL);  assert (next_se != NULL);  LFDS611_BARRIER_LOAD;  *next_se = (struct lfds611_slist_element *)se->next;  lfds611_slist_internal_move_to_first_undeleted_element (next_se);  return (*next_se);}/****************************************************************************/struct lfds611_slist_element           *lfds611_slist_get_head_and_then_next (  struct lfds611_slist_state *ss,  struct lfds611_slist_element **se) {  assert (ss != NULL);  assert (se != NULL);  if (*se == NULL)    lfds611_slist_get_head (ss, se);  else    lfds611_slist_get_next (*se, se);  return (*se);}/****************************************************************************/voidlfds611_slist_internal_move_to_first_undeleted_element (  struct lfds611_slist_element **se) {  assert (se != NULL);  while (*se != NULL and (lfds611_atom_t) (*se)->user_data_and_flags[LFDS611_SLIST_FLAGS] & LFDS611_SLIST_FLAG_DELETED)    (*se) = (struct lfds611_slist_element *)(*se)->next;  return;}
