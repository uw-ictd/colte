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


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "assertions.h"
#include "hashtable.h"
#include "log.h"
#include "msc.h"
#include "conversions.h"
#include "intertask_interface.h"
#include "asn1_conversions.h"
#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_encoder.h"
#include "s1ap_mme_itti_messaging.h"
#include "s1ap_mme.h"
#include "dynamic_memory_check.h"

/* Every time a new UE is associated, increment this variable.
   But care if it wraps to increment also the mme_ue_s1ap_id_has_wrapped
   variable. Limit: UINT32_MAX (in stdint.h).
*/
//static mme_ue_s1ap_id_t                 mme_ue_s1ap_id = 0;
//static bool                             mme_ue_s1ap_id_has_wrapped = false;

extern const char                      *s1ap_direction2String[];
extern hash_table_ts_t g_s1ap_mme_id2assoc_id_coll; // contains sctp association id, key is mme_ue_s1ap_id;


//------------------------------------------------------------------------------
int
s1ap_mme_handle_initial_ue_message (
  const sctp_assoc_id_t assoc_id,
  const sctp_stream_id_t stream,
  struct s1ap_message_s *message)
{
  S1ap_InitialUEMessageIEs_t             *initialUEMessage_p = NULL;
  ue_description_t                       *ue_ref = NULL;
  enb_description_t                      *eNB_ref = NULL;
  enb_ue_s1ap_id_t                        enb_ue_s1ap_id = 0;

  OAILOG_FUNC_IN (LOG_S1AP);
  initialUEMessage_p = &message->msg.s1ap_InitialUEMessageIEs;

  OAILOG_INFO (LOG_S1AP, "Received S1AP INITIAL_UE_MESSAGE eNB_UE_S1AP_ID " ENB_UE_S1AP_ID_FMT "\n", (enb_ue_s1ap_id_t)initialUEMessage_p->eNB_UE_S1AP_ID);

  MSC_LOG_RX_MESSAGE (MSC_S1AP_MME, MSC_S1AP_ENB, NULL, 0, "0 initialUEMessage/%s assoc_id %u stream %u " ENB_UE_S1AP_ID_FMT " ",
          s1ap_direction2String[message->direction], assoc_id, stream, (enb_ue_s1ap_id_t)initialUEMessage_p->eNB_UE_S1AP_ID);

  if ((eNB_ref = s1ap_is_enb_assoc_id_in_list (assoc_id)) == NULL) {
    OAILOG_ERROR (LOG_S1AP, "Unknown eNB on assoc_id %d\n", assoc_id);
    OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
  }
  // eNB UE S1AP ID is limited to 24 bits
  enb_ue_s1ap_id = (enb_ue_s1ap_id_t) (initialUEMessage_p->eNB_UE_S1AP_ID & 0x00ffffff);
  OAILOG_INFO (LOG_S1AP, "New Initial UE message received with eNB UE S1AP ID: " ENB_UE_S1AP_ID_FMT "\n", enb_ue_s1ap_id);
  ue_ref = s1ap_is_ue_enb_id_in_list (eNB_ref, enb_ue_s1ap_id);

