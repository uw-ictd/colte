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

#include "lfds611_freelist_internal.h"/****************************************************************************/voidlfds611_freelist_query (  struct lfds611_freelist_state *fs,  enum lfds611_freelist_query_type query_type,  void *query_input,  void *query_output) {  assert (fs != NULL);       // TRD : query type can be any value in its range  // TRD : query_input can be NULL in some cases  assert (query_output != NULL);  LFDS611_BARRIER_LOAD;  switch (query_type) {    case LFDS611_FREELIST_QUERY_ELEMENT_COUNT:      assert (query_input == NULL);      *(lfds611_atom_t *) query_output = fs->element_count;      break;    case LFDS611_FREELIST_QUERY_VALIDATE:      // TRD : query_input can be NULL      lfds611_freelist_internal_validate (fs, (struct lfds611_validation_info *)query_input, (enum lfds611_data_structure_validity *)query_output);      break;  }  return;}/****************************************************************************/voidlfds611_freelist_internal_validate (  struct lfds611_freelist_state *fs,  struct lfds611_validation_info *vi,  enum lfds611_data_structure_validity *lfds611_freelist_validity) {  struct lfds611_freelist_element    *fe,    *fe_slow,    *fe_fast;  lfds611_atom_t  element_count = 0;  assert (fs != NULL);  // TRD : vi can be NULL  assert (lfds611_freelist_validity != NULL);  *lfds611_freelist_validity = LFDS611_VALIDITY_VALID;  fe_slow = fe_fast = (struct lfds611_freelist_element *)fs->top[LFDS611_FREELIST_POINTER];  /*     TRD : first, check for a loop     we have two pointers     both of which start at the top of the lfds611_freelist     we enter a loop     and on each iteration     we advance one pointer by one element     and the other by two     we exit the loop when both pointers are NULL     (have reached the end of the lfds611_freelist)     or     if we fast pointer 'sees' the slow pointer     which means we have a loop  */  if (fe_slow != NULL)    do {      fe_slow = fe_slow->next[LFDS611_FREELIST_POINTER];      if (fe_fast != NULL)        fe_fast = fe_fast->next[LFDS611_FREELIST_POINTER];      if (fe_fast != NULL)        fe_fast = fe_fast->next[LFDS611_FREELIST_POINTER];    } while (fe_slow != NULL and fe_fast != fe_slow);  if (fe_fast != NULL and fe_slow != NULL and fe_fast == fe_slow)    *lfds611_freelist_validity = LFDS611_VALIDITY_INVALID_LOOP;  /*     TRD : now check for expected number of elements     vi can be NULL, in which case we do not check     we know we don't have a loop from our earlier check  */  if (*lfds611_freelist_validity == LFDS611_VALIDITY_VALID and vi != NULL) {    fe = (struct lfds611_freelist_element *)fs->top[LFDS611_FREELIST_POINTER];    while (fe != NULL) {      element_count++;      fe = (struct lfds611_freelist_element *)fe->next[LFDS611_FREELIST_POINTER];    }    if (element_count < vi->min_elements)      *lfds611_freelist_validity = LFDS611_VALIDITY_INVALID_MISSING_ELEMENTS;    if (element_count > vi->max_elements)      *lfds611_freelist_validity = LFDS611_VALIDITY_INVALID_ADDITIONAL_ELEMENTS;  }  return;}
