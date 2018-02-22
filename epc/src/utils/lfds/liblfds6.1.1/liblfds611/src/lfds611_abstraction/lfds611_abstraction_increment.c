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

static LFDS611_INLINE                   lfds611_atom_t
lfds611_abstraction_increment (
  volatile lfds611_atom_t * value)
{
  lfds611_atom_t                          rv;

  assert (value != NULL);
  LFDS611_BARRIER_COMPILER_FULL;
  rv = (lfds611_atom_t) _InterlockedIncrement64 ((__int64 *) value);
  LFDS611_BARRIER_COMPILER_FULL;
  return (rv);
}

#endif





/****************************************************************************/
#if (!defined _WIN64 && defined _WIN32 && defined _MSC_VER)

/* TRD : 32 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

         (!defined _WIN64 && defined _WIN32)  indicates 32 bit Windows
         _MSC_VER                             indicates Microsoft C compiler
*/

static LFDS611_INLINE                   lfds611_atom_t
lfds611_abstraction_increment (
  volatile lfds611_atom_t * value)
{
  lfds611_atom_t                          rv;

  assert (value != NULL);
  LFDS611_BARRIER_COMPILER_FULL;
  rv = (lfds611_atom_t) _InterlockedIncrement ((long int *)value);
  LFDS611_BARRIER_COMPILER_FULL;
  return (rv);
}

#endif





/****************************************************************************/
#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 1 && __GNUC_PATCHLEVEL__ >= 0)

/* TRD : any OS on any CPU with GCC 4.1.0 or better

         GCC 4.1.0 introduced the __sync_*() atomic intrinsics

         __GNUC__ / __GNUC_MINOR__ / __GNUC_PATCHLEVEL__  indicates GCC and which version
*/

static LFDS611_INLINE                   lfds611_atom_t
lfds611_abstraction_increment (
  volatile lfds611_atom_t * value)
{
  lfds611_atom_t                          rv;

  assert (value != NULL);
  // TRD : no need for casting here, GCC has a __sync_add_and_fetch() for all native types
  LFDS611_BARRIER_COMPILER_FULL;
  rv = (lfds611_atom_t) __sync_add_and_fetch (value, 1);
  LFDS611_BARRIER_COMPILER_FULL;
  return (rv);
}

#endif