  if (ue_ref == NULL) {
    tai_t                                   tai = {.plmn = {0}, .tac = INVALID_TAC_0000};
    gummei_t                                gummei = {.plmn = {0}, .mme_code = 0, .mme_gid = 0}; // initialized after
    as_stmsi_t                              s_tmsi = {.mme_code = 0, .m_tmsi = INVALID_M_TMSI};
    ecgi_t                                  ecgi = {.plmn = {0}, .cell_identity = {0}};
    csg_id_t                                csg_id = 0;

    /*
     * This UE eNB Id has currently no known s1 association.
     * * * * Create new UE context by associating new mme_ue_s1ap_id.
     * * * * Update eNB UE list.
     * * * * Forward message to NAS.
     */
    if ((ue_ref = s1ap_new_ue (assoc_id, enb_ue_s1ap_id)) == NULL) {
      // If we failed to allocate a new UE return -1
      OAILOG_ERROR (LOG_S1AP, "S1AP:Initial UE Message- Failed to allocate S1AP UE Context, eNBUeS1APId:" ENB_UE_S1AP_ID_FMT "\n", enb_ue_s1ap_id);
      OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
    }

    ue_ref->s1_ue_state = S1AP_UE_WAITING_CSR;

    ue_ref->enb_ue_s1ap_id = enb_ue_s1ap_id;
    // Will be allocated by NAS
    ue_ref->mme_ue_s1ap_id = INVALID_MME_UE_S1AP_ID;
    
    ue_ref->s1ap_ue_context_rel_timer.id  = S1AP_TIMER_INACTIVE_ID;
    ue_ref->s1ap_ue_context_rel_timer.sec = S1AP_UE_CONTEXT_REL_COMP_TIMER;

    // On which stream we received the message
    ue_ref->sctp_stream_recv = stream;
    ue_ref->sctp_stream_send = ue_ref->enb->next_sctp_stream;

    /*
     * Increment the sctp stream for the eNB association.
     * If the next sctp stream is >= instream negociated between eNB and MME, wrap to first stream.
     * TODO: search for the first available stream instead.
     */

    /* 
     * TODO task#15456359.
     * Below logic seems to be incorrect , revisit it.
     */
    ue_ref->enb->next_sctp_stream += 1;
    if (ue_ref->enb->next_sctp_stream >= ue_ref->enb->instreams) {
      ue_ref->enb->next_sctp_stream = 1;
    }
    s1ap_dump_enb (ue_ref->enb);
    // TAI mandatory IE
    OCTET_STRING_TO_TAC (&initialUEMessage_p->tai.tAC, tai.tac);
    DevAssert (initialUEMessage_p->tai.pLMNidentity.size == 3);
    TBCD_TO_PLMN_T(&initialUEMessage_p->tai.pLMNidentity, &tai.plmn);

    // CGI mandatory IE
    DevAssert (initialUEMessage_p->eutran_cgi.pLMNidentity.size == 3);
    TBCD_TO_PLMN_T(&initialUEMessage_p->eutran_cgi.pLMNidentity, &ecgi.plmn);
    BIT_STRING_TO_CELL_IDENTITY (&initialUEMessage_p->eutran_cgi.cell_ID, ecgi.cell_identity);

    if (initialUEMessage_p->presenceMask & S1AP_INITIALUEMESSAGEIES_S_TMSI_PRESENT) {
      OCTET_STRING_TO_MME_CODE(&initialUEMessage_p->s_tmsi.mMEC, s_tmsi.mme_code);
      OCTET_STRING_TO_M_TMSI(&initialUEMessage_p->s_tmsi.m_TMSI, s_tmsi.m_tmsi);
    }

    if (initialUEMessage_p->presenceMask & S1AP_INITIALUEMESSAGEIES_CSG_ID_PRESENT) {
      csg_id = BIT_STRING_to_uint32(&initialUEMessage_p->csG_Id);
    }

    memset(&gummei, 0, sizeof(gummei));
    if (initialUEMessage_p->presenceMask & S1AP_INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT) {
      //TODO OCTET_STRING_TO_PLMN(&initialUEMessage_p->gummei_id.pLMN_Identity, gummei.plmn);
      OCTET_STRING_TO_MME_GID(&initialUEMessage_p->gummei_id.mME_Group_ID, gummei.mme_gid);
      OCTET_STRING_TO_MME_CODE(&initialUEMessage_p->gummei_id.mME_Code, gummei.mme_code);
    }
    /*
     * We received the first NAS transport message: initial UE message.
     * * * * Send a NAS ESTAeNBBLISH IND to NAS layer
     */
#if ORIGINAL_CODE
    s1ap_mme_itti_nas_establish_ind (ue_ref->mme_ue_s1ap_id, initialUEMessage_p->nas_pdu.buf, initialUEMessage_p->nas_pdu.size,
        initialUEMessage_p->rrC_Establishment_Cause, tai_tac);
#else
#if ITTI_LITE
    itf_mme_app_ll_initial_ue_message(assoc_id,
        ue_ref->enb_ue_s1ap_id,
        ue_ref->mme_ue_s1ap_id,
        initialUEMessage_p->nas_pdu.buf,
        initialUEMessage_p->nas_pdu.size,
        initialUEMessage_p->rrC_Establishment_Cause,
        &tai, &cgi, &s_tmsi, &gummei);
#else
    s1ap_mme_itti_mme_app_initial_ue_message (assoc_id,
        ue_ref->enb->enb_id,
        ue_ref->enb_ue_s1ap_id,
        ue_ref->mme_ue_s1ap_id,
        initialUEMessage_p->nas_pdu.buf,
        initialUEMessage_p->nas_pdu.size,
        &tai,
        &ecgi,
        initialUEMessage_p->rrC_Establishment_Cause,
        (initialUEMessage_p->presenceMask & S1AP_INITIALUEMESSAGEIES_S_TMSI_PRESENT) ? &s_tmsi:NULL,
        (initialUEMessage_p->presenceMask & S1AP_INITIALUEMESSAGEIES_CSG_ID_PRESENT) ? &csg_id:NULL,
        (initialUEMessage_p->presenceMask & S1AP_INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT) ? &gummei:NULL,
        NULL, // CELL ACCESS MODE
        NULL, // GW Transport Layer Address
        NULL  //Relay Node Indicator
        );
#endif
#endif
  } else {
    OAILOG_ERROR (LOG_S1AP, "S1AP:Initial UE Message- Duplicate ENB_UE_S1AP_ID. Ignoring the message, eNBUeS1APId:" ENB_UE_S1AP_ID_FMT "\n", enb_ue_s1ap_id);
  }

