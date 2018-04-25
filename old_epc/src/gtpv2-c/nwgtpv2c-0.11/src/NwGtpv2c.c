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
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "NwTypes.h"
#include "NwUtils.h"
#include "NwError.h"
#include "NwGtpv2cPrivate.h"
#include "NwGtpv2c.h"
#include "NwGtpv2cIe.h"
#include "NwGtpv2cTrxn.h"
#include "NwGtpv2cLog.h"
#include "dynamic_memory_check.h"
#include "gcc_diag.h"

#ifdef _NWGTPV2C_HAVE_TIMERADD
#  define NW_GTPV2C_TIMER_ADD(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))
#  define NW_GTPV2C_TIMER_SUB(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))
#  define NW_GTPV2C_TIMER_CMP_P(a, b, CMP)   timercmp(a, b, CMP)
#else

#  define NW_GTPV2C_TIMER_ADD(tvp, uvp, vvp)                               \
  do {                                                                  \
    (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;                      \
    (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;                   \
    if ((vvp)->tv_usec >= 1000000) {                                    \
      (vvp)->tv_sec++;                                                  \
      (vvp)->tv_usec -= 1000000;                                        \
    }                                                                   \
  } while (0)

#  define NW_GTPV2C_TIMER_SUB(tvp, uvp, vvp)                               \
  do {                                                                  \
    (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;                      \
    (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;                   \
    if ((vvp)->tv_usec < 0) {                                           \
      (vvp)->tv_sec--;                                                  \
      (vvp)->tv_usec += 1000000;                                        \
    }                                                                   \
  } while (0)

#  define NW_GTPV2C_TIMER_CMP_P(a, b, CMP)                                \
  (((a)->tv_sec == (b)->tv_sec) ?                                       \
   ((a)->tv_usec CMP (b)->tv_usec) :                                    \
   ((a)->tv_sec CMP (b)->tv_sec))

#endif


#define NW_GTPV2C_INIT_MSG_IE_PARSE_INFO(__thiz, __msgType)             \
  do {                                                                \
    __thiz->pGtpv2cMsgIeParseInfo[__msgType] =                        \
        nwGtpv2cMsgIeParseInfoNew((NwGtpv2cStackHandleT)__thiz, __msgType);\
  } while(0)

#define NW_GTPV2C_UDP_PORT                                              (2123)

#ifdef __cplusplus
extern                                  "C" {
#endif

  static NwGtpv2cTimeoutInfoT            *gpGtpv2cTimeoutInfoPool = NULL;

  typedef struct {
    int                                     currSize;
    int                                     maxSize;
    NwGtpv2cTimeoutInfoT                  **pHeap;
  } NwGtpv2cTmrMinHeapT;

#define NW_HEAP_PARENT_INDEX(__child)           ( ( (__child) - 1 ) / 2 )


  NwGtpv2cTmrMinHeapT                    *nwGtpv2cTmrMinHeapNew (
  int maxSize) {
    NwGtpv2cTmrMinHeapT                    *thiz = (NwGtpv2cTmrMinHeapT *) malloc (sizeof (NwGtpv2cTmrMinHeapT));

    if (thiz) {
      thiz->currSize = 0;
      thiz->maxSize = maxSize;
      thiz->pHeap = (NwGtpv2cTimeoutInfoT **) malloc (maxSize * sizeof (NwGtpv2cTimeoutInfoT *));
    }

    return                                  thiz;
  }

  void
                                          nwGtpv2cTmrMinHeapDelete (
  NwGtpv2cTmrMinHeapT * thiz) {
    free_wrapper ((void**) &thiz->pHeap);
    free_wrapper ((void**) &thiz);
  }

  static NwRcT                            nwGtpv2cTmrMinHeapInsert (
  NwGtpv2cTmrMinHeapT * thiz,
  NwGtpv2cTimeoutInfoT * pTimerEvent) {
    int                                     holeIndex = thiz->currSize++;

    NW_ASSERT (thiz->currSize < thiz->maxSize);

    while ((holeIndex > 0) && NW_GTPV2C_TIMER_CMP_P (&(thiz->pHeap[NW_HEAP_PARENT_INDEX (holeIndex)])->tvTimeout, &(pTimerEvent->tvTimeout), >)) {
      thiz->pHeap[holeIndex] = thiz->pHeap[NW_HEAP_PARENT_INDEX (holeIndex)];
      thiz->pHeap[holeIndex]->timerMinHeapIndex = holeIndex;
      holeIndex = NW_HEAP_PARENT_INDEX (holeIndex);
    }

    thiz->pHeap[holeIndex] = pTimerEvent;
    pTimerEvent->timerMinHeapIndex = holeIndex;
    return holeIndex;
  }

#define NW_MIN_HEAP_INDEX_INVALID                       (0xFFFFFFFF)
  static NwRcT                            nwGtpv2cTmrMinHeapRemove (
  NwGtpv2cTmrMinHeapT * thiz,
  int minHeapIndex) {
    NwGtpv2cTimeoutInfoT                   *pTimerEvent = NULL;
    int                                     holeIndex = minHeapIndex;
    int                                     minChild = 0,
                                            maxChild = 0;

    //printf("- Trying Removing %p from index %u, currSize %u, minChild %u, maxChild %u\n", thiz->pHeap[minHeapIndex], minHeapIndex, thiz->currSize, minChild, maxChild);
    if (minHeapIndex == NW_MIN_HEAP_INDEX_INVALID)
      return NW_FAILURE;

    if (minHeapIndex < thiz->currSize) {
      thiz->pHeap[minHeapIndex]->timerMinHeapIndex = NW_MIN_HEAP_INDEX_INVALID;
      thiz->currSize--;
      pTimerEvent = thiz->pHeap[thiz->currSize];
      holeIndex = minHeapIndex;
      minChild = (2 * holeIndex) + 1;
      maxChild = minChild + 1;

      //printf("- Removing %p from index %u, currSize %u, minChild %u, maxChild %u\n", thiz->pHeap[minHeapIndex], minHeapIndex, thiz->currSize, minChild, maxChild);

      while ((maxChild) <= thiz->currSize) {
        if (NW_GTPV2C_TIMER_CMP_P (&(thiz->pHeap[minChild]->tvTimeout), &(thiz->pHeap[maxChild]->tvTimeout), >))
          minChild = maxChild;

        if (NW_GTPV2C_TIMER_CMP_P (&(pTimerEvent->tvTimeout), &(thiz->pHeap[minChild]->tvTimeout), <)) {
          break;
        }

        thiz->pHeap[holeIndex] = thiz->pHeap[minChild];
        thiz->pHeap[holeIndex]->timerMinHeapIndex = holeIndex;
        holeIndex = minChild;
        minChild = (2 * holeIndex) + 1;
        maxChild = minChild + 1;
      }

      while ((holeIndex > 0) && NW_GTPV2C_TIMER_CMP_P (&((thiz->pHeap[NW_HEAP_PARENT_INDEX (holeIndex)])->tvTimeout), &(pTimerEvent->tvTimeout), >)) {
        thiz->pHeap[holeIndex] = thiz->pHeap[NW_HEAP_PARENT_INDEX (holeIndex)];
        thiz->pHeap[holeIndex]->timerMinHeapIndex = holeIndex;
        holeIndex = NW_HEAP_PARENT_INDEX (holeIndex);
      }

      if (holeIndex < thiz->currSize) {
        thiz->pHeap[holeIndex] = pTimerEvent;
        pTimerEvent->timerMinHeapIndex = holeIndex;
      }

      thiz->pHeap[thiz->currSize] = NULL;
      return NW_OK;
    }

    return NW_FAILURE;
  }

  static NwGtpv2cTimeoutInfoT            *nwGtpv2cTmrMinHeapPeek (
  NwGtpv2cTmrMinHeapT * thiz) {
    if (thiz->currSize) {
      //printf("- Peek Returning %p at index %u(%u)\n", thiz->pHeap[0], thiz->pHeap[0]->timerMinHeapIndex, thiz->currSize);
      return thiz->pHeap[0];
    }

    return NULL;
  }
/*--------------------------------------------------------------------------*
                      P R I V A T E    F U N C T I O N S
  --------------------------------------------------------------------------*/

  static void
                                          nwGtpv2cDisplayBanner (
  NwGtpv2cStackT * thiz) {
#if DISPLAY_LICENCE_INFO
    OAILOG_INFO(LOG_GTPV2C, " *----------------------------------------------------------------------------*\n");
    OAILOG_INFO(LOG_GTPV2C, " *                                                                            *\n");
    OAILOG_INFO(LOG_GTPV2C, " *                             n w - g t p v 2 c                              *\n");
    OAILOG_INFO(LOG_GTPV2C, " *    G P R S    T u n n e l i n g    P r o t o c o l   v 2 c   S t a c k     *\n");
    OAILOG_INFO(LOG_GTPV2C, " *                                                                            *\n");
    OAILOG_INFO(LOG_GTPV2C, " *                                                                            *\n");
    OAILOG_INFO(LOG_GTPV2C, " * Copyright (c) 2010-2011 Amit Chawre                                        *\n");
    OAILOG_INFO(LOG_GTPV2C, " * All rights reserved.                                                       *\n");
    OAILOG_INFO(LOG_GTPV2C, " *                                                                            *\n");
    OAILOG_INFO(LOG_GTPV2C, " * Redistribution and use in source and binary forms, with or without         *\n");
    OAILOG_INFO(LOG_GTPV2C, " * modification, are permitted provided that the following conditions         *\n");
    OAILOG_INFO(LOG_GTPV2C, " * are met:                                                                   *\n");
    OAILOG_INFO(LOG_GTPV2C, " *                                                                            *\n");
    OAILOG_INFO(LOG_GTPV2C, " * 1. Redistributions of source code must retain the above copyright          *\n");
    OAILOG_INFO(LOG_GTPV2C, " *    notice, this list of conditions and the following disclaimer.           *\n");
    OAILOG_INFO(LOG_GTPV2C, " * 2. Redistributions in binary form must reproduce the above copyright       *\n");
    OAILOG_INFO(LOG_GTPV2C, " *    notice, this list of conditions and the following disclaimer in the     *\n");
    OAILOG_INFO(LOG_GTPV2C, " *    documentation and/or other materials provided with the distribution.    *\n");
    OAILOG_INFO(LOG_GTPV2C, " * 3. The name of the author may not be used to endorse or promote products   *\n");
    OAILOG_INFO(LOG_GTPV2C, " *    derived from this software without specific prior written permission.   *\n");
    OAILOG_INFO(LOG_GTPV2C, " *                                                                            *\n");
    OAILOG_INFO(LOG_GTPV2C, " * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR       *\n");
    OAILOG_INFO(LOG_GTPV2C, " * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *\n");
    OAILOG_INFO(LOG_GTPV2C, " * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.    *\n");
    OAILOG_INFO(LOG_GTPV2C, " * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,           *\n");
    OAILOG_INFO(LOG_GTPV2C, " * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT   *\n");
    OAILOG_INFO(LOG_GTPV2C, " * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *\n");
    OAILOG_INFO(LOG_GTPV2C, " * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY      *\n");
    OAILOG_INFO(LOG_GTPV2C, " * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT        *\n");
    OAILOG_INFO(LOG_GTPV2C, " * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF   *\n");
    OAILOG_INFO(LOG_GTPV2C, " * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *\n");
    OAILOG_INFO(LOG_GTPV2C, " *----------------------------------------------------------------------------*\n\n");
#endif
  }

/*---------------------------------------------------------------------------
   Tunnel RBTree Search Data Structure
  --------------------------------------------------------------------------*/

/**
  Comparator funtion for comparing two sequence number transactions.

  @param[in] a: Pointer to session a.
  @param[in] b: Pointer to session b.
  @return  An integer greater than, equal to or less than zero according to whether the
  object pointed to by a is greater than, equal to or less than the object pointed to by b.
*/

  static inline NwS32T                    nwGtpv2cCompareTunnel (
  struct NwGtpv2cTunnel *a,
  struct NwGtpv2cTunnel *b) {
    if (a->teid > b->teid)
      return 1;

    if (a->teid < b->teid)
      return -1;

    if (a->ipv4AddrRemote > b->ipv4AddrRemote)
      return 1;

    if (a->ipv4AddrRemote < b->ipv4AddrRemote)
      return -1;

    return 0;
  }

  RB_GENERATE (NwGtpv2cTunnelMap, NwGtpv2cTunnel, tunnelMapRbtNode, nwGtpv2cCompareTunnel)

/*---------------------------------------------------------------------------
   Transaction RBTree Search Data Structure
  --------------------------------------------------------------------------*/
/**
  Comparator funtion for comparing two outstancing TX transactions.

  @param[in] a: Pointer to session a.
  @param[in] b: Pointer to session b.
  @return  An integer greater than, equal to or less than zero according to whether the
  object pointed to by a is greater than, equal to or less than the object pointed to by b.
*/
  static inline NwS32T                    nwGtpv2cCompareOutstandingTxSeqNumTrxn (
  struct NwGtpv2cTrxn *a,
  struct NwGtpv2cTrxn *b) {
    if (a->seqNum > b->seqNum)
      return 1;

    if (a->seqNum < b->seqNum)
      return -1;

    if (a->peerIp > b->peerIp)
      return 1;

    if (a->peerIp < b->peerIp)
      return -1;

    return 0;
  }

  RB_GENERATE (NwGtpv2cOutstandingTxSeqNumTrxnMap, NwGtpv2cTrxn, outstandingTxSeqNumMapRbtNode, nwGtpv2cCompareOutstandingTxSeqNumTrxn)

/**
  Comparator funtion for comparing outstanding RX transactions.

  @param[in] a: Pointer to session a.
  @param[in] b: Pointer to session b.
  @return  An integer greater than, equal to or less than zero according to whether the
  object pointed to by a is greater than, equal to or less than the object pointed to by b.
*/
  static inline NwS32T                    nwGtpv2cCompareOutstandingRxSeqNumTrxn (
  struct NwGtpv2cTrxn *a,
  struct NwGtpv2cTrxn *b) {
    if (a->seqNum > b->seqNum)
      return 1;

    if (a->seqNum < b->seqNum)
      return -1;

    if (a->peerIp > b->peerIp)
      return 1;

    if (a->peerIp < b->peerIp)
      return -1;

    if (a->peerPort > b->peerPort)
      return 1;

    if (a->peerPort < b->peerPort)
      return -1;

    return 0;
  }

  RB_GENERATE (NwGtpv2cOutstandingRxSeqNumTrxnMap, NwGtpv2cTrxn, outstandingRxSeqNumMapRbtNode, nwGtpv2cCompareOutstandingRxSeqNumTrxn)

/*---------------------------------------------------------------------------
   Timer RB-tree data structure.
  --------------------------------------------------------------------------*/
/**
  Comparator funtion for comparing two outstancing TX transactions.

  @param[in] a: Pointer to session a.
  @param[in] b: Pointer to session b.
  @return  An integer greater than, equal to or less than zero according to whether the
  object pointed to by a is greater than, equal to or less than the object pointed to by b.
*/
  static inline NwS32T                    nwGtpv2cCompareOutstandingTxRexmitTime (
  struct NwGtpv2cTimeoutInfo *a,
  struct NwGtpv2cTimeoutInfo *b) {
    if (NW_GTPV2C_TIMER_CMP_P (&a->tvTimeout, &b->tvTimeout, >))
      return 1;

    if (NW_GTPV2C_TIMER_CMP_P (&a->tvTimeout, &b->tvTimeout, <))
      return -1;

    return 0;
  }

  RB_GENERATE (NwGtpv2cActiveTimerList, NwGtpv2cTimeoutInfo, activeTimerListRbtNode, nwGtpv2cCompareOutstandingTxRexmitTime)



/**
   Send msg to peer via data request to UDP Entity

   @param[in] thiz : Pointer to stack.
   @param[in] peerIp : Peer Ip address.
   @param[in] peerPort : Peer Ip port.
   @param[in] pMsg : Message to be sent.
   @return NW_OK on success.
*/
  static NwRcT                            nwGtpv2cCreateAndSendMsg (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN uint32_t seqNum,
  NW_IN uint32_t peerIp,
  NW_IN uint32_t peerPort,
  NW_IN NwGtpv2cMsgT * pMsg) {
    NwRcT                                   rc = NW_FAILURE;
    uint8_t                                *msgHdr = NULL;

    NW_ASSERT (thiz);
    NW_ASSERT (pMsg);
    msgHdr = pMsg->msgBuf;
    /*
     * Set flags in header
     */
    *(msgHdr++) = (pMsg->version << 5) | (pMsg->teidPresent << 3);
    /*
     * Set msg type in header
     */
    *(msgHdr++) = (pMsg->msgType);
    /*
     * Set msg length in header
     */
    *((uint16_t *) msgHdr) = htons (pMsg->msgLen - 4);
    msgHdr += 2;

    /*
     * Set TEID, if present in header
     */
    if (pMsg->teidPresent) {
      *((uint32_t *) msgHdr) = htonl (pMsg->teid);
      msgHdr += 4;
    }

    /*
     * Set seq num in header
     */
    *((uint32_t *) msgHdr) = htonl (seqNum << 8);
    /*
     * Call UDP data request callback
     */
    NW_ASSERT (thiz->udp.udpDataReqCallback != NULL);
    rc = thiz->udp.udpDataReqCallback (thiz->udp.hUdp, pMsg->msgBuf, pMsg->msgLen, peerIp, peerPort);
    NW_ASSERT (NW_OK == rc);
    return rc;
  }

/**
  Send an Version Not Supported message

  @param[in] thiz : Stack pointer
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cSendVersionNotSupportedInd (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN uint32_t peerIp,
  NW_IN uint32_t peerPort,
  NW_IN uint32_t seqNum) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cMsgHandleT                      hMsg = 0;

    rc = nwGtpv2cMsgNew ((NwGtpv2cStackHandleT) thiz, NW_FALSE, NW_GTP_VERSION_NOT_SUPPORTED_IND, 0x00, seqNum, (&hMsg));
    NW_ASSERT (NW_OK == rc);
    OAILOG_NOTICE (LOG_GTPV2C,  "Sending Version Not Supported Indication message to %x:%x with seq %u\n", peerIp, peerPort, seqNum);
    rc = nwGtpv2cCreateAndSendMsg (thiz, seqNum, peerIp, peerPort, (NwGtpv2cMsgT *) hMsg);
    rc = nwGtpv2cMsgDelete ((NwGtpv2cStackHandleT) thiz, hMsg);
    NW_ASSERT (NW_OK == rc);
    return rc;
  }

/**
  Create a local tunnel.

  @param[in] thiz : Stack pointer
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cCreateLocalTunnel (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN uint32_t teid,
  NW_IN uint32_t ipv4Remote,
  NW_IN NwGtpv2cUlpTunnelHandleT hUlpTunnel,
  NW_OUT NwGtpv2cTunnelHandleT * phTunnel) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cTunnelT                        *pTunnel = NULL,
                                           *pCollision = NULL;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    OAILOG_DEBUG (LOG_GTPV2C, "Creating local tunnel with teid '0x%x' and peer IP 0x%x\n", teid, ipv4Remote);
    pTunnel = nwGtpv2cTunnelNew (thiz, teid, ipv4Remote, hUlpTunnel);

    if (pTunnel) {
      pCollision = RB_INSERT (NwGtpv2cTunnelMap, &(thiz->tunnelMap), pTunnel);

      if (pCollision) {
        rc = nwGtpv2cTunnelDelete (thiz, pTunnel);
        NW_ASSERT (NW_OK == rc);
        *phTunnel = (NwGtpv2cTunnelHandleT) 0;
        OAILOG_WARNING (LOG_GTPV2C,  "Local tunnel creation failed for teid '0x%x' and peer IP " NW_IPV4_ADDR ". Tunnel already exists!\n", teid, NW_IPV4_ADDR_FORMAT (ipv4Remote));
        NW_ASSERT (0);
        OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
      }
    } else {
      rc = NW_FAILURE;
    }

    *phTunnel = (NwGtpv2cTunnelHandleT) pTunnel;
    OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
  }

/**
  Delete a local tunnel.

  @param[in] thiz : Stack pointer
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cDeleteLocalTunnel (
  NW_IN NwGtpv2cStackT * thiz,
  NW_OUT NwGtpv2cTunnelHandleT hTunnel) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cTunnelT                        *pTunnel = (NwGtpv2cTunnelT *) hTunnel;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    pTunnel = RB_REMOVE (NwGtpv2cTunnelMap, &(thiz->tunnelMap), (NwGtpv2cTunnelT *) hTunnel);
    NW_ASSERT (pTunnel == (NwGtpv2cTunnelT *) hTunnel);
    OAILOG_DEBUG (LOG_GTPV2C, "Deleting local tunnel with teid '0x%x' and peer IP 0x%x\n", pTunnel->teid, pTunnel->ipv4AddrRemote);
    rc = nwGtpv2cTunnelDelete (thiz, pTunnel);
    NW_ASSERT (NW_OK == rc);
    OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
  }

/*---------------------------------------------------------------------------
   ULP API Processing Functions
  --------------------------------------------------------------------------*/

/**
  Process NW_GTPV2C_ULP_API_INITIAL_REQ Request from ULP entity.

  @param[in] hGtpcStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleUlpInitialReq (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN NwGtpv2cUlpApiT * pUlpReq) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cTrxnT                          *pTrxn = NULL;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    /*
     * Create New Transaction
     */
    pTrxn = nwGtpv2cTrxnNew (thiz);

    if (pTrxn) {
      if (!pUlpReq->apiInfo.initialReqInfo.hTunnel) {
        rc = nwGtpv2cCreateLocalTunnel (thiz, pUlpReq->apiInfo.initialReqInfo.teidLocal, pUlpReq->apiInfo.initialReqInfo.peerIp, pUlpReq->apiInfo.initialReqInfo.hUlpTunnel, &pUlpReq->apiInfo.initialReqInfo.hTunnel);
        NW_ASSERT (NW_OK == rc);
      }

      pTrxn->pMsg = (NwGtpv2cMsgT *) pUlpReq->hMsg;
      pTrxn->hTunnel = pUlpReq->apiInfo.initialReqInfo.hTunnel;
      pTrxn->hUlpTrxn = pUlpReq->apiInfo.initialReqInfo.hUlpTrxn;
      pTrxn->peerIp = ((NwGtpv2cTunnelT *) (pTrxn->hTunnel))->ipv4AddrRemote;
      pTrxn->peerPort = NW_GTPV2C_UDP_PORT;

      if (pUlpReq->apiType & NW_GTPV2C_ULP_API_FLAG_IS_COMMAND_MESSAGE) {
        pTrxn->seqNum |= 0x00100000UL;
      }

      rc = nwGtpv2cCreateAndSendMsg (thiz, pTrxn->seqNum, pTrxn->peerIp, pTrxn->peerPort, pTrxn->pMsg);

      if (NW_OK == rc) {
        /*
         * Start guard timer
         */
        rc = nwGtpv2cTrxnStartPeerRspWaitTimer (pTrxn);
        NW_ASSERT (NW_OK == rc);
        /*
         * Insert into search tree
         */
        pTrxn = RB_INSERT (NwGtpv2cOutstandingTxSeqNumTrxnMap, &(thiz->outstandingTxSeqNumMap), pTrxn);
        NW_ASSERT (pTrxn == NULL);
      } else {
        rc = nwGtpv2cTrxnDelete (&pTrxn);
        NW_ASSERT (NW_OK == rc);
      }
    } else {
      rc = NW_FAILURE;
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
  Process NW_GTPV2C_ULP_API_TRIGGERED_REQ Request from ULP entity.

  @param[in] hGtpcStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleUlpTriggeredReq (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN NwGtpv2cUlpApiT * pUlpReq) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cTrxnT                          *pTrxn = NULL;
    NwGtpv2cTrxnT                          *pReqTrxn = NULL;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    /*
     * Create New Transaction
     */
    pTrxn = nwGtpv2cTrxnWithSeqNumNew (thiz, (((NwGtpv2cMsgT *) (pUlpReq->hMsg))->seqNum));

    if (pTrxn) {
      pReqTrxn = (NwGtpv2cTrxnT *) pUlpReq->apiInfo.triggeredReqInfo.hTrxn;
      pTrxn->hUlpTrxn = pUlpReq->apiInfo.triggeredReqInfo.hUlpTrxn;
      pTrxn->peerIp = pReqTrxn->peerIp;
      pTrxn->peerPort = pReqTrxn->peerPort;
      pTrxn->pMsg = (NwGtpv2cMsgT *) pUlpReq->hMsg;
      rc = nwGtpv2cCreateAndSendMsg (thiz, pTrxn->seqNum, pTrxn->peerIp, pTrxn->peerPort, pTrxn->pMsg);

      if (NW_OK == rc) {
        /*
         * Start guard timer
         */
        rc = nwGtpv2cTrxnStartPeerRspWaitTimer (pTrxn);
        NW_ASSERT (NW_OK == rc);
        /*
         * Insert into search tree
         */
        RB_INSERT (NwGtpv2cOutstandingTxSeqNumTrxnMap, &(thiz->outstandingTxSeqNumMap), pTrxn);

        if (!pUlpReq->apiInfo.triggeredReqInfo.hTunnel) {
          rc = nwGtpv2cCreateLocalTunnel (thiz, pUlpReq->apiInfo.triggeredReqInfo.teidLocal, pReqTrxn->peerIp, pUlpReq->apiInfo.triggeredReqInfo.hUlpTunnel, &pUlpReq->apiInfo.triggeredReqInfo.hTunnel);
        }
      } else {
        rc = nwGtpv2cTrxnDelete (&pTrxn);
        NW_ASSERT (NW_OK == rc);
      }
    } else {
      rc = NW_FAILURE;
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
  Process NW_GTPV2C_ULP_API_TRIGGERED_RSP Request from ULP entity.

  @param[in] hGtpcStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleUlpTriggeredRsp (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN NwGtpv2cUlpApiT * pUlpRsp) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cTrxnT                          *pReqTrxn = NULL;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    pReqTrxn = (NwGtpv2cTrxnT *) pUlpRsp->apiInfo.triggeredRspInfo.hTrxn;
    NW_ASSERT (pReqTrxn != NULL);

    if (((NwGtpv2cMsgT *) pUlpRsp->hMsg)->seqNum == 0)
      ((NwGtpv2cMsgT *) pUlpRsp->hMsg)->seqNum = pReqTrxn->seqNum;

    OAILOG_DEBUG (LOG_GTPV2C, "Sending response message over seq '0x%x'\n", pReqTrxn->seqNum);
    rc = nwGtpv2cCreateAndSendMsg (thiz, pReqTrxn->seqNum, pReqTrxn->peerIp, pReqTrxn->peerPort, (NwGtpv2cMsgT *) pUlpRsp->hMsg);
    pReqTrxn->pMsg = (NwGtpv2cMsgT *) pUlpRsp->hMsg;
    rc = nwGtpv2cTrxnStartDulpicateRequestWaitTimer (pReqTrxn);

    if ((pUlpRsp->apiType & 0xFF000000) == NW_GTPV2C_ULP_API_FLAG_CREATE_LOCAL_TUNNEL) {
      rc = nwGtpv2cCreateLocalTunnel (thiz, pUlpRsp->apiInfo.triggeredRspInfo.teidLocal, pReqTrxn->peerIp, pUlpRsp->apiInfo.triggeredRspInfo.hUlpTunnel, &pUlpRsp->apiInfo.triggeredRspInfo.hTunnel);
    }

    OAILOG_FUNC_RETURN( LOG_GTPV2C, rc);
  }

/**
  Process NW_GTPV2C_ULP_CREATE_LOCAL_TUNNEL Request from ULP entity.

  @param[in] hGtpcStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleUlpCreateLocalTunnel (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN NwGtpv2cUlpApiT * pUlpReq) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cTunnelT                        *pTunnel = NULL,
                                           *pCollision = NULL;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    OAILOG_DEBUG (LOG_GTPV2C, "Creating local tunnel with teid '0x%x' and peer IP 0x%x\n", pUlpReq->apiInfo.createLocalTunnelInfo.teidLocal, pUlpReq->apiInfo.createLocalTunnelInfo.peerIp);
    pTunnel = nwGtpv2cTunnelNew (thiz, pUlpReq->apiInfo.createLocalTunnelInfo.teidLocal, pUlpReq->apiInfo.createLocalTunnelInfo.peerIp, pUlpReq->apiInfo.triggeredRspInfo.hUlpTunnel);
    NW_ASSERT (pTunnel);
    pCollision = RB_INSERT (NwGtpv2cTunnelMap, &(thiz->tunnelMap), pTunnel);

    if (pCollision) {
      rc = nwGtpv2cTunnelDelete (thiz, pTunnel);
      NW_ASSERT (NW_OK == rc);
      pUlpReq->apiInfo.createLocalTunnelInfo.hTunnel = (NwGtpv2cTunnelHandleT) 0;
      OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_FAILURE);
    }

    pUlpReq->apiInfo.createLocalTunnelInfo.hTunnel = (NwGtpv2cTunnelHandleT) pTunnel;
    OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
  }

/**
  Process NW_GTPV2C_ULP_DELETE_LOCAL_TUNNEL Request from ULP entity.

  @param[in] hGtpcStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleUlpDeleteLocalTunnel (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN NwGtpv2cUlpApiT * pUlpReq) {
    NwRcT                                   rc = NW_FAILURE;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    rc = nwGtpv2cDeleteLocalTunnel (thiz, pUlpReq->apiInfo.deleteLocalTunnelInfo.hTunnel);
    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
  Send GTPv2c Initial Request Message Indication to ULP entity.

  @param[in] hGtpcStackHandle : Stack handle
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cSendInitialReqIndToUlp (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN NwGtpv2cErrorT * pError,
  NW_IN NwGtpv2cTrxnT * pTrxn,
  NW_IN uint32_t hUlpTunnel,
  NW_IN uint32_t msgType,
  NW_IN uint32_t peerIp,
  NW_IN uint16_t peerPort,
  NW_IN NwGtpv2cMsgHandleT hMsg) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cUlpApiT                         ulpApi;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    ulpApi.hMsg = hMsg;
    ulpApi.apiType = NW_GTPV2C_ULP_API_INITIAL_REQ_IND;
    ulpApi.apiInfo.initialReqIndInfo.msgType = msgType;
    ulpApi.apiInfo.initialReqIndInfo.hTrxn = (NwGtpv2cTrxnHandleT) pTrxn;
    ulpApi.apiInfo.initialReqIndInfo.hUlpTunnel = hUlpTunnel;
    ulpApi.apiInfo.initialReqIndInfo.peerIp = peerIp;
    ulpApi.apiInfo.initialReqIndInfo.peerPort = peerPort;
    ulpApi.apiInfo.triggeredRspIndInfo.error = *pError;
    rc = thiz->ulp.ulpReqCallback (thiz->ulp.hUlp, &ulpApi);
    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
  Send GTPv2c Triggered Response Indication to ULP entity.

  @param[in] hGtpcStackHandle : Stack handle
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cSendTriggeredRspIndToUlp (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN NwGtpv2cErrorT * pError,
  NW_IN uint32_t hUlpTrxn,
  NW_IN uint32_t hUlpTunnel,
  NW_IN uint32_t msgType,
  NW_IN NwGtpv2cMsgHandleT hMsg) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cUlpApiT                         ulpApi;

    OAILOG_FUNC_IN (LOG_GTPV2C);
    ulpApi.hMsg = hMsg;
    ulpApi.apiType = NW_GTPV2C_ULP_API_TRIGGERED_RSP_IND;
    ulpApi.apiInfo.triggeredRspIndInfo.msgType = msgType;
    ulpApi.apiInfo.triggeredRspIndInfo.hUlpTrxn = hUlpTrxn;
    ulpApi.apiInfo.triggeredRspIndInfo.hUlpTunnel = hUlpTunnel;
    ulpApi.apiInfo.triggeredRspIndInfo.error = *pError;
    rc = thiz->ulp.ulpReqCallback (thiz->ulp.hUlp, &ulpApi);
    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
  Handle Echo Request from Peer Entity.

  @param[in] thiz : Stack context
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleEchoReq (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN uint32_t msgType,
  NW_IN uint8_t * msgBuf,
  NW_IN uint32_t msgBufLen,
  NW_IN uint16_t peerPort,
  NW_IN uint32_t peerIp) {
    NwRcT                                   rc = NW_FAILURE;
    uint32_t                                seqNum = 0;
    NwGtpv2cMsgHandleT                      hMsg = 0;

    seqNum = ntohl (*((uint32_t *) (msgBuf + (((*msgBuf) & 0x08) ? 8 : 4)))) >> 8;
    /*
     * Send Echo Response
     */
    rc = nwGtpv2cMsgNew ((NwGtpv2cStackHandleT) thiz, NW_FALSE, /* TEID present flag    */
                         NW_GTP_ECHO_RSP,       /* Msg Type             */
                         0x00,  /* TEID                 */
                         seqNum,        /* Seq Number           */
                         (&hMsg));
    NW_ASSERT (NW_OK == rc);
    rc = nwGtpv2cMsgAddIeTV1 (hMsg, NW_GTPV2C_IE_RECOVERY, 0, thiz->restartCounter);
    OAILOG_ERROR (LOG_GTPV2C, "Sending NW_GTP_ECHO_RSP message to " NW_IPV4_ADDR ":%u with seq %u\n", NW_IPV4_ADDR_FORMAT (peerIp), peerPort, (seqNum));
    rc = nwGtpv2cCreateAndSendMsg (thiz, (seqNum), peerIp, peerPort, (NwGtpv2cMsgT *) hMsg);
    rc = nwGtpv2cMsgDelete ((NwGtpv2cStackHandleT) thiz, hMsg);
    NW_ASSERT (NW_OK == rc);
    return rc;
  }

/**
  Handle Initial Request from Peer Entity.

  @param[in] thiz : Stack context
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleInitialReq (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN uint32_t msgType,
  NW_IN uint8_t * msgBuf,
  NW_IN uint32_t msgBufLen,
  NW_IN uint16_t peerPort,
  NW_IN uint32_t peerIp) {
    NwRcT                                   rc = NW_FAILURE;
    uint32_t                                seqNum = 0;
    uint32_t                                teidLocal = 0;
    NwGtpv2cTrxnT                          *pTrxn = NULL;
    NwGtpv2cTunnelT                        *pLocalTunnel = NULL,
                                            keyTunnel = {0};
    NwGtpv2cMsgHandleT                      hMsg = 0;
    NwGtpv2cUlpTunnelHandleT                hUlpTunnel = 0;
    NwGtpv2cErrorT                          error = {0};

    teidLocal = *((uint32_t *) (msgBuf + 4));

    if (teidLocal) {
      keyTunnel.teid = ntohl (teidLocal);
      keyTunnel.ipv4AddrRemote = peerIp;
      pLocalTunnel = RB_FIND (NwGtpv2cTunnelMap, &(thiz->tunnelMap), &keyTunnel);

      if (!pLocalTunnel) {
        OAILOG_WARNING (LOG_GTPV2C,  "Request message received on non-existent teid 0x%x from peer 0x%x received! Discarding.\n", ntohl (teidLocal), htonl (peerIp));
        return NW_OK;
      }

      hUlpTunnel = pLocalTunnel->hUlpTunnel;
    } else {
      hUlpTunnel = 0;
    }

    seqNum = ntohl (*((uint32_t *) (msgBuf + (((*msgBuf) & 0x08) ? 8 : 4)))) >> 8;
    pTrxn = nwGtpv2cTrxnOutstandingRxNew (thiz, ntohl (teidLocal), peerIp, peerPort, (seqNum));

    if (pTrxn) {
      rc = nwGtpv2cMsgFromBufferNew ((NwGtpv2cStackHandleT) thiz, msgBuf, msgBufLen, &(hMsg));
      NW_ASSERT (thiz->pGtpv2cMsgIeParseInfo[msgType]);
      rc = nwGtpv2cMsgIeParse (thiz->pGtpv2cMsgIeParseInfo[msgType], hMsg, &error);

      if (rc != NW_OK) {
        OAILOG_WARNING (LOG_GTPV2C,  "Malformed request message received on TEID %u from peer 0x%x. Notifying ULP.\n", ntohl (teidLocal), htonl (peerIp));
      }

      rc = nwGtpv2cSendInitialReqIndToUlp (thiz, &error, pTrxn, hUlpTunnel, msgType, peerIp, peerPort, hMsg);
    }

    return NW_OK;
  }

/**
  Handle Triggered Response from Peer Entity.

  @param[in] thiz : Stack context
  @return NW_OK on success.
*/

  static NwRcT                            nwGtpv2cHandleTriggeredRsp (
  NW_IN NwGtpv2cStackT * thiz,
  NW_IN uint32_t msgType,
  NW_IN uint8_t * msgBuf,
  NW_IN uint32_t msgBufLen,
  NW_IN uint16_t peerPort,
  NW_IN uint32_t peerIp) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cTrxnT                          *pTrxn = NULL,
                                            keyTrxn;
    NwGtpv2cMsgHandleT                      hMsg = 0;
    NwGtpv2cErrorT                          error = {0};

    keyTrxn.seqNum = ntohl (*((uint32_t *) (msgBuf + (((*msgBuf) & 0x08) ? 8 : 4)))) >> 8;;
    keyTrxn.peerIp = peerIp;
    pTrxn = RB_FIND (NwGtpv2cOutstandingTxSeqNumTrxnMap, &(thiz->outstandingTxSeqNumMap), &keyTrxn);

    if (pTrxn) {
      uint32_t                                hUlpTrxn;
      uint32_t                                hUlpTunnel;

      hUlpTrxn = pTrxn->hUlpTrxn;
      hUlpTunnel = (pTrxn->hTunnel ? ((NwGtpv2cTunnelT *) (pTrxn->hTunnel))->hUlpTunnel : 0);
      RB_REMOVE (NwGtpv2cOutstandingTxSeqNumTrxnMap, &(thiz->outstandingTxSeqNumMap), pTrxn);
      rc = nwGtpv2cTrxnDelete (&pTrxn);
      NW_ASSERT (NW_OK == rc);
      NW_ASSERT (msgBuf && msgBufLen);
      rc = nwGtpv2cMsgFromBufferNew ((NwGtpv2cStackHandleT) thiz, msgBuf, msgBufLen, &(hMsg));
      NW_ASSERT (thiz->pGtpv2cMsgIeParseInfo[msgType]);
      rc = nwGtpv2cMsgIeParse (thiz->pGtpv2cMsgIeParseInfo[msgType], hMsg, &error);

      if (rc != NW_OK) {
        OAILOG_WARNING (LOG_GTPV2C,  "Malformed message received on TEID %u from peer 0x%x. Notifying ULP.\n", ntohl ((*((uint32_t *) (msgBuf + 4)))), htonl (peerIp));
      }

      rc = nwGtpv2cSendTriggeredRspIndToUlp (thiz, &error, hUlpTrxn, hUlpTunnel, msgType, hMsg);
    } else {
      OAILOG_WARNING (LOG_GTPV2C,  "Response message without a matching outstanding request received! Discarding.\n");
      rc = NW_OK;
    }

    return rc;
  }

/*--------------------------------------------------------------------------*
                       P U B L I C   F U N C T I O N S
  --------------------------------------------------------------------------*/

/**
   Constructor
*/

  NwRcT                                   nwGtpv2cInitialize (
  NW_INOUT NwGtpv2cStackHandleT * hGtpcStackHandle) {
    NwRcT                                   rc = NW_OK;
    NwGtpv2cStackT                         *thiz = NULL;

    thiz = (NwGtpv2cStackT *) malloc (sizeof (NwGtpv2cStackT));
    memset (thiz, 0, sizeof (NwGtpv2cStackT));

    if (thiz) {
      OAI_GCC_DIAG_OFF(pointer-to-int-cast);
      thiz->id = (uint32_t) thiz;
      thiz->seqNum = ((uint32_t) thiz) & 0x0000FFFF;
      OAI_GCC_DIAG_ON(pointer-to-int-cast);
      RB_INIT (&(thiz->tunnelMap));
      RB_INIT (&(thiz->outstandingTxSeqNumMap));
      RB_INIT (&(thiz->outstandingRxSeqNumMap));
      RB_INIT (&(thiz->activeTimerList));
      OAI_GCC_DIAG_OFF(pointer-to-int-cast);
      thiz->hTmrMinHeap = (NwHandleT) nwGtpv2cTmrMinHeapNew (10000);
      OAI_GCC_DIAG_ON(pointer-to-int-cast);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_ECHO_RSP);
      /*
       * For S11 interface
       */
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_CREATE_SESSION_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_CREATE_SESSION_RSP);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_DELETE_SESSION_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_DELETE_SESSION_RSP);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_MODIFY_BEARER_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_MODIFY_BEARER_RSP);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_RELEASE_ACCESS_BEARERS_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_RELEASE_ACCESS_BEARERS_RSP);
      /*
       * For S10 interface
       */
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_FORWARD_RELOCATION_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_FORWARD_RELOCATION_RSP);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_FORWARD_RELOCATION_COMPLETE_NTF);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_FORWARD_RELOCATION_COMPLETE_ACK);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_CONTEXT_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_CONTEXT_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_CONTEXT_ACK);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_IDENTIFICATION_REQ);
      NW_GTPV2C_INIT_MSG_IE_PARSE_INFO (thiz, NW_GTP_IDENTIFICATION_RSP);
      nwGtpv2cDisplayBanner (thiz);
    } else {
      rc = NW_FAILURE;
    }

    *hGtpcStackHandle = (NwGtpv2cStackHandleT) thiz;
    return rc;
  }


