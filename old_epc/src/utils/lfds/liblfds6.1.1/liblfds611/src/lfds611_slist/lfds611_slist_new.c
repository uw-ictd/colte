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

#include "lfds611_slist_internal.h"/****************************************************************************/intlfds611_slist_new (  struct lfds611_slist_state **ss,  void                                    (*user_data_delete_function) (void *user_data,      void *user_state),  void *user_state) {  int  rv = 0;  assert (ss != NULL);      // TRD : user_data_delete_function can be NULL  // TRD : user_state can be NULL  *ss = (struct lfds611_slist_state *)lfds611_liblfds_aligned_malloc (sizeof (struct lfds611_slist_state), LFDS611_ALIGN_SINGLE_POINTER);  if (*ss != NULL) {    lfds611_slist_internal_init_slist (*ss, user_data_delete_function, user_state);    rv = 1;  }  LFDS611_BARRIER_STORE;  return (rv);}/****************************************************************************///#pragma warning( disable : 4100 )voidlfds611_slist_use (  struct lfds611_slist_state *ss) {  assert (ss != NULL);  LFDS611_BARRIER_LOAD;  return;}//#pragma warning( default : 4100 )/****************************************************************************/voidlfds611_slist_internal_init_slist (  struct lfds611_slist_state *ss,  void                                    (*user_data_delete_function) (void *user_data,      void *user_state),  void *user_state) {  assert (ss != NULL);  // TRD : user_data_delete_function can be NULL  // TRD : user_state can be NULL  ss->head = NULL;  ss->user_data_delete_function = user_data_delete_function;  ss->user_state = user_state;  return;}/****************************************************************************/struct lfds611_slist_element           *lfds611_slist_new_head (  struct lfds611_slist_state *ss,  void *user_data) {  LFDS611_ALIGN (LFDS611_ALIGN_SINGLE_POINTER) struct lfds611_slist_element    *volatile se;  assert (ss != NULL);  // TRD : user_data can be NULL  se = (struct lfds611_slist_element *)lfds611_liblfds_aligned_malloc (sizeof (struct lfds611_slist_element), LFDS611_ALIGN_DOUBLE_POINTER);  if (se != NULL) {    se->user_data_and_flags[LFDS611_SLIST_USER_DATA] = user_data;    se->user_data_and_flags[LFDS611_SLIST_FLAGS] = LFDS611_SLIST_NO_FLAGS;    lfds611_slist_internal_link_element_to_head (ss, se);  }  return ((struct lfds611_slist_element *)se);}/****************************************************************************/struct lfds611_slist_element           *lfds611_slist_new_next (  struct lfds611_slist_element *se,  void *user_data) {  LFDS611_ALIGN (LFDS611_ALIGN_SINGLE_POINTER) struct lfds611_slist_element    *volatile se_next;  assert (se != NULL);  // TRD : user_data can be NULL  se_next = (struct lfds611_slist_element *)lfds611_liblfds_aligned_malloc (sizeof (struct lfds611_slist_element), LFDS611_ALIGN_DOUBLE_POINTER);  if (se_next != NULL) {    se_next->user_data_and_flags[LFDS611_SLIST_USER_DATA] = user_data;    se_next->user_data_and_flags[LFDS611_SLIST_FLAGS] = LFDS611_SLIST_NO_FLAGS;    lfds611_slist_internal_link_element_after_element (se, se_next);  }  return ((struct lfds611_slist_element *)se_next);}