  OAILOG_FUNC_RETURN (LOG_S1AP, RETURNok);
}


//------------------------------------------------------------------------------
int
s1ap_mme_handle_uplink_nas_transport (
  const sctp_assoc_id_t assoc_id,
  __attribute__((unused)) const sctp_stream_id_t stream,
  struct s1ap_message_s *message)
{
  S1ap_UplinkNASTransportIEs_t           *uplinkNASTransport_p = NULL;
  ue_description_t                       *ue_ref = NULL;
  enb_description_t                      *enb_ref = NULL;
  tai_t                                   tai = {.plmn = {0}, .tac = INVALID_TAC_0000};
  ecgi_t                                  ecgi = {.plmn = {0}, .cell_identity = {0}};

  OAILOG_FUNC_IN (LOG_S1AP);
  uplinkNASTransport_p = &message->msg.s1ap_UplinkNASTransportIEs;

  if (INVALID_MME_UE_S1AP_ID == uplinkNASTransport_p->mme_ue_s1ap_id) {
    OAILOG_WARNING (LOG_S1AP, "Received S1AP UPLINK_NAS_TRANSPORT message MME_UE_S1AP_ID unknown\n");

    enb_ref = s1ap_is_enb_assoc_id_in_list (assoc_id);

    if (!(ue_ref = s1ap_is_ue_enb_id_in_list ( enb_ref, (enb_ue_s1ap_id_t)uplinkNASTransport_p->eNB_UE_S1AP_ID))) {
      OAILOG_WARNING (LOG_S1AP, "Received S1AP UPLINK_NAS_TRANSPORT No UE is attached to this enb_ue_s1ap_id: " ENB_UE_S1AP_ID_FMT "\n",
          (enb_ue_s1ap_id_t)uplinkNASTransport_p->eNB_UE_S1AP_ID);
      OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
    }
  } else {
    OAILOG_INFO (LOG_S1AP, "Received S1AP UPLINK_NAS_TRANSPORT message MME_UE_S1AP_ID " MME_UE_S1AP_ID_FMT "\n",
        (mme_ue_s1ap_id_t)uplinkNASTransport_p->mme_ue_s1ap_id);

    if (!(ue_ref = s1ap_is_ue_mme_id_in_list (uplinkNASTransport_p->mme_ue_s1ap_id))) {
      OAILOG_WARNING (LOG_S1AP, "Received S1AP UPLINK_NAS_TRANSPORT No UE is attached to this mme_ue_s1ap_id: " MME_UE_S1AP_ID_FMT "\n",
          (mme_ue_s1ap_id_t)uplinkNASTransport_p->mme_ue_s1ap_id);
      OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
    }
  }



