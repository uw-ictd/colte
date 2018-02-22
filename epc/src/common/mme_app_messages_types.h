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
#ifndef FILE_MME_APP_MESSAGES_TYPES_SEEN
#define FILE_MME_APP_MESSAGES_TYPES_SEEN

#define MME_APP_INITIAL_UE_MESSAGE(mSGpTR)               (mSGpTR)->ittiMsg.mme_app_initial_ue_message
#define MME_APP_CONNECTION_ESTABLISHMENT_CNF(mSGpTR)     (mSGpTR)->ittiMsg.mme_app_connection_establishment_cnf
#define MME_APP_INITIAL_CONTEXT_SETUP_RSP(mSGpTR)        (mSGpTR)->ittiMsg.mme_app_initial_context_setup_rsp
#define MME_APP_INITIAL_CONTEXT_SETUP_FAILURE(mSGpTR)    (mSGpTR)->ittiMsg.mme_app_initial_context_setup_failure
#define MME_APP_S1AP_MME_UE_ID_NOTIFICATION(mSGpTR)      (mSGpTR)->ittiMsg.mme_app_s1ap_mme_ue_id_notification

typedef struct itti_mme_app_initial_ue_message_s {
  sctp_assoc_id_t     sctp_assoc_id; // key stored in MME_APP for MME_APP forward NAS response to S1AP
  uint32_t            enb_id; 
  mme_ue_s1ap_id_t    mme_ue_s1ap_id;
  enb_ue_s1ap_id_t    enb_ue_s1ap_id;
  bstring             nas;
  tai_t               tai;               /* Indicating the Tracking Area from which the UE has sent the NAS message.                         */
  ecgi_t              cgi;               /* Indicating the cell from which the UE has sent the NAS message.                         */
  as_cause_t          as_cause;          /* Establishment cause                     */

  bool                is_s_tmsi_valid;
  bool                is_csg_id_valid;
  bool                is_gummei_valid;
  as_stmsi_t          opt_s_tmsi;
  csg_id_t            opt_csg_id;
  gummei_t            opt_gummei;
  //void                opt_cell_access_mode;
  //void                opt_cell_gw_transport_address;
  //void                opt_relay_node_indicator;
  /* Transparent message from s1ap to be forwarded to MME_APP or
   * to S1AP if connection establishment is rejected by NAS.
   */
  itti_s1ap_initial_ue_message_t transparent;
} itti_mme_app_initial_ue_message_t;

typedef struct itti_mme_app_connection_establishment_cnf_s {
  ebi_t                   eps_bearer_id;
  FTeid_t                 bearer_s1u_sgw_fteid;
  qci_t                   bearer_qos_qci;
  priority_level_t        bearer_qos_prio_level;
  pre_emp_vulnerability_t bearer_qos_pre_emp_vulnerability;
  pre_emp_capability_t    bearer_qos_pre_emp_capability;
  ambr_t                  ambr;

  /* Key eNB */
  uint8_t                 kenb[AUTH_KASME_SIZE];
  uint16_t                security_capabilities_encryption_algorithms;
  uint16_t                security_capabilities_integrity_algorithms;

  uint8_t                 *ue_radio_capabilities;
  int                     ue_radio_cap_length;

  itti_nas_conn_est_cnf_t nas_conn_est_cnf;
} itti_mme_app_connection_establishment_cnf_t;

typedef struct itti_mme_app_initial_context_setup_rsp_s {
  uint32_t                mme_ue_s1ap_id;
  ebi_t                   eps_bearer_id;
  FTeid_t                 bearer_s1u_enb_fteid;
} itti_mme_app_initial_context_setup_rsp_t;

typedef struct itti_mme_app_initial_context_setup_failure_s {
  uint32_t                mme_ue_s1ap_id;
} itti_mme_app_initial_context_setup_failure_t;

typedef struct itti_mme_app_delete_session_rsp_s {
  /* UE identifier */
  mme_ue_s1ap_id_t	  ue_id;
} itti_mme_app_delete_session_rsp_t;

typedef struct itti_mme_app_s1ap_mme_ue_id_notification_s {
  enb_ue_s1ap_id_t	    enb_ue_s1ap_id;
  mme_ue_s1ap_id_t	    mme_ue_s1ap_id;
  sctp_assoc_id_t       sctp_assoc_id;
} itti_mme_app_s1ap_mme_ue_id_notification_t;

#endif /* FILE_MME_APP_MESSAGES_TYPES_SEEN */
