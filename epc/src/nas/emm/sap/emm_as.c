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

/*****************************************************************************

  Source      emm_as.c

  Version     0.1

  Date        2012/10/16

  Product     NAS stack

  Subsystem   EPS Mobility Management

  Author      Frederic Maurel

  Description Defines the EMMAS Service Access Point that provides
        services to the EPS Mobility Management for NAS message
        transfer to/from the Access Stratum sublayer.

*****************************************************************************/
#include <string.h>             // memset
#include <stdlib.h>             // malloc, free_wrapper

#include "dynamic_memory_check.h"
#include "log.h"
#include "msc.h"
#include "3gpp_requirements_24.301.h"
#include "emm_as.h"
#include "emm_recv.h"
#include "emm_send.h"
#include "emmData.h"
#include "commonDef.h"
#include "TLVDecoder.h"
#include "as_message.h"
#include "nas_message.h"
#include "emm_cause.h"
#include "LowerLayer.h"
#include "nas_itti_messaging.h"
#include "emm_proc.h"
#include "nas_proc.h"

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/


extern int                              emm_proc_status (
  mme_ue_s1ap_id_t ue_id,
  int emm_cause);

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/*
   String representation of EMMAS-SAP primitives
*/
static const char                      *_emm_as_primitive_str[] = {
  "EMMAS_SECURITY_REQ",
  "EMMAS_SECURITY_IND",
  "EMMAS_SECURITY_RES",
  "EMMAS_SECURITY_REJ",
  "EMMAS_ESTABLISH_REQ",
  "EMMAS_ESTABLISH_CNF",
  "EMMAS_ESTABLISH_REJ",
  "EMMAS_RELEASE_REQ",
  "EMMAS_RELEASE_IND",
  "EMMAS_DATA_REQ",
  "EMMAS_DATA_IND",
  "EMMAS_PAGE_IND",
  "EMMAS_STATUS_IND",
};

/*
   Functions executed to process EMM procedures upon receiving
   data from the network
*/
static int _emm_as_recv (
    mme_ue_s1ap_id_t ue_id,
    tai_t  const *originating_tai,
    ecgi_t  const *originating_ecgi,
    bstring msg,
    size_t len,
    int *emm_cause,
    nas_message_decode_status_t   * decode_status);


static int _emm_as_establish_req (const emm_as_establish_t * msg, int *emm_cause);
static int _emm_as_data_ind (const emm_as_data_t * msg, int *emm_cause);

/*
   Functions executed to send data to the network when requested
   within EMM procedure processing
*/
static EMM_msg *_emm_as_set_header (nas_message_t * msg, const emm_as_security_data_t * security);

static int _emm_as_encode (bstring *info, nas_message_t * msg, size_t length, emm_security_context_t * emm_security_context);

static int _emm_as_encrypt (bstring *info, const nas_message_security_header_t * header, const unsigned char *buffer,
    size_t length, emm_security_context_t * emm_security_context);

static int _emm_as_send (const emm_as_t * msg);
static int _emm_as_security_req (const emm_as_security_t *, dl_info_transfer_req_t *);
static int _emm_as_security_rej (const emm_as_security_t *, dl_info_transfer_req_t *);
static int _emm_as_establish_cnf (const emm_as_establish_t *, nas_establish_rsp_t *);
static int _emm_as_establish_rej (const emm_as_establish_t *, nas_establish_rsp_t *);
static int _emm_as_page_ind (const emm_as_page_t *, paging_req_t *);
static int _emm_as_data_req (const emm_as_data_t *, dl_info_transfer_req_t *);
static int _emm_as_status_ind (const emm_as_status_t *, dl_info_transfer_req_t *);
static int _emm_as_release_req (const emm_as_release_t *, nas_release_req_t *);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:    emm_as_initialize()                                       **
 **                                                                        **
 ** Description: Initializes the EMMAS Service Access Point                **
 **                                                                        **
 ** Inputs:  None                                                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    None                                       **
 **      Others:    NONE                                       **
 **                                                                        **
 ***************************************************************************/
void emm_as_initialize (void)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  /*
   * TODO: Initialize the EMMAS-SAP
   */
  OAILOG_FUNC_OUT (LOG_NAS_EMM);
}

