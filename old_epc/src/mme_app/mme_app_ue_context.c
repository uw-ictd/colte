/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the Apache License, Version 2.0  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */


#include "common_types.h"
#include "mme_app_ue_context.h"
#include "conversions.h"

static mme_ue_s1ap_id_t mme_app_ue_s1ap_id_generator = 1;

/**
 * @brief mme_app_convert_imsi_to_imsi_mme: converts the imsi_t struct to the imsi mme struct
 * @param imsi_dst
 * @param imsi_src
 */
// TODO: (amar) This and below functions are only used in testing possibly move
// these to the testing module
void
mme_app_convert_imsi_to_imsi_mme (mme_app_imsi_t * imsi_dst,
                                  const imsi_t *imsi_src)
{
  memset(imsi_dst->data,  (uint8_t) '\0', sizeof(imsi_dst->data));
  IMSI_TO_STRING(imsi_src, imsi_dst->data, IMSI_BCD_DIGITS_MAX + 1);
  imsi_dst->length = strlen(imsi_dst->data);
}

/**
 * @brief mme_app_copy_imsi: copies an mme imsi to another mme imsi
 * @param imsi_dst
 * @param imsi_src
 */

void
mme_app_copy_imsi (mme_app_imsi_t * imsi_dst,
                   const mme_app_imsi_t *imsi_src)
{
  strncpy(imsi_dst->data, imsi_src->data, IMSI_BCD_DIGITS_MAX + 1);
  imsi_dst->length = imsi_src->length;
}

/**
 * @brief mme_app_imsi_compare: compares to imsis returns true if the same else false
 * @param imsi_a
 * @param imsi_b
 * @return
 */

bool
mme_app_imsi_compare (mme_app_imsi_t const * imsi_a,
                      mme_app_imsi_t const * imsi_b)
{
  if((strncmp(imsi_a->data, imsi_b->data, IMSI_BCD_DIGITS_MAX) == 0)
          && imsi_a->length == imsi_b->length) {
    return true;
  }else
    return false;
}

/**
 * @brief mme_app_string_to_imsi converst the a string to the imsi mme structure
 * @param imsi_dst
 * @param imsi_string_src
 */

void
mme_app_string_to_imsi (mme_app_imsi_t * const imsi_dst,
                       char const * const imsi_string_src)
{
    strncpy(imsi_dst->data, imsi_string_src, IMSI_BCD_DIGITS_MAX + 1);
    imsi_dst->length = strlen(imsi_dst->data);
    return;
}

/**
 * @brief mme_app_imsi_to_string converts imsi structure to a string
 * @param imsi_dst
 * @param imsi_src
 */

void
mme_app_imsi_to_string (char * const imsi_dst,
                       mme_app_imsi_t const * const imsi_src)
{
    strncpy(imsi_dst, imsi_src->data, IMSI_BCD_DIGITS_MAX + 1);
    return;
}

/**
 * @brief mme_app_is_imsi_empty: checks if an imsi structe is empty returns true if it is empty
 * @param imsi
 * @return
 */
bool
mme_app_is_imsi_empty (mme_app_imsi_t const * imsi)
{
  return (imsi->length == 0) ? true : false;
}

/**
 * @brief mme_app_imsi_to_u64: converts imsi to uint64 (be carefull leading 00 will be cut off)
 * @param imsi_src
 * @return
 */

uint64_t
mme_app_imsi_to_u64 (mme_app_imsi_t imsi_src)
{
  uint64_t uint_imsi;
  sscanf(imsi_src.data, "%" SCNu64, &uint_imsi);
  return uint_imsi;
}

mme_ue_s1ap_id_t mme_app_ctx_get_new_ue_id(void)
{
  mme_ue_s1ap_id_t tmp = 0;
  tmp = __sync_fetch_and_add (&mme_app_ue_s1ap_id_generator, 1);
  return tmp;
}
