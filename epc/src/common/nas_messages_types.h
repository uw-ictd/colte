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
#include "as_message.h"
#include "nas_message.h"

#ifndef FILE_NAS_MESSAGES_TYPES_SEEN
#define FILE_NAS_MESSAGES_TYPES_SEEN

#define NAS_DL_EMM_RAW_MSG(mSGpTR)                  (mSGpTR)->ittiMsg.nas_dl_emm_raw_msg
#define NAS_UL_EMM_RAW_MSG(mSGpTR)                  (mSGpTR)->ittiMsg.nas_ul_emm_raw_msg

#define NAS_DL_EMM_PROTECTED_MSG(mSGpTR)            (mSGpTR)->ittiMsg.nas_dl_emm_protected_msg
#define NAS_UL_EMM_PROTECTED_MSG(mSGpTR)            (mSGpTR)->ittiMsg.nas_ul_emm_protected_msg
#define NAS_DL_EMM_PLAIN_MSG(mSGpTR)                (mSGpTR)->ittiMsg.nas_dl_emm_plain_msg
#define NAS_UL_EMM_PLAIN_MSG(mSGpTR)                (mSGpTR)->ittiMsg.nas_ul_emm_plain_msg

#define NAS_DL_ESM_RAW_MSG(mSGpTR)                  (mSGpTR)->ittiMsg.nas_dl_esm_raw_msg
#define NAS_UL_ESM_RAW_MSG(mSGpTR)                  (mSGpTR)->ittiMsg.nas_ul_esm_raw_msg

#define NAS_DL_ESM_PROTECTED_MSG(mSGpTR)            (mSGpTR)->ittiMsg.nas_dl_esm_protected_msg
#define NAS_UL_ESM_PROTECTED_MSG(mSGpTR)            (mSGpTR)->ittiMsg.nas_ul_esm_protected_msg
#define NAS_DL_ESM_PLAIN_MSG(mSGpTR)                (mSGpTR)->ittiMsg.nas_dl_esm_plain_msg
#define NAS_UL_ESM_PLAIN_MSG(mSGpTR)                (mSGpTR)->ittiMsg.nas_ul_esm_plain_msg
#define NAS_UL_DATA_IND(mSGpTR)                     (mSGpTR)->ittiMsg.nas_ul_data_ind
#define NAS_DL_DATA_REQ(mSGpTR)                     (mSGpTR)->ittiMsg.nas_dl_data_req
#define NAS_DL_DATA_CNF(mSGpTR)                     (mSGpTR)->ittiMsg.nas_dl_data_cnf
#define NAS_DL_DATA_REJ(mSGpTR)                     (mSGpTR)->ittiMsg.nas_dl_data_rej
#define NAS_PDN_CONNECTIVITY_REQ(mSGpTR)            (mSGpTR)->ittiMsg.nas_pdn_connectivity_req
#define NAS_PDN_CONNECTIVITY_RSP(mSGpTR)            (mSGpTR)->ittiMsg.nas_pdn_connectivity_rsp
#define NAS_PDN_CONNECTIVITY_FAIL(mSGpTR)           (mSGpTR)->ittiMsg.nas_pdn_connectivity_fail
#define NAS_INITIAL_UE_MESSAGE(mSGpTR)              (mSGpTR)->ittiMsg.nas_initial_ue_message
#define NAS_CONNECTION_ESTABLISHMENT_CNF(mSGpTR)    (mSGpTR)->ittiMsg.nas_conn_est_cnf
#define NAS_BEARER_PARAM(mSGpTR)                    (mSGpTR)->ittiMsg.nas_bearer_param
#define NAS_AUTHENTICATION_REQ(mSGpTR)              (mSGpTR)->ittiMsg.nas_auth_req
#define NAS_AUTHENTICATION_PARAM_REQ(mSGpTR)        (mSGpTR)->ittiMsg.nas_auth_param_req
#define NAS_DETACH_REQ(mSGpTR)                      (mSGpTR)->ittiMsg.nas_detach_req
#define NAS_IMPLICIT_DETACH_UE_IND(mSGpTR)          (mSGpTR)->ittiMsg.nas_implicit_detach_ue_ind
#define NAS_DATA_LENGHT_MAX     256

