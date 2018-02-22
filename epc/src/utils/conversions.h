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


#include "assertions.h"

#ifndef FILE_CONVERSIONS_SEEN
#define FILE_CONVERSIONS_SEEN

/* Endianness conversions for 16 and 32 bits integers from host to network order */
#if (BYTE_ORDER == LITTLE_ENDIAN)
# define hton_int32(x)   \
    (((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) |  \
    ((x & 0x00FF0000) >> 8) | ((x & 0xFF000000) >> 24))

# define hton_int16(x)   \
    (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8)

# define ntoh_int32_buf(bUF)        \
    ((*(bUF)) << 24) | ((*((bUF) + 1)) << 16) | ((*((bUF) + 2)) << 8)   \
  | (*((bUF) + 3))
#else
# define hton_int32(x) (x)
# define hton_int16(x) (x)
#endif

#define IN_ADDR_TO_BUFFER(X,bUFF) INT32_TO_BUFFER((X).s_addr,(char*)bUFF)

#define IN6_ADDR_TO_BUFFER(X,bUFF)                     \
    do {                                               \
        ((uint8_t*)(bUFF))[0]  = (X).s6_addr[0];  \
        ((uint8_t*)(bUFF))[1]  = (X).s6_addr[1];  \
        ((uint8_t*)(bUFF))[2]  = (X).s6_addr[2];  \
        ((uint8_t*)(bUFF))[3]  = (X).s6_addr[3];  \
        ((uint8_t*)(bUFF))[4]  = (X).s6_addr[4];  \
        ((uint8_t*)(bUFF))[5]  = (X).s6_addr[5];  \
        ((uint8_t*)(bUFF))[6]  = (X).s6_addr[6];  \
        ((uint8_t*)(bUFF))[7]  = (X).s6_addr[7];  \
        ((uint8_t*)(bUFF))[8]  = (X).s6_addr[8];  \
        ((uint8_t*)(bUFF))[9]  = (X).s6_addr[9];  \
        ((uint8_t*)(bUFF))[10] = (X).s6_addr[10]; \
        ((uint8_t*)(bUFF))[11] = (X).s6_addr[11]; \
        ((uint8_t*)(bUFF))[12] = (X).s6_addr[12]; \
        ((uint8_t*)(bUFF))[13] = (X).s6_addr[13]; \
        ((uint8_t*)(bUFF))[14] = (X).s6_addr[14]; \
        ((uint8_t*)(bUFF))[15] = (X).s6_addr[15]; \
    } while(0)

#define BUFFER_TO_INT8(buf, x) (x = ((buf)[0]))

#define INT8_TO_BUFFER(x, buf) ((buf)[0] = (x))

/* Convert an integer on 16 bits to the given bUFFER */
#define INT16_TO_BUFFER(x, buf) \
do {                            \
    (buf)[0] = (x) >> 8;        \
    (buf)[1] = (x);             \
} while(0)

/* Convert an array of char containing vALUE to x */
#define BUFFER_TO_INT16(buf, x) \
do {                            \
    x = ((buf)[0] << 8)  |      \
        ((buf)[1]);             \
} while(0)

/* Convert an integer on 32 bits to the given bUFFER */
#define INT32_TO_BUFFER(x, buf) \
do {                            \
    (buf)[0] = (x) >> 24;       \
    (buf)[1] = (x) >> 16;       \
    (buf)[2] = (x) >> 8;        \
    (buf)[3] = (x);             \
} while(0)

/* Convert an array of char containing vALUE to x */
#define BUFFER_TO_INT32(buf, x) \
do {                            \
    x = ((buf)[0] << 24) |      \
        ((buf)[1] << 16) |      \
        ((buf)[2] << 8)  |      \
        ((buf)[3]);             \
} while(0)

/* Convert an integer on 32 bits to an octet string from aSN1c tool */
#define INT32_TO_OCTET_STRING(x, aSN)           \
do {                                            \
    (aSN)->buf = calloc(4, sizeof(uint8_t));    \
    INT32_TO_BUFFER(x, ((aSN)->buf));           \
    (aSN)->size = 4;                            \
} while(0)

#define INT32_TO_BIT_STRING(x, aSN) \
do {                                \
    INT32_TO_OCTET_STRING(x, aSN);  \
    (aSN)->bits_unused = 0;         \
} while(0)

#define INT16_TO_OCTET_STRING(x, aSN)           \
do {                                            \
    (aSN)->buf = calloc(2, sizeof(uint8_t));    \
    (aSN)->size = 2;              \
    INT16_TO_BUFFER(x, (aSN)->buf);             \
} while(0)

#define INT8_TO_OCTET_STRING(x, aSN)            \
do {                                            \
    (aSN)->buf = calloc(1, sizeof(uint8_t));    \
    (aSN)->size = 1;                            \
    INT8_TO_BUFFER(x, (aSN)->buf);              \
} while(0)

#define MME_CODE_TO_OCTET_STRING INT8_TO_OCTET_STRING
#define M_TMSI_TO_OCTET_STRING   INT32_TO_OCTET_STRING
#define MME_GID_TO_OCTET_STRING  INT16_TO_OCTET_STRING

#define OCTET_STRING_TO_INT8(aSN, x)    \
do {                                    \
    DevCheck((aSN)->size == 1, (aSN)->size, 0, 0);           \
    BUFFER_TO_INT8((aSN)->buf, x);    \
} while(0)

#define OCTET_STRING_TO_INT16(aSN, x)   \
do {                                    \
    DevCheck((aSN)->size == 2, (aSN)->size, 0, 0);           \
    BUFFER_TO_INT16((aSN)->buf, x);    \
} while(0)

#define OCTET_STRING_TO_INT32(aSN, x)   \
do {                                    \
    DevCheck((aSN)->size == 4, (aSN)->size, 0, 0);           \
    BUFFER_TO_INT32((aSN)->buf, x);    \
} while(0)

#define BIT_STRING_TO_INT32(aSN, x)     \
do {                                    \
    DevCheck((aSN)->bits_unused == 0, (aSN)->bits_unused, 0, 0);    \
    OCTET_STRING_TO_INT32(aSN, x);      \
} while(0)

#define BIT_STRING_TO_CELL_IDENTITY(aSN, vALUE)                     \
do {                                                                \
    DevCheck((aSN)->bits_unused == 4, (aSN)->bits_unused, 4, 0);    \
    vALUE.enb_id = ((aSN)->buf[0] << 12) | ((aSN)->buf[1] << 4) |   \
        ((aSN)->buf[2] >> 4);                                       \
    vALUE.cell_id = ((aSN)->buf[2] << 4) | ((aSN)->buf[3] >> 4);    \
} while(0)

#define MCC_HUNDREDS(vALUE) \
    ((vALUE) / 100)
/* When MNC is only composed of 2 digits, set the hundreds unit to 0xf */
#define MNC_HUNDREDS(vALUE, mNCdIGITlENGTH) \
    ( mNCdIGITlENGTH == 2 ? 15 : (vALUE) / 100)
#define MCC_MNC_DECIMAL(vALUE) \
    (((vALUE) / 10) % 10)
#define MCC_MNC_DIGIT(vALUE) \
    ((vALUE) % 10)

#define MCC_TO_BUFFER(mCC, bUFFER)      \
do {                                    \
    DevAssert(bUFFER != NULL);          \
    (bUFFER)[0] = MCC_HUNDREDS(mCC);    \
    (bUFFER)[1] = MCC_MNC_DECIMAL(mCC); \
    (bUFFER)[2] = MCC_MNC_DIGIT(mCC);   \
} while(0)

#define MCC_MNC_TO_PLMNID(mCC, mNC, mNCdIGITlENGTH, oCTETsTRING)               \
do {                                                                           \
    (oCTETsTRING)->buf = calloc(3, sizeof(uint8_t));                           \
    (oCTETsTRING)->buf[0] = (MCC_MNC_DECIMAL(mCC) << 4) | MCC_HUNDREDS(mCC);   \
    (oCTETsTRING)->buf[1] = (MNC_HUNDREDS(mNC,mNCdIGITlENGTH) << 4) | MCC_MNC_DIGIT(mCC);     \
    (oCTETsTRING)->buf[2] = (MCC_MNC_DIGIT(mNC) << 4) | MCC_MNC_DECIMAL(mNC);  \
    (oCTETsTRING)->size = 3;                                                   \
} while(0)

#define MCC_MNC_TO_TBCD(mCC, mNC, mNCdIGITlENGTH, tBCDsTRING)        \
do {                                                                 \
    char _buf[3];                                                    \
     DevAssert((mNCdIGITlENGTH == 3) || (mNCdIGITlENGTH == 2));      \
    _buf[0] = (MCC_MNC_DECIMAL(mCC) << 4) | MCC_HUNDREDS(mCC);       \
    _buf[1] = (MNC_HUNDREDS(mNC,mNCdIGITlENGTH) << 4) | MCC_MNC_DIGIT(mCC);\
    _buf[2] = (MCC_MNC_DIGIT(mNC) << 4) | MCC_MNC_DECIMAL(mNC);      \
    OCTET_STRING_fromBuf(tBCDsTRING, _buf, 3);                       \
} while(0)

#define TBCD_TO_MCC_MNC(tBCDsTRING, mCC, mNC, mNCdIGITlENGTH)    \
do {                                                             \
    int mNC_hundred;                                             \
    DevAssert((tBCDsTRING)->size == 3);                          \
    mNC_hundred = (((tBCDsTRING)->buf[1] & 0xf0) >> 4);          \
    if (mNC_hundred == 0xf) {                                    \
        mNC_hundred = 0;                                         \
        mNCdIGITlENGTH = 2;                                      \
    } else {                                                     \
            mNCdIGITlENGTH = 3;                                  \
    }                                                            \
    mCC = (((((tBCDsTRING)->buf[0]) & 0xf0) >> 4) * 10) +        \
        ((((tBCDsTRING)->buf[0]) & 0x0f) * 100) +                \
        (((tBCDsTRING)->buf[1]) & 0x0f);                         \
    mNC = (mNC_hundred * 100) +                                  \
        ((((tBCDsTRING)->buf[2]) & 0xf0) >> 4) +                 \
        ((((tBCDsTRING)->buf[2]) & 0x0f) * 10);                  \
} while(0)

#define TBCD_TO_PLMN_T(tBCDsTRING, pLMN)                            \
do {                                                                \
    DevAssert((tBCDsTRING)->size == 3);                             \
    (pLMN)->mcc_digit2 = (((tBCDsTRING)->buf[0] & 0xf0) >> 4);      \
    (pLMN)->mcc_digit1 = ((tBCDsTRING)->buf[0]  & 0x0f);            \
    (pLMN)->mnc_digit3 = (((tBCDsTRING)->buf[1]  & 0xf0) >> 4);     \
    (pLMN)->mcc_digit3 = ((tBCDsTRING)->buf[1]  & 0x0f);            \
    (pLMN)->mnc_digit2 = (((tBCDsTRING)->buf[2] & 0xf0) >> 4);      \
    (pLMN)->mnc_digit1 = ((tBCDsTRING)->buf[2]  & 0x0f);            \
} while(0)

#define PLMN_T_TO_TBCD(pLMN, tBCDsTRING, mNClENGTH)                 \
do {                                                                \
    tBCDsTRING[0] = (pLMN.mcc_digit2 << 4) | pLMN.mcc_digit1;         \
    /* ambiguous (think about len 2) */                             \
    if (mNClENGTH == 2) {                                      \
        tBCDsTRING[1] = (0x0F << 4) | pLMN.mcc_digit3;               \
        tBCDsTRING[2] = (pLMN.mnc_digit2 << 4) | pLMN.mnc_digit1;     \
    } else {                                                        \
        tBCDsTRING[1] = (pLMN.mnc_digit3 << 4) | pLMN.mcc_digit3;     \
        tBCDsTRING[2] = (pLMN.mnc_digit2 << 4) | pLMN.mnc_digit1;     \
    }                                                               \
} while(0)

#define PLMN_T_TO_MCC_MNC(pLMN, mCC, mNC, mNCdIGITlENGTH)               \
do {                                                                    \
    mCC = pLMN.mcc_digit3 * 100 + pLMN.mcc_digit2 * 10 + pLMN.mcc_digit1;  \
    mNCdIGITlENGTH = (pLMN.mnc_digit3 == 0xF ? 2 : 3);                   \
    mNC = (mNCdIGITlENGTH == 2 ? 0 : pLMN.mnc_digit3 * 100)              \
          + pLMN.mnc_digit2 * 10 + pLMN.mnc_digit1;                       \
} while(0)

/*
 * TS 36.413 v10.9.0 section 9.2.1.37:
 * Macro eNB ID:
 * Equal to the 20 leftmost bits of the Cell
 * Identity IE contained in the E-UTRAN CGI
 * IE (see subclause 9.2.1.38) of each cell
 * served by the eNB.
 */
#define MACRO_ENB_ID_TO_BIT_STRING(mACRO, bITsTRING)    \
do {                                                    \
    (bITsTRING)->buf = calloc(3, sizeof(uint8_t));      \
    (bITsTRING)->buf[0] = ((mACRO) >> 12);              \
    (bITsTRING)->buf[1] = (mACRO) >> 4;                 \
    (bITsTRING)->buf[2] = ((mACRO) & 0x0f) << 4;        \
    (bITsTRING)->size = 3;                              \
    (bITsTRING)->bits_unused = 4;                       \
} while(0)
/*
 * TS 36.413 v10.9.0 section 9.2.1.38:
 * E-UTRAN CGI/Cell Identity
 * The leftmost bits of the Cell
 * Identity correspond to the eNB
 * ID (defined in subclause 9.2.1.37).
 */
#define MACRO_ENB_ID_TO_CELL_IDENTITY(mACRO, cELL_iD, bITsTRING) \
do {                                                    \
    (bITsTRING)->buf = calloc(4, sizeof(uint8_t));      \
    (bITsTRING)->buf[0] = ((mACRO) >> 12);              \
    (bITsTRING)->buf[1] = (mACRO) >> 4;                 \
    (bITsTRING)->buf[2] = (((mACRO) & 0x0f) << 4) | ((cELL_iD) >> 4);        \
    (bITsTRING)->buf[3] = ((cELL_iD) & 0x0f) << 4;        \
    (bITsTRING)->size = 4;                              \
    (bITsTRING)->bits_unused = 4;                       \
} while(0)

/* Used to format an uint32_t containing an ipv4 address */
#define IPV4_ADDR    "%u.%u.%u.%u"
#define IPV4_ADDR_FORMAT(aDDRESS)               \
    (uint8_t)((aDDRESS)  & 0x000000ff),         \
    (uint8_t)(((aDDRESS) & 0x0000ff00) >> 8 ),  \
    (uint8_t)(((aDDRESS) & 0x00ff0000) >> 16),  \
    (uint8_t)(((aDDRESS) & 0xff000000) >> 24)

#define IPV4_ADDR_DISPLAY_8(aDDRESS)            \
    (aDDRESS)[0], (aDDRESS)[1], (aDDRESS)[2], (aDDRESS)[3]

#define TAC_TO_ASN1 INT16_TO_OCTET_STRING
#define GTP_TEID_TO_ASN1 INT32_TO_OCTET_STRING
#define OCTET_STRING_TO_TAC OCTET_STRING_TO_INT16
#define OCTET_STRING_TO_MME_CODE OCTET_STRING_TO_INT8
#define OCTET_STRING_TO_M_TMSI   OCTET_STRING_TO_INT32
#define OCTET_STRING_TO_MME_GID  OCTET_STRING_TO_INT16
#define OCTET_STRING_TO_CSG_ID   OCTET_STRING_TO_INT27

/* Convert the IMSI contained by a char string NULL terminated to uint64_t */
#define IMSI_STRING_TO_IMSI64(sTRING, iMSI64_pTr) sscanf(sTRING, IMSI_64_FMT, iMSI64_pTr)
#define IMSI64_TO_STRING(iMSI64, sTRING) snprintf(sTRING, IMSI_BCD_DIGITS_MAX+1, IMSI_64_FMT, iMSI64)
#define IMSI_TO_IMSI64(iMsI_t_PtR,iMsI_u64) \
        {\
          uint64_t mUlT = 1; \
          iMsI_u64 = (iMsI_t_PtR)->u.num.digit1; \
          if ((iMsI_t_PtR)->u.num.digit15 != 0xf) { \
            iMsI_u64 = (iMsI_t_PtR)->u.num.digit15 + \
                       (iMsI_t_PtR)->u.num.digit14 *10 + \
                       (iMsI_t_PtR)->u.num.digit13 *100 + \
                       (iMsI_t_PtR)->u.num.digit12 *1000 + \
                       (iMsI_t_PtR)->u.num.digit11 *10000 + \
                       (iMsI_t_PtR)->u.num.digit10 *100000 + \
                       (iMsI_t_PtR)->u.num.digit9  *1000000 + \
                       (iMsI_t_PtR)->u.num.digit8  *10000000 + \
                       (iMsI_t_PtR)->u.num.digit7  *100000000;  \
            mUlT = 1000000000; \
          } else { \
            iMsI_u64 = (iMsI_t_PtR)->u.num.digit14  + \
                       (iMsI_t_PtR)->u.num.digit13 *10 + \
                       (iMsI_t_PtR)->u.num.digit12 *100 + \
                       (iMsI_t_PtR)->u.num.digit11 *1000 + \
                       (iMsI_t_PtR)->u.num.digit10 *10000 + \
                       (iMsI_t_PtR)->u.num.digit9  *100000 + \
                       (iMsI_t_PtR)->u.num.digit8  *1000000 + \
                       (iMsI_t_PtR)->u.num.digit7  *10000000;  \
            mUlT = 100000000; \
          } \
          if ((iMsI_t_PtR)->u.num.digit6 != 0xf) {\
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit6 *mUlT;   \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit5 *mUlT*10; \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit4 *mUlT*100; \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit3 *mUlT*1000; \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit2 *mUlT*10000; \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit1 *mUlT*100000; \
          } else { \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit5 *mUlT;    \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit4 *mUlT*10;  \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit3 *mUlT*100;  \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit2 *mUlT*1000;  \
              iMsI_u64 += (iMsI_t_PtR)->u.num.digit1 *mUlT*10000;  \
          } \
        }
/*#define IMSI_TO_STRING(iMsI_t_PtR,iMsI_sTr, MaXlEn) \
        {\
          int l_offset = 0;\
          int l_ret    = 0;\
          l_ret = snprintf(iMsI_sTr + l_offset, MaXlEn - l_offset, "%u%u%u%u%u",\
              (iMsI_t_PtR)->u.num.digit1, (iMsI_t_PtR)->u.num.digit2,\
                  (iMsI_t_PtR)->u.num.digit3, (iMsI_t_PtR)->u.num.digit4,\
                  (iMsI_t_PtR)->u.num.digit5);\
          if (((iMsI_t_PtR)->u.num.digit6 != 0xf)  && (l_ret > 0)) {\
            l_offset += l_ret;\
            l_ret = snprintf(iMsI_sTr + l_offset, MaXlEn - l_offset,  "%u", (iMsI_t_PtR)->u.num.digit6);\
          }\
          if (l_ret > 0) {\
            l_offset += l_ret;\
            l_ret = snprintf(iMsI_sTr + l_offset, MaXlEn - l_offset, "%u%u%u%u%u%u%u%u",\
                (iMsI_t_PtR)->u.num.digit7, (iMsI_t_PtR)->u.num.digit8,\
                (iMsI_t_PtR)->u.num.digit9, (iMsI_t_PtR)->u.num.digit10,\
                (iMsI_t_PtR)->u.num.digit11, (iMsI_t_PtR)->u.num.digit12,\
                (iMsI_t_PtR)->u.num.digit13, (iMsI_t_PtR)->u.num.digit14);\
          }\
          if (((iMsI_t_PtR)->u.num.digit15 != 0x0)   && (l_ret > 0)) {\
            l_offset += l_ret;\
            l_ret = snprintf(iMsI_sTr + l_offset, MaXlEn - l_offset, "%u", (iMsI_t_PtR)->u.num.digit15);\
          }\
        }*/

#define IMSI_TO_STRING(iMsI_t_PtR,iMsI_sTr, MaXlEn) \
        do { \
          int l_i = 0; \
          int l_j = 0; \
          while((l_i < IMSI_BCD8_SIZE) && (l_j < MaXlEn - 1)){ \
            if((((iMsI_t_PtR)->u.value[l_i] & 0xf0) >> 4) > 9) \
              break; \
            sprintf(((iMsI_sTr) + l_j), "%u",(((iMsI_t_PtR)->u.value[l_i] & 0xf0) >> 4)); \
            l_j++; \
            if(((iMsI_t_PtR)->u.value[l_i] & 0xf) > 9 || (l_j >= MaXlEn - 1)) \
              break; \
            sprintf(((iMsI_sTr) + l_j), "%u", ((iMsI_t_PtR)->u.value[l_i] & 0xf)); \
            l_j++; \
            l_i++; \
          } \
          for(; l_j < MaXlEn; l_j++) \
              iMsI_sTr[l_j] = '\0'; \
        } while (0);\

#define IMEI_TO_STRING(iMeI_t_PtR,iMeI_sTr, MaXlEn) \
        {\
          int l_offset = 0;\
          int l_ret    = 0;\
          l_ret = snprintf(iMeI_sTr + l_offset, MaXlEn - l_offset, "%u%u%u%u%u%u%u%u",\
                  (iMeI_t_PtR)->u.num.tac1, (iMeI_t_PtR)->u.num.tac2,\
                  (iMeI_t_PtR)->u.num.tac3, (iMeI_t_PtR)->u.num.tac4,\
                  (iMeI_t_PtR)->u.num.tac5, (iMeI_t_PtR)->u.num.tac6,\
                  (iMeI_t_PtR)->u.num.tac7, (iMeI_t_PtR)->u.num.tac8);\
          if (l_ret > 0) {\
            l_offset += l_ret;\
            l_ret = snprintf(iMeI_sTr + l_offset, MaXlEn - l_offset, "%u%u%u%u%u%u",\
                (iMeI_t_PtR)->u.num.snr1, (iMeI_t_PtR)->u.num.snr2,\
                (iMeI_t_PtR)->u.num.snr3, (iMeI_t_PtR)->u.num.snr4,\
                (iMeI_t_PtR)->u.num.snr5, (iMeI_t_PtR)->u.num.snr6);\
          }\
          if (((iMeI_t_PtR)->u.num.parity != 0x0)   && (l_ret > 0)) {\
            l_offset += l_ret;\
            l_ret = snprintf(iMeI_sTr + l_offset, MaXlEn - l_offset, "%u", (iMeI_t_PtR)->u.num.cdsd);\
          }\
        }


void hexa_to_ascii(uint8_t *from, char *to, size_t length);

int ascii_to_hex(uint8_t *dst, const char *h);

#endif /* FILE_CONVERSIONS_SEEN */
