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
/*! \file s1ap_messages_types.h
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/
#ifndef FILE_S1AP_MESSAGES_TYPES_SEEN
#define FILE_S1AP_MESSAGES_TYPES_SEEN

#include "3gpp_36.401.h"
#include "3gpp_36.413.h"
#include "3gpp_36.331.h"
#include "3gpp_23.003.h"
#include "TrackingAreaIdentity.h"

#define S1AP_ENB_DEREGISTERED_IND(mSGpTR)        (mSGpTR)->ittiMsg.s1ap_eNB_deregistered_ind
#define S1AP_ENB_INITIATED_RESET_REQ(mSGpTR)     (mSGpTR)->ittiMsg.s1ap_enb_initiated_reset_req
#define S1AP_ENB_INITIATED_RESET_ACK(mSGpTR)     (mSGpTR)->ittiMsg.s1ap_enb_initiated_reset_ack
#define S1AP_DEREGISTER_UE_REQ(mSGpTR)           (mSGpTR)->ittiMsg.s1ap_deregister_ue_req
#define S1AP_UE_CONTEXT_RELEASE_REQ(mSGpTR)      (mSGpTR)->ittiMsg.s1ap_ue_context_release_req
#define S1AP_UE_CONTEXT_RELEASE_COMMAND(mSGpTR)  (mSGpTR)->ittiMsg.s1ap_ue_context_release_command
#define S1AP_UE_CONTEXT_RELEASE_COMPLETE(mSGpTR) (mSGpTR)->ittiMsg.s1ap_ue_context_release_complete
#define S1AP_E_RAB_SETUP_REQ(mSGpTR)             (mSGpTR)->ittiMsg.s1ap_e_rab_setup_req
#define S1AP_E_RAB_SETUP_RSP(mSGpTR)             (mSGpTR)->ittiMsg.s1ap_e_rab_setup_rsp
#define S1AP_INITIAL_UE_MESSAGE(mSGpTR)          (mSGpTR)->ittiMsg.s1ap_initial_ue_message
#define S1AP_NAS_DL_DATA_REQ(mSGpTR)             (mSGpTR)->ittiMsg.s1ap_nas_dl_data_req

// NOT a ITTI message
typedef struct s1ap_initial_ue_message_s {
  enb_ue_s1ap_id_t     enb_ue_s1ap_id:24;
  ecgi_t                e_utran_cgi;
} s1ap_initial_ue_message_t;

typedef struct itti_s1ap_initial_ctxt_setup_req_s {
  mme_ue_s1ap_id_t        mme_ue_s1ap_id;
  enb_ue_s1ap_id_t        enb_ue_s1ap_id:24;

  /* Key eNB */
  uint8_t                 kenb[32];

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
  teid_t                  teid;
  /* S-GW IP address for User-Plane */
  ip_address_t            s_gw_address;
} itti_s1ap_initial_ctxt_setup_req_t;

typedef struct itti_s1ap_ue_cap_ind_s {
  mme_ue_s1ap_id_t  mme_ue_s1ap_id;
  enb_ue_s1ap_id_t  enb_ue_s1ap_id:24;
  uint8_t           *radio_capabilities;
  size_t            radio_capabilities_length;
} itti_s1ap_ue_cap_ind_t;

#define S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE 128
typedef struct itti_s1ap_eNB_deregistered_ind_s {
  uint16_t         nb_ue_to_deregister;
  enb_ue_s1ap_id_t enb_ue_s1ap_id[S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE];
  mme_ue_s1ap_id_t mme_ue_s1ap_id[S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE];
  uint32_t         enb_id;
} itti_s1ap_eNB_deregistered_ind_t;

typedef struct itti_s1ap_deregister_ue_req_s {
  mme_ue_s1ap_id_t mme_ue_s1ap_id;
} itti_s1ap_deregister_ue_req_t;

typedef struct itti_s1ap_ue_context_release_req_s {
  mme_ue_s1ap_id_t  mme_ue_s1ap_id;
  enb_ue_s1ap_id_t  enb_ue_s1ap_id:24;
  uint32_t          enb_id;
  S1ap_Cause_t      cause;
} itti_s1ap_ue_context_release_req_t;

typedef enum s1ap_reset_type_e {
  RESET_ALL = 0,
  RESET_PARTIAL
} s1ap_reset_type_t;

typedef struct s1_sig_conn_id_s {
  mme_ue_s1ap_id_t*  mme_ue_s1ap_id;
  enb_ue_s1ap_id_t*  enb_ue_s1ap_id;
} s1_sig_conn_id_t;

