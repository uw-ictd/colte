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

/***** the library wide include file *****/
#include "liblfds611_internal.h"

/***** defines *****/
#define LFDS611_FREELIST_POINTER 0
#define LFDS611_FREELIST_COUNTER 1
#define LFDS611_FREELIST_PAC_SIZE 2

/***** structures *****/
#pragma pack( push, LFDS611_ALIGN_DOUBLE_POINTER )

struct lfds611_freelist_state {
  struct lfds611_freelist_element
      *volatile top[LFDS611_FREELIST_PAC_SIZE];

  int
  (*user_data_init_function)( void **user_data, void *user_state );

  void
  *user_state;

  lfds611_atom_t
  aba_counter,
  element_count;
};

struct lfds611_freelist_element {
  struct lfds611_freelist_element
      *next[LFDS611_FREELIST_PAC_SIZE];

  void
  *user_data;
};

#pragma pack( pop )

/***** private prototypes *****/
lfds611_atom_t lfds611_freelist_internal_new_element( struct lfds611_freelist_state *fs, struct lfds611_freelist_element **fe );
void lfds611_freelist_internal_validate( struct lfds611_freelist_state *fs, struct lfds611_validation_info *vi, enum lfds611_data_structure_validity *lfds611_freelist_validity );

