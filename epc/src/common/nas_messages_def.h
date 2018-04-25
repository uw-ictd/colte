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
//WARNING: Do not include this header directly. Use intertask_interface.h instead.

/*! \file nas_messages_def.h
  \brief
  \author Sebastien ROUX, Lionel Gauthier
  \company Eurecom
  \email: lionel.gauthier@eurecom.fr
*/

MESSAGE_DEF(NAS_PDN_CONNECTIVITY_REQ,           MESSAGE_PRIORITY_MED,   itti_nas_pdn_connectivity_req_t, nas_pdn_connectivity_req)
MESSAGE_DEF(NAS_CONNECTION_ESTABLISHMENT_CNF,   MESSAGE_PRIORITY_MED,   itti_nas_conn_est_cnf_t,         nas_conn_est_cnf)
MESSAGE_DEF(NAS_CONNECTION_RELEASE_IND,         MESSAGE_PRIORITY_MED,   itti_nas_conn_rel_ind_t,         nas_conn_rel_ind)
MESSAGE_DEF(NAS_UPLINK_DATA_IND,                MESSAGE_PRIORITY_MED,   itti_nas_ul_data_ind_t,          nas_ul_data_ind)
MESSAGE_DEF(NAS_DOWNLINK_DATA_REQ,              MESSAGE_PRIORITY_MED,   itti_nas_dl_data_req_t,          nas_dl_data_req)
MESSAGE_DEF(NAS_DOWNLINK_DATA_CNF,              MESSAGE_PRIORITY_MED,   itti_nas_dl_data_cnf_t,          nas_dl_data_cnf)
MESSAGE_DEF(NAS_DOWNLINK_DATA_REJ,              MESSAGE_PRIORITY_MED,   itti_nas_dl_data_rej_t,          nas_dl_data_rej)
MESSAGE_DEF(NAS_ERAB_SETUP_REQ,                 MESSAGE_PRIORITY_MED,   itti_erab_setup_req_t,           itti_erab_setup_req)
//MESSAGE_DEF(NAS_RAB_RELEASE_REQ,                MESSAGE_PRIORITY_MED,   itti_nas_rab_rel_req_t,          nas_rab_rel_req)

/* NAS layer -> MME app messages */
MESSAGE_DEF(NAS_AUTHENTICATION_PARAM_REQ,       MESSAGE_PRIORITY_MED,   itti_nas_auth_param_req_t,       nas_auth_param_req)
MESSAGE_DEF(NAS_DETACH_REQ,       		MESSAGE_PRIORITY_MED,   itti_nas_detach_req_t,           	nas_detach_req)
MESSAGE_DEF(NAS_PDN_CONFIG_REQ,                 MESSAGE_PRIORITY_MED,   itti_nas_pdn_config_req_t,       nas_pdn_config_req)

/* MME app -> NAS layer messages */
MESSAGE_DEF(NAS_PDN_CONNECTIVITY_RSP,           MESSAGE_PRIORITY_MED,   itti_nas_pdn_connectivity_rsp_t,  nas_pdn_connectivity_rsp)
MESSAGE_DEF(NAS_PDN_CONNECTIVITY_FAIL,          MESSAGE_PRIORITY_MED,   itti_nas_pdn_connectivity_fail_t, nas_pdn_connectivity_fail)
MESSAGE_DEF(NAS_PDN_CONFIG_RSP,                 MESSAGE_PRIORITY_MED,   itti_nas_pdn_config_rsp_t,       nas_pdn_config_rsp)
MESSAGE_DEF(NAS_PDN_CONFIG_FAIL,                MESSAGE_PRIORITY_MED,   itti_nas_pdn_config_fail_t,      nas_pdn_config_fail)
MESSAGE_DEF(NAS_IMPLICIT_DETACH_UE_IND,         MESSAGE_PRIORITY_MED,   itti_nas_implicit_detach_ue_ind_t, nas_implicit_detach_ue_ind)

