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
/*! \file nas_messages_types.h
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/

#ifndef FILE_NAS_MESSAGES_TYPES_SEEN
#define FILE_NAS_MESSAGES_TYPES_SEEN

#include "nas_message.h"
#include "as_message.h"
#include "networkDef.h"

#define NAS_UL_DATA_IND(mSGpTR)                     (mSGpTR)->ittiMsg.nas_ul_data_ind
#define NAS_DL_DATA_REQ(mSGpTR)                     (mSGpTR)->ittiMsg.nas_dl_data_req
#define NAS_DL_DATA_CNF(mSGpTR)                     (mSGpTR)->ittiMsg.nas_dl_data_cnf
#define NAS_DL_DATA_REJ(mSGpTR)                     (mSGpTR)->ittiMsg.nas_dl_data_rej
#define NAS_PDN_CONFIG_REQ(mSGpTR)                  (mSGpTR)->ittiMsg.nas_pdn_config_req
#define NAS_PDN_CONFIG_RSP(mSGpTR)                  (mSGpTR)->ittiMsg.nas_pdn_config_rsp
#define NAS_PDN_CONFIG_FAIL(mSGpTR)                 (mSGpTR)->ittiMsg.nas_pdn_config_fail
#define NAS_PDN_CONNECTIVITY_REQ(mSGpTR)            (mSGpTR)->ittiMsg.nas_pdn_connectivity_req
#define NAS_PDN_CONNECTIVITY_RSP(mSGpTR)            (mSGpTR)->ittiMsg.nas_pdn_connectivity_rsp
#define NAS_PDN_CONNECTIVITY_FAIL(mSGpTR)           (mSGpTR)->ittiMsg.nas_pdn_connectivity_fail
#define NAS_CONNECTION_ESTABLISHMENT_CNF(mSGpTR)    (mSGpTR)->ittiMsg.nas_conn_est_cnf
#define NAS_BEARER_PARAM(mSGpTR)                    (mSGpTR)->ittiMsg.nas_bearer_param
#define NAS_AUTHENTICATION_REQ(mSGpTR)              (mSGpTR)->ittiMsg.nas_auth_req
#define NAS_AUTHENTICATION_PARAM_REQ(mSGpTR)        (mSGpTR)->ittiMsg.nas_auth_param_req
#define NAS_DETACH_REQ(mSGpTR)                      (mSGpTR)->ittiMsg.nas_detach_req
#define NAS_ERAB_SETUP_REQ(mSGpTR)                  (mSGpTR)->ittiMsg.itti_erab_setup_req
#define NAS_IMPLICIT_DETACH_UE_IND(mSGpTR)          (mSGpTR)->ittiMsg.nas_implicit_detach_ue_ind


typedef enum pdn_conn_rsp_cause_e {
  CAUSE_OK = 16,
  CAUSE_CONTEXT_NOT_FOUND = 64,                     
  CAUSE_INVALID_MESSAGE_FORMAT = 65, 
  CAUSE_SERVICE_NOT_SUPPORTED = 68,                  
  CAUSE_SYSTEM_FAILURE = 72,                        
  CAUSE_NO_RESOURCES_AVAILABLE = 73,           
  CAUSE_ALL_DYNAMIC_ADDRESSES_OCCUPIED = 84   
} pdn_conn_rsp_cause_t;





typedef struct itti_nas_pdn_connectivity_req_s {
  proc_tid_t             pti;   // nas ref  Identity of the procedure transaction executed to activate the PDN connection entry
  mme_ue_s1ap_id_t       ue_id; // nas ref
  char                   imsi[16];
  uint8_t                imsi_length;
  bearer_qos_t           bearer_qos;
  protocol_configuration_options_t pco;
  bstring                apn;
  pdn_cid_t              pdn_cid;
  bstring                pdn_addr;
  int                    pdn_type;
  int                    request_type;
} itti_nas_pdn_connectivity_req_t;


typedef struct itti_nas_pdn_connectivity_rsp_s {
  pdn_cid_t               pdn_cid;
  proc_tid_t              pti;   // nas ref  Identity of the procedure transaction executed to activate the PDN connection entry
  network_qos_t           qos;
  protocol_configuration_options_t pco;
  bstring                 pdn_addr;
  int                     pdn_type;
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
  pre_emption_vulnerability_t pre_emp_vulnerability;
  pre_emption_capability_t    pre_emp_capability;

  /* S-GW TEID for user-plane */
  /* S-GW IP address for User-Plane */
  fteid_t                  sgw_s1u_fteid;
} itti_nas_pdn_connectivity_rsp_t;


typedef struct itti_nas_pdn_connectivity_fail_s {
  mme_ue_s1ap_id_t        ue_id; 
  int                     pti;  
  pdn_conn_rsp_cause_t    cause;  
} itti_nas_pdn_connectivity_fail_t;


typedef struct itti_nas_pdn_config_req_s {
  proc_tid_t             pti;   // nas ref  Identity of the procedure transaction executed to activate the PDN connection entry
  mme_ue_s1ap_id_t       ue_id; // nas ref
  char                   imsi[16];
  uint8_t                imsi_length;
  bstring                apn;
  bstring                pdn_addr;
  pdn_type_t             pdn_type;
  int                    request_type;
} itti_nas_pdn_config_req_t;

typedef struct itti_nas_pdn_config_rsp_s {
  mme_ue_s1ap_id_t        ue_id; // nas ref
} itti_nas_pdn_config_rsp_t;

typedef struct itti_nas_pdn_config_fail_s {
  mme_ue_s1ap_id_t        ue_id; // nas ref
} itti_nas_pdn_config_fail_t;


typedef struct itti_nas_conn_est_rej_s {
  mme_ue_s1ap_id_t ue_id;         /* UE lower layer identifier   */
  s_tmsi_t         s_tmsi;        /* UE identity                 */
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
  int              err_code;
} itti_nas_dl_data_rej_t;

typedef struct itti_erab_setup_req_s {
  mme_ue_s1ap_id_t ue_id;            /* UE lower layer identifier   */
  ebi_t            ebi;              /* EPS bearer id        */
  bstring          nas_msg;          /* NAS erab bearer context activation message           */
  bitrate_t        mbr_dl;
  bitrate_t        mbr_ul;
  bitrate_t        gbr_dl;
  bitrate_t        gbr_ul;
} itti_erab_setup_req_t;


typedef struct itti_nas_attach_req_s {
  /* TODO: Set the correct size */
  char apn[100];
  char imsi[16];
#define INITIAL_REQUEST (0x1)
  unsigned initial:1;
  s1ap_initial_ue_message_t transparent;
} itti_nas_attach_req_t;


typedef struct itti_nas_auth_req_s {
  /* UE imsi */
  char imsi[16];

#define NAS_FAILURE_OK  0x0
#define NAS_FAILURE_IND 0x1
  unsigned failure:1;
  int cause;
} itti_nas_auth_req_t;


typedef struct itti_nas_auth_rsp_s {
  char imsi[16];
} itti_nas_auth_rsp_t;

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
  long             cause;
} itti_nas_detach_req_t;

typedef struct itti_nas_implicit_detach_ue_ind_s {
  /* UE identifier */
  mme_ue_s1ap_id_t ue_id;
} itti_nas_implicit_detach_ue_ind_t;


#endif /* FILE_NAS_MESSAGES_TYPES_SEEN */
