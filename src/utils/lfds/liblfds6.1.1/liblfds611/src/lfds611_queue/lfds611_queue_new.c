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

#include "lfds611_queue_internal.h"/****************************************************************************/intlfds611_queue_new (  struct lfds611_queue_state **qs,  lfds611_atom_t number_elements) {  int  rv = 0;  struct lfds611_queue_element    *qe[LFDS611_QUEUE_PAC_SIZE];  assert (qs != NULL);  // TRD : number_elements can be any value in its range  *qs = (struct lfds611_queue_state *)lfds611_liblfds_aligned_malloc (sizeof (struct lfds611_queue_state), LFDS611_ALIGN_DOUBLE_POINTER);  if (*qs != NULL) {    // TRD : the size of the lfds611_freelist is the size of the lfds611_queue (+1 for the leading dummy element, which is hidden from the caller)    lfds611_freelist_new (&(*qs)->fs, number_elements + 1, lfds611_queue_internal_freelist_init_function, NULL);    if ((*qs)->fs != NULL) {      lfds611_queue_internal_new_element_from_freelist (*qs, qe, NULL);      (*qs)->enqueue[LFDS611_QUEUE_POINTER] = (*qs)->dequeue[LFDS611_QUEUE_POINTER] = qe[LFDS611_QUEUE_POINTER];      (*qs)->enqueue[LFDS611_QUEUE_COUNTER] = (*qs)->dequeue[LFDS611_QUEUE_COUNTER] = 0;      (*qs)->aba_counter = 0;      rv = 1;    }    if ((*qs)->fs == NULL) {      lfds611_liblfds_aligned_free (*qs);      *qs = NULL;    }  }  LFDS611_BARRIER_STORE;  return (rv);}/****************************************************************************///#pragma warning( disable : 4100 )voidlfds611_queue_use (  struct lfds611_queue_state *qs) {  assert (qs != NULL);  LFDS611_BARRIER_LOAD;  return;}//#pragma warning( default : 4100 )/****************************************************************************///#pragma warning( disable : 4100 )intlfds611_queue_internal_freelist_init_function (  void **user_data,  void *user_state) {  int  rv = 0;  assert (user_data != NULL);  assert (user_state == NULL);  *user_data = lfds611_liblfds_aligned_malloc (sizeof (struct lfds611_queue_element), LFDS611_ALIGN_DOUBLE_POINTER);  if (*user_data != NULL)    rv = 1;  return (rv);}//#pragma warning( default : 4100 )/****************************************************************************/voidlfds611_queue_internal_new_element_from_freelist (  struct lfds611_queue_state *qs,  struct lfds611_queue_element *qe[LFDS611_QUEUE_PAC_SIZE],  void *user_data) {  struct lfds611_freelist_element    *fe;  assert (qs != NULL);  assert (qe != NULL);  // TRD : user_data can be any value in its range  qe[LFDS611_QUEUE_POINTER] = NULL;  lfds611_freelist_pop (qs->fs, &fe);  if (fe != NULL)    lfds611_queue_internal_init_element (qs, qe, fe, user_data);  return;}/****************************************************************************/voidlfds611_queue_internal_guaranteed_new_element_from_freelist (  struct lfds611_queue_state *qs,  struct lfds611_queue_element *qe[LFDS611_QUEUE_PAC_SIZE],  void *user_data) {  struct lfds611_freelist_element    *fe;  assert (qs != NULL);  assert (qe != NULL);  // TRD : user_data can be any value in its range  qe[LFDS611_QUEUE_POINTER] = NULL;  lfds611_freelist_guaranteed_pop (qs->fs, &fe);  if (fe != NULL)    lfds611_queue_internal_init_element (qs, qe, fe, user_data);  return;}/****************************************************************************/voidlfds611_queue_internal_init_element (  struct lfds611_queue_state *qs,  struct lfds611_queue_element *qe[LFDS611_QUEUE_PAC_SIZE],  struct lfds611_freelist_element *fe,  void *user_data) {  assert (qs != NULL);  assert (qe != NULL);  assert (fe != NULL);  // TRD : user_data can be any value in its range  lfds611_freelist_get_user_data_from_element (fe, (void **)&qe[LFDS611_QUEUE_POINTER]);  qe[LFDS611_QUEUE_COUNTER] = (struct lfds611_queue_element *)lfds611_abstraction_increment ((lfds611_atom_t *) & qs->aba_counter);  qe[LFDS611_QUEUE_POINTER]->next[LFDS611_QUEUE_POINTER] = NULL;  qe[LFDS611_QUEUE_POINTER]->next[LFDS611_QUEUE_COUNTER] = (struct lfds611_queue_element *)lfds611_abstraction_increment ((lfds611_atom_t *) & qs->aba_counter);  qe[LFDS611_QUEUE_POINTER]->fe = fe;  qe[LFDS611_QUEUE_POINTER]->user_data = user_data;  return;}