  if (S1AP_UE_CONNECTED != ue_ref->s1_ue_state) {
    OAILOG_WARNING (LOG_S1AP, "Received S1AP UPLINK_NAS_TRANSPORT while UE in state != S1AP_UE_CONNECTED\n");
    MSC_LOG_RX_DISCARDED_MESSAGE (MSC_S1AP_MME,
                        MSC_S1AP_ENB,
                        NULL, 0,
                        "0 uplinkNASTransport/%s mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT " nas len %u",
                        s1ap_direction2String[message->direction],
                        (mme_ue_s1ap_id_t)uplinkNASTransport_p->mme_ue_s1ap_id,
                        (enb_ue_s1ap_id_t)uplinkNASTransport_p->eNB_UE_S1AP_ID,
                        uplinkNASTransport_p->nas_pdu.size);

    OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
  }

  // TAI mandatory IE
  OCTET_STRING_TO_TAC (&uplinkNASTransport_p->tai.tAC, tai.tac);
  DevAssert (uplinkNASTransport_p->tai.pLMNidentity.size == 3);
  TBCD_TO_PLMN_T(&uplinkNASTransport_p->tai.pLMNidentity, &tai.plmn);

  // CGI mandatory IE
  DevAssert (uplinkNASTransport_p->eutran_cgi.pLMNidentity.size == 3);
  TBCD_TO_PLMN_T(&uplinkNASTransport_p->eutran_cgi.pLMNidentity, &ecgi.plmn);
  BIT_STRING_TO_CELL_IDENTITY (&uplinkNASTransport_p->eutran_cgi.cell_ID, ecgi.cell_identity);

  // TODO optional GW Transport Layer Address


  MSC_LOG_RX_MESSAGE (MSC_S1AP_MME,
                      MSC_S1AP_ENB,
                      NULL, 0,
                      "0 uplinkNASTransport/%s mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT " nas len %u",
                      s1ap_direction2String[message->direction],
                      (mme_ue_s1ap_id_t)uplinkNASTransport_p->mme_ue_s1ap_id,
                      (enb_ue_s1ap_id_t)uplinkNASTransport_p->eNB_UE_S1AP_ID,
                      uplinkNASTransport_p->nas_pdu.size);

  bstring b = blk2bstr(uplinkNASTransport_p->nas_pdu.buf, uplinkNASTransport_p->nas_pdu.size);
  s1ap_mme_itti_nas_uplink_ind (uplinkNASTransport_p->mme_ue_s1ap_id,
                                &b,
                                &tai,
                                &ecgi);
  OAILOG_FUNC_RETURN (LOG_S1AP, RETURNok);
}


//------------------------------------------------------------------------------
int
s1ap_mme_handle_nas_non_delivery (
    __attribute__((unused)) sctp_assoc_id_t assoc_id,
  sctp_stream_id_t stream,
  struct s1ap_message_s *message)
{
  S1ap_NASNonDeliveryIndication_IEs_t    *nasNonDeliveryIndication_p = NULL;
  ue_description_t                       *ue_ref = NULL;

  OAILOG_FUNC_IN (LOG_S1AP);
  /*
   * UE associated signalling on stream == 0 is not valid.
   */
  if (stream == 0) {
    OAILOG_NOTICE (LOG_S1AP, "Received S1AP NAS_NON_DELIVERY_INDICATION message on invalid sctp stream 0\n");
    OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
  }

  nasNonDeliveryIndication_p = &message->msg.s1ap_NASNonDeliveryIndication_IEs;

  OAILOG_NOTICE (LOG_S1AP, "Received S1AP NAS_NON_DELIVERY_INDICATION message MME_UE_S1AP_ID " MME_UE_S1AP_ID_FMT " enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT "\n",
      (mme_ue_s1ap_id_t)nasNonDeliveryIndication_p->mme_ue_s1ap_id, (enb_ue_s1ap_id_t)nasNonDeliveryIndication_p->eNB_UE_S1AP_ID);