typedef enum pdn_conn_rsp_cause_e {
  CAUSE_OK = 16,
  CAUSE_CONTEXT_NOT_FOUND = 64,                     
  CAUSE_INVALID_MESSAGE_FORMAT = 65, 
  CAUSE_SERVICE_NOT_SUPPORTED = 68,                  
  CAUSE_SYSTEM_FAILURE = 72,                        
  CAUSE_NO_RESOURCES_AVAILABLE = 73,           
  CAUSE_ALL_DYNAMIC_ADDRESSES_OCCUPIED = 84   
} pdn_conn_rsp_cause_t;


typedef enum {
  EMM_MSG_HEADER = 1,
  EMM_MSG_ATTACH_REQUEST,
  EMM_MSG_ATTACH_ACCEPT,
  EMM_MSG_ATTACH_COMPLETE,
  EMM_MSG_ATTACH_REJECT,
  EMM_MSG_DETACH_REQUEST,
  EMM_MSG_DETACH_ACCEPT,
  EMM_MSG_TRACKING_AREA_UPDATE_REQUEST,
  EMM_MSG_TRACKING_AREA_UPDATE_ACCEPT,
  EMM_MSG_TRACKING_AREA_UPDATE_COMPLETE,
  EMM_MSG_TRACKING_AREA_UPDATE_REJECT,
  EMM_MSG_EXTENDED_SERVICE_REQUEST,
  EMM_MSG_SERVICE_REQUEST,
  EMM_MSG_SERVICE_REJECT,
  EMM_MSG_GUTI_REALLOCATION_COMMAND,
  EMM_MSG_GUTI_REALLOCATION_COMPLETE,
  EMM_MSG_AUTHENTICATION_REQUEST,
  EMM_MSG_AUTHENTICATION_RESPONSE,
  EMM_MSG_AUTHENTICATION_REJECT,
  EMM_MSG_AUTHENTICATION_FAILURE,
  EMM_MSG_IDENTITY_REQUEST,
  EMM_MSG_IDENTITY_RESPONSE,
  EMM_MSG_SECURITY_MODE_COMMAND,
  EMM_MSG_SECURITY_MODE_COMPLETE,
  EMM_MSG_SECURITY_MODE_REJECT,
  EMM_MSG_EMM_STATUS,
  EMM_MSG_EMM_INFORMATION,
  EMM_MSG_DOWNLINK_NAS_TRANSPORT,
  EMM_MSG_UPLINK_NAS_TRANSPORT,
  EMM_MSG_CS_SERVICE_NOTIFICATION,
} itti_emm_message_ids_t;



typedef enum {
  ESM_MSG_HEADER = 1,
  ESM_MSG_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST,
  ESM_MSG_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT,
  ESM_MSG_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT,
  ESM_MSG_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST,
  ESM_MSG_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT,
  ESM_MSG_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT,
  ESM_MSG_MODIFY_EPS_BEARER_CONTEXT_REQUEST,
  ESM_MSG_MODIFY_EPS_BEARER_CONTEXT_ACCEPT,
  ESM_MSG_MODIFY_EPS_BEARER_CONTEXT_REJECT,
  ESM_MSG_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST,
  ESM_MSG_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT,
  ESM_MSG_PDN_CONNECTIVITY_REQUEST,
  ESM_MSG_PDN_CONNECTIVITY_REJECT,
  ESM_MSG_PDN_DISCONNECT_REQUEST,
  ESM_MSG_PDN_DISCONNECT_REJECT,
  ESM_MSG_BEARER_RESOURCE_ALLOCATION_REQUEST,
  ESM_MSG_BEARER_RESOURCE_ALLOCATION_REJECT,
  ESM_MSG_BEARER_RESOURCE_MODIFICATION_REQUEST,
  ESM_MSG_BEARER_RESOURCE_MODIFICATION_REJECT,
  ESM_MSG_ESM_INFORMATION_REQUEST,
  ESM_MSG_ESM_INFORMATION_RESPONSE,
  ESM_MSG_ESM_STATUS,
} itti_esm_message_ids_t;



