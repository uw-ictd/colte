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

#include "lfds611_stack_internal.h"/****************************************************************************/intlfds611_stack_push (  struct lfds611_stack_state *ss,  void *user_data) {  LFDS611_ALIGN (LFDS611_ALIGN_DOUBLE_POINTER) struct lfds611_stack_element    *se[LFDS611_STACK_PAC_SIZE];  assert (ss != NULL);    // TRD : user_data can be NULL  lfds611_stack_internal_new_element_from_freelist (ss, se, user_data);  if (se[LFDS611_STACK_POINTER] == NULL)    return (0);  lfds611_stack_internal_push (ss, se);  return (1);}/****************************************************************************/intlfds611_stack_guaranteed_push (  struct lfds611_stack_state *ss,  void *user_data) {  LFDS611_ALIGN (LFDS611_ALIGN_DOUBLE_POINTER) struct lfds611_stack_element    *se[LFDS611_STACK_PAC_SIZE];  assert (ss != NULL);  // TRD : user_data can be NULL  /*     TRD : this function allocated a new lfds611_freelist element and uses that     to push onto the lfds611_stack, guaranteeing success (unless malloc()     fails of course)  */  lfds611_stack_internal_new_element (ss, se, user_data);  // TRD : malloc failed  if (se[LFDS611_STACK_POINTER] == NULL)    return (0);  lfds611_stack_internal_push (ss, se);  return (1);}/****************************************************************************/voidlfds611_stack_internal_push (  struct lfds611_stack_state *ss,  struct lfds611_stack_element *se[LFDS611_STACK_PAC_SIZE]) {  LFDS611_ALIGN (LFDS611_ALIGN_DOUBLE_POINTER) struct lfds611_stack_element    *original_se_next[LFDS611_STACK_PAC_SIZE];  assert (ss != NULL);  assert (se != NULL);  LFDS611_BARRIER_LOAD;  original_se_next[LFDS611_STACK_POINTER] = ss->top[LFDS611_STACK_POINTER];  original_se_next[LFDS611_STACK_COUNTER] = ss->top[LFDS611_STACK_COUNTER];  do {    se[LFDS611_STACK_POINTER]->next[LFDS611_STACK_POINTER] = original_se_next[LFDS611_STACK_POINTER];    se[LFDS611_STACK_POINTER]->next[LFDS611_STACK_COUNTER] = original_se_next[LFDS611_STACK_COUNTER];  } while (0 == lfds611_abstraction_dcas ((volatile lfds611_atom_t *)ss->top, (lfds611_atom_t *) se, (lfds611_atom_t *) original_se_next));  return;}/****************************************************************************/intlfds611_stack_pop (  struct lfds611_stack_state *ss,  void **user_data) {  LFDS611_ALIGN (LFDS611_ALIGN_DOUBLE_POINTER) struct lfds611_stack_element    *se[LFDS611_STACK_PAC_SIZE];  assert (ss != NULL);  assert (user_data != NULL);  LFDS611_BARRIER_LOAD;  se[LFDS611_STACK_COUNTER] = ss->top[LFDS611_STACK_COUNTER];  se[LFDS611_STACK_POINTER] = ss->top[LFDS611_STACK_POINTER];  do {    if (se[LFDS611_STACK_POINTER] == NULL)      return (0);  } while (0 == lfds611_abstraction_dcas ((volatile lfds611_atom_t *)ss->top, (lfds611_atom_t *) se[LFDS611_STACK_POINTER]->next, (lfds611_atom_t *) se));  *user_data = se[LFDS611_STACK_POINTER]->user_data;  lfds611_freelist_push (ss->fs, se[LFDS611_STACK_POINTER]->fe);  return (1);}