  MSC_LOG_RX_MESSAGE (MSC_S1AP_MME,
                      MSC_S1AP_ENB,
                      NULL, 0,
                      "0 NASNonDeliveryIndication/%s mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT " cause %u nas len %u",
                      s1ap_direction2String[message->direction],
                      (mme_ue_s1ap_id_t)nasNonDeliveryIndication_p->mme_ue_s1ap_id,
                      (enb_ue_s1ap_id_t)nasNonDeliveryIndication_p->eNB_UE_S1AP_ID,
                      nasNonDeliveryIndication_p->cause,
                      nasNonDeliveryIndication_p->nas_pdu.size);

  if ((ue_ref = s1ap_is_ue_mme_id_in_list (nasNonDeliveryIndication_p->mme_ue_s1ap_id))
      == NULL) {
    OAILOG_DEBUG (LOG_S1AP, "No UE is attached to this mme UE s1ap id: " MME_UE_S1AP_ID_FMT "\n", (mme_ue_s1ap_id_t)nasNonDeliveryIndication_p->mme_ue_s1ap_id);
    OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
  }

  if (ue_ref->s1_ue_state != S1AP_UE_CONNECTED) {
    OAILOG_DEBUG (LOG_S1AP, "Received S1AP NAS_NON_DELIVERY_INDICATION while UE in state != S1AP_UE_CONNECTED\n");
    OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
  }
  //TODO: forward NAS PDU to NAS
  s1ap_mme_itti_nas_non_delivery_ind (nasNonDeliveryIndication_p->mme_ue_s1ap_id,
                                      nasNonDeliveryIndication_p->nas_pdu.buf,
                                      nasNonDeliveryIndication_p->nas_pdu.size,
                                      &nasNonDeliveryIndication_p->cause);
  OAILOG_FUNC_RETURN (LOG_S1AP, RETURNok);
}

//------------------------------------------------------------------------------
int
s1ap_generate_downlink_nas_transport (
  const enb_ue_s1ap_id_t enb_ue_s1ap_id,
  const mme_ue_s1ap_id_t ue_id,
  STOLEN_REF bstring *payload)
{
  ue_description_t                       *ue_ref = NULL;
  uint8_t                                *buffer_p = NULL;
  uint32_t                                length = 0;
  void                                   *id = NULL;

  OAILOG_FUNC_IN (LOG_S1AP);

  // Try to retrieve SCTP assoication id using mme_ue_s1ap_id
  if (HASH_TABLE_OK ==  hashtable_ts_get (&g_s1ap_mme_id2assoc_id_coll, (const hash_key_t)ue_id, (void **)&id)) {
    sctp_assoc_id_t sctp_assoc_id = (sctp_assoc_id_t)(uintptr_t)id;
    enb_description_t  *enb_ref = s1ap_is_enb_assoc_id_in_list (sctp_assoc_id);
    if (enb_ref) {
      ue_ref = s1ap_is_ue_enb_id_in_list (enb_ref,enb_ue_s1ap_id);
    } else {
      OAILOG_ERROR (LOG_S1AP, "No eNB for SCTP association id %d \n", sctp_assoc_id);
      OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
    }
  }
  // TODO remove soon:
  if (!ue_ref) {
    ue_ref = s1ap_is_ue_mme_id_in_list (ue_id);
  }
  // finally!
  if (!ue_ref) {
    /*
     * If the UE-associated logical S1-connection is not established,
     * * * * the MME shall allocate a unique MME UE S1AP ID to be used for the UE.
     */
    OAILOG_WARNING (LOG_S1AP, "Unknown UE MME ID " MME_UE_S1AP_ID_FMT ", This case is not handled right now\n", ue_id);
    OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
  } else {
    /*
     * We have fount the UE in the list.
     * * * * Create new IE list message and encode it.
     */
    S1ap_DownlinkNASTransportIEs_t         *downlinkNasTransport = NULL;
    s1ap_message                            message = {0};

    message.procedureCode = S1ap_ProcedureCode_id_downlinkNASTransport;
    message.direction = S1AP_PDU_PR_initiatingMessage;
    ue_ref->s1_ue_state = S1AP_UE_CONNECTED;
    downlinkNasTransport = &message.msg.s1ap_DownlinkNASTransportIEs;
    /*
     * Setting UE informations with the ones fount in ue_ref
     */
    downlinkNasTransport->mme_ue_s1ap_id = ue_ref->mme_ue_s1ap_id;
    downlinkNasTransport->eNB_UE_S1AP_ID = ue_ref->enb_ue_s1ap_id;
    /*eNB
     * Fill in the NAS pdu
     */
    OCTET_STRING_fromBuf (&downlinkNasTransport->nas_pdu, (char *)bdata(*payload), blength(*payload));
    bdestroy(*payload);
    *payload = NULL;

    if (s1ap_mme_encode_pdu (&message, &buffer_p, &length) < 0) {
      // TODO: handle something
      OAILOG_FUNC_RETURN (LOG_S1AP, RETURNerror);
    }

    OAILOG_NOTICE (LOG_S1AP, "Send S1AP DOWNLINK_NAS_TRANSPORT message ue_id = " MME_UE_S1AP_ID_FMT " MME_UE_S1AP_ID = " MME_UE_S1AP_ID_FMT " eNB_UE_S1AP_ID = " ENB_UE_S1AP_ID_FMT "\n",
                ue_id, (mme_ue_s1ap_id_t)downlinkNasTransport->mme_ue_s1ap_id, (enb_ue_s1ap_id_t)downlinkNasTransport->eNB_UE_S1AP_ID);
    MSC_LOG_TX_MESSAGE (MSC_S1AP_MME,
                        MSC_S1AP_ENB,
                        NULL, 0,
                        "0 downlinkNASTransport/initiatingMessage ue_id " MME_UE_S1AP_ID_FMT " mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " enb_ue_s1ap_id" ENB_UE_S1AP_ID_FMT " nas length %u",
                        ue_id, (mme_ue_s1ap_id_t)downlinkNasTransport->mme_ue_s1ap_id, (enb_ue_s1ap_id_t)downlinkNasTransport->eNB_UE_S1AP_ID, length);
    bstring b = blk2bstr(buffer_p, length);
    s1ap_mme_itti_send_sctp_request (&b , ue_ref->enb->sctp_assoc_id, ue_ref->sctp_stream_send, ue_ref->mme_ue_s1ap_id);
  }

  OAILOG_FUNC_RETURN (LOG_S1AP, RETURNok);
}