typedef struct itti_nas_raw_msg_s {
  size_t                          length;
  uint8_t                         data[NAS_DATA_LENGHT_MAX];
} itti_nas_raw_msg_t;



typedef struct itti_nas_emm_plain_msg_s {
  itti_emm_message_ids_t          present;
  EMM_msg                         choice;

} itti_nas_emm_plain_msg_t;



typedef struct itti_nas_emm_protected_msg_s {
  nas_message_security_header_t   header;
  itti_emm_message_ids_t          present;
  EMM_msg                         choice;
} itti_nas_emm_protected_msg_t;


typedef struct itti_nas_esm_plain_msg_s {
  itti_esm_message_ids_t          present;
  ESM_msg                         choice;
} itti_nas_esm_plain_msg_t;


typedef struct itti_nas_esm_protected_msg_s {
  nas_message_security_header_t   header;
  itti_esm_message_ids_t          present;
  ESM_msg                         choice;
} itti_nas_esm_protected_msg_t;


typedef struct itti_nas_paging_ind_s {

} itti_nas_paging_ind_t;


typedef struct itti_nas_pdn_connectivity_req_s {
  int                    pti;   // nas ref  Identity of the procedure transaction executed to activate the PDN connection entry
  mme_ue_s1ap_id_t       ue_id; // nas ref
  char                   imsi[16];
  uint8_t                imsi_length;
  network_qos_t          qos;
  protocol_configuration_options_t pco;
  bstring                apn;
  bstring                pdn_addr;
  int                    pdn_type;
  void                  *proc_data;
  int                    request_type;
} itti_nas_pdn_connectivity_req_t;


typedef struct itti_nas_pdn_connectivity_rsp_s {
  int                     pti;   // nas ref  Identity of the procedure transaction executed to activate the PDN connection entry
  network_qos_t           qos;
  protocol_configuration_options_t pco;
  bstring                 apn;
  bstring                 pdn_addr;
  int                     pdn_type;
  void                   *proc_data;
  int                     request_type;

  mme_ue_s1ap_id_t        ue_id;

  /* Key eNB */
  //uint8_t                 kenb[32];

  ambr_t                  ambr;
  ambr_t                  apn_ambr;

  /* EPS bearer ID */
  unsigned                ebi:4;

  /* QoS */
  qci_t                   qci;
  priority_level_t        prio_level;
  pre_emp_vulnerability_t pre_emp_vulnerability;
  pre_emp_capability_t    pre_emp_capability;

  /* S-GW TEID for user-plane */
  teid_t                  sgw_s1u_teid;
  /* S-GW IP address for User-Plane */
  ip_address_t            sgw_s1u_address;
} itti_nas_pdn_connectivity_rsp_t;


typedef struct itti_nas_pdn_connectivity_fail_s {
  mme_ue_s1ap_id_t        ue_id; 
  int                     pti;  
  pdn_conn_rsp_cause_t    cause;  
} itti_nas_pdn_connectivity_fail_t;


typedef struct itti_nas_initial_ue_message_s {
  nas_establish_ind_t nas;

  /* Transparent message from s1ap to be forwarded to MME_APP or
   * to S1AP if connection establishment is rejected by NAS.
   */
  itti_s1ap_initial_ue_message_t transparent;
} itti_nas_initial_ue_message_t;


typedef struct itti_nas_conn_est_rej_s {
  mme_ue_s1ap_id_t ue_id;         /* UE lower layer identifier   */
  as_stmsi_t       s_tmsi;        /* UE identity                 */
  nas_error_code_t err_code;      /* Transaction status          */
  bstring          nas_msg;       /* NAS message to transfer     */
  uint32_t         nas_ul_count;  /* UL NAS COUNT                */
  uint16_t         selected_encryption_algorithm;
  uint16_t         selected_integrity_algorithm;
} itti_nas_conn_est_rej_t;