/**
   Destructor
*/

  NwRcT                                   nwGtpv2cFinalize (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle) {
    if (!hGtpcStackHandle)
      return NW_FAILURE;

    free_wrapper ((void **) &hGtpcStackHandle);
    return NW_OK;
  }


/**
   Set ULP entity
*/

  NwRcT                                   nwGtpv2cSetUlpEntity (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN NwGtpv2cUlpEntityT * pUlpEntity) {
    NwGtpv2cStackT                         *thiz = (NwGtpv2cStackT *) hGtpcStackHandle;

    if (!pUlpEntity)
      return NW_FAILURE;

    thiz->ulp = *(pUlpEntity);
    return NW_OK;
  }

/**
   Set UDP entity
*/

  NwRcT                                   nwGtpv2cSetUdpEntity (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN NwGtpv2cUdpEntityT * pUdpEntity) {
    NwGtpv2cStackT                         *thiz = (NwGtpv2cStackT *) hGtpcStackHandle;

    if (!pUdpEntity)
      return NW_FAILURE;

    thiz->udp = *(pUdpEntity);
    return NW_OK;
  }

/**
   Set MEM MGR entity
*/

  NwRcT                                   nwGtpv2cSetMemMgrEntity (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN NwGtpv2cMemMgrEntityT * pMemMgrEntity) {
    NwGtpv2cStackT                         *thiz = (NwGtpv2cStackT *) hGtpcStackHandle;

    if (!pMemMgrEntity)
      return NW_FAILURE;

    thiz->memMgr = *(pMemMgrEntity);
    return NW_OK;
  }