//------------------------------------------------------------------------------
void
s1ap_handle_conn_est_cnf (
  const itti_mme_app_connection_establishment_cnf_t * const conn_est_cnf_pP)
{
  /*
   * We received create session response from S-GW on S11 interface abstraction.
   * At least one bearer has been established. We can now send s1ap initial context setup request
   * message to eNB.
   */
  uint                                    offset = 0;
  uint8_t                                *buffer_p = NULL;
  uint32_t                                length = 0;
  ue_description_t                       *ue_ref = NULL;
  S1ap_InitialContextSetupRequestIEs_t   *initialContextSetupRequest_p = NULL;
  S1ap_E_RABToBeSetupItemCtxtSUReq_t      e_RABToBeSetup = {0}; // yes, alloc on stack
  S1ap_NAS_PDU_t                          nas_pdu = {0}; // yes, alloc on stack
  s1ap_message                            message = {0}; // yes, alloc on stack

  OAILOG_FUNC_IN (LOG_S1AP);
  DevAssert (conn_est_cnf_pP != NULL);
  
   ue_ref = s1ap_is_ue_mme_id_in_list (conn_est_cnf_pP->nas_conn_est_cnf.ue_id);
  if (!ue_ref) {
    OAILOG_ERROR (LOG_S1AP, "This mme ue s1ap id (" MME_UE_S1AP_ID_FMT ") is not attached to any UE context\n", conn_est_cnf_pP->nas_conn_est_cnf.ue_id);
    // There are some race conditions were NAS T3450 timer is stopped and removed at same time
    OAILOG_FUNC_OUT (LOG_S1AP);
  }

  /*
   * Start the outcome response timer.
   * * * * When time is reached, MME consider that procedure outcome has failed.
   */
  //     timer_setup(mme_config.s1ap_config.outcome_drop_timer_sec, 0, TASK_S1AP, INSTANCE_DEFAULT,
  //                 TIMER_ONE_SHOT,
  //                 NULL,
  //                 &ue_ref->outcome_response_timer_id);
  /*
   * Insert the timer in the MAP of mme_ue_s1ap_id <-> timer_id
   */
  //     s1ap_timer_insert(ue_ref->mme_ue_s1ap_id, ue_ref->outcome_response_timer_id);
  message.procedureCode = S1ap_ProcedureCode_id_InitialContextSetup;
  message.direction = S1AP_PDU_PR_initiatingMessage;
  initialContextSetupRequest_p = &message.msg.s1ap_InitialContextSetupRequestIEs;
  initialContextSetupRequest_p->mme_ue_s1ap_id = (unsigned long)ue_ref->mme_ue_s1ap_id;
  initialContextSetupRequest_p->eNB_UE_S1AP_ID = (unsigned long)ue_ref->enb_ue_s1ap_id;

  /*
   * Only add capability information if it's not empty.
   */
  if (conn_est_cnf_pP->ue_radio_cap_length) {
    OAILOG_DEBUG (LOG_S1AP, "UE radio capability found, adding to message\n");
    initialContextSetupRequest_p->presenceMask |=
      S1AP_INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT;
    OCTET_STRING_fromBuf(&initialContextSetupRequest_p->ueRadioCapability,
                        (const char*) conn_est_cnf_pP->ue_radio_capabilities,
                         conn_est_cnf_pP->ue_radio_cap_length);
    free_wrapper((void**) &(conn_est_cnf_pP->ue_radio_capabilities));
  }

  /*
   * uEaggregateMaximumBitrateDL and uEaggregateMaximumBitrateUL expressed in term of bits/sec
   */
  asn_uint642INTEGER (&initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL, conn_est_cnf_pP->ambr.br_dl);
  asn_uint642INTEGER (&initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL, conn_est_cnf_pP->ambr.br_ul);
  e_RABToBeSetup.e_RAB_ID = conn_est_cnf_pP->eps_bearer_id;     //5;
  e_RABToBeSetup.e_RABlevelQoSParameters.qCI = conn_est_cnf_pP->bearer_qos_qci;

  if (conn_est_cnf_pP->nas_conn_est_cnf.nas_msg != NULL) {
    // NAS PDU is optional in rab_setup
    nas_pdu.size = conn_est_cnf_pP->nas_conn_est_cnf.nas_msg->slen;
    nas_pdu.buf  = conn_est_cnf_pP->nas_conn_est_cnf.nas_msg->data;
    e_RABToBeSetup.nAS_PDU = &nas_pdu;
  }
#  if ORIGINAL_S1AP_CODE
  e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel = S1ap_PriorityLevel_lowest;
  e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability = S1ap_Pre_emptionCapability_shall_not_trigger_pre_emption;
  e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability = S1ap_Pre_emptionVulnerability_not_pre_emptable;
#  else
  e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel = conn_est_cnf_pP->bearer_qos_prio_level;
  e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability = conn_est_cnf_pP->bearer_qos_pre_emp_capability;
  e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability = conn_est_cnf_pP->bearer_qos_pre_emp_vulnerability;
#  endif
  /*
   * Set the GTP-TEID. This is the S1-U S-GW TEID
   */
  INT32_TO_OCTET_STRING (conn_est_cnf_pP->bearer_s1u_sgw_fteid.teid, &e_RABToBeSetup.gTP_TEID);

  /*
   * S-GW IP address(es) for user-plane
   */
  if (conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv4) {
    e_RABToBeSetup.transportLayerAddress.buf = calloc (4, sizeof (uint8_t));
    /*
     * Only IPv4 supported
     */
    memcpy (e_RABToBeSetup.transportLayerAddress.buf, &conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv4_address, 4);
    offset += 4;
    e_RABToBeSetup.transportLayerAddress.size = 4;
    e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
  }

  if (conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv6) {
    if (offset == 0) {
      /*
       * Both IPv4 and IPv6 provided
       */
      /*
       * TODO: check memory allocation
       */
      e_RABToBeSetup.transportLayerAddress.buf = calloc (16, sizeof (uint8_t));
    } else {
      /*
       * Only IPv6 supported
       */
      /*
       * TODO: check memory allocation
       */
      e_RABToBeSetup.transportLayerAddress.buf = realloc (e_RABToBeSetup.transportLayerAddress.buf, (16 + offset) * sizeof (uint8_t));
    }

    memcpy (&e_RABToBeSetup.transportLayerAddress.buf[offset], conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv6_address, 16);
    e_RABToBeSetup.transportLayerAddress.size = 16 + offset;
    e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
  }

  ASN_SEQUENCE_ADD (&initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq, &e_RABToBeSetup);
  initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.buf = (uint8_t *) & conn_est_cnf_pP->security_capabilities_encryption_algorithms;
  initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.size = 2;
  initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.bits_unused = 0;
  initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.buf = (uint8_t *) & conn_est_cnf_pP->security_capabilities_integrity_algorithms;
  initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.size = 2;
  initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.bits_unused = 0;
  OAILOG_DEBUG (LOG_S1AP, "security_capabilities_encryption_algorithms 0x%04X\n", conn_est_cnf_pP->security_capabilities_encryption_algorithms);
  OAILOG_DEBUG (LOG_S1AP, "security_capabilities_integrity_algorithms 0x%04X\n", conn_est_cnf_pP->security_capabilities_integrity_algorithms);

  if (conn_est_cnf_pP->kenb) {
    initialContextSetupRequest_p->securityKey.buf = calloc (32, sizeof(uint8_t));
    memcpy (initialContextSetupRequest_p->securityKey.buf, conn_est_cnf_pP->kenb, 32);
    initialContextSetupRequest_p->securityKey.size = 32;
  } else {
    OAILOG_DEBUG (LOG_S1AP, "No kenb\n");
    initialContextSetupRequest_p->securityKey.buf = NULL;
    initialContextSetupRequest_p->securityKey.size = 0;
  }

  initialContextSetupRequest_p->securityKey.bits_unused = 0;

  if (s1ap_mme_encode_pdu (&message, &buffer_p, &length) < 0) {
    // TODO: handle something
    DevMessage ("Failed to encode initial context setup request message\n");
  }

  if (conn_est_cnf_pP->nas_conn_est_cnf.nas_msg != NULL) {
    bdestroy (conn_est_cnf_pP->nas_conn_est_cnf.nas_msg);
  }
  OAILOG_NOTICE (LOG_S1AP, "Send S1AP_INITIAL_CONTEXT_SETUP_REQUEST message MME_UE_S1AP_ID = " MME_UE_S1AP_ID_FMT " eNB_UE_S1AP_ID = " ENB_UE_S1AP_ID_FMT "\n",
              (mme_ue_s1ap_id_t)initialContextSetupRequest_p->mme_ue_s1ap_id, (enb_ue_s1ap_id_t)initialContextSetupRequest_p->eNB_UE_S1AP_ID);
  MSC_LOG_TX_MESSAGE (MSC_S1AP_MME,
                      MSC_S1AP_ENB,
                      NULL, 0,
                      "0 InitialContextSetup/initiatingMessage mme_ue_s1ap_id " MME_UE_S1AP_ID_FMT " enb_ue_s1ap_id " ENB_UE_S1AP_ID_FMT " nas length %u",
                      (mme_ue_s1ap_id_t)initialContextSetupRequest_p->mme_ue_s1ap_id,
                      (enb_ue_s1ap_id_t)initialContextSetupRequest_p->eNB_UE_S1AP_ID, nas_pdu.size);
  bstring b = blk2bstr(buffer_p, length);
  s1ap_mme_itti_send_sctp_request (&b, ue_ref->enb->sctp_assoc_id, ue_ref->sctp_stream_send, ue_ref->mme_ue_s1ap_id);
  OAILOG_FUNC_OUT (LOG_S1AP);
}
//------------------------------------------------------------------------------
void
s1ap_handle_mme_ue_id_notification (
  const itti_mme_app_s1ap_mme_ue_id_notification_t * const notification_p)
{

  OAILOG_FUNC_IN (LOG_S1AP);
  DevAssert (notification_p != NULL);
  s1ap_notified_new_ue_mme_s1ap_id_association (
                          notification_p->sctp_assoc_id, notification_p->enb_ue_s1ap_id, notification_p->mme_ue_s1ap_id);
  OAILOG_FUNC_OUT (LOG_S1AP);
}
