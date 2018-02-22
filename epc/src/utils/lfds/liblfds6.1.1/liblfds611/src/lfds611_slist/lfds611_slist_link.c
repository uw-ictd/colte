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

#include "lfds611_slist_internal.h"/****************************************************************************/voidlfds611_slist_internal_link_element_to_head (  struct lfds611_slist_state *ss,  struct lfds611_slist_element *volatile se) {  LFDS611_ALIGN (LFDS611_ALIGN_SINGLE_POINTER) struct lfds611_slist_element    *se_next;  assert (ss != NULL);  assert (se != NULL);  LFDS611_BARRIER_LOAD;  se_next = ss->head;  do {    se->next = se_next;  } while (se->next != (se_next = (struct lfds611_slist_element *)lfds611_abstraction_cas ((volatile lfds611_atom_t *)&ss->head, (lfds611_atom_t) se, (lfds611_atom_t) se->next)));  return;}/****************************************************************************/voidlfds611_slist_internal_link_element_after_element (  struct lfds611_slist_element *volatile lfds611_slist_in_list_element,  struct lfds611_slist_element *volatile se) {  LFDS611_ALIGN (LFDS611_ALIGN_SINGLE_POINTER) struct lfds611_slist_element    *se_prev,    *se_next;  assert (lfds611_slist_in_list_element != NULL);  assert (se != NULL);  LFDS611_BARRIER_LOAD;  se_prev = (struct lfds611_slist_element *)lfds611_slist_in_list_element;  se_next = se_prev->next;  do {    se->next = se_next;  } while (se->next != (se_next = (struct lfds611_slist_element *)lfds611_abstraction_cas ((volatile lfds611_atom_t *)&se_prev->next, (lfds611_atom_t) se, (lfds611_atom_t) se->next)));  return;}
