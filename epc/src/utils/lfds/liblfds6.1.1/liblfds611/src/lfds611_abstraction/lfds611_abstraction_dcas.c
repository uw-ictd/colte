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

#include "lfds611_abstraction_internal_body.h"





/****************************************************************************/
#if (defined _WIN64 && defined _MSC_VER)

/* TRD : 64 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

         _WIN64    indicates 64 bit Windows
         _MSC_VER  indicates Microsoft C compiler
*/

static LFDS611_INLINE unsigned char
lfds611_abstraction_dcas (
  volatile lfds611_atom_t * destination,
  lfds611_atom_t * exchange,
  lfds611_atom_t * compare)
{
  unsigned char
                                          cas_result;

  assert (destination != NULL);
  assert (exchange != NULL);
  assert (compare != NULL);
  LFDS611_BARRIER_COMPILER_FULL;
  cas_result = _InterlockedCompareExchange128 ((volatile __int64 *)destination, (__int64) * (exchange + 1), (__int64) * exchange, (__int64 *) compare);
  LFDS611_BARRIER_COMPILER_FULL;
  return (cas_result);
}

#endif





/****************************************************************************/
#if (!defined _WIN64 && defined _WIN32 && defined _MSC_VER)

/* TRD : 32 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

         (!defined _WIN64 && defined _WIN32)  indicates 32 bit Windows
         _MSC_VER                             indicates Microsoft C compiler
*/

static LFDS611_INLINE unsigned char
lfds611_abstraction_dcas (
  volatile lfds611_atom_t * destination,
  lfds611_atom_t * exchange,
  lfds611_atom_t * compare)
{
  __int64                                 original_compare;

  assert (destination != NULL);
  assert (exchange != NULL);
  assert (compare != NULL);
  *(__int64 *) & original_compare = *(__int64 *) compare;
  LFDS611_BARRIER_COMPILER_FULL;
  *(__int64 *) compare = _InterlockedCompareExchange64 ((volatile __int64 *)destination, *(__int64 *) exchange, *(__int64 *) compare);
  LFDS611_BARRIER_COMPILER_FULL;
  return ((unsigned char)(*(__int64 *) compare == *(__int64 *) & original_compare));
}

#endif





/****************************************************************************/
#if (defined __x86_64__ && defined __GNUC__)

/* TRD : any OS on x64 with GCC

         __x86_64__  indicates x64
         __GNUC__    indicates GCC
*/

static LFDS611_INLINE unsigned char
lfds611_abstraction_dcas (
  volatile lfds611_atom_t * destination,
  lfds611_atom_t * exchange,
  lfds611_atom_t * compare)
{
  unsigned char
                                          cas_result;

  assert (destination != NULL);
  assert (exchange != NULL);
  assert (compare != NULL);
  // TRD : __asm__ with "memory" in the clobber list is for GCC a full compiler barrier
  __asm__                                 __volatile__ (
  "lock;"                       // make cmpxchg16b atomic
  "cmpxchg16b %0;"              // cmpxchg16b sets ZF on success
  "setz       %3;"              // if ZF set, set cas_result to 1
  // output
  :"+m"                                   (*(volatile lfds611_atom_t (*)[2])destination),
  "+a"                                    (*compare),
  "+d"                                    (*(compare + 1)),
  "=q"                                    (cas_result)
  // input
  :"b"                                    (*exchange),
  "c"                                     (*(exchange + 1))
  // clobbered
  :"cc",
  "memory");

  return (cas_result);
}

#endif





/****************************************************************************/
#if ((defined __i686__ || defined __arm__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1 && __GNUC_PATCHLEVEL__ >= 0)

/* TRD : any OS on x86 or ARM with GCC 4.1.0 or better

         GCC 4.1.0 introduced the __sync_*() atomic intrinsics

         __GNUC__ / __GNUC_MINOR__ / __GNUC_PATCHLEVEL__  indicates GCC and which version
*/

static LFDS611_INLINE unsigned char
lfds611_abstraction_dcas (
  volatile lfds611_atom_t * destination,
  lfds611_atom_t * exchange,
  lfds611_atom_t * compare)
{
  unsigned char
                                          cas_result = 0;
  unsigned long long int
                                          original_destination;

  assert (destination != NULL);
  assert (exchange != NULL);
  assert (compare != NULL);
  LFDS611_BARRIER_COMPILER_FULL;
  original_destination = __sync_val_compare_and_swap ((volatile unsigned long long int *)destination, *(unsigned long long int *)compare, *(unsigned long long int *)exchange);
  LFDS611_BARRIER_COMPILER_FULL;

  if (original_destination == *(unsigned long long int *)compare)
    cas_result = 1;

  *(unsigned long long int *)compare = original_destination;
  return (cas_result);
}

#endif