typedef struct itti_nas_conn_est_cnf_s {
  mme_ue_s1ap_id_t        ue_id;            /* UE lower layer identifier   */
  nas_error_code_t        err_code;         /* Transaction status          */
  bstring                 nas_msg;          /* NAS message to transfer     */

  uint8_t                 kenb[32];


  uint32_t                ul_nas_count;
  uint16_t                encryption_algorithm_capabilities;
  uint16_t                integrity_algorithm_capabilities;
} itti_nas_conn_est_cnf_t;

typedef struct itti_nas_conn_rel_ind_s {

} itti_nas_conn_rel_ind_t;


typedef struct itti_nas_info_transfer_s {
  mme_ue_s1ap_id_t  ue_id;          /* UE lower layer identifier        */
  //nas_error_code_t err_code;     /* Transaction status               */
  bstring           nas_msg;        /* Uplink NAS message           */
} itti_nas_info_transfer_t;


typedef struct itti_nas_ul_data_ind_s {
  mme_ue_s1ap_id_t  ue_id;          /* UE lower layer identifier        */
  bstring           nas_msg;        /* Uplink NAS message           */
  tai_t             tai;            /* Indicating the Tracking Area from which the UE has sent the NAS message.  */
  ecgi_t            cgi;            /* Indicating the cell from which the UE has sent the NAS message.   */
} itti_nas_ul_data_ind_t;


typedef struct itti_nas_dl_data_req_s {
  mme_ue_s1ap_id_t  ue_id;              /* UE lower layer identifier        */
  nas_error_code_t  transaction_status;  /* Transaction status               */
  bstring           nas_msg;            /* Downlink NAS message             */
} itti_nas_dl_data_req_t;

typedef struct itti_nas_dl_data_cnf_s {
  mme_ue_s1ap_id_t ue_id;      /* UE lower layer identifier        */
  nas_error_code_t err_code;   /* Transaction status               */
} itti_nas_dl_data_cnf_t;

typedef struct itti_nas_dl_data_rej_s {
  mme_ue_s1ap_id_t ue_id;            /* UE lower layer identifier   */
  bstring          nas_msg;          /* Uplink NAS message           */
} itti_nas_dl_data_rej_t;

typedef struct itti_nas_rab_est_req_s {

} itti_nas_rab_est_req_t;


typedef struct itti_nas_rab_est_rsp_s {

} itti_nas_rab_est_rsp_t;


typedef struct itti_nas_rab_rel_req_s {

} itti_nas_rab_rel_req_t;


typedef struct itti_nas_attach_req_s {
  /* TODO: Set the correct size */
  char apn[100];
  char imsi[16];
#define INITIAL_REQUEST (0x1)
  unsigned initial:1;
  itti_s1ap_initial_ue_message_t transparent;
} itti_nas_attach_req_t;


typedef struct itti_nas_auth_req_s {
  /* UE imsi */
  char imsi[16];

#define NAS_FAILURE_OK  0x0
#define NAS_FAILURE_IND 0x1
  unsigned failure:1;
  int cause;
} itti_nas_auth_req_t;


typedef struct itti_nas_auth_resp_s {
  char imsi[16];
} itti_nas_auth_resp_t;

typedef struct itti_nas_auth_param_req_s {
  /* UE identifier */
  mme_ue_s1ap_id_t ue_id;

  /* Imsi of the UE (In case of initial request) */
  char     imsi[16];
  uint8_t  imsi_length;

  /* Indicates whether the procedure corresponds to a new connection or not */
  uint8_t  initial_req:1;

  uint8_t  re_synchronization:1;
  uint8_t  auts[14];
  uint8_t  num_vectors;
} itti_nas_auth_param_req_t;

typedef struct itti_nas_detach_req_s {
  /* UE identifier */
  mme_ue_s1ap_id_t ue_id;
} itti_nas_detach_req_t;

typedef struct itti_nas_implicit_detach_ue_ind_s {
  /* UE identifier */
  mme_ue_s1ap_id_t ue_id;
} itti_nas_implicit_detach_ue_ind_t;


#endif /* FILE_NAS_MESSAGES_TYPES_SEEN */