typedef struct itti_s1ap_enb_initiated_reset_req_s {
  uint32_t          sctp_assoc_id;
  uint16_t          sctp_stream_id;
  uint32_t          enb_id;
  s1ap_reset_type_t  s1ap_reset_type;
  uint32_t          num_ue;
  s1_sig_conn_id_t  *ue_to_reset_list;
} itti_s1ap_enb_initiated_reset_req_t;

typedef struct itti_s1ap_enb_initiated_reset_ack_s {
  uint32_t          sctp_assoc_id;
  uint16_t          sctp_stream_id;
  s1ap_reset_type_t  s1ap_reset_type;
  uint32_t          num_ue;
  s1_sig_conn_id_t  *ue_to_reset_list;
} itti_s1ap_enb_initiated_reset_ack_t;

// List of possible causes for MME generated UE context release command towards eNB
enum s1cause {
  S1AP_INVALID_CAUSE = 0,
  S1AP_NAS_NORMAL_RELEASE,
  S1AP_NAS_DETACH,
  S1AP_RADIO_EUTRAN_GENERATED_REASON,
  S1AP_IMPLICIT_CONTEXT_RELEASE,
  S1AP_INITIAL_CONTEXT_SETUP_FAILED,
  S1AP_SCTP_SHUTDOWN_OR_RESET
};
typedef struct itti_s1ap_ue_context_release_command_s {
  mme_ue_s1ap_id_t  mme_ue_s1ap_id;
  enb_ue_s1ap_id_t  enb_ue_s1ap_id:24;
  enum s1cause      cause;
} itti_s1ap_ue_context_release_command_t;

typedef struct itti_s1ap_dl_nas_data_req_s {
  mme_ue_s1ap_id_t  mme_ue_s1ap_id;
  enb_ue_s1ap_id_t  enb_ue_s1ap_id:24;
  bstring           nas_msg;            /* Downlink NAS message             */
} itti_s1ap_nas_dl_data_req_t;

typedef struct itti_s1ap_ue_context_release_complete_s {
  mme_ue_s1ap_id_t  mme_ue_s1ap_id;
  enb_ue_s1ap_id_t  enb_ue_s1ap_id:24;
} itti_s1ap_ue_context_release_complete_t;

typedef struct itti_s1ap_initial_ue_message_s {
  sctp_assoc_id_t     sctp_assoc_id; // key stored in MME_APP for MME_APP forward NAS response to S1AP
  uint32_t            enb_id; 
  enb_ue_s1ap_id_t    enb_ue_s1ap_id;
  mme_ue_s1ap_id_t    mme_ue_s1ap_id;
  bstring             nas;
  tai_t               tai;               /* Indicating the Tracking Area from which the UE has sent the NAS message.                         */
  ecgi_t              ecgi;              /* Indicating the cell from which the UE has sent the NAS message.                         */
  rrc_establishment_cause_t      rrc_establishment_cause;          /* Establishment cause                     */

  bool                is_s_tmsi_valid;
  bool                is_csg_id_valid;
  bool                is_gummei_valid;
  s_tmsi_t            opt_s_tmsi;
  csg_id_t            opt_csg_id;
  gummei_t            opt_gummei;
  //void                opt_cell_access_mode;
  //void                opt_cell_gw_transport_address;
  //void                opt_relay_node_indicator;
  /* Transparent message from s1ap to be forwarded to MME_APP or
   * to S1AP if connection establishment is rejected by NAS.
   */
  s1ap_initial_ue_message_t transparent;
} itti_s1ap_initial_ue_message_t;

typedef struct itti_s1ap_e_rab_setup_req_s {
  mme_ue_s1ap_id_t    mme_ue_s1ap_id;
  enb_ue_s1ap_id_t    enb_ue_s1ap_id;

  // Applicable for non-GBR E-RABs
  bool                            ue_aggregate_maximum_bit_rate_present;
  ue_aggregate_maximum_bit_rate_t ue_aggregate_maximum_bit_rate;

  // E-RAB to Be Setup List
  e_rab_to_be_setup_list_t        e_rab_to_be_setup_list;

} itti_s1ap_e_rab_setup_req_t;


typedef struct itti_s1ap_e_rab_setup_rsp_s {
  mme_ue_s1ap_id_t    mme_ue_s1ap_id;
  enb_ue_s1ap_id_t    enb_ue_s1ap_id;

  // E-RAB to Be Setup List
  e_rab_setup_list_t                  e_rab_setup_list;

  // Optional
  e_rab_list_t        e_rab_failed_to_setup_list;

} itti_s1ap_e_rab_setup_rsp_t;

#endif /* FILE_S1AP_MESSAGES_TYPES_SEEN */
