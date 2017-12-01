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

/*******************************************************************************
 * This file had been created by asn1tostruct.py script v1.0.2
 * Please do not modify this file but regenerate it via script.
 * Created on: 2017-11-28 16:02:51.568056 by spencer
 * from ['/home/spencer/Desktop/epc-dolte/src/s1ap/messages/asn1/r10.5/S1AP-PDU-Contents.asn']
 ******************************************************************************/
#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "log.h"

int s1ap_decode_s1ap_deactivatetraceies(
    S1ap_DeactivateTraceIEs_t *s1ap_DeactivateTraceIEs,
    ANY_t *any_p) {

    S1ap_DeactivateTrace_t  s1ap_DeactivateTrace;
    S1ap_DeactivateTrace_t *s1ap_DeactivateTrace_p = &s1ap_DeactivateTrace;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_DeactivateTraceIEs != NULL);

    memset(s1ap_DeactivateTraceIEs, 0, sizeof(S1ap_DeactivateTraceIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_DeactivateTraceIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_DeactivateTrace, (void**)&s1ap_DeactivateTrace_p);

    for (i = 0; i < s1ap_DeactivateTrace_p->s1ap_DeactivateTrace_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_DeactivateTrace_p->s1ap_DeactivateTrace_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_DeactivateTraceIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_DeactivateTraceIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_UTRAN_Trace_ID:
            {
                S1ap_E_UTRAN_Trace_ID_t *s1apEUTRANTraceID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_UTRAN_Trace_ID, (void**)&s1apEUTRANTraceID_p);
                if (tempDecoded < 0 || s1apEUTRANTraceID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_UTRAN_Trace_ID failed\n");
                    if (s1apEUTRANTraceID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_UTRAN_Trace_ID, s1apEUTRANTraceID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_UTRAN_Trace_ID, s1apEUTRANTraceID_p);
                memcpy(&s1ap_DeactivateTraceIEs->e_UTRAN_Trace_ID, s1apEUTRANTraceID_p, sizeof(S1ap_E_UTRAN_Trace_ID_t));
                FREEMEM(s1apEUTRANTraceID_p);
                s1apEUTRANTraceID_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_deactivatetraceies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_DeactivateTrace, s1ap_DeactivateTrace_p);
    return decoded;
}

int s1ap_decode_s1ap_enbdirectinformationtransferies(
    S1ap_ENBDirectInformationTransferIEs_t *s1ap_ENBDirectInformationTransferIEs,
    ANY_t *any_p) {

    S1ap_ENBDirectInformationTransfer_t  s1ap_ENBDirectInformationTransfer;
    S1ap_ENBDirectInformationTransfer_t *s1ap_ENBDirectInformationTransfer_p = &s1ap_ENBDirectInformationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ENBDirectInformationTransferIEs != NULL);

    memset(s1ap_ENBDirectInformationTransferIEs, 0, sizeof(S1ap_ENBDirectInformationTransferIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ENBDirectInformationTransferIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ENBDirectInformationTransfer, (void**)&s1ap_ENBDirectInformationTransfer_p);

    for (i = 0; i < s1ap_ENBDirectInformationTransfer_p->s1ap_ENBDirectInformationTransfer_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ENBDirectInformationTransfer_p->s1ap_ENBDirectInformationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Inter_SystemInformationTransferTypeEDT:
            {
                S1ap_Inter_SystemInformationTransferType_t *s1apInterSystemInformationTransferType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Inter_SystemInformationTransferType, (void**)&s1apInterSystemInformationTransferType_p);
                if (tempDecoded < 0 || s1apInterSystemInformationTransferType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE inter_SystemInformationTransferTypeEDT failed\n");
                    if (s1apInterSystemInformationTransferType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Inter_SystemInformationTransferType, s1apInterSystemInformationTransferType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Inter_SystemInformationTransferType, s1apInterSystemInformationTransferType_p);
                memcpy(&s1ap_ENBDirectInformationTransferIEs->inter_SystemInformationTransferTypeEDT, s1apInterSystemInformationTransferType_p, sizeof(S1ap_Inter_SystemInformationTransferType_t));
                FREEMEM(s1apInterSystemInformationTransferType_p);
                s1apInterSystemInformationTransferType_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_enbdirectinformationtransferies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ENBDirectInformationTransfer, s1ap_ENBDirectInformationTransfer_p);
    return decoded;
}

int s1ap_decode_s1ap_e_rabreleaseresponseies(
    S1ap_E_RABReleaseResponseIEs_t *s1ap_E_RABReleaseResponseIEs,
    ANY_t *any_p) {

    S1ap_E_RABReleaseResponse_t  s1ap_E_RABReleaseResponse;
    S1ap_E_RABReleaseResponse_t *s1ap_E_RABReleaseResponse_p = &s1ap_E_RABReleaseResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_E_RABReleaseResponseIEs != NULL);

    memset(s1ap_E_RABReleaseResponseIEs, 0, sizeof(S1ap_E_RABReleaseResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_E_RABReleaseResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_E_RABReleaseResponse, (void**)&s1ap_E_RABReleaseResponse_p);

    for (i = 0; i < s1ap_E_RABReleaseResponse_p->s1ap_E_RABReleaseResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_E_RABReleaseResponse_p->s1ap_E_RABReleaseResponse_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_E_RABReleaseResponseIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_E_RABReleaseResponseIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABReleaseListBearerRelComp:
            {
                S1ap_E_RABReleaseListBearerRelComp_t *s1apERABReleaseListBearerRelComp_p = NULL;
                s1ap_E_RABReleaseResponseIEs->presenceMask |= S1AP_E_RABRELEASERESPONSEIES_E_RABRELEASELISTBEARERRELCOMP_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABReleaseListBearerRelComp, (void**)&s1apERABReleaseListBearerRelComp_p);
                if (tempDecoded < 0 || s1apERABReleaseListBearerRelComp_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABReleaseListBearerRelComp failed\n");
                    if (s1apERABReleaseListBearerRelComp_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABReleaseListBearerRelComp, s1apERABReleaseListBearerRelComp_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABReleaseListBearerRelComp, s1apERABReleaseListBearerRelComp_p);
                if (s1ap_decode_s1ap_e_rabreleaselistbearerrelcomp(&s1ap_E_RABReleaseResponseIEs->e_RABReleaseListBearerRelComp, s1apERABReleaseListBearerRelComp_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABReleaseListBearerRelComp failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABReleaseListBearerRelComp, s1apERABReleaseListBearerRelComp_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABFailedToReleaseList:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                s1ap_E_RABReleaseResponseIEs->presenceMask |= S1AP_E_RABRELEASERESPONSEIES_E_RABFAILEDTORELEASELIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABFailedToReleaseList failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_E_RABReleaseResponseIEs->e_RABFailedToReleaseList, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_E_RABReleaseResponseIEs->presenceMask |= S1AP_E_RABRELEASERESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_E_RABReleaseResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_e_rabreleaseresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABReleaseResponse, s1ap_E_RABReleaseResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_uplinknonueassociatedlppatransport_ies(
    S1ap_UplinkNonUEAssociatedLPPaTransport_IEs_t *s1ap_UplinkNonUEAssociatedLPPaTransport_IEs,
    ANY_t *any_p) {

    S1ap_UplinkNonUEAssociatedLPPaTransport_t  s1ap_UplinkNonUEAssociatedLPPaTransport;
    S1ap_UplinkNonUEAssociatedLPPaTransport_t *s1ap_UplinkNonUEAssociatedLPPaTransport_p = &s1ap_UplinkNonUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UplinkNonUEAssociatedLPPaTransport_IEs != NULL);

    memset(s1ap_UplinkNonUEAssociatedLPPaTransport_IEs, 0, sizeof(S1ap_UplinkNonUEAssociatedLPPaTransport_IEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UplinkNonUEAssociatedLPPaTransport_IEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UplinkNonUEAssociatedLPPaTransport, (void**)&s1ap_UplinkNonUEAssociatedLPPaTransport_p);

    for (i = 0; i < s1ap_UplinkNonUEAssociatedLPPaTransport_p->s1ap_UplinkNonUEAssociatedLPPaTransport_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UplinkNonUEAssociatedLPPaTransport_p->s1ap_UplinkNonUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Routing_ID:
            {
                S1ap_Routing_ID_t *s1apRoutingID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Routing_ID, (void**)&s1apRoutingID_p);
                if (tempDecoded < 0 || s1apRoutingID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE routing_ID failed\n");
                    if (s1apRoutingID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                memcpy(&s1ap_UplinkNonUEAssociatedLPPaTransport_IEs->routing_ID, s1apRoutingID_p, sizeof(S1ap_Routing_ID_t));
                FREEMEM(s1apRoutingID_p);
                s1apRoutingID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_LPPa_PDU:
            {
                S1ap_LPPa_PDU_t *s1apLPPaPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_LPPa_PDU, (void**)&s1apLPPaPDU_p);
                if (tempDecoded < 0 || s1apLPPaPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE lpPa_PDU failed\n");
                    if (s1apLPPaPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                memcpy(&s1ap_UplinkNonUEAssociatedLPPaTransport_IEs->lpPa_PDU, s1apLPPaPDU_p, sizeof(S1ap_LPPa_PDU_t));
                FREEMEM(s1apLPPaPDU_p);
                s1apLPPaPDU_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinknonueassociatedlppatransport_ies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UplinkNonUEAssociatedLPPaTransport, s1ap_UplinkNonUEAssociatedLPPaTransport_p);
    return decoded;
}

int s1ap_decode_s1ap_writereplacewarningresponseies(
    S1ap_WriteReplaceWarningResponseIEs_t *s1ap_WriteReplaceWarningResponseIEs,
    ANY_t *any_p) {

    S1ap_WriteReplaceWarningResponse_t  s1ap_WriteReplaceWarningResponse;
    S1ap_WriteReplaceWarningResponse_t *s1ap_WriteReplaceWarningResponse_p = &s1ap_WriteReplaceWarningResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_WriteReplaceWarningResponseIEs != NULL);

    memset(s1ap_WriteReplaceWarningResponseIEs, 0, sizeof(S1ap_WriteReplaceWarningResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_WriteReplaceWarningResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_WriteReplaceWarningResponse, (void**)&s1ap_WriteReplaceWarningResponse_p);

    for (i = 0; i < s1ap_WriteReplaceWarningResponse_p->s1ap_WriteReplaceWarningResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_WriteReplaceWarningResponse_p->s1ap_WriteReplaceWarningResponse_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MessageIdentifier:
            {
                S1ap_MessageIdentifier_t *s1apMessageIdentifier_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MessageIdentifier, (void**)&s1apMessageIdentifier_p);
                if (tempDecoded < 0 || s1apMessageIdentifier_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE messageIdentifier failed\n");
                    if (s1apMessageIdentifier_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                memcpy(&s1ap_WriteReplaceWarningResponseIEs->messageIdentifier, s1apMessageIdentifier_p, sizeof(S1ap_MessageIdentifier_t));
                FREEMEM(s1apMessageIdentifier_p);
                s1apMessageIdentifier_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_SerialNumber:
            {
                S1ap_SerialNumber_t *s1apSerialNumber_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SerialNumber, (void**)&s1apSerialNumber_p);
                if (tempDecoded < 0 || s1apSerialNumber_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE serialNumber failed\n");
                    if (s1apSerialNumber_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                memcpy(&s1ap_WriteReplaceWarningResponseIEs->serialNumber, s1apSerialNumber_p, sizeof(S1ap_SerialNumber_t));
                FREEMEM(s1apSerialNumber_p);
                s1apSerialNumber_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_BroadcastCompletedAreaList:
            {
                S1ap_BroadcastCompletedAreaList_t *s1apBroadcastCompletedAreaList_p = NULL;
                s1ap_WriteReplaceWarningResponseIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGRESPONSEIES_BROADCASTCOMPLETEDAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_BroadcastCompletedAreaList, (void**)&s1apBroadcastCompletedAreaList_p);
                if (tempDecoded < 0 || s1apBroadcastCompletedAreaList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE broadcastCompletedAreaList failed\n");
                    if (s1apBroadcastCompletedAreaList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_BroadcastCompletedAreaList, s1apBroadcastCompletedAreaList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_BroadcastCompletedAreaList, s1apBroadcastCompletedAreaList_p);
                memcpy(&s1ap_WriteReplaceWarningResponseIEs->broadcastCompletedAreaList, s1apBroadcastCompletedAreaList_p, sizeof(S1ap_BroadcastCompletedAreaList_t));
                FREEMEM(s1apBroadcastCompletedAreaList_p);
                s1apBroadcastCompletedAreaList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_WriteReplaceWarningResponseIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_WriteReplaceWarningResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_writereplacewarningresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_WriteReplaceWarningResponse, s1ap_WriteReplaceWarningResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_handovercancelies(
    S1ap_HandoverCancelIEs_t *s1ap_HandoverCancelIEs,
    ANY_t *any_p) {

    S1ap_HandoverCancel_t  s1ap_HandoverCancel;
    S1ap_HandoverCancel_t *s1ap_HandoverCancel_p = &s1ap_HandoverCancel;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverCancelIEs != NULL);

    memset(s1ap_HandoverCancelIEs, 0, sizeof(S1ap_HandoverCancelIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverCancelIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverCancel, (void**)&s1ap_HandoverCancel_p);

    for (i = 0; i < s1ap_HandoverCancel_p->s1ap_HandoverCancel_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverCancel_p->s1ap_HandoverCancel_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverCancelIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_HandoverCancelIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_HandoverCancelIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handovercancelies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverCancel, s1ap_HandoverCancel_p);
    return decoded;
}

int s1ap_decode_s1ap_handoverpreparationfailureies(
    S1ap_HandoverPreparationFailureIEs_t *s1ap_HandoverPreparationFailureIEs,
    ANY_t *any_p) {

    S1ap_HandoverPreparationFailure_t  s1ap_HandoverPreparationFailure;
    S1ap_HandoverPreparationFailure_t *s1ap_HandoverPreparationFailure_p = &s1ap_HandoverPreparationFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverPreparationFailureIEs != NULL);

    memset(s1ap_HandoverPreparationFailureIEs, 0, sizeof(S1ap_HandoverPreparationFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverPreparationFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverPreparationFailure, (void**)&s1ap_HandoverPreparationFailure_p);

    for (i = 0; i < s1ap_HandoverPreparationFailure_p->s1ap_HandoverPreparationFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverPreparationFailure_p->s1ap_HandoverPreparationFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverPreparationFailureIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_HandoverPreparationFailureIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_HandoverPreparationFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_HandoverPreparationFailureIEs->presenceMask |= S1AP_HANDOVERPREPARATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_HandoverPreparationFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handoverpreparationfailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverPreparationFailure, s1ap_HandoverPreparationFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_killrequesties(
    S1ap_KillRequestIEs_t *s1ap_KillRequestIEs,
    ANY_t *any_p) {

    S1ap_KillRequest_t  s1ap_KillRequest;
    S1ap_KillRequest_t *s1ap_KillRequest_p = &s1ap_KillRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_KillRequestIEs != NULL);

    memset(s1ap_KillRequestIEs, 0, sizeof(S1ap_KillRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_KillRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_KillRequest, (void**)&s1ap_KillRequest_p);

    for (i = 0; i < s1ap_KillRequest_p->s1ap_KillRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_KillRequest_p->s1ap_KillRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MessageIdentifier:
            {
                S1ap_MessageIdentifier_t *s1apMessageIdentifier_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MessageIdentifier, (void**)&s1apMessageIdentifier_p);
                if (tempDecoded < 0 || s1apMessageIdentifier_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE messageIdentifier failed\n");
                    if (s1apMessageIdentifier_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                memcpy(&s1ap_KillRequestIEs->messageIdentifier, s1apMessageIdentifier_p, sizeof(S1ap_MessageIdentifier_t));
                FREEMEM(s1apMessageIdentifier_p);
                s1apMessageIdentifier_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_SerialNumber:
            {
                S1ap_SerialNumber_t *s1apSerialNumber_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SerialNumber, (void**)&s1apSerialNumber_p);
                if (tempDecoded < 0 || s1apSerialNumber_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE serialNumber failed\n");
                    if (s1apSerialNumber_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                memcpy(&s1ap_KillRequestIEs->serialNumber, s1apSerialNumber_p, sizeof(S1ap_SerialNumber_t));
                FREEMEM(s1apSerialNumber_p);
                s1apSerialNumber_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_WarningAreaList:
            {
                S1ap_WarningAreaList_t *s1apWarningAreaList_p = NULL;
                s1ap_KillRequestIEs->presenceMask |= S1AP_KILLREQUESTIES_WARNINGAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_WarningAreaList, (void**)&s1apWarningAreaList_p);
                if (tempDecoded < 0 || s1apWarningAreaList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE warningAreaList failed\n");
                    if (s1apWarningAreaList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_WarningAreaList, s1apWarningAreaList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_WarningAreaList, s1apWarningAreaList_p);
                memcpy(&s1ap_KillRequestIEs->warningAreaList, s1apWarningAreaList_p, sizeof(S1ap_WarningAreaList_t));
                FREEMEM(s1apWarningAreaList_p);
                s1apWarningAreaList_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_killrequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_KillRequest, s1ap_KillRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_writereplacewarningrequesties(
    S1ap_WriteReplaceWarningRequestIEs_t *s1ap_WriteReplaceWarningRequestIEs,
    ANY_t *any_p) {

    S1ap_WriteReplaceWarningRequest_t  s1ap_WriteReplaceWarningRequest;
    S1ap_WriteReplaceWarningRequest_t *s1ap_WriteReplaceWarningRequest_p = &s1ap_WriteReplaceWarningRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_WriteReplaceWarningRequestIEs != NULL);

    memset(s1ap_WriteReplaceWarningRequestIEs, 0, sizeof(S1ap_WriteReplaceWarningRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_WriteReplaceWarningRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_WriteReplaceWarningRequest, (void**)&s1ap_WriteReplaceWarningRequest_p);

    for (i = 0; i < s1ap_WriteReplaceWarningRequest_p->s1ap_WriteReplaceWarningRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_WriteReplaceWarningRequest_p->s1ap_WriteReplaceWarningRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MessageIdentifier:
            {
                S1ap_MessageIdentifier_t *s1apMessageIdentifier_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MessageIdentifier, (void**)&s1apMessageIdentifier_p);
                if (tempDecoded < 0 || s1apMessageIdentifier_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE messageIdentifier failed\n");
                    if (s1apMessageIdentifier_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->messageIdentifier, s1apMessageIdentifier_p, sizeof(S1ap_MessageIdentifier_t));
                FREEMEM(s1apMessageIdentifier_p);
                s1apMessageIdentifier_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_SerialNumber:
            {
                S1ap_SerialNumber_t *s1apSerialNumber_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SerialNumber, (void**)&s1apSerialNumber_p);
                if (tempDecoded < 0 || s1apSerialNumber_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE serialNumber failed\n");
                    if (s1apSerialNumber_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->serialNumber, s1apSerialNumber_p, sizeof(S1ap_SerialNumber_t));
                FREEMEM(s1apSerialNumber_p);
                s1apSerialNumber_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_WarningAreaList:
            {
                S1ap_WarningAreaList_t *s1apWarningAreaList_p = NULL;
                s1ap_WriteReplaceWarningRequestIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_WarningAreaList, (void**)&s1apWarningAreaList_p);
                if (tempDecoded < 0 || s1apWarningAreaList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE warningAreaList failed\n");
                    if (s1apWarningAreaList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_WarningAreaList, s1apWarningAreaList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_WarningAreaList, s1apWarningAreaList_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->warningAreaList, s1apWarningAreaList_p, sizeof(S1ap_WarningAreaList_t));
                FREEMEM(s1apWarningAreaList_p);
                s1apWarningAreaList_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_RepetitionPeriod:
            {
                S1ap_RepetitionPeriod_t *s1apRepetitionPeriod_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RepetitionPeriod, (void**)&s1apRepetitionPeriod_p);
                if (tempDecoded < 0 || s1apRepetitionPeriod_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE repetitionPeriod failed\n");
                    if (s1apRepetitionPeriod_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RepetitionPeriod, s1apRepetitionPeriod_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RepetitionPeriod, s1apRepetitionPeriod_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->repetitionPeriod, s1apRepetitionPeriod_p, sizeof(S1ap_RepetitionPeriod_t));
                FREEMEM(s1apRepetitionPeriod_p);
                s1apRepetitionPeriod_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_ExtendedRepetitionPeriod:
            {
                S1ap_ExtendedRepetitionPeriod_t *s1apExtendedRepetitionPeriod_p = NULL;
                s1ap_WriteReplaceWarningRequestIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGREQUESTIES_EXTENDEDREPETITIONPERIOD_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ExtendedRepetitionPeriod, (void**)&s1apExtendedRepetitionPeriod_p);
                if (tempDecoded < 0 || s1apExtendedRepetitionPeriod_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE extendedRepetitionPeriod failed\n");
                    if (s1apExtendedRepetitionPeriod_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ExtendedRepetitionPeriod, s1apExtendedRepetitionPeriod_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ExtendedRepetitionPeriod, s1apExtendedRepetitionPeriod_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->extendedRepetitionPeriod, s1apExtendedRepetitionPeriod_p, sizeof(S1ap_ExtendedRepetitionPeriod_t));
                FREEMEM(s1apExtendedRepetitionPeriod_p);
                s1apExtendedRepetitionPeriod_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_NumberofBroadcastRequest:
            {
                S1ap_NumberofBroadcastRequest_t *s1apNumberofBroadcastRequest_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_NumberofBroadcastRequest, (void**)&s1apNumberofBroadcastRequest_p);
                if (tempDecoded < 0 || s1apNumberofBroadcastRequest_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE numberofBroadcastRequest failed\n");
                    if (s1apNumberofBroadcastRequest_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_NumberofBroadcastRequest, s1apNumberofBroadcastRequest_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_NumberofBroadcastRequest, s1apNumberofBroadcastRequest_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->numberofBroadcastRequest, s1apNumberofBroadcastRequest_p, sizeof(S1ap_NumberofBroadcastRequest_t));
                FREEMEM(s1apNumberofBroadcastRequest_p);
                s1apNumberofBroadcastRequest_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_WarningType:
            {
                S1ap_WarningType_t *s1apWarningType_p = NULL;
                s1ap_WriteReplaceWarningRequestIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGTYPE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_WarningType, (void**)&s1apWarningType_p);
                if (tempDecoded < 0 || s1apWarningType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE warningType failed\n");
                    if (s1apWarningType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_WarningType, s1apWarningType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_WarningType, s1apWarningType_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->warningType, s1apWarningType_p, sizeof(S1ap_WarningType_t));
                FREEMEM(s1apWarningType_p);
                s1apWarningType_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_WarningSecurityInfo:
            {
                S1ap_WarningSecurityInfo_t *s1apWarningSecurityInfo_p = NULL;
                s1ap_WriteReplaceWarningRequestIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGSECURITYINFO_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_WarningSecurityInfo, (void**)&s1apWarningSecurityInfo_p);
                if (tempDecoded < 0 || s1apWarningSecurityInfo_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE warningSecurityInfo failed\n");
                    if (s1apWarningSecurityInfo_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_WarningSecurityInfo, s1apWarningSecurityInfo_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_WarningSecurityInfo, s1apWarningSecurityInfo_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->warningSecurityInfo, s1apWarningSecurityInfo_p, sizeof(S1ap_WarningSecurityInfo_t));
                FREEMEM(s1apWarningSecurityInfo_p);
                s1apWarningSecurityInfo_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_DataCodingScheme:
            {
                S1ap_DataCodingScheme_t *s1apDataCodingScheme_p = NULL;
                s1ap_WriteReplaceWarningRequestIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGREQUESTIES_DATACODINGSCHEME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_DataCodingScheme, (void**)&s1apDataCodingScheme_p);
                if (tempDecoded < 0 || s1apDataCodingScheme_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE dataCodingScheme failed\n");
                    if (s1apDataCodingScheme_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_DataCodingScheme, s1apDataCodingScheme_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_DataCodingScheme, s1apDataCodingScheme_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->dataCodingScheme, s1apDataCodingScheme_p, sizeof(S1ap_DataCodingScheme_t));
                FREEMEM(s1apDataCodingScheme_p);
                s1apDataCodingScheme_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_WarningMessageContents:
            {
                S1ap_WarningMessageContents_t *s1apWarningMessageContents_p = NULL;
                s1ap_WriteReplaceWarningRequestIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGMESSAGECONTENTS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_WarningMessageContents, (void**)&s1apWarningMessageContents_p);
                if (tempDecoded < 0 || s1apWarningMessageContents_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE warningMessageContents failed\n");
                    if (s1apWarningMessageContents_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_WarningMessageContents, s1apWarningMessageContents_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_WarningMessageContents, s1apWarningMessageContents_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->warningMessageContents, s1apWarningMessageContents_p, sizeof(S1ap_WarningMessageContents_t));
                FREEMEM(s1apWarningMessageContents_p);
                s1apWarningMessageContents_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_ConcurrentWarningMessageIndicator:
            {
                S1ap_ConcurrentWarningMessageIndicator_t *s1apConcurrentWarningMessageIndicator_p = NULL;
                s1ap_WriteReplaceWarningRequestIEs->presenceMask |= S1AP_WRITEREPLACEWARNINGREQUESTIES_CONCURRENTWARNINGMESSAGEINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ConcurrentWarningMessageIndicator, (void**)&s1apConcurrentWarningMessageIndicator_p);
                if (tempDecoded < 0 || s1apConcurrentWarningMessageIndicator_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE concurrentWarningMessageIndicator failed\n");
                    if (s1apConcurrentWarningMessageIndicator_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ConcurrentWarningMessageIndicator, s1apConcurrentWarningMessageIndicator_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ConcurrentWarningMessageIndicator, s1apConcurrentWarningMessageIndicator_p);
                memcpy(&s1ap_WriteReplaceWarningRequestIEs->concurrentWarningMessageIndicator, s1apConcurrentWarningMessageIndicator_p, sizeof(S1ap_ConcurrentWarningMessageIndicator_t));
                FREEMEM(s1apConcurrentWarningMessageIndicator_p);
                s1apConcurrentWarningMessageIndicator_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_writereplacewarningrequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_WriteReplaceWarningRequest, s1ap_WriteReplaceWarningRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_uecapabilityinfoindicationies(
    S1ap_UECapabilityInfoIndicationIEs_t *s1ap_UECapabilityInfoIndicationIEs,
    ANY_t *any_p) {

    S1ap_UECapabilityInfoIndication_t  s1ap_UECapabilityInfoIndication;
    S1ap_UECapabilityInfoIndication_t *s1ap_UECapabilityInfoIndication_p = &s1ap_UECapabilityInfoIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UECapabilityInfoIndicationIEs != NULL);

    memset(s1ap_UECapabilityInfoIndicationIEs, 0, sizeof(S1ap_UECapabilityInfoIndicationIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UECapabilityInfoIndicationIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UECapabilityInfoIndication, (void**)&s1ap_UECapabilityInfoIndication_p);

    for (i = 0; i < s1ap_UECapabilityInfoIndication_p->s1ap_UECapabilityInfoIndication_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UECapabilityInfoIndication_p->s1ap_UECapabilityInfoIndication_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UECapabilityInfoIndicationIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UECapabilityInfoIndicationIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_UERadioCapability:
            {
                S1ap_UERadioCapability_t *s1apUERadioCapability_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UERadioCapability, (void**)&s1apUERadioCapability_p);
                if (tempDecoded < 0 || s1apUERadioCapability_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE ueRadioCapability failed\n");
                    if (s1apUERadioCapability_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UERadioCapability, s1apUERadioCapability_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UERadioCapability, s1apUERadioCapability_p);
                memcpy(&s1ap_UECapabilityInfoIndicationIEs->ueRadioCapability, s1apUERadioCapability_p, sizeof(S1ap_UERadioCapability_t));
                FREEMEM(s1apUERadioCapability_p);
                s1apUERadioCapability_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uecapabilityinfoindicationies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UECapabilityInfoIndication, s1ap_UECapabilityInfoIndication_p);
    return decoded;
}

int s1ap_decode_s1ap_locationreportingfailureindicationies(
    S1ap_LocationReportingFailureIndicationIEs_t *s1ap_LocationReportingFailureIndicationIEs,
    ANY_t *any_p) {

    S1ap_LocationReportingFailureIndication_t  s1ap_LocationReportingFailureIndication;
    S1ap_LocationReportingFailureIndication_t *s1ap_LocationReportingFailureIndication_p = &s1ap_LocationReportingFailureIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_LocationReportingFailureIndicationIEs != NULL);

    memset(s1ap_LocationReportingFailureIndicationIEs, 0, sizeof(S1ap_LocationReportingFailureIndicationIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_LocationReportingFailureIndicationIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_LocationReportingFailureIndication, (void**)&s1ap_LocationReportingFailureIndication_p);

    for (i = 0; i < s1ap_LocationReportingFailureIndication_p->s1ap_LocationReportingFailureIndication_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_LocationReportingFailureIndication_p->s1ap_LocationReportingFailureIndication_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_LocationReportingFailureIndicationIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_LocationReportingFailureIndicationIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_LocationReportingFailureIndicationIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_locationreportingfailureindicationies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_LocationReportingFailureIndication, s1ap_LocationReportingFailureIndication_p);
    return decoded;
}

int s1ap_decode_s1ap_initialcontextsetuprequesties(
    S1ap_InitialContextSetupRequestIEs_t *s1ap_InitialContextSetupRequestIEs,
    ANY_t *any_p) {

    S1ap_InitialContextSetupRequest_t  s1ap_InitialContextSetupRequest;
    S1ap_InitialContextSetupRequest_t *s1ap_InitialContextSetupRequest_p = &s1ap_InitialContextSetupRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_InitialContextSetupRequestIEs != NULL);

    memset(s1ap_InitialContextSetupRequestIEs, 0, sizeof(S1ap_InitialContextSetupRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_InitialContextSetupRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_InitialContextSetupRequest, (void**)&s1ap_InitialContextSetupRequest_p);

    for (i = 0; i < s1ap_InitialContextSetupRequest_p->s1ap_InitialContextSetupRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_InitialContextSetupRequest_p->s1ap_InitialContextSetupRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                S1ap_UEAggregateMaximumBitrate_t *s1apUEAggregateMaximumBitrate_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEAggregateMaximumBitrate, (void**)&s1apUEAggregateMaximumBitrate_p);
                if (tempDecoded < 0 || s1apUEAggregateMaximumBitrate_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uEaggregateMaximumBitrate failed\n");
                    if (s1apUEAggregateMaximumBitrate_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->uEaggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p, sizeof(S1ap_UEAggregateMaximumBitrate_t));
                FREEMEM(s1apUEAggregateMaximumBitrate_p);
                s1apUEAggregateMaximumBitrate_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABToBeSetupListCtxtSUReq:
            {
                S1ap_E_RABToBeSetupListCtxtSUReq_t *s1apERABToBeSetupListCtxtSUReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSetupListCtxtSUReq, (void**)&s1apERABToBeSetupListCtxtSUReq_p);
                if (tempDecoded < 0 || s1apERABToBeSetupListCtxtSUReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSetupListCtxtSUReq failed\n");
                    if (s1apERABToBeSetupListCtxtSUReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupListCtxtSUReq, s1apERABToBeSetupListCtxtSUReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSetupListCtxtSUReq, s1apERABToBeSetupListCtxtSUReq_p);
                if (s1ap_decode_s1ap_e_rabtobesetuplistctxtsureq(&s1ap_InitialContextSetupRequestIEs->e_RABToBeSetupListCtxtSUReq, s1apERABToBeSetupListCtxtSUReq_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABToBeSetupListCtxtSUReq failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupListCtxtSUReq, s1apERABToBeSetupListCtxtSUReq_p);
                }
            } break;
            case S1ap_ProtocolIE_ID_id_UESecurityCapabilities:
            {
                S1ap_UESecurityCapabilities_t *s1apUESecurityCapabilities_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UESecurityCapabilities, (void**)&s1apUESecurityCapabilities_p);
                if (tempDecoded < 0 || s1apUESecurityCapabilities_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE ueSecurityCapabilities failed\n");
                    if (s1apUESecurityCapabilities_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->ueSecurityCapabilities, s1apUESecurityCapabilities_p, sizeof(S1ap_UESecurityCapabilities_t));
                FREEMEM(s1apUESecurityCapabilities_p);
                s1apUESecurityCapabilities_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_SecurityKey:
            {
                S1ap_SecurityKey_t *s1apSecurityKey_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SecurityKey, (void**)&s1apSecurityKey_p);
                if (tempDecoded < 0 || s1apSecurityKey_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE securityKey failed\n");
                    if (s1apSecurityKey_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SecurityKey, s1apSecurityKey_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SecurityKey, s1apSecurityKey_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->securityKey, s1apSecurityKey_p, sizeof(S1ap_SecurityKey_t));
                FREEMEM(s1apSecurityKey_p);
                s1apSecurityKey_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_TraceActivation:
            {
                S1ap_TraceActivation_t *s1apTraceActivation_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_TRACEACTIVATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TraceActivation, (void**)&s1apTraceActivation_p);
                if (tempDecoded < 0 || s1apTraceActivation_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE traceActivation failed\n");
                    if (s1apTraceActivation_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TraceActivation, s1apTraceActivation_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TraceActivation, s1apTraceActivation_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->traceActivation, s1apTraceActivation_p, sizeof(S1ap_TraceActivation_t));
                FREEMEM(s1apTraceActivation_p);
                s1apTraceActivation_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_HandoverRestrictionList:
            {
                S1ap_HandoverRestrictionList_t *s1apHandoverRestrictionList_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_HandoverRestrictionList, (void**)&s1apHandoverRestrictionList_p);
                if (tempDecoded < 0 || s1apHandoverRestrictionList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE handoverRestrictionList failed\n");
                    if (s1apHandoverRestrictionList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverRestrictionList, s1apHandoverRestrictionList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_HandoverRestrictionList, s1apHandoverRestrictionList_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->handoverRestrictionList, s1apHandoverRestrictionList_p, sizeof(S1ap_HandoverRestrictionList_t));
                FREEMEM(s1apHandoverRestrictionList_p);
                s1apHandoverRestrictionList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_UERadioCapability:
            {
                S1ap_UERadioCapability_t *s1apUERadioCapability_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UERadioCapability, (void**)&s1apUERadioCapability_p);
                if (tempDecoded < 0 || s1apUERadioCapability_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE ueRadioCapability failed\n");
                    if (s1apUERadioCapability_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UERadioCapability, s1apUERadioCapability_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UERadioCapability, s1apUERadioCapability_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->ueRadioCapability, s1apUERadioCapability_p, sizeof(S1ap_UERadioCapability_t));
                FREEMEM(s1apUERadioCapability_p);
                s1apUERadioCapability_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SubscriberProfileIDforRFP:
            {
                S1ap_SubscriberProfileIDforRFP_t *s1apSubscriberProfileIDforRFP_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SubscriberProfileIDforRFP, (void**)&s1apSubscriberProfileIDforRFP_p);
                if (tempDecoded < 0 || s1apSubscriberProfileIDforRFP_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE subscriberProfileIDforRFP failed\n");
                    if (s1apSubscriberProfileIDforRFP_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SubscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SubscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->subscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p, sizeof(S1ap_SubscriberProfileIDforRFP_t));
                FREEMEM(s1apSubscriberProfileIDforRFP_p);
                s1apSubscriberProfileIDforRFP_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSFallbackIndicator:
            {
                S1ap_CSFallbackIndicator_t *s1apCSFallbackIndicator_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_CSFALLBACKINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSFallbackIndicator, (void**)&s1apCSFallbackIndicator_p);
                if (tempDecoded < 0 || s1apCSFallbackIndicator_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csFallbackIndicator failed\n");
                    if (s1apCSFallbackIndicator_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSFallbackIndicator, s1apCSFallbackIndicator_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSFallbackIndicator, s1apCSFallbackIndicator_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->csFallbackIndicator, s1apCSFallbackIndicator_p, sizeof(S1ap_CSFallbackIndicator_t));
                FREEMEM(s1apCSFallbackIndicator_p);
                s1apCSFallbackIndicator_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SRVCCOperationPossible:
            {
                S1ap_SRVCCOperationPossible_t *s1apSRVCCOperationPossible_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SRVCCOperationPossible, (void**)&s1apSRVCCOperationPossible_p);
                if (tempDecoded < 0 || s1apSRVCCOperationPossible_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE srvccOperationPossible failed\n");
                    if (s1apSRVCCOperationPossible_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SRVCCOperationPossible, s1apSRVCCOperationPossible_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SRVCCOperationPossible, s1apSRVCCOperationPossible_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->srvccOperationPossible, s1apSRVCCOperationPossible_p, sizeof(S1ap_SRVCCOperationPossible_t));
                FREEMEM(s1apSRVCCOperationPossible_p);
                s1apSRVCCOperationPossible_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSGMembershipStatus:
            {
                S1ap_CSGMembershipStatus_t *s1apCSGMembershipStatus_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSGMembershipStatus, (void**)&s1apCSGMembershipStatus_p);
                if (tempDecoded < 0 || s1apCSGMembershipStatus_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csgMembershipStatus failed\n");
                    if (s1apCSGMembershipStatus_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSGMembershipStatus, s1apCSGMembershipStatus_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSGMembershipStatus, s1apCSGMembershipStatus_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->csgMembershipStatus, s1apCSGMembershipStatus_p, sizeof(S1ap_CSGMembershipStatus_t));
                FREEMEM(s1apCSGMembershipStatus_p);
                s1apCSGMembershipStatus_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_RegisteredLAI:
            {
                S1ap_LAI_t *s1apLAI_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_REGISTEREDLAI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_LAI, (void**)&s1apLAI_p);
                if (tempDecoded < 0 || s1apLAI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE registeredLAI failed\n");
                    if (s1apLAI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_LAI, s1apLAI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_LAI, s1apLAI_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->registeredLAI, s1apLAI_p, sizeof(S1ap_LAI_t));
                FREEMEM(s1apLAI_p);
                s1apLAI_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GUMMEI_ID:
            {
                S1ap_GUMMEI_t *s1apGUMMEI_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_GUMMEI_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_GUMMEI, (void**)&s1apGUMMEI_p);
                if (tempDecoded < 0 || s1apGUMMEI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gummei_id failed\n");
                    if (s1apGUMMEI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->gummei_id, s1apGUMMEI_p, sizeof(S1ap_GUMMEI_t));
                FREEMEM(s1apGUMMEI_p);
                s1apGUMMEI_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID_2:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_MME_UE_S1AP_ID_2_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id_2 failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->mme_ue_s1ap_id_2, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_ManagementBasedMDTAllowed:
            {
                S1ap_ManagementBasedMDTAllowed_t *s1apManagementBasedMDTAllowed_p = NULL;
                s1ap_InitialContextSetupRequestIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPREQUESTIES_MANAGEMENTBASEDMDTALLOWED_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ManagementBasedMDTAllowed, (void**)&s1apManagementBasedMDTAllowed_p);
                if (tempDecoded < 0 || s1apManagementBasedMDTAllowed_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE managementBasedMDTAllowed failed\n");
                    if (s1apManagementBasedMDTAllowed_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ManagementBasedMDTAllowed, s1apManagementBasedMDTAllowed_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ManagementBasedMDTAllowed, s1apManagementBasedMDTAllowed_p);
                memcpy(&s1ap_InitialContextSetupRequestIEs->managementBasedMDTAllowed, s1apManagementBasedMDTAllowed_p, sizeof(S1ap_ManagementBasedMDTAllowed_t));
                FREEMEM(s1apManagementBasedMDTAllowed_p);
                s1apManagementBasedMDTAllowed_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_initialcontextsetuprequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_InitialContextSetupRequest, s1ap_InitialContextSetupRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_initialuemessageies(
    S1ap_InitialUEMessageIEs_t *s1ap_InitialUEMessageIEs,
    ANY_t *any_p) {

    S1ap_InitialUEMessage_t  s1ap_InitialUEMessage;
    S1ap_InitialUEMessage_t *s1ap_InitialUEMessage_p = &s1ap_InitialUEMessage;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_InitialUEMessageIEs != NULL);

    memset(s1ap_InitialUEMessageIEs, 0, sizeof(S1ap_InitialUEMessageIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_InitialUEMessageIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_InitialUEMessage, (void**)&s1ap_InitialUEMessage_p);

    for (i = 0; i < s1ap_InitialUEMessage_p->s1ap_InitialUEMessage_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_InitialUEMessage_p->s1ap_InitialUEMessage_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_InitialUEMessageIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_NAS_PDU:
            {
                S1ap_NAS_PDU_t *s1apNASPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_NAS_PDU, (void**)&s1apNASPDU_p);
                if (tempDecoded < 0 || s1apNASPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE nas_pdu failed\n");
                    if (s1apNASPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                memcpy(&s1ap_InitialUEMessageIEs->nas_pdu, s1apNASPDU_p, sizeof(S1ap_NAS_PDU_t));
                FREEMEM(s1apNASPDU_p);
                s1apNASPDU_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TAI:
            {
                S1ap_TAI_t *s1apTAI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TAI, (void**)&s1apTAI_p);
                if (tempDecoded < 0 || s1apTAI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE tai failed\n");
                    if (s1apTAI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TAI, s1apTAI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TAI, s1apTAI_p);
                memcpy(&s1ap_InitialUEMessageIEs->tai, s1apTAI_p, sizeof(S1ap_TAI_t));
                FREEMEM(s1apTAI_p);
                s1apTAI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_EUTRAN_CGI:
            {
                S1ap_EUTRAN_CGI_t *s1apEUTRANCGI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_EUTRAN_CGI, (void**)&s1apEUTRANCGI_p);
                if (tempDecoded < 0 || s1apEUTRANCGI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eutran_cgi failed\n");
                    if (s1apEUTRANCGI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                memcpy(&s1ap_InitialUEMessageIEs->eutran_cgi, s1apEUTRANCGI_p, sizeof(S1ap_EUTRAN_CGI_t));
                FREEMEM(s1apEUTRANCGI_p);
                s1apEUTRANCGI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_RRC_Establishment_Cause:
            {
                S1ap_RRC_Establishment_Cause_t *s1apRRCEstablishmentCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RRC_Establishment_Cause, (void**)&s1apRRCEstablishmentCause_p);
                if (tempDecoded < 0 || s1apRRCEstablishmentCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE rrC_Establishment_Cause failed\n");
                    if (s1apRRCEstablishmentCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RRC_Establishment_Cause, s1apRRCEstablishmentCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RRC_Establishment_Cause, s1apRRCEstablishmentCause_p);
                memcpy(&s1ap_InitialUEMessageIEs->rrC_Establishment_Cause, s1apRRCEstablishmentCause_p, sizeof(S1ap_RRC_Establishment_Cause_t));
                FREEMEM(s1apRRCEstablishmentCause_p);
                s1apRRCEstablishmentCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_S_TMSI:
            {
                S1ap_S_TMSI_t *s1apSTMSI_p = NULL;
                s1ap_InitialUEMessageIEs->presenceMask |= S1AP_INITIALUEMESSAGEIES_S_TMSI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_S_TMSI, (void**)&s1apSTMSI_p);
                if (tempDecoded < 0 || s1apSTMSI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE s_tmsi failed\n");
                    if (s1apSTMSI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_S_TMSI, s1apSTMSI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_S_TMSI, s1apSTMSI_p);
                memcpy(&s1ap_InitialUEMessageIEs->s_tmsi, s1apSTMSI_p, sizeof(S1ap_S_TMSI_t));
                FREEMEM(s1apSTMSI_p);
                s1apSTMSI_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_Id:
            {
                S1ap_CSG_Id_t *s1apCSGId_p = NULL;
                s1ap_InitialUEMessageIEs->presenceMask |= S1AP_INITIALUEMESSAGEIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_Id, (void**)&s1apCSGId_p);
                if (tempDecoded < 0 || s1apCSGId_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_Id failed\n");
                    if (s1apCSGId_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                memcpy(&s1ap_InitialUEMessageIEs->csG_Id, s1apCSGId_p, sizeof(S1ap_CSG_Id_t));
                FREEMEM(s1apCSGId_p);
                s1apCSGId_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GUMMEI_ID:
            {
                S1ap_GUMMEI_t *s1apGUMMEI_p = NULL;
                s1ap_InitialUEMessageIEs->presenceMask |= S1AP_INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_GUMMEI, (void**)&s1apGUMMEI_p);
                if (tempDecoded < 0 || s1apGUMMEI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gummei_id failed\n");
                    if (s1apGUMMEI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                memcpy(&s1ap_InitialUEMessageIEs->gummei_id, s1apGUMMEI_p, sizeof(S1ap_GUMMEI_t));
                FREEMEM(s1apGUMMEI_p);
                s1apGUMMEI_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CellAccessMode:
            {
                S1ap_CellAccessMode_t *s1apCellAccessMode_p = NULL;
                s1ap_InitialUEMessageIEs->presenceMask |= S1AP_INITIALUEMESSAGEIES_CELLACCESSMODE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CellAccessMode, (void**)&s1apCellAccessMode_p);
                if (tempDecoded < 0 || s1apCellAccessMode_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cellAccessMode failed\n");
                    if (s1apCellAccessMode_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CellAccessMode, s1apCellAccessMode_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CellAccessMode, s1apCellAccessMode_p);
                memcpy(&s1ap_InitialUEMessageIEs->cellAccessMode, s1apCellAccessMode_p, sizeof(S1ap_CellAccessMode_t));
                FREEMEM(s1apCellAccessMode_p);
                s1apCellAccessMode_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GW_TransportLayerAddress:
            {
                S1ap_TransportLayerAddress_t *s1apTransportLayerAddress_p = NULL;
                s1ap_InitialUEMessageIEs->presenceMask |= S1AP_INITIALUEMESSAGEIES_GW_TRANSPORTLAYERADDRESS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TransportLayerAddress, (void**)&s1apTransportLayerAddress_p);
                if (tempDecoded < 0 || s1apTransportLayerAddress_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gW_TransportLayerAddress failed\n");
                    if (s1apTransportLayerAddress_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TransportLayerAddress, s1apTransportLayerAddress_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TransportLayerAddress, s1apTransportLayerAddress_p);
                memcpy(&s1ap_InitialUEMessageIEs->gW_TransportLayerAddress, s1apTransportLayerAddress_p, sizeof(S1ap_TransportLayerAddress_t));
                FREEMEM(s1apTransportLayerAddress_p);
                s1apTransportLayerAddress_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_RelayNode_Indicator:
            {
                S1ap_RelayNode_Indicator_t *s1apRelayNodeIndicator_p = NULL;
                s1ap_InitialUEMessageIEs->presenceMask |= S1AP_INITIALUEMESSAGEIES_RELAYNODE_INDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RelayNode_Indicator, (void**)&s1apRelayNodeIndicator_p);
                if (tempDecoded < 0 || s1apRelayNodeIndicator_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE relayNode_Indicator failed\n");
                    if (s1apRelayNodeIndicator_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RelayNode_Indicator, s1apRelayNodeIndicator_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RelayNode_Indicator, s1apRelayNodeIndicator_p);
                memcpy(&s1ap_InitialUEMessageIEs->relayNode_Indicator, s1apRelayNodeIndicator_p, sizeof(S1ap_RelayNode_Indicator_t));
                FREEMEM(s1apRelayNodeIndicator_p);
                s1apRelayNodeIndicator_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_initialuemessageies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_InitialUEMessage, s1ap_InitialUEMessage_p);
    return decoded;
}

int s1ap_decode_s1ap_locationreporties(
    S1ap_LocationReportIEs_t *s1ap_LocationReportIEs,
    ANY_t *any_p) {

    S1ap_LocationReport_t  s1ap_LocationReport;
    S1ap_LocationReport_t *s1ap_LocationReport_p = &s1ap_LocationReport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_LocationReportIEs != NULL);

    memset(s1ap_LocationReportIEs, 0, sizeof(S1ap_LocationReportIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_LocationReportIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_LocationReport, (void**)&s1ap_LocationReport_p);

    for (i = 0; i < s1ap_LocationReport_p->s1ap_LocationReport_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_LocationReport_p->s1ap_LocationReport_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_LocationReportIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_LocationReportIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_EUTRAN_CGI:
            {
                S1ap_EUTRAN_CGI_t *s1apEUTRANCGI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_EUTRAN_CGI, (void**)&s1apEUTRANCGI_p);
                if (tempDecoded < 0 || s1apEUTRANCGI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eutran_cgi failed\n");
                    if (s1apEUTRANCGI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                memcpy(&s1ap_LocationReportIEs->eutran_cgi, s1apEUTRANCGI_p, sizeof(S1ap_EUTRAN_CGI_t));
                FREEMEM(s1apEUTRANCGI_p);
                s1apEUTRANCGI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TAI:
            {
                S1ap_TAI_t *s1apTAI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TAI, (void**)&s1apTAI_p);
                if (tempDecoded < 0 || s1apTAI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE tai failed\n");
                    if (s1apTAI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TAI, s1apTAI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TAI, s1apTAI_p);
                memcpy(&s1ap_LocationReportIEs->tai, s1apTAI_p, sizeof(S1ap_TAI_t));
                FREEMEM(s1apTAI_p);
                s1apTAI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_RequestType:
            {
                S1ap_RequestType_t *s1apRequestType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RequestType, (void**)&s1apRequestType_p);
                if (tempDecoded < 0 || s1apRequestType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE requestType failed\n");
                    if (s1apRequestType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RequestType, s1apRequestType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RequestType, s1apRequestType_p);
                memcpy(&s1ap_LocationReportIEs->requestType, s1apRequestType_p, sizeof(S1ap_RequestType_t));
                FREEMEM(s1apRequestType_p);
                s1apRequestType_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_locationreporties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_LocationReport, s1ap_LocationReport_p);
    return decoded;
}

int s1ap_decode_s1ap_e_rabreleasecommandies(
    S1ap_E_RABReleaseCommandIEs_t *s1ap_E_RABReleaseCommandIEs,
    ANY_t *any_p) {

    S1ap_E_RABReleaseCommand_t  s1ap_E_RABReleaseCommand;
    S1ap_E_RABReleaseCommand_t *s1ap_E_RABReleaseCommand_p = &s1ap_E_RABReleaseCommand;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_E_RABReleaseCommandIEs != NULL);

    memset(s1ap_E_RABReleaseCommandIEs, 0, sizeof(S1ap_E_RABReleaseCommandIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_E_RABReleaseCommandIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_E_RABReleaseCommand, (void**)&s1ap_E_RABReleaseCommand_p);

    for (i = 0; i < s1ap_E_RABReleaseCommand_p->s1ap_E_RABReleaseCommand_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_E_RABReleaseCommand_p->s1ap_E_RABReleaseCommand_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_E_RABReleaseCommandIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_E_RABReleaseCommandIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                S1ap_UEAggregateMaximumBitrate_t *s1apUEAggregateMaximumBitrate_p = NULL;
                s1ap_E_RABReleaseCommandIEs->presenceMask |= S1AP_E_RABRELEASECOMMANDIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEAggregateMaximumBitrate, (void**)&s1apUEAggregateMaximumBitrate_p);
                if (tempDecoded < 0 || s1apUEAggregateMaximumBitrate_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uEaggregateMaximumBitrate failed\n");
                    if (s1apUEAggregateMaximumBitrate_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                memcpy(&s1ap_E_RABReleaseCommandIEs->uEaggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p, sizeof(S1ap_UEAggregateMaximumBitrate_t));
                FREEMEM(s1apUEAggregateMaximumBitrate_p);
                s1apUEAggregateMaximumBitrate_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABToBeReleasedList:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeReleasedList failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_E_RABReleaseCommandIEs->e_RABToBeReleasedList, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_NAS_PDU:
            {
                S1ap_NAS_PDU_t *s1apNASPDU_p = NULL;
                s1ap_E_RABReleaseCommandIEs->presenceMask |= S1AP_E_RABRELEASECOMMANDIES_NAS_PDU_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_NAS_PDU, (void**)&s1apNASPDU_p);
                if (tempDecoded < 0 || s1apNASPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE nas_pdu failed\n");
                    if (s1apNASPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                memcpy(&s1ap_E_RABReleaseCommandIEs->nas_pdu, s1apNASPDU_p, sizeof(S1ap_NAS_PDU_t));
                FREEMEM(s1apNASPDU_p);
                s1apNASPDU_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_e_rabreleasecommandies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABReleaseCommand, s1ap_E_RABReleaseCommand_p);
    return decoded;
}

int s1ap_decode_s1ap_uecontextmodificationresponseies(
    S1ap_UEContextModificationResponseIEs_t *s1ap_UEContextModificationResponseIEs,
    ANY_t *any_p) {

    S1ap_UEContextModificationResponse_t  s1ap_UEContextModificationResponse;
    S1ap_UEContextModificationResponse_t *s1ap_UEContextModificationResponse_p = &s1ap_UEContextModificationResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UEContextModificationResponseIEs != NULL);

    memset(s1ap_UEContextModificationResponseIEs, 0, sizeof(S1ap_UEContextModificationResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UEContextModificationResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UEContextModificationResponse, (void**)&s1ap_UEContextModificationResponse_p);

    for (i = 0; i < s1ap_UEContextModificationResponse_p->s1ap_UEContextModificationResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UEContextModificationResponse_p->s1ap_UEContextModificationResponse_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UEContextModificationResponseIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UEContextModificationResponseIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_UEContextModificationResponseIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_UEContextModificationResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uecontextmodificationresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UEContextModificationResponse, s1ap_UEContextModificationResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_uecontextmodificationrequesties(
    S1ap_UEContextModificationRequestIEs_t *s1ap_UEContextModificationRequestIEs,
    ANY_t *any_p) {

    S1ap_UEContextModificationRequest_t  s1ap_UEContextModificationRequest;
    S1ap_UEContextModificationRequest_t *s1ap_UEContextModificationRequest_p = &s1ap_UEContextModificationRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UEContextModificationRequestIEs != NULL);

    memset(s1ap_UEContextModificationRequestIEs, 0, sizeof(S1ap_UEContextModificationRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UEContextModificationRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UEContextModificationRequest, (void**)&s1ap_UEContextModificationRequest_p);

    for (i = 0; i < s1ap_UEContextModificationRequest_p->s1ap_UEContextModificationRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UEContextModificationRequest_p->s1ap_UEContextModificationRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SecurityKey:
            {
                S1ap_SecurityKey_t *s1apSecurityKey_p = NULL;
                s1ap_UEContextModificationRequestIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONREQUESTIES_SECURITYKEY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SecurityKey, (void**)&s1apSecurityKey_p);
                if (tempDecoded < 0 || s1apSecurityKey_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE securityKey failed\n");
                    if (s1apSecurityKey_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SecurityKey, s1apSecurityKey_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SecurityKey, s1apSecurityKey_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->securityKey, s1apSecurityKey_p, sizeof(S1ap_SecurityKey_t));
                FREEMEM(s1apSecurityKey_p);
                s1apSecurityKey_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SubscriberProfileIDforRFP:
            {
                S1ap_SubscriberProfileIDforRFP_t *s1apSubscriberProfileIDforRFP_p = NULL;
                s1ap_UEContextModificationRequestIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SubscriberProfileIDforRFP, (void**)&s1apSubscriberProfileIDforRFP_p);
                if (tempDecoded < 0 || s1apSubscriberProfileIDforRFP_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE subscriberProfileIDforRFP failed\n");
                    if (s1apSubscriberProfileIDforRFP_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SubscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SubscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->subscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p, sizeof(S1ap_SubscriberProfileIDforRFP_t));
                FREEMEM(s1apSubscriberProfileIDforRFP_p);
                s1apSubscriberProfileIDforRFP_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                S1ap_UEAggregateMaximumBitrate_t *s1apUEAggregateMaximumBitrate_p = NULL;
                s1ap_UEContextModificationRequestIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEAggregateMaximumBitrate, (void**)&s1apUEAggregateMaximumBitrate_p);
                if (tempDecoded < 0 || s1apUEAggregateMaximumBitrate_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uEaggregateMaximumBitrate failed\n");
                    if (s1apUEAggregateMaximumBitrate_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->uEaggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p, sizeof(S1ap_UEAggregateMaximumBitrate_t));
                FREEMEM(s1apUEAggregateMaximumBitrate_p);
                s1apUEAggregateMaximumBitrate_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSFallbackIndicator:
            {
                S1ap_CSFallbackIndicator_t *s1apCSFallbackIndicator_p = NULL;
                s1ap_UEContextModificationRequestIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONREQUESTIES_CSFALLBACKINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSFallbackIndicator, (void**)&s1apCSFallbackIndicator_p);
                if (tempDecoded < 0 || s1apCSFallbackIndicator_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csFallbackIndicator failed\n");
                    if (s1apCSFallbackIndicator_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSFallbackIndicator, s1apCSFallbackIndicator_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSFallbackIndicator, s1apCSFallbackIndicator_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->csFallbackIndicator, s1apCSFallbackIndicator_p, sizeof(S1ap_CSFallbackIndicator_t));
                FREEMEM(s1apCSFallbackIndicator_p);
                s1apCSFallbackIndicator_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_UESecurityCapabilities:
            {
                S1ap_UESecurityCapabilities_t *s1apUESecurityCapabilities_p = NULL;
                s1ap_UEContextModificationRequestIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONREQUESTIES_UESECURITYCAPABILITIES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UESecurityCapabilities, (void**)&s1apUESecurityCapabilities_p);
                if (tempDecoded < 0 || s1apUESecurityCapabilities_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE ueSecurityCapabilities failed\n");
                    if (s1apUESecurityCapabilities_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->ueSecurityCapabilities, s1apUESecurityCapabilities_p, sizeof(S1ap_UESecurityCapabilities_t));
                FREEMEM(s1apUESecurityCapabilities_p);
                s1apUESecurityCapabilities_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSGMembershipStatus:
            {
                S1ap_CSGMembershipStatus_t *s1apCSGMembershipStatus_p = NULL;
                s1ap_UEContextModificationRequestIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSGMembershipStatus, (void**)&s1apCSGMembershipStatus_p);
                if (tempDecoded < 0 || s1apCSGMembershipStatus_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csgMembershipStatus failed\n");
                    if (s1apCSGMembershipStatus_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSGMembershipStatus, s1apCSGMembershipStatus_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSGMembershipStatus, s1apCSGMembershipStatus_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->csgMembershipStatus, s1apCSGMembershipStatus_p, sizeof(S1ap_CSGMembershipStatus_t));
                FREEMEM(s1apCSGMembershipStatus_p);
                s1apCSGMembershipStatus_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_RegisteredLAI:
            {
                S1ap_LAI_t *s1apLAI_p = NULL;
                s1ap_UEContextModificationRequestIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONREQUESTIES_REGISTEREDLAI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_LAI, (void**)&s1apLAI_p);
                if (tempDecoded < 0 || s1apLAI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE registeredLAI failed\n");
                    if (s1apLAI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_LAI, s1apLAI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_LAI, s1apLAI_p);
                memcpy(&s1ap_UEContextModificationRequestIEs->registeredLAI, s1apLAI_p, sizeof(S1ap_LAI_t));
                FREEMEM(s1apLAI_p);
                s1apLAI_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uecontextmodificationrequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UEContextModificationRequest, s1ap_UEContextModificationRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_locationreportingcontrolies(
    S1ap_LocationReportingControlIEs_t *s1ap_LocationReportingControlIEs,
    ANY_t *any_p) {

    S1ap_LocationReportingControl_t  s1ap_LocationReportingControl;
    S1ap_LocationReportingControl_t *s1ap_LocationReportingControl_p = &s1ap_LocationReportingControl;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_LocationReportingControlIEs != NULL);

    memset(s1ap_LocationReportingControlIEs, 0, sizeof(S1ap_LocationReportingControlIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_LocationReportingControlIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_LocationReportingControl, (void**)&s1ap_LocationReportingControl_p);

    for (i = 0; i < s1ap_LocationReportingControl_p->s1ap_LocationReportingControl_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_LocationReportingControl_p->s1ap_LocationReportingControl_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_LocationReportingControlIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_LocationReportingControlIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_RequestType:
            {
                S1ap_RequestType_t *s1apRequestType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RequestType, (void**)&s1apRequestType_p);
                if (tempDecoded < 0 || s1apRequestType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE requestType failed\n");
                    if (s1apRequestType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RequestType, s1apRequestType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RequestType, s1apRequestType_p);
                memcpy(&s1ap_LocationReportingControlIEs->requestType, s1apRequestType_p, sizeof(S1ap_RequestType_t));
                FREEMEM(s1apRequestType_p);
                s1apRequestType_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_locationreportingcontrolies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_LocationReportingControl, s1ap_LocationReportingControl_p);
    return decoded;
}

int s1ap_decode_s1ap_uplinks1cdma2000tunnelingies(
    S1ap_UplinkS1cdma2000tunnelingIEs_t *s1ap_UplinkS1cdma2000tunnelingIEs,
    ANY_t *any_p) {

    S1ap_UplinkS1cdma2000tunneling_t  s1ap_UplinkS1cdma2000tunneling;
    S1ap_UplinkS1cdma2000tunneling_t *s1ap_UplinkS1cdma2000tunneling_p = &s1ap_UplinkS1cdma2000tunneling;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UplinkS1cdma2000tunnelingIEs != NULL);

    memset(s1ap_UplinkS1cdma2000tunnelingIEs, 0, sizeof(S1ap_UplinkS1cdma2000tunnelingIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UplinkS1cdma2000tunnelingIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UplinkS1cdma2000tunneling, (void**)&s1ap_UplinkS1cdma2000tunneling_p);

    for (i = 0; i < s1ap_UplinkS1cdma2000tunneling_p->s1ap_UplinkS1cdma2000tunneling_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UplinkS1cdma2000tunneling_p->s1ap_UplinkS1cdma2000tunneling_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_cdma2000RATType:
            {
                S1ap_Cdma2000RATType_t *s1apCdma2000RATType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000RATType, (void**)&s1apCdma2000RATType_p);
                if (tempDecoded < 0 || s1apCdma2000RATType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000RATType failed\n");
                    if (s1apCdma2000RATType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000RATType, s1apCdma2000RATType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000RATType, s1apCdma2000RATType_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000RATType, s1apCdma2000RATType_p, sizeof(S1ap_Cdma2000RATType_t));
                FREEMEM(s1apCdma2000RATType_p);
                s1apCdma2000RATType_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_cdma2000SectorID:
            {
                S1ap_Cdma2000SectorID_t *s1apCdma2000SectorID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000SectorID, (void**)&s1apCdma2000SectorID_p);
                if (tempDecoded < 0 || s1apCdma2000SectorID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000SectorID failed\n");
                    if (s1apCdma2000SectorID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000SectorID, s1apCdma2000SectorID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000SectorID, s1apCdma2000SectorID_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000SectorID, s1apCdma2000SectorID_p, sizeof(S1ap_Cdma2000SectorID_t));
                FREEMEM(s1apCdma2000SectorID_p);
                s1apCdma2000SectorID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_cdma2000HORequiredIndication:
            {
                S1ap_Cdma2000HORequiredIndication_t *s1apCdma2000HORequiredIndication_p = NULL;
                s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask |= S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000HOREQUIREDINDICATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000HORequiredIndication, (void**)&s1apCdma2000HORequiredIndication_p);
                if (tempDecoded < 0 || s1apCdma2000HORequiredIndication_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000HORequiredIndication failed\n");
                    if (s1apCdma2000HORequiredIndication_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000HORequiredIndication, s1apCdma2000HORequiredIndication_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000HORequiredIndication, s1apCdma2000HORequiredIndication_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000HORequiredIndication, s1apCdma2000HORequiredIndication_p, sizeof(S1ap_Cdma2000HORequiredIndication_t));
                FREEMEM(s1apCdma2000HORequiredIndication_p);
                s1apCdma2000HORequiredIndication_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_cdma2000OneXSRVCCInfo:
            {
                S1ap_Cdma2000OneXSRVCCInfo_t *s1apCdma2000OneXSRVCCInfo_p = NULL;
                s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask |= S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXSRVCCINFO_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000OneXSRVCCInfo, (void**)&s1apCdma2000OneXSRVCCInfo_p);
                if (tempDecoded < 0 || s1apCdma2000OneXSRVCCInfo_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000OneXSRVCCInfo failed\n");
                    if (s1apCdma2000OneXSRVCCInfo_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000OneXSRVCCInfo, s1apCdma2000OneXSRVCCInfo_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000OneXSRVCCInfo, s1apCdma2000OneXSRVCCInfo_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000OneXSRVCCInfo, s1apCdma2000OneXSRVCCInfo_p, sizeof(S1ap_Cdma2000OneXSRVCCInfo_t));
                FREEMEM(s1apCdma2000OneXSRVCCInfo_p);
                s1apCdma2000OneXSRVCCInfo_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_cdma2000OneXRAND:
            {
                S1ap_Cdma2000OneXRAND_t *s1apCdma2000OneXRAND_p = NULL;
                s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask |= S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXRAND_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000OneXRAND, (void**)&s1apCdma2000OneXRAND_p);
                if (tempDecoded < 0 || s1apCdma2000OneXRAND_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000OneXRAND failed\n");
                    if (s1apCdma2000OneXRAND_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000OneXRAND, s1apCdma2000OneXRAND_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000OneXRAND, s1apCdma2000OneXRAND_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000OneXRAND, s1apCdma2000OneXRAND_p, sizeof(S1ap_Cdma2000OneXRAND_t));
                FREEMEM(s1apCdma2000OneXRAND_p);
                s1apCdma2000OneXRAND_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_cdma2000PDU:
            {
                S1ap_Cdma2000PDU_t *s1apCdma2000PDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000PDU, (void**)&s1apCdma2000PDU_p);
                if (tempDecoded < 0 || s1apCdma2000PDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000PDU failed\n");
                    if (s1apCdma2000PDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000PDU, s1apCdma2000PDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000PDU, s1apCdma2000PDU_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000PDU, s1apCdma2000PDU_p, sizeof(S1ap_Cdma2000PDU_t));
                FREEMEM(s1apCdma2000PDU_p);
                s1apCdma2000PDU_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_EUTRANRoundTripDelayEstimationInfo:
            {
                S1ap_EUTRANRoundTripDelayEstimationInfo_t *s1apEUTRANRoundTripDelayEstimationInfo_p = NULL;
                s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask |= S1AP_UPLINKS1CDMA2000TUNNELINGIES_EUTRANROUNDTRIPDELAYESTIMATIONINFO_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_EUTRANRoundTripDelayEstimationInfo, (void**)&s1apEUTRANRoundTripDelayEstimationInfo_p);
                if (tempDecoded < 0 || s1apEUTRANRoundTripDelayEstimationInfo_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eutranRoundTripDelayEstimationInfo failed\n");
                    if (s1apEUTRANRoundTripDelayEstimationInfo_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_EUTRANRoundTripDelayEstimationInfo, s1apEUTRANRoundTripDelayEstimationInfo_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_EUTRANRoundTripDelayEstimationInfo, s1apEUTRANRoundTripDelayEstimationInfo_p);
                memcpy(&s1ap_UplinkS1cdma2000tunnelingIEs->eutranRoundTripDelayEstimationInfo, s1apEUTRANRoundTripDelayEstimationInfo_p, sizeof(S1ap_EUTRANRoundTripDelayEstimationInfo_t));
                FREEMEM(s1apEUTRANRoundTripDelayEstimationInfo_p);
                s1apEUTRANRoundTripDelayEstimationInfo_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinks1cdma2000tunnelingies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UplinkS1cdma2000tunneling, s1ap_UplinkS1cdma2000tunneling_p);
    return decoded;
}

int s1ap_decode_s1ap_handoverrequesties(
    S1ap_HandoverRequestIEs_t *s1ap_HandoverRequestIEs,
    ANY_t *any_p) {

    S1ap_HandoverRequest_t  s1ap_HandoverRequest;
    S1ap_HandoverRequest_t *s1ap_HandoverRequest_p = &s1ap_HandoverRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverRequestIEs != NULL);

    memset(s1ap_HandoverRequestIEs, 0, sizeof(S1ap_HandoverRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverRequest, (void**)&s1ap_HandoverRequest_p);

    for (i = 0; i < s1ap_HandoverRequest_p->s1ap_HandoverRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverRequest_p->s1ap_HandoverRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverRequestIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_HandoverType:
            {
                S1ap_HandoverType_t *s1apHandoverType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_HandoverType, (void**)&s1apHandoverType_p);
                if (tempDecoded < 0 || s1apHandoverType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE handoverType failed\n");
                    if (s1apHandoverType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverType, s1apHandoverType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_HandoverType, s1apHandoverType_p);
                memcpy(&s1ap_HandoverRequestIEs->handoverType, s1apHandoverType_p, sizeof(S1ap_HandoverType_t));
                FREEMEM(s1apHandoverType_p);
                s1apHandoverType_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_HandoverRequestIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                S1ap_UEAggregateMaximumBitrate_t *s1apUEAggregateMaximumBitrate_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEAggregateMaximumBitrate, (void**)&s1apUEAggregateMaximumBitrate_p);
                if (tempDecoded < 0 || s1apUEAggregateMaximumBitrate_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uEaggregateMaximumBitrate failed\n");
                    if (s1apUEAggregateMaximumBitrate_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                memcpy(&s1ap_HandoverRequestIEs->uEaggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p, sizeof(S1ap_UEAggregateMaximumBitrate_t));
                FREEMEM(s1apUEAggregateMaximumBitrate_p);
                s1apUEAggregateMaximumBitrate_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABToBeSetupListHOReq:
            {
                S1ap_E_RABToBeSetupListHOReq_t *s1apERABToBeSetupListHOReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSetupListHOReq, (void**)&s1apERABToBeSetupListHOReq_p);
                if (tempDecoded < 0 || s1apERABToBeSetupListHOReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSetupListHOReq failed\n");
                    if (s1apERABToBeSetupListHOReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupListHOReq, s1apERABToBeSetupListHOReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSetupListHOReq, s1apERABToBeSetupListHOReq_p);
                if (s1ap_decode_s1ap_e_rabtobesetuplisthoreq(&s1ap_HandoverRequestIEs->e_RABToBeSetupListHOReq, s1apERABToBeSetupListHOReq_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABToBeSetupListHOReq failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupListHOReq, s1apERABToBeSetupListHOReq_p);
                }
            } break;
            case S1ap_ProtocolIE_ID_id_Source_ToTarget_TransparentContainer:
            {
                S1ap_Source_ToTarget_TransparentContainer_t *s1apSourceToTargetTransparentContainer_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Source_ToTarget_TransparentContainer, (void**)&s1apSourceToTargetTransparentContainer_p);
                if (tempDecoded < 0 || s1apSourceToTargetTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE source_ToTarget_TransparentContainer failed\n");
                    if (s1apSourceToTargetTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p);
                memcpy(&s1ap_HandoverRequestIEs->source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p, sizeof(S1ap_Source_ToTarget_TransparentContainer_t));
                FREEMEM(s1apSourceToTargetTransparentContainer_p);
                s1apSourceToTargetTransparentContainer_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_UESecurityCapabilities:
            {
                S1ap_UESecurityCapabilities_t *s1apUESecurityCapabilities_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UESecurityCapabilities, (void**)&s1apUESecurityCapabilities_p);
                if (tempDecoded < 0 || s1apUESecurityCapabilities_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE ueSecurityCapabilities failed\n");
                    if (s1apUESecurityCapabilities_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                memcpy(&s1ap_HandoverRequestIEs->ueSecurityCapabilities, s1apUESecurityCapabilities_p, sizeof(S1ap_UESecurityCapabilities_t));
                FREEMEM(s1apUESecurityCapabilities_p);
                s1apUESecurityCapabilities_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_HandoverRestrictionList:
            {
                S1ap_HandoverRestrictionList_t *s1apHandoverRestrictionList_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_HandoverRestrictionList, (void**)&s1apHandoverRestrictionList_p);
                if (tempDecoded < 0 || s1apHandoverRestrictionList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE handoverRestrictionList failed\n");
                    if (s1apHandoverRestrictionList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverRestrictionList, s1apHandoverRestrictionList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_HandoverRestrictionList, s1apHandoverRestrictionList_p);
                memcpy(&s1ap_HandoverRequestIEs->handoverRestrictionList, s1apHandoverRestrictionList_p, sizeof(S1ap_HandoverRestrictionList_t));
                FREEMEM(s1apHandoverRestrictionList_p);
                s1apHandoverRestrictionList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_TraceActivation:
            {
                S1ap_TraceActivation_t *s1apTraceActivation_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_TRACEACTIVATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TraceActivation, (void**)&s1apTraceActivation_p);
                if (tempDecoded < 0 || s1apTraceActivation_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE traceActivation failed\n");
                    if (s1apTraceActivation_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TraceActivation, s1apTraceActivation_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TraceActivation, s1apTraceActivation_p);
                memcpy(&s1ap_HandoverRequestIEs->traceActivation, s1apTraceActivation_p, sizeof(S1ap_TraceActivation_t));
                FREEMEM(s1apTraceActivation_p);
                s1apTraceActivation_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_RequestType:
            {
                S1ap_RequestType_t *s1apRequestType_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_REQUESTTYPE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RequestType, (void**)&s1apRequestType_p);
                if (tempDecoded < 0 || s1apRequestType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE requestType failed\n");
                    if (s1apRequestType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RequestType, s1apRequestType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RequestType, s1apRequestType_p);
                memcpy(&s1ap_HandoverRequestIEs->requestType, s1apRequestType_p, sizeof(S1ap_RequestType_t));
                FREEMEM(s1apRequestType_p);
                s1apRequestType_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SRVCCOperationPossible:
            {
                S1ap_SRVCCOperationPossible_t *s1apSRVCCOperationPossible_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SRVCCOperationPossible, (void**)&s1apSRVCCOperationPossible_p);
                if (tempDecoded < 0 || s1apSRVCCOperationPossible_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE srvccOperationPossible failed\n");
                    if (s1apSRVCCOperationPossible_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SRVCCOperationPossible, s1apSRVCCOperationPossible_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SRVCCOperationPossible, s1apSRVCCOperationPossible_p);
                memcpy(&s1ap_HandoverRequestIEs->srvccOperationPossible, s1apSRVCCOperationPossible_p, sizeof(S1ap_SRVCCOperationPossible_t));
                FREEMEM(s1apSRVCCOperationPossible_p);
                s1apSRVCCOperationPossible_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_SecurityContext:
            {
                S1ap_SecurityContext_t *s1apSecurityContext_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SecurityContext, (void**)&s1apSecurityContext_p);
                if (tempDecoded < 0 || s1apSecurityContext_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE securityContext failed\n");
                    if (s1apSecurityContext_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SecurityContext, s1apSecurityContext_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SecurityContext, s1apSecurityContext_p);
                memcpy(&s1ap_HandoverRequestIEs->securityContext, s1apSecurityContext_p, sizeof(S1ap_SecurityContext_t));
                FREEMEM(s1apSecurityContext_p);
                s1apSecurityContext_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_Id:
            {
                S1ap_CSG_Id_t *s1apCSGId_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_Id, (void**)&s1apCSGId_p);
                if (tempDecoded < 0 || s1apCSGId_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_Id failed\n");
                    if (s1apCSGId_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                memcpy(&s1ap_HandoverRequestIEs->csG_Id, s1apCSGId_p, sizeof(S1ap_CSG_Id_t));
                FREEMEM(s1apCSGId_p);
                s1apCSGId_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSGMembershipStatus:
            {
                S1ap_CSGMembershipStatus_t *s1apCSGMembershipStatus_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSGMembershipStatus, (void**)&s1apCSGMembershipStatus_p);
                if (tempDecoded < 0 || s1apCSGMembershipStatus_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csgMembershipStatus failed\n");
                    if (s1apCSGMembershipStatus_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSGMembershipStatus, s1apCSGMembershipStatus_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSGMembershipStatus, s1apCSGMembershipStatus_p);
                memcpy(&s1ap_HandoverRequestIEs->csgMembershipStatus, s1apCSGMembershipStatus_p, sizeof(S1ap_CSGMembershipStatus_t));
                FREEMEM(s1apCSGMembershipStatus_p);
                s1apCSGMembershipStatus_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GUMMEI_ID:
            {
                S1ap_GUMMEI_t *s1apGUMMEI_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_GUMMEI_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_GUMMEI, (void**)&s1apGUMMEI_p);
                if (tempDecoded < 0 || s1apGUMMEI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gummei_id failed\n");
                    if (s1apGUMMEI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                memcpy(&s1ap_HandoverRequestIEs->gummei_id, s1apGUMMEI_p, sizeof(S1ap_GUMMEI_t));
                FREEMEM(s1apGUMMEI_p);
                s1apGUMMEI_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID_2:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_MME_UE_S1AP_ID_2_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id_2 failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverRequestIEs->mme_ue_s1ap_id_2, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_ManagementBasedMDTAllowed:
            {
                S1ap_ManagementBasedMDTAllowed_t *s1apManagementBasedMDTAllowed_p = NULL;
                s1ap_HandoverRequestIEs->presenceMask |= S1AP_HANDOVERREQUESTIES_MANAGEMENTBASEDMDTALLOWED_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ManagementBasedMDTAllowed, (void**)&s1apManagementBasedMDTAllowed_p);
                if (tempDecoded < 0 || s1apManagementBasedMDTAllowed_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE managementBasedMDTAllowed failed\n");
                    if (s1apManagementBasedMDTAllowed_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ManagementBasedMDTAllowed, s1apManagementBasedMDTAllowed_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ManagementBasedMDTAllowed, s1apManagementBasedMDTAllowed_p);
                memcpy(&s1ap_HandoverRequestIEs->managementBasedMDTAllowed, s1apManagementBasedMDTAllowed_p, sizeof(S1ap_ManagementBasedMDTAllowed_t));
                FREEMEM(s1apManagementBasedMDTAllowed_p);
                s1apManagementBasedMDTAllowed_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handoverrequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverRequest, s1ap_HandoverRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_handoverrequiredies(
    S1ap_HandoverRequiredIEs_t *s1ap_HandoverRequiredIEs,
    ANY_t *any_p) {

    S1ap_HandoverRequired_t  s1ap_HandoverRequired;
    S1ap_HandoverRequired_t *s1ap_HandoverRequired_p = &s1ap_HandoverRequired;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverRequiredIEs != NULL);

    memset(s1ap_HandoverRequiredIEs, 0, sizeof(S1ap_HandoverRequiredIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverRequiredIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverRequired, (void**)&s1ap_HandoverRequired_p);

    for (i = 0; i < s1ap_HandoverRequired_p->s1ap_HandoverRequired_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverRequired_p->s1ap_HandoverRequired_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverRequiredIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_HandoverRequiredIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_HandoverType:
            {
                S1ap_HandoverType_t *s1apHandoverType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_HandoverType, (void**)&s1apHandoverType_p);
                if (tempDecoded < 0 || s1apHandoverType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE handoverType failed\n");
                    if (s1apHandoverType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverType, s1apHandoverType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_HandoverType, s1apHandoverType_p);
                memcpy(&s1ap_HandoverRequiredIEs->handoverType, s1apHandoverType_p, sizeof(S1ap_HandoverType_t));
                FREEMEM(s1apHandoverType_p);
                s1apHandoverType_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_HandoverRequiredIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TargetID:
            {
                S1ap_TargetID_t *s1apTargetID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TargetID, (void**)&s1apTargetID_p);
                if (tempDecoded < 0 || s1apTargetID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE targetID failed\n");
                    if (s1apTargetID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TargetID, s1apTargetID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TargetID, s1apTargetID_p);
                memcpy(&s1ap_HandoverRequiredIEs->targetID, s1apTargetID_p, sizeof(S1ap_TargetID_t));
                FREEMEM(s1apTargetID_p);
                s1apTargetID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_Direct_Forwarding_Path_Availability:
            {
                S1ap_Direct_Forwarding_Path_Availability_t *s1apDirectForwardingPathAvailability_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_DIRECT_FORWARDING_PATH_AVAILABILITY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Direct_Forwarding_Path_Availability, (void**)&s1apDirectForwardingPathAvailability_p);
                if (tempDecoded < 0 || s1apDirectForwardingPathAvailability_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE direct_Forwarding_Path_Availability failed\n");
                    if (s1apDirectForwardingPathAvailability_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Direct_Forwarding_Path_Availability, s1apDirectForwardingPathAvailability_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Direct_Forwarding_Path_Availability, s1apDirectForwardingPathAvailability_p);
                memcpy(&s1ap_HandoverRequiredIEs->direct_Forwarding_Path_Availability, s1apDirectForwardingPathAvailability_p, sizeof(S1ap_Direct_Forwarding_Path_Availability_t));
                FREEMEM(s1apDirectForwardingPathAvailability_p);
                s1apDirectForwardingPathAvailability_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SRVCCHOIndication:
            {
                S1ap_SRVCCHOIndication_t *s1apSRVCCHOIndication_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_SRVCCHOINDICATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SRVCCHOIndication, (void**)&s1apSRVCCHOIndication_p);
                if (tempDecoded < 0 || s1apSRVCCHOIndication_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE srvcchoIndication failed\n");
                    if (s1apSRVCCHOIndication_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SRVCCHOIndication, s1apSRVCCHOIndication_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SRVCCHOIndication, s1apSRVCCHOIndication_p);
                memcpy(&s1ap_HandoverRequiredIEs->srvcchoIndication, s1apSRVCCHOIndication_p, sizeof(S1ap_SRVCCHOIndication_t));
                FREEMEM(s1apSRVCCHOIndication_p);
                s1apSRVCCHOIndication_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Source_ToTarget_TransparentContainer:
            {
                S1ap_Source_ToTarget_TransparentContainer_t *s1apSourceToTargetTransparentContainer_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Source_ToTarget_TransparentContainer, (void**)&s1apSourceToTargetTransparentContainer_p);
                if (tempDecoded < 0 || s1apSourceToTargetTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE source_ToTarget_TransparentContainer failed\n");
                    if (s1apSourceToTargetTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p);
                memcpy(&s1ap_HandoverRequiredIEs->source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p, sizeof(S1ap_Source_ToTarget_TransparentContainer_t));
                FREEMEM(s1apSourceToTargetTransparentContainer_p);
                s1apSourceToTargetTransparentContainer_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_Source_ToTarget_TransparentContainer_Secondary:
            {
                S1ap_Source_ToTarget_TransparentContainer_t *s1apSourceToTargetTransparentContainer_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_SOURCE_TOTARGET_TRANSPARENTCONTAINER_SECONDARY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Source_ToTarget_TransparentContainer, (void**)&s1apSourceToTargetTransparentContainer_p);
                if (tempDecoded < 0 || s1apSourceToTargetTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE source_ToTarget_TransparentContainer_Secondary failed\n");
                    if (s1apSourceToTargetTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Source_ToTarget_TransparentContainer, s1apSourceToTargetTransparentContainer_p);
                memcpy(&s1ap_HandoverRequiredIEs->source_ToTarget_TransparentContainer_Secondary, s1apSourceToTargetTransparentContainer_p, sizeof(S1ap_Source_ToTarget_TransparentContainer_t));
                FREEMEM(s1apSourceToTargetTransparentContainer_p);
                s1apSourceToTargetTransparentContainer_p = NULL;
            } break;
            /* Conditional field */
            case S1ap_ProtocolIE_ID_id_MSClassmark2:
            {
                S1ap_MSClassmark2_t *s1apMSClassmark2_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_MSCLASSMARK2_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MSClassmark2, (void**)&s1apMSClassmark2_p);
                if (tempDecoded < 0 || s1apMSClassmark2_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE msClassmark2 failed\n");
                    if (s1apMSClassmark2_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MSClassmark2, s1apMSClassmark2_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MSClassmark2, s1apMSClassmark2_p);
                memcpy(&s1ap_HandoverRequiredIEs->msClassmark2, s1apMSClassmark2_p, sizeof(S1ap_MSClassmark2_t));
                FREEMEM(s1apMSClassmark2_p);
                s1apMSClassmark2_p = NULL;
            } break;
            /* Conditional field */
            case S1ap_ProtocolIE_ID_id_MSClassmark3:
            {
                S1ap_MSClassmark3_t *s1apMSClassmark3_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_MSCLASSMARK3_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MSClassmark3, (void**)&s1apMSClassmark3_p);
                if (tempDecoded < 0 || s1apMSClassmark3_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE msClassmark3 failed\n");
                    if (s1apMSClassmark3_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MSClassmark3, s1apMSClassmark3_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MSClassmark3, s1apMSClassmark3_p);
                memcpy(&s1ap_HandoverRequiredIEs->msClassmark3, s1apMSClassmark3_p, sizeof(S1ap_MSClassmark3_t));
                FREEMEM(s1apMSClassmark3_p);
                s1apMSClassmark3_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_Id:
            {
                S1ap_CSG_Id_t *s1apCSGId_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_Id, (void**)&s1apCSGId_p);
                if (tempDecoded < 0 || s1apCSGId_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_Id failed\n");
                    if (s1apCSGId_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                memcpy(&s1ap_HandoverRequiredIEs->csG_Id, s1apCSGId_p, sizeof(S1ap_CSG_Id_t));
                FREEMEM(s1apCSGId_p);
                s1apCSGId_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CellAccessMode:
            {
                S1ap_CellAccessMode_t *s1apCellAccessMode_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_CELLACCESSMODE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CellAccessMode, (void**)&s1apCellAccessMode_p);
                if (tempDecoded < 0 || s1apCellAccessMode_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cellAccessMode failed\n");
                    if (s1apCellAccessMode_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CellAccessMode, s1apCellAccessMode_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CellAccessMode, s1apCellAccessMode_p);
                memcpy(&s1ap_HandoverRequiredIEs->cellAccessMode, s1apCellAccessMode_p, sizeof(S1ap_CellAccessMode_t));
                FREEMEM(s1apCellAccessMode_p);
                s1apCellAccessMode_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_PS_ServiceNotAvailable:
            {
                S1ap_PS_ServiceNotAvailable_t *s1apPSServiceNotAvailable_p = NULL;
                s1ap_HandoverRequiredIEs->presenceMask |= S1AP_HANDOVERREQUIREDIES_PS_SERVICENOTAVAILABLE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_PS_ServiceNotAvailable, (void**)&s1apPSServiceNotAvailable_p);
                if (tempDecoded < 0 || s1apPSServiceNotAvailable_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE pS_ServiceNotAvailable failed\n");
                    if (s1apPSServiceNotAvailable_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_PS_ServiceNotAvailable, s1apPSServiceNotAvailable_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_PS_ServiceNotAvailable, s1apPSServiceNotAvailable_p);
                memcpy(&s1ap_HandoverRequiredIEs->pS_ServiceNotAvailable, s1apPSServiceNotAvailable_p, sizeof(S1ap_PS_ServiceNotAvailable_t));
                FREEMEM(s1apPSServiceNotAvailable_p);
                s1apPSServiceNotAvailable_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handoverrequiredies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverRequired, s1ap_HandoverRequired_p);
    return decoded;
}

int s1ap_decode_s1ap_mmeconfigurationupdateacknowledgeies(
    S1ap_MMEConfigurationUpdateAcknowledgeIEs_t *s1ap_MMEConfigurationUpdateAcknowledgeIEs,
    ANY_t *any_p) {

    S1ap_MMEConfigurationUpdateAcknowledge_t  s1ap_MMEConfigurationUpdateAcknowledge;
    S1ap_MMEConfigurationUpdateAcknowledge_t *s1ap_MMEConfigurationUpdateAcknowledge_p = &s1ap_MMEConfigurationUpdateAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_MMEConfigurationUpdateAcknowledgeIEs != NULL);

    memset(s1ap_MMEConfigurationUpdateAcknowledgeIEs, 0, sizeof(S1ap_MMEConfigurationUpdateAcknowledgeIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_MMEConfigurationUpdateAcknowledgeIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_MMEConfigurationUpdateAcknowledge, (void**)&s1ap_MMEConfigurationUpdateAcknowledge_p);

    for (i = 0; i < s1ap_MMEConfigurationUpdateAcknowledge_p->s1ap_MMEConfigurationUpdateAcknowledge_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_MMEConfigurationUpdateAcknowledge_p->s1ap_MMEConfigurationUpdateAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_MMEConfigurationUpdateAcknowledgeIEs->presenceMask |= S1AP_MMECONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_MMEConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_mmeconfigurationupdateacknowledgeies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_MMEConfigurationUpdateAcknowledge, s1ap_MMEConfigurationUpdateAcknowledge_p);
    return decoded;
}

int s1ap_decode_s1ap_killresponseies(
    S1ap_KillResponseIEs_t *s1ap_KillResponseIEs,
    ANY_t *any_p) {

    S1ap_KillResponse_t  s1ap_KillResponse;
    S1ap_KillResponse_t *s1ap_KillResponse_p = &s1ap_KillResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_KillResponseIEs != NULL);

    memset(s1ap_KillResponseIEs, 0, sizeof(S1ap_KillResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_KillResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_KillResponse, (void**)&s1ap_KillResponse_p);

    for (i = 0; i < s1ap_KillResponse_p->s1ap_KillResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_KillResponse_p->s1ap_KillResponse_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MessageIdentifier:
            {
                S1ap_MessageIdentifier_t *s1apMessageIdentifier_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MessageIdentifier, (void**)&s1apMessageIdentifier_p);
                if (tempDecoded < 0 || s1apMessageIdentifier_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE messageIdentifier failed\n");
                    if (s1apMessageIdentifier_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MessageIdentifier, s1apMessageIdentifier_p);
                memcpy(&s1ap_KillResponseIEs->messageIdentifier, s1apMessageIdentifier_p, sizeof(S1ap_MessageIdentifier_t));
                FREEMEM(s1apMessageIdentifier_p);
                s1apMessageIdentifier_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_SerialNumber:
            {
                S1ap_SerialNumber_t *s1apSerialNumber_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SerialNumber, (void**)&s1apSerialNumber_p);
                if (tempDecoded < 0 || s1apSerialNumber_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE serialNumber failed\n");
                    if (s1apSerialNumber_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SerialNumber, s1apSerialNumber_p);
                memcpy(&s1ap_KillResponseIEs->serialNumber, s1apSerialNumber_p, sizeof(S1ap_SerialNumber_t));
                FREEMEM(s1apSerialNumber_p);
                s1apSerialNumber_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_BroadcastCancelledAreaList:
            {
                S1ap_BroadcastCancelledAreaList_t *s1apBroadcastCancelledAreaList_p = NULL;
                s1ap_KillResponseIEs->presenceMask |= S1AP_KILLRESPONSEIES_BROADCASTCANCELLEDAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_BroadcastCancelledAreaList, (void**)&s1apBroadcastCancelledAreaList_p);
                if (tempDecoded < 0 || s1apBroadcastCancelledAreaList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE broadcastCancelledAreaList failed\n");
                    if (s1apBroadcastCancelledAreaList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_BroadcastCancelledAreaList, s1apBroadcastCancelledAreaList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_BroadcastCancelledAreaList, s1apBroadcastCancelledAreaList_p);
                memcpy(&s1ap_KillResponseIEs->broadcastCancelledAreaList, s1apBroadcastCancelledAreaList_p, sizeof(S1ap_BroadcastCancelledAreaList_t));
                FREEMEM(s1apBroadcastCancelledAreaList_p);
                s1apBroadcastCancelledAreaList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_KillResponseIEs->presenceMask |= S1AP_KILLRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_KillResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_killresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_KillResponse, s1ap_KillResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_mmestatustransferies(
    S1ap_MMEStatusTransferIEs_t *s1ap_MMEStatusTransferIEs,
    ANY_t *any_p) {

    S1ap_MMEStatusTransfer_t  s1ap_MMEStatusTransfer;
    S1ap_MMEStatusTransfer_t *s1ap_MMEStatusTransfer_p = &s1ap_MMEStatusTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_MMEStatusTransferIEs != NULL);

    memset(s1ap_MMEStatusTransferIEs, 0, sizeof(S1ap_MMEStatusTransferIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_MMEStatusTransferIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_MMEStatusTransfer, (void**)&s1ap_MMEStatusTransfer_p);

    for (i = 0; i < s1ap_MMEStatusTransfer_p->s1ap_MMEStatusTransfer_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_MMEStatusTransfer_p->s1ap_MMEStatusTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_MMEStatusTransferIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_MMEStatusTransferIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer:
            {
                S1ap_ENB_StatusTransfer_TransparentContainer_t *s1apENBStatusTransferTransparentContainer_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, (void**)&s1apENBStatusTransferTransparentContainer_p);
                if (tempDecoded < 0 || s1apENBStatusTransferTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_StatusTransfer_TransparentContainer failed\n");
                    if (s1apENBStatusTransferTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, s1apENBStatusTransferTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, s1apENBStatusTransferTransparentContainer_p);
                memcpy(&s1ap_MMEStatusTransferIEs->eNB_StatusTransfer_TransparentContainer, s1apENBStatusTransferTransparentContainer_p, sizeof(S1ap_ENB_StatusTransfer_TransparentContainer_t));
                FREEMEM(s1apENBStatusTransferTransparentContainer_p);
                s1apENBStatusTransferTransparentContainer_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_mmestatustransferies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_MMEStatusTransfer, s1ap_MMEStatusTransfer_p);
    return decoded;
}

int s1ap_decode_s1ap_mmeconfigurationupdateies(
    S1ap_MMEConfigurationUpdateIEs_t *s1ap_MMEConfigurationUpdateIEs,
    ANY_t *any_p) {

    S1ap_MMEConfigurationUpdate_t  s1ap_MMEConfigurationUpdate;
    S1ap_MMEConfigurationUpdate_t *s1ap_MMEConfigurationUpdate_p = &s1ap_MMEConfigurationUpdate;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_MMEConfigurationUpdateIEs != NULL);

    memset(s1ap_MMEConfigurationUpdateIEs, 0, sizeof(S1ap_MMEConfigurationUpdateIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_MMEConfigurationUpdateIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_MMEConfigurationUpdate, (void**)&s1ap_MMEConfigurationUpdate_p);

    for (i = 0; i < s1ap_MMEConfigurationUpdate_p->s1ap_MMEConfigurationUpdate_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_MMEConfigurationUpdate_p->s1ap_MMEConfigurationUpdate_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_MMEname:
            {
                S1ap_MMEname_t *s1apMMEname_p = NULL;
                s1ap_MMEConfigurationUpdateIEs->presenceMask |= S1AP_MMECONFIGURATIONUPDATEIES_MMENAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MMEname, (void**)&s1apMMEname_p);
                if (tempDecoded < 0 || s1apMMEname_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mmEname failed\n");
                    if (s1apMMEname_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MMEname, s1apMMEname_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MMEname, s1apMMEname_p);
                memcpy(&s1ap_MMEConfigurationUpdateIEs->mmEname, s1apMMEname_p, sizeof(S1ap_MMEname_t));
                FREEMEM(s1apMMEname_p);
                s1apMMEname_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_ServedGUMMEIs:
            {
                S1ap_ServedGUMMEIs_t *s1apServedGUMMEIs_p = NULL;
                s1ap_MMEConfigurationUpdateIEs->presenceMask |= S1AP_MMECONFIGURATIONUPDATEIES_SERVEDGUMMEIS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ServedGUMMEIs, (void**)&s1apServedGUMMEIs_p);
                if (tempDecoded < 0 || s1apServedGUMMEIs_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE servedGUMMEIs failed\n");
                    if (s1apServedGUMMEIs_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ServedGUMMEIs, s1apServedGUMMEIs_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ServedGUMMEIs, s1apServedGUMMEIs_p);
                memcpy(&s1ap_MMEConfigurationUpdateIEs->servedGUMMEIs, s1apServedGUMMEIs_p, sizeof(S1ap_ServedGUMMEIs_t));
                FREEMEM(s1apServedGUMMEIs_p);
                s1apServedGUMMEIs_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_RelativeMMECapacity:
            {
                S1ap_RelativeMMECapacity_t *s1apRelativeMMECapacity_p = NULL;
                s1ap_MMEConfigurationUpdateIEs->presenceMask |= S1AP_MMECONFIGURATIONUPDATEIES_RELATIVEMMECAPACITY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RelativeMMECapacity, (void**)&s1apRelativeMMECapacity_p);
                if (tempDecoded < 0 || s1apRelativeMMECapacity_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE relativeMMECapacity failed\n");
                    if (s1apRelativeMMECapacity_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RelativeMMECapacity, s1apRelativeMMECapacity_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RelativeMMECapacity, s1apRelativeMMECapacity_p);
                memcpy(&s1ap_MMEConfigurationUpdateIEs->relativeMMECapacity, s1apRelativeMMECapacity_p, sizeof(S1ap_RelativeMMECapacity_t));
                FREEMEM(s1apRelativeMMECapacity_p);
                s1apRelativeMMECapacity_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_mmeconfigurationupdateies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_MMEConfigurationUpdate, s1ap_MMEConfigurationUpdate_p);
    return decoded;
}

int s1ap_decode_s1ap_e_rabmodifyrequesties(
    S1ap_E_RABModifyRequestIEs_t *s1ap_E_RABModifyRequestIEs,
    ANY_t *any_p) {

    S1ap_E_RABModifyRequest_t  s1ap_E_RABModifyRequest;
    S1ap_E_RABModifyRequest_t *s1ap_E_RABModifyRequest_p = &s1ap_E_RABModifyRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_E_RABModifyRequestIEs != NULL);

    memset(s1ap_E_RABModifyRequestIEs, 0, sizeof(S1ap_E_RABModifyRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_E_RABModifyRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_E_RABModifyRequest, (void**)&s1ap_E_RABModifyRequest_p);

    for (i = 0; i < s1ap_E_RABModifyRequest_p->s1ap_E_RABModifyRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_E_RABModifyRequest_p->s1ap_E_RABModifyRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_E_RABModifyRequestIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_E_RABModifyRequestIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                S1ap_UEAggregateMaximumBitrate_t *s1apUEAggregateMaximumBitrate_p = NULL;
                s1ap_E_RABModifyRequestIEs->presenceMask |= S1AP_E_RABMODIFYREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEAggregateMaximumBitrate, (void**)&s1apUEAggregateMaximumBitrate_p);
                if (tempDecoded < 0 || s1apUEAggregateMaximumBitrate_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uEaggregateMaximumBitrate failed\n");
                    if (s1apUEAggregateMaximumBitrate_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                memcpy(&s1ap_E_RABModifyRequestIEs->uEaggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p, sizeof(S1ap_UEAggregateMaximumBitrate_t));
                FREEMEM(s1apUEAggregateMaximumBitrate_p);
                s1apUEAggregateMaximumBitrate_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABToBeModifiedListBearerModReq:
            {
                S1ap_E_RABToBeModifiedListBearerModReq_t *s1apERABToBeModifiedListBearerModReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeModifiedListBearerModReq, (void**)&s1apERABToBeModifiedListBearerModReq_p);
                if (tempDecoded < 0 || s1apERABToBeModifiedListBearerModReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeModifiedListBearerModReq failed\n");
                    if (s1apERABToBeModifiedListBearerModReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeModifiedListBearerModReq, s1apERABToBeModifiedListBearerModReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeModifiedListBearerModReq, s1apERABToBeModifiedListBearerModReq_p);
                if (s1ap_decode_s1ap_e_rabtobemodifiedlistbearermodreq(&s1ap_E_RABModifyRequestIEs->e_RABToBeModifiedListBearerModReq, s1apERABToBeModifiedListBearerModReq_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABToBeModifiedListBearerModReq failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeModifiedListBearerModReq, s1apERABToBeModifiedListBearerModReq_p);
                }
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_e_rabmodifyrequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABModifyRequest, s1ap_E_RABModifyRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_downlinknastransporties(
    S1ap_DownlinkNASTransportIEs_t *s1ap_DownlinkNASTransportIEs,
    ANY_t *any_p) {

    S1ap_DownlinkNASTransport_t  s1ap_DownlinkNASTransport;
    S1ap_DownlinkNASTransport_t *s1ap_DownlinkNASTransport_p = &s1ap_DownlinkNASTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_DownlinkNASTransportIEs != NULL);

    memset(s1ap_DownlinkNASTransportIEs, 0, sizeof(S1ap_DownlinkNASTransportIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_DownlinkNASTransportIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_DownlinkNASTransport, (void**)&s1ap_DownlinkNASTransport_p);

    for (i = 0; i < s1ap_DownlinkNASTransport_p->s1ap_DownlinkNASTransport_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_DownlinkNASTransport_p->s1ap_DownlinkNASTransport_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_DownlinkNASTransportIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_DownlinkNASTransportIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_NAS_PDU:
            {
                S1ap_NAS_PDU_t *s1apNASPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_NAS_PDU, (void**)&s1apNASPDU_p);
                if (tempDecoded < 0 || s1apNASPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE nas_pdu failed\n");
                    if (s1apNASPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                memcpy(&s1ap_DownlinkNASTransportIEs->nas_pdu, s1apNASPDU_p, sizeof(S1ap_NAS_PDU_t));
                FREEMEM(s1apNASPDU_p);
                s1apNASPDU_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_HandoverRestrictionList:
            {
                S1ap_HandoverRestrictionList_t *s1apHandoverRestrictionList_p = NULL;
                s1ap_DownlinkNASTransportIEs->presenceMask |= S1AP_DOWNLINKNASTRANSPORTIES_HANDOVERRESTRICTIONLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_HandoverRestrictionList, (void**)&s1apHandoverRestrictionList_p);
                if (tempDecoded < 0 || s1apHandoverRestrictionList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE handoverRestrictionList failed\n");
                    if (s1apHandoverRestrictionList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverRestrictionList, s1apHandoverRestrictionList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_HandoverRestrictionList, s1apHandoverRestrictionList_p);
                memcpy(&s1ap_DownlinkNASTransportIEs->handoverRestrictionList, s1apHandoverRestrictionList_p, sizeof(S1ap_HandoverRestrictionList_t));
                FREEMEM(s1apHandoverRestrictionList_p);
                s1apHandoverRestrictionList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SubscriberProfileIDforRFP:
            {
                S1ap_SubscriberProfileIDforRFP_t *s1apSubscriberProfileIDforRFP_p = NULL;
                s1ap_DownlinkNASTransportIEs->presenceMask |= S1AP_DOWNLINKNASTRANSPORTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SubscriberProfileIDforRFP, (void**)&s1apSubscriberProfileIDforRFP_p);
                if (tempDecoded < 0 || s1apSubscriberProfileIDforRFP_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE subscriberProfileIDforRFP failed\n");
                    if (s1apSubscriberProfileIDforRFP_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SubscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SubscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p);
                memcpy(&s1ap_DownlinkNASTransportIEs->subscriberProfileIDforRFP, s1apSubscriberProfileIDforRFP_p, sizeof(S1ap_SubscriberProfileIDforRFP_t));
                FREEMEM(s1apSubscriberProfileIDforRFP_p);
                s1apSubscriberProfileIDforRFP_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_downlinknastransporties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_DownlinkNASTransport, s1ap_DownlinkNASTransport_p);
    return decoded;
}

int s1ap_decode_s1ap_enbconfigurationupdatefailureies(
    S1ap_ENBConfigurationUpdateFailureIEs_t *s1ap_ENBConfigurationUpdateFailureIEs,
    ANY_t *any_p) {

    S1ap_ENBConfigurationUpdateFailure_t  s1ap_ENBConfigurationUpdateFailure;
    S1ap_ENBConfigurationUpdateFailure_t *s1ap_ENBConfigurationUpdateFailure_p = &s1ap_ENBConfigurationUpdateFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ENBConfigurationUpdateFailureIEs != NULL);

    memset(s1ap_ENBConfigurationUpdateFailureIEs, 0, sizeof(S1ap_ENBConfigurationUpdateFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ENBConfigurationUpdateFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ENBConfigurationUpdateFailure, (void**)&s1ap_ENBConfigurationUpdateFailure_p);

    for (i = 0; i < s1ap_ENBConfigurationUpdateFailure_p->s1ap_ENBConfigurationUpdateFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ENBConfigurationUpdateFailure_p->s1ap_ENBConfigurationUpdateFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_ENBConfigurationUpdateFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_TimeToWait:
            {
                S1ap_TimeToWait_t *s1apTimeToWait_p = NULL;
                s1ap_ENBConfigurationUpdateFailureIEs->presenceMask |= S1AP_ENBCONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TimeToWait, (void**)&s1apTimeToWait_p);
                if (tempDecoded < 0 || s1apTimeToWait_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE timeToWait failed\n");
                    if (s1apTimeToWait_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TimeToWait, s1apTimeToWait_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TimeToWait, s1apTimeToWait_p);
                memcpy(&s1ap_ENBConfigurationUpdateFailureIEs->timeToWait, s1apTimeToWait_p, sizeof(S1ap_TimeToWait_t));
                FREEMEM(s1apTimeToWait_p);
                s1apTimeToWait_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_ENBConfigurationUpdateFailureIEs->presenceMask |= S1AP_ENBCONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_ENBConfigurationUpdateFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_enbconfigurationupdatefailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ENBConfigurationUpdateFailure, s1ap_ENBConfigurationUpdateFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_uecontextreleasecompleteies(
    S1ap_UEContextReleaseCompleteIEs_t *s1ap_UEContextReleaseCompleteIEs,
    ANY_t *any_p) {

    S1ap_UEContextReleaseComplete_t  s1ap_UEContextReleaseComplete;
    S1ap_UEContextReleaseComplete_t *s1ap_UEContextReleaseComplete_p = &s1ap_UEContextReleaseComplete;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UEContextReleaseCompleteIEs != NULL);

    memset(s1ap_UEContextReleaseCompleteIEs, 0, sizeof(S1ap_UEContextReleaseCompleteIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UEContextReleaseCompleteIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UEContextReleaseComplete, (void**)&s1ap_UEContextReleaseComplete_p);

    for (i = 0; i < s1ap_UEContextReleaseComplete_p->s1ap_UEContextReleaseComplete_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UEContextReleaseComplete_p->s1ap_UEContextReleaseComplete_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UEContextReleaseCompleteIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UEContextReleaseCompleteIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_UEContextReleaseCompleteIEs->presenceMask |= S1AP_UECONTEXTRELEASECOMPLETEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_UEContextReleaseCompleteIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uecontextreleasecompleteies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UEContextReleaseComplete, s1ap_UEContextReleaseComplete_p);
    return decoded;
}

int s1ap_decode_s1ap_reseties(
    S1ap_ResetIEs_t *s1ap_ResetIEs,
    ANY_t *any_p) {

    S1ap_Reset_t  s1ap_Reset;
    S1ap_Reset_t *s1ap_Reset_p = &s1ap_Reset;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ResetIEs != NULL);

    memset(s1ap_ResetIEs, 0, sizeof(S1ap_ResetIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ResetIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_Reset, (void**)&s1ap_Reset_p);

    for (i = 0; i < s1ap_Reset_p->s1ap_Reset_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_Reset_p->s1ap_Reset_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_ResetIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_ResetType:
            {
                S1ap_ResetType_t *s1apResetType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ResetType, (void**)&s1apResetType_p);
                if (tempDecoded < 0 || s1apResetType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE resetType failed\n");
                    if (s1apResetType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ResetType, s1apResetType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ResetType, s1apResetType_p);
                memcpy(&s1ap_ResetIEs->resetType, s1apResetType_p, sizeof(S1ap_ResetType_t));
                FREEMEM(s1apResetType_p);
                s1apResetType_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_reseties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_Reset, s1ap_Reset_p);
    return decoded;
}

int s1ap_decode_s1ap_tracestarties(
    S1ap_TraceStartIEs_t *s1ap_TraceStartIEs,
    ANY_t *any_p) {

    S1ap_TraceStart_t  s1ap_TraceStart;
    S1ap_TraceStart_t *s1ap_TraceStart_p = &s1ap_TraceStart;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_TraceStartIEs != NULL);

    memset(s1ap_TraceStartIEs, 0, sizeof(S1ap_TraceStartIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_TraceStartIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_TraceStart, (void**)&s1ap_TraceStart_p);

    for (i = 0; i < s1ap_TraceStart_p->s1ap_TraceStart_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_TraceStart_p->s1ap_TraceStart_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_TraceStartIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_TraceStartIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TraceActivation:
            {
                S1ap_TraceActivation_t *s1apTraceActivation_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TraceActivation, (void**)&s1apTraceActivation_p);
                if (tempDecoded < 0 || s1apTraceActivation_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE traceActivation failed\n");
                    if (s1apTraceActivation_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TraceActivation, s1apTraceActivation_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TraceActivation, s1apTraceActivation_p);
                memcpy(&s1ap_TraceStartIEs->traceActivation, s1apTraceActivation_p, sizeof(S1ap_TraceActivation_t));
                FREEMEM(s1apTraceActivation_p);
                s1apTraceActivation_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_tracestarties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_TraceStart, s1ap_TraceStart_p);
    return decoded;
}

int s1ap_decode_s1ap_s1setupresponseies(
    S1ap_S1SetupResponseIEs_t *s1ap_S1SetupResponseIEs,
    ANY_t *any_p) {

    S1ap_S1SetupResponse_t  s1ap_S1SetupResponse;
    S1ap_S1SetupResponse_t *s1ap_S1SetupResponse_p = &s1ap_S1SetupResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_S1SetupResponseIEs != NULL);

    memset(s1ap_S1SetupResponseIEs, 0, sizeof(S1ap_S1SetupResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_S1SetupResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_S1SetupResponse, (void**)&s1ap_S1SetupResponse_p);

    for (i = 0; i < s1ap_S1SetupResponse_p->s1ap_S1SetupResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_S1SetupResponse_p->s1ap_S1SetupResponse_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_MMEname:
            {
                S1ap_MMEname_t *s1apMMEname_p = NULL;
                s1ap_S1SetupResponseIEs->presenceMask |= S1AP_S1SETUPRESPONSEIES_MMENAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MMEname, (void**)&s1apMMEname_p);
                if (tempDecoded < 0 || s1apMMEname_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mmEname failed\n");
                    if (s1apMMEname_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MMEname, s1apMMEname_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MMEname, s1apMMEname_p);
                memcpy(&s1ap_S1SetupResponseIEs->mmEname, s1apMMEname_p, sizeof(S1ap_MMEname_t));
                FREEMEM(s1apMMEname_p);
                s1apMMEname_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_ServedGUMMEIs:
            {
                S1ap_ServedGUMMEIs_t *s1apServedGUMMEIs_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ServedGUMMEIs, (void**)&s1apServedGUMMEIs_p);
                if (tempDecoded < 0 || s1apServedGUMMEIs_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE servedGUMMEIs failed\n");
                    if (s1apServedGUMMEIs_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ServedGUMMEIs, s1apServedGUMMEIs_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ServedGUMMEIs, s1apServedGUMMEIs_p);
                memcpy(&s1ap_S1SetupResponseIEs->servedGUMMEIs, s1apServedGUMMEIs_p, sizeof(S1ap_ServedGUMMEIs_t));
                FREEMEM(s1apServedGUMMEIs_p);
                s1apServedGUMMEIs_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_RelativeMMECapacity:
            {
                S1ap_RelativeMMECapacity_t *s1apRelativeMMECapacity_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_RelativeMMECapacity, (void**)&s1apRelativeMMECapacity_p);
                if (tempDecoded < 0 || s1apRelativeMMECapacity_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE relativeMMECapacity failed\n");
                    if (s1apRelativeMMECapacity_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_RelativeMMECapacity, s1apRelativeMMECapacity_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_RelativeMMECapacity, s1apRelativeMMECapacity_p);
                memcpy(&s1ap_S1SetupResponseIEs->relativeMMECapacity, s1apRelativeMMECapacity_p, sizeof(S1ap_RelativeMMECapacity_t));
                FREEMEM(s1apRelativeMMECapacity_p);
                s1apRelativeMMECapacity_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_MMERelaySupportIndicator:
            {
                S1ap_MMERelaySupportIndicator_t *s1apMMERelaySupportIndicator_p = NULL;
                s1ap_S1SetupResponseIEs->presenceMask |= S1AP_S1SETUPRESPONSEIES_MMERELAYSUPPORTINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MMERelaySupportIndicator, (void**)&s1apMMERelaySupportIndicator_p);
                if (tempDecoded < 0 || s1apMMERelaySupportIndicator_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mmeRelaySupportIndicator failed\n");
                    if (s1apMMERelaySupportIndicator_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MMERelaySupportIndicator, s1apMMERelaySupportIndicator_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MMERelaySupportIndicator, s1apMMERelaySupportIndicator_p);
                memcpy(&s1ap_S1SetupResponseIEs->mmeRelaySupportIndicator, s1apMMERelaySupportIndicator_p, sizeof(S1ap_MMERelaySupportIndicator_t));
                FREEMEM(s1apMMERelaySupportIndicator_p);
                s1apMMERelaySupportIndicator_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_S1SetupResponseIEs->presenceMask |= S1AP_S1SETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_S1SetupResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_s1setupresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_S1SetupResponse, s1ap_S1SetupResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_pagingies(
    S1ap_PagingIEs_t *s1ap_PagingIEs,
    ANY_t *any_p) {

    S1ap_Paging_t  s1ap_Paging;
    S1ap_Paging_t *s1ap_Paging_p = &s1ap_Paging;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_PagingIEs != NULL);

    memset(s1ap_PagingIEs, 0, sizeof(S1ap_PagingIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_PagingIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_Paging, (void**)&s1ap_Paging_p);

    for (i = 0; i < s1ap_Paging_p->s1ap_Paging_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_Paging_p->s1ap_Paging_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_UEIdentityIndexValue:
            {
                S1ap_UEIdentityIndexValue_t *s1apUEIdentityIndexValue_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEIdentityIndexValue, (void**)&s1apUEIdentityIndexValue_p);
                if (tempDecoded < 0 || s1apUEIdentityIndexValue_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE ueIdentityIndexValue failed\n");
                    if (s1apUEIdentityIndexValue_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEIdentityIndexValue, s1apUEIdentityIndexValue_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEIdentityIndexValue, s1apUEIdentityIndexValue_p);
                memcpy(&s1ap_PagingIEs->ueIdentityIndexValue, s1apUEIdentityIndexValue_p, sizeof(S1ap_UEIdentityIndexValue_t));
                FREEMEM(s1apUEIdentityIndexValue_p);
                s1apUEIdentityIndexValue_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_UEPagingID:
            {
                S1ap_UEPagingID_t *s1apUEPagingID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEPagingID, (void**)&s1apUEPagingID_p);
                if (tempDecoded < 0 || s1apUEPagingID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uePagingID failed\n");
                    if (s1apUEPagingID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEPagingID, s1apUEPagingID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEPagingID, s1apUEPagingID_p);
                memcpy(&s1ap_PagingIEs->uePagingID, s1apUEPagingID_p, sizeof(S1ap_UEPagingID_t));
                FREEMEM(s1apUEPagingID_p);
                s1apUEPagingID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_pagingDRX:
            {
                S1ap_PagingDRX_t *s1apPagingDRX_p = NULL;
                s1ap_PagingIEs->presenceMask |= S1AP_PAGINGIES_PAGINGDRX_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_PagingDRX, (void**)&s1apPagingDRX_p);
                if (tempDecoded < 0 || s1apPagingDRX_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE pagingDRX failed\n");
                    if (s1apPagingDRX_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_PagingDRX, s1apPagingDRX_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_PagingDRX, s1apPagingDRX_p);
                memcpy(&s1ap_PagingIEs->pagingDRX, s1apPagingDRX_p, sizeof(S1ap_PagingDRX_t));
                FREEMEM(s1apPagingDRX_p);
                s1apPagingDRX_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_CNDomain:
            {
                S1ap_CNDomain_t *s1apCNDomain_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CNDomain, (void**)&s1apCNDomain_p);
                if (tempDecoded < 0 || s1apCNDomain_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cnDomain failed\n");
                    if (s1apCNDomain_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CNDomain, s1apCNDomain_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CNDomain, s1apCNDomain_p);
                memcpy(&s1ap_PagingIEs->cnDomain, s1apCNDomain_p, sizeof(S1ap_CNDomain_t));
                FREEMEM(s1apCNDomain_p);
                s1apCNDomain_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TAIList:
            {
                S1ap_TAIList_t *s1apTAIList_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TAIList, (void**)&s1apTAIList_p);
                if (tempDecoded < 0 || s1apTAIList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE taiList failed\n");
                    if (s1apTAIList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TAIList, s1apTAIList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TAIList, s1apTAIList_p);
                if (s1ap_decode_s1ap_tailist(&s1ap_PagingIEs->taiList, s1apTAIList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apTAIList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_TAIList, s1apTAIList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_IdList:
            {
                S1ap_CSG_IdList_t *s1apCSGIdList_p = NULL;
                s1ap_PagingIEs->presenceMask |= S1AP_PAGINGIES_CSG_IDLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_IdList, (void**)&s1apCSGIdList_p);
                if (tempDecoded < 0 || s1apCSGIdList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_IdList failed\n");
                    if (s1apCSGIdList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_IdList, s1apCSGIdList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_IdList, s1apCSGIdList_p);
                memcpy(&s1ap_PagingIEs->csG_IdList, s1apCSGIdList_p, sizeof(S1ap_CSG_IdList_t));
                FREEMEM(s1apCSGIdList_p);
                s1apCSGIdList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_PagingPriority:
            {
                S1ap_PagingPriority_t *s1apPagingPriority_p = NULL;
                s1ap_PagingIEs->presenceMask |= S1AP_PAGINGIES_PAGINGPRIORITY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_PagingPriority, (void**)&s1apPagingPriority_p);
                if (tempDecoded < 0 || s1apPagingPriority_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE pagingPriority failed\n");
                    if (s1apPagingPriority_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_PagingPriority, s1apPagingPriority_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_PagingPriority, s1apPagingPriority_p);
                memcpy(&s1ap_PagingIEs->pagingPriority, s1apPagingPriority_p, sizeof(S1ap_PagingPriority_t));
                FREEMEM(s1apPagingPriority_p);
                s1apPagingPriority_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_pagingies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_Paging, s1ap_Paging_p);
    return decoded;
}

int s1ap_decode_s1ap_enbconfigurationupdateacknowledgeies(
    S1ap_ENBConfigurationUpdateAcknowledgeIEs_t *s1ap_ENBConfigurationUpdateAcknowledgeIEs,
    ANY_t *any_p) {

    S1ap_ENBConfigurationUpdateAcknowledge_t  s1ap_ENBConfigurationUpdateAcknowledge;
    S1ap_ENBConfigurationUpdateAcknowledge_t *s1ap_ENBConfigurationUpdateAcknowledge_p = &s1ap_ENBConfigurationUpdateAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ENBConfigurationUpdateAcknowledgeIEs != NULL);

    memset(s1ap_ENBConfigurationUpdateAcknowledgeIEs, 0, sizeof(S1ap_ENBConfigurationUpdateAcknowledgeIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ENBConfigurationUpdateAcknowledgeIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ENBConfigurationUpdateAcknowledge, (void**)&s1ap_ENBConfigurationUpdateAcknowledge_p);

    for (i = 0; i < s1ap_ENBConfigurationUpdateAcknowledge_p->s1ap_ENBConfigurationUpdateAcknowledge_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ENBConfigurationUpdateAcknowledge_p->s1ap_ENBConfigurationUpdateAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_ENBConfigurationUpdateAcknowledgeIEs->presenceMask |= S1AP_ENBCONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_ENBConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_enbconfigurationupdateacknowledgeies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ENBConfigurationUpdateAcknowledge, s1ap_ENBConfigurationUpdateAcknowledge_p);
    return decoded;
}

int s1ap_decode_s1ap_pathswitchrequestfailureies(
    S1ap_PathSwitchRequestFailureIEs_t *s1ap_PathSwitchRequestFailureIEs,
    ANY_t *any_p) {

    S1ap_PathSwitchRequestFailure_t  s1ap_PathSwitchRequestFailure;
    S1ap_PathSwitchRequestFailure_t *s1ap_PathSwitchRequestFailure_p = &s1ap_PathSwitchRequestFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_PathSwitchRequestFailureIEs != NULL);

    memset(s1ap_PathSwitchRequestFailureIEs, 0, sizeof(S1ap_PathSwitchRequestFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_PathSwitchRequestFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_PathSwitchRequestFailure, (void**)&s1ap_PathSwitchRequestFailure_p);

    for (i = 0; i < s1ap_PathSwitchRequestFailure_p->s1ap_PathSwitchRequestFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_PathSwitchRequestFailure_p->s1ap_PathSwitchRequestFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_PathSwitchRequestFailureIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_PathSwitchRequestFailureIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_PathSwitchRequestFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_PathSwitchRequestFailureIEs->presenceMask |= S1AP_PATHSWITCHREQUESTFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_PathSwitchRequestFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_pathswitchrequestfailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_PathSwitchRequestFailure, s1ap_PathSwitchRequestFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_e_rabmodifyresponseies(
    S1ap_E_RABModifyResponseIEs_t *s1ap_E_RABModifyResponseIEs,
    ANY_t *any_p) {

    S1ap_E_RABModifyResponse_t  s1ap_E_RABModifyResponse;
    S1ap_E_RABModifyResponse_t *s1ap_E_RABModifyResponse_p = &s1ap_E_RABModifyResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_E_RABModifyResponseIEs != NULL);

    memset(s1ap_E_RABModifyResponseIEs, 0, sizeof(S1ap_E_RABModifyResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_E_RABModifyResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_E_RABModifyResponse, (void**)&s1ap_E_RABModifyResponse_p);

    for (i = 0; i < s1ap_E_RABModifyResponse_p->s1ap_E_RABModifyResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_E_RABModifyResponse_p->s1ap_E_RABModifyResponse_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_E_RABModifyResponseIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_E_RABModifyResponseIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABModifyListBearerModRes:
            {
                S1ap_E_RABModifyListBearerModRes_t *s1apERABModifyListBearerModRes_p = NULL;
                s1ap_E_RABModifyResponseIEs->presenceMask |= S1AP_E_RABMODIFYRESPONSEIES_E_RABMODIFYLISTBEARERMODRES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABModifyListBearerModRes, (void**)&s1apERABModifyListBearerModRes_p);
                if (tempDecoded < 0 || s1apERABModifyListBearerModRes_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABModifyListBearerModRes failed\n");
                    if (s1apERABModifyListBearerModRes_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABModifyListBearerModRes, s1apERABModifyListBearerModRes_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABModifyListBearerModRes, s1apERABModifyListBearerModRes_p);
                if (s1ap_decode_s1ap_e_rabmodifylistbearermodres(&s1ap_E_RABModifyResponseIEs->e_RABModifyListBearerModRes, s1apERABModifyListBearerModRes_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABModifyListBearerModRes failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABModifyListBearerModRes, s1apERABModifyListBearerModRes_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABFailedToModifyList:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                s1ap_E_RABModifyResponseIEs->presenceMask |= S1AP_E_RABMODIFYRESPONSEIES_E_RABFAILEDTOMODIFYLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABFailedToModifyList failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_E_RABModifyResponseIEs->e_RABFailedToModifyList, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_E_RABModifyResponseIEs->presenceMask |= S1AP_E_RABMODIFYRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_E_RABModifyResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_e_rabmodifyresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABModifyResponse, s1ap_E_RABModifyResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_handovernotifyies(
    S1ap_HandoverNotifyIEs_t *s1ap_HandoverNotifyIEs,
    ANY_t *any_p) {

    S1ap_HandoverNotify_t  s1ap_HandoverNotify;
    S1ap_HandoverNotify_t *s1ap_HandoverNotify_p = &s1ap_HandoverNotify;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverNotifyIEs != NULL);

    memset(s1ap_HandoverNotifyIEs, 0, sizeof(S1ap_HandoverNotifyIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverNotifyIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverNotify, (void**)&s1ap_HandoverNotify_p);

    for (i = 0; i < s1ap_HandoverNotify_p->s1ap_HandoverNotify_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverNotify_p->s1ap_HandoverNotify_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverNotifyIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_HandoverNotifyIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_EUTRAN_CGI:
            {
                S1ap_EUTRAN_CGI_t *s1apEUTRANCGI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_EUTRAN_CGI, (void**)&s1apEUTRANCGI_p);
                if (tempDecoded < 0 || s1apEUTRANCGI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eutran_cgi failed\n");
                    if (s1apEUTRANCGI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                memcpy(&s1ap_HandoverNotifyIEs->eutran_cgi, s1apEUTRANCGI_p, sizeof(S1ap_EUTRAN_CGI_t));
                FREEMEM(s1apEUTRANCGI_p);
                s1apEUTRANCGI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TAI:
            {
                S1ap_TAI_t *s1apTAI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TAI, (void**)&s1apTAI_p);
                if (tempDecoded < 0 || s1apTAI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE tai failed\n");
                    if (s1apTAI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TAI, s1apTAI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TAI, s1apTAI_p);
                memcpy(&s1ap_HandoverNotifyIEs->tai, s1apTAI_p, sizeof(S1ap_TAI_t));
                FREEMEM(s1apTAI_p);
                s1apTAI_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handovernotifyies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverNotify, s1ap_HandoverNotify_p);
    return decoded;
}

int s1ap_decode_s1ap_handovercancelacknowledgeies(
    S1ap_HandoverCancelAcknowledgeIEs_t *s1ap_HandoverCancelAcknowledgeIEs,
    ANY_t *any_p) {

    S1ap_HandoverCancelAcknowledge_t  s1ap_HandoverCancelAcknowledge;
    S1ap_HandoverCancelAcknowledge_t *s1ap_HandoverCancelAcknowledge_p = &s1ap_HandoverCancelAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverCancelAcknowledgeIEs != NULL);

    memset(s1ap_HandoverCancelAcknowledgeIEs, 0, sizeof(S1ap_HandoverCancelAcknowledgeIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverCancelAcknowledgeIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverCancelAcknowledge, (void**)&s1ap_HandoverCancelAcknowledge_p);

    for (i = 0; i < s1ap_HandoverCancelAcknowledge_p->s1ap_HandoverCancelAcknowledge_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverCancelAcknowledge_p->s1ap_HandoverCancelAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverCancelAcknowledgeIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_HandoverCancelAcknowledgeIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_HandoverCancelAcknowledgeIEs->presenceMask |= S1AP_HANDOVERCANCELACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_HandoverCancelAcknowledgeIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handovercancelacknowledgeies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverCancelAcknowledge, s1ap_HandoverCancelAcknowledge_p);
    return decoded;
}

int s1ap_decode_s1ap_initialcontextsetupfailureies(
    S1ap_InitialContextSetupFailureIEs_t *s1ap_InitialContextSetupFailureIEs,
    ANY_t *any_p) {

    S1ap_InitialContextSetupFailure_t  s1ap_InitialContextSetupFailure;
    S1ap_InitialContextSetupFailure_t *s1ap_InitialContextSetupFailure_p = &s1ap_InitialContextSetupFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_InitialContextSetupFailureIEs != NULL);

    memset(s1ap_InitialContextSetupFailureIEs, 0, sizeof(S1ap_InitialContextSetupFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_InitialContextSetupFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_InitialContextSetupFailure, (void**)&s1ap_InitialContextSetupFailure_p);

    for (i = 0; i < s1ap_InitialContextSetupFailure_p->s1ap_InitialContextSetupFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_InitialContextSetupFailure_p->s1ap_InitialContextSetupFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_InitialContextSetupFailureIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_InitialContextSetupFailureIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_InitialContextSetupFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_InitialContextSetupFailureIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_InitialContextSetupFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_initialcontextsetupfailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_InitialContextSetupFailure, s1ap_InitialContextSetupFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_handovercommandies(
    S1ap_HandoverCommandIEs_t *s1ap_HandoverCommandIEs,
    ANY_t *any_p) {

    S1ap_HandoverCommand_t  s1ap_HandoverCommand;
    S1ap_HandoverCommand_t *s1ap_HandoverCommand_p = &s1ap_HandoverCommand;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverCommandIEs != NULL);

    memset(s1ap_HandoverCommandIEs, 0, sizeof(S1ap_HandoverCommandIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverCommandIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverCommand, (void**)&s1ap_HandoverCommand_p);

    for (i = 0; i < s1ap_HandoverCommand_p->s1ap_HandoverCommand_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverCommand_p->s1ap_HandoverCommand_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverCommandIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_HandoverCommandIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_HandoverType:
            {
                S1ap_HandoverType_t *s1apHandoverType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_HandoverType, (void**)&s1apHandoverType_p);
                if (tempDecoded < 0 || s1apHandoverType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE handoverType failed\n");
                    if (s1apHandoverType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverType, s1apHandoverType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_HandoverType, s1apHandoverType_p);
                memcpy(&s1ap_HandoverCommandIEs->handoverType, s1apHandoverType_p, sizeof(S1ap_HandoverType_t));
                FREEMEM(s1apHandoverType_p);
                s1apHandoverType_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABSubjecttoDataForwardingList:
            {
                S1ap_E_RABDataForwardingList_t *s1apERABDataForwardingList_p = NULL;
                s1ap_HandoverCommandIEs->presenceMask |= S1AP_HANDOVERCOMMANDIES_E_RABSUBJECTTODATAFORWARDINGLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABDataForwardingList, (void**)&s1apERABDataForwardingList_p);
                if (tempDecoded < 0 || s1apERABDataForwardingList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABSubjecttoDataForwardingList failed\n");
                    if (s1apERABDataForwardingList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABDataForwardingList, s1apERABDataForwardingList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABDataForwardingList, s1apERABDataForwardingList_p);
                if (s1ap_decode_s1ap_e_rabdataforwardinglist(&s1ap_HandoverCommandIEs->e_RABSubjecttoDataForwardingList, s1apERABDataForwardingList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABDataForwardingList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABDataForwardingList, s1apERABDataForwardingList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABtoReleaseListHOCmd:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                s1ap_HandoverCommandIEs->presenceMask |= S1AP_HANDOVERCOMMANDIES_E_RABTORELEASELISTHOCMD_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABtoReleaseListHOCmd failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_HandoverCommandIEs->e_RABtoReleaseListHOCmd, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            case S1ap_ProtocolIE_ID_id_Target_ToSource_TransparentContainer:
            {
                S1ap_Target_ToSource_TransparentContainer_t *s1apTargetToSourceTransparentContainer_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Target_ToSource_TransparentContainer, (void**)&s1apTargetToSourceTransparentContainer_p);
                if (tempDecoded < 0 || s1apTargetToSourceTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE target_ToSource_TransparentContainer failed\n");
                    if (s1apTargetToSourceTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p);
                memcpy(&s1ap_HandoverCommandIEs->target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p, sizeof(S1ap_Target_ToSource_TransparentContainer_t));
                FREEMEM(s1apTargetToSourceTransparentContainer_p);
                s1apTargetToSourceTransparentContainer_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_Target_ToSource_TransparentContainer_Secondary:
            {
                S1ap_Target_ToSource_TransparentContainer_t *s1apTargetToSourceTransparentContainer_p = NULL;
                s1ap_HandoverCommandIEs->presenceMask |= S1AP_HANDOVERCOMMANDIES_TARGET_TOSOURCE_TRANSPARENTCONTAINER_SECONDARY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Target_ToSource_TransparentContainer, (void**)&s1apTargetToSourceTransparentContainer_p);
                if (tempDecoded < 0 || s1apTargetToSourceTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE target_ToSource_TransparentContainer_Secondary failed\n");
                    if (s1apTargetToSourceTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p);
                memcpy(&s1ap_HandoverCommandIEs->target_ToSource_TransparentContainer_Secondary, s1apTargetToSourceTransparentContainer_p, sizeof(S1ap_Target_ToSource_TransparentContainer_t));
                FREEMEM(s1apTargetToSourceTransparentContainer_p);
                s1apTargetToSourceTransparentContainer_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_HandoverCommandIEs->presenceMask |= S1AP_HANDOVERCOMMANDIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_HandoverCommandIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handovercommandies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverCommand, s1ap_HandoverCommand_p);
    return decoded;
}

int s1ap_decode_s1ap_downlinknonueassociatedlppatransport_ies(
    S1ap_DownlinkNonUEAssociatedLPPaTransport_IEs_t *s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs,
    ANY_t *any_p) {

    S1ap_DownlinkNonUEAssociatedLPPaTransport_t  s1ap_DownlinkNonUEAssociatedLPPaTransport;
    S1ap_DownlinkNonUEAssociatedLPPaTransport_t *s1ap_DownlinkNonUEAssociatedLPPaTransport_p = &s1ap_DownlinkNonUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs != NULL);

    memset(s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs, 0, sizeof(S1ap_DownlinkNonUEAssociatedLPPaTransport_IEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_DownlinkNonUEAssociatedLPPaTransport_IEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_DownlinkNonUEAssociatedLPPaTransport, (void**)&s1ap_DownlinkNonUEAssociatedLPPaTransport_p);

    for (i = 0; i < s1ap_DownlinkNonUEAssociatedLPPaTransport_p->s1ap_DownlinkNonUEAssociatedLPPaTransport_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_DownlinkNonUEAssociatedLPPaTransport_p->s1ap_DownlinkNonUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Routing_ID:
            {
                S1ap_Routing_ID_t *s1apRoutingID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Routing_ID, (void**)&s1apRoutingID_p);
                if (tempDecoded < 0 || s1apRoutingID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE routing_ID failed\n");
                    if (s1apRoutingID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                memcpy(&s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs->routing_ID, s1apRoutingID_p, sizeof(S1ap_Routing_ID_t));
                FREEMEM(s1apRoutingID_p);
                s1apRoutingID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_LPPa_PDU:
            {
                S1ap_LPPa_PDU_t *s1apLPPaPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_LPPa_PDU, (void**)&s1apLPPaPDU_p);
                if (tempDecoded < 0 || s1apLPPaPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE lpPa_PDU failed\n");
                    if (s1apLPPaPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                memcpy(&s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs->lpPa_PDU, s1apLPPaPDU_p, sizeof(S1ap_LPPa_PDU_t));
                FREEMEM(s1apLPPaPDU_p);
                s1apLPPaPDU_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_downlinknonueassociatedlppatransport_ies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_DownlinkNonUEAssociatedLPPaTransport, s1ap_DownlinkNonUEAssociatedLPPaTransport_p);
    return decoded;
}

int s1ap_decode_s1ap_errorindicationies(
    S1ap_ErrorIndicationIEs_t *s1ap_ErrorIndicationIEs,
    ANY_t *any_p) {

    S1ap_ErrorIndication_t  s1ap_ErrorIndication;
    S1ap_ErrorIndication_t *s1ap_ErrorIndication_p = &s1ap_ErrorIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ErrorIndicationIEs != NULL);

    memset(s1ap_ErrorIndicationIEs, 0, sizeof(S1ap_ErrorIndicationIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ErrorIndicationIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ErrorIndication, (void**)&s1ap_ErrorIndication_p);

    for (i = 0; i < s1ap_ErrorIndication_p->s1ap_ErrorIndication_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ErrorIndication_p->s1ap_ErrorIndication_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                s1ap_ErrorIndicationIEs->presenceMask |= S1AP_ERRORINDICATIONIES_MME_UE_S1AP_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_ErrorIndicationIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                s1ap_ErrorIndicationIEs->presenceMask |= S1AP_ERRORINDICATIONIES_ENB_UE_S1AP_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_ErrorIndicationIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                s1ap_ErrorIndicationIEs->presenceMask |= S1AP_ERRORINDICATIONIES_CAUSE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_ErrorIndicationIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_ErrorIndicationIEs->presenceMask |= S1AP_ERRORINDICATIONIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_ErrorIndicationIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_errorindicationies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ErrorIndication, s1ap_ErrorIndication_p);
    return decoded;
}

int s1ap_decode_s1ap_overloadstarties(
    S1ap_OverloadStartIEs_t *s1ap_OverloadStartIEs,
    ANY_t *any_p) {

    S1ap_OverloadStart_t  s1ap_OverloadStart;
    S1ap_OverloadStart_t *s1ap_OverloadStart_p = &s1ap_OverloadStart;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_OverloadStartIEs != NULL);

    memset(s1ap_OverloadStartIEs, 0, sizeof(S1ap_OverloadStartIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_OverloadStartIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_OverloadStart, (void**)&s1ap_OverloadStart_p);

    for (i = 0; i < s1ap_OverloadStart_p->s1ap_OverloadStart_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_OverloadStart_p->s1ap_OverloadStart_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_OverloadResponse:
            {
                S1ap_OverloadResponse_t *s1apOverloadResponse_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_OverloadResponse, (void**)&s1apOverloadResponse_p);
                if (tempDecoded < 0 || s1apOverloadResponse_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE overloadResponse failed\n");
                    if (s1apOverloadResponse_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_OverloadResponse, s1apOverloadResponse_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_OverloadResponse, s1apOverloadResponse_p);
                memcpy(&s1ap_OverloadStartIEs->overloadResponse, s1apOverloadResponse_p, sizeof(S1ap_OverloadResponse_t));
                FREEMEM(s1apOverloadResponse_p);
                s1apOverloadResponse_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GUMMEIList:
            {
                S1ap_GUMMEIList_t *s1apGUMMEIList_p = NULL;
                s1ap_OverloadStartIEs->presenceMask |= S1AP_OVERLOADSTARTIES_GUMMEILIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_GUMMEIList, (void**)&s1apGUMMEIList_p);
                if (tempDecoded < 0 || s1apGUMMEIList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gummeiList failed\n");
                    if (s1apGUMMEIList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_GUMMEIList, s1apGUMMEIList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_GUMMEIList, s1apGUMMEIList_p);
                memcpy(&s1ap_OverloadStartIEs->gummeiList, s1apGUMMEIList_p, sizeof(S1ap_GUMMEIList_t));
                FREEMEM(s1apGUMMEIList_p);
                s1apGUMMEIList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_TrafficLoadReductionIndication:
            {
                S1ap_TrafficLoadReductionIndication_t *s1apTrafficLoadReductionIndication_p = NULL;
                s1ap_OverloadStartIEs->presenceMask |= S1AP_OVERLOADSTARTIES_TRAFFICLOADREDUCTIONINDICATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TrafficLoadReductionIndication, (void**)&s1apTrafficLoadReductionIndication_p);
                if (tempDecoded < 0 || s1apTrafficLoadReductionIndication_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE trafficLoadReductionIndication failed\n");
                    if (s1apTrafficLoadReductionIndication_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TrafficLoadReductionIndication, s1apTrafficLoadReductionIndication_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TrafficLoadReductionIndication, s1apTrafficLoadReductionIndication_p);
                memcpy(&s1ap_OverloadStartIEs->trafficLoadReductionIndication, s1apTrafficLoadReductionIndication_p, sizeof(S1ap_TrafficLoadReductionIndication_t));
                FREEMEM(s1apTrafficLoadReductionIndication_p);
                s1apTrafficLoadReductionIndication_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_overloadstarties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_OverloadStart, s1ap_OverloadStart_p);
    return decoded;
}

int s1ap_decode_s1ap_mmedirectinformationtransferies(
    S1ap_MMEDirectInformationTransferIEs_t *s1ap_MMEDirectInformationTransferIEs,
    ANY_t *any_p) {

    S1ap_MMEDirectInformationTransfer_t  s1ap_MMEDirectInformationTransfer;
    S1ap_MMEDirectInformationTransfer_t *s1ap_MMEDirectInformationTransfer_p = &s1ap_MMEDirectInformationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_MMEDirectInformationTransferIEs != NULL);

    memset(s1ap_MMEDirectInformationTransferIEs, 0, sizeof(S1ap_MMEDirectInformationTransferIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_MMEDirectInformationTransferIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_MMEDirectInformationTransfer, (void**)&s1ap_MMEDirectInformationTransfer_p);

    for (i = 0; i < s1ap_MMEDirectInformationTransfer_p->s1ap_MMEDirectInformationTransfer_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_MMEDirectInformationTransfer_p->s1ap_MMEDirectInformationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Inter_SystemInformationTransferTypeMDT:
            {
                S1ap_Inter_SystemInformationTransferType_t *s1apInterSystemInformationTransferType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Inter_SystemInformationTransferType, (void**)&s1apInterSystemInformationTransferType_p);
                if (tempDecoded < 0 || s1apInterSystemInformationTransferType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE inter_SystemInformationTransferTypeMDT failed\n");
                    if (s1apInterSystemInformationTransferType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Inter_SystemInformationTransferType, s1apInterSystemInformationTransferType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Inter_SystemInformationTransferType, s1apInterSystemInformationTransferType_p);
                memcpy(&s1ap_MMEDirectInformationTransferIEs->inter_SystemInformationTransferTypeMDT, s1apInterSystemInformationTransferType_p, sizeof(S1ap_Inter_SystemInformationTransferType_t));
                FREEMEM(s1apInterSystemInformationTransferType_p);
                s1apInterSystemInformationTransferType_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_mmedirectinformationtransferies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_MMEDirectInformationTransfer, s1ap_MMEDirectInformationTransfer_p);
    return decoded;
}

int s1ap_decode_s1ap_handoverrequestacknowledgeies(
    S1ap_HandoverRequestAcknowledgeIEs_t *s1ap_HandoverRequestAcknowledgeIEs,
    ANY_t *any_p) {

    S1ap_HandoverRequestAcknowledge_t  s1ap_HandoverRequestAcknowledge;
    S1ap_HandoverRequestAcknowledge_t *s1ap_HandoverRequestAcknowledge_p = &s1ap_HandoverRequestAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverRequestAcknowledgeIEs != NULL);

    memset(s1ap_HandoverRequestAcknowledgeIEs, 0, sizeof(S1ap_HandoverRequestAcknowledgeIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverRequestAcknowledgeIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverRequestAcknowledge, (void**)&s1ap_HandoverRequestAcknowledge_p);

    for (i = 0; i < s1ap_HandoverRequestAcknowledge_p->s1ap_HandoverRequestAcknowledge_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverRequestAcknowledge_p->s1ap_HandoverRequestAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverRequestAcknowledgeIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_HandoverRequestAcknowledgeIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABAdmittedList:
            {
                S1ap_E_RABAdmittedList_t *s1apERABAdmittedList_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABAdmittedList, (void**)&s1apERABAdmittedList_p);
                if (tempDecoded < 0 || s1apERABAdmittedList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABAdmittedList failed\n");
                    if (s1apERABAdmittedList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABAdmittedList, s1apERABAdmittedList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABAdmittedList, s1apERABAdmittedList_p);
                if (s1ap_decode_s1ap_e_rabadmittedlist(&s1ap_HandoverRequestAcknowledgeIEs->e_RABAdmittedList, s1apERABAdmittedList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABAdmittedList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABAdmittedList, s1apERABAdmittedList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABFailedToSetupListHOReqAck:
            {
                S1ap_E_RABFailedToSetupListHOReqAck_t *s1apERABFailedToSetupListHOReqAck_p = NULL;
                s1ap_HandoverRequestAcknowledgeIEs->presenceMask |= S1AP_HANDOVERREQUESTACKNOWLEDGEIES_E_RABFAILEDTOSETUPLISTHOREQACK_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABFailedToSetupListHOReqAck, (void**)&s1apERABFailedToSetupListHOReqAck_p);
                if (tempDecoded < 0 || s1apERABFailedToSetupListHOReqAck_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABFailedToSetupListHOReqAck failed\n");
                    if (s1apERABFailedToSetupListHOReqAck_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABFailedToSetupListHOReqAck, s1apERABFailedToSetupListHOReqAck_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABFailedToSetupListHOReqAck, s1apERABFailedToSetupListHOReqAck_p);
                if (s1ap_decode_s1ap_e_rabfailedtosetuplisthoreqack(&s1ap_HandoverRequestAcknowledgeIEs->e_RABFailedToSetupListHOReqAck, s1apERABFailedToSetupListHOReqAck_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABFailedToSetupListHOReqAck failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABFailedToSetupListHOReqAck, s1apERABFailedToSetupListHOReqAck_p);
                }
            } break;
            case S1ap_ProtocolIE_ID_id_Target_ToSource_TransparentContainer:
            {
                S1ap_Target_ToSource_TransparentContainer_t *s1apTargetToSourceTransparentContainer_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Target_ToSource_TransparentContainer, (void**)&s1apTargetToSourceTransparentContainer_p);
                if (tempDecoded < 0 || s1apTargetToSourceTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE target_ToSource_TransparentContainer failed\n");
                    if (s1apTargetToSourceTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p);
                memcpy(&s1ap_HandoverRequestAcknowledgeIEs->target_ToSource_TransparentContainer, s1apTargetToSourceTransparentContainer_p, sizeof(S1ap_Target_ToSource_TransparentContainer_t));
                FREEMEM(s1apTargetToSourceTransparentContainer_p);
                s1apTargetToSourceTransparentContainer_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_Id:
            {
                S1ap_CSG_Id_t *s1apCSGId_p = NULL;
                s1ap_HandoverRequestAcknowledgeIEs->presenceMask |= S1AP_HANDOVERREQUESTACKNOWLEDGEIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_Id, (void**)&s1apCSGId_p);
                if (tempDecoded < 0 || s1apCSGId_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_Id failed\n");
                    if (s1apCSGId_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                memcpy(&s1ap_HandoverRequestAcknowledgeIEs->csG_Id, s1apCSGId_p, sizeof(S1ap_CSG_Id_t));
                FREEMEM(s1apCSGId_p);
                s1apCSGId_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_HandoverRequestAcknowledgeIEs->presenceMask |= S1AP_HANDOVERREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_HandoverRequestAcknowledgeIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handoverrequestacknowledgeies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverRequestAcknowledge, s1ap_HandoverRequestAcknowledge_p);
    return decoded;
}

int s1ap_decode_s1ap_downlinkueassociatedlppatransport_ies(
    S1ap_DownlinkUEAssociatedLPPaTransport_IEs_t *s1ap_DownlinkUEAssociatedLPPaTransport_IEs,
    ANY_t *any_p) {

    S1ap_DownlinkUEAssociatedLPPaTransport_t  s1ap_DownlinkUEAssociatedLPPaTransport;
    S1ap_DownlinkUEAssociatedLPPaTransport_t *s1ap_DownlinkUEAssociatedLPPaTransport_p = &s1ap_DownlinkUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_DownlinkUEAssociatedLPPaTransport_IEs != NULL);

    memset(s1ap_DownlinkUEAssociatedLPPaTransport_IEs, 0, sizeof(S1ap_DownlinkUEAssociatedLPPaTransport_IEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_DownlinkUEAssociatedLPPaTransport_IEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_DownlinkUEAssociatedLPPaTransport, (void**)&s1ap_DownlinkUEAssociatedLPPaTransport_p);

    for (i = 0; i < s1ap_DownlinkUEAssociatedLPPaTransport_p->s1ap_DownlinkUEAssociatedLPPaTransport_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_DownlinkUEAssociatedLPPaTransport_p->s1ap_DownlinkUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_DownlinkUEAssociatedLPPaTransport_IEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_DownlinkUEAssociatedLPPaTransport_IEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Routing_ID:
            {
                S1ap_Routing_ID_t *s1apRoutingID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Routing_ID, (void**)&s1apRoutingID_p);
                if (tempDecoded < 0 || s1apRoutingID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE routing_ID failed\n");
                    if (s1apRoutingID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                memcpy(&s1ap_DownlinkUEAssociatedLPPaTransport_IEs->routing_ID, s1apRoutingID_p, sizeof(S1ap_Routing_ID_t));
                FREEMEM(s1apRoutingID_p);
                s1apRoutingID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_LPPa_PDU:
            {
                S1ap_LPPa_PDU_t *s1apLPPaPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_LPPa_PDU, (void**)&s1apLPPaPDU_p);
                if (tempDecoded < 0 || s1apLPPaPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE lpPa_PDU failed\n");
                    if (s1apLPPaPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                memcpy(&s1ap_DownlinkUEAssociatedLPPaTransport_IEs->lpPa_PDU, s1apLPPaPDU_p, sizeof(S1ap_LPPa_PDU_t));
                FREEMEM(s1apLPPaPDU_p);
                s1apLPPaPDU_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_downlinkueassociatedlppatransport_ies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_DownlinkUEAssociatedLPPaTransport, s1ap_DownlinkUEAssociatedLPPaTransport_p);
    return decoded;
}

int s1ap_decode_s1ap_uecontextmodificationfailureies(
    S1ap_UEContextModificationFailureIEs_t *s1ap_UEContextModificationFailureIEs,
    ANY_t *any_p) {

    S1ap_UEContextModificationFailure_t  s1ap_UEContextModificationFailure;
    S1ap_UEContextModificationFailure_t *s1ap_UEContextModificationFailure_p = &s1ap_UEContextModificationFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UEContextModificationFailureIEs != NULL);

    memset(s1ap_UEContextModificationFailureIEs, 0, sizeof(S1ap_UEContextModificationFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UEContextModificationFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UEContextModificationFailure, (void**)&s1ap_UEContextModificationFailure_p);

    for (i = 0; i < s1ap_UEContextModificationFailure_p->s1ap_UEContextModificationFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UEContextModificationFailure_p->s1ap_UEContextModificationFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UEContextModificationFailureIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UEContextModificationFailureIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_UEContextModificationFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_UEContextModificationFailureIEs->presenceMask |= S1AP_UECONTEXTMODIFICATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_UEContextModificationFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uecontextmodificationfailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UEContextModificationFailure, s1ap_UEContextModificationFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_uecontextreleaserequesties(
    S1ap_UEContextReleaseRequestIEs_t *s1ap_UEContextReleaseRequestIEs,
    ANY_t *any_p) {

    S1ap_UEContextReleaseRequest_t  s1ap_UEContextReleaseRequest;
    S1ap_UEContextReleaseRequest_t *s1ap_UEContextReleaseRequest_p = &s1ap_UEContextReleaseRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UEContextReleaseRequestIEs != NULL);

    memset(s1ap_UEContextReleaseRequestIEs, 0, sizeof(S1ap_UEContextReleaseRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UEContextReleaseRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UEContextReleaseRequest, (void**)&s1ap_UEContextReleaseRequest_p);

    for (i = 0; i < s1ap_UEContextReleaseRequest_p->s1ap_UEContextReleaseRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UEContextReleaseRequest_p->s1ap_UEContextReleaseRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UEContextReleaseRequestIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UEContextReleaseRequestIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_UEContextReleaseRequestIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GWContextReleaseIndication:
            {
                S1ap_GWContextReleaseIndication_t *s1apGWContextReleaseIndication_p = NULL;
                s1ap_UEContextReleaseRequestIEs->presenceMask |= S1AP_UECONTEXTRELEASEREQUESTIES_GWCONTEXTRELEASEINDICATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_GWContextReleaseIndication, (void**)&s1apGWContextReleaseIndication_p);
                if (tempDecoded < 0 || s1apGWContextReleaseIndication_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gwContextReleaseIndication failed\n");
                    if (s1apGWContextReleaseIndication_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_GWContextReleaseIndication, s1apGWContextReleaseIndication_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_GWContextReleaseIndication, s1apGWContextReleaseIndication_p);
                memcpy(&s1ap_UEContextReleaseRequestIEs->gwContextReleaseIndication, s1apGWContextReleaseIndication_p, sizeof(S1ap_GWContextReleaseIndication_t));
                FREEMEM(s1apGWContextReleaseIndication_p);
                s1apGWContextReleaseIndication_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uecontextreleaserequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UEContextReleaseRequest, s1ap_UEContextReleaseRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_mmeconfigurationtransferies(
    S1ap_MMEConfigurationTransferIEs_t *s1ap_MMEConfigurationTransferIEs,
    ANY_t *any_p) {

    S1ap_MMEConfigurationTransfer_t  s1ap_MMEConfigurationTransfer;
    S1ap_MMEConfigurationTransfer_t *s1ap_MMEConfigurationTransfer_p = &s1ap_MMEConfigurationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_MMEConfigurationTransferIEs != NULL);

    memset(s1ap_MMEConfigurationTransferIEs, 0, sizeof(S1ap_MMEConfigurationTransferIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_MMEConfigurationTransferIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_MMEConfigurationTransfer, (void**)&s1ap_MMEConfigurationTransfer_p);

    for (i = 0; i < s1ap_MMEConfigurationTransfer_p->s1ap_MMEConfigurationTransfer_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_MMEConfigurationTransfer_p->s1ap_MMEConfigurationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SONConfigurationTransferMCT:
            {
                S1ap_SONConfigurationTransfer_t *s1apSONConfigurationTransfer_p = NULL;
                s1ap_MMEConfigurationTransferIEs->presenceMask |= S1AP_MMECONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFERMCT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SONConfigurationTransfer, (void**)&s1apSONConfigurationTransfer_p);
                if (tempDecoded < 0 || s1apSONConfigurationTransfer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE sonConfigurationTransferMCT failed\n");
                    if (s1apSONConfigurationTransfer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SONConfigurationTransfer, s1apSONConfigurationTransfer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SONConfigurationTransfer, s1apSONConfigurationTransfer_p);
                memcpy(&s1ap_MMEConfigurationTransferIEs->sonConfigurationTransferMCT, s1apSONConfigurationTransfer_p, sizeof(S1ap_SONConfigurationTransfer_t));
                FREEMEM(s1apSONConfigurationTransfer_p);
                s1apSONConfigurationTransfer_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_mmeconfigurationtransferies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_MMEConfigurationTransfer, s1ap_MMEConfigurationTransfer_p);
    return decoded;
}

int s1ap_decode_s1ap_pathswitchrequestacknowledgeies(
    S1ap_PathSwitchRequestAcknowledgeIEs_t *s1ap_PathSwitchRequestAcknowledgeIEs,
    ANY_t *any_p) {

    S1ap_PathSwitchRequestAcknowledge_t  s1ap_PathSwitchRequestAcknowledge;
    S1ap_PathSwitchRequestAcknowledge_t *s1ap_PathSwitchRequestAcknowledge_p = &s1ap_PathSwitchRequestAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_PathSwitchRequestAcknowledgeIEs != NULL);

    memset(s1ap_PathSwitchRequestAcknowledgeIEs, 0, sizeof(S1ap_PathSwitchRequestAcknowledgeIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_PathSwitchRequestAcknowledgeIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_PathSwitchRequestAcknowledge, (void**)&s1ap_PathSwitchRequestAcknowledge_p);

    for (i = 0; i < s1ap_PathSwitchRequestAcknowledge_p->s1ap_PathSwitchRequestAcknowledge_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_PathSwitchRequestAcknowledge_p->s1ap_PathSwitchRequestAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_PathSwitchRequestAcknowledgeIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_PathSwitchRequestAcknowledgeIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                S1ap_UEAggregateMaximumBitrate_t *s1apUEAggregateMaximumBitrate_p = NULL;
                s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask |= S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEAggregateMaximumBitrate, (void**)&s1apUEAggregateMaximumBitrate_p);
                if (tempDecoded < 0 || s1apUEAggregateMaximumBitrate_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uEaggregateMaximumBitrate failed\n");
                    if (s1apUEAggregateMaximumBitrate_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                memcpy(&s1ap_PathSwitchRequestAcknowledgeIEs->uEaggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p, sizeof(S1ap_UEAggregateMaximumBitrate_t));
                FREEMEM(s1apUEAggregateMaximumBitrate_p);
                s1apUEAggregateMaximumBitrate_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABToBeSwitchedULList:
            {
                S1ap_E_RABToBeSwitchedULList_t *s1apERABToBeSwitchedULList_p = NULL;
                s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask |= S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBESWITCHEDULLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSwitchedULList, (void**)&s1apERABToBeSwitchedULList_p);
                if (tempDecoded < 0 || s1apERABToBeSwitchedULList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSwitchedULList failed\n");
                    if (s1apERABToBeSwitchedULList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSwitchedULList, s1apERABToBeSwitchedULList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSwitchedULList, s1apERABToBeSwitchedULList_p);
                if (s1ap_decode_s1ap_e_rabtobeswitchedullist(&s1ap_PathSwitchRequestAcknowledgeIEs->e_RABToBeSwitchedULList, s1apERABToBeSwitchedULList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABToBeSwitchedULList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSwitchedULList, s1apERABToBeSwitchedULList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABToBeReleasedList:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask |= S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBERELEASEDLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeReleasedList failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_PathSwitchRequestAcknowledgeIEs->e_RABToBeReleasedList, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            case S1ap_ProtocolIE_ID_id_SecurityContext:
            {
                S1ap_SecurityContext_t *s1apSecurityContext_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SecurityContext, (void**)&s1apSecurityContext_p);
                if (tempDecoded < 0 || s1apSecurityContext_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE securityContext failed\n");
                    if (s1apSecurityContext_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SecurityContext, s1apSecurityContext_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SecurityContext, s1apSecurityContext_p);
                memcpy(&s1ap_PathSwitchRequestAcknowledgeIEs->securityContext, s1apSecurityContext_p, sizeof(S1ap_SecurityContext_t));
                FREEMEM(s1apSecurityContext_p);
                s1apSecurityContext_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask |= S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_PathSwitchRequestAcknowledgeIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID_2:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask |= S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_MME_UE_S1AP_ID_2_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id_2 failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_PathSwitchRequestAcknowledgeIEs->mme_ue_s1ap_id_2, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_pathswitchrequestacknowledgeies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_PathSwitchRequestAcknowledge, s1ap_PathSwitchRequestAcknowledge_p);
    return decoded;
}

int s1ap_decode_s1ap_s1setupfailureies(
    S1ap_S1SetupFailureIEs_t *s1ap_S1SetupFailureIEs,
    ANY_t *any_p) {

    S1ap_S1SetupFailure_t  s1ap_S1SetupFailure;
    S1ap_S1SetupFailure_t *s1ap_S1SetupFailure_p = &s1ap_S1SetupFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_S1SetupFailureIEs != NULL);

    memset(s1ap_S1SetupFailureIEs, 0, sizeof(S1ap_S1SetupFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_S1SetupFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_S1SetupFailure, (void**)&s1ap_S1SetupFailure_p);

    for (i = 0; i < s1ap_S1SetupFailure_p->s1ap_S1SetupFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_S1SetupFailure_p->s1ap_S1SetupFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_S1SetupFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_TimeToWait:
            {
                S1ap_TimeToWait_t *s1apTimeToWait_p = NULL;
                s1ap_S1SetupFailureIEs->presenceMask |= S1AP_S1SETUPFAILUREIES_TIMETOWAIT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TimeToWait, (void**)&s1apTimeToWait_p);
                if (tempDecoded < 0 || s1apTimeToWait_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE timeToWait failed\n");
                    if (s1apTimeToWait_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TimeToWait, s1apTimeToWait_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TimeToWait, s1apTimeToWait_p);
                memcpy(&s1ap_S1SetupFailureIEs->timeToWait, s1apTimeToWait_p, sizeof(S1ap_TimeToWait_t));
                FREEMEM(s1apTimeToWait_p);
                s1apTimeToWait_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_S1SetupFailureIEs->presenceMask |= S1AP_S1SETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_S1SetupFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_s1setupfailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_S1SetupFailure, s1ap_S1SetupFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_mmeconfigurationupdatefailureies(
    S1ap_MMEConfigurationUpdateFailureIEs_t *s1ap_MMEConfigurationUpdateFailureIEs,
    ANY_t *any_p) {

    S1ap_MMEConfigurationUpdateFailure_t  s1ap_MMEConfigurationUpdateFailure;
    S1ap_MMEConfigurationUpdateFailure_t *s1ap_MMEConfigurationUpdateFailure_p = &s1ap_MMEConfigurationUpdateFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_MMEConfigurationUpdateFailureIEs != NULL);

    memset(s1ap_MMEConfigurationUpdateFailureIEs, 0, sizeof(S1ap_MMEConfigurationUpdateFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_MMEConfigurationUpdateFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_MMEConfigurationUpdateFailure, (void**)&s1ap_MMEConfigurationUpdateFailure_p);

    for (i = 0; i < s1ap_MMEConfigurationUpdateFailure_p->s1ap_MMEConfigurationUpdateFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_MMEConfigurationUpdateFailure_p->s1ap_MMEConfigurationUpdateFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_MMEConfigurationUpdateFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_TimeToWait:
            {
                S1ap_TimeToWait_t *s1apTimeToWait_p = NULL;
                s1ap_MMEConfigurationUpdateFailureIEs->presenceMask |= S1AP_MMECONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TimeToWait, (void**)&s1apTimeToWait_p);
                if (tempDecoded < 0 || s1apTimeToWait_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE timeToWait failed\n");
                    if (s1apTimeToWait_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TimeToWait, s1apTimeToWait_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TimeToWait, s1apTimeToWait_p);
                memcpy(&s1ap_MMEConfigurationUpdateFailureIEs->timeToWait, s1apTimeToWait_p, sizeof(S1ap_TimeToWait_t));
                FREEMEM(s1apTimeToWait_p);
                s1apTimeToWait_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_MMEConfigurationUpdateFailureIEs->presenceMask |= S1AP_MMECONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_MMEConfigurationUpdateFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_mmeconfigurationupdatefailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_MMEConfigurationUpdateFailure, s1ap_MMEConfigurationUpdateFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_initialcontextsetupresponseies(
    S1ap_InitialContextSetupResponseIEs_t *s1ap_InitialContextSetupResponseIEs,
    ANY_t *any_p) {

    S1ap_InitialContextSetupResponse_t  s1ap_InitialContextSetupResponse;
    S1ap_InitialContextSetupResponse_t *s1ap_InitialContextSetupResponse_p = &s1ap_InitialContextSetupResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_InitialContextSetupResponseIEs != NULL);

    memset(s1ap_InitialContextSetupResponseIEs, 0, sizeof(S1ap_InitialContextSetupResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_InitialContextSetupResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_InitialContextSetupResponse, (void**)&s1ap_InitialContextSetupResponse_p);

    for (i = 0; i < s1ap_InitialContextSetupResponse_p->s1ap_InitialContextSetupResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_InitialContextSetupResponse_p->s1ap_InitialContextSetupResponse_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_InitialContextSetupResponseIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_InitialContextSetupResponseIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABSetupListCtxtSURes:
            {
                S1ap_E_RABSetupListCtxtSURes_t *s1apERABSetupListCtxtSURes_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABSetupListCtxtSURes, (void**)&s1apERABSetupListCtxtSURes_p);
                if (tempDecoded < 0 || s1apERABSetupListCtxtSURes_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABSetupListCtxtSURes failed\n");
                    if (s1apERABSetupListCtxtSURes_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupListCtxtSURes, s1apERABSetupListCtxtSURes_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABSetupListCtxtSURes, s1apERABSetupListCtxtSURes_p);
                if (s1ap_decode_s1ap_e_rabsetuplistctxtsures(&s1ap_InitialContextSetupResponseIEs->e_RABSetupListCtxtSURes, s1apERABSetupListCtxtSURes_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABSetupListCtxtSURes failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupListCtxtSURes, s1apERABSetupListCtxtSURes_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABFailedToSetupListCtxtSURes:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                s1ap_InitialContextSetupResponseIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPRESPONSEIES_E_RABFAILEDTOSETUPLISTCTXTSURES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABFailedToSetupListCtxtSURes failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_InitialContextSetupResponseIEs->e_RABFailedToSetupListCtxtSURes, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_InitialContextSetupResponseIEs->presenceMask |= S1AP_INITIALCONTEXTSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_InitialContextSetupResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_initialcontextsetupresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_InitialContextSetupResponse, s1ap_InitialContextSetupResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_enbconfigurationupdateies(
    S1ap_ENBConfigurationUpdateIEs_t *s1ap_ENBConfigurationUpdateIEs,
    ANY_t *any_p) {

    S1ap_ENBConfigurationUpdate_t  s1ap_ENBConfigurationUpdate;
    S1ap_ENBConfigurationUpdate_t *s1ap_ENBConfigurationUpdate_p = &s1ap_ENBConfigurationUpdate;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ENBConfigurationUpdateIEs != NULL);

    memset(s1ap_ENBConfigurationUpdateIEs, 0, sizeof(S1ap_ENBConfigurationUpdateIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ENBConfigurationUpdateIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ENBConfigurationUpdate, (void**)&s1ap_ENBConfigurationUpdate_p);

    for (i = 0; i < s1ap_ENBConfigurationUpdate_p->s1ap_ENBConfigurationUpdate_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ENBConfigurationUpdate_p->s1ap_ENBConfigurationUpdate_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_eNBname:
            {
                S1ap_ENBname_t *s1apENBname_p = NULL;
                s1ap_ENBConfigurationUpdateIEs->presenceMask |= S1AP_ENBCONFIGURATIONUPDATEIES_ENBNAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENBname, (void**)&s1apENBname_p);
                if (tempDecoded < 0 || s1apENBname_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNBname failed\n");
                    if (s1apENBname_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENBname, s1apENBname_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENBname, s1apENBname_p);
                memcpy(&s1ap_ENBConfigurationUpdateIEs->eNBname, s1apENBname_p, sizeof(S1ap_ENBname_t));
                FREEMEM(s1apENBname_p);
                s1apENBname_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SupportedTAs:
            {
                S1ap_SupportedTAs_t *s1apSupportedTAs_p = NULL;
                s1ap_ENBConfigurationUpdateIEs->presenceMask |= S1AP_ENBCONFIGURATIONUPDATEIES_SUPPORTEDTAS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SupportedTAs, (void**)&s1apSupportedTAs_p);
                if (tempDecoded < 0 || s1apSupportedTAs_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE supportedTAs failed\n");
                    if (s1apSupportedTAs_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SupportedTAs, s1apSupportedTAs_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SupportedTAs, s1apSupportedTAs_p);
                memcpy(&s1ap_ENBConfigurationUpdateIEs->supportedTAs, s1apSupportedTAs_p, sizeof(S1ap_SupportedTAs_t));
                FREEMEM(s1apSupportedTAs_p);
                s1apSupportedTAs_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_IdList:
            {
                S1ap_CSG_IdList_t *s1apCSGIdList_p = NULL;
                s1ap_ENBConfigurationUpdateIEs->presenceMask |= S1AP_ENBCONFIGURATIONUPDATEIES_CSG_IDLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_IdList, (void**)&s1apCSGIdList_p);
                if (tempDecoded < 0 || s1apCSGIdList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_IdList failed\n");
                    if (s1apCSGIdList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_IdList, s1apCSGIdList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_IdList, s1apCSGIdList_p);
                memcpy(&s1ap_ENBConfigurationUpdateIEs->csG_IdList, s1apCSGIdList_p, sizeof(S1ap_CSG_IdList_t));
                FREEMEM(s1apCSGIdList_p);
                s1apCSGIdList_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_DefaultPagingDRX:
            {
                S1ap_PagingDRX_t *s1apPagingDRX_p = NULL;
                s1ap_ENBConfigurationUpdateIEs->presenceMask |= S1AP_ENBCONFIGURATIONUPDATEIES_DEFAULTPAGINGDRX_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_PagingDRX, (void**)&s1apPagingDRX_p);
                if (tempDecoded < 0 || s1apPagingDRX_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE defaultPagingDRX failed\n");
                    if (s1apPagingDRX_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_PagingDRX, s1apPagingDRX_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_PagingDRX, s1apPagingDRX_p);
                memcpy(&s1ap_ENBConfigurationUpdateIEs->defaultPagingDRX, s1apPagingDRX_p, sizeof(S1ap_PagingDRX_t));
                FREEMEM(s1apPagingDRX_p);
                s1apPagingDRX_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_enbconfigurationupdateies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ENBConfigurationUpdate, s1ap_ENBConfigurationUpdate_p);
    return decoded;
}

int s1ap_decode_s1ap_overloadstopies(
    S1ap_OverloadStopIEs_t *s1ap_OverloadStopIEs,
    ANY_t *any_p) {

    S1ap_OverloadStop_t  s1ap_OverloadStop;
    S1ap_OverloadStop_t *s1ap_OverloadStop_p = &s1ap_OverloadStop;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_OverloadStopIEs != NULL);

    memset(s1ap_OverloadStopIEs, 0, sizeof(S1ap_OverloadStopIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_OverloadStopIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_OverloadStop, (void**)&s1ap_OverloadStop_p);

    for (i = 0; i < s1ap_OverloadStop_p->s1ap_OverloadStop_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_OverloadStop_p->s1ap_OverloadStop_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GUMMEIList:
            {
                S1ap_GUMMEIList_t *s1apGUMMEIList_p = NULL;
                s1ap_OverloadStopIEs->presenceMask |= S1AP_OVERLOADSTOPIES_GUMMEILIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_GUMMEIList, (void**)&s1apGUMMEIList_p);
                if (tempDecoded < 0 || s1apGUMMEIList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gummeiList failed\n");
                    if (s1apGUMMEIList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_GUMMEIList, s1apGUMMEIList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_GUMMEIList, s1apGUMMEIList_p);
                memcpy(&s1ap_OverloadStopIEs->gummeiList, s1apGUMMEIList_p, sizeof(S1ap_GUMMEIList_t));
                FREEMEM(s1apGUMMEIList_p);
                s1apGUMMEIList_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_overloadstopies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_OverloadStop, s1ap_OverloadStop_p);
    return decoded;
}

int s1ap_decode_s1ap_uplinknastransporties(
    S1ap_UplinkNASTransportIEs_t *s1ap_UplinkNASTransportIEs,
    ANY_t *any_p) {

    S1ap_UplinkNASTransport_t  s1ap_UplinkNASTransport;
    S1ap_UplinkNASTransport_t *s1ap_UplinkNASTransport_p = &s1ap_UplinkNASTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UplinkNASTransportIEs != NULL);

    memset(s1ap_UplinkNASTransportIEs, 0, sizeof(S1ap_UplinkNASTransportIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UplinkNASTransportIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UplinkNASTransport, (void**)&s1ap_UplinkNASTransport_p);

    for (i = 0; i < s1ap_UplinkNASTransport_p->s1ap_UplinkNASTransport_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UplinkNASTransport_p->s1ap_UplinkNASTransport_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UplinkNASTransportIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UplinkNASTransportIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_NAS_PDU:
            {
                S1ap_NAS_PDU_t *s1apNASPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_NAS_PDU, (void**)&s1apNASPDU_p);
                if (tempDecoded < 0 || s1apNASPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE nas_pdu failed\n");
                    if (s1apNASPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                memcpy(&s1ap_UplinkNASTransportIEs->nas_pdu, s1apNASPDU_p, sizeof(S1ap_NAS_PDU_t));
                FREEMEM(s1apNASPDU_p);
                s1apNASPDU_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_EUTRAN_CGI:
            {
                S1ap_EUTRAN_CGI_t *s1apEUTRANCGI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_EUTRAN_CGI, (void**)&s1apEUTRANCGI_p);
                if (tempDecoded < 0 || s1apEUTRANCGI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eutran_cgi failed\n");
                    if (s1apEUTRANCGI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                memcpy(&s1ap_UplinkNASTransportIEs->eutran_cgi, s1apEUTRANCGI_p, sizeof(S1ap_EUTRAN_CGI_t));
                FREEMEM(s1apEUTRANCGI_p);
                s1apEUTRANCGI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TAI:
            {
                S1ap_TAI_t *s1apTAI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TAI, (void**)&s1apTAI_p);
                if (tempDecoded < 0 || s1apTAI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE tai failed\n");
                    if (s1apTAI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TAI, s1apTAI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TAI, s1apTAI_p);
                memcpy(&s1ap_UplinkNASTransportIEs->tai, s1apTAI_p, sizeof(S1ap_TAI_t));
                FREEMEM(s1apTAI_p);
                s1apTAI_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_GW_TransportLayerAddress:
            {
                S1ap_TransportLayerAddress_t *s1apTransportLayerAddress_p = NULL;
                s1ap_UplinkNASTransportIEs->presenceMask |= S1AP_UPLINKNASTRANSPORTIES_GW_TRANSPORTLAYERADDRESS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TransportLayerAddress, (void**)&s1apTransportLayerAddress_p);
                if (tempDecoded < 0 || s1apTransportLayerAddress_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE gW_TransportLayerAddress failed\n");
                    if (s1apTransportLayerAddress_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TransportLayerAddress, s1apTransportLayerAddress_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TransportLayerAddress, s1apTransportLayerAddress_p);
                memcpy(&s1ap_UplinkNASTransportIEs->gW_TransportLayerAddress, s1apTransportLayerAddress_p, sizeof(S1ap_TransportLayerAddress_t));
                FREEMEM(s1apTransportLayerAddress_p);
                s1apTransportLayerAddress_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinknastransporties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UplinkNASTransport, s1ap_UplinkNASTransport_p);
    return decoded;
}

int s1ap_decode_s1ap_tracefailureindicationies(
    S1ap_TraceFailureIndicationIEs_t *s1ap_TraceFailureIndicationIEs,
    ANY_t *any_p) {

    S1ap_TraceFailureIndication_t  s1ap_TraceFailureIndication;
    S1ap_TraceFailureIndication_t *s1ap_TraceFailureIndication_p = &s1ap_TraceFailureIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_TraceFailureIndicationIEs != NULL);

    memset(s1ap_TraceFailureIndicationIEs, 0, sizeof(S1ap_TraceFailureIndicationIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_TraceFailureIndicationIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_TraceFailureIndication, (void**)&s1ap_TraceFailureIndication_p);

    for (i = 0; i < s1ap_TraceFailureIndication_p->s1ap_TraceFailureIndication_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_TraceFailureIndication_p->s1ap_TraceFailureIndication_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_TraceFailureIndicationIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_TraceFailureIndicationIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_UTRAN_Trace_ID:
            {
                S1ap_E_UTRAN_Trace_ID_t *s1apEUTRANTraceID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_UTRAN_Trace_ID, (void**)&s1apEUTRANTraceID_p);
                if (tempDecoded < 0 || s1apEUTRANTraceID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_UTRAN_Trace_ID failed\n");
                    if (s1apEUTRANTraceID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_UTRAN_Trace_ID, s1apEUTRANTraceID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_UTRAN_Trace_ID, s1apEUTRANTraceID_p);
                memcpy(&s1ap_TraceFailureIndicationIEs->e_UTRAN_Trace_ID, s1apEUTRANTraceID_p, sizeof(S1ap_E_UTRAN_Trace_ID_t));
                FREEMEM(s1apEUTRANTraceID_p);
                s1apEUTRANTraceID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_TraceFailureIndicationIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_tracefailureindicationies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_TraceFailureIndication, s1ap_TraceFailureIndication_p);
    return decoded;
}

int s1ap_decode_s1ap_uecontextreleasecommandies(
    S1ap_UEContextReleaseCommandIEs_t *s1ap_UEContextReleaseCommandIEs,
    ANY_t *any_p) {

    S1ap_UEContextReleaseCommand_t  s1ap_UEContextReleaseCommand;
    S1ap_UEContextReleaseCommand_t *s1ap_UEContextReleaseCommand_p = &s1ap_UEContextReleaseCommand;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UEContextReleaseCommandIEs != NULL);

    memset(s1ap_UEContextReleaseCommandIEs, 0, sizeof(S1ap_UEContextReleaseCommandIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UEContextReleaseCommandIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UEContextReleaseCommand, (void**)&s1ap_UEContextReleaseCommand_p);

    for (i = 0; i < s1ap_UEContextReleaseCommand_p->s1ap_UEContextReleaseCommand_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UEContextReleaseCommand_p->s1ap_UEContextReleaseCommand_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_UE_S1AP_IDs:
            {
                S1ap_UE_S1AP_IDs_t *s1apUES1APIDs_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UE_S1AP_IDs, (void**)&s1apUES1APIDs_p);
                if (tempDecoded < 0 || s1apUES1APIDs_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uE_S1AP_IDs failed\n");
                    if (s1apUES1APIDs_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UE_S1AP_IDs, s1apUES1APIDs_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UE_S1AP_IDs, s1apUES1APIDs_p);
                memcpy(&s1ap_UEContextReleaseCommandIEs->uE_S1AP_IDs, s1apUES1APIDs_p, sizeof(S1ap_UE_S1AP_IDs_t));
                FREEMEM(s1apUES1APIDs_p);
                s1apUES1APIDs_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_UEContextReleaseCommandIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uecontextreleasecommandies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UEContextReleaseCommand, s1ap_UEContextReleaseCommand_p);
    return decoded;
}

int s1ap_decode_s1ap_enbconfigurationtransferies(
    S1ap_ENBConfigurationTransferIEs_t *s1ap_ENBConfigurationTransferIEs,
    ANY_t *any_p) {

    S1ap_ENBConfigurationTransfer_t  s1ap_ENBConfigurationTransfer;
    S1ap_ENBConfigurationTransfer_t *s1ap_ENBConfigurationTransfer_p = &s1ap_ENBConfigurationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ENBConfigurationTransferIEs != NULL);

    memset(s1ap_ENBConfigurationTransferIEs, 0, sizeof(S1ap_ENBConfigurationTransferIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ENBConfigurationTransferIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ENBConfigurationTransfer, (void**)&s1ap_ENBConfigurationTransfer_p);

    for (i = 0; i < s1ap_ENBConfigurationTransfer_p->s1ap_ENBConfigurationTransfer_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ENBConfigurationTransfer_p->s1ap_ENBConfigurationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SONConfigurationTransferECT:
            {
                S1ap_SONConfigurationTransfer_t *s1apSONConfigurationTransfer_p = NULL;
                s1ap_ENBConfigurationTransferIEs->presenceMask |= S1AP_ENBCONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFERECT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SONConfigurationTransfer, (void**)&s1apSONConfigurationTransfer_p);
                if (tempDecoded < 0 || s1apSONConfigurationTransfer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE sonConfigurationTransferECT failed\n");
                    if (s1apSONConfigurationTransfer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SONConfigurationTransfer, s1apSONConfigurationTransfer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SONConfigurationTransfer, s1apSONConfigurationTransfer_p);
                memcpy(&s1ap_ENBConfigurationTransferIEs->sonConfigurationTransferECT, s1apSONConfigurationTransfer_p, sizeof(S1ap_SONConfigurationTransfer_t));
                FREEMEM(s1apSONConfigurationTransfer_p);
                s1apSONConfigurationTransfer_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_enbconfigurationtransferies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ENBConfigurationTransfer, s1ap_ENBConfigurationTransfer_p);
    return decoded;
}

int s1ap_decode_s1ap_handoverfailureies(
    S1ap_HandoverFailureIEs_t *s1ap_HandoverFailureIEs,
    ANY_t *any_p) {

    S1ap_HandoverFailure_t  s1ap_HandoverFailure;
    S1ap_HandoverFailure_t *s1ap_HandoverFailure_p = &s1ap_HandoverFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_HandoverFailureIEs != NULL);

    memset(s1ap_HandoverFailureIEs, 0, sizeof(S1ap_HandoverFailureIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_HandoverFailureIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_HandoverFailure, (void**)&s1ap_HandoverFailure_p);

    for (i = 0; i < s1ap_HandoverFailure_p->s1ap_HandoverFailure_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_HandoverFailure_p->s1ap_HandoverFailure_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_HandoverFailureIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_HandoverFailureIEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_HandoverFailureIEs->presenceMask |= S1AP_HANDOVERFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_HandoverFailureIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_handoverfailureies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_HandoverFailure, s1ap_HandoverFailure_p);
    return decoded;
}

int s1ap_decode_s1ap_e_rabsetupresponseies(
    S1ap_E_RABSetupResponseIEs_t *s1ap_E_RABSetupResponseIEs,
    ANY_t *any_p) {

    S1ap_E_RABSetupResponse_t  s1ap_E_RABSetupResponse;
    S1ap_E_RABSetupResponse_t *s1ap_E_RABSetupResponse_p = &s1ap_E_RABSetupResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_E_RABSetupResponseIEs != NULL);

    memset(s1ap_E_RABSetupResponseIEs, 0, sizeof(S1ap_E_RABSetupResponseIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_E_RABSetupResponseIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_E_RABSetupResponse, (void**)&s1ap_E_RABSetupResponse_p);

    for (i = 0; i < s1ap_E_RABSetupResponse_p->s1ap_E_RABSetupResponse_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_E_RABSetupResponse_p->s1ap_E_RABSetupResponse_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_E_RABSetupResponseIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_E_RABSetupResponseIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABSetupListBearerSURes:
            {
                S1ap_E_RABSetupListBearerSURes_t *s1apERABSetupListBearerSURes_p = NULL;
                s1ap_E_RABSetupResponseIEs->presenceMask |= S1AP_E_RABSETUPRESPONSEIES_E_RABSETUPLISTBEARERSURES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABSetupListBearerSURes, (void**)&s1apERABSetupListBearerSURes_p);
                if (tempDecoded < 0 || s1apERABSetupListBearerSURes_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABSetupListBearerSURes failed\n");
                    if (s1apERABSetupListBearerSURes_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupListBearerSURes, s1apERABSetupListBearerSURes_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABSetupListBearerSURes, s1apERABSetupListBearerSURes_p);
                if (s1ap_decode_s1ap_e_rabsetuplistbearersures(&s1ap_E_RABSetupResponseIEs->e_RABSetupListBearerSURes, s1apERABSetupListBearerSURes_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABSetupListBearerSURes failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupListBearerSURes, s1apERABSetupListBearerSURes_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABFailedToSetupListBearerSURes:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                s1ap_E_RABSetupResponseIEs->presenceMask |= S1AP_E_RABSETUPRESPONSEIES_E_RABFAILEDTOSETUPLISTBEARERSURES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABFailedToSetupListBearerSURes failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_E_RABSetupResponseIEs->e_RABFailedToSetupListBearerSURes, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_E_RABSetupResponseIEs->presenceMask |= S1AP_E_RABSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_E_RABSetupResponseIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_e_rabsetupresponseies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupResponse, s1ap_E_RABSetupResponse_p);
    return decoded;
}

int s1ap_decode_s1ap_pathswitchrequesties(
    S1ap_PathSwitchRequestIEs_t *s1ap_PathSwitchRequestIEs,
    ANY_t *any_p) {

    S1ap_PathSwitchRequest_t  s1ap_PathSwitchRequest;
    S1ap_PathSwitchRequest_t *s1ap_PathSwitchRequest_p = &s1ap_PathSwitchRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_PathSwitchRequestIEs != NULL);

    memset(s1ap_PathSwitchRequestIEs, 0, sizeof(S1ap_PathSwitchRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_PathSwitchRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_PathSwitchRequest, (void**)&s1ap_PathSwitchRequest_p);

    for (i = 0; i < s1ap_PathSwitchRequest_p->s1ap_PathSwitchRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_PathSwitchRequest_p->s1ap_PathSwitchRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_PathSwitchRequestIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABToBeSwitchedDLList:
            {
                S1ap_E_RABToBeSwitchedDLList_t *s1apERABToBeSwitchedDLList_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSwitchedDLList, (void**)&s1apERABToBeSwitchedDLList_p);
                if (tempDecoded < 0 || s1apERABToBeSwitchedDLList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSwitchedDLList failed\n");
                    if (s1apERABToBeSwitchedDLList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSwitchedDLList, s1apERABToBeSwitchedDLList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSwitchedDLList, s1apERABToBeSwitchedDLList_p);
                if (s1ap_decode_s1ap_e_rabtobeswitcheddllist(&s1ap_PathSwitchRequestIEs->e_RABToBeSwitchedDLList, s1apERABToBeSwitchedDLList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABToBeSwitchedDLList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSwitchedDLList, s1apERABToBeSwitchedDLList_p);
                }
            } break;
            case S1ap_ProtocolIE_ID_id_SourceMME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE sourceMME_UE_S1AP_ID failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_PathSwitchRequestIEs->sourceMME_UE_S1AP_ID, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_EUTRAN_CGI:
            {
                S1ap_EUTRAN_CGI_t *s1apEUTRANCGI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_EUTRAN_CGI, (void**)&s1apEUTRANCGI_p);
                if (tempDecoded < 0 || s1apEUTRANCGI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eutran_cgi failed\n");
                    if (s1apEUTRANCGI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                memcpy(&s1ap_PathSwitchRequestIEs->eutran_cgi, s1apEUTRANCGI_p, sizeof(S1ap_EUTRAN_CGI_t));
                FREEMEM(s1apEUTRANCGI_p);
                s1apEUTRANCGI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TAI:
            {
                S1ap_TAI_t *s1apTAI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TAI, (void**)&s1apTAI_p);
                if (tempDecoded < 0 || s1apTAI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE tai failed\n");
                    if (s1apTAI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TAI, s1apTAI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TAI, s1apTAI_p);
                memcpy(&s1ap_PathSwitchRequestIEs->tai, s1apTAI_p, sizeof(S1ap_TAI_t));
                FREEMEM(s1apTAI_p);
                s1apTAI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_UESecurityCapabilities:
            {
                S1ap_UESecurityCapabilities_t *s1apUESecurityCapabilities_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UESecurityCapabilities, (void**)&s1apUESecurityCapabilities_p);
                if (tempDecoded < 0 || s1apUESecurityCapabilities_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE ueSecurityCapabilities failed\n");
                    if (s1apUESecurityCapabilities_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UESecurityCapabilities, s1apUESecurityCapabilities_p);
                memcpy(&s1ap_PathSwitchRequestIEs->ueSecurityCapabilities, s1apUESecurityCapabilities_p, sizeof(S1ap_UESecurityCapabilities_t));
                FREEMEM(s1apUESecurityCapabilities_p);
                s1apUESecurityCapabilities_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_Id:
            {
                S1ap_CSG_Id_t *s1apCSGId_p = NULL;
                s1ap_PathSwitchRequestIEs->presenceMask |= S1AP_PATHSWITCHREQUESTIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_Id, (void**)&s1apCSGId_p);
                if (tempDecoded < 0 || s1apCSGId_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_Id failed\n");
                    if (s1apCSGId_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_Id, s1apCSGId_p);
                memcpy(&s1ap_PathSwitchRequestIEs->csG_Id, s1apCSGId_p, sizeof(S1ap_CSG_Id_t));
                FREEMEM(s1apCSGId_p);
                s1apCSGId_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CellAccessMode:
            {
                S1ap_CellAccessMode_t *s1apCellAccessMode_p = NULL;
                s1ap_PathSwitchRequestIEs->presenceMask |= S1AP_PATHSWITCHREQUESTIES_CELLACCESSMODE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CellAccessMode, (void**)&s1apCellAccessMode_p);
                if (tempDecoded < 0 || s1apCellAccessMode_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cellAccessMode failed\n");
                    if (s1apCellAccessMode_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CellAccessMode, s1apCellAccessMode_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CellAccessMode, s1apCellAccessMode_p);
                memcpy(&s1ap_PathSwitchRequestIEs->cellAccessMode, s1apCellAccessMode_p, sizeof(S1ap_CellAccessMode_t));
                FREEMEM(s1apCellAccessMode_p);
                s1apCellAccessMode_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_SourceMME_GUMMEI:
            {
                S1ap_GUMMEI_t *s1apGUMMEI_p = NULL;
                s1ap_PathSwitchRequestIEs->presenceMask |= S1AP_PATHSWITCHREQUESTIES_SOURCEMME_GUMMEI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_GUMMEI, (void**)&s1apGUMMEI_p);
                if (tempDecoded < 0 || s1apGUMMEI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE sourceMME_GUMMEI failed\n");
                    if (s1apGUMMEI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_GUMMEI, s1apGUMMEI_p);
                memcpy(&s1ap_PathSwitchRequestIEs->sourceMME_GUMMEI, s1apGUMMEI_p, sizeof(S1ap_GUMMEI_t));
                FREEMEM(s1apGUMMEI_p);
                s1apGUMMEI_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_pathswitchrequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_PathSwitchRequest, s1ap_PathSwitchRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_downlinks1cdma2000tunnelingies(
    S1ap_DownlinkS1cdma2000tunnelingIEs_t *s1ap_DownlinkS1cdma2000tunnelingIEs,
    ANY_t *any_p) {

    S1ap_DownlinkS1cdma2000tunneling_t  s1ap_DownlinkS1cdma2000tunneling;
    S1ap_DownlinkS1cdma2000tunneling_t *s1ap_DownlinkS1cdma2000tunneling_p = &s1ap_DownlinkS1cdma2000tunneling;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_DownlinkS1cdma2000tunnelingIEs != NULL);

    memset(s1ap_DownlinkS1cdma2000tunnelingIEs, 0, sizeof(S1ap_DownlinkS1cdma2000tunnelingIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_DownlinkS1cdma2000tunnelingIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_DownlinkS1cdma2000tunneling, (void**)&s1ap_DownlinkS1cdma2000tunneling_p);

    for (i = 0; i < s1ap_DownlinkS1cdma2000tunneling_p->s1ap_DownlinkS1cdma2000tunneling_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_DownlinkS1cdma2000tunneling_p->s1ap_DownlinkS1cdma2000tunneling_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_DownlinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_DownlinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_E_RABSubjecttoDataForwardingList:
            {
                S1ap_E_RABDataForwardingList_t *s1apERABDataForwardingList_p = NULL;
                s1ap_DownlinkS1cdma2000tunnelingIEs->presenceMask |= S1AP_DOWNLINKS1CDMA2000TUNNELINGIES_E_RABSUBJECTTODATAFORWARDINGLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABDataForwardingList, (void**)&s1apERABDataForwardingList_p);
                if (tempDecoded < 0 || s1apERABDataForwardingList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABSubjecttoDataForwardingList failed\n");
                    if (s1apERABDataForwardingList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABDataForwardingList, s1apERABDataForwardingList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABDataForwardingList, s1apERABDataForwardingList_p);
                if (s1ap_decode_s1ap_e_rabdataforwardinglist(&s1ap_DownlinkS1cdma2000tunnelingIEs->e_RABSubjecttoDataForwardingList, s1apERABDataForwardingList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABDataForwardingList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABDataForwardingList, s1apERABDataForwardingList_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_cdma2000HOStatus:
            {
                S1ap_Cdma2000HOStatus_t *s1apCdma2000HOStatus_p = NULL;
                s1ap_DownlinkS1cdma2000tunnelingIEs->presenceMask |= S1AP_DOWNLINKS1CDMA2000TUNNELINGIES_CDMA2000HOSTATUS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000HOStatus, (void**)&s1apCdma2000HOStatus_p);
                if (tempDecoded < 0 || s1apCdma2000HOStatus_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000HOStatus failed\n");
                    if (s1apCdma2000HOStatus_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000HOStatus, s1apCdma2000HOStatus_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000HOStatus, s1apCdma2000HOStatus_p);
                memcpy(&s1ap_DownlinkS1cdma2000tunnelingIEs->cdma2000HOStatus, s1apCdma2000HOStatus_p, sizeof(S1ap_Cdma2000HOStatus_t));
                FREEMEM(s1apCdma2000HOStatus_p);
                s1apCdma2000HOStatus_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_cdma2000RATType:
            {
                S1ap_Cdma2000RATType_t *s1apCdma2000RATType_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000RATType, (void**)&s1apCdma2000RATType_p);
                if (tempDecoded < 0 || s1apCdma2000RATType_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000RATType failed\n");
                    if (s1apCdma2000RATType_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000RATType, s1apCdma2000RATType_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000RATType, s1apCdma2000RATType_p);
                memcpy(&s1ap_DownlinkS1cdma2000tunnelingIEs->cdma2000RATType, s1apCdma2000RATType_p, sizeof(S1ap_Cdma2000RATType_t));
                FREEMEM(s1apCdma2000RATType_p);
                s1apCdma2000RATType_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_cdma2000PDU:
            {
                S1ap_Cdma2000PDU_t *s1apCdma2000PDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cdma2000PDU, (void**)&s1apCdma2000PDU_p);
                if (tempDecoded < 0 || s1apCdma2000PDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cdma2000PDU failed\n");
                    if (s1apCdma2000PDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cdma2000PDU, s1apCdma2000PDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cdma2000PDU, s1apCdma2000PDU_p);
                memcpy(&s1ap_DownlinkS1cdma2000tunnelingIEs->cdma2000PDU, s1apCdma2000PDU_p, sizeof(S1ap_Cdma2000PDU_t));
                FREEMEM(s1apCdma2000PDU_p);
                s1apCdma2000PDU_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_downlinks1cdma2000tunnelingies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_DownlinkS1cdma2000tunneling, s1ap_DownlinkS1cdma2000tunneling_p);
    return decoded;
}

int s1ap_decode_s1ap_e_rabreleaseindicationies(
    S1ap_E_RABReleaseIndicationIEs_t *s1ap_E_RABReleaseIndicationIEs,
    ANY_t *any_p) {

    S1ap_E_RABReleaseIndication_t  s1ap_E_RABReleaseIndication;
    S1ap_E_RABReleaseIndication_t *s1ap_E_RABReleaseIndication_p = &s1ap_E_RABReleaseIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_E_RABReleaseIndicationIEs != NULL);

    memset(s1ap_E_RABReleaseIndicationIEs, 0, sizeof(S1ap_E_RABReleaseIndicationIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_E_RABReleaseIndicationIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_E_RABReleaseIndication, (void**)&s1ap_E_RABReleaseIndication_p);

    for (i = 0; i < s1ap_E_RABReleaseIndication_p->s1ap_E_RABReleaseIndication_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_E_RABReleaseIndication_p->s1ap_E_RABReleaseIndication_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_E_RABReleaseIndicationIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_E_RABReleaseIndicationIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABReleasedList:
            {
                S1ap_E_RABList_t *s1apERABList_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABList, (void**)&s1apERABList_p);
                if (tempDecoded < 0 || s1apERABList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABReleasedList failed\n");
                    if (s1apERABList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABList, s1apERABList_p);
                if (s1ap_decode_s1ap_e_rablist(&s1ap_E_RABReleaseIndicationIEs->e_RABReleasedList, s1apERABList_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABList failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABList, s1apERABList_p);
                }
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_e_rabreleaseindicationies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABReleaseIndication, s1ap_E_RABReleaseIndication_p);
    return decoded;
}

int s1ap_decode_s1ap_s1setuprequesties(
    S1ap_S1SetupRequestIEs_t *s1ap_S1SetupRequestIEs,
    ANY_t *any_p) {

    S1ap_S1SetupRequest_t  s1ap_S1SetupRequest;
    S1ap_S1SetupRequest_t *s1ap_S1SetupRequest_p = &s1ap_S1SetupRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_S1SetupRequestIEs != NULL);

    memset(s1ap_S1SetupRequestIEs, 0, sizeof(S1ap_S1SetupRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_S1SetupRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_S1SetupRequest, (void**)&s1ap_S1SetupRequest_p);

    for (i = 0; i < s1ap_S1SetupRequest_p->s1ap_S1SetupRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_S1SetupRequest_p->s1ap_S1SetupRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Global_ENB_ID:
            {
                S1ap_Global_ENB_ID_t *s1apGlobalENBID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Global_ENB_ID, (void**)&s1apGlobalENBID_p);
                if (tempDecoded < 0 || s1apGlobalENBID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE global_ENB_ID failed\n");
                    if (s1apGlobalENBID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Global_ENB_ID, s1apGlobalENBID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Global_ENB_ID, s1apGlobalENBID_p);
                memcpy(&s1ap_S1SetupRequestIEs->global_ENB_ID, s1apGlobalENBID_p, sizeof(S1ap_Global_ENB_ID_t));
                FREEMEM(s1apGlobalENBID_p);
                s1apGlobalENBID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_eNBname:
            {
                S1ap_ENBname_t *s1apENBname_p = NULL;
                s1ap_S1SetupRequestIEs->presenceMask |= S1AP_S1SETUPREQUESTIES_ENBNAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENBname, (void**)&s1apENBname_p);
                if (tempDecoded < 0 || s1apENBname_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNBname failed\n");
                    if (s1apENBname_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENBname, s1apENBname_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENBname, s1apENBname_p);
                memcpy(&s1ap_S1SetupRequestIEs->eNBname, s1apENBname_p, sizeof(S1ap_ENBname_t));
                FREEMEM(s1apENBname_p);
                s1apENBname_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_SupportedTAs:
            {
                S1ap_SupportedTAs_t *s1apSupportedTAs_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_SupportedTAs, (void**)&s1apSupportedTAs_p);
                if (tempDecoded < 0 || s1apSupportedTAs_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE supportedTAs failed\n");
                    if (s1apSupportedTAs_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_SupportedTAs, s1apSupportedTAs_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_SupportedTAs, s1apSupportedTAs_p);
                memcpy(&s1ap_S1SetupRequestIEs->supportedTAs, s1apSupportedTAs_p, sizeof(S1ap_SupportedTAs_t));
                FREEMEM(s1apSupportedTAs_p);
                s1apSupportedTAs_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_DefaultPagingDRX:
            {
                S1ap_PagingDRX_t *s1apPagingDRX_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_PagingDRX, (void**)&s1apPagingDRX_p);
                if (tempDecoded < 0 || s1apPagingDRX_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE defaultPagingDRX failed\n");
                    if (s1apPagingDRX_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_PagingDRX, s1apPagingDRX_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_PagingDRX, s1apPagingDRX_p);
                memcpy(&s1ap_S1SetupRequestIEs->defaultPagingDRX, s1apPagingDRX_p, sizeof(S1ap_PagingDRX_t));
                FREEMEM(s1apPagingDRX_p);
                s1apPagingDRX_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CSG_IdList:
            {
                S1ap_CSG_IdList_t *s1apCSGIdList_p = NULL;
                s1ap_S1SetupRequestIEs->presenceMask |= S1AP_S1SETUPREQUESTIES_CSG_IDLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CSG_IdList, (void**)&s1apCSGIdList_p);
                if (tempDecoded < 0 || s1apCSGIdList_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE csG_IdList failed\n");
                    if (s1apCSGIdList_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CSG_IdList, s1apCSGIdList_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CSG_IdList, s1apCSGIdList_p);
                memcpy(&s1ap_S1SetupRequestIEs->csG_IdList, s1apCSGIdList_p, sizeof(S1ap_CSG_IdList_t));
                FREEMEM(s1apCSGIdList_p);
                s1apCSGIdList_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_s1setuprequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_S1SetupRequest, s1ap_S1SetupRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_nasnondeliveryindication_ies(
    S1ap_NASNonDeliveryIndication_IEs_t *s1ap_NASNonDeliveryIndication_IEs,
    ANY_t *any_p) {

    S1ap_NASNonDeliveryIndication_t  s1ap_NASNonDeliveryIndication;
    S1ap_NASNonDeliveryIndication_t *s1ap_NASNonDeliveryIndication_p = &s1ap_NASNonDeliveryIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_NASNonDeliveryIndication_IEs != NULL);

    memset(s1ap_NASNonDeliveryIndication_IEs, 0, sizeof(S1ap_NASNonDeliveryIndication_IEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_NASNonDeliveryIndication_IEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_NASNonDeliveryIndication, (void**)&s1ap_NASNonDeliveryIndication_p);

    for (i = 0; i < s1ap_NASNonDeliveryIndication_p->s1ap_NASNonDeliveryIndication_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_NASNonDeliveryIndication_p->s1ap_NASNonDeliveryIndication_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_NASNonDeliveryIndication_IEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_NASNonDeliveryIndication_IEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_NAS_PDU:
            {
                S1ap_NAS_PDU_t *s1apNASPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_NAS_PDU, (void**)&s1apNASPDU_p);
                if (tempDecoded < 0 || s1apNASPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE nas_pdu failed\n");
                    if (s1apNASPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_NAS_PDU, s1apNASPDU_p);
                memcpy(&s1ap_NASNonDeliveryIndication_IEs->nas_pdu, s1apNASPDU_p, sizeof(S1ap_NAS_PDU_t));
                FREEMEM(s1apNASPDU_p);
                s1apNASPDU_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Cause:
            {
                S1ap_Cause_t *s1apCause_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Cause, (void**)&s1apCause_p);
                if (tempDecoded < 0 || s1apCause_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE cause failed\n");
                    if (s1apCause_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Cause, s1apCause_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Cause, s1apCause_p);
                memcpy(&s1ap_NASNonDeliveryIndication_IEs->cause, s1apCause_p, sizeof(S1ap_Cause_t));
                FREEMEM(s1apCause_p);
                s1apCause_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_nasnondeliveryindication_ies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_NASNonDeliveryIndication, s1ap_NASNonDeliveryIndication_p);
    return decoded;
}

int s1ap_decode_s1ap_celltraffictraceies(
    S1ap_CellTrafficTraceIEs_t *s1ap_CellTrafficTraceIEs,
    ANY_t *any_p) {

    S1ap_CellTrafficTrace_t  s1ap_CellTrafficTrace;
    S1ap_CellTrafficTrace_t *s1ap_CellTrafficTrace_p = &s1ap_CellTrafficTrace;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_CellTrafficTraceIEs != NULL);

    memset(s1ap_CellTrafficTraceIEs, 0, sizeof(S1ap_CellTrafficTraceIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_CellTrafficTraceIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_CellTrafficTrace, (void**)&s1ap_CellTrafficTrace_p);

    for (i = 0; i < s1ap_CellTrafficTrace_p->s1ap_CellTrafficTrace_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_CellTrafficTrace_p->s1ap_CellTrafficTrace_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_CellTrafficTraceIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_CellTrafficTraceIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_UTRAN_Trace_ID:
            {
                S1ap_E_UTRAN_Trace_ID_t *s1apEUTRANTraceID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_UTRAN_Trace_ID, (void**)&s1apEUTRANTraceID_p);
                if (tempDecoded < 0 || s1apEUTRANTraceID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_UTRAN_Trace_ID failed\n");
                    if (s1apEUTRANTraceID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_UTRAN_Trace_ID, s1apEUTRANTraceID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_UTRAN_Trace_ID, s1apEUTRANTraceID_p);
                memcpy(&s1ap_CellTrafficTraceIEs->e_UTRAN_Trace_ID, s1apEUTRANTraceID_p, sizeof(S1ap_E_UTRAN_Trace_ID_t));
                FREEMEM(s1apEUTRANTraceID_p);
                s1apEUTRANTraceID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_EUTRAN_CGI:
            {
                S1ap_EUTRAN_CGI_t *s1apEUTRANCGI_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_EUTRAN_CGI, (void**)&s1apEUTRANCGI_p);
                if (tempDecoded < 0 || s1apEUTRANCGI_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eutran_cgi failed\n");
                    if (s1apEUTRANCGI_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_EUTRAN_CGI, s1apEUTRANCGI_p);
                memcpy(&s1ap_CellTrafficTraceIEs->eutran_cgi, s1apEUTRANCGI_p, sizeof(S1ap_EUTRAN_CGI_t));
                FREEMEM(s1apEUTRANCGI_p);
                s1apEUTRANCGI_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_TraceCollectionEntityIPAddress:
            {
                S1ap_TransportLayerAddress_t *s1apTransportLayerAddress_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TransportLayerAddress, (void**)&s1apTransportLayerAddress_p);
                if (tempDecoded < 0 || s1apTransportLayerAddress_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE traceCollectionEntityIPAddress failed\n");
                    if (s1apTransportLayerAddress_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TransportLayerAddress, s1apTransportLayerAddress_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TransportLayerAddress, s1apTransportLayerAddress_p);
                memcpy(&s1ap_CellTrafficTraceIEs->traceCollectionEntityIPAddress, s1apTransportLayerAddress_p, sizeof(S1ap_TransportLayerAddress_t));
                FREEMEM(s1apTransportLayerAddress_p);
                s1apTransportLayerAddress_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_PrivacyIndicator:
            {
                S1ap_PrivacyIndicator_t *s1apPrivacyIndicator_p = NULL;
                s1ap_CellTrafficTraceIEs->presenceMask |= S1AP_CELLTRAFFICTRACEIES_PRIVACYINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_PrivacyIndicator, (void**)&s1apPrivacyIndicator_p);
                if (tempDecoded < 0 || s1apPrivacyIndicator_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE privacyIndicator failed\n");
                    if (s1apPrivacyIndicator_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_PrivacyIndicator, s1apPrivacyIndicator_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_PrivacyIndicator, s1apPrivacyIndicator_p);
                memcpy(&s1ap_CellTrafficTraceIEs->privacyIndicator, s1apPrivacyIndicator_p, sizeof(S1ap_PrivacyIndicator_t));
                FREEMEM(s1apPrivacyIndicator_p);
                s1apPrivacyIndicator_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_celltraffictraceies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_CellTrafficTrace, s1ap_CellTrafficTrace_p);
    return decoded;
}

int s1ap_decode_s1ap_e_rabsetuprequesties(
    S1ap_E_RABSetupRequestIEs_t *s1ap_E_RABSetupRequestIEs,
    ANY_t *any_p) {

    S1ap_E_RABSetupRequest_t  s1ap_E_RABSetupRequest;
    S1ap_E_RABSetupRequest_t *s1ap_E_RABSetupRequest_p = &s1ap_E_RABSetupRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_E_RABSetupRequestIEs != NULL);

    memset(s1ap_E_RABSetupRequestIEs, 0, sizeof(S1ap_E_RABSetupRequestIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_E_RABSetupRequestIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_E_RABSetupRequest, (void**)&s1ap_E_RABSetupRequest_p);

    for (i = 0; i < s1ap_E_RABSetupRequest_p->s1ap_E_RABSetupRequest_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_E_RABSetupRequest_p->s1ap_E_RABSetupRequest_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_E_RABSetupRequestIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_E_RABSetupRequestIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                S1ap_UEAggregateMaximumBitrate_t *s1apUEAggregateMaximumBitrate_p = NULL;
                s1ap_E_RABSetupRequestIEs->presenceMask |= S1AP_E_RABSETUPREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UEAggregateMaximumBitrate, (void**)&s1apUEAggregateMaximumBitrate_p);
                if (tempDecoded < 0 || s1apUEAggregateMaximumBitrate_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uEaggregateMaximumBitrate failed\n");
                    if (s1apUEAggregateMaximumBitrate_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UEAggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p);
                memcpy(&s1ap_E_RABSetupRequestIEs->uEaggregateMaximumBitrate, s1apUEAggregateMaximumBitrate_p, sizeof(S1ap_UEAggregateMaximumBitrate_t));
                FREEMEM(s1apUEAggregateMaximumBitrate_p);
                s1apUEAggregateMaximumBitrate_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_E_RABToBeSetupListBearerSUReq:
            {
                S1ap_E_RABToBeSetupListBearerSUReq_t *s1apERABToBeSetupListBearerSUReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSetupListBearerSUReq, (void**)&s1apERABToBeSetupListBearerSUReq_p);
                if (tempDecoded < 0 || s1apERABToBeSetupListBearerSUReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSetupListBearerSUReq failed\n");
                    if (s1apERABToBeSetupListBearerSUReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupListBearerSUReq, s1apERABToBeSetupListBearerSUReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSetupListBearerSUReq, s1apERABToBeSetupListBearerSUReq_p);
                if (s1ap_decode_s1ap_e_rabtobesetuplistbearersureq(&s1ap_E_RABSetupRequestIEs->e_RABToBeSetupListBearerSUReq, s1apERABToBeSetupListBearerSUReq_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apERABToBeSetupListBearerSUReq failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupListBearerSUReq, s1apERABToBeSetupListBearerSUReq_p);
                }
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_e_rabsetuprequesties\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupRequest, s1ap_E_RABSetupRequest_p);
    return decoded;
}

int s1ap_decode_s1ap_enbstatustransferies(
    S1ap_ENBStatusTransferIEs_t *s1ap_ENBStatusTransferIEs,
    ANY_t *any_p) {

    S1ap_ENBStatusTransfer_t  s1ap_ENBStatusTransfer;
    S1ap_ENBStatusTransfer_t *s1ap_ENBStatusTransfer_p = &s1ap_ENBStatusTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ENBStatusTransferIEs != NULL);

    memset(s1ap_ENBStatusTransferIEs, 0, sizeof(S1ap_ENBStatusTransferIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ENBStatusTransferIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ENBStatusTransfer, (void**)&s1ap_ENBStatusTransfer_p);

    for (i = 0; i < s1ap_ENBStatusTransfer_p->s1ap_ENBStatusTransfer_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ENBStatusTransfer_p->s1ap_ENBStatusTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_ENBStatusTransferIEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_ENBStatusTransferIEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer:
            {
                S1ap_ENB_StatusTransfer_TransparentContainer_t *s1apENBStatusTransferTransparentContainer_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, (void**)&s1apENBStatusTransferTransparentContainer_p);
                if (tempDecoded < 0 || s1apENBStatusTransferTransparentContainer_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_StatusTransfer_TransparentContainer failed\n");
                    if (s1apENBStatusTransferTransparentContainer_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, s1apENBStatusTransferTransparentContainer_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, s1apENBStatusTransferTransparentContainer_p);
                memcpy(&s1ap_ENBStatusTransferIEs->eNB_StatusTransfer_TransparentContainer, s1apENBStatusTransferTransparentContainer_p, sizeof(S1ap_ENB_StatusTransfer_TransparentContainer_t));
                FREEMEM(s1apENBStatusTransferTransparentContainer_p);
                s1apENBStatusTransferTransparentContainer_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_enbstatustransferies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ENBStatusTransfer, s1ap_ENBStatusTransfer_p);
    return decoded;
}

int s1ap_decode_s1ap_resetacknowledgeies(
    S1ap_ResetAcknowledgeIEs_t *s1ap_ResetAcknowledgeIEs,
    ANY_t *any_p) {

    S1ap_ResetAcknowledge_t  s1ap_ResetAcknowledge;
    S1ap_ResetAcknowledge_t *s1ap_ResetAcknowledge_p = &s1ap_ResetAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_ResetAcknowledgeIEs != NULL);

    memset(s1ap_ResetAcknowledgeIEs, 0, sizeof(S1ap_ResetAcknowledgeIEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_ResetAcknowledgeIEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_ResetAcknowledge, (void**)&s1ap_ResetAcknowledge_p);

    for (i = 0; i < s1ap_ResetAcknowledge_p->s1ap_ResetAcknowledge_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_ResetAcknowledge_p->s1ap_ResetAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionListResAck:
            {
                S1ap_UE_associatedLogicalS1_ConnectionListResAck_t *s1apUEassociatedLogicalS1ConnectionListResAck_p = NULL;
                s1ap_ResetAcknowledgeIEs->presenceMask |= S1AP_RESETACKNOWLEDGEIES_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionListResAck, (void**)&s1apUEassociatedLogicalS1ConnectionListResAck_p);
                if (tempDecoded < 0 || s1apUEassociatedLogicalS1ConnectionListResAck_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uE_associatedLogicalS1_ConnectionListResAck failed\n");
                    if (s1apUEassociatedLogicalS1ConnectionListResAck_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionListResAck, s1apUEassociatedLogicalS1ConnectionListResAck_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionListResAck, s1apUEassociatedLogicalS1ConnectionListResAck_p);
                if (s1ap_decode_s1ap_ue_associatedlogicals1_connectionlistresack(&s1ap_ResetAcknowledgeIEs->uE_associatedLogicalS1_ConnectionListResAck, s1apUEassociatedLogicalS1ConnectionListResAck_p) < 0) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of encapsulated IE s1apUEassociatedLogicalS1ConnectionListResAck failed\n");
                    ASN_STRUCT_FREE(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionListResAck, s1apUEassociatedLogicalS1ConnectionListResAck_p);
                }
            } break;
            /* Optional field */
            case S1ap_ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                S1ap_CriticalityDiagnostics_t *s1apCriticalityDiagnostics_p = NULL;
                s1ap_ResetAcknowledgeIEs->presenceMask |= S1AP_RESETACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_CriticalityDiagnostics, (void**)&s1apCriticalityDiagnostics_p);
                if (tempDecoded < 0 || s1apCriticalityDiagnostics_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE criticalityDiagnostics failed\n");
                    if (s1apCriticalityDiagnostics_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_CriticalityDiagnostics, s1apCriticalityDiagnostics_p);
                memcpy(&s1ap_ResetAcknowledgeIEs->criticalityDiagnostics, s1apCriticalityDiagnostics_p, sizeof(S1ap_CriticalityDiagnostics_t));
                FREEMEM(s1apCriticalityDiagnostics_p);
                s1apCriticalityDiagnostics_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_resetacknowledgeies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_ResetAcknowledge, s1ap_ResetAcknowledge_p);
    return decoded;
}

int s1ap_decode_s1ap_uplinkueassociatedlppatransport_ies(
    S1ap_UplinkUEAssociatedLPPaTransport_IEs_t *s1ap_UplinkUEAssociatedLPPaTransport_IEs,
    ANY_t *any_p) {

    S1ap_UplinkUEAssociatedLPPaTransport_t  s1ap_UplinkUEAssociatedLPPaTransport;
    S1ap_UplinkUEAssociatedLPPaTransport_t *s1ap_UplinkUEAssociatedLPPaTransport_p = &s1ap_UplinkUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1ap_UplinkUEAssociatedLPPaTransport_IEs != NULL);

    memset(s1ap_UplinkUEAssociatedLPPaTransport_IEs, 0, sizeof(S1ap_UplinkUEAssociatedLPPaTransport_IEs_t));
   OAILOG_DEBUG (LOG_S1AP, "Decoding message S1ap_UplinkUEAssociatedLPPaTransport_IEs (%s:%d)\n", __FILE__, __LINE__);

    ANY_to_type_aper(any_p, &asn_DEF_S1ap_UplinkUEAssociatedLPPaTransport, (void**)&s1ap_UplinkUEAssociatedLPPaTransport_p);

    for (i = 0; i < s1ap_UplinkUEAssociatedLPPaTransport_p->s1ap_UplinkUEAssociatedLPPaTransport_ies.list.count; i++) {
        S1ap_IE_t *ie_p;
        ie_p = s1ap_UplinkUEAssociatedLPPaTransport_p->s1ap_UplinkUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case S1ap_ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                S1ap_MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_MME_UE_S1AP_ID, (void**)&s1apMMEUES1APID_p);
                if (tempDecoded < 0 || s1apMMEUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE mme_ue_s1ap_id failed\n");
                    if (s1apMMEUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_MME_UE_S1AP_ID, s1apMMEUES1APID_p);
                memcpy(&s1ap_UplinkUEAssociatedLPPaTransport_IEs->mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(S1ap_MME_UE_S1AP_ID_t));
                FREEMEM(s1apMMEUES1APID_p);
                s1apMMEUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                S1ap_ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_ENB_UE_S1AP_ID, (void**)&s1apENBUES1APID_p);
                if (tempDecoded < 0 || s1apENBUES1APID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE eNB_UE_S1AP_ID failed\n");
                    if (s1apENBUES1APID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_ENB_UE_S1AP_ID, s1apENBUES1APID_p);
                memcpy(&s1ap_UplinkUEAssociatedLPPaTransport_IEs->eNB_UE_S1AP_ID, s1apENBUES1APID_p, sizeof(S1ap_ENB_UE_S1AP_ID_t));
                FREEMEM(s1apENBUES1APID_p);
                s1apENBUES1APID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_Routing_ID:
            {
                S1ap_Routing_ID_t *s1apRoutingID_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Routing_ID, (void**)&s1apRoutingID_p);
                if (tempDecoded < 0 || s1apRoutingID_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE routing_ID failed\n");
                    if (s1apRoutingID_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Routing_ID, s1apRoutingID_p);
                memcpy(&s1ap_UplinkUEAssociatedLPPaTransport_IEs->routing_ID, s1apRoutingID_p, sizeof(S1ap_Routing_ID_t));
                FREEMEM(s1apRoutingID_p);
                s1apRoutingID_p = NULL;
            } break;
            case S1ap_ProtocolIE_ID_id_LPPa_PDU:
            {
                S1ap_LPPa_PDU_t *s1apLPPaPDU_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_LPPa_PDU, (void**)&s1apLPPaPDU_p);
                if (tempDecoded < 0 || s1apLPPaPDU_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE lpPa_PDU failed\n");
                    if (s1apLPPaPDU_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_LPPa_PDU, s1apLPPaPDU_p);
                memcpy(&s1ap_UplinkUEAssociatedLPPaTransport_IEs->lpPa_PDU, s1apLPPaPDU_p, sizeof(S1ap_LPPa_PDU_t));
                FREEMEM(s1apLPPaPDU_p);
                s1apLPPaPDU_p = NULL;
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_ies\n", (int)ie_p->id);
                return -1;
        }
    }
    ASN_STRUCT_FREE(asn_DEF_S1ap_UplinkUEAssociatedLPPaTransport, s1ap_UplinkUEAssociatedLPPaTransport_p);
    return decoded;
}

int free_s1ap_e_rabreleaselistbearerrelcomp(
    S1ap_E_RABReleaseListBearerRelCompIEs_t *s1ap_E_RABReleaseListBearerRelCompIEs) {

    assert(s1ap_E_RABReleaseListBearerRelCompIEs != NULL);

    for (int i = 0; i < s1ap_E_RABReleaseListBearerRelCompIEs->s1ap_E_RABReleaseItemBearerRelComp.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABReleaseItemBearerRelComp, s1ap_E_RABReleaseListBearerRelCompIEs->s1ap_E_RABReleaseItemBearerRelComp.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabtobesetuplisthoreq(
    S1ap_E_RABToBeSetupListHOReqIEs_t *s1ap_E_RABToBeSetupListHOReqIEs) {

    assert(s1ap_E_RABToBeSetupListHOReqIEs != NULL);

    for (int i = 0; i < s1ap_E_RABToBeSetupListHOReqIEs->s1ap_E_RABToBeSetupItemHOReq.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSetupItemHOReq, s1ap_E_RABToBeSetupListHOReqIEs->s1ap_E_RABToBeSetupItemHOReq.array[i]);
    }
    return 0;
}

int free_s1ap_bearers_subjecttostatustransfer_list(
    S1ap_Bearers_SubjectToStatusTransfer_ListIEs_t *s1ap_Bearers_SubjectToStatusTransfer_ListIEs) {

    assert(s1ap_Bearers_SubjectToStatusTransfer_ListIEs != NULL);

    for (int i = 0; i < s1ap_Bearers_SubjectToStatusTransfer_ListIEs->s1ap_Bearers_SubjectToStatusTransfer_Item.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Bearers_SubjectToStatusTransfer_Item, s1ap_Bearers_SubjectToStatusTransfer_ListIEs->s1ap_Bearers_SubjectToStatusTransfer_Item.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabtobemodifiedlistbearermodreq(
    S1ap_E_RABToBeModifiedListBearerModReqIEs_t *s1ap_E_RABToBeModifiedListBearerModReqIEs) {

    assert(s1ap_E_RABToBeModifiedListBearerModReqIEs != NULL);

    for (int i = 0; i < s1ap_E_RABToBeModifiedListBearerModReqIEs->s1ap_E_RABToBeModifiedItemBearerModReq.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeModifiedItemBearerModReq, s1ap_E_RABToBeModifiedListBearerModReqIEs->s1ap_E_RABToBeModifiedItemBearerModReq.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabadmittedlist(
    S1ap_E_RABAdmittedListIEs_t *s1ap_E_RABAdmittedListIEs) {

    assert(s1ap_E_RABAdmittedListIEs != NULL);

    for (int i = 0; i < s1ap_E_RABAdmittedListIEs->s1ap_E_RABAdmittedItem.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABAdmittedItem, s1ap_E_RABAdmittedListIEs->s1ap_E_RABAdmittedItem.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabtobesetuplistbearersureq(
    S1ap_E_RABToBeSetupListBearerSUReqIEs_t *s1ap_E_RABToBeSetupListBearerSUReqIEs) {

    assert(s1ap_E_RABToBeSetupListBearerSUReqIEs != NULL);

    for (int i = 0; i < s1ap_E_RABToBeSetupListBearerSUReqIEs->s1ap_E_RABToBeSetupItemBearerSUReq.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSetupItemBearerSUReq, s1ap_E_RABToBeSetupListBearerSUReqIEs->s1ap_E_RABToBeSetupItemBearerSUReq.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabdataforwardinglist(
    S1ap_E_RABDataForwardingListIEs_t *s1ap_E_RABDataForwardingListIEs) {

    assert(s1ap_E_RABDataForwardingListIEs != NULL);

    for (int i = 0; i < s1ap_E_RABDataForwardingListIEs->s1ap_E_RABDataForwardingItem.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABDataForwardingItem, s1ap_E_RABDataForwardingListIEs->s1ap_E_RABDataForwardingItem.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabfailedtosetuplisthoreqack(
    S1ap_E_RABFailedToSetupListHOReqAckIEs_t *s1ap_E_RABFailedToSetupListHOReqAckIEs) {

    assert(s1ap_E_RABFailedToSetupListHOReqAckIEs != NULL);

    for (int i = 0; i < s1ap_E_RABFailedToSetupListHOReqAckIEs->s1ap_E_RABFailedToSetupItemHOReqAck.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABFailedToSetupItemHOReqAck, s1ap_E_RABFailedToSetupListHOReqAckIEs->s1ap_E_RABFailedToSetupItemHOReqAck.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabtobeswitcheddllist(
    S1ap_E_RABToBeSwitchedDLListIEs_t *s1ap_E_RABToBeSwitchedDLListIEs) {

    assert(s1ap_E_RABToBeSwitchedDLListIEs != NULL);

    for (int i = 0; i < s1ap_E_RABToBeSwitchedDLListIEs->s1ap_E_RABToBeSwitchedDLItem.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSwitchedDLItem, s1ap_E_RABToBeSwitchedDLListIEs->s1ap_E_RABToBeSwitchedDLItem.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabsetuplistctxtsures(
    S1ap_E_RABSetupListCtxtSUResIEs_t *s1ap_E_RABSetupListCtxtSUResIEs) {

    assert(s1ap_E_RABSetupListCtxtSUResIEs != NULL);

    for (int i = 0; i < s1ap_E_RABSetupListCtxtSUResIEs->s1ap_E_RABSetupItemCtxtSURes.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABSetupItemCtxtSURes, s1ap_E_RABSetupListCtxtSUResIEs->s1ap_E_RABSetupItemCtxtSURes.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabtobeswitchedullist(
    S1ap_E_RABToBeSwitchedULListIEs_t *s1ap_E_RABToBeSwitchedULListIEs) {

    assert(s1ap_E_RABToBeSwitchedULListIEs != NULL);

    for (int i = 0; i < s1ap_E_RABToBeSwitchedULListIEs->s1ap_E_RABToBeSwitchedULItem.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSwitchedULItem, s1ap_E_RABToBeSwitchedULListIEs->s1ap_E_RABToBeSwitchedULItem.array[i]);
    }
    return 0;
}

int free_s1ap_e_rablist(
    S1ap_E_RABListIEs_t *s1ap_E_RABListIEs) {

    assert(s1ap_E_RABListIEs != NULL);

    for (int i = 0; i < s1ap_E_RABListIEs->s1ap_E_RABItem.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABItem, s1ap_E_RABListIEs->s1ap_E_RABItem.array[i]);
    }
    return 0;
}

int free_s1ap_tailist(
    S1ap_TAIListIEs_t *s1ap_TAIListIEs) {

    assert(s1ap_TAIListIEs != NULL);

    for (int i = 0; i < s1ap_TAIListIEs->s1ap_TAIItem.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TAIItem, s1ap_TAIListIEs->s1ap_TAIItem.array[i]);
    }
    return 0;
}

int free_s1ap_ue_associatedlogicals1_connectionlistres(
    S1ap_UE_associatedLogicalS1_ConnectionListResIEs_t *s1ap_UE_associatedLogicalS1_ConnectionListRes) {

    assert(s1ap_UE_associatedLogicalS1_ConnectionListRes != NULL);

    for (int i = 0; i < s1ap_UE_associatedLogicalS1_ConnectionListRes->s1ap_UE_associatedLogicalS1_ConnectionItemRes.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, s1ap_UE_associatedLogicalS1_ConnectionListRes->s1ap_UE_associatedLogicalS1_ConnectionItemRes.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabsetuplistbearersures(
    S1ap_E_RABSetupListBearerSUResIEs_t *s1ap_E_RABSetupListBearerSUResIEs) {

    assert(s1ap_E_RABSetupListBearerSUResIEs != NULL);

    for (int i = 0; i < s1ap_E_RABSetupListBearerSUResIEs->s1ap_E_RABSetupItemBearerSURes.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABSetupItemBearerSURes, s1ap_E_RABSetupListBearerSUResIEs->s1ap_E_RABSetupItemBearerSURes.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabmodifylistbearermodres(
    S1ap_E_RABModifyListBearerModResIEs_t *s1ap_E_RABModifyListBearerModResIEs) {

    assert(s1ap_E_RABModifyListBearerModResIEs != NULL);

    for (int i = 0; i < s1ap_E_RABModifyListBearerModResIEs->s1ap_E_RABModifyItemBearerModRes.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABModifyItemBearerModRes, s1ap_E_RABModifyListBearerModResIEs->s1ap_E_RABModifyItemBearerModRes.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabtobesetuplistctxtsureq(
    S1ap_E_RABToBeSetupListCtxtSUReqIEs_t *s1ap_E_RABToBeSetupListCtxtSUReqIEs) {

    assert(s1ap_E_RABToBeSetupListCtxtSUReqIEs != NULL);

    for (int i = 0; i < s1ap_E_RABToBeSetupListCtxtSUReqIEs->s1ap_E_RABToBeSetupItemCtxtSUReq.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSetupItemCtxtSUReq, s1ap_E_RABToBeSetupListCtxtSUReqIEs->s1ap_E_RABToBeSetupItemCtxtSUReq.array[i]);
    }
    return 0;
}

int free_s1ap_ue_associatedlogicals1_connectionlistresack(
    S1ap_UE_associatedLogicalS1_ConnectionListResAckIEs_t *s1ap_UE_associatedLogicalS1_ConnectionListResAck) {

    assert(s1ap_UE_associatedLogicalS1_ConnectionListResAck != NULL);

    for (int i = 0; i < s1ap_UE_associatedLogicalS1_ConnectionListResAck->s1ap_UE_associatedLogicalS1_ConnectionItemResAck.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, s1ap_UE_associatedLogicalS1_ConnectionListResAck->s1ap_UE_associatedLogicalS1_ConnectionItemResAck.array[i]);
    }
    return 0;
}

int free_s1ap_e_rabinformationlist(
    S1ap_E_RABInformationListIEs_t *s1ap_E_RABInformationListIEs) {

    assert(s1ap_E_RABInformationListIEs != NULL);

    for (int i = 0; i < s1ap_E_RABInformationListIEs->s1ap_E_RABInformationItem.count; i++) {
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABInformationListItem, s1ap_E_RABInformationListIEs->s1ap_E_RABInformationItem.array[i]);
    }
    return 0;
}

int free_s1ap_deactivatetrace(
    S1ap_DeactivateTraceIEs_t *s1ap_DeactivateTraceIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_DeactivateTraceIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_DeactivateTraceIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_UTRAN_Trace_ID, &s1ap_DeactivateTraceIEs->e_UTRAN_Trace_ID);
    return 0;
}

int free_s1ap_e_rabreleaseitembearerrelcomp(
    S1ap_E_RABReleaseItemBearerRelCompIEs_t *s1ap_E_RABReleaseItemBearerRelCompIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABReleaseItemBearerRelComp, &s1ap_E_RABReleaseItemBearerRelCompIEs->e_RABReleaseItemBearerRelComp);
    return 0;
}

int free_s1ap_enbdirectinformationtransfer(
    S1ap_ENBDirectInformationTransferIEs_t *s1ap_ENBDirectInformationTransferIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Inter_SystemInformationTransferType, &s1ap_ENBDirectInformationTransferIEs->inter_SystemInformationTransferTypeEDT);
    return 0;
}

int free_s1ap_e_rabreleaseresponse(
    S1ap_E_RABReleaseResponseIEs_t *s1ap_E_RABReleaseResponseIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_E_RABReleaseResponseIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_E_RABReleaseResponseIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_E_RABReleaseResponseIEs->presenceMask & S1AP_E_RABRELEASERESPONSEIES_E_RABRELEASELISTBEARERRELCOMP_PRESENT)
        == S1AP_E_RABRELEASERESPONSEIES_E_RABRELEASELISTBEARERRELCOMP_PRESENT) 
        free_s1ap_e_rabreleaselistbearerrelcomp(&s1ap_E_RABReleaseResponseIEs->e_RABReleaseListBearerRelComp);
    /* Optional field */
    if ((s1ap_E_RABReleaseResponseIEs->presenceMask & S1AP_E_RABRELEASERESPONSEIES_E_RABFAILEDTORELEASELIST_PRESENT)
        == S1AP_E_RABRELEASERESPONSEIES_E_RABFAILEDTORELEASELIST_PRESENT) 
        free_s1ap_e_rablist(&s1ap_E_RABReleaseResponseIEs->e_RABFailedToReleaseList);
    /* Optional field */
    if ((s1ap_E_RABReleaseResponseIEs->presenceMask & S1AP_E_RABRELEASERESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_E_RABRELEASERESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_E_RABReleaseResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_uplinknonueassociatedlppatransport_(
    S1ap_UplinkNonUEAssociatedLPPaTransport_IEs_t *s1ap_UplinkNonUEAssociatedLPPaTransport_IEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Routing_ID, &s1ap_UplinkNonUEAssociatedLPPaTransport_IEs->routing_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_LPPa_PDU, &s1ap_UplinkNonUEAssociatedLPPaTransport_IEs->lpPa_PDU);
    return 0;
}

int free_s1ap_e_rabtobesetupitemhoreq(
    S1ap_E_RABToBeSetupItemHOReqIEs_t *s1ap_E_RABToBeSetupItemHOReqIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSetupItemHOReq, &s1ap_E_RABToBeSetupItemHOReqIEs->e_RABToBeSetupItemHOReq);
    return 0;
}

int free_s1ap_bearers_subjecttostatustransfer_item(
    S1ap_Bearers_SubjectToStatusTransfer_ItemIEs_t *s1ap_Bearers_SubjectToStatusTransfer_ItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Bearers_SubjectToStatusTransfer_Item, &s1ap_Bearers_SubjectToStatusTransfer_ItemIEs->bearers_SubjectToStatusTransfer_Item);
    return 0;
}

int free_s1ap_writereplacewarningresponse(
    S1ap_WriteReplaceWarningResponseIEs_t *s1ap_WriteReplaceWarningResponseIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MessageIdentifier, &s1ap_WriteReplaceWarningResponseIEs->messageIdentifier);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SerialNumber, &s1ap_WriteReplaceWarningResponseIEs->serialNumber);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningResponseIEs->presenceMask & S1AP_WRITEREPLACEWARNINGRESPONSEIES_BROADCASTCOMPLETEDAREALIST_PRESENT)
        == S1AP_WRITEREPLACEWARNINGRESPONSEIES_BROADCASTCOMPLETEDAREALIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_BroadcastCompletedAreaList, &s1ap_WriteReplaceWarningResponseIEs->broadcastCompletedAreaList);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningResponseIEs->presenceMask & S1AP_WRITEREPLACEWARNINGRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_WRITEREPLACEWARNINGRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_WriteReplaceWarningResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_handovercancel(
    S1ap_HandoverCancelIEs_t *s1ap_HandoverCancelIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverCancelIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_HandoverCancelIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_HandoverCancelIEs->cause);
    return 0;
}

int free_s1ap_handoverpreparationfailure(
    S1ap_HandoverPreparationFailureIEs_t *s1ap_HandoverPreparationFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverPreparationFailureIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_HandoverPreparationFailureIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_HandoverPreparationFailureIEs->cause);
    /* Optional field */
    if ((s1ap_HandoverPreparationFailureIEs->presenceMask & S1AP_HANDOVERPREPARATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_HANDOVERPREPARATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_HandoverPreparationFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_e_rabtobemodifieditembearermodreq(
    S1ap_E_RABToBeModifiedItemBearerModReqIEs_t *s1ap_E_RABToBeModifiedItemBearerModReqIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeModifiedItemBearerModReq, &s1ap_E_RABToBeModifiedItemBearerModReqIEs->e_RABToBeModifiedItemBearerModReq);
    return 0;
}

int free_s1ap_e_rabadmitteditem(
    S1ap_E_RABAdmittedItemIEs_t *s1ap_E_RABAdmittedItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABAdmittedItem, &s1ap_E_RABAdmittedItemIEs->e_RABAdmittedItem);
    return 0;
}

int free_s1ap_killrequest(
    S1ap_KillRequestIEs_t *s1ap_KillRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MessageIdentifier, &s1ap_KillRequestIEs->messageIdentifier);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SerialNumber, &s1ap_KillRequestIEs->serialNumber);
    /* Optional field */
    if ((s1ap_KillRequestIEs->presenceMask & S1AP_KILLREQUESTIES_WARNINGAREALIST_PRESENT)
        == S1AP_KILLREQUESTIES_WARNINGAREALIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_WarningAreaList, &s1ap_KillRequestIEs->warningAreaList);
    return 0;
}

int free_s1ap_writereplacewarningrequest(
    S1ap_WriteReplaceWarningRequestIEs_t *s1ap_WriteReplaceWarningRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MessageIdentifier, &s1ap_WriteReplaceWarningRequestIEs->messageIdentifier);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SerialNumber, &s1ap_WriteReplaceWarningRequestIEs->serialNumber);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningRequestIEs->presenceMask & S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGAREALIST_PRESENT)
        == S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGAREALIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_WarningAreaList, &s1ap_WriteReplaceWarningRequestIEs->warningAreaList);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RepetitionPeriod, &s1ap_WriteReplaceWarningRequestIEs->repetitionPeriod);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningRequestIEs->presenceMask & S1AP_WRITEREPLACEWARNINGREQUESTIES_EXTENDEDREPETITIONPERIOD_PRESENT)
        == S1AP_WRITEREPLACEWARNINGREQUESTIES_EXTENDEDREPETITIONPERIOD_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ExtendedRepetitionPeriod, &s1ap_WriteReplaceWarningRequestIEs->extendedRepetitionPeriod);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_NumberofBroadcastRequest, &s1ap_WriteReplaceWarningRequestIEs->numberofBroadcastRequest);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningRequestIEs->presenceMask & S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGTYPE_PRESENT)
        == S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGTYPE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_WarningType, &s1ap_WriteReplaceWarningRequestIEs->warningType);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningRequestIEs->presenceMask & S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGSECURITYINFO_PRESENT)
        == S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGSECURITYINFO_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_WarningSecurityInfo, &s1ap_WriteReplaceWarningRequestIEs->warningSecurityInfo);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningRequestIEs->presenceMask & S1AP_WRITEREPLACEWARNINGREQUESTIES_DATACODINGSCHEME_PRESENT)
        == S1AP_WRITEREPLACEWARNINGREQUESTIES_DATACODINGSCHEME_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_DataCodingScheme, &s1ap_WriteReplaceWarningRequestIEs->dataCodingScheme);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningRequestIEs->presenceMask & S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGMESSAGECONTENTS_PRESENT)
        == S1AP_WRITEREPLACEWARNINGREQUESTIES_WARNINGMESSAGECONTENTS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_WarningMessageContents, &s1ap_WriteReplaceWarningRequestIEs->warningMessageContents);
    /* Optional field */
    if ((s1ap_WriteReplaceWarningRequestIEs->presenceMask & S1AP_WRITEREPLACEWARNINGREQUESTIES_CONCURRENTWARNINGMESSAGEINDICATOR_PRESENT)
        == S1AP_WRITEREPLACEWARNINGREQUESTIES_CONCURRENTWARNINGMESSAGEINDICATOR_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ConcurrentWarningMessageIndicator, &s1ap_WriteReplaceWarningRequestIEs->concurrentWarningMessageIndicator);
    return 0;
}

int free_s1ap_uecapabilityinfoindication(
    S1ap_UECapabilityInfoIndicationIEs_t *s1ap_UECapabilityInfoIndicationIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UECapabilityInfoIndicationIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UECapabilityInfoIndicationIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UERadioCapability, &s1ap_UECapabilityInfoIndicationIEs->ueRadioCapability);
    return 0;
}

int free_s1ap_e_rabtobesetupitembearersureq(
    S1ap_E_RABToBeSetupItemBearerSUReqIEs_t *s1ap_E_RABToBeSetupItemBearerSUReqIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSetupItemBearerSUReq, &s1ap_E_RABToBeSetupItemBearerSUReqIEs->e_RABToBeSetupItemBearerSUReq);
    return 0;
}

int free_s1ap_e_rabdataforwardingitem(
    S1ap_E_RABDataForwardingItemIEs_t *s1ap_E_RABDataForwardingItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABDataForwardingItem, &s1ap_E_RABDataForwardingItemIEs->e_RABDataForwardingItem);
    return 0;
}

int free_s1ap_locationreportingfailureindication(
    S1ap_LocationReportingFailureIndicationIEs_t *s1ap_LocationReportingFailureIndicationIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_LocationReportingFailureIndicationIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_LocationReportingFailureIndicationIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_LocationReportingFailureIndicationIEs->cause);
    return 0;
}

int free_s1ap_initialcontextsetuprequest(
    S1ap_InitialContextSetupRequestIEs_t *s1ap_InitialContextSetupRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_InitialContextSetupRequestIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_InitialContextSetupRequestIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEAggregateMaximumBitrate, &s1ap_InitialContextSetupRequestIEs->uEaggregateMaximumBitrate);
    free_s1ap_e_rabtobesetuplistctxtsureq(&s1ap_InitialContextSetupRequestIEs->e_RABToBeSetupListCtxtSUReq);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UESecurityCapabilities, &s1ap_InitialContextSetupRequestIEs->ueSecurityCapabilities);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SecurityKey, &s1ap_InitialContextSetupRequestIEs->securityKey);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_TRACEACTIVATION_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_TRACEACTIVATION_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TraceActivation, &s1ap_InitialContextSetupRequestIEs->traceActivation);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_HandoverRestrictionList, &s1ap_InitialContextSetupRequestIEs->handoverRestrictionList);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UERadioCapability, &s1ap_InitialContextSetupRequestIEs->ueRadioCapability);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SubscriberProfileIDforRFP, &s1ap_InitialContextSetupRequestIEs->subscriberProfileIDforRFP);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_CSFALLBACKINDICATOR_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_CSFALLBACKINDICATOR_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSFallbackIndicator, &s1ap_InitialContextSetupRequestIEs->csFallbackIndicator);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SRVCCOperationPossible, &s1ap_InitialContextSetupRequestIEs->srvccOperationPossible);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSGMembershipStatus, &s1ap_InitialContextSetupRequestIEs->csgMembershipStatus);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_REGISTEREDLAI_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_REGISTEREDLAI_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_LAI, &s1ap_InitialContextSetupRequestIEs->registeredLAI);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_GUMMEI_ID_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_GUMMEI_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_GUMMEI, &s1ap_InitialContextSetupRequestIEs->gummei_id);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_MME_UE_S1AP_ID_2_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_MME_UE_S1AP_ID_2_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_InitialContextSetupRequestIEs->mme_ue_s1ap_id_2);
    /* Optional field */
    if ((s1ap_InitialContextSetupRequestIEs->presenceMask & S1AP_INITIALCONTEXTSETUPREQUESTIES_MANAGEMENTBASEDMDTALLOWED_PRESENT)
        == S1AP_INITIALCONTEXTSETUPREQUESTIES_MANAGEMENTBASEDMDTALLOWED_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ManagementBasedMDTAllowed, &s1ap_InitialContextSetupRequestIEs->managementBasedMDTAllowed);
    return 0;
}

int free_s1ap_initialuemessage(
    S1ap_InitialUEMessageIEs_t *s1ap_InitialUEMessageIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_InitialUEMessageIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_NAS_PDU, &s1ap_InitialUEMessageIEs->nas_pdu);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TAI, &s1ap_InitialUEMessageIEs->tai);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_EUTRAN_CGI, &s1ap_InitialUEMessageIEs->eutran_cgi);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RRC_Establishment_Cause, &s1ap_InitialUEMessageIEs->rrC_Establishment_Cause);
    /* Optional field */
    if ((s1ap_InitialUEMessageIEs->presenceMask & S1AP_INITIALUEMESSAGEIES_S_TMSI_PRESENT)
        == S1AP_INITIALUEMESSAGEIES_S_TMSI_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_S_TMSI, &s1ap_InitialUEMessageIEs->s_tmsi);
    /* Optional field */
    if ((s1ap_InitialUEMessageIEs->presenceMask & S1AP_INITIALUEMESSAGEIES_CSG_ID_PRESENT)
        == S1AP_INITIALUEMESSAGEIES_CSG_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_Id, &s1ap_InitialUEMessageIEs->csG_Id);
    /* Optional field */
    if ((s1ap_InitialUEMessageIEs->presenceMask & S1AP_INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT)
        == S1AP_INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_GUMMEI, &s1ap_InitialUEMessageIEs->gummei_id);
    /* Optional field */
    if ((s1ap_InitialUEMessageIEs->presenceMask & S1AP_INITIALUEMESSAGEIES_CELLACCESSMODE_PRESENT)
        == S1AP_INITIALUEMESSAGEIES_CELLACCESSMODE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CellAccessMode, &s1ap_InitialUEMessageIEs->cellAccessMode);
    /* Optional field */
    if ((s1ap_InitialUEMessageIEs->presenceMask & S1AP_INITIALUEMESSAGEIES_GW_TRANSPORTLAYERADDRESS_PRESENT)
        == S1AP_INITIALUEMESSAGEIES_GW_TRANSPORTLAYERADDRESS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TransportLayerAddress, &s1ap_InitialUEMessageIEs->gW_TransportLayerAddress);
    /* Optional field */
    if ((s1ap_InitialUEMessageIEs->presenceMask & S1AP_INITIALUEMESSAGEIES_RELAYNODE_INDICATOR_PRESENT)
        == S1AP_INITIALUEMESSAGEIES_RELAYNODE_INDICATOR_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RelayNode_Indicator, &s1ap_InitialUEMessageIEs->relayNode_Indicator);
    return 0;
}

int free_s1ap_locationreport(
    S1ap_LocationReportIEs_t *s1ap_LocationReportIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_LocationReportIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_LocationReportIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_EUTRAN_CGI, &s1ap_LocationReportIEs->eutran_cgi);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TAI, &s1ap_LocationReportIEs->tai);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RequestType, &s1ap_LocationReportIEs->requestType);
    return 0;
}

int free_s1ap_e_rabreleasecommand(
    S1ap_E_RABReleaseCommandIEs_t *s1ap_E_RABReleaseCommandIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_E_RABReleaseCommandIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_E_RABReleaseCommandIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_E_RABReleaseCommandIEs->presenceMask & S1AP_E_RABRELEASECOMMANDIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == S1AP_E_RABRELEASECOMMANDIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEAggregateMaximumBitrate, &s1ap_E_RABReleaseCommandIEs->uEaggregateMaximumBitrate);
    free_s1ap_e_rablist(&s1ap_E_RABReleaseCommandIEs->e_RABToBeReleasedList);
    /* Optional field */
    if ((s1ap_E_RABReleaseCommandIEs->presenceMask & S1AP_E_RABRELEASECOMMANDIES_NAS_PDU_PRESENT)
        == S1AP_E_RABRELEASECOMMANDIES_NAS_PDU_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_NAS_PDU, &s1ap_E_RABReleaseCommandIEs->nas_pdu);
    return 0;
}

int free_s1ap_uecontextmodificationresponse(
    S1ap_UEContextModificationResponseIEs_t *s1ap_UEContextModificationResponseIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UEContextModificationResponseIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UEContextModificationResponseIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_UEContextModificationResponseIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_UEContextModificationResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_uecontextmodificationrequest(
    S1ap_UEContextModificationRequestIEs_t *s1ap_UEContextModificationRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UEContextModificationRequestIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UEContextModificationRequestIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_UEContextModificationRequestIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONREQUESTIES_SECURITYKEY_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONREQUESTIES_SECURITYKEY_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SecurityKey, &s1ap_UEContextModificationRequestIEs->securityKey);
    /* Optional field */
    if ((s1ap_UEContextModificationRequestIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SubscriberProfileIDforRFP, &s1ap_UEContextModificationRequestIEs->subscriberProfileIDforRFP);
    /* Optional field */
    if ((s1ap_UEContextModificationRequestIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEAggregateMaximumBitrate, &s1ap_UEContextModificationRequestIEs->uEaggregateMaximumBitrate);
    /* Optional field */
    if ((s1ap_UEContextModificationRequestIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONREQUESTIES_CSFALLBACKINDICATOR_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONREQUESTIES_CSFALLBACKINDICATOR_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSFallbackIndicator, &s1ap_UEContextModificationRequestIEs->csFallbackIndicator);
    /* Optional field */
    if ((s1ap_UEContextModificationRequestIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONREQUESTIES_UESECURITYCAPABILITIES_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONREQUESTIES_UESECURITYCAPABILITIES_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UESecurityCapabilities, &s1ap_UEContextModificationRequestIEs->ueSecurityCapabilities);
    /* Optional field */
    if ((s1ap_UEContextModificationRequestIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSGMembershipStatus, &s1ap_UEContextModificationRequestIEs->csgMembershipStatus);
    /* Optional field */
    if ((s1ap_UEContextModificationRequestIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONREQUESTIES_REGISTEREDLAI_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONREQUESTIES_REGISTEREDLAI_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_LAI, &s1ap_UEContextModificationRequestIEs->registeredLAI);
    return 0;
}

int free_s1ap_locationreportingcontrol(
    S1ap_LocationReportingControlIEs_t *s1ap_LocationReportingControlIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_LocationReportingControlIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_LocationReportingControlIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RequestType, &s1ap_LocationReportingControlIEs->requestType);
    return 0;
}

int free_s1ap_uplinks1cdma2000tunneling(
    S1ap_UplinkS1cdma2000tunnelingIEs_t *s1ap_UplinkS1cdma2000tunnelingIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UplinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UplinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000RATType, &s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000RATType);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000SectorID, &s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000SectorID);
    /* Optional field */
    if ((s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask & S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000HOREQUIREDINDICATION_PRESENT)
        == S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000HOREQUIREDINDICATION_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000HORequiredIndication, &s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000HORequiredIndication);
    /* Optional field */
    if ((s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask & S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXSRVCCINFO_PRESENT)
        == S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXSRVCCINFO_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000OneXSRVCCInfo, &s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000OneXSRVCCInfo);
    /* Optional field */
    if ((s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask & S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXRAND_PRESENT)
        == S1AP_UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXRAND_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000OneXRAND, &s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000OneXRAND);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000PDU, &s1ap_UplinkS1cdma2000tunnelingIEs->cdma2000PDU);
    /* Optional field */
    if ((s1ap_UplinkS1cdma2000tunnelingIEs->presenceMask & S1AP_UPLINKS1CDMA2000TUNNELINGIES_EUTRANROUNDTRIPDELAYESTIMATIONINFO_PRESENT)
        == S1AP_UPLINKS1CDMA2000TUNNELINGIES_EUTRANROUNDTRIPDELAYESTIMATIONINFO_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_EUTRANRoundTripDelayEstimationInfo, &s1ap_UplinkS1cdma2000tunnelingIEs->eutranRoundTripDelayEstimationInfo);
    return 0;
}

int free_s1ap_handoverrequest(
    S1ap_HandoverRequestIEs_t *s1ap_HandoverRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverRequestIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_HandoverType, &s1ap_HandoverRequestIEs->handoverType);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_HandoverRequestIEs->cause);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEAggregateMaximumBitrate, &s1ap_HandoverRequestIEs->uEaggregateMaximumBitrate);
    free_s1ap_e_rabtobesetuplisthoreq(&s1ap_HandoverRequestIEs->e_RABToBeSetupListHOReq);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Source_ToTarget_TransparentContainer, &s1ap_HandoverRequestIEs->source_ToTarget_TransparentContainer);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UESecurityCapabilities, &s1ap_HandoverRequestIEs->ueSecurityCapabilities);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT)
        == S1AP_HANDOVERREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_HandoverRestrictionList, &s1ap_HandoverRequestIEs->handoverRestrictionList);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_TRACEACTIVATION_PRESENT)
        == S1AP_HANDOVERREQUESTIES_TRACEACTIVATION_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TraceActivation, &s1ap_HandoverRequestIEs->traceActivation);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_REQUESTTYPE_PRESENT)
        == S1AP_HANDOVERREQUESTIES_REQUESTTYPE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RequestType, &s1ap_HandoverRequestIEs->requestType);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT)
        == S1AP_HANDOVERREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SRVCCOperationPossible, &s1ap_HandoverRequestIEs->srvccOperationPossible);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SecurityContext, &s1ap_HandoverRequestIEs->securityContext);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_CSG_ID_PRESENT)
        == S1AP_HANDOVERREQUESTIES_CSG_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_Id, &s1ap_HandoverRequestIEs->csG_Id);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT)
        == S1AP_HANDOVERREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSGMembershipStatus, &s1ap_HandoverRequestIEs->csgMembershipStatus);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_GUMMEI_ID_PRESENT)
        == S1AP_HANDOVERREQUESTIES_GUMMEI_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_GUMMEI, &s1ap_HandoverRequestIEs->gummei_id);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_MME_UE_S1AP_ID_2_PRESENT)
        == S1AP_HANDOVERREQUESTIES_MME_UE_S1AP_ID_2_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverRequestIEs->mme_ue_s1ap_id_2);
    /* Optional field */
    if ((s1ap_HandoverRequestIEs->presenceMask & S1AP_HANDOVERREQUESTIES_MANAGEMENTBASEDMDTALLOWED_PRESENT)
        == S1AP_HANDOVERREQUESTIES_MANAGEMENTBASEDMDTALLOWED_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ManagementBasedMDTAllowed, &s1ap_HandoverRequestIEs->managementBasedMDTAllowed);
    return 0;
}

int free_s1ap_handoverrequired(
    S1ap_HandoverRequiredIEs_t *s1ap_HandoverRequiredIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverRequiredIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_HandoverRequiredIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_HandoverType, &s1ap_HandoverRequiredIEs->handoverType);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_HandoverRequiredIEs->cause);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TargetID, &s1ap_HandoverRequiredIEs->targetID);
    /* Optional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_DIRECT_FORWARDING_PATH_AVAILABILITY_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_DIRECT_FORWARDING_PATH_AVAILABILITY_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Direct_Forwarding_Path_Availability, &s1ap_HandoverRequiredIEs->direct_Forwarding_Path_Availability);
    /* Optional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_SRVCCHOINDICATION_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_SRVCCHOINDICATION_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SRVCCHOIndication, &s1ap_HandoverRequiredIEs->srvcchoIndication);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Source_ToTarget_TransparentContainer, &s1ap_HandoverRequiredIEs->source_ToTarget_TransparentContainer);
    /* Optional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_SOURCE_TOTARGET_TRANSPARENTCONTAINER_SECONDARY_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_SOURCE_TOTARGET_TRANSPARENTCONTAINER_SECONDARY_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Source_ToTarget_TransparentContainer, &s1ap_HandoverRequiredIEs->source_ToTarget_TransparentContainer_Secondary);
    /* Conditional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_MSCLASSMARK2_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_MSCLASSMARK2_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MSClassmark2, &s1ap_HandoverRequiredIEs->msClassmark2);
    /* Conditional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_MSCLASSMARK3_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_MSCLASSMARK3_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MSClassmark3, &s1ap_HandoverRequiredIEs->msClassmark3);
    /* Optional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_CSG_ID_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_CSG_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_Id, &s1ap_HandoverRequiredIEs->csG_Id);
    /* Optional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_CELLACCESSMODE_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_CELLACCESSMODE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CellAccessMode, &s1ap_HandoverRequiredIEs->cellAccessMode);
    /* Optional field */
    if ((s1ap_HandoverRequiredIEs->presenceMask & S1AP_HANDOVERREQUIREDIES_PS_SERVICENOTAVAILABLE_PRESENT)
        == S1AP_HANDOVERREQUIREDIES_PS_SERVICENOTAVAILABLE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_PS_ServiceNotAvailable, &s1ap_HandoverRequiredIEs->pS_ServiceNotAvailable);
    return 0;
}

int free_s1ap_e_rabfailedtosetupitemhoreqack(
    S1ap_E_RABFailedToSetupItemHOReqAckIEs_t *s1ap_E_RABFailedToSetupItemHOReqAckIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABFailedToSetupItemHOReqAck, &s1ap_E_RABFailedToSetupItemHOReqAckIEs->e_RABFailedToSetupItemHOReqAck);
    return 0;
}

int free_s1ap_e_rabtobeswitcheddlitem(
    S1ap_E_RABToBeSwitchedDLItemIEs_t *s1ap_E_RABToBeSwitchedDLItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSwitchedDLItem, &s1ap_E_RABToBeSwitchedDLItemIEs->e_RABToBeSwitchedDLItem);
    return 0;
}

int free_s1ap_mmeconfigurationupdateacknowledge(
    S1ap_MMEConfigurationUpdateAcknowledgeIEs_t *s1ap_MMEConfigurationUpdateAcknowledgeIEs) {

    /* Optional field */
    if ((s1ap_MMEConfigurationUpdateAcknowledgeIEs->presenceMask & S1AP_MMECONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_MMECONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_MMEConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_killresponse(
    S1ap_KillResponseIEs_t *s1ap_KillResponseIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MessageIdentifier, &s1ap_KillResponseIEs->messageIdentifier);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SerialNumber, &s1ap_KillResponseIEs->serialNumber);
    /* Optional field */
    if ((s1ap_KillResponseIEs->presenceMask & S1AP_KILLRESPONSEIES_BROADCASTCANCELLEDAREALIST_PRESENT)
        == S1AP_KILLRESPONSEIES_BROADCASTCANCELLEDAREALIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_BroadcastCancelledAreaList, &s1ap_KillResponseIEs->broadcastCancelledAreaList);
    /* Optional field */
    if ((s1ap_KillResponseIEs->presenceMask & S1AP_KILLRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_KILLRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_KillResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_mmestatustransfer(
    S1ap_MMEStatusTransferIEs_t *s1ap_MMEStatusTransferIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_MMEStatusTransferIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_MMEStatusTransferIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, &s1ap_MMEStatusTransferIEs->eNB_StatusTransfer_TransparentContainer);
    return 0;
}

int free_s1ap_mmeconfigurationupdate(
    S1ap_MMEConfigurationUpdateIEs_t *s1ap_MMEConfigurationUpdateIEs) {

    /* Optional field */
    if ((s1ap_MMEConfigurationUpdateIEs->presenceMask & S1AP_MMECONFIGURATIONUPDATEIES_MMENAME_PRESENT)
        == S1AP_MMECONFIGURATIONUPDATEIES_MMENAME_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MMEname, &s1ap_MMEConfigurationUpdateIEs->mmEname);
    /* Optional field */
    if ((s1ap_MMEConfigurationUpdateIEs->presenceMask & S1AP_MMECONFIGURATIONUPDATEIES_SERVEDGUMMEIS_PRESENT)
        == S1AP_MMECONFIGURATIONUPDATEIES_SERVEDGUMMEIS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ServedGUMMEIs, &s1ap_MMEConfigurationUpdateIEs->servedGUMMEIs);
    /* Optional field */
    if ((s1ap_MMEConfigurationUpdateIEs->presenceMask & S1AP_MMECONFIGURATIONUPDATEIES_RELATIVEMMECAPACITY_PRESENT)
        == S1AP_MMECONFIGURATIONUPDATEIES_RELATIVEMMECAPACITY_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RelativeMMECapacity, &s1ap_MMEConfigurationUpdateIEs->relativeMMECapacity);
    return 0;
}

int free_s1ap_e_rabmodifyrequest(
    S1ap_E_RABModifyRequestIEs_t *s1ap_E_RABModifyRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_E_RABModifyRequestIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_E_RABModifyRequestIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_E_RABModifyRequestIEs->presenceMask & S1AP_E_RABMODIFYREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == S1AP_E_RABMODIFYREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEAggregateMaximumBitrate, &s1ap_E_RABModifyRequestIEs->uEaggregateMaximumBitrate);
    free_s1ap_e_rabtobemodifiedlistbearermodreq(&s1ap_E_RABModifyRequestIEs->e_RABToBeModifiedListBearerModReq);
    return 0;
}

int free_s1ap_e_rabsetupitemctxtsures(
    S1ap_E_RABSetupItemCtxtSUResIEs_t *s1ap_E_RABSetupItemCtxtSUResIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABSetupItemCtxtSURes, &s1ap_E_RABSetupItemCtxtSUResIEs->e_RABSetupItemCtxtSURes);
    return 0;
}

int free_s1ap_e_rabtobeswitchedulitem(
    S1ap_E_RABToBeSwitchedULItemIEs_t *s1ap_E_RABToBeSwitchedULItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSwitchedULItem, &s1ap_E_RABToBeSwitchedULItemIEs->e_RABToBeSwitchedULItem);
    return 0;
}

int free_s1ap_downlinknastransport(
    S1ap_DownlinkNASTransportIEs_t *s1ap_DownlinkNASTransportIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_DownlinkNASTransportIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_DownlinkNASTransportIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_NAS_PDU, &s1ap_DownlinkNASTransportIEs->nas_pdu);
    /* Optional field */
    if ((s1ap_DownlinkNASTransportIEs->presenceMask & S1AP_DOWNLINKNASTRANSPORTIES_HANDOVERRESTRICTIONLIST_PRESENT)
        == S1AP_DOWNLINKNASTRANSPORTIES_HANDOVERRESTRICTIONLIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_HandoverRestrictionList, &s1ap_DownlinkNASTransportIEs->handoverRestrictionList);
    /* Optional field */
    if ((s1ap_DownlinkNASTransportIEs->presenceMask & S1AP_DOWNLINKNASTRANSPORTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT)
        == S1AP_DOWNLINKNASTRANSPORTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SubscriberProfileIDforRFP, &s1ap_DownlinkNASTransportIEs->subscriberProfileIDforRFP);
    return 0;
}

int free_s1ap_enbconfigurationupdatefailure(
    S1ap_ENBConfigurationUpdateFailureIEs_t *s1ap_ENBConfigurationUpdateFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_ENBConfigurationUpdateFailureIEs->cause);
    /* Optional field */
    if ((s1ap_ENBConfigurationUpdateFailureIEs->presenceMask & S1AP_ENBCONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT)
        == S1AP_ENBCONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TimeToWait, &s1ap_ENBConfigurationUpdateFailureIEs->timeToWait);
    /* Optional field */
    if ((s1ap_ENBConfigurationUpdateFailureIEs->presenceMask & S1AP_ENBCONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_ENBCONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_ENBConfigurationUpdateFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_uecontextreleasecomplete(
    S1ap_UEContextReleaseCompleteIEs_t *s1ap_UEContextReleaseCompleteIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UEContextReleaseCompleteIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UEContextReleaseCompleteIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_UEContextReleaseCompleteIEs->presenceMask & S1AP_UECONTEXTRELEASECOMPLETEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_UECONTEXTRELEASECOMPLETEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_UEContextReleaseCompleteIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_reset(
    S1ap_ResetIEs_t *s1ap_ResetIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_ResetIEs->cause);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ResetType, &s1ap_ResetIEs->resetType);
    return 0;
}

int free_s1ap_tracestart(
    S1ap_TraceStartIEs_t *s1ap_TraceStartIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_TraceStartIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_TraceStartIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TraceActivation, &s1ap_TraceStartIEs->traceActivation);
    return 0;
}

int free_s1ap_e_rabitem(
    S1ap_E_RABItemIEs_t *s1ap_E_RABItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABItem, &s1ap_E_RABItemIEs->e_RABItem);
    return 0;
}

int free_s1ap_s1setupresponse(
    S1ap_S1SetupResponseIEs_t *s1ap_S1SetupResponseIEs) {

    /* Optional field */
    if ((s1ap_S1SetupResponseIEs->presenceMask & S1AP_S1SETUPRESPONSEIES_MMENAME_PRESENT)
        == S1AP_S1SETUPRESPONSEIES_MMENAME_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MMEname, &s1ap_S1SetupResponseIEs->mmEname);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ServedGUMMEIs, &s1ap_S1SetupResponseIEs->servedGUMMEIs);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_RelativeMMECapacity, &s1ap_S1SetupResponseIEs->relativeMMECapacity);
    /* Optional field */
    if ((s1ap_S1SetupResponseIEs->presenceMask & S1AP_S1SETUPRESPONSEIES_MMERELAYSUPPORTINDICATOR_PRESENT)
        == S1AP_S1SETUPRESPONSEIES_MMERELAYSUPPORTINDICATOR_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MMERelaySupportIndicator, &s1ap_S1SetupResponseIEs->mmeRelaySupportIndicator);
    /* Optional field */
    if ((s1ap_S1SetupResponseIEs->presenceMask & S1AP_S1SETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_S1SETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_S1SetupResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_paging(
    S1ap_PagingIEs_t *s1ap_PagingIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEIdentityIndexValue, &s1ap_PagingIEs->ueIdentityIndexValue);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEPagingID, &s1ap_PagingIEs->uePagingID);
    /* Optional field */
    if ((s1ap_PagingIEs->presenceMask & S1AP_PAGINGIES_PAGINGDRX_PRESENT)
        == S1AP_PAGINGIES_PAGINGDRX_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_PagingDRX, &s1ap_PagingIEs->pagingDRX);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CNDomain, &s1ap_PagingIEs->cnDomain);
    free_s1ap_tailist(&s1ap_PagingIEs->taiList);
    /* Optional field */
    if ((s1ap_PagingIEs->presenceMask & S1AP_PAGINGIES_CSG_IDLIST_PRESENT)
        == S1AP_PAGINGIES_CSG_IDLIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_IdList, &s1ap_PagingIEs->csG_IdList);
    /* Optional field */
    if ((s1ap_PagingIEs->presenceMask & S1AP_PAGINGIES_PAGINGPRIORITY_PRESENT)
        == S1AP_PAGINGIES_PAGINGPRIORITY_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_PagingPriority, &s1ap_PagingIEs->pagingPriority);
    return 0;
}

int free_s1ap_enbconfigurationupdateacknowledge(
    S1ap_ENBConfigurationUpdateAcknowledgeIEs_t *s1ap_ENBConfigurationUpdateAcknowledgeIEs) {

    /* Optional field */
    if ((s1ap_ENBConfigurationUpdateAcknowledgeIEs->presenceMask & S1AP_ENBCONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_ENBCONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_ENBConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_pathswitchrequestfailure(
    S1ap_PathSwitchRequestFailureIEs_t *s1ap_PathSwitchRequestFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_PathSwitchRequestFailureIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_PathSwitchRequestFailureIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_PathSwitchRequestFailureIEs->cause);
    /* Optional field */
    if ((s1ap_PathSwitchRequestFailureIEs->presenceMask & S1AP_PATHSWITCHREQUESTFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_PATHSWITCHREQUESTFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_PathSwitchRequestFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_e_rabmodifyresponse(
    S1ap_E_RABModifyResponseIEs_t *s1ap_E_RABModifyResponseIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_E_RABModifyResponseIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_E_RABModifyResponseIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_E_RABModifyResponseIEs->presenceMask & S1AP_E_RABMODIFYRESPONSEIES_E_RABMODIFYLISTBEARERMODRES_PRESENT)
        == S1AP_E_RABMODIFYRESPONSEIES_E_RABMODIFYLISTBEARERMODRES_PRESENT) 
        free_s1ap_e_rabmodifylistbearermodres(&s1ap_E_RABModifyResponseIEs->e_RABModifyListBearerModRes);
    /* Optional field */
    if ((s1ap_E_RABModifyResponseIEs->presenceMask & S1AP_E_RABMODIFYRESPONSEIES_E_RABFAILEDTOMODIFYLIST_PRESENT)
        == S1AP_E_RABMODIFYRESPONSEIES_E_RABFAILEDTOMODIFYLIST_PRESENT) 
        free_s1ap_e_rablist(&s1ap_E_RABModifyResponseIEs->e_RABFailedToModifyList);
    /* Optional field */
    if ((s1ap_E_RABModifyResponseIEs->presenceMask & S1AP_E_RABMODIFYRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_E_RABMODIFYRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_E_RABModifyResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_handovernotify(
    S1ap_HandoverNotifyIEs_t *s1ap_HandoverNotifyIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverNotifyIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_HandoverNotifyIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_EUTRAN_CGI, &s1ap_HandoverNotifyIEs->eutran_cgi);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TAI, &s1ap_HandoverNotifyIEs->tai);
    return 0;
}

int free_s1ap_handovercancelacknowledge(
    S1ap_HandoverCancelAcknowledgeIEs_t *s1ap_HandoverCancelAcknowledgeIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverCancelAcknowledgeIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_HandoverCancelAcknowledgeIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_HandoverCancelAcknowledgeIEs->presenceMask & S1AP_HANDOVERCANCELACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_HANDOVERCANCELACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_HandoverCancelAcknowledgeIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_initialcontextsetupfailure(
    S1ap_InitialContextSetupFailureIEs_t *s1ap_InitialContextSetupFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_InitialContextSetupFailureIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_InitialContextSetupFailureIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_InitialContextSetupFailureIEs->cause);
    /* Optional field */
    if ((s1ap_InitialContextSetupFailureIEs->presenceMask & S1AP_INITIALCONTEXTSETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_INITIALCONTEXTSETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_InitialContextSetupFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_handovercommand(
    S1ap_HandoverCommandIEs_t *s1ap_HandoverCommandIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverCommandIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_HandoverCommandIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_HandoverType, &s1ap_HandoverCommandIEs->handoverType);
    /* Optional field */
    if ((s1ap_HandoverCommandIEs->presenceMask & S1AP_HANDOVERCOMMANDIES_E_RABSUBJECTTODATAFORWARDINGLIST_PRESENT)
        == S1AP_HANDOVERCOMMANDIES_E_RABSUBJECTTODATAFORWARDINGLIST_PRESENT) 
        free_s1ap_e_rabdataforwardinglist(&s1ap_HandoverCommandIEs->e_RABSubjecttoDataForwardingList);
    /* Optional field */
    if ((s1ap_HandoverCommandIEs->presenceMask & S1AP_HANDOVERCOMMANDIES_E_RABTORELEASELISTHOCMD_PRESENT)
        == S1AP_HANDOVERCOMMANDIES_E_RABTORELEASELISTHOCMD_PRESENT) 
        free_s1ap_e_rablist(&s1ap_HandoverCommandIEs->e_RABtoReleaseListHOCmd);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Target_ToSource_TransparentContainer, &s1ap_HandoverCommandIEs->target_ToSource_TransparentContainer);
    /* Optional field */
    if ((s1ap_HandoverCommandIEs->presenceMask & S1AP_HANDOVERCOMMANDIES_TARGET_TOSOURCE_TRANSPARENTCONTAINER_SECONDARY_PRESENT)
        == S1AP_HANDOVERCOMMANDIES_TARGET_TOSOURCE_TRANSPARENTCONTAINER_SECONDARY_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Target_ToSource_TransparentContainer, &s1ap_HandoverCommandIEs->target_ToSource_TransparentContainer_Secondary);
    /* Optional field */
    if ((s1ap_HandoverCommandIEs->presenceMask & S1AP_HANDOVERCOMMANDIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_HANDOVERCOMMANDIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_HandoverCommandIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_downlinknonueassociatedlppatransport_(
    S1ap_DownlinkNonUEAssociatedLPPaTransport_IEs_t *s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Routing_ID, &s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs->routing_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_LPPa_PDU, &s1ap_DownlinkNonUEAssociatedLPPaTransport_IEs->lpPa_PDU);
    return 0;
}

int free_s1ap_errorindication(
    S1ap_ErrorIndicationIEs_t *s1ap_ErrorIndicationIEs) {

    /* Optional field */
    if ((s1ap_ErrorIndicationIEs->presenceMask & S1AP_ERRORINDICATIONIES_MME_UE_S1AP_ID_PRESENT)
        == S1AP_ERRORINDICATIONIES_MME_UE_S1AP_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_ErrorIndicationIEs->mme_ue_s1ap_id);
    /* Optional field */
    if ((s1ap_ErrorIndicationIEs->presenceMask & S1AP_ERRORINDICATIONIES_ENB_UE_S1AP_ID_PRESENT)
        == S1AP_ERRORINDICATIONIES_ENB_UE_S1AP_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_ErrorIndicationIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_ErrorIndicationIEs->presenceMask & S1AP_ERRORINDICATIONIES_CAUSE_PRESENT)
        == S1AP_ERRORINDICATIONIES_CAUSE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_ErrorIndicationIEs->cause);
    /* Optional field */
    if ((s1ap_ErrorIndicationIEs->presenceMask & S1AP_ERRORINDICATIONIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_ERRORINDICATIONIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_ErrorIndicationIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_taiitem(
    S1ap_TAIItemIEs_t *s1ap_TAIItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TAIItem, &s1ap_TAIItemIEs->taiItem);
    return 0;
}

int free_s1ap_overloadstart(
    S1ap_OverloadStartIEs_t *s1ap_OverloadStartIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_OverloadResponse, &s1ap_OverloadStartIEs->overloadResponse);
    /* Optional field */
    if ((s1ap_OverloadStartIEs->presenceMask & S1AP_OVERLOADSTARTIES_GUMMEILIST_PRESENT)
        == S1AP_OVERLOADSTARTIES_GUMMEILIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_GUMMEIList, &s1ap_OverloadStartIEs->gummeiList);
    /* Optional field */
    if ((s1ap_OverloadStartIEs->presenceMask & S1AP_OVERLOADSTARTIES_TRAFFICLOADREDUCTIONINDICATION_PRESENT)
        == S1AP_OVERLOADSTARTIES_TRAFFICLOADREDUCTIONINDICATION_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TrafficLoadReductionIndication, &s1ap_OverloadStartIEs->trafficLoadReductionIndication);
    return 0;
}

int free_s1ap_mmedirectinformationtransfer(
    S1ap_MMEDirectInformationTransferIEs_t *s1ap_MMEDirectInformationTransferIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Inter_SystemInformationTransferType, &s1ap_MMEDirectInformationTransferIEs->inter_SystemInformationTransferTypeMDT);
    return 0;
}

int free_s1ap_handoverrequestacknowledge(
    S1ap_HandoverRequestAcknowledgeIEs_t *s1ap_HandoverRequestAcknowledgeIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverRequestAcknowledgeIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_HandoverRequestAcknowledgeIEs->eNB_UE_S1AP_ID);
    free_s1ap_e_rabadmittedlist(&s1ap_HandoverRequestAcknowledgeIEs->e_RABAdmittedList);
    /* Optional field */
    if ((s1ap_HandoverRequestAcknowledgeIEs->presenceMask & S1AP_HANDOVERREQUESTACKNOWLEDGEIES_E_RABFAILEDTOSETUPLISTHOREQACK_PRESENT)
        == S1AP_HANDOVERREQUESTACKNOWLEDGEIES_E_RABFAILEDTOSETUPLISTHOREQACK_PRESENT) 
        free_s1ap_e_rabfailedtosetuplisthoreqack(&s1ap_HandoverRequestAcknowledgeIEs->e_RABFailedToSetupListHOReqAck);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Target_ToSource_TransparentContainer, &s1ap_HandoverRequestAcknowledgeIEs->target_ToSource_TransparentContainer);
    /* Optional field */
    if ((s1ap_HandoverRequestAcknowledgeIEs->presenceMask & S1AP_HANDOVERREQUESTACKNOWLEDGEIES_CSG_ID_PRESENT)
        == S1AP_HANDOVERREQUESTACKNOWLEDGEIES_CSG_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_Id, &s1ap_HandoverRequestAcknowledgeIEs->csG_Id);
    /* Optional field */
    if ((s1ap_HandoverRequestAcknowledgeIEs->presenceMask & S1AP_HANDOVERREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_HANDOVERREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_HandoverRequestAcknowledgeIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_downlinkueassociatedlppatransport_(
    S1ap_DownlinkUEAssociatedLPPaTransport_IEs_t *s1ap_DownlinkUEAssociatedLPPaTransport_IEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_DownlinkUEAssociatedLPPaTransport_IEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_DownlinkUEAssociatedLPPaTransport_IEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Routing_ID, &s1ap_DownlinkUEAssociatedLPPaTransport_IEs->routing_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_LPPa_PDU, &s1ap_DownlinkUEAssociatedLPPaTransport_IEs->lpPa_PDU);
    return 0;
}

int free_s1ap_uecontextmodificationfailure(
    S1ap_UEContextModificationFailureIEs_t *s1ap_UEContextModificationFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UEContextModificationFailureIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UEContextModificationFailureIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_UEContextModificationFailureIEs->cause);
    /* Optional field */
    if ((s1ap_UEContextModificationFailureIEs->presenceMask & S1AP_UECONTEXTMODIFICATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_UECONTEXTMODIFICATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_UEContextModificationFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_uecontextreleaserequest(
    S1ap_UEContextReleaseRequestIEs_t *s1ap_UEContextReleaseRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UEContextReleaseRequestIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UEContextReleaseRequestIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_UEContextReleaseRequestIEs->cause);
    /* Optional field */
    if ((s1ap_UEContextReleaseRequestIEs->presenceMask & S1AP_UECONTEXTRELEASEREQUESTIES_GWCONTEXTRELEASEINDICATION_PRESENT)
        == S1AP_UECONTEXTRELEASEREQUESTIES_GWCONTEXTRELEASEINDICATION_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_GWContextReleaseIndication, &s1ap_UEContextReleaseRequestIEs->gwContextReleaseIndication);
    return 0;
}

int free_s1ap_mmeconfigurationtransfer(
    S1ap_MMEConfigurationTransferIEs_t *s1ap_MMEConfigurationTransferIEs) {

    /* Optional field */
    if ((s1ap_MMEConfigurationTransferIEs->presenceMask & S1AP_MMECONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFERMCT_PRESENT)
        == S1AP_MMECONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFERMCT_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SONConfigurationTransfer, &s1ap_MMEConfigurationTransferIEs->sonConfigurationTransferMCT);
    return 0;
}

int free_s1ap_pathswitchrequestacknowledge(
    S1ap_PathSwitchRequestAcknowledgeIEs_t *s1ap_PathSwitchRequestAcknowledgeIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_PathSwitchRequestAcknowledgeIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_PathSwitchRequestAcknowledgeIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask & S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEAggregateMaximumBitrate, &s1ap_PathSwitchRequestAcknowledgeIEs->uEaggregateMaximumBitrate);
    /* Optional field */
    if ((s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask & S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBESWITCHEDULLIST_PRESENT)
        == S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBESWITCHEDULLIST_PRESENT) 
        free_s1ap_e_rabtobeswitchedullist(&s1ap_PathSwitchRequestAcknowledgeIEs->e_RABToBeSwitchedULList);
    /* Optional field */
    if ((s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask & S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBERELEASEDLIST_PRESENT)
        == S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBERELEASEDLIST_PRESENT) 
        free_s1ap_e_rablist(&s1ap_PathSwitchRequestAcknowledgeIEs->e_RABToBeReleasedList);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SecurityContext, &s1ap_PathSwitchRequestAcknowledgeIEs->securityContext);
    /* Optional field */
    if ((s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask & S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_PathSwitchRequestAcknowledgeIEs->criticalityDiagnostics);
    /* Optional field */
    if ((s1ap_PathSwitchRequestAcknowledgeIEs->presenceMask & S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_MME_UE_S1AP_ID_2_PRESENT)
        == S1AP_PATHSWITCHREQUESTACKNOWLEDGEIES_MME_UE_S1AP_ID_2_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_PathSwitchRequestAcknowledgeIEs->mme_ue_s1ap_id_2);
    return 0;
}

int free_s1ap_ue_associatedlogicals1_connectionitemres(
    S1ap_UE_associatedLogicalS1_ConnectionItemRes_t *s1ap_UE_associatedLogicalS1_ConnectionItemRes) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, &s1ap_UE_associatedLogicalS1_ConnectionItemRes->uE_associatedLogicalS1_ConnectionItem);
    return 0;
}

int free_s1ap_s1setupfailure(
    S1ap_S1SetupFailureIEs_t *s1ap_S1SetupFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_S1SetupFailureIEs->cause);
    /* Optional field */
    if ((s1ap_S1SetupFailureIEs->presenceMask & S1AP_S1SETUPFAILUREIES_TIMETOWAIT_PRESENT)
        == S1AP_S1SETUPFAILUREIES_TIMETOWAIT_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TimeToWait, &s1ap_S1SetupFailureIEs->timeToWait);
    /* Optional field */
    if ((s1ap_S1SetupFailureIEs->presenceMask & S1AP_S1SETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_S1SETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_S1SetupFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_mmeconfigurationupdatefailure(
    S1ap_MMEConfigurationUpdateFailureIEs_t *s1ap_MMEConfigurationUpdateFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_MMEConfigurationUpdateFailureIEs->cause);
    /* Optional field */
    if ((s1ap_MMEConfigurationUpdateFailureIEs->presenceMask & S1AP_MMECONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT)
        == S1AP_MMECONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TimeToWait, &s1ap_MMEConfigurationUpdateFailureIEs->timeToWait);
    /* Optional field */
    if ((s1ap_MMEConfigurationUpdateFailureIEs->presenceMask & S1AP_MMECONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_MMECONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_MMEConfigurationUpdateFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_initialcontextsetupresponse(
    S1ap_InitialContextSetupResponseIEs_t *s1ap_InitialContextSetupResponseIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_InitialContextSetupResponseIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_InitialContextSetupResponseIEs->eNB_UE_S1AP_ID);
    free_s1ap_e_rabsetuplistctxtsures(&s1ap_InitialContextSetupResponseIEs->e_RABSetupListCtxtSURes);
    /* Optional field */
    if ((s1ap_InitialContextSetupResponseIEs->presenceMask & S1AP_INITIALCONTEXTSETUPRESPONSEIES_E_RABFAILEDTOSETUPLISTCTXTSURES_PRESENT)
        == S1AP_INITIALCONTEXTSETUPRESPONSEIES_E_RABFAILEDTOSETUPLISTCTXTSURES_PRESENT) 
        free_s1ap_e_rablist(&s1ap_InitialContextSetupResponseIEs->e_RABFailedToSetupListCtxtSURes);
    /* Optional field */
    if ((s1ap_InitialContextSetupResponseIEs->presenceMask & S1AP_INITIALCONTEXTSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_INITIALCONTEXTSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_InitialContextSetupResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_enbconfigurationupdate(
    S1ap_ENBConfigurationUpdateIEs_t *s1ap_ENBConfigurationUpdateIEs) {

    /* Optional field */
    if ((s1ap_ENBConfigurationUpdateIEs->presenceMask & S1AP_ENBCONFIGURATIONUPDATEIES_ENBNAME_PRESENT)
        == S1AP_ENBCONFIGURATIONUPDATEIES_ENBNAME_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENBname, &s1ap_ENBConfigurationUpdateIEs->eNBname);
    /* Optional field */
    if ((s1ap_ENBConfigurationUpdateIEs->presenceMask & S1AP_ENBCONFIGURATIONUPDATEIES_SUPPORTEDTAS_PRESENT)
        == S1AP_ENBCONFIGURATIONUPDATEIES_SUPPORTEDTAS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SupportedTAs, &s1ap_ENBConfigurationUpdateIEs->supportedTAs);
    /* Optional field */
    if ((s1ap_ENBConfigurationUpdateIEs->presenceMask & S1AP_ENBCONFIGURATIONUPDATEIES_CSG_IDLIST_PRESENT)
        == S1AP_ENBCONFIGURATIONUPDATEIES_CSG_IDLIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_IdList, &s1ap_ENBConfigurationUpdateIEs->csG_IdList);
    /* Optional field */
    if ((s1ap_ENBConfigurationUpdateIEs->presenceMask & S1AP_ENBCONFIGURATIONUPDATEIES_DEFAULTPAGINGDRX_PRESENT)
        == S1AP_ENBCONFIGURATIONUPDATEIES_DEFAULTPAGINGDRX_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_PagingDRX, &s1ap_ENBConfigurationUpdateIEs->defaultPagingDRX);
    return 0;
}

int free_s1ap_overloadstop(
    S1ap_OverloadStopIEs_t *s1ap_OverloadStopIEs) {

    /* Optional field */
    if ((s1ap_OverloadStopIEs->presenceMask & S1AP_OVERLOADSTOPIES_GUMMEILIST_PRESENT)
        == S1AP_OVERLOADSTOPIES_GUMMEILIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_GUMMEIList, &s1ap_OverloadStopIEs->gummeiList);
    return 0;
}

int free_s1ap_e_rabsetupitembearersures(
    S1ap_E_RABSetupItemBearerSUResIEs_t *s1ap_E_RABSetupItemBearerSUResIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABSetupItemBearerSURes, &s1ap_E_RABSetupItemBearerSUResIEs->e_RABSetupItemBearerSURes);
    return 0;
}

int free_s1ap_uplinknastransport(
    S1ap_UplinkNASTransportIEs_t *s1ap_UplinkNASTransportIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UplinkNASTransportIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UplinkNASTransportIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_NAS_PDU, &s1ap_UplinkNASTransportIEs->nas_pdu);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_EUTRAN_CGI, &s1ap_UplinkNASTransportIEs->eutran_cgi);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TAI, &s1ap_UplinkNASTransportIEs->tai);
    /* Optional field */
    if ((s1ap_UplinkNASTransportIEs->presenceMask & S1AP_UPLINKNASTRANSPORTIES_GW_TRANSPORTLAYERADDRESS_PRESENT)
        == S1AP_UPLINKNASTRANSPORTIES_GW_TRANSPORTLAYERADDRESS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TransportLayerAddress, &s1ap_UplinkNASTransportIEs->gW_TransportLayerAddress);
    return 0;
}

int free_s1ap_e_rabmodifyitembearermodres(
    S1ap_E_RABModifyItemBearerModResIEs_t *s1ap_E_RABModifyItemBearerModResIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABModifyItemBearerModRes, &s1ap_E_RABModifyItemBearerModResIEs->e_RABModifyItemBearerModRes);
    return 0;
}

int free_s1ap_e_rabtobesetupitemctxtsureq(
    S1ap_E_RABToBeSetupItemCtxtSUReqIEs_t *s1ap_E_RABToBeSetupItemCtxtSUReqIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABToBeSetupItemCtxtSUReq, &s1ap_E_RABToBeSetupItemCtxtSUReqIEs->e_RABToBeSetupItemCtxtSUReq);
    return 0;
}

int free_s1ap_tracefailureindication(
    S1ap_TraceFailureIndicationIEs_t *s1ap_TraceFailureIndicationIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_TraceFailureIndicationIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_TraceFailureIndicationIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_UTRAN_Trace_ID, &s1ap_TraceFailureIndicationIEs->e_UTRAN_Trace_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_TraceFailureIndicationIEs->cause);
    return 0;
}

int free_s1ap_uecontextreleasecommand(
    S1ap_UEContextReleaseCommandIEs_t *s1ap_UEContextReleaseCommandIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UE_S1AP_IDs, &s1ap_UEContextReleaseCommandIEs->uE_S1AP_IDs);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_UEContextReleaseCommandIEs->cause);
    return 0;
}

int free_s1ap_enbconfigurationtransfer(
    S1ap_ENBConfigurationTransferIEs_t *s1ap_ENBConfigurationTransferIEs) {

    /* Optional field */
    if ((s1ap_ENBConfigurationTransferIEs->presenceMask & S1AP_ENBCONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFERECT_PRESENT)
        == S1AP_ENBCONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFERECT_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SONConfigurationTransfer, &s1ap_ENBConfigurationTransferIEs->sonConfigurationTransferECT);
    return 0;
}

int free_s1ap_handoverfailure(
    S1ap_HandoverFailureIEs_t *s1ap_HandoverFailureIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_HandoverFailureIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_HandoverFailureIEs->cause);
    /* Optional field */
    if ((s1ap_HandoverFailureIEs->presenceMask & S1AP_HANDOVERFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_HANDOVERFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_HandoverFailureIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_ue_associatedlogicals1_connectionitemresack(
    S1ap_UE_associatedLogicalS1_ConnectionItemResAck_t *s1ap_UE_associatedLogicalS1_ConnectionItemResAck) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, &s1ap_UE_associatedLogicalS1_ConnectionItemResAck->uE_associatedLogicalS1_ConnectionItem);
    return 0;
}

int free_s1ap_e_rabsetupresponse(
    S1ap_E_RABSetupResponseIEs_t *s1ap_E_RABSetupResponseIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_E_RABSetupResponseIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_E_RABSetupResponseIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_E_RABSetupResponseIEs->presenceMask & S1AP_E_RABSETUPRESPONSEIES_E_RABSETUPLISTBEARERSURES_PRESENT)
        == S1AP_E_RABSETUPRESPONSEIES_E_RABSETUPLISTBEARERSURES_PRESENT) 
        free_s1ap_e_rabsetuplistbearersures(&s1ap_E_RABSetupResponseIEs->e_RABSetupListBearerSURes);
    /* Optional field */
    if ((s1ap_E_RABSetupResponseIEs->presenceMask & S1AP_E_RABSETUPRESPONSEIES_E_RABFAILEDTOSETUPLISTBEARERSURES_PRESENT)
        == S1AP_E_RABSETUPRESPONSEIES_E_RABFAILEDTOSETUPLISTBEARERSURES_PRESENT) 
        free_s1ap_e_rablist(&s1ap_E_RABSetupResponseIEs->e_RABFailedToSetupListBearerSURes);
    /* Optional field */
    if ((s1ap_E_RABSetupResponseIEs->presenceMask & S1AP_E_RABSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_E_RABSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_E_RABSetupResponseIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_pathswitchrequest(
    S1ap_PathSwitchRequestIEs_t *s1ap_PathSwitchRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_PathSwitchRequestIEs->eNB_UE_S1AP_ID);
    free_s1ap_e_rabtobeswitcheddllist(&s1ap_PathSwitchRequestIEs->e_RABToBeSwitchedDLList);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_PathSwitchRequestIEs->sourceMME_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_EUTRAN_CGI, &s1ap_PathSwitchRequestIEs->eutran_cgi);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TAI, &s1ap_PathSwitchRequestIEs->tai);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UESecurityCapabilities, &s1ap_PathSwitchRequestIEs->ueSecurityCapabilities);
    /* Optional field */
    if ((s1ap_PathSwitchRequestIEs->presenceMask & S1AP_PATHSWITCHREQUESTIES_CSG_ID_PRESENT)
        == S1AP_PATHSWITCHREQUESTIES_CSG_ID_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_Id, &s1ap_PathSwitchRequestIEs->csG_Id);
    /* Optional field */
    if ((s1ap_PathSwitchRequestIEs->presenceMask & S1AP_PATHSWITCHREQUESTIES_CELLACCESSMODE_PRESENT)
        == S1AP_PATHSWITCHREQUESTIES_CELLACCESSMODE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CellAccessMode, &s1ap_PathSwitchRequestIEs->cellAccessMode);
    /* Optional field */
    if ((s1ap_PathSwitchRequestIEs->presenceMask & S1AP_PATHSWITCHREQUESTIES_SOURCEMME_GUMMEI_PRESENT)
        == S1AP_PATHSWITCHREQUESTIES_SOURCEMME_GUMMEI_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_GUMMEI, &s1ap_PathSwitchRequestIEs->sourceMME_GUMMEI);
    return 0;
}

int free_s1ap_downlinks1cdma2000tunneling(
    S1ap_DownlinkS1cdma2000tunnelingIEs_t *s1ap_DownlinkS1cdma2000tunnelingIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_DownlinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_DownlinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_DownlinkS1cdma2000tunnelingIEs->presenceMask & S1AP_DOWNLINKS1CDMA2000TUNNELINGIES_E_RABSUBJECTTODATAFORWARDINGLIST_PRESENT)
        == S1AP_DOWNLINKS1CDMA2000TUNNELINGIES_E_RABSUBJECTTODATAFORWARDINGLIST_PRESENT) 
        free_s1ap_e_rabdataforwardinglist(&s1ap_DownlinkS1cdma2000tunnelingIEs->e_RABSubjecttoDataForwardingList);
    /* Optional field */
    if ((s1ap_DownlinkS1cdma2000tunnelingIEs->presenceMask & S1AP_DOWNLINKS1CDMA2000TUNNELINGIES_CDMA2000HOSTATUS_PRESENT)
        == S1AP_DOWNLINKS1CDMA2000TUNNELINGIES_CDMA2000HOSTATUS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000HOStatus, &s1ap_DownlinkS1cdma2000tunnelingIEs->cdma2000HOStatus);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000RATType, &s1ap_DownlinkS1cdma2000tunnelingIEs->cdma2000RATType);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cdma2000PDU, &s1ap_DownlinkS1cdma2000tunnelingIEs->cdma2000PDU);
    return 0;
}

int free_s1ap_e_rabreleaseindication(
    S1ap_E_RABReleaseIndicationIEs_t *s1ap_E_RABReleaseIndicationIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_E_RABReleaseIndicationIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_E_RABReleaseIndicationIEs->eNB_UE_S1AP_ID);
    free_s1ap_e_rablist(&s1ap_E_RABReleaseIndicationIEs->e_RABReleasedList);
    return 0;
}

int free_s1ap_s1setuprequest(
    S1ap_S1SetupRequestIEs_t *s1ap_S1SetupRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Global_ENB_ID, &s1ap_S1SetupRequestIEs->global_ENB_ID);
    /* Optional field */
    if ((s1ap_S1SetupRequestIEs->presenceMask & S1AP_S1SETUPREQUESTIES_ENBNAME_PRESENT)
        == S1AP_S1SETUPREQUESTIES_ENBNAME_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENBname, &s1ap_S1SetupRequestIEs->eNBname);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_SupportedTAs, &s1ap_S1SetupRequestIEs->supportedTAs);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_PagingDRX, &s1ap_S1SetupRequestIEs->defaultPagingDRX);
    /* Optional field */
    if ((s1ap_S1SetupRequestIEs->presenceMask & S1AP_S1SETUPREQUESTIES_CSG_IDLIST_PRESENT)
        == S1AP_S1SETUPREQUESTIES_CSG_IDLIST_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CSG_IdList, &s1ap_S1SetupRequestIEs->csG_IdList);
    return 0;
}

int free_s1ap_nasnondeliveryindication_(
    S1ap_NASNonDeliveryIndication_IEs_t *s1ap_NASNonDeliveryIndication_IEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_NASNonDeliveryIndication_IEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_NASNonDeliveryIndication_IEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_NAS_PDU, &s1ap_NASNonDeliveryIndication_IEs->nas_pdu);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Cause, &s1ap_NASNonDeliveryIndication_IEs->cause);
    return 0;
}

int free_s1ap_celltraffictrace(
    S1ap_CellTrafficTraceIEs_t *s1ap_CellTrafficTraceIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_CellTrafficTraceIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_CellTrafficTraceIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_UTRAN_Trace_ID, &s1ap_CellTrafficTraceIEs->e_UTRAN_Trace_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_EUTRAN_CGI, &s1ap_CellTrafficTraceIEs->eutran_cgi);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_TransportLayerAddress, &s1ap_CellTrafficTraceIEs->traceCollectionEntityIPAddress);
    /* Optional field */
    if ((s1ap_CellTrafficTraceIEs->presenceMask & S1AP_CELLTRAFFICTRACEIES_PRIVACYINDICATOR_PRESENT)
        == S1AP_CELLTRAFFICTRACEIES_PRIVACYINDICATOR_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_PrivacyIndicator, &s1ap_CellTrafficTraceIEs->privacyIndicator);
    return 0;
}

int free_s1ap_e_rabsetuprequest(
    S1ap_E_RABSetupRequestIEs_t *s1ap_E_RABSetupRequestIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_E_RABSetupRequestIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_E_RABSetupRequestIEs->eNB_UE_S1AP_ID);
    /* Optional field */
    if ((s1ap_E_RABSetupRequestIEs->presenceMask & S1AP_E_RABSETUPREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == S1AP_E_RABSETUPREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_UEAggregateMaximumBitrate, &s1ap_E_RABSetupRequestIEs->uEaggregateMaximumBitrate);
    free_s1ap_e_rabtobesetuplistbearersureq(&s1ap_E_RABSetupRequestIEs->e_RABToBeSetupListBearerSUReq);
    return 0;
}

int free_s1ap_enbstatustransfer(
    S1ap_ENBStatusTransferIEs_t *s1ap_ENBStatusTransferIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_ENBStatusTransferIEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_ENBStatusTransferIEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_StatusTransfer_TransparentContainer, &s1ap_ENBStatusTransferIEs->eNB_StatusTransfer_TransparentContainer);
    return 0;
}

int free_s1ap_resetacknowledge(
    S1ap_ResetAcknowledgeIEs_t *s1ap_ResetAcknowledgeIEs) {

    /* Optional field */
    if ((s1ap_ResetAcknowledgeIEs->presenceMask & S1AP_RESETACKNOWLEDGEIES_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_PRESENT)
        == S1AP_RESETACKNOWLEDGEIES_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_PRESENT) 
        free_s1ap_ue_associatedlogicals1_connectionlistresack(&s1ap_ResetAcknowledgeIEs->uE_associatedLogicalS1_ConnectionListResAck);
    /* Optional field */
    if ((s1ap_ResetAcknowledgeIEs->presenceMask & S1AP_RESETACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1AP_RESETACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) 
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_CriticalityDiagnostics, &s1ap_ResetAcknowledgeIEs->criticalityDiagnostics);
    return 0;
}

int free_s1ap_e_rabinformationitem(
    S1ap_E_RABInformationItemIEs_t *s1ap_E_RABInformationItemIEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_E_RABInformationListItem, &s1ap_E_RABInformationItemIEs->e_RABInformationListItem);
    return 0;
}

int free_s1ap_uplinkueassociatedlppatransport_(
    S1ap_UplinkUEAssociatedLPPaTransport_IEs_t *s1ap_UplinkUEAssociatedLPPaTransport_IEs) {

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_MME_UE_S1AP_ID, &s1ap_UplinkUEAssociatedLPPaTransport_IEs->mme_ue_s1ap_id);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ENB_UE_S1AP_ID, &s1ap_UplinkUEAssociatedLPPaTransport_IEs->eNB_UE_S1AP_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_Routing_ID, &s1ap_UplinkUEAssociatedLPPaTransport_IEs->routing_ID);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_LPPa_PDU, &s1ap_UplinkUEAssociatedLPPaTransport_IEs->lpPa_PDU);
    return 0;
}

int s1ap_decode_s1ap_e_rabreleaselistbearerrelcomp(
    S1ap_E_RABReleaseListBearerRelCompIEs_t *s1ap_E_RABReleaseListBearerRelCompIEs,
    S1ap_E_RABReleaseListBearerRelComp_t *s1ap_E_RABReleaseListBearerRelComp) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABReleaseListBearerRelComp != NULL);
    assert(s1ap_E_RABReleaseListBearerRelCompIEs != NULL);

    for (i = 0; i < s1ap_E_RABReleaseListBearerRelComp->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABReleaseListBearerRelComp->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABReleaseItemBearerRelComp:
            {
                S1ap_E_RABReleaseItemBearerRelComp_t *s1apERABReleaseItemBearerRelComp_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABReleaseItemBearerRelComp, (void**)&s1apERABReleaseItemBearerRelComp_p);
                if (tempDecoded < 0 || s1apERABReleaseItemBearerRelComp_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABReleaseItemBearerRelComp for message S1ap_E_RABReleaseListBearerRelComp failed\n");
                    if (s1apERABReleaseItemBearerRelComp_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABReleaseItemBearerRelComp, s1apERABReleaseItemBearerRelComp_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABReleaseItemBearerRelComp, s1apERABReleaseItemBearerRelComp_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABReleaseListBearerRelCompIEs->s1ap_E_RABReleaseItemBearerRelComp, s1apERABReleaseItemBearerRelComp_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabtobesetuplisthoreq(
    S1ap_E_RABToBeSetupListHOReqIEs_t *s1ap_E_RABToBeSetupListHOReqIEs,
    S1ap_E_RABToBeSetupListHOReq_t *s1ap_E_RABToBeSetupListHOReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABToBeSetupListHOReq != NULL);
    assert(s1ap_E_RABToBeSetupListHOReqIEs != NULL);

    for (i = 0; i < s1ap_E_RABToBeSetupListHOReq->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABToBeSetupListHOReq->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABToBeSetupItemHOReq:
            {
                S1ap_E_RABToBeSetupItemHOReq_t *s1apERABToBeSetupItemHOReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSetupItemHOReq, (void**)&s1apERABToBeSetupItemHOReq_p);
                if (tempDecoded < 0 || s1apERABToBeSetupItemHOReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSetupItemHOReq for message S1ap_E_RABToBeSetupListHOReq failed\n");
                    if (s1apERABToBeSetupItemHOReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupItemHOReq, s1apERABToBeSetupItemHOReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSetupItemHOReq, s1apERABToBeSetupItemHOReq_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABToBeSetupListHOReqIEs->s1ap_E_RABToBeSetupItemHOReq, s1apERABToBeSetupItemHOReq_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_bearers_subjecttostatustransfer_list(
    S1ap_Bearers_SubjectToStatusTransfer_ListIEs_t *s1ap_Bearers_SubjectToStatusTransfer_ListIEs,
    S1ap_Bearers_SubjectToStatusTransfer_List_t *s1ap_Bearers_SubjectToStatusTransfer_List) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_Bearers_SubjectToStatusTransfer_List != NULL);
    assert(s1ap_Bearers_SubjectToStatusTransfer_ListIEs != NULL);

    for (i = 0; i < s1ap_Bearers_SubjectToStatusTransfer_List->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_Bearers_SubjectToStatusTransfer_List->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_Bearers_SubjectToStatusTransfer_Item:
            {
                S1ap_Bearers_SubjectToStatusTransfer_Item_t *s1apBearersSubjectToStatusTransferItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_Bearers_SubjectToStatusTransfer_Item, (void**)&s1apBearersSubjectToStatusTransferItem_p);
                if (tempDecoded < 0 || s1apBearersSubjectToStatusTransferItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE bearers_SubjectToStatusTransfer_Item for message S1ap_Bearers_SubjectToStatusTransfer_List failed\n");
                    if (s1apBearersSubjectToStatusTransferItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_Bearers_SubjectToStatusTransfer_Item, s1apBearersSubjectToStatusTransferItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_Bearers_SubjectToStatusTransfer_Item, s1apBearersSubjectToStatusTransferItem_p);
                ASN_SEQUENCE_ADD(&s1ap_Bearers_SubjectToStatusTransfer_ListIEs->s1ap_Bearers_SubjectToStatusTransfer_Item, s1apBearersSubjectToStatusTransferItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabtobemodifiedlistbearermodreq(
    S1ap_E_RABToBeModifiedListBearerModReqIEs_t *s1ap_E_RABToBeModifiedListBearerModReqIEs,
    S1ap_E_RABToBeModifiedListBearerModReq_t *s1ap_E_RABToBeModifiedListBearerModReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABToBeModifiedListBearerModReq != NULL);
    assert(s1ap_E_RABToBeModifiedListBearerModReqIEs != NULL);

    for (i = 0; i < s1ap_E_RABToBeModifiedListBearerModReq->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABToBeModifiedListBearerModReq->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABToBeModifiedItemBearerModReq:
            {
                S1ap_E_RABToBeModifiedItemBearerModReq_t *s1apERABToBeModifiedItemBearerModReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeModifiedItemBearerModReq, (void**)&s1apERABToBeModifiedItemBearerModReq_p);
                if (tempDecoded < 0 || s1apERABToBeModifiedItemBearerModReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeModifiedItemBearerModReq for message S1ap_E_RABToBeModifiedListBearerModReq failed\n");
                    if (s1apERABToBeModifiedItemBearerModReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeModifiedItemBearerModReq, s1apERABToBeModifiedItemBearerModReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeModifiedItemBearerModReq, s1apERABToBeModifiedItemBearerModReq_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABToBeModifiedListBearerModReqIEs->s1ap_E_RABToBeModifiedItemBearerModReq, s1apERABToBeModifiedItemBearerModReq_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabadmittedlist(
    S1ap_E_RABAdmittedListIEs_t *s1ap_E_RABAdmittedListIEs,
    S1ap_E_RABAdmittedList_t *s1ap_E_RABAdmittedList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABAdmittedList != NULL);
    assert(s1ap_E_RABAdmittedListIEs != NULL);

    for (i = 0; i < s1ap_E_RABAdmittedList->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABAdmittedList->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABAdmittedItem:
            {
                S1ap_E_RABAdmittedItem_t *s1apERABAdmittedItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABAdmittedItem, (void**)&s1apERABAdmittedItem_p);
                if (tempDecoded < 0 || s1apERABAdmittedItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABAdmittedItem for message S1ap_E_RABAdmittedList failed\n");
                    if (s1apERABAdmittedItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABAdmittedItem, s1apERABAdmittedItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABAdmittedItem, s1apERABAdmittedItem_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABAdmittedListIEs->s1ap_E_RABAdmittedItem, s1apERABAdmittedItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabtobesetuplistbearersureq(
    S1ap_E_RABToBeSetupListBearerSUReqIEs_t *s1ap_E_RABToBeSetupListBearerSUReqIEs,
    S1ap_E_RABToBeSetupListBearerSUReq_t *s1ap_E_RABToBeSetupListBearerSUReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABToBeSetupListBearerSUReq != NULL);
    assert(s1ap_E_RABToBeSetupListBearerSUReqIEs != NULL);

    for (i = 0; i < s1ap_E_RABToBeSetupListBearerSUReq->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABToBeSetupListBearerSUReq->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABToBeSetupItemBearerSUReq:
            {
                S1ap_E_RABToBeSetupItemBearerSUReq_t *s1apERABToBeSetupItemBearerSUReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSetupItemBearerSUReq, (void**)&s1apERABToBeSetupItemBearerSUReq_p);
                if (tempDecoded < 0 || s1apERABToBeSetupItemBearerSUReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSetupItemBearerSUReq for message S1ap_E_RABToBeSetupListBearerSUReq failed\n");
                    if (s1apERABToBeSetupItemBearerSUReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupItemBearerSUReq, s1apERABToBeSetupItemBearerSUReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSetupItemBearerSUReq, s1apERABToBeSetupItemBearerSUReq_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABToBeSetupListBearerSUReqIEs->s1ap_E_RABToBeSetupItemBearerSUReq, s1apERABToBeSetupItemBearerSUReq_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabdataforwardinglist(
    S1ap_E_RABDataForwardingListIEs_t *s1ap_E_RABDataForwardingListIEs,
    S1ap_E_RABDataForwardingList_t *s1ap_E_RABDataForwardingList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABDataForwardingList != NULL);
    assert(s1ap_E_RABDataForwardingListIEs != NULL);

    for (i = 0; i < s1ap_E_RABDataForwardingList->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABDataForwardingList->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABDataForwardingItem:
            {
                S1ap_E_RABDataForwardingItem_t *s1apERABDataForwardingItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABDataForwardingItem, (void**)&s1apERABDataForwardingItem_p);
                if (tempDecoded < 0 || s1apERABDataForwardingItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABDataForwardingItem for message S1ap_E_RABDataForwardingList failed\n");
                    if (s1apERABDataForwardingItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABDataForwardingItem, s1apERABDataForwardingItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABDataForwardingItem, s1apERABDataForwardingItem_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABDataForwardingListIEs->s1ap_E_RABDataForwardingItem, s1apERABDataForwardingItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabfailedtosetuplisthoreqack(
    S1ap_E_RABFailedToSetupListHOReqAckIEs_t *s1ap_E_RABFailedToSetupListHOReqAckIEs,
    S1ap_E_RABFailedToSetupListHOReqAck_t *s1ap_E_RABFailedToSetupListHOReqAck) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABFailedToSetupListHOReqAck != NULL);
    assert(s1ap_E_RABFailedToSetupListHOReqAckIEs != NULL);

    for (i = 0; i < s1ap_E_RABFailedToSetupListHOReqAck->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABFailedToSetupListHOReqAck->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABFailedToSetupItemHOReqAck:
            {
                S1ap_E_RABFailedToSetupItemHOReqAck_t *s1apERABFailedToSetupItemHOReqAck_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABFailedToSetupItemHOReqAck, (void**)&s1apERABFailedToSetupItemHOReqAck_p);
                if (tempDecoded < 0 || s1apERABFailedToSetupItemHOReqAck_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABFailedToSetupItemHOReqAck for message S1ap_E_RABFailedToSetupListHOReqAck failed\n");
                    if (s1apERABFailedToSetupItemHOReqAck_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABFailedToSetupItemHOReqAck, s1apERABFailedToSetupItemHOReqAck_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABFailedToSetupItemHOReqAck, s1apERABFailedToSetupItemHOReqAck_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABFailedToSetupListHOReqAckIEs->s1ap_E_RABFailedToSetupItemHOReqAck, s1apERABFailedToSetupItemHOReqAck_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabtobeswitcheddllist(
    S1ap_E_RABToBeSwitchedDLListIEs_t *s1ap_E_RABToBeSwitchedDLListIEs,
    S1ap_E_RABToBeSwitchedDLList_t *s1ap_E_RABToBeSwitchedDLList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABToBeSwitchedDLList != NULL);
    assert(s1ap_E_RABToBeSwitchedDLListIEs != NULL);

    for (i = 0; i < s1ap_E_RABToBeSwitchedDLList->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABToBeSwitchedDLList->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABToBeSwitchedDLItem:
            {
                S1ap_E_RABToBeSwitchedDLItem_t *s1apERABToBeSwitchedDLItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSwitchedDLItem, (void**)&s1apERABToBeSwitchedDLItem_p);
                if (tempDecoded < 0 || s1apERABToBeSwitchedDLItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSwitchedDLItem for message S1ap_E_RABToBeSwitchedDLList failed\n");
                    if (s1apERABToBeSwitchedDLItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSwitchedDLItem, s1apERABToBeSwitchedDLItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSwitchedDLItem, s1apERABToBeSwitchedDLItem_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABToBeSwitchedDLListIEs->s1ap_E_RABToBeSwitchedDLItem, s1apERABToBeSwitchedDLItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabsetuplistctxtsures(
    S1ap_E_RABSetupListCtxtSUResIEs_t *s1ap_E_RABSetupListCtxtSUResIEs,
    S1ap_E_RABSetupListCtxtSURes_t *s1ap_E_RABSetupListCtxtSURes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABSetupListCtxtSURes != NULL);
    assert(s1ap_E_RABSetupListCtxtSUResIEs != NULL);

    for (i = 0; i < s1ap_E_RABSetupListCtxtSURes->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABSetupListCtxtSURes->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABSetupItemCtxtSURes:
            {
                S1ap_E_RABSetupItemCtxtSURes_t *s1apERABSetupItemCtxtSURes_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABSetupItemCtxtSURes, (void**)&s1apERABSetupItemCtxtSURes_p);
                if (tempDecoded < 0 || s1apERABSetupItemCtxtSURes_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABSetupItemCtxtSURes for message S1ap_E_RABSetupListCtxtSURes failed\n");
                    if (s1apERABSetupItemCtxtSURes_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupItemCtxtSURes, s1apERABSetupItemCtxtSURes_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABSetupItemCtxtSURes, s1apERABSetupItemCtxtSURes_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABSetupListCtxtSUResIEs->s1ap_E_RABSetupItemCtxtSURes, s1apERABSetupItemCtxtSURes_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabtobeswitchedullist(
    S1ap_E_RABToBeSwitchedULListIEs_t *s1ap_E_RABToBeSwitchedULListIEs,
    S1ap_E_RABToBeSwitchedULList_t *s1ap_E_RABToBeSwitchedULList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABToBeSwitchedULList != NULL);
    assert(s1ap_E_RABToBeSwitchedULListIEs != NULL);

    for (i = 0; i < s1ap_E_RABToBeSwitchedULList->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABToBeSwitchedULList->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABToBeSwitchedULItem:
            {
                S1ap_E_RABToBeSwitchedULItem_t *s1apERABToBeSwitchedULItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSwitchedULItem, (void**)&s1apERABToBeSwitchedULItem_p);
                if (tempDecoded < 0 || s1apERABToBeSwitchedULItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSwitchedULItem for message S1ap_E_RABToBeSwitchedULList failed\n");
                    if (s1apERABToBeSwitchedULItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSwitchedULItem, s1apERABToBeSwitchedULItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSwitchedULItem, s1apERABToBeSwitchedULItem_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABToBeSwitchedULListIEs->s1ap_E_RABToBeSwitchedULItem, s1apERABToBeSwitchedULItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rablist(
    S1ap_E_RABListIEs_t *s1ap_E_RABListIEs,
    S1ap_E_RABList_t *s1ap_E_RABList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABList != NULL);
    assert(s1ap_E_RABListIEs != NULL);

    for (i = 0; i < s1ap_E_RABList->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABList->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABItem:
            {
                S1ap_E_RABItem_t *s1apERABItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABItem, (void**)&s1apERABItem_p);
                if (tempDecoded < 0 || s1apERABItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABItem for message S1ap_E_RABList failed\n");
                    if (s1apERABItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABItem, s1apERABItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABItem, s1apERABItem_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABListIEs->s1ap_E_RABItem, s1apERABItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_tailist(
    S1ap_TAIListIEs_t *s1ap_TAIListIEs,
    S1ap_TAIList_t *s1ap_TAIList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_TAIList != NULL);
    assert(s1ap_TAIListIEs != NULL);

    for (i = 0; i < s1ap_TAIList->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_TAIList->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_TAIItem:
            {
                S1ap_TAIItem_t *s1apTAIItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_TAIItem, (void**)&s1apTAIItem_p);
                if (tempDecoded < 0 || s1apTAIItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE taiItem for message S1ap_TAIList failed\n");
                    if (s1apTAIItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_TAIItem, s1apTAIItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_TAIItem, s1apTAIItem_p);
                ASN_SEQUENCE_ADD(&s1ap_TAIListIEs->s1ap_TAIItem, s1apTAIItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_ue_associatedlogicals1_connectionlistres(
    S1ap_UE_associatedLogicalS1_ConnectionListResIEs_t *s1ap_UE_associatedLogicalS1_ConnectionListResIEs,
    S1ap_UE_associatedLogicalS1_ConnectionListRes_t *s1ap_UE_associatedLogicalS1_ConnectionListRes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_UE_associatedLogicalS1_ConnectionListRes != NULL);
    assert(s1ap_UE_associatedLogicalS1_ConnectionListResIEs != NULL);

    for (i = 0; i < s1ap_UE_associatedLogicalS1_ConnectionListRes->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_UE_associatedLogicalS1_ConnectionListRes->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem:
            {
                S1ap_UE_associatedLogicalS1_ConnectionItem_t *s1apUEassociatedLogicalS1ConnectionItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, (void**)&s1apUEassociatedLogicalS1ConnectionItem_p);
                if (tempDecoded < 0 || s1apUEassociatedLogicalS1ConnectionItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uE_associatedLogicalS1_ConnectionItem for message S1ap_UE_associatedLogicalS1_ConnectionListRes failed\n");
                    if (s1apUEassociatedLogicalS1ConnectionItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, s1apUEassociatedLogicalS1ConnectionItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, s1apUEassociatedLogicalS1ConnectionItem_p);
                ASN_SEQUENCE_ADD(&s1ap_UE_associatedLogicalS1_ConnectionListResIEs->s1ap_UE_associatedLogicalS1_ConnectionItemRes, s1apUEassociatedLogicalS1ConnectionItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabsetuplistbearersures(
    S1ap_E_RABSetupListBearerSUResIEs_t *s1ap_E_RABSetupListBearerSUResIEs,
    S1ap_E_RABSetupListBearerSURes_t *s1ap_E_RABSetupListBearerSURes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABSetupListBearerSURes != NULL);
    assert(s1ap_E_RABSetupListBearerSUResIEs != NULL);

    for (i = 0; i < s1ap_E_RABSetupListBearerSURes->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABSetupListBearerSURes->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABSetupItemBearerSURes:
            {
                S1ap_E_RABSetupItemBearerSURes_t *s1apERABSetupItemBearerSURes_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABSetupItemBearerSURes, (void**)&s1apERABSetupItemBearerSURes_p);
                if (tempDecoded < 0 || s1apERABSetupItemBearerSURes_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABSetupItemBearerSURes for message S1ap_E_RABSetupListBearerSURes failed\n");
                    if (s1apERABSetupItemBearerSURes_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABSetupItemBearerSURes, s1apERABSetupItemBearerSURes_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABSetupItemBearerSURes, s1apERABSetupItemBearerSURes_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABSetupListBearerSUResIEs->s1ap_E_RABSetupItemBearerSURes, s1apERABSetupItemBearerSURes_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabmodifylistbearermodres(
    S1ap_E_RABModifyListBearerModResIEs_t *s1ap_E_RABModifyListBearerModResIEs,
    S1ap_E_RABModifyListBearerModRes_t *s1ap_E_RABModifyListBearerModRes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABModifyListBearerModRes != NULL);
    assert(s1ap_E_RABModifyListBearerModResIEs != NULL);

    for (i = 0; i < s1ap_E_RABModifyListBearerModRes->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABModifyListBearerModRes->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABModifyItemBearerModRes:
            {
                S1ap_E_RABModifyItemBearerModRes_t *s1apERABModifyItemBearerModRes_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABModifyItemBearerModRes, (void**)&s1apERABModifyItemBearerModRes_p);
                if (tempDecoded < 0 || s1apERABModifyItemBearerModRes_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABModifyItemBearerModRes for message S1ap_E_RABModifyListBearerModRes failed\n");
                    if (s1apERABModifyItemBearerModRes_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABModifyItemBearerModRes, s1apERABModifyItemBearerModRes_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABModifyItemBearerModRes, s1apERABModifyItemBearerModRes_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABModifyListBearerModResIEs->s1ap_E_RABModifyItemBearerModRes, s1apERABModifyItemBearerModRes_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabtobesetuplistctxtsureq(
    S1ap_E_RABToBeSetupListCtxtSUReqIEs_t *s1ap_E_RABToBeSetupListCtxtSUReqIEs,
    S1ap_E_RABToBeSetupListCtxtSUReq_t *s1ap_E_RABToBeSetupListCtxtSUReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABToBeSetupListCtxtSUReq != NULL);
    assert(s1ap_E_RABToBeSetupListCtxtSUReqIEs != NULL);

    for (i = 0; i < s1ap_E_RABToBeSetupListCtxtSUReq->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABToBeSetupListCtxtSUReq->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABToBeSetupItemCtxtSUReq:
            {
                S1ap_E_RABToBeSetupItemCtxtSUReq_t *s1apERABToBeSetupItemCtxtSUReq_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABToBeSetupItemCtxtSUReq, (void**)&s1apERABToBeSetupItemCtxtSUReq_p);
                if (tempDecoded < 0 || s1apERABToBeSetupItemCtxtSUReq_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABToBeSetupItemCtxtSUReq for message S1ap_E_RABToBeSetupListCtxtSUReq failed\n");
                    if (s1apERABToBeSetupItemCtxtSUReq_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABToBeSetupItemCtxtSUReq, s1apERABToBeSetupItemCtxtSUReq_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABToBeSetupItemCtxtSUReq, s1apERABToBeSetupItemCtxtSUReq_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABToBeSetupListCtxtSUReqIEs->s1ap_E_RABToBeSetupItemCtxtSUReq, s1apERABToBeSetupItemCtxtSUReq_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_ue_associatedlogicals1_connectionlistresack(
    S1ap_UE_associatedLogicalS1_ConnectionListResAckIEs_t *s1ap_UE_associatedLogicalS1_ConnectionListResAckIEs,
    S1ap_UE_associatedLogicalS1_ConnectionListResAck_t *s1ap_UE_associatedLogicalS1_ConnectionListResAck) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_UE_associatedLogicalS1_ConnectionListResAck != NULL);
    assert(s1ap_UE_associatedLogicalS1_ConnectionListResAckIEs != NULL);

    for (i = 0; i < s1ap_UE_associatedLogicalS1_ConnectionListResAck->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_UE_associatedLogicalS1_ConnectionListResAck->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem:
            {
                S1ap_UE_associatedLogicalS1_ConnectionItem_t *s1apUEassociatedLogicalS1ConnectionItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, (void**)&s1apUEassociatedLogicalS1ConnectionItem_p);
                if (tempDecoded < 0 || s1apUEassociatedLogicalS1ConnectionItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE uE_associatedLogicalS1_ConnectionItem for message S1ap_UE_associatedLogicalS1_ConnectionListResAck failed\n");
                    if (s1apUEassociatedLogicalS1ConnectionItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, s1apUEassociatedLogicalS1ConnectionItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_UE_associatedLogicalS1_ConnectionItem, s1apUEassociatedLogicalS1ConnectionItem_p);
                ASN_SEQUENCE_ADD(&s1ap_UE_associatedLogicalS1_ConnectionListResAckIEs->s1ap_UE_associatedLogicalS1_ConnectionItemResAck, s1apUEassociatedLogicalS1ConnectionItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1ap_e_rabinformationlist(
    S1ap_E_RABInformationListIEs_t *s1ap_E_RABInformationListIEs,
    S1ap_E_RABInformationList_t *s1ap_E_RABInformationList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    assert(s1ap_E_RABInformationList != NULL);
    assert(s1ap_E_RABInformationListIEs != NULL);

    for (i = 0; i < s1ap_E_RABInformationList->list.count; i++) {
        S1ap_IE_t *ie_p = s1ap_E_RABInformationList->list.array[i];
        switch (ie_p->id) {
            case S1ap_ProtocolIE_ID_id_E_RABInformationListItem:
            {
                S1ap_E_RABInformationListItem_t *s1apERABInformationListItem_p = NULL;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S1ap_E_RABInformationListItem, (void**)&s1apERABInformationListItem_p);
                if (tempDecoded < 0 || s1apERABInformationListItem_p == NULL) {
                   OAILOG_ERROR (LOG_S1AP, "Decoding of IE e_RABInformationListItem for message S1ap_E_RABInformationList failed\n");
                    if (s1apERABInformationListItem_p)
                        ASN_STRUCT_FREE(asn_DEF_S1ap_E_RABInformationListItem, s1apERABInformationListItem_p);
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S1ap_E_RABInformationListItem, s1apERABInformationListItem_p);
                ASN_SEQUENCE_ADD(&s1ap_E_RABInformationListIEs->s1ap_E_RABInformationItem, s1apERABInformationListItem_p);
            } break;
            default:
               OAILOG_ERROR (LOG_S1AP, "Unknown protocol IE id (%d) for message s1ap_uplinkueassociatedlppatransport_\n", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