/****************************************************************************
 **                                                                        **
 ** Name:    emm_as_send()                                             **
 **                                                                        **
 ** Description: Processes the EMMAS Service Access Point primitive.       **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int emm_as_send (const emm_as_t * msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNok;
  int                                     emm_cause = EMM_CAUSE_SUCCESS;
  emm_as_primitive_t                      primitive = msg->primitive;
  uint32_t                                ue_id = 0;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received primitive %s (%d)\n", _emm_as_primitive_str[primitive - _EMMAS_START - 1], primitive);

  switch (primitive) {
  case _EMMAS_DATA_IND:
    rc = _emm_as_data_ind (&msg->u.data, &emm_cause);
    ue_id = msg->u.data.ue_id;
    break;

  case _EMMAS_ESTABLISH_REQ:
    rc = _emm_as_establish_req (&msg->u.establish, &emm_cause);
    ue_id = msg->u.establish.ue_id;
    break;

  default:
    /*
     * Other primitives are forwarded to lower layers (S1AP)
     */
    rc = _emm_as_send (msg);

    if (rc != RETURNok) {
      OAILOG_ERROR (LOG_NAS_EMM, "EMMAS-SAP - " "Failed to process primitive %s (%d)\n", _emm_as_primitive_str[primitive - _EMMAS_START - 1], primitive);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
    }

    break;
  }

  /*
   * Handle decoding errors
   */
  if ((emm_cause != EMM_CAUSE_SUCCESS) && (emm_cause != EMM_CAUSE_UE_IDENTITY_CANT_BE_DERIVED_BY_NW)) {
    /*
     * Ignore received message that is too short to contain a complete
     * * * * message type information element
     */
    if (rc == TLV_BUFFER_TOO_SHORT) {
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
    }
    /*
     * Ignore received message that contains not supported protocol
     * * * * discriminator
     */
    else if (rc == TLV_PROTOCOL_NOT_SUPPORTED) {
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
    } else if (rc == TLV_WRONG_MESSAGE_TYPE) {
      emm_cause = EMM_CAUSE_MESSAGE_TYPE_NOT_IMPLEMENTED;
    }

    /*
     * EMM message processing failed
     */
    OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Received EMM message is not valid " "(cause=%d)\n", emm_cause);
    /*
     * Return an EMM status message
     */
    rc = emm_proc_status (ue_id, emm_cause);
  }

  if (rc != RETURNok) {
    OAILOG_ERROR (LOG_NAS_EMM, "EMMAS-SAP - Failed to process primitive %s (%d)\n", _emm_as_primitive_str[primitive - _EMMAS_START - 1], primitive);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/*
   --------------------------------------------------------------------------
   Functions executed to process EMM procedures upon receiving data from the
   network
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_recv()                                            **
 **                                                                        **
 ** Description: Decodes and processes the EPS Mobility Management message **
 **      received from the Access Stratum                          **
 **                                                                        **
 ** Inputs:  ue_id:      UE lower layer identifier                  **
 **      msg:       The EMM message to process                 **
 **      len:       The length of the EMM message              **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_recv (
  mme_ue_s1ap_id_t ue_id,
  tai_t  const *originating_tai,
  ecgi_t  const *originating_ecgi,
  bstring msg,
  size_t len,
  int *emm_cause,
  nas_message_decode_status_t   * decode_status)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  nas_message_decode_status_t             local_decode_status = {0};
  int                                     decoder_rc = RETURNok;
  int                                     rc = RETURNerror;
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};
  emm_security_context_t                 *emm_security_context = NULL;      /* Current EPS NAS security context     */

  if (decode_status) {
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received EMM message (length=%lu) integrity protected %d ciphered %d mac matched %d security context %d\n",
        len, decode_status->integrity_protected_message, decode_status->ciphered_message, decode_status->mac_matched, decode_status->security_context_available);
  } else {
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received EMM message (length=%lu)\n", len);
  }

  memset (&nas_msg,       0, sizeof (nas_msg));
  if (! decode_status) {
    memset (&local_decode_status, 0, sizeof (local_decode_status));
    decode_status = &local_decode_status;
  }

  emm_data_context_t     *emm_ctx =  emm_data_context_get (&_emm_data, ue_id);

  if (emm_ctx) {
    if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
      emm_security_context = &emm_ctx->_security;
    }
  }

  /*
   * Decode the received message
   */
  decoder_rc = nas_message_decode (msg->data, &nas_msg, len, emm_security_context, decode_status);

  if (decoder_rc < 0) {
    OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Failed to decode NAS message " "(err=%d)\n", decoder_rc);
    *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
  }

  /*
   * Process NAS message
   */
  EMM_msg                                *emm_msg = &nas_msg.plain.emm;

  switch (emm_msg->header.message_type) {
  case EMM_STATUS:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1);
    if ((0 == decode_status->security_context_available) ||
        (0 == decode_status->integrity_protected_message) ||
       // Requirement MME24.301R10_4.4.4.3_2
       ((1 == decode_status->security_context_available) && (0 == decode_status->mac_matched))) {
      *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
    }
    rc = emm_recv_status (ue_id, &emm_msg->emm_status, emm_cause, decode_status);
    break;

  case ATTACH_REQUEST:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1); // Integrity checking of NAS signalling messages in the MME
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__2); // Integrity checking of NAS signalling messages in the MME
    rc = emm_recv_attach_request (ue_id, originating_tai, originating_ecgi, &emm_msg->attach_request, emm_cause, decode_status);
    break;

  case IDENTITY_RESPONSE:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1); // Integrity checking of NAS signalling messages in the MME
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__2); // Integrity checking of NAS signalling messages in the MME
    rc = emm_recv_identity_response (ue_id, &emm_msg->identity_response, emm_cause, decode_status);
    break;

  case AUTHENTICATION_RESPONSE:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1); // Integrity checking of NAS signalling messages in the MME
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__2); // Integrity checking of NAS signalling messages in the MME
    rc = emm_recv_authentication_response (ue_id, &emm_msg->authentication_response, emm_cause, decode_status);
    break;

  case AUTHENTICATION_FAILURE:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1); // Integrity checking of NAS signalling messages in the MME
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__2); // Integrity checking of NAS signalling messages in the MME
    rc = emm_recv_authentication_failure (ue_id, &emm_msg->authentication_failure, emm_cause, decode_status);
    break;

  case SECURITY_MODE_COMPLETE:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1);
    if ((0 == decode_status->security_context_available) ||
        (0 == decode_status->integrity_protected_message) ||
       // Requirement MME24.301R10_4.4.4.3_2
       ((1 == decode_status->security_context_available) && (0 == decode_status->mac_matched))) {
      *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
    }

    rc = emm_recv_security_mode_complete (ue_id, &emm_msg->security_mode_complete, emm_cause, decode_status);
    break;

  case SECURITY_MODE_REJECT:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1); // Integrity checking of NAS signalling messages in the MME
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__2); // Integrity checking of NAS signalling messages in the MME
    rc = emm_recv_security_mode_reject (ue_id, &emm_msg->security_mode_reject, emm_cause, decode_status);
    break;

  case ATTACH_COMPLETE:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1);
    if ((0 == decode_status->security_context_available) ||
        (0 == decode_status->integrity_protected_message) ||
       // Requirement MME24.301R10_4.4.4.3_2
       ((1 == decode_status->security_context_available) && (0 == decode_status->mac_matched))) {
      *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
    }

    rc = emm_recv_attach_complete (ue_id, &emm_msg->attach_complete, emm_cause, decode_status);
    break;

  case TRACKING_AREA_UPDATE_COMPLETE:
  case GUTI_REALLOCATION_COMPLETE:
  case UPLINK_NAS_TRANSPORT:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1);
    if ((0 == decode_status->security_context_available) ||
        (0 == decode_status->integrity_protected_message) ||
       // Requirement MME24.301R10_4.4.4.3_2
       ((1 == decode_status->security_context_available) && (0 == decode_status->mac_matched))) {
      *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
    }

    /*
     * TODO
     */
    break;

  case DETACH_REQUEST:
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1);
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__2);
    if ((1 == decode_status->security_context_available) && (0 < emm_security_context->activated) &&
        ((0 == decode_status->integrity_protected_message) ||
       (0 == decode_status->mac_matched))) {
      *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
    }

    rc = emm_recv_detach_request (ue_id, &emm_msg->detach_request, emm_cause, decode_status);
    break;

  default:
    OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - EMM message 0x%x is not valid", emm_msg->header.message_type);
    *emm_cause = EMM_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE;
    break;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_data_ind()                                        **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP data transfer indication          **
 **      primitive                                                 **
 **                                                                        **
 ** EMMAS-SAP - AS->EMM: DATA_IND - Data transfer procedure                **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_data_ind (const emm_as_data_t * msg, int *emm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     rc = RETURNerror;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received AS data transfer indication " "(ue_id=" MME_UE_S1AP_ID_FMT ", delivered=%s, length=%d)\n",
      msg->ue_id, (msg->delivered) ? "true" : "false", blength(msg->nas_msg));

  if ( EMM_AS_DATA_DELIVERED_TRUE == msg->delivered) {
    if (blength(msg->nas_msg) > 0) {
      /*
       * Process the received NAS message
       */
      bstring                                   plain_msg = bstrcpy(msg->nas_msg);

      if (plain_msg) {
        nas_message_security_header_t           header = {0};
        emm_security_context_t                 *security = NULL;        /* Current EPS NAS security context     */
        nas_message_decode_status_t             decode_status = {0};

        /*
         * Decrypt the received security protected message
         */
        emm_data_context_t                     *emm_ctx = NULL;

        if (msg->ue_id > INVALID_MME_UE_S1AP_ID) {
          emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);
          if (emm_ctx) {
            if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
              security = &emm_ctx->_security;
            }
          }
        }

        int  bytes = nas_message_decrypt (msg->nas_msg->data,
            plain_msg->data,
            &header,
            blength(msg->nas_msg),
            security,
            &decode_status);

        if ((bytes < 0) &&
            (bytes != TLV_MAC_MISMATCH)) { // not in spec, (case identity response for attach with unknown GUTI)
          /*
           * Failed to decrypt the message
           */
          *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
          OAILOG_FUNC_RETURN (LOG_NAS_EMM, bytes);
        } else if (header.protocol_discriminator == EPS_MOBILITY_MANAGEMENT_MESSAGE) {
          /*
           * Process EMM data
           */
          tai_t                                   originating_tai = {.plmn = {0}, .tac = INVALID_TAC_0000}; // originating TAI
          originating_tai.tac = msg->tac;
          originating_tai.plmn.mcc_digit1 = msg->plmn_id->mcc_digit1;
          originating_tai.plmn.mcc_digit2 = msg->plmn_id->mcc_digit2;
          originating_tai.plmn.mcc_digit3 = msg->plmn_id->mcc_digit3;
          originating_tai.plmn.mnc_digit1 = msg->plmn_id->mnc_digit1;
          originating_tai.plmn.mnc_digit2 = msg->plmn_id->mnc_digit2;
          originating_tai.plmn.mnc_digit3 = msg->plmn_id->mnc_digit3;

          rc = _emm_as_recv (msg->ue_id, &originating_tai, &msg->ecgi, plain_msg, bytes, emm_cause, &decode_status);
        } else if (header.protocol_discriminator == EPS_SESSION_MANAGEMENT_MESSAGE) {
          /*
           * Foward ESM data to EPS session management
           */
          rc = lowerlayer_data_ind (msg->ue_id, plain_msg);
        }

        bdestroy (plain_msg);
      }
    } else {
      /*
       * Process successfull lower layer transfer indication
       */
      rc = lowerlayer_success (msg->ue_id);
    }
  } else if ( EMM_AS_DATA_DELIVERED_LOWER_LAYER_FAILURE == msg->delivered) {

    /*
     * Process lower layer transmission failure of NAS message
     */
    rc = lowerlayer_failure (msg->ue_id);
  } else {
    rc = lowerlayer_non_delivery_indication (msg->ue_id);
  }
  bdestroy(msg->nas_msg);
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_establish_req()                                   **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP connection establish request      **
 **      primitive                                                 **
 **                                                                        **
 ** EMMAS-SAP - AS->EMM: ESTABLISH_REQ - NAS signalling connection         **
 **     The AS notifies the NAS that establishment of the signal-  **
 **     ling connection has been requested to tranfer initial NAS  **
 **     message from the UE.                                       **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     emm_cause: EMM cause code                             **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_establish_req (const emm_as_establish_t * msg, int *emm_cause)
{
  struct emm_data_context_s              *emm_ctx = NULL;
  emm_security_context_t                 *emm_security_context = NULL;
  nas_message_decode_status_t             decode_status = {0};
  int                                     decoder_rc = 0;
  int                                     rc = RETURNerror;
  tai_t                                   originating_tai = {.plmn = {0}, .tac = INVALID_TAC_0000};

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Received AS connection establish request\n");
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};

  emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);

  if (emm_ctx) {
    OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - got context %p\n", emm_ctx);
    if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
      emm_security_context = &emm_ctx->_security;
    }
  }

  /*
   * Decode initial NAS message
   */
  decoder_rc = nas_message_decode (msg->nas_msg->data, &nas_msg, blength(msg->nas_msg), emm_security_context, &decode_status);
  bdestroy(msg->nas_msg);

  if (decoder_rc < TLV_FATAL_ERROR) {
    *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
  } else if (decoder_rc == TLV_UNEXPECTED_IEI) {
    *emm_cause = EMM_CAUSE_IE_NOT_IMPLEMENTED;
  } else if (decoder_rc < 0) {
    *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
  }

  /*
   * Process initial NAS message
   */
  EMM_msg                                *emm_msg = &nas_msg.plain.emm;

  switch (emm_msg->header.message_type) {
  case ATTACH_REQUEST:
    originating_tai.tac = msg->tac;
    originating_tai.plmn.mcc_digit1 = msg->plmn_id->mcc_digit1;
    originating_tai.plmn.mcc_digit2 = msg->plmn_id->mcc_digit2;
    originating_tai.plmn.mcc_digit3 = msg->plmn_id->mcc_digit3;
    originating_tai.plmn.mnc_digit1 = msg->plmn_id->mnc_digit1;
    originating_tai.plmn.mnc_digit2 = msg->plmn_id->mnc_digit2;
    originating_tai.plmn.mnc_digit3 = msg->plmn_id->mnc_digit3;
    rc = emm_recv_attach_request (msg->ue_id, &originating_tai, &msg->ecgi, &emm_msg->attach_request, emm_cause, &decode_status);
    break;

  case DETACH_REQUEST:
    if (emm_ctx == NULL) {
      /*
       * This means UE context is not present and this UE is not known in the EPC.
       * If message is Detach Request. Ignore the message.
       */
      *emm_cause = EMM_CAUSE_UE_IDENTITY_CANT_BE_DERIVED_BY_NW;

      //Clean up S1AP and MME UE Context 
      nas_itti_detach_req(msg->ue_id);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
    }
    
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__1);
    REQUIREMENT_3GPP_24_301(R10_4_4_4_3__2);
    if ((1 == decode_status.security_context_available) && 
                    (0 < emm_security_context->activated) &&
                      ((0 == decode_status.integrity_protected_message) ||
                                                (0 == decode_status.mac_matched))) {
      *emm_cause = EMM_CAUSE_UE_IDENTITY_CANT_BE_DERIVED_BY_NW;
      // Delete EMM,ESM conext, MMEAPP UE context and S1AP context
      nas_proc_implicit_detach_ue_ind(emm_ctx->ue_id);       
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
    }
    // Process Detach Request
    rc = emm_recv_detach_request (
      msg->ue_id, &emm_msg->detach_request, emm_cause, &decode_status);
    break;

  case TRACKING_AREA_UPDATE_REQUEST:
    // Check for emm_ctx and integrity verification 
    if ((emm_ctx == NULL) || 
            ((0 == decode_status.security_context_available) ||
                (0 == decode_status.integrity_protected_message) ||
                    ((1 == decode_status.security_context_available) && (0 == decode_status.mac_matched)))) {
      
      *emm_cause = EMM_CAUSE_UE_IDENTITY_CANT_BE_DERIVED_BY_NW;
      // Send Reject with cause "UE identity cannot be derived by the network" to trigger fresh attach 
      rc = emm_proc_tracking_area_update_reject (msg->ue_id, EMM_CAUSE_UE_IDENTITY_CANT_BE_DERIVED_BY_NW);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM,rc);
    }
    
    // Process periodic TAU   
    rc = emm_recv_tracking_area_update_request (msg->ue_id, &emm_msg->tracking_area_update_request, emm_cause, &decode_status);
    break;

  case SERVICE_REQUEST:
    // Requirement MME24.301R10_4.4.4.3_1
    if ((emm_ctx == NULL) || 
          ((0 == decode_status.security_context_available) ||
              (0 == decode_status.integrity_protected_message) ||
                // Requirement MME24.301R10_4.4.4.3_2
                    ((1 == decode_status.security_context_available) && (0 == decode_status.mac_matched)))) {
      
      *emm_cause = EMM_CAUSE_UE_IDENTITY_CANT_BE_DERIVED_BY_NW;
      // Send Service Reject with cause "UE identity cannot be derived by the network" to trigger fresh attach 
      rc = emm_proc_service_reject (msg->ue_id, EMM_CAUSE_UE_IDENTITY_CANT_BE_DERIVED_BY_NW);
      OAILOG_FUNC_RETURN (LOG_NAS_EMM,rc);
    }
    // Process Service request
    rc = emm_recv_service_request (msg->ue_id, &emm_msg->service_request, emm_cause, &decode_status);
    break;

  case EXTENDED_SERVICE_REQUEST:
    // Requirement MME24.301R10_4.4.4.3_1
    if ((0 == decode_status.security_context_available) ||
        (0 == decode_status.integrity_protected_message) ||
       // Requirement MME24.301R10_4.4.4.3_2
       ((1 == decode_status.security_context_available) && (0 == decode_status.mac_matched))) {
      *emm_cause = EMM_CAUSE_PROTOCOL_ERROR;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, decoder_rc);
    }

    OAILOG_ERROR (LOG_NAS_EMM, "EMMAS-SAP - Initial NAS message *****EXTENDED_SERVICE_REQUEST NOT SUPPORTED****\n");
    *emm_cause = EMM_CAUSE_MESSAGE_TYPE_NOT_IMPLEMENTED;
    rc = RETURNok;              /* TODO */
    break;

  default:
    OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Initial NAS message 0x%x is " "not valid\n", emm_msg->header.message_type);
    *emm_cause = EMM_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE;
    break;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, rc);
}


