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

#include "lfds611_ringbuffer_internal.h"/****************************************************************************///#pragma warning( disable : 4100 )voidlfds611_ringbuffer_query (  struct lfds611_ringbuffer_state *rs,  enum lfds611_ringbuffer_query_type query_type,  void *query_input,  void *query_output) {  assert (rs != NULL);    // TRD : query_type can be any value in its range  // TRD : query_input can be NULL  assert (query_output != NULL);  switch (query_type) {    case LFDS611_RINGBUFFER_QUERY_VALIDATE:      // TRD : query_input can be NULL      lfds611_ringbuffer_internal_validate (rs, (struct lfds611_validation_info *)query_input, (enum lfds611_data_structure_validity *)query_output,                                            ((enum lfds611_data_structure_validity *)query_output) + 2);      break;  }  return;}//#pragma warning( default : 4100 )/****************************************************************************/voidlfds611_ringbuffer_internal_validate (  struct lfds611_ringbuffer_state *rs,  struct lfds611_validation_info *vi,  enum lfds611_data_structure_validity *lfds611_queue_validity,  enum lfds611_data_structure_validity *lfds611_freelist_validity) {  assert (rs != NULL);  // TRD : vi can be NULL  assert (lfds611_queue_validity != NULL);  assert (lfds611_freelist_validity != NULL);  lfds611_queue_query (rs->qs, LFDS611_QUEUE_QUERY_VALIDATE, vi, lfds611_queue_validity);  if (vi != NULL) {    struct lfds611_validation_info      lfds611_freelist_vi;    lfds611_atom_t    total_elements;    lfds611_freelist_query (rs->fs, LFDS611_FREELIST_QUERY_ELEMENT_COUNT, NULL, (void *)&total_elements);    lfds611_freelist_vi.min_elements = total_elements - vi->max_elements;    lfds611_freelist_vi.max_elements = total_elements - vi->min_elements;    lfds611_freelist_query (rs->fs, LFDS611_FREELIST_QUERY_VALIDATE, (void *)&lfds611_freelist_vi, (void *)lfds611_freelist_validity);  }  if (vi == NULL)    lfds611_freelist_query (rs->fs, LFDS611_FREELIST_QUERY_VALIDATE, NULL, (void *)lfds611_freelist_validity);  return;}
