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

#include "lfds611_slist_internal.h"/****************************************************************************/voidlfds611_slist_delete (  struct lfds611_slist_state *ss) {  lfds611_slist_single_threaded_physically_delete_all_elements (ss);  lfds611_liblfds_aligned_free (ss);  return;}/****************************************************************************/intlfds611_slist_logically_delete_element (  struct lfds611_slist_state *ss,  struct lfds611_slist_element *se) {  LFDS611_ALIGN (LFDS611_ALIGN_DOUBLE_POINTER) void  *volatile user_data_and_flags[2],  *volatile new_user_data_and_flags[2];  unsigned char  cas_rv = 0;  assert (ss != NULL);  assert (se != NULL);  LFDS611_BARRIER_LOAD;  user_data_and_flags[LFDS611_SLIST_USER_DATA] = se->user_data_and_flags[LFDS611_SLIST_USER_DATA];  user_data_and_flags[LFDS611_SLIST_FLAGS] = se->user_data_and_flags[LFDS611_SLIST_FLAGS];  do {    new_user_data_and_flags[LFDS611_SLIST_USER_DATA] = user_data_and_flags[LFDS611_SLIST_USER_DATA];    new_user_data_and_flags[LFDS611_SLIST_FLAGS] = (void *)((lfds611_atom_t) user_data_and_flags[LFDS611_SLIST_FLAGS] | LFDS611_SLIST_FLAG_DELETED);  } while (!((lfds611_atom_t) user_data_and_flags[LFDS611_SLIST_FLAGS] & LFDS611_SLIST_FLAG_DELETED)           and 0 == (cas_rv = lfds611_abstraction_dcas ((volatile lfds611_atom_t *)se->user_data_and_flags, (lfds611_atom_t *) new_user_data_and_flags, (lfds611_atom_t *) user_data_and_flags)));  if (cas_rv == 1)    if (ss->user_data_delete_function != NULL)      ss->user_data_delete_function ((void *)user_data_and_flags[LFDS611_SLIST_USER_DATA], ss->user_state);  return (cas_rv);}/****************************************************************************/voidlfds611_slist_single_threaded_physically_delete_all_elements (  struct lfds611_slist_state *ss) {  struct lfds611_slist_element    *volatile se,    *volatile se_temp;  LFDS611_BARRIER_LOAD;  se = ss->head;  while (se != NULL) { // TRD : if a non-deleted element and there is a delete function, call the delete function    if (ss->user_data_delete_function != NULL)      ss->user_data_delete_function ((void *)se->user_data_and_flags[LFDS611_SLIST_USER_DATA], ss->user_state);    se_temp = se;    se = se->next;    lfds611_liblfds_aligned_free ((void *)se_temp);  }  lfds611_slist_internal_init_slist (ss, ss->user_data_delete_function, ss->user_state);  LFDS611_BARRIER_STORE;  return;}