/*
   --------------------------------------------------------------------------
   Functions executed to send data to the network when requested within EMM
   procedure processing
   --------------------------------------------------------------------------
*/

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_set_header()                                      **
 **                                                                        **
 ** Description: Setup the security header of the given NAS message        **
 **                                                                        **
 ** Inputs:  security:  The NAS security data to use               **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The NAS message                            **
 **      Return:    Pointer to the plain NAS message to be se- **
 **             curity protected if setting of the securi- **
 **             ty header succeed;                         **
 **             NULL pointer otherwise                     **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static EMM_msg *_emm_as_set_header (nas_message_t * msg, const emm_as_security_data_t * security)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  msg->header.protocol_discriminator = EPS_MOBILITY_MANAGEMENT_MESSAGE;

  if (security && (security->ksi != KSI_NO_KEY_AVAILABLE)) {
    /*
     * A valid EPS security context exists
     */
    if (security->is_new) {
      /*
       * New EPS security context is taken into use
       */
      if (security->is_knas_int_present) {
        if (security->is_knas_enc_present) {
          /*
           * NAS integrity and cyphering keys are available
           */
          msg->header.security_header_type = SECURITY_HEADER_TYPE_INTEGRITY_PROTECTED_CYPHERED_NEW;
        } else {
          /*
           * NAS integrity key only is available
           */
          msg->header.security_header_type = SECURITY_HEADER_TYPE_INTEGRITY_PROTECTED_NEW;
        }

        OAILOG_FUNC_RETURN (LOG_NAS_EMM, &msg->security_protected.plain.emm);
      }
    } else if (security->is_knas_int_present) {
      if (security->is_knas_enc_present) {
        /*
         * NAS integrity and cyphering keys are available
         */
        msg->header.security_header_type = SECURITY_HEADER_TYPE_INTEGRITY_PROTECTED_CYPHERED;
      } else {
        /*
         * NAS integrity key only is available
         */
        msg->header.security_header_type = SECURITY_HEADER_TYPE_INTEGRITY_PROTECTED;
      }

      OAILOG_FUNC_RETURN (LOG_NAS_EMM, &msg->security_protected.plain.emm);
    } else {
      /*
       * No valid EPS security context exists
       */
      msg->header.security_header_type = SECURITY_HEADER_TYPE_NOT_PROTECTED;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, &msg->plain.emm);
    }
  } else {
    /*
     * No valid EPS security context exists
     */
    msg->header.security_header_type = SECURITY_HEADER_TYPE_NOT_PROTECTED;
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, &msg->plain.emm);
  }

  /*
   * A valid EPS security context exists but NAS integrity key
   * * * * is not available
   */
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, NULL);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_encode()                                          **
 **                                                                        **
 ** Description: Encodes NAS message into NAS information container        **
 **                                                                        **
 ** Inputs:  msg:       The NAS message to encode                  **
 **      length:    The maximum length of the NAS message      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     info:      The NAS information container              **
 **      msg:       The NAS message to encode                  **
 **      Return:    The number of bytes successfully encoded   **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_encode (bstring *info, nas_message_t * msg,
  size_t length, emm_security_context_t * emm_security_context)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     bytes = 0;

  if (msg->header.security_header_type != SECURITY_HEADER_TYPE_NOT_PROTECTED) {
    emm_msg_header_t                       *header = &msg->security_protected.plain.emm.header;

    /*
     * Expand size of protected NAS message
     */
    length += NAS_MESSAGE_SECURITY_HEADER_SIZE;
    /*
     * Set header of plain NAS message
     */
    header->protocol_discriminator = EPS_MOBILITY_MANAGEMENT_MESSAGE;
    header->security_header_type = SECURITY_HEADER_TYPE_NOT_PROTECTED;
  }

  /*
   * Allocate memory to the NAS information container
   */
  *info = bfromcstralloc(length, "\0");

  if (*info) {
    /*
     * Encode the NAS message
     */
    bytes = nas_message_encode ((*info)->data, msg, length, emm_security_context);

    if (bytes > 0) {
      (*info)->slen = bytes;
    } else {
      bdestroy (*info);
      *info = NULL;
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, bytes);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_encrypt()                                         **
 **                                                                        **
 ** Description: Encryts NAS message into NAS information container        **
 **                                                                        **
 ** Inputs:  header:    The Security header in used                **
 **      msg:       The NAS message to encrypt                 **
 **      length:    The maximum length of the NAS message      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     info:      The NAS information container              **
 **      Return:    The number of bytes successfully encrypted **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_encrypt (bstring *info, const nas_message_security_header_t * header,
  const unsigned char *msg, size_t length, emm_security_context_t * emm_security_context)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     bytes = 0;

  if (header->security_header_type != SECURITY_HEADER_TYPE_NOT_PROTECTED) {
    /*
     * Expand size of protected NAS message
     */
    length += NAS_MESSAGE_SECURITY_HEADER_SIZE;
  }

  /*
   * Allocate memory to the NAS information container
   */
  *info = bfromcstralloc(length, "\0");

  if (*info) {
    /*
     * Encrypt the NAS information message
     */
    bytes = nas_message_encrypt (msg, (*info)->data, header, length, emm_security_context);

    if (bytes > 0) {
      (*info)->slen = bytes;
    } else {
      bdestroy(*info);
      *info = NULL;
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, bytes);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_send()                                            **
 **                                                                        **
 ** Description: Builds NAS message according to the given EMMAS Service   **
 **      Access Point primitive and sends it to the Access Stratum **
 **      sublayer                                                  **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to be sent         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_send (const emm_as_t * msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  as_message_t                            as_msg = {0};

  switch (msg->primitive) {
  case _EMMAS_DATA_REQ:
    as_msg.msg_id = _emm_as_data_req (&msg->u.data, &as_msg.msg.dl_info_transfer_req);
    break;

  case _EMMAS_STATUS_IND:
    as_msg.msg_id = _emm_as_status_ind (&msg->u.status, &as_msg.msg.dl_info_transfer_req);
    break;

  case _EMMAS_RELEASE_REQ:
    as_msg.msg_id = _emm_as_release_req (&msg->u.release, &as_msg.msg.nas_release_req);
    break;

  case _EMMAS_SECURITY_REQ:
    as_msg.msg_id = _emm_as_security_req (&msg->u.security, &as_msg.msg.dl_info_transfer_req);
    break;

  case _EMMAS_SECURITY_REJ:
    as_msg.msg_id = _emm_as_security_rej (&msg->u.security, &as_msg.msg.dl_info_transfer_req);
    break;

  case _EMMAS_ESTABLISH_CNF:
    as_msg.msg_id = _emm_as_establish_cnf (&msg->u.establish, &as_msg.msg.nas_establish_rsp);
    break;

  case _EMMAS_ESTABLISH_REJ:
    as_msg.msg_id = _emm_as_establish_rej (&msg->u.establish, &as_msg.msg.nas_establish_rsp);
    break;

  case _EMMAS_PAGE_IND:
    as_msg.msg_id = _emm_as_page_ind (&msg->u.page, &as_msg.msg.paging_req);
    break;

  default:
    as_msg.msg_id = 0;
    break;
  }

  /*
   * Send the message to the Access Stratum or S1AP in case of MME
   */
  if (as_msg.msg_id > 0) {
    OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - " "Sending msg with id 0x%x, primitive %s (%d) to S1AP layer for transmission\n", as_msg.msg_id, _emm_as_primitive_str[msg->primitive - _EMMAS_START - 1], msg->primitive);

    switch (as_msg.msg_id) {
    case AS_DL_INFO_TRANSFER_REQ:{
        nas_itti_dl_data_req (as_msg.msg.dl_info_transfer_req.ue_id, as_msg.msg.dl_info_transfer_req.nas_msg,as_msg.msg.dl_info_transfer_req.err_code);
        OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
      }
      break;

    case AS_NAS_ESTABLISH_RSP:
    case AS_NAS_ESTABLISH_CNF:{
        if (as_msg.msg.nas_establish_rsp.err_code != AS_SUCCESS) {
          // This flow is to release the UE context after sending the NAS message. 
          nas_itti_dl_data_req (as_msg.msg.nas_establish_rsp.ue_id, as_msg.msg.nas_establish_rsp.nas_msg, as_msg.msg.nas_establish_rsp.err_code);
          OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
        } else {
          OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - Sending nas_itti_establish_cnf to S1AP UE ID 0x%x sea 0x%04X sia 0x%04X\n",
                     as_msg.msg.nas_establish_rsp.ue_id,
                     as_msg.msg.nas_establish_rsp.selected_encryption_algorithm,
                     as_msg.msg.nas_establish_rsp.selected_integrity_algorithm);
          /*
           * Handle success case
           */
          nas_itti_establish_cnf (as_msg.msg.nas_establish_rsp.ue_id,
                                  as_msg.msg.nas_establish_rsp.err_code,
                                  as_msg.msg.nas_establish_rsp.nas_msg,
                                  as_msg.msg.nas_establish_rsp.selected_encryption_algorithm,
                                  as_msg.msg.nas_establish_rsp.selected_integrity_algorithm);
          OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNok);
        }
      }
      break;

    case AS_NAS_RELEASE_REQ:
      break;

    default:
      break;
    }

  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, RETURNerror);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_data_req()                                        **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP data transfer request             **
 **      primitive                                                 **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: DATA_REQ - Data transfer procedure                **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_data_req (const emm_as_data_t * msg, dl_info_transfer_req_t * as_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = 0;
  int                                     is_encoded = false;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS data transfer request\n");
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};

  /*
   * Setup the AS message
   */
  if (msg->guti) {
    as_msg->s_tmsi.mme_code = msg->guti->gummei.mme_code;
    as_msg->s_tmsi.m_tmsi = msg->guti->m_tmsi;
  } else {
    as_msg->ue_id = msg->ue_id;
  }

  /*
   * Setup the NAS security header
   */
  EMM_msg                                *emm_msg = _emm_as_set_header (&nas_msg, &msg->sctx);

  /*
   * Setup the NAS information message
   */
  if (emm_msg )
    switch (msg->nas_info) {
    case EMM_AS_NAS_DATA_ATTACH_ACCEPT:
      size = emm_send_attach_accept_dl_nas (msg, &emm_msg->attach_accept);
      break;
    
    case EMM_AS_NAS_DATA_DETACH:
      size = emm_send_detach_accept (msg, &emm_msg->detach_accept);
      break;

    case EMM_AS_NAS_DATA_TAU: 
      size = emm_send_tracking_area_update_accept_dl_nas (msg, &emm_msg->tracking_area_update_accept);
      break;
    
    default:
      /*
       * Send other NAS messages as already encoded ESM messages
       */
      size = msg->nas_msg->slen;
      is_encoded = true;
      break;
    }

  if (size > 0) {
    int                                     bytes = 0;
    emm_security_context_t                 *emm_security_context = NULL;
    struct emm_data_context_s              *emm_ctx = NULL;

    emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);

    if (emm_ctx) {
      if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
        emm_security_context = &emm_ctx->_security;
      }
    }

    if (emm_security_context) {
      nas_msg.header.sequence_number = emm_security_context->dl_count.seq_num;
      OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.header.sequence_number -> %u\n", nas_msg.header.sequence_number);
    }

    if (!is_encoded) {
      /*
       * Encode the NAS information message
       */
      bytes = _emm_as_encode (&as_msg->nas_msg, &nas_msg, size, emm_security_context);
    } else {
      /*
       * Encrypt the NAS information message
       */
      bytes = _emm_as_encrypt (&as_msg->nas_msg, &nas_msg.header, msg->nas_msg->data, size, emm_security_context);
    }

    if (bytes > 0) {
      if (msg->nas_info == EMM_AS_NAS_DATA_TAU) {
        as_msg->err_code = AS_TERMINATED_NAS;
      } else {
        as_msg->err_code = AS_SUCCESS;
      }
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, AS_DL_INFO_TRANSFER_REQ);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_status_ind()                                      **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP status indication primitive       **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: STATUS_IND - EMM status report procedure          **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_status_ind (const emm_as_status_t * msg, dl_info_transfer_req_t * as_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = 0;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS status indication (cause=%d)\n", msg->emm_cause);
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};

  /*
   * Setup the AS message
   */
  if (msg->guti) {
    as_msg->s_tmsi.mme_code = msg->guti->gummei.mme_code;
    as_msg->s_tmsi.m_tmsi = msg->guti->m_tmsi;
  } else {
    as_msg->ue_id = msg->ue_id;
  }

  /*
   * Setup the NAS security header
   */
  EMM_msg                                *emm_msg = _emm_as_set_header (&nas_msg, &msg->sctx);

  /*
   * Setup the NAS information message
   */
  if (emm_msg ) {
    size = emm_send_status (msg, &emm_msg->emm_status);
  }

  if (size > 0) {
    emm_security_context_t                 *emm_security_context = NULL;
    struct emm_data_context_s              *emm_ctx = NULL;

    emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);

    if (emm_ctx) {
      if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
        emm_security_context = &emm_ctx->_security;
      }
    }

    if (emm_security_context) {
      nas_msg.header.sequence_number = emm_security_context->dl_count.seq_num;
      OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.header.sequence_number -> %u\n", nas_msg.header.sequence_number);
    }

    /*
     * Encode the NAS information message
     */
    int                                     bytes = _emm_as_encode (&as_msg->nas_msg,
                                                                    &nas_msg,
                                                                    size,
                                                                    emm_security_context);

    if (bytes > 0) {
      as_msg->err_code = AS_SUCCESS;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, AS_DL_INFO_TRANSFER_REQ);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_release_req()                                     **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP connection release request        **
 **      primitive                                                 **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: RELEASE_REQ - NAS signalling release procedure    **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_release_req (const emm_as_release_t * msg, nas_release_req_t * as_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS release request\n");

  /*
   * Setup the AS message
   */
  if (msg->guti) {
    as_msg->s_tmsi.mme_code = msg->guti->gummei.mme_code;
    as_msg->s_tmsi.m_tmsi = msg->guti->m_tmsi;
  } else {
    as_msg->ue_id = msg->ue_id;
  }

  if (msg->cause == EMM_AS_CAUSE_AUTHENTICATION) {
    as_msg->cause = AS_AUTHENTICATION_FAILURE;
  } else if (msg->cause == EMM_AS_CAUSE_DETACH) {
    as_msg->cause = AS_DETACH;
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, AS_NAS_RELEASE_REQ);
}


