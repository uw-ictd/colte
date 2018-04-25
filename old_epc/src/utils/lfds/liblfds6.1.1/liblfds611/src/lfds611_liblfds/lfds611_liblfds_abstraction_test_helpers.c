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

#include "lfds611_liblfds_internal.h"/****************************************************************************/voidlfds611_liblfds_abstraction_test_helper_increment_non_atomic (  lfds611_atom_t *shared_counter) {  /*     TRD : lfds611_atom_t must be volatile or the compiler     optimizes it away into a single store  */  volatile                               lfds611_atom_t  count = 0;  assert (shared_counter != NULL);  while (count++ < 10000000)    (*(lfds611_atom_t *) shared_counter)++;  return;}/****************************************************************************/voidlfds611_liblfds_abstraction_test_helper_increment_atomic (  volatile lfds611_atom_t *shared_counter) {  lfds611_atom_t  count = 0;  assert (shared_counter != NULL);  while (count++ < 10000000)    lfds611_abstraction_increment (shared_counter);  return;}/****************************************************************************/voidlfds611_liblfds_abstraction_test_helper_cas (  volatile lfds611_atom_t *shared_counter,  lfds611_atom_t *local_counter) {  lfds611_atom_t  loop = 0,  original_destination;  LFDS611_ALIGN (LFDS611_ALIGN_SINGLE_POINTER) lfds611_atom_t  exchange,  compare;  assert (shared_counter != NULL);  assert (local_counter != NULL);  while (loop++ < 1000000) {    do {      compare = *shared_counter;      exchange = compare + 1;      original_destination = lfds611_abstraction_cas (shared_counter, exchange, compare);    } while (original_destination != compare);    (*local_counter)++;  }  return;}/****************************************************************************/voidlfds611_liblfds_abstraction_test_helper_dcas (  volatile lfds611_atom_t *shared_counter,  lfds611_atom_t *local_counter) {  lfds611_atom_t  loop = 0;  LFDS611_ALIGN (LFDS611_ALIGN_DOUBLE_POINTER) lfds611_atom_t  exchange[2],           compare[2];  assert (shared_counter != NULL);  assert (local_counter != NULL);  while (loop++ < 1000000) {    compare[0] = *shared_counter;    compare[1] = *(shared_counter + 1);    do {      exchange[0] = compare[0] + 1;      exchange[1] = compare[1];    } while (0 == lfds611_abstraction_dcas (shared_counter, exchange, compare));    (*local_counter)++;  }  return;}
