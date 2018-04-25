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
  Source      esm_send.c

  Version     0.1

  Date        2013/02/11

  Product     NAS stack

  Subsystem   EPS Session Management

  Author      Frederic Maurel

  Description Defines functions executed at the ESM Service Access
        Point to send EPS Session Management messages to the
        EPS Mobility Management sublayer.

*****************************************************************************/

#include <string.h>             // strlen
#include "log.h"
#include "commonDef.h"
#include "3gpp_24.007.h"
#include "3gpp_24.301.h"
#include "esm_send.h"
#include "esm_msgDef.h"
#include "esm_cause.h"


/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
   --------------------------------------------------------------------------
   Functions executed by both the UE and the MME to send ESM messages
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_status()                                         **
 **                                                                        **
 ** Description: Builds ESM status message                                 **
 **                                                                        **
 **      The ESM status message is sent by the network or the UE   **
 **      to pass information on the status of the indicated EPS    **
 **      bearer context and report certain error conditions.       **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_send_status (
  int pti,
  int ebi,
  esm_status_msg * msg,
  int esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  /*
   * Mandatory - ESM message header
   */
  msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
  msg->epsbeareridentity = ebi;
  msg->messagetype = ESM_STATUS;
  msg->proceduretransactionidentity = pti;
  /*
   * Mandatory - ESM cause code
   */
  msg->esmcause = esm_cause;
  OAILOG_WARNING (LOG_NAS_ESM, "ESM-SAP   - Send ESM Status message (pti=%d, ebi=%d)\n", msg->proceduretransactionidentity, msg->epsbeareridentity);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}


