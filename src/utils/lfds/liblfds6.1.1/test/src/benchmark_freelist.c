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

#include "internal.h"/****************************************************************************/voidbenchmark_lfds611_freelist (  void) {  unsigned int  loop,  thread_count,  cpu_count;  struct lfds611_freelist_state    *fs;  struct lfds611_freelist_benchmark    *fb;  thread_state_t  *thread_handles;  lfds611_atom_t  total_operations_for_full_test_for_all_cpus,  total_operations_for_full_test_for_all_cpus_for_one_cpu = 0;  double  mean_operations_per_second_per_cpu,  difference_per_second_per_cpu,  total_difference_per_second_per_cpu,  std_dev_per_second_per_cpu,  scalability;  /*     TRD : here we benchmark the freelist     the benchmark is to have a single freelist     where a worker thread busy-works popping and then pushing  */  cpu_count = abstraction_cpu_count ();  thread_handles = (thread_state_t *) malloc (sizeof (thread_state_t) * cpu_count);  fb = (struct lfds611_freelist_benchmark *)malloc (sizeof (struct lfds611_freelist_benchmark) * cpu_count);    // TRD : print the benchmark ID and CSV header  printf ("\n"          "Release %s Freelist Benchmark #1\n"          "CPUs,total ops,mean ops/sec per CPU,standard deviation,scalability\n", LFDS611_RELEASE_NUMBER_STRING);  // TRD : we run CPU count times for scalability  for (thread_count = 1; thread_count <= cpu_count; thread_count++) {    // TRD : initialisation    lfds611_freelist_new (&fs, 1000, NULL, NULL);    for (loop = 0; loop < cpu_count; loop++) {      (fb + loop)->fs = fs;      (fb + loop)->operation_count = 0;    }    // TRD : main test    for (loop = 0; loop < thread_count; loop++)      abstraction_thread_start (&thread_handles[loop], loop, benchmark_lfds611_freelist_thread_pop_and_push, fb + loop);    for (loop = 0; loop < thread_count; loop++)      abstraction_thread_wait (thread_handles[loop]);    // TRD : post test math    total_operations_for_full_test_for_all_cpus = 0;    total_difference_per_second_per_cpu = 0;    for (loop = 0; loop < thread_count; loop++)      total_operations_for_full_test_for_all_cpus += (fb + loop)->operation_count;    mean_operations_per_second_per_cpu = ((double)total_operations_for_full_test_for_all_cpus / (double)thread_count) / (double)10;    if (thread_count == 1)      total_operations_for_full_test_for_all_cpus_for_one_cpu = total_operations_for_full_test_for_all_cpus;    for (loop = 0; loop < thread_count; loop++) {      difference_per_second_per_cpu = ((double)(fb + loop)->operation_count / (double)10) - mean_operations_per_second_per_cpu;      total_difference_per_second_per_cpu += difference_per_second_per_cpu * difference_per_second_per_cpu;    }    std_dev_per_second_per_cpu = sqrt ((double)total_difference_per_second_per_cpu);    scalability = (double)total_operations_for_full_test_for_all_cpus / (double)(total_operations_for_full_test_for_all_cpus_for_one_cpu * thread_count);    printf ("%u,%u,%.0f,%.0f,%0.2f\n", thread_count, (unsigned int)total_operations_for_full_test_for_all_cpus, mean_operations_per_second_per_cpu, std_dev_per_second_per_cpu, scalability);    // TRD : cleanup    lfds611_freelist_delete (fs, NULL, NULL);  }  free (fb);  free (thread_handles);  return;}/****************************************************************************/thread_return_t CALLING_CONVENTION benchmark_lfds611_freelist_thread_pop_and_push (void *freelist_benchmark) {  struct lfds611_freelist_benchmark    *fb;  struct lfds611_freelist_element    *fe;  time_t  start_time;  assert (freelist_benchmark != NULL);  fb = (struct lfds611_freelist_benchmark *)freelist_benchmark;  time (&start_time);  while (time (NULL) < start_time + 10) {    lfds611_freelist_pop (fb->fs, &fe);    lfds611_freelist_push (fb->fs, fe);    fb->operation_count += 2;  }  return ((thread_return_t) EXIT_SUCCESS);}
