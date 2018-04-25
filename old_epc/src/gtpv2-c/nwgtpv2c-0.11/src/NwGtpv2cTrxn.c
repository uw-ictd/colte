/*----------------------------------------------------------------------------*
 *                                                                            *
                                n w - g t p v 2 c
      G P R S   T u n n e l i n g    P r o t o c o l   v 2 c    S t a c k
 *                                                                            *
 *                                                                            *
   Copyright (c) 2010-2011 Amit Chawre
   All rights reserved.
 *                                                                            *
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
 *                                                                            *
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. The name of the author may not be used to endorse or promote products
      derived from this software without specific prior written permission.
 *                                                                            *
   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  ----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "NwTypes.h"
#include "NwLog.h"
#include "NwUtils.h"
#include "NwGtpv2cLog.h"
#include "NwGtpv2c.h"
#include "NwGtpv2cPrivate.h"
#include "NwGtpv2cTrxn.h"

/*--------------------------------------------------------------------------*
                   P R I V A T E  D E C L A R A T I O N S
  --------------------------------------------------------------------------*/

#ifdef __cplusplus
extern                                  "C" {
#endif

  static NwGtpv2cTrxnT                   *gpGtpv2cTrxnPool = NULL;

/*--------------------------------------------------------------------------*
                     P R I V A T E      F U N C T I O N S
  --------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   Send msg retransmission to peer via data request to UDP Entity
  --------------------------------------------------------------------------*/

  static NwRcT                            nwGtpv2cTrxnSendMsgRetransmission (NwGtpv2cTrxnT * thiz) {
    NwRcT                                   rc;
    NW_ASSERT (thiz);
    NW_ASSERT (thiz->pMsg);
    rc = thiz->pStack->udp.udpDataReqCallback (thiz->pStack->udp.hUdp, thiz->pMsg->msgBuf, thiz->pMsg->msgLen, thiz->peerIp, thiz->peerPort);
    thiz->maxRetries--;
    return rc;
  }

  static NwRcT                            nwGtpv2cTrxnPeerRspWaitTimeout (void *arg) {
    NwRcT                                   rc = NW_OK;
    NwGtpv2cTrxnT                          *thiz;
    NwGtpv2cStackT                         *pStack;

    thiz = ((NwGtpv2cTrxnT *) arg);
    pStack = thiz->pStack;
    NW_ASSERT (pStack);
    OAILOG_WARNING (LOG_GTPV2C,  "T3 Response timer expired for transaction 0x%p\n", thiz);
    thiz->hRspTmr = 0;

    if (thiz->maxRetries) {
      rc = nwGtpv2cTrxnSendMsgRetransmission (thiz);
      NW_ASSERT (NW_OK == rc);
      rc = nwGtpv2cStartTimer (thiz->pStack, thiz->t3Timer, 0, NW_GTPV2C_TMR_TYPE_ONE_SHOT, nwGtpv2cTrxnPeerRspWaitTimeout, thiz, &thiz->hRspTmr);
    } else {
      NwGtpv2cUlpApiT                         ulpApi;

      ulpApi.hMsg = 0;
      ulpApi.apiType = NW_GTPV2C_ULP_API_RSP_FAILURE_IND;
      ulpApi.apiInfo.rspFailureInfo.hUlpTrxn = thiz->hUlpTrxn;
      ulpApi.apiInfo.rspFailureInfo.hUlpTunnel = ((thiz->hTunnel) ? ((NwGtpv2cTunnelT *) (thiz->hTunnel))->hUlpTunnel : 0);
      OAILOG_ERROR (LOG_GTPV2C, "N3 retries expired for transaction 0x%p\n", thiz);
      RB_REMOVE (NwGtpv2cOutstandingTxSeqNumTrxnMap, &(pStack->outstandingTxSeqNumMap), thiz);
      rc = nwGtpv2cTrxnDelete (&thiz);
      rc = pStack->ulp.ulpReqCallback (pStack->ulp.hUlp, &ulpApi);
    }

    return rc;
  }

  static NwRcT                            nwGtpv2cTrxnDuplicateRequestWaitTimeout (
  void *arg) {
    NwRcT                                   rc = NW_OK;
    NwGtpv2cTrxnT                          *thiz;
    NwGtpv2cStackT                         *pStack;

    thiz = ((NwGtpv2cTrxnT *) arg);
    NW_ASSERT (thiz);
    pStack = thiz->pStack;
    NW_ASSERT (pStack);
    OAILOG_DEBUG (LOG_GTPV2C,  "Duplicate request hold timer expired for transaction 0x%p\n", thiz);
    thiz->hRspTmr = 0;
    RB_REMOVE (NwGtpv2cOutstandingRxSeqNumTrxnMap, &(pStack->outstandingRxSeqNumMap), thiz);
    rc = nwGtpv2cTrxnDelete (&thiz);
    NW_ASSERT (NW_OK == rc);
    return rc;
  }

/**
   Start timer to wait for rsp of a req message

   @param[in] thiz : Pointer to transaction
   @param[in] timeoutCallbackFunc : Timeout handler callback function.
   @return NW_OK on success.
*/

  NwRcT                                   nwGtpv2cTrxnStartPeerRspWaitTimer (
  NwGtpv2cTrxnT * thiz) {
    NwRcT                                   rc;

    rc = nwGtpv2cStartTimer (thiz->pStack, thiz->t3Timer, 0, NW_GTPV2C_TMR_TYPE_ONE_SHOT, nwGtpv2cTrxnPeerRspWaitTimeout, thiz, &thiz->hRspTmr);
    return rc;
  }

/**
  Start timer to wait before pruginf a req tran for which response has been sent

  @param[in] thiz : Pointer to transaction
  @return NW_OK on success.
*/

  NwRcT                                   nwGtpv2cTrxnStartDulpicateRequestWaitTimer (
  NwGtpv2cTrxnT * thiz) {
    NwRcT                                   rc;

    rc = nwGtpv2cStartTimer (thiz->pStack, thiz->t3Timer * thiz->maxRetries, 0, NW_GTPV2C_TMR_TYPE_ONE_SHOT, nwGtpv2cTrxnDuplicateRequestWaitTimeout, thiz, &thiz->hRspTmr);
    return rc;
  }

/**
  Send timer stop request to TmrMgr Entity.

  @param[in] thiz : Pointer to transaction
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cTrxnStopPeerRspTimer (
  NwGtpv2cTrxnT * thiz) {
    NwRcT                                   rc;

    NW_ASSERT (thiz->pStack->tmrMgr.tmrStopCallback != NULL);
    rc = nwGtpv2cStopTimer (thiz->pStack, thiz->hRspTmr);
    thiz->hRspTmr = 0;
    return rc;
  }

/*--------------------------------------------------------------------------*
                        P U B L I C    F U N C T I O N S
  --------------------------------------------------------------------------*/

/**
   Constructor

   @param[in] thiz : Pointer to stack
   @param[out] ppTrxn : Pointer to pointer to Trxn object.
   @return NW_OK on success.
*/
  NwGtpv2cTrxnT                          *nwGtpv2cTrxnNew (
  NW_IN NwGtpv2cStackT * thiz) {
    NwGtpv2cTrxnT                          *pTrxn;

    if (gpGtpv2cTrxnPool) {
      pTrxn = gpGtpv2cTrxnPool;
      gpGtpv2cTrxnPool = gpGtpv2cTrxnPool->next;
    } else {
      NW_GTPV2C_MALLOC (thiz, sizeof (NwGtpv2cTrxnT), pTrxn, NwGtpv2cTrxnT *);
    }

    if (pTrxn) {
      pTrxn->pStack = thiz;
      pTrxn->pMsg = NULL;
      pTrxn->maxRetries = 2;
      pTrxn->t3Timer = 2;
      pTrxn->seqNum = thiz->seqNum;
      /*
       * Increment sequence number
       */
      thiz->seqNum++;

      if (thiz->seqNum == 0x800000)
        thiz->seqNum = 0;
    }

    OAILOG_DEBUG (LOG_GTPV2C,  "Created transaction 0x%p\n", pTrxn);
    return pTrxn;
  }

/**
   Overloaded Constructor

   @param[in] thiz : Pointer to stack.
   @param[in] seqNum : Sequence number for this transaction.
   @return Pointer to Trxn object.
*/
  NwGtpv2cTrxnT                          *nwGtpv2cTrxnWithSeqNumNew (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN uint32_t seqNum) {
    NwGtpv2cTrxnT                          *pTrxn;

    if (gpGtpv2cTrxnPool) {
      pTrxn = gpGtpv2cTrxnPool;
      gpGtpv2cTrxnPool = gpGtpv2cTrxnPool->next;
    } else {
      NW_GTPV2C_MALLOC (thiz, sizeof (NwGtpv2cTrxnT), pTrxn, NwGtpv2cTrxnT *);
    }

    if (pTrxn) {
      pTrxn->pStack = thiz;
      pTrxn->pMsg = NULL;
      pTrxn->maxRetries = 2;
      pTrxn->t3Timer = 2;
      pTrxn->seqNum = seqNum;
      pTrxn->pMsg = NULL;
    }

    OAILOG_DEBUG (LOG_GTPV2C,  "Created transaction 0x%p\n", pTrxn);
    return pTrxn;
  }

/**
   Another overloaded constructor. Create transaction as outstanding
   RX transaction for detecting duplicated requests.

   @param[in] thiz : Pointer to stack.
   @param[in] teidLocal : Trxn teid.
   @param[in] peerIp : Peer Ip address.
   @param[in] peerPort : Peer Ip port.
   @param[in] seqNum : Seq Number.
   @return NW_OK on success.
*/

  NwGtpv2cTrxnT                          *nwGtpv2cTrxnOutstandingRxNew (
  NW_IN NwGtpv2cStackT * thiz,
  __attribute__ ((unused)) NW_IN uint32_t teidLocal,
  NW_IN uint32_t peerIp,
  NW_IN uint32_t peerPort,
  NW_IN uint32_t seqNum) {
    NwRcT                                   rc;
    NwGtpv2cTrxnT                          *pTrxn,
                                           *pCollision;

    if (gpGtpv2cTrxnPool) {
      pTrxn = gpGtpv2cTrxnPool;
      gpGtpv2cTrxnPool = gpGtpv2cTrxnPool->next;
    } else {
      NW_GTPV2C_MALLOC (thiz, sizeof (NwGtpv2cTrxnT), pTrxn, NwGtpv2cTrxnT *);
    }

    if (pTrxn) {
      pTrxn->pStack = thiz;
      pTrxn->maxRetries = 2;
      pTrxn->t3Timer = 2;
      pTrxn->seqNum = seqNum;
      pTrxn->peerIp = peerIp;
      pTrxn->peerPort = peerPort;
      pTrxn->pMsg = NULL;
      pTrxn->hRspTmr = 0;
      pCollision = RB_INSERT (NwGtpv2cOutstandingRxSeqNumTrxnMap, &(thiz->outstandingRxSeqNumMap), pTrxn);

      if (pCollision) {
        OAILOG_WARNING (LOG_GTPV2C,  "Duplicate request message received for seq num 0x%x!\n", (uint32_t) seqNum);

        /*
         * Case of duplicate request message from peer. Retransmit response.
         */
        if (pCollision->pMsg) {
          rc = pCollision->pStack->udp.udpDataReqCallback (pCollision->pStack->udp.hUdp, pCollision->pMsg->msgBuf, pCollision->pMsg->msgLen, pCollision->peerIp, pCollision->peerPort);
        }

        rc = nwGtpv2cTrxnDelete (&pTrxn);
        NW_ASSERT (NW_OK == rc);
        pTrxn = NULL;
      }
    }

    if (pTrxn)
      OAILOG_DEBUG (LOG_GTPV2C,  "Created outstanding RX transaction 0x%p\n", pTrxn);

    return (pTrxn);
  }

/**
   Destructor

   @param[out] pthiz : Pointer to pointer to Trxn object.
   @return NW_OK on success.
*/
  NwRcT                                   nwGtpv2cTrxnDelete (
  NW_INOUT NwGtpv2cTrxnT ** pthiz) {
    NwRcT                                   rc = NW_OK;
    NwGtpv2cStackT                         *pStack;
    NwGtpv2cTrxnT                          *thiz = *pthiz;

    pStack = thiz->pStack;

    if (thiz->hRspTmr) {
      rc = nwGtpv2cTrxnStopPeerRspTimer (thiz);
      NW_ASSERT (NW_OK == rc);
    }

    if (thiz->pMsg) {
      rc = nwGtpv2cMsgDelete ((NwGtpv2cStackHandleT) pStack, (NwGtpv2cMsgHandleT) thiz->pMsg);
      NW_ASSERT (NW_OK == rc);
    }

    OAILOG_DEBUG (LOG_GTPV2C,  "Purging  transaction 0x%p\n", thiz);
    thiz->next = gpGtpv2cTrxnPool;
    gpGtpv2cTrxnPool = thiz;
    *pthiz = NULL;
    return rc;
  }

#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*
                            E N D   O F   F I L E
  --------------------------------------------------------------------------*/