/*
   --------------------------------------------------------------------------
   Functions executed by the MME to send ESM message to the UE
   --------------------------------------------------------------------------
*/
/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_pdn_connectivity_reject()                        **
 **                                                                        **
 ** Description: Builds PDN Connectivity Reject message                    **
 **                                                                        **
 **      The PDN connectivity reject message is sent by the net-   **
 **      work to the UE to reject establishment of a PDN connec-   **
 **      tion.                                                     **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_send_pdn_connectivity_reject (
  int pti,
  pdn_connectivity_reject_msg * msg,
  int esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  /*
   * Mandatory - ESM message header
   */
  msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
  msg->epsbeareridentity = EPS_BEARER_IDENTITY_UNASSIGNED;
  msg->messagetype = PDN_CONNECTIVITY_REJECT;
  msg->proceduretransactionidentity = pti;
  /*
   * Mandatory - ESM cause code
   */
  msg->esmcause = esm_cause;
  /*
   * Optional IEs
   */
  msg->presencemask = 0;
  OAILOG_DEBUG (LOG_NAS_ESM, "ESM-SAP   - Send PDN Connectivity Reject message " "(pti=%d, ebi=%d)\n", msg->proceduretransactionidentity, msg->epsbeareridentity);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_pdn_disconnect_reject()                          **
 **                                                                        **
 ** Description: Builds PDN Disconnect Reject message                      **
 **                                                                        **
 **      The PDN disconnect reject message is sent by the network  **
 **      to the UE to reject release of a PDN connection.          **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_send_pdn_disconnect_reject (
  int pti,
  pdn_disconnect_reject_msg * msg,
  int esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  /*
   * Mandatory - ESM message header
   */
  msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
  msg->epsbeareridentity = EPS_BEARER_IDENTITY_UNASSIGNED;
  msg->messagetype = PDN_DISCONNECT_REJECT;
  msg->proceduretransactionidentity = pti;
  /*
   * Mandatory - ESM cause code
   */
  msg->esmcause = esm_cause;
  /*
   * Optional IEs
   */
  msg->presencemask = 0;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - Send PDN Disconnect Reject message " "(pti=%d, ebi=%d)\n", msg->proceduretransactionidentity, msg->epsbeareridentity);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_default_eps_bearer_context_request()    **
 **                                                                        **
 ** Description: Builds Activate Default EPS Bearer Context Request        **
 **      message                                                   **
 **                                                                        **
 **      The activate default EPS bearer context request message   **
 **      is sent by the network to the UE to request activation of **
 **      a default EPS bearer context.                             **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      qos:       Subscribed EPS quality of service          **
 **      apn:       Access Point Name in used                  **
 **      pdn_addr:  PDN IPv4 address and/or IPv6 suffix        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_send_activate_default_eps_bearer_context_request (
  int pti,
  int ebi,
  activate_default_eps_bearer_context_request_msg * msg,
  bstring apn,
  const ProtocolConfigurationOptions * pco,
  int pdn_type,
  bstring pdn_addr,
  const EpsQualityOfService * qos,
  int esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  /*
   * Mandatory - ESM message header
   */
  msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
  msg->epsbeareridentity = ebi;
  msg->messagetype = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST;
  msg->proceduretransactionidentity = pti;
  /*
   * Mandatory - EPS QoS
   */
  msg->epsqos = *qos;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  qci:  %u\n", qos->qci);

  if (qos->bitRatesPresent) {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  maxBitRateForUL:  %u\n", qos->bitRates.maxBitRateForUL);
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  maxBitRateForDL:  %u\n", qos->bitRates.maxBitRateForDL);
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  guarBitRateForUL: %u\n", qos->bitRates.guarBitRateForUL);
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  guarBitRateForDL: %u\n", qos->bitRates.guarBitRateForDL);
  } else {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  no bit rates defined\n");
  }

  if (qos->bitRatesExtPresent) {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  maxBitRateForUL  Ext: %u\n", qos->bitRatesExt.maxBitRateForUL);
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  maxBitRateForDL  Ext: %u\n", qos->bitRatesExt.maxBitRateForDL);
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  guarBitRateForUL Ext: %u\n", qos->bitRatesExt.guarBitRateForUL);
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  guarBitRateForDL Ext: %u\n", qos->bitRatesExt.guarBitRateForDL);
  } else {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - epsqos  no bit rates ext defined\n");
  }

  if (apn == NULL) {
    OAILOG_WARNING (LOG_NAS_ESM, "ESM-SAP   - apn is NULL!\n");
  } else {
    OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - apn is %s\n", bdata(apn));
  }
  /*
   * Mandatory - Access Point Name
   */
  msg->accesspointname = apn;
  /*
   * Mandatory - PDN address
   */
  OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - pdn_type is %u\n", pdn_type);
  msg->pdnaddress.pdntypevalue = pdn_type;
  OAILOG_STREAM_HEX (OAILOG_LEVEL_DEBUG, LOG_NAS_ESM, "ESM-SAP   - pdn_addr is ", bdata(pdn_addr), blength(pdn_addr));
  msg->pdnaddress.pdnaddressinformation = pdn_addr;
  /*
   * Optional - ESM cause code
   */
  msg->presencemask = 0;

  if (esm_cause != ESM_CAUSE_SUCCESS) {
    msg->presencemask |= ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_ESM_CAUSE_PRESENT;
    msg->esmcause = esm_cause;
  }

  if (pco) {
    msg->presencemask |= ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
    copy_protocol_configuration_options(&msg->protocolconfigurationoptions, pco);
  }
//#pragma message  "TEST LG FORCE APN-AMBR"
  OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - FORCE APN-AMBR\n");
  msg->presencemask |= ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_APNAMBR_PRESENT;
  // APN AMBR is hardcoded to DL AMBR = 200 Mbps and UL APN MBR = 100 Mbps - Which is ok for now for TDD 20 MHz
  // TODO task#14477798 - need to change these to apm-subscribed values 
  msg->apnambr.apnambrfordownlink = 0xfe;       // (8640kbps)
  msg->apnambr.apnambrforuplink = 0xfe; // (8640kbps)
  msg->apnambr.apnambrfordownlink_extended = 0xde;      // (200Mbps)
  msg->apnambr.apnambrforuplink_extended = 0x9e;        // (100Mbps)
  msg->apnambr.apnambrfordownlink_extended2 = 0;
  msg->apnambr.apnambrforuplink_extended2 = 0;
  msg->apnambr.extensions = 0 | APN_AGGREGATE_MAXIMUM_BIT_RATE_MAXIMUM_EXTENSION_PRESENT;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - Send Activate Default EPS Bearer Context " "Request message (pti=%d, ebi=%d)\n",
      msg->proceduretransactionidentity, msg->epsbeareridentity);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_activate_dedicated_eps_bearer_context_request()  **
 **                                                                        **
 ** Description: Builds Activate Dedicated EPS Bearer Context Request      **
 **      message                                                   **
 **                                                                        **
 **      The activate dedicated EPS bearer context request message **
 **      is sent by the network to the UE to request activation of **
 **      a dedicated EPS bearer context associated with the same   **
 **      PDN address(es) and APN as an already active default EPS  **
 **      bearer context.                                           **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      linked_ebi:    EPS bearer identity of the default bearer  **
 **             associated with the EPS dedicated bearer   **
 **             to be activated                            **
 **      qos:       EPS quality of service                     **
 **      tft:       Traffic flow template                      **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_send_activate_dedicated_eps_bearer_context_request (
  int pti,
  int ebi,
  activate_dedicated_eps_bearer_context_request_msg * msg,
  int linked_ebi,
  const EpsQualityOfService * qos,
  PacketFilters * pkfs,
  int n_pkfs)
{
  int                                     i;

  OAILOG_FUNC_IN (LOG_NAS_ESM);
  /*
   * Mandatory - ESM message header
   */
  msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
  msg->epsbeareridentity = ebi;
  msg->messagetype = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST;
  msg->proceduretransactionidentity = pti;
  /*
   * Mandatory - EPS QoS
   */
  msg->epsqos = *qos;
  /*
   * Mandatory - traffic flow template
   */
  msg->tft.tftoperationcode = TRAFFIC_FLOW_TEMPLATE_OPCODE_CREATE;
  msg->tft.ebit = TRAFFIC_FLOW_TEMPLATE_PARAMETER_LIST_IS_NOT_INCLUDED;
  msg->tft.numberofpacketfilters = n_pkfs;

  for (i = 0; i < msg->tft.numberofpacketfilters; i++) {
    msg->tft.packetfilterlist.createtft[i] = (*pkfs)[i];
  }

  /*
   * Optional
   */
  msg->presencemask = 0;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - Send Activate Dedicated EPS Bearer Context " "Request message (pti=%d, ebi=%d)\n", msg->proceduretransactionidentity, msg->epsbeareridentity);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:    esm_send_deactivate_eps_bearer_context_request()          **
 **                                                                        **
 ** Description: Builds Deactivate EPS Bearer Context Request message      **
 **                                                                        **
 **      The deactivate EPS bearer context request message is sent **
 **      by the network to request deactivation of an active EPS   **
 **      bearer context.                                           **
 **                                                                        **
 ** Inputs:  pti:       Procedure transaction identity             **
 **      ebi:       EPS bearer identity                        **
 **      esm_cause: ESM cause code                             **
 **      Others:    None                                       **
 **                                                                        **
 ** Outputs:     msg:       The ESM message to be sent                 **
 **      Return:    RETURNok, RETURNerror                      **
 **      Others:    None                                       **
 **                                                                        **
 ***************************************************************************/
int
esm_send_deactivate_eps_bearer_context_request (
  int pti,
  int ebi,
  deactivate_eps_bearer_context_request_msg * msg,
  int esm_cause)
{
  OAILOG_FUNC_IN (LOG_NAS_ESM);
  /*
   * Mandatory - ESM message header
   */
  msg->protocoldiscriminator = EPS_SESSION_MANAGEMENT_MESSAGE;
  msg->epsbeareridentity = ebi;
  msg->messagetype = DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST;
  msg->proceduretransactionidentity = pti;
  /*
   * Mandatory - ESM cause code
   */
  msg->esmcause = esm_cause;
  /*
   * Optional IEs
   */
  msg->presencemask = 0;
  OAILOG_INFO (LOG_NAS_ESM, "ESM-SAP   - Send Deactivate EPS Bearer Context Request " "message (pti=%d, ebi=%d)\n", msg->proceduretransactionidentity, msg->epsbeareridentity);
  OAILOG_FUNC_RETURN (LOG_NAS_ESM, RETURNok);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/