/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_security_req()                                    **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP security request primitive        **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: SECURITY_REQ - Security mode control procedure    **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_security_req (const emm_as_security_t * msg, dl_info_transfer_req_t * as_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = 0;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS security request\n");
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};

  /*
   * Setup the AS message
   */
  if (msg->guti) {
    as_msg->s_tmsi.mme_code = msg->guti->gummei.mme_code;
    as_msg->s_tmsi.m_tmsi = msg->guti->m_tmsi;
  } else {
    as_msg->ue_id = msg->ue_id;
  }
  /*
   * Setup the NAS security header
   */
  EMM_msg                                *emm_msg = _emm_as_set_header (&nas_msg, &msg->sctx);

  /*
   * Setup the NAS security message
   */
  if (emm_msg )
    switch (msg->msg_type) {
    case EMM_AS_MSG_TYPE_IDENT:
      if (msg->guti) {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send IDENTITY_REQUEST to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      } else {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send IDENTITY_REQUEST to ue id " MME_UE_S1AP_ID_FMT " ", as_msg->ue_id);
      }

      size = emm_send_identity_request (msg, &emm_msg->identity_request);
      break;

    case EMM_AS_MSG_TYPE_AUTH:
      if (msg->guti) {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send AUTHENTICATION_REQUEST to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      } else {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send AUTHENTICATION_REQUEST to ue id " MME_UE_S1AP_ID_FMT " ", as_msg->ue_id);
      }

      size = emm_send_authentication_request (msg, &emm_msg->authentication_request);
      break;

    case EMM_AS_MSG_TYPE_SMC:
      if (msg->guti) {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send SECURITY_MODE_COMMAND to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      } else {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send SECURITY_MODE_COMMAND to ue id " MME_UE_S1AP_ID_FMT " ", as_msg->ue_id);
      }

      size = emm_send_security_mode_command (msg, &emm_msg->security_mode_command);
      break;

    default:
      OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Type of NAS security " "message 0x%.2x is not valid\n", msg->msg_type);
    }

  if (size > 0) {
    struct emm_data_context_s              *emm_ctx = NULL;
    emm_security_context_t                 *emm_security_context = NULL;

    emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);

    if (emm_ctx) {

      if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
        emm_security_context = &emm_ctx->_security;
        nas_msg.header.sequence_number = emm_ctx->_security.dl_count.seq_num;
        OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.header.sequence_number -> %u\n", nas_msg.header.sequence_number);
      }
    }

    /*
     * Encode the NAS security message
     */
    int                                     bytes = _emm_as_encode (&as_msg->nas_msg,
                                                                    &nas_msg,
                                                                    size,
                                                                    emm_security_context);

    if (bytes > 0) {
      as_msg->err_code = AS_SUCCESS;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, AS_DL_INFO_TRANSFER_REQ);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_security_rej()                                    **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP security reject primitive         **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: SECURITY_REJ - Security mode control procedure    **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_security_rej (const emm_as_security_t * msg, dl_info_transfer_req_t * as_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     size = 0;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS security reject\n");
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};

  /*
   * Setup the AS message
   */
  if (msg->guti) {
    as_msg->s_tmsi.mme_code = msg->guti->gummei.mme_code;
    as_msg->s_tmsi.m_tmsi = msg->guti->m_tmsi;
  } else {
    as_msg->ue_id = msg->ue_id;
  }
  
  /*
   * Setup the NAS security header
   */
  EMM_msg                                *emm_msg = _emm_as_set_header (&nas_msg, &msg->sctx);

  /*
   * Setup the NAS security message
   */
  if (emm_msg )
    switch (msg->msg_type) {
    case EMM_AS_MSG_TYPE_AUTH:
      if (msg->guti) {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send AUTHENTICATION_REJECT to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      } else {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send AUTHENTICATION_REJECT to ue id " MME_UE_S1AP_ID_FMT " ", as_msg->ue_id);
      }

      size = emm_send_authentication_reject (&emm_msg->authentication_reject);
      break;

    default:
      OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Type of NAS security " "message 0x%.2x is not valid\n", msg->msg_type);
    }

  if (size > 0) {
    struct emm_data_context_s              *emm_ctx = NULL;
    emm_security_context_t                 *emm_security_context = NULL;

    emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);


    if (emm_ctx) {
      if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
        emm_security_context = &emm_ctx->_security;
        nas_msg.header.sequence_number = emm_security_context->dl_count.seq_num;
        OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.header.sequence_number -> %u\n", nas_msg.header.sequence_number);
      } else {
        OAILOG_DEBUG (LOG_NAS_EMM, "No security context, not set nas_msg.header.sequence_number -> %u\n", nas_msg.header.sequence_number);
      }
    }

    /*
     * Encode the NAS security message
     */
    int                                     bytes = _emm_as_encode (&as_msg->nas_msg,
                                                                    &nas_msg,
                                                                    size,
                                                                    emm_security_context);

    if (bytes > 0) {
      /* 
       *  Indicate to lower layer that procedure needs to be terminated after sending DL NAS message.
       */
      as_msg->err_code = AS_TERMINATED_NAS;
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, AS_DL_INFO_TRANSFER_REQ);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_establish_cnf()                                   **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP connection establish confirm      **
 **      primitive                                                 **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: ESTABLISH_CNF - NAS signalling connection         **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_establish_cnf (const emm_as_establish_t * msg, nas_establish_rsp_t * as_msg)
{
  EMM_msg                                *emm_msg = NULL;
  int                                     size = 0;
  int                                     ret_val = 0;

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS connection establish confirmation\n");
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};
  /*
   * Setup the AS message
   */
  as_msg->ue_id = msg->ue_id;

  if (msg->eps_id.guti == NULL) {
    OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - GUTI is NULL...");
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, ret_val);
  }

  as_msg->s_tmsi.mme_code = msg->eps_id.guti->gummei.mme_code;
  as_msg->s_tmsi.m_tmsi = msg->eps_id.guti->m_tmsi;
  as_msg->nas_msg = msg->nas_msg; 
  
  struct emm_data_context_s              *emm_ctx = NULL;
  emm_security_context_t                 *emm_security_context = NULL;
  emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);
  if (emm_ctx) {
    if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
      emm_security_context = &emm_ctx->_security;
      as_msg->selected_encryption_algorithm = (uint16_t) htons(0x10000 >> emm_security_context->selected_algorithms.encryption);
      as_msg->selected_integrity_algorithm  = (uint16_t) htons(0x10000 >> emm_security_context->selected_algorithms.integrity);
      OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.selected_encryption_algorithm -> NBO: 0x%04X (%u)\n", as_msg->selected_encryption_algorithm, emm_security_context->selected_algorithms.encryption);
      OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.selected_integrity_algorithm -> NBO: 0x%04X (%u)\n", as_msg->selected_integrity_algorithm, emm_security_context->selected_algorithms.integrity);
      as_msg->nas_ul_count = 0x00000000 | (emm_security_context->ul_count.overflow << 8) | emm_security_context->ul_count.seq_num;  // This is sent to calculate KeNB 
      OAILOG_DEBUG (LOG_NAS_EMM, "EMMAS-SAP - NAS UL COUNT %8x\n", as_msg->nas_ul_count);
    }
  }
  switch (msg->nas_info) {
  case EMM_AS_NAS_INFO_ATTACH:
    /*
     * Setup the NAS security header
     */
    emm_msg = _emm_as_set_header (&nas_msg, &msg->sctx);
    if (emm_msg) {
      OAILOG_TRACE (LOG_NAS_EMM, "EMMAS-SAP - emm_as_establish.nasMSG.length=%d\n", msg->nas_msg->slen);
      MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send ATTACH_ACCEPT to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      size = emm_send_attach_accept (msg, &emm_msg->attach_accept);
    }
    break;
    
  case EMM_AS_NAS_INFO_TAU:
    /*
     * Setup the NAS security header
     */
    emm_msg = _emm_as_set_header (&nas_msg, &msg->sctx);
    if (emm_msg) {
      MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send TAU_ACCEPT to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      size = emm_send_tracking_area_update_accept (msg, &emm_msg->tracking_area_update_accept);
    }
    break;
  case EMM_AS_NAS_INFO_NONE:  //Response to SR
    as_msg->err_code = AS_SUCCESS;
    ret_val = AS_NAS_ESTABLISH_CNF;
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, ret_val);
  default:
    OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Type of initial NAS " "message 0x%.2x is not valid\n", msg->nas_info);
    break;
  } 

  if (size > 0) {
    nas_msg.header.sequence_number = emm_security_context->dl_count.seq_num;
    OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.header.sequence_number -> %u\n", nas_msg.header.sequence_number);
  } else {
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, ret_val);
  }
  /*
   * Encode the initial NAS information message
   */
  int bytes = _emm_as_encode (&as_msg->nas_msg, &nas_msg, size, emm_security_context);

  if (bytes > 0) {
    as_msg->err_code = AS_SUCCESS;
    ret_val = AS_NAS_ESTABLISH_CNF;
  }
  OAILOG_FUNC_RETURN (LOG_NAS_EMM, ret_val);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_establish_rej()                                   **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP connection establish reject       **
 **      primitive                                                 **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: ESTABLISH_REJ - NAS signalling connection         **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_establish_rej (const emm_as_establish_t * msg, nas_establish_rsp_t * as_msg)
{
  EMM_msg                                *emm_msg = NULL;
  int                                     size = 0;
  nas_message_t                           nas_msg = {.security_protected.header = {0},
                                                     .security_protected.plain.emm.header = {0},
                                                     .security_protected.plain.esm.header = {0}};

  OAILOG_FUNC_IN (LOG_NAS_EMM);
  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS connection establish reject\n");

  /*
   * Setup the AS message
   */
  if (msg->eps_id.guti) {
    as_msg->s_tmsi.mme_code = msg->eps_id.guti->gummei.mme_code;
    as_msg->s_tmsi.m_tmsi = msg->eps_id.guti->m_tmsi;
  } else {
    as_msg->ue_id = msg->ue_id;
  }

  /*
   * Setup the NAS security header
   */
  emm_msg = _emm_as_set_header (&nas_msg, &msg->sctx);

  /*
   * Setup the NAS information messag
   */
  if (emm_msg) {
    switch (msg->nas_info) {
    case EMM_AS_NAS_INFO_ATTACH:
      if (msg->eps_id.guti) {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send ATTACH_REJECT to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      } else {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send ATTACH_REJECT to ue id " MME_UE_S1AP_ID_FMT " ", as_msg->ue_id);
      }

      size = emm_send_attach_reject (msg, &emm_msg->attach_reject);
      break;

    case EMM_AS_NAS_INFO_TAU:
      if (msg->eps_id.guti) {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send TRACKING_AREA_UPDATE_REJECT to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      } else {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send TRACKING_AREA_UPDATE_REJECT to ue id " MME_UE_S1AP_ID_FMT " ", as_msg->ue_id);
      }

      size = emm_send_tracking_area_update_reject (msg, &emm_msg->tracking_area_update_reject);
      break;

    case EMM_AS_NAS_INFO_SR:
      if (msg->eps_id.guti) {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send SERVICE_REJECT to s_TMSI %u.%u ", as_msg->s_tmsi.mme_code, as_msg->s_tmsi.m_tmsi);
      } else {
        MSC_LOG_EVENT (MSC_NAS_EMM_MME, "send SERVICE_REJECT to ue id " MME_UE_S1AP_ID_FMT " ", as_msg->ue_id);
      }

      size = emm_send_service_reject (msg, &emm_msg->service_reject);
      break;

    default:
      OAILOG_WARNING (LOG_NAS_EMM, "EMMAS-SAP - Type of initial NAS " "message 0x%.2x is not valid\n", msg->nas_info);
    }
  }

  if (size > 0) {
    struct emm_data_context_s              *emm_ctx = NULL;
    emm_security_context_t                 *emm_security_context = NULL;

    emm_ctx = emm_data_context_get (&_emm_data, msg->ue_id);

    if (emm_ctx) {
      if (IS_EMM_CTXT_PRESENT_SECURITY(emm_ctx)) {
        emm_security_context = &emm_ctx->_security;
        nas_msg.header.sequence_number = emm_security_context->dl_count.seq_num;
        OAILOG_DEBUG (LOG_NAS_EMM, "Set nas_msg.header.sequence_number -> %u\n", nas_msg.header.sequence_number);
      }
    }

    /*
     * Encode the initial NAS information message
     */
    int bytes = _emm_as_encode (&as_msg->nas_msg,
                                               &nas_msg,
                                               size,
                                               emm_security_context);
    if (bytes > 0) {
      // This is to indicate MME-APP to release the S1AP UE context after sending the message.
      as_msg->err_code = AS_TERMINATED_NAS; 
      OAILOG_FUNC_RETURN (LOG_NAS_EMM, AS_NAS_ESTABLISH_RSP);
    }
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);
}

/****************************************************************************
 **                                                                        **
 ** Name:    _emm_as_page_ind()                                        **
 **                                                                        **
 ** Description: Processes the EMMAS-SAP paging data indication primitive  **
 **                                                                        **
 ** EMMAS-SAP - EMM->AS: PAGE_IND - Paging data procedure                  **
 **                                                                        **
 ** Inputs:  msg:       The EMMAS-SAP primitive to process         **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     as_msg:    The message to send to the AS              **
 **      Return:    The identifier of the AS message           **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
static int _emm_as_page_ind (const emm_as_page_t * msg, paging_req_t * as_msg)
{
  OAILOG_FUNC_IN (LOG_NAS_EMM);
  int                                     bytes = 0;

  OAILOG_INFO (LOG_NAS_EMM, "EMMAS-SAP - Send AS data paging indication\n");

  /*
   * TODO
   */

  if (bytes > 0) {
    OAILOG_FUNC_RETURN (LOG_NAS_EMM, AS_PAGING_IND);
  }

  OAILOG_FUNC_RETURN (LOG_NAS_EMM, 0);
}

