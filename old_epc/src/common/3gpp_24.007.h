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

#ifndef FILE_3GPP_24_007_SEEN
#define FILE_3GPP_24_007_SEEN

#include <stdint.h>

//..............................................................................
// 11.2.3.1.1  Protocol discriminator
//..............................................................................
typedef enum eps_protocol_discriminator_e {
  /* Protocol discriminator identifier for EPS Session Management */
  EPS_SESSION_MANAGEMENT_MESSAGE =    0x2,
  /* Protocol discriminator identifier for EPS Mobility Management */
  EPS_MOBILITY_MANAGEMENT_MESSAGE =   0x7,

} eps_protocol_discriminator_t;

//..............................................................................
// 11.2.3.1.5  EPS bearer identity
//..............................................................................
typedef uint8_t                       ebi_t; //4 bits only

#define EPS_BEARER_IDENTITY_UNASSIGNED   (ebi_t)0
#define EPS_BEARER_IDENTITY_RESERVED1    (ebi_t)1
#define EPS_BEARER_IDENTITY_RESERVED2    (ebi_t)2
#define EPS_BEARER_IDENTITY_RESERVED3    (ebi_t)3
#define EPS_BEARER_IDENTITY_RESERVED4    (ebi_t)4
#define EPS_BEARER_IDENTITY_FIRST        (ebi_t)5
#define EPS_BEARER_IDENTITY_LAST         (ebi_t)15
//..............................................................................
// 11.2.3.1a   Procedure transaction identity
//..............................................................................
typedef uint8_t                       pti_t;

#define PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED   (pti_t)0
#define PROCEDURE_TRANSACTION_IDENTITY_FIRST        (pti_t)1
#define PROCEDURE_TRANSACTION_IDENTITY_LAST         (pti_t)254
#define PROCEDURE_TRANSACTION_IDENTITY_RESERVED     (pti_t)255


#endif /* FILE_3GPP_24_007_SEEN */
