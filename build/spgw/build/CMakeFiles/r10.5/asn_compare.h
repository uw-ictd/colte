#ifndef	_ASN_COMPARE_H_
#define	_ASN_COMPARE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */


typedef enum COMPARE_ERR_CODE_e {
  COMPARE_ERR_CODE_START = 0,
  COMPARE_ERR_CODE_NONE = COMPARE_ERR_CODE_START,
  COMPARE_ERR_CODE_NO_MATCH,
  COMPARE_ERR_CODE_TYPE_MISMATCH,
  COMPARE_ERR_CODE_TYPE_ARG_NULL,
  COMPARE_ERR_CODE_VALUE_NULL,
  COMPARE_ERR_CODE_VALUE_ARG_NULL,
  COMPARE_ERR_CODE_CHOICE_NUM,
  COMPARE_ERR_CODE_CHOICE_PRESENT,
  COMPARE_ERR_CODE_CHOICE_MALFORMED,
  COMPARE_ERR_CODE_SET_MALFORMED,
  COMPARE_ERR_CODE_COLLECTION_NUM_ELEMENTS,
  COMPARE_ERR_CODE_END
} COMPARE_ERR_CODE_t;

typedef struct asn_comp_rval_s {
  enum COMPARE_ERR_CODE_e err_code;
  char                   *name; // e_S1ap_ProtocolIE_ID not available for all ASN1 use (RRC vs S1AP, X2AP)
  const void             *structure1;
  const void             *structure2;
  struct asn_comp_rval_s *next;
} asn_comp_rval_t;

#define COMPARE_CHECK_ARGS(aRg_tYpE_dEf1, aRg_tYpE_dEf2, aRg_vAl1, aRg_vAl2, rEsUlT) \
    do {\
      if ((aRg_tYpE_dEf1) && (aRg_tYpE_dEf2)) {\
        if ((aRg_tYpE_dEf1->name) && (aRg_tYpE_dEf2->name)) {\
          if (strcmp(aRg_tYpE_dEf1->name, aRg_tYpE_dEf2->name)) {\
            rEsUlT           = (asn_comp_rval_t *)calloc(1, sizeof(asn_comp_rval_t));\
            rEsUlT->err_code = COMPARE_ERR_CODE_TYPE_MISMATCH;\
            rEsUlT->name     = aRg_tYpE_dEf1->name;\
            return rEsUlT;\
          }\
        } else {\
          if ((aRg_tYpE_dEf1->xml_tag) && (aRg_tYpE_dEf2->xml_tag)) {\
            if (strcmp(aRg_tYpE_dEf1->xml_tag, aRg_tYpE_dEf2->xml_tag)) {\
              rEsUlT           = (asn_comp_rval_t *)calloc(1, sizeof(asn_comp_rval_t));\
              rEsUlT->err_code = COMPARE_ERR_CODE_TYPE_MISMATCH;\
              rEsUlT->name     = aRg_tYpE_dEf1->xml_tag;\
              return rEsUlT;\
            }\
          }\
        }\
      } else {\
        rEsUlT             = (asn_comp_rval_t *)calloc(1, sizeof(asn_comp_rval_t));\
        rEsUlT->name       = aRg_tYpE_dEf1->name;\
        rEsUlT->structure1 = aRg_vAl1;\
        rEsUlT->structure2 = aRg_vAl2;\
        rEsUlT->err_code   = COMPARE_ERR_CODE_TYPE_ARG_NULL;\
        return rEsUlT;\
      }\
      if ((NULL == aRg_vAl1) || (NULL == aRg_vAl2)){\
        rEsUlT             = (asn_comp_rval_t *)calloc(1, sizeof(asn_comp_rval_t));\
        rEsUlT->name       = aRg_tYpE_dEf1->name;\
        rEsUlT->structure1 = aRg_vAl1;\
        rEsUlT->structure2 = aRg_vAl2;\
        rEsUlT->err_code   = COMPARE_ERR_CODE_VALUE_ARG_NULL;\
        return rEsUlT;\
      }\
    } while (0);


#ifdef __cplusplus
}
#endif

#endif	/* _ASN_COMPARE_H_ */
