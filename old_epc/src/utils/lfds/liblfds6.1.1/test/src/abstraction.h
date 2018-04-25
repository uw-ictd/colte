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

/***** defines *****/
#if (defined _WIN32 && defined _MSC_VER && !defined WIN_KERNEL_BUILD)
/* TRD : any Windows (user-mode) on any CPU with the Microsoft C compiler

         _WIN32             indicates 64-bit or 32-bit Windows
         _MSC_VER           indicates Microsoft C compiler
         !WIN_KERNEL_BUILD  indicates Windows user-mode
*/

#include <windows.h>
typedef HANDLE              thread_state_t;
typedef DWORD               thread_return_t;
#define CALLING_CONVENTION  WINAPI
#endif

#if (defined _WIN32 && defined _MSC_VER && defined WIN_KERNEL_BUILD)
/* TRD : any Windows (kernel-mode) on any CPU with the Microsoft C compiler

         _WIN32            indicates 64-bit or 32-bit Windows
         _MSC_VER          indicates Microsoft C compiler
         WIN_KERNEL_BUILD  indicates Windows kernel
*/

#include <wdm.h>
typedef HANDLE              thread_state_t;
typedef VOID                thread_return_t;
#define CALLING_CONVENTION
#endif

#if (defined __unix__ && defined __GNUC__)
/* TRD : any UNIX on any CPU with GCC

         __unix__   indicates Solaris, Linux, HPUX, etc
         __GNUC__   indicates GCC
*/
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
typedef pthread_t           thread_state_t;
typedef void *              thread_return_t;
#define CALLING_CONVENTION
#endif

typedef thread_return_t (CALLING_CONVENTION *thread_function_t)( void *thread_user_state );

/***** public prototypes *****/
unsigned int abstraction_cpu_count( void );
int abstraction_thread_start( thread_state_t *thread_state, unsigned int cpu, thread_function_t thread_function, void *thread_user_state );
void abstraction_thread_wait( thread_state_t thread_state );

