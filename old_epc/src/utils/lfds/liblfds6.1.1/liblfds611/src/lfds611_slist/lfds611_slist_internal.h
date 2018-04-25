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
#define LFDS611_SLIST_USER_DATA  0
#define LFDS611_SLIST_FLAGS      1

#define LFDS611_SLIST_NO_FLAGS      0x0
#define LFDS611_SLIST_FLAG_DELETED  0x1

/***** structures *****/
#pragma pack( push, LFDS611_ALIGN_SINGLE_POINTER )

struct lfds611_slist_state {
  struct lfds611_slist_element
      *volatile head;

  void
  (*user_data_delete_function)( void *user_data, void *user_state ),
  *user_state;
};

#pragma pack( pop )

#pragma pack( push, LFDS611_ALIGN_DOUBLE_POINTER )

/* TRD : this pragma pack doesn't seem to work under Windows
         if the structure members are the correct way round
         (next first), then user_data_and_flags ends up on
         a single pointer boundary and DCAS crashes

         accordingly, I've moved user_data_and_flags first
*/

struct lfds611_slist_element {
  void
  *volatile user_data_and_flags[2];

  // TRD : requires volatile as is target of CAS
  struct lfds611_slist_element
      *volatile next;
};

#pragma pack( pop )

/***** private prototypes *****/
void lfds611_slist_internal_init_slist( struct lfds611_slist_state *ss, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state );

void lfds611_slist_internal_link_element_to_head( struct lfds611_slist_state *lfds611_slist_state, struct lfds611_slist_element *volatile se );
void lfds611_slist_internal_link_element_after_element( struct lfds611_slist_element *volatile lfds611_slist_in_list_element, struct lfds611_slist_element *volatile se );

void lfds611_slist_internal_move_to_first_undeleted_element( struct lfds611_slist_element **se );

