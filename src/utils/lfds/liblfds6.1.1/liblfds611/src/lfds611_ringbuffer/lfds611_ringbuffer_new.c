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

#include "lfds611_ringbuffer_internal.h"/****************************************************************************/intlfds611_ringbuffer_new (  struct lfds611_ringbuffer_state **rs,  lfds611_atom_t number_elements,  int                                     (*user_data_init_function) (void **user_data,      void *user_state),  void *user_state) {  int  rv = 0;  assert (rs != NULL);      // TRD : number_elements can be any value in its range  // TRD : user_data_init_function can be NULL  // TRD : user_state can be NULL  *rs = (struct lfds611_ringbuffer_state *)lfds611_liblfds_aligned_malloc (sizeof (struct lfds611_ringbuffer_state), LFDS611_ALIGN_DOUBLE_POINTER);  if (*rs != NULL) {    lfds611_freelist_new (&(*rs)->fs, number_elements, user_data_init_function, user_state);    if ((*rs)->fs != NULL) {      lfds611_queue_new (&(*rs)->qs, number_elements);      if ((*rs)->qs != NULL)        rv = 1;      if ((*rs)->qs == NULL) {        lfds611_liblfds_aligned_free (*rs);        *rs = NULL;      }    }    if ((*rs)->fs == NULL) {      lfds611_liblfds_aligned_free (*rs);      *rs = NULL;    }  }  LFDS611_BARRIER_STORE;  return (rv);}/****************************************************************************///#pragma warning( disable : 4100 )voidlfds611_ringbuffer_use (  struct lfds611_ringbuffer_state *rs) {  assert (rs != NULL);  LFDS611_BARRIER_LOAD;  return;}//#pragma warning( default : 4100 )
