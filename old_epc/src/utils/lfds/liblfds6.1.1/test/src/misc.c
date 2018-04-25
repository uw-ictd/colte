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

#include "internal.h"/****************************************************************************/voidinternal_display_test_name (  char *test_name) {  assert (test_name != NULL);  printf ("%s...", test_name);  fflush (stdout);  return;}/****************************************************************************/voidinternal_display_test_result (  unsigned int number_name_dvs_pairs,  ...) {  va_list  va;  int  passed_flag = RAISED;  unsigned int  loop;  char  *name;  enum lfds611_data_structure_validity  dvs;        // TRD : number_name_dvs_pairs can be any value in its range  va_start (va, number_name_dvs_pairs);  for (loop = 0; loop < number_name_dvs_pairs; loop++) {    name = va_arg (va, char *);    dvs = va_arg (va, enum lfds611_data_structure_validity);    if (dvs != LFDS611_VALIDITY_VALID) {      passed_flag = LOWERED;      break;    }  }  va_end (va);  if (passed_flag == RAISED)    puts ("passed");  if (passed_flag == LOWERED) {    printf ("failed (");    va_start (va, number_name_dvs_pairs);    for (loop = 0; loop < number_name_dvs_pairs; loop++) {      name = va_arg (va, char *);      dvs = va_arg (va, enum lfds611_data_structure_validity);      printf ("%s ", name);      internal_display_lfds611_data_structure_validity (dvs);      if (loop + 1 < number_name_dvs_pairs)        printf (", ");    }    va_end (va);    printf (")\n");  }  return;}/****************************************************************************/voidinternal_display_lfds611_data_structure_validity (  enum lfds611_data_structure_validity dvs) {  char  *string = NULL;  switch (dvs) {    case LFDS611_VALIDITY_VALID:      string = "valid";      break;    case LFDS611_VALIDITY_INVALID_LOOP:      string = "invalid - loop detected";      break;    case LFDS611_VALIDITY_INVALID_MISSING_ELEMENTS:      string = "invalid - missing elements";      break;    case LFDS611_VALIDITY_INVALID_ADDITIONAL_ELEMENTS:      string = "invalid - additional elements";      break;    case LFDS611_VALIDITY_INVALID_TEST_DATA:      string = "invalid - invalid test data";      break;  }  printf ("%s", string);  return;}