/**
   Set TMR MGR entity
*/

  NwRcT                                   nwGtpv2cSetTimerMgrEntity (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN NwGtpv2cTimerMgrEntityT * pTmrMgrEntity) {
    NwGtpv2cStackT                         *thiz = (NwGtpv2cStackT *) hGtpcStackHandle;

    if (!pTmrMgrEntity)
      return NW_FAILURE;

    thiz->tmrMgr = *(pTmrMgrEntity);
    return NW_OK;
  }


/**
   Set LOG MGR entity
*/

  NwRcT                                   nwGtpv2cSetLogMgrEntity (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN NwGtpv2cLogMgrEntityT * pLogMgrEntity) {
    NwGtpv2cStackT                         *thiz = (NwGtpv2cStackT *) hGtpcStackHandle;

    if (!pLogMgrEntity)
      return NW_FAILURE;

    thiz->logMgr = *(pLogMgrEntity);
    return NW_OK;
  }

/**
  Set log level for the stack.
*/

  NwRcT                                   nwGtpv2cSetLogLevel (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN uint32_t logLevel) {
    NwGtpv2cStackT                         *thiz = (NwGtpv2cStackT *) hGtpcStackHandle;

    thiz->logLevel = logLevel;
    return NW_OK;
  }

/**
   Process Request from Udp Layer
*/

  NwRcT                                   nwGtpv2cProcessUdpReq (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN uint8_t * udpData,
  NW_IN uint32_t udpDataLen,
  NW_IN uint16_t peerPort,
  NW_IN uint32_t peerIp) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cStackT                         *thiz = NULL;
    uint16_t                                msgType = 0;

    thiz = (NwGtpv2cStackT *) hGtpcStackHandle;
    NW_ASSERT (thiz);
    OAILOG_FUNC_IN (LOG_GTPV2C);

    if (udpDataLen < NW_GTPV2C_MINIMUM_HEADER_SIZE) {
      /*
       * TS 29.274 Section 7.7.3:
       * If a GTP entity receives a message, which is too short to
       * contain the respective GTPv2 header, the GTP-PDU shall be
       * silently discarded
       */
      OAILOG_WARNING (LOG_GTPV2C,  "Received message too small! Discarding.\n");
      OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
    }

    if ((ntohs (*((uint16_t *) ((uint8_t *) udpData + 2)))      /* Length */
         +((*((uint8_t *) (udpData)) & 0x08) ? 4 : 0) /* Extra Header length if TEID present */ ) > udpDataLen) {
      OAILOG_WARNING (LOG_GTPV2C,  "Received message with erroneous length of %u against expected length of %u! Discarding\n", udpDataLen, ntohs (*((uint16_t *) ((uint8_t *) udpData + 2))) + ((*((uint8_t *) (udpData)) & 0x08) ? 4 : 0));
      OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
    }

    if (((*((uint8_t *) (udpData)) & 0xE0) >> 5) != NW_GTP_VERSION) {
      OAILOG_WARNING (LOG_GTPV2C,  "Received unsupported GTP version '%u' message! Discarding.\n", ((*((uint8_t *) (udpData)) & 0xE0) >> 5));
      /*
       * Send Version Not Supported Message to peer
       */
      rc = nwGtpv2cSendVersionNotSupportedInd (thiz, peerIp, peerPort, *((uint32_t *) (udpData + ((*((uint8_t *) (udpData)) & 0x08) ? 8 : 4))) /* Seq Num */ );
      OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
    }

    msgType = *((uint8_t *) (udpData + 1));

    switch (msgType) {
    case NW_GTP_ECHO_REQ:{
        rc = nwGtpv2cHandleEchoReq (thiz, msgType, udpData, udpDataLen, peerPort, peerIp);
      }
      break;

    case NW_GTP_CREATE_SESSION_REQ:
    case NW_GTP_MODIFY_BEARER_REQ:
    case NW_GTP_DELETE_SESSION_REQ:
    case NW_GTP_CREATE_BEARER_REQ:
    case NW_GTP_UPDATE_BEARER_REQ:
    case NW_GTP_DELETE_BEARER_REQ:
    case NW_GTP_RELEASE_ACCESS_BEARERS_REQ:
    case NW_GTP_CREATE_INDIRECT_DATA_FORWARDING_TUNNEL_REQ:
    case NW_GTP_DELETE_INDIRECT_DATA_FORWARDING_TUNNEL_REQ:{
        rc = nwGtpv2cHandleInitialReq (thiz, msgType, udpData, udpDataLen, peerPort, peerIp);
      }
      break;

    case NW_GTP_ECHO_RSP:
    case NW_GTP_CREATE_SESSION_RSP:
    case NW_GTP_MODIFY_BEARER_RSP:
    case NW_GTP_DELETE_SESSION_RSP:
    case NW_GTP_CREATE_BEARER_RSP:
    case NW_GTP_UPDATE_BEARER_RSP:
    case NW_GTP_DELETE_BEARER_RSP:
    case NW_GTP_RELEASE_ACCESS_BEARERS_RSP:
    case NW_GTP_CREATE_INDIRECT_DATA_FORWARDING_TUNNEL_RSP:
    case NW_GTP_DELETE_INDIRECT_DATA_FORWARDING_TUNNEL_RSP: {
        rc = nwGtpv2cHandleTriggeredRsp (thiz, msgType, udpData, udpDataLen, peerPort, peerIp);
      }
      break;

    default:{
        /*
         * TS 29.274 Section 7.7.4:
         * If a GTP entity receives a message with an unknown Message Type
         * value, it shall silently discard the message.
         */
        OAILOG_WARNING (LOG_GTPV2C,  "Received unknown message type %u from UDP! Ignoring.\n", msgType);
        rc = NW_OK;
      }
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }


/*
   Process Request from Upper Layer
*/

  NwRcT                                   nwGtpv2cProcessUlpReq (
  NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
  NW_IN NwGtpv2cUlpApiT * pUlpReq) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cStackT                         *thiz = (NwGtpv2cStackT *) hGtpcStackHandle;

    NW_ASSERT (thiz);
    NW_ASSERT (pUlpReq != NULL);
    OAILOG_FUNC_IN (LOG_GTPV2C);

    switch (pUlpReq->apiType & 0x00FFFFFFL) {
    case NW_GTPV2C_ULP_API_INITIAL_REQ:{
        OAILOG_DEBUG (LOG_GTPV2C, "Received initial request from ulp\n");
        rc = nwGtpv2cHandleUlpInitialReq (thiz, pUlpReq);
      }
      break;

    case NW_GTPV2C_ULP_API_TRIGGERED_REQ:{
        OAILOG_DEBUG (LOG_GTPV2C, "Received triggered request from ulp\n");
        rc = nwGtpv2cHandleUlpTriggeredReq (thiz, pUlpReq);
      }
      break;

    case NW_GTPV2C_ULP_API_TRIGGERED_RSP:{
        OAILOG_DEBUG (LOG_GTPV2C, "Received triggered response from ulp\n");
        rc = nwGtpv2cHandleUlpTriggeredRsp (thiz, pUlpReq);
      }
      break;

    case NW_GTPV2C_ULP_CREATE_LOCAL_TUNNEL:{
        OAILOG_DEBUG (LOG_GTPV2C, "Received create local tunnel from ulp\n");
        rc = nwGtpv2cHandleUlpCreateLocalTunnel (thiz, pUlpReq);
      }
      break;

    case NW_GTPV2C_ULP_DELETE_LOCAL_TUNNEL:{
        OAILOG_DEBUG (LOG_GTPV2C, "Received delete local tunnel from ulp\n");
        rc = nwGtpv2cHandleUlpDeleteLocalTunnel (thiz, pUlpReq);
      }
      break;

    default:{
        OAILOG_WARNING (LOG_GTPV2C,  "Received unhandled API 0x%x from ULP! Ignoring.\n", pUlpReq->apiType);
        rc = NW_FAILURE;
      }
      break;
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
   Process Timer timeout Request from Timer ULP Manager
*/

  NwRcT                                   nwGtpv2cProcessTimeoutOld (
  void *arg) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cStackT                         *thiz = NULL;
    NwGtpv2cTimeoutInfoT                   *timeoutInfo = (NwGtpv2cTimeoutInfoT *) arg;
    NwGtpv2cTimeoutInfoT                   *pNextTimeoutInfo = NULL;
    struct timeval                          tv = {0};

    NW_ASSERT (timeoutInfo != NULL);
    thiz = (NwGtpv2cStackT *) (((NwGtpv2cTimeoutInfoT *) timeoutInfo)->hStack);
    NW_ASSERT (thiz != NULL);
    OAILOG_FUNC_IN (LOG_GTPV2C);

    if (thiz->activeTimerInfo == timeoutInfo) {
      thiz->activeTimerInfo = NULL;
      RB_REMOVE (NwGtpv2cActiveTimerList, &(thiz->activeTimerList), timeoutInfo);
      timeoutInfo->next = gpGtpv2cTimeoutInfoPool;
      gpGtpv2cTimeoutInfoPool = timeoutInfo;
      rc = ((timeoutInfo)->timeoutCallbackFunc) (timeoutInfo->timeoutArg);
    } else {
      OAILOG_WARNING (LOG_GTPV2C,  "Received timeout event from ULP for non-existent timeoutInfo 0x%p and activeTimer 0x%p!\n", timeoutInfo, thiz->activeTimerInfo);
      OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
    }

    NW_ASSERT (gettimeofday (&tv, NULL) == 0);

    for ((timeoutInfo) = RB_MIN (NwGtpv2cActiveTimerList, &(thiz->activeTimerList)); (timeoutInfo) != NULL;) {
      if (NW_GTPV2C_TIMER_CMP_P (&timeoutInfo->tvTimeout, &tv, >))
        break;

      pNextTimeoutInfo = RB_NEXT (NwGtpv2cActiveTimerList, &(thiz->activeTimerList), timeoutInfo);
      RB_REMOVE (NwGtpv2cActiveTimerList, &(thiz->activeTimerList), timeoutInfo);
      timeoutInfo->next = gpGtpv2cTimeoutInfoPool;
      gpGtpv2cTimeoutInfoPool = timeoutInfo;
      rc = ((timeoutInfo)->timeoutCallbackFunc) (timeoutInfo->timeoutArg);
      timeoutInfo = pNextTimeoutInfo;
    }

    /*
     * activeTimerInfo may be reset by the timeoutCallbackFunc call above
     */
    if (thiz->activeTimerInfo == NULL) {
      timeoutInfo = RB_MIN (NwGtpv2cActiveTimerList, &(thiz->activeTimerList));

      if (timeoutInfo) {
        NW_GTPV2C_TIMER_SUB (&timeoutInfo->tvTimeout, &tv, &tv);
        rc = thiz->tmrMgr.tmrStartCallback (thiz->tmrMgr.tmrMgrHandle, tv.tv_sec, tv.tv_usec, timeoutInfo->tmrType, (void *)timeoutInfo, &timeoutInfo->hTimer);
        NW_ASSERT (NW_OK == rc);
        thiz->activeTimerInfo = timeoutInfo;
      }
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

  NwRcT                                   nwGtpv2cProcessTimeout (
  void *arg) {
    NwRcT                                   rc = NW_FAILURE;
    NwGtpv2cStackT                         *thiz = NULL;
    NwGtpv2cTimeoutInfoT                   *timeoutInfo = (NwGtpv2cTimeoutInfoT *) arg;
    struct timeval                          tv = {0};

    NW_ASSERT (timeoutInfo != NULL);
    thiz = (NwGtpv2cStackT *) (timeoutInfo->hStack);
    NW_ASSERT (thiz != NULL);
    OAILOG_FUNC_IN (LOG_GTPV2C);

    if (thiz->activeTimerInfo == timeoutInfo) {
      thiz->activeTimerInfo = NULL;
      OAI_GCC_DIAG_OFF(int-to-pointer-cast);
      rc = nwGtpv2cTmrMinHeapRemove ((NwGtpv2cTmrMinHeapT*)thiz->hTmrMinHeap, timeoutInfo->timerMinHeapIndex);
      OAI_GCC_DIAG_ON(int-to-pointer-cast);
      timeoutInfo->next = gpGtpv2cTimeoutInfoPool;
      gpGtpv2cTimeoutInfoPool = timeoutInfo;
      rc = ((timeoutInfo)->timeoutCallbackFunc) (timeoutInfo->timeoutArg);
    } else {
      OAILOG_WARNING (LOG_GTPV2C,  "Received timeout event from ULP for " "non-existent timeoutInfo 0x%p and activeTimer 0x%p!\n", timeoutInfo, thiz->activeTimerInfo);
      OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
    }

    NW_ASSERT (gettimeofday (&tv, NULL) == 0);
    //printf("------ Start -------\n");
    OAI_GCC_DIAG_OFF(int-to-pointer-cast);
    timeoutInfo = nwGtpv2cTmrMinHeapPeek ((NwGtpv2cTmrMinHeapT *)thiz->hTmrMinHeap);
    OAI_GCC_DIAG_ON(int-to-pointer-cast);

    while ((timeoutInfo) != NULL) {
      if (NW_GTPV2C_TIMER_CMP_P (&timeoutInfo->tvTimeout, &tv, >))
        break;

      OAI_GCC_DIAG_OFF(int-to-pointer-cast);
      rc = nwGtpv2cTmrMinHeapRemove ((NwGtpv2cTmrMinHeapT *)thiz->hTmrMinHeap, timeoutInfo->timerMinHeapIndex);
      OAI_GCC_DIAG_ON(int-to-pointer-cast);
      timeoutInfo->next = gpGtpv2cTimeoutInfoPool;
      gpGtpv2cTimeoutInfoPool = timeoutInfo;
      rc = ((timeoutInfo)->timeoutCallbackFunc) (timeoutInfo->timeoutArg);
      OAI_GCC_DIAG_OFF(int-to-pointer-cast);
      timeoutInfo = nwGtpv2cTmrMinHeapPeek ((NwGtpv2cTmrMinHeapT *)thiz->hTmrMinHeap);
      OAI_GCC_DIAG_ON(int-to-pointer-cast);
    }

    /*
     * activeTimerInfo may be reset by the timeoutCallbackFunc call above
     */
    if (thiz->activeTimerInfo == NULL) {
      OAI_GCC_DIAG_OFF(int-to-pointer-cast);
      timeoutInfo = nwGtpv2cTmrMinHeapPeek ((NwGtpv2cTmrMinHeapT *)thiz->hTmrMinHeap);
      OAI_GCC_DIAG_ON(int-to-pointer-cast);

      if (timeoutInfo) {
        NW_GTPV2C_TIMER_SUB (&timeoutInfo->tvTimeout, &tv, &tv);
        rc = thiz->tmrMgr.tmrStartCallback (thiz->tmrMgr.tmrMgrHandle, tv.tv_sec, tv.tv_usec, timeoutInfo->tmrType, (void *)timeoutInfo, &timeoutInfo->hTimer);
        NW_ASSERT (NW_OK == rc);
        thiz->activeTimerInfo = timeoutInfo;
      }
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
   Start Timer with ULP Timer Manager
*/





  NwRcT                                   nwGtpv2cStartTimer (
  NwGtpv2cStackT * thiz,
  uint32_t timeoutSec,
  uint32_t timeoutUsec,
  uint32_t tmrType,
  NwRcT                                   (*timeoutCallbackFunc) (void *),
  void *timeoutCallbackArg,
  NwGtpv2cTimerHandleT * phTimer) {
    NwRcT                                   rc = NW_OK;
    struct timeval                          tv = { 0 };
    NwGtpv2cTimeoutInfoT                   *timeoutInfo = NULL;

    OAILOG_FUNC_IN (LOG_GTPV2C);

    if (gpGtpv2cTimeoutInfoPool) {
      timeoutInfo = gpGtpv2cTimeoutInfoPool;
      gpGtpv2cTimeoutInfoPool = gpGtpv2cTimeoutInfoPool->next;
    } else {
      NW_GTPV2C_MALLOC (thiz, sizeof (NwGtpv2cTimeoutInfoT), timeoutInfo, NwGtpv2cTimeoutInfoT *);
    }

    if (timeoutInfo) {
      timeoutInfo->tmrType = tmrType;
      timeoutInfo->timeoutArg = timeoutCallbackArg;
      timeoutInfo->timeoutCallbackFunc = timeoutCallbackFunc;
      timeoutInfo->hStack = (NwGtpv2cStackHandleT) thiz;
      NW_ASSERT (gettimeofday (&tv, NULL) == 0);
      NW_ASSERT (gettimeofday (&timeoutInfo->tvTimeout, NULL) == 0);
      timeoutInfo->tvTimeout.tv_sec = timeoutSec;
      timeoutInfo->tvTimeout.tv_usec = timeoutUsec;
      NW_GTPV2C_TIMER_ADD (&tv, &timeoutInfo->tvTimeout, &timeoutInfo->tvTimeout);
      OAI_GCC_DIAG_OFF(int-to-pointer-cast);
      rc = nwGtpv2cTmrMinHeapInsert ((NwGtpv2cTmrMinHeapT *)thiz->hTmrMinHeap, timeoutInfo);
      OAI_GCC_DIAG_ON(int-to-pointer-cast);
#if 0

      do {
        collision = RB_INSERT (NwGtpv2cActiveTimerList, &(thiz->activeTimerList), timeoutInfo);

        if (!collision)
          break;

        OAILOG_WARNING (LOG_GTPV2C,  "timer collision!\n");
        timeoutInfo->tvTimeout.tv_usec++;       /* HACK: In case there is a collision, schedule this event 1 usec later */

        if (timeoutInfo->tvTimeout.tv_usec > (999999 /*1000000 - 1 */ )) {
          timeoutInfo->tvTimeout.tv_usec = 0;
          timeoutInfo->tvTimeout.tv_sec++;
        }
      } while (1);

#endif

      if (thiz->activeTimerInfo) {
        if (NW_GTPV2C_TIMER_CMP_P (&(thiz->activeTimerInfo->tvTimeout), &(timeoutInfo->tvTimeout), >)) {
          OAILOG_DEBUG (LOG_GTPV2C, "Stopping active timer 0x%" PRIxPTR " for info 0x%p!\n", thiz->activeTimerInfo->hTimer, thiz->activeTimerInfo);
          rc = thiz->tmrMgr.tmrStopCallback (thiz->tmrMgr.tmrMgrHandle, thiz->activeTimerInfo->hTimer);
          NW_ASSERT (NW_OK == rc);
        } else {
          OAILOG_DEBUG (LOG_GTPV2C, "Already Started timer 0x%" PRIxPTR " for info 0x%p!\n", thiz->activeTimerInfo->hTimer, thiz->activeTimerInfo);
          *phTimer = (NwGtpv2cTimerHandleT) timeoutInfo;
          OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
        }
      }

      rc = thiz->tmrMgr.tmrStartCallback (thiz->tmrMgr.tmrMgrHandle, timeoutSec, timeoutUsec, tmrType, (void *)timeoutInfo, &timeoutInfo->hTimer);
      OAILOG_DEBUG (LOG_GTPV2C, "Started timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);
      NW_ASSERT (NW_OK == rc);
      thiz->activeTimerInfo = timeoutInfo;
    }

    *phTimer = (NwGtpv2cTimerHandleT) timeoutInfo;
    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

  NwRcT                                   nwGtpv2cStartTimerOld (
  NwGtpv2cStackT * thiz,
  uint32_t timeoutSec,
  uint32_t timeoutUsec,
  uint32_t tmrType,
  NwRcT                                   (*timeoutCallbackFunc) (void *),
  void *timeoutCallbackArg,
  NwGtpv2cTimerHandleT * phTimer) {
    NwRcT                                   rc = NW_OK;
    struct timeval                          tv;
    NwGtpv2cTimeoutInfoT                   *timeoutInfo;
    NwGtpv2cTimeoutInfoT                   *collision;

    NW_ASSERT (thiz != NULL);
    OAILOG_FUNC_IN (LOG_GTPV2C);

    if (gpGtpv2cTimeoutInfoPool) {
      timeoutInfo = gpGtpv2cTimeoutInfoPool;
      gpGtpv2cTimeoutInfoPool = gpGtpv2cTimeoutInfoPool->next;
    } else {
      NW_GTPV2C_MALLOC (thiz, sizeof (NwGtpv2cTimeoutInfoT), timeoutInfo, NwGtpv2cTimeoutInfoT *);
    }

    if (timeoutInfo) {
      timeoutInfo->tmrType = tmrType;
      timeoutInfo->timeoutArg = timeoutCallbackArg;
      timeoutInfo->timeoutCallbackFunc = timeoutCallbackFunc;
      timeoutInfo->hStack = (NwGtpv2cStackHandleT) thiz;
      NW_ASSERT (gettimeofday (&tv, NULL) == 0);
      NW_ASSERT (gettimeofday (&timeoutInfo->tvTimeout, NULL) == 0);
      timeoutInfo->tvTimeout.tv_sec = timeoutSec;
      timeoutInfo->tvTimeout.tv_usec = timeoutUsec;
      NW_GTPV2C_TIMER_ADD (&tv, &timeoutInfo->tvTimeout, &timeoutInfo->tvTimeout);

      do {
        collision = RB_INSERT (NwGtpv2cActiveTimerList, &(thiz->activeTimerList), timeoutInfo);

        if (!collision)
          break;

        OAILOG_WARNING (LOG_GTPV2C,  "timer collision!\n");
        timeoutInfo->tvTimeout.tv_usec++;       /* HACK: In case there is a collision, schedule this event 1 usec later */

        if (timeoutInfo->tvTimeout.tv_usec > (999999 /*1000000 - 1 */ )) {
          timeoutInfo->tvTimeout.tv_usec = 0;
          timeoutInfo->tvTimeout.tv_sec++;
        }
      } while (1);

      if (thiz->activeTimerInfo) {
        if (NW_GTPV2C_TIMER_CMP_P (&(thiz->activeTimerInfo->tvTimeout), &(timeoutInfo->tvTimeout), >)) {
          OAILOG_DEBUG (LOG_GTPV2C, "Stopping active timer 0x%" PRIxPTR " for info 0x%p!\n", thiz->activeTimerInfo->hTimer, thiz->activeTimerInfo);
          rc = thiz->tmrMgr.tmrStopCallback (thiz->tmrMgr.tmrMgrHandle, thiz->activeTimerInfo->hTimer);
          NW_ASSERT (NW_OK == rc);
        } else {
          OAILOG_DEBUG (LOG_GTPV2C, "Already Started timer 0x%" PRIxPTR " for info 0x%p!\n", thiz->activeTimerInfo->hTimer, thiz->activeTimerInfo);
          *phTimer = (NwGtpv2cTimerHandleT) timeoutInfo;
          OAILOG_FUNC_RETURN (LOG_GTPV2C, NW_OK);
        }
      }

      rc = thiz->tmrMgr.tmrStartCallback (thiz->tmrMgr.tmrMgrHandle, timeoutSec, timeoutUsec, tmrType, (void *)timeoutInfo, &timeoutInfo->hTimer);
      OAILOG_DEBUG (LOG_GTPV2C, "Started timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);
      NW_ASSERT (NW_OK == rc);
      thiz->activeTimerInfo = timeoutInfo;
    }

    *phTimer = (NwGtpv2cTimerHandleT) timeoutInfo;
    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

/**
   Stop Timer with ULP Timer Manager
*/
  NwRcT                                   nwGtpv2cStopTimer (
  NwGtpv2cStackT * thiz,
  NwGtpv2cTimerHandleT hTimer) {
    NwRcT                                   rc = NW_OK;
    struct timeval                          tv;
    NwGtpv2cTimeoutInfoT                   *timeoutInfo;

    NW_ASSERT (thiz != NULL);
    OAILOG_FUNC_IN (LOG_GTPV2C);
    timeoutInfo = (NwGtpv2cTimeoutInfoT *) hTimer;
    OAI_GCC_DIAG_OFF(int-to-pointer-cast);
    rc = nwGtpv2cTmrMinHeapRemove ((NwGtpv2cTmrMinHeapT *)thiz->hTmrMinHeap, timeoutInfo->timerMinHeapIndex);
    OAI_GCC_DIAG_ON(int-to-pointer-cast);
    timeoutInfo->next = gpGtpv2cTimeoutInfoPool;
    gpGtpv2cTimeoutInfoPool = timeoutInfo;
    OAILOG_DEBUG (LOG_GTPV2C, "Stopping active timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);

    if (thiz->activeTimerInfo == timeoutInfo) {
      OAILOG_DEBUG (LOG_GTPV2C, "Stopping active timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);
      rc = thiz->tmrMgr.tmrStopCallback (thiz->tmrMgr.tmrMgrHandle, timeoutInfo->hTimer);
      thiz->activeTimerInfo = NULL;
      NW_ASSERT (NW_OK == rc);
      OAI_GCC_DIAG_OFF(int-to-pointer-cast);
      timeoutInfo = nwGtpv2cTmrMinHeapPeek ((NwGtpv2cTmrMinHeapT *)thiz->hTmrMinHeap);
      OAI_GCC_DIAG_ON(int-to-pointer-cast);

      if (timeoutInfo) {
        NW_ASSERT (gettimeofday (&tv, NULL) == 0);

        if (NW_GTPV2C_TIMER_CMP_P (&timeoutInfo->tvTimeout, &tv, <)) {
          thiz->activeTimerInfo = timeoutInfo;
          rc = nwGtpv2cProcessTimeout (timeoutInfo);
          NW_ASSERT (NW_OK == rc);
        } else {
          NW_GTPV2C_TIMER_SUB (&timeoutInfo->tvTimeout, &tv, &tv);
          rc = thiz->tmrMgr.tmrStartCallback (thiz->tmrMgr.tmrMgrHandle, tv.tv_sec, tv.tv_usec, timeoutInfo->tmrType, (void *)timeoutInfo, &timeoutInfo->hTimer);
          NW_ASSERT (NW_OK == rc);
          OAILOG_DEBUG (LOG_GTPV2C, "Started timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);
          thiz->activeTimerInfo = timeoutInfo;
        }
      }
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

  NwRcT                                   nwGtpv2cStopTimerOld (
  NwGtpv2cStackT * thiz,
  NwGtpv2cTimerHandleT hTimer) {
    NwRcT                                   rc = NW_OK;
    struct timeval                          tv;
    NwGtpv2cTimeoutInfoT                   *timeoutInfo;

    NW_ASSERT (thiz != NULL);
    OAILOG_FUNC_IN (LOG_GTPV2C);
    timeoutInfo = (NwGtpv2cTimeoutInfoT *) hTimer;
    RB_REMOVE (NwGtpv2cActiveTimerList, &(thiz->activeTimerList), timeoutInfo);
    timeoutInfo->next = gpGtpv2cTimeoutInfoPool;
    gpGtpv2cTimeoutInfoPool = timeoutInfo;
    OAILOG_DEBUG (LOG_GTPV2C, "Stopping active timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);

    if (thiz->activeTimerInfo == timeoutInfo) {
      OAILOG_DEBUG (LOG_GTPV2C, "Stopping active timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);
      rc = thiz->tmrMgr.tmrStopCallback (thiz->tmrMgr.tmrMgrHandle, timeoutInfo->hTimer);
      thiz->activeTimerInfo = NULL;
      NW_ASSERT (NW_OK == rc);
      timeoutInfo = RB_MIN (NwGtpv2cActiveTimerList, &(thiz->activeTimerList));

      if (timeoutInfo) {
        NW_ASSERT (gettimeofday (&tv, NULL) == 0);

        if (NW_GTPV2C_TIMER_CMP_P (&timeoutInfo->tvTimeout, &tv, <)) {
          thiz->activeTimerInfo = timeoutInfo;
          rc = nwGtpv2cProcessTimeout (timeoutInfo);
          NW_ASSERT (NW_OK == rc);
        } else {
          NW_GTPV2C_TIMER_SUB (&timeoutInfo->tvTimeout, &tv, &tv);
          rc = thiz->tmrMgr.tmrStartCallback (thiz->tmrMgr.tmrMgrHandle, tv.tv_sec, tv.tv_usec, timeoutInfo->tmrType, (void *)timeoutInfo, &timeoutInfo->hTimer);
          NW_ASSERT (NW_OK == rc);
          OAILOG_DEBUG (LOG_GTPV2C, "Started timer 0x%" PRIxPTR " for info 0x%p!\n", timeoutInfo->hTimer, timeoutInfo);
          thiz->activeTimerInfo = timeoutInfo;
        }
      }
    }

    OAILOG_FUNC_RETURN (LOG_GTPV2C, rc);
  }

#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*
                        E N D     O F    F I L E
  --------------------------------------------------------------------------*/
