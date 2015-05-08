/**
 * @file    ClientApi.cpp
 * Definitions for class and functions used by the Client API.
 *
 * @date    03/25/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "ClientApi.h"
#include "ClientShared.h"
//#include "MainMgr.h"
//#include "MainMgrDefs.h"
#include "cencode.h"
#include "cdecode.h"
#include "base64_wrapper.h"
#include "LogHelper.h"
#include "msg_utils.h"
#include "fwLdr.h"
#include "comm.h"

#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/lockfree/queue.hpp>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_API );

/* Private typedefs ----------------------------------------------------------*/
///* storage for event pools... */
//static union SmallEvents {
//    void   *e0;                                       /* minimum event size */
//    uint8_t e1[sizeof(QEvent)];
//    uint8_t e2[sizeof(ExitEvt)];
//} l_smlPoolSto[10];                     /* storage for the small event pool */
//
//static union MediumEvents {
//    void   *e0;                                       /* minimum event size */
//    uint8_t *e1[sizeof(LrgDataEvt)];
//} l_medPoolSto[15];                    /* storage for the medium event pool */

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
//static QEvt const *l_MainMgrQueueSto[10];
//static QEvt const *l_cliQueueSto[10]; /**< Storage for raw QE queue for
//                                                 communicating with ClientApi */
//static QSubscrList l_subscrSto[MAX_PUB_SIG];

static Comm *l_pComm;

boost::lockfree::queue<MsgData_t> queue(128);



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void runMainMgr( void )
{
//   /* start the active objects... */
//   QActive_start(
//         AO_MainMgr,
//         MAIN_MGR_PRIORITY,
//         l_MainMgrQueueSto,
//         Q_DIM(l_MainMgrQueueSto),
//         (void *)0,
//         0,
//         (QEvent *)0,
//         "MainMgr"
//   );
//   QF_run();                              /* run the QF application */
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******************************************************************************/
ClientError_t ClientApi::DC3_getMode(CBErrorCode *status, CBBootMode *mode)
{
   this->enableMsgCallbacks();

   /* Common settings for most messages */
   this->m_basicMsg._msgID       = this->m_msgId;
   this->m_basicMsg._msgReqProg  = (unsigned long)this->m_bRequestProg;
   this->m_basicMsg._msgRoute    = this->m_msgRoute;

   /* Settings specific to this message */
   this->m_basicMsg._msgType     = _CB_Req;
   this->m_basicMsg._msgName     = _CBGetBootModeMsg;
   this->m_basicMsg._msgPayload  = _CBNoMsg;

//   char *buffer = new char(CB_MAX_MSG_LEN);
   uint8_t buffer[300];
   unsigned int bufferLen = 0;
   bufferLen = CBBasicMsg_write_delimited_to(&m_basicMsg, buffer, 0);
   l_pComm->write_some((char *)buffer, bufferLen);

   /* Wait for Ack */
   CBBasicMsg basicMsg;
   CBPayloadMsgUnion_t payloadMsgUnion;
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
   ClientError_t clientStatus = waitForResp(
         &basicMsg,
         &payloadMsgUnion,
         5
   );

//   LrgDataEvt *evt = Q_NEW(LrgDataEvt, MSG_SEND_OUT_SIG);
//   evt->dataLen = CBBasicMsg_write_delimited_to(&m_basicMsg, evt->dataBuf, 0);
//   evt->dst = m_msgRoute;
//   evt->src = m_msgRoute;
//   QACTIVE_POST(AO_MainMgr, (QEvt *)(evt), AO_MainMgr);

//   char *tmp = (char *)malloc(sizeof(char) * 1000);
//   uint16_t tmpStrLen;
//   ClientError_t convertStatus = MSG_hexToStr(
//         evt->dataBuf,
//         evt->dataLen,
//         tmp,
//         1000,
//         &tmpStrLen,
//         0,
//         '-',
//         true
//   );
//
//   DBG_printf(m_pLog,"ConStatus: 0x%x, sending a buffer with: %s", convertStatus, tmp);
//   free(tmp);

   /* Wait for Done msg */
   clientStatus = waitForResp(
         &basicMsg,
         &payloadMsgUnion,
         5
   );

   if ( CLI_ERR_NONE == clientStatus ) {
      *status = (CBErrorCode)payloadMsgUnion.bootmodePayload._errorCode;
      *mode = payloadMsgUnion.bootmodePayload._bootMode;
   }

   return clientStatus;
}

/******************************************************************************/
ClientError_t ClientApi::DC3_flashFW(
      CBErrorCode *status,
      CBBootMode type,
      const char* filename
)
{
   this->disableMsgCallbacks(); /* There are too many msgs flying about for us
   to log all of them so just turn this off */

   ClientError_t clientStatus = CLI_ERR_NONE;
   FWLdr *fw = NULL;
   try {
      fw = new FWLdr( m_pLog );
   } catch ( exception &e ) {
      ERR_printf(
            m_pLog,"Exception trying to get a FWLdr object: %s",
            e.what()
      );
   }

   /* Read the file */
   clientStatus = fw->loadFromFile(filename);

   /* CBFlashMsg is a little different than other messages:
    * 1. must request progress msgs.
    * 2. msgIds must stay sequential */


   uint8_t chunkSize = 112; /* This is the safest amount of data to send */

   /* Common settings for most messages */
   this->m_basicMsg._msgID       = this->m_msgId;
   this->m_basicMsg._msgReqProg  = 0;
   this->m_basicMsg._msgRoute    = this->m_msgRoute;

   /* Settings specific to this message */
   this->m_basicMsg._msgType     = _CB_Req;
   this->m_basicMsg._msgName     = _CBFlashMsg;
   this->m_basicMsg._msgPayload  = _CBFlashMetaPayloadMsg;

   /* Construct the flashMetaPayloadMsg that tells the bootloader about all
    * the data to expect. */
   this->m_flashMetaPayloadMsg._imageType = type;
   this->m_flashMetaPayloadMsg._imageCrc = fw->getImageCRC32();
   this->m_flashMetaPayloadMsg._imageMaj = fw->getMajVer();
   this->m_flashMetaPayloadMsg._imageMin = fw->getMinVer();
   this->m_flashMetaPayloadMsg._imageSize = fw->getSize();
   this->m_flashMetaPayloadMsg._imageNumPackets = fw->calcNumberOfPackets( chunkSize );
   this->m_flashMetaPayloadMsg._imageDatetime_len = fw->getDatetimeLen();
   memcpy(
         this->m_flashMetaPayloadMsg._imageDatetime,
         fw->getDatetime(),
         this->m_flashMetaPayloadMsg._imageDatetime_len
   );

   LOG_printf(m_pLog, "Starting FW flash of DC3");
   LOG_printf(m_pLog, "FW image built on: %s", this->m_flashMetaPayloadMsg._imageDatetime);
   LOG_printf(m_pLog, "FW image CRC is: 0x%08x", this->m_flashMetaPayloadMsg._imageCrc);
   LOG_printf(m_pLog, "FW image size is: %d", this->m_flashMetaPayloadMsg._imageSize);

//   LrgDataEvt *evt = Q_NEW(LrgDataEvt, MSG_SEND_OUT_SIG);
//   evt->dataLen = CBBasicMsg_write_delimited_to(&m_basicMsg, evt->dataBuf, 0);
//   evt->dataLen = CBFlashMetaPayloadMsg_write_delimited_to(&m_flashMetaPayloadMsg, evt->dataBuf, evt->dataLen);
//   evt->dst = m_msgRoute;
//   evt->src = m_msgRoute;
//   QACTIVE_POST(AO_MainMgr, (QEvt *)(evt), AO_MainMgr);


   CBBasicMsg basicMsg;
   CBPayloadMsgUnion_t payloadMsgUnion;
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));

   clientStatus = waitForResp(
         &basicMsg,
         &payloadMsgUnion,
         5
   );

   if ( CLI_ERR_NONE == clientStatus ) {
      *status = (CBErrorCode)payloadMsgUnion.statusPayload._errorCode;
      if ( ERR_NONE != *status ) {
         ERR_printf(m_pLog, "Status from DC3: 0x%08x", *status);
         return clientStatus;
      }
   }

   size_t bytesTransferred = 0;
   uint16_t nPacketSeqNum = 1;
   while ( bytesTransferred != this->m_flashMetaPayloadMsg._imageSize ) {

      this->m_msgId++; /* Increment msg id for every new send*/

      /* Set up the basic msg */
      this->m_basicMsg._msgID       = this->m_msgId;
      this->m_basicMsg._msgReqProg  = 0;
      this->m_basicMsg._msgRoute    = this->m_msgRoute;
      this->m_basicMsg._msgType     = _CB_Req;
      this->m_basicMsg._msgName     = _CBFlashMsg;
      this->m_basicMsg._msgPayload  = _CBFlashDataPayloadMsg;

      uint8_t *buffer = new uint8_t(chunkSize);
      uint32_t crc = 0;
      size_t size = fw->getChunkAndCRC( chunkSize, buffer, &crc );
//      DBG_printf(m_pLog, "Got FW chunk of size %d with CRC 0x%08x", size, crc);

      /* Set up the payload */
      memset(&m_flashDataPayloadMsg, 0, sizeof(m_flashDataPayloadMsg));
      m_flashDataPayloadMsg._dataBuf_len = size;
      m_flashDataPayloadMsg._dataCrc = crc;
      m_flashDataPayloadMsg._seqCurr = nPacketSeqNum;
      memcpy(
            m_flashDataPayloadMsg._dataBuf,
            buffer,
            m_flashDataPayloadMsg._dataBuf_len
      );

      delete[] buffer;

      if ( nPacketSeqNum % 100 == 0 ) {
         LOG_printf(m_pLog, "Sending FW data packet %d of %d total...", nPacketSeqNum, this->m_flashMetaPayloadMsg._imageNumPackets);
      }

      /* Send the data and wait for a response */
//      LrgDataEvt *evt = Q_NEW(LrgDataEvt, MSG_SEND_OUT_SIG);
//      evt->dataLen = CBBasicMsg_write_delimited_to(&m_basicMsg, evt->dataBuf, 0);
//      evt->dataLen = CBFlashDataPayloadMsg_write_delimited_to(&m_flashDataPayloadMsg, evt->dataBuf, evt->dataLen);
//      evt->dst = m_msgRoute;
//      evt->src = m_msgRoute;
//      QACTIVE_POST(AO_MainMgr, (QEvt *)(evt), AO_MainMgr);

      CBBasicMsg basicMsg;
      CBPayloadMsgUnion_t payloadMsgUnion;
      memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));

      clientStatus = waitForResp(
            &basicMsg,
            &payloadMsgUnion,
            5
      );

      /* Make sure there were no intenal client errors. */
      if ( CLI_ERR_NONE != clientStatus ) {
         ERR_printf(
               m_pLog,
               "DC3 client failed with error 0x%08x during FW update while "
               "trying to send FW data packet %d of %d total with CRC 0x%08x",
               clientStatus, nPacketSeqNum,
               m_flashMetaPayloadMsg._imageNumPackets, crc
         );
         return( clientStatus );
      }

      /* Make sure the status of the done msg has no errors */
      *status = (CBErrorCode)payloadMsgUnion.statusPayload._errorCode;
      if ( ERR_NONE != *status ) {
         ERR_printf(
               m_pLog,
               "DC3 failed with error 0x%08x during FW update while trying to "
               "write FW data packet %d of %d total with CRC 0x%08x",
               *status, nPacketSeqNum,
               m_flashMetaPayloadMsg._imageNumPackets, crc
         );
         return( clientStatus );
      }

      /* If we got here, everything is ok so far and we can either loop back
       * around and do the next packet or exit depending if everything has been
       * transfered. */
      bytesTransferred += size;
      nPacketSeqNum++;

      if ( nPacketSeqNum == m_flashMetaPayloadMsg._imageNumPackets ) {               /* Last packet */
         DBG_printf(
               m_pLog,
               "This should be the last packet (%d of %d total)...",
               nPacketSeqNum, this->m_flashMetaPayloadMsg._imageNumPackets
         );
      DBG_printf( m_pLog,"1");
      }

//      if ( CLI_ERR_NONE == clientStatus ) {
//         if ( nPacketSeqNum == m_flashMetaPayloadMsg._imageNumPackets ) {               /* Last packet */
////            *status = (CBErrorCode)payloadMsgUnion.flashMetaPayload._errorCode;
//            *status = (CBErrorCode)payloadMsgUnion.statusPayload._errorCode;
////            DBG_printf(m_pLog, "BuildDatetime str len: %d", payloadMsgUnion.flashMetaPayload._imageDatetime_len);
////            DBG_printf(m_pLog, "BuildDatetime is: %s", payloadMsgUnion.flashMetaPayload._imageDatetime);
////            DBG_printf(m_pLog, "CRC  is: 0x%08x", payloadMsgUnion.flashMetaPayload._imageCrc);
////            DBG_printf(m_pLog, "Size is: %d", payloadMsgUnion.flashMetaPayload._imageSize);
//         } else {
//            *status = (CBErrorCode)payloadMsgUnion.statusPayload._errorCode;
//         }
//
//         if ( ERR_NONE != *status ) {
//            ERR_printf(m_pLog, "Status from DC3: 0x%08x", *status);
//            ERR_printf(
//               m_pLog,
//               "DC3 failed with error 0x%08x during FW update while trying to "
//               "write FW data packet %d of %d total with CRC 0x%08x",
//               *status, nPacketSeqNum,
//               m_flashMetaPayloadMsg._imageNumPackets, crc
//            );
//            return( clientStatus );
//         }
//         bytesTransferred += size;
//         nPacketSeqNum++;
//      } else {
//         ERR_printf(
//               m_pLog,
//               "DC3 client failed with error 0x%08x during FW update while "
//               "trying to send FW data packet %d of %d total with CRC 0x%08x",
//               clientStatus, nPacketSeqNum,
//               m_flashMetaPayloadMsg._imageNumPackets, crc
//         );
//         return( clientStatus );
//      }

   }
DBG_printf( m_pLog,"2");
   return( clientStatus );
}

/******************************************************************************/
ClientError_t ClientApi::setNewConnection(
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort
)
{
   try {
      l_pComm = new Comm(m_pLog, ipAddress, pRemPort, pLocPort, &queue);
   } catch  ( exception &e ) {
      ERR_printf(
            m_pLog,"Exception trying to open UDP connection: %s",
            e.what()
      );
      return( CLI_ERR_UDP_EXCEPTION_CAUGHT );
   }

   m_msgRoute = _CB_EthCli;
   return( CLI_ERR_NONE );
}

/******************************************************************************/
ClientError_t ClientApi::setNewConnection(
      const char *dev_name,
      int baud_rate,
      bool bDFUSEComm
)
{
   try {
      l_pComm = new Comm(m_pLog, dev_name, baud_rate, bDFUSEComm, &queue);
   } catch ( exception &e ) {
      ERR_printf(
            m_pLog,
            "Exception trying to open serial connection: %s",
            e.what()
      );
      return( CLI_ERR_SER_EXCEPTION_CAUGHT );
   }

   m_msgRoute = _CB_Serial;
   return( CLI_ERR_NONE );
}

/******************************************************************************/
void ClientApi::qfSetup( void )
{
//   /* instantiate all active objects */
//   MainMgr_ctor( m_pLog );
//
//   /* initialize the framework and the underlying RT kernel */
//   QF_init();
//
//   QS_OBJ_DICTIONARY(l_smlPoolSto);
//   QS_OBJ_DICTIONARY(l_medPoolSto);
//   QS_OBJ_DICTIONARY(l_MainMgrQueueSto);
//
//   /* init publish-subscribe */
//   QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));
//
//   /* initialize event pools... */
//   QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
//   QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));
//
//   /* initialize the raw queues */
//   QEQueue_init(&cliQueue, l_cliQueueSto, Q_DIM(l_cliQueueSto));
}

/******************************************************************************/
void ClientApi::start( void )
{
//   this->m_workerThread = boost::thread( runMainMgr );
//   /* Put this thread to sleep to allow QF to start up */
//   boost::this_thread::sleep(boost::posix_time::milliseconds(10));
//   DBG_printf(m_pLog,"QF Framework started successfully.");
}

/******************************************************************************/
void ClientApi::waitForStop( void )
{
//   DBG_printf(m_pLog,"Waiting for MainMgr to be finished.");
//   this->m_workerThread.join();
//   DBG_printf(m_pLog,"Thread finished. Returning");
}

/******************************************************************************/
void ClientApi::setLogging( LogStub *log )
{
   m_pLog = log;
   DBG_printf(m_pLog,"Logging setup successful.");
}

/******************************************************************************/
void ClientApi::stop( void )
{
//   DBG_printf(m_pLog,"Stopping client.");
//   QEvt* evt = Q_NEW(QEvt, EXIT_SIG);
//   QF_PUBLISH((QEvt *)evt, AO_MainMgr);

}

/******************************************************************************/
void ClientApi::startJob( void )
{
//   DBG_printf(m_pLog,"Starting job.");
//   /* Create the event and directly post it to the right AO. */
//   QEvt* evt = Q_NEW(QEvt, TEST_JOB_SIG);
//   QACTIVE_POST(AO_MainMgr, (QEvt *)(evt), AO_MainMgr);
}

/******************************************************************************/
ClientError_t ClientApi::pollForResp(
      CBBasicMsg *basicMsg,
      CBPayloadMsgUnion_t *payloadMsgUnion
)
{

   /* This code indicates to the caller that the function is didn't find any
    * events in the queue this call. */
   ClientError_t status = CLI_ERR_MSG_WAITING_FOR_RESP;
   if ( queue.empty() ) {
      return status;
   }

   MsgData_t msg;
   unsigned int offset = 0;
   memset(&msg, 0, sizeof(msg));
   if( queue.pop( msg ) ) {
      offset = CBBasicMsg_read_delimited_from(
            msg.dataBuf,
            basicMsg,
            0
      );

      /* If this is an Ack msg, don't bother checking for a payload.  There
       * shouldn't be one.*/
      if ( _CB_Ack == basicMsg->_msgType ) {
         return CLI_ERR_NONE;
      }

      /* We should never get a request from DC3 */
      if ( _CB_Req == basicMsg->_msgType ) {
         return CLI_ERR_MSG_UNEXPECTED_REQ_RECEIVED;
      }

      /* Extract the payload (if exists) since this buffer is going away the moment we get
       * into a state.  We'll figure out later if it's valid, right before we send an Ack */
      switch( basicMsg->_msgPayload ) {
         case _CBNoMsg:
            status = CLI_ERR_MSG_MISSING_EXPECTED_PAYLOAD;
            break;
         case _CBStatusPayloadMsg:
            //                  DBG_printf( m_pLog, "Status payload detected");
            status = CLI_ERR_NONE;
            CBStatusPayloadMsg_read_delimited_from(
                  (void*)msg.dataBuf,
                  &(payloadMsgUnion->statusPayload),
                  offset
            );
            break;
         case _CBVersionPayloadMsg:
            status = CLI_ERR_NONE;
            DBG_printf( m_pLog, "Version payload detected");
            CBVersionPayloadMsg_read_delimited_from(
                  (void*)msg.dataBuf,
                  &(payloadMsgUnion->versionPayload),
                  offset
            );
            break;
         case _CBBootModePayloadMsg:
            status = CLI_ERR_NONE;
            DBG_printf( m_pLog, "BootMode payload detected");
            CBBootModePayloadMsg_read_delimited_from(
                  (void*)msg.dataBuf,
                  &(payloadMsgUnion->bootmodePayload),
                  offset
            );
            break;
         default:
            status = CLI_ERR_MSG_UNKNOWN_PAYLOAD;
            ERR_printf( m_pLog, "Unknown payload detected. Error: 0x%08x", status);
            break;
      }
   } else {
      status = CLI_ERR_MSG_UNABLE_TO_GET_FROM_QUEUE;
      ERR_printf( m_pLog, "Unable to get msg from queue. Error: 0x%08x", status);
   }

   return status;





//   /* Check if there's data in the queue and process it if there. */
//   QEvt const *evt = QEQueue_get(&cliQueue);
//   cout << "Checking for event... " << endl;
//   if ( evt != (QEvt *)0 ) { /* Check whether an event is present in queue */
//
//      cout << "Found event... " << endl;
//      switch( evt->sig ) {        /* Identify the event by its signal enum */
//         case TEST_JOB_DONE_SIG:
//            break;
//         case MSG_ACK_RECVD_SIG:
//            cout << "Got ack in poll" << endl;
//            CBBasicMsg_read_delimited_from(
//                  (void*)((LrgDataEvt const *) evt)->dataBuf,
//                  basicMsg,
//                  0
//            );
//            if ( this->m_bAckLogEnable == true && this->m_pAckHandlerCBFunction) {
//               this->m_pAckHandlerCBFunction( *basicMsg );
//            }
//            status = CLI_ERR_NONE;
//            break;
//         case MSG_PROG_RECVD_SIG:
//            DBG_printf(m_pLog, "Got MSG_PROG_RECVD_SIG");
//            if (this->m_bAckLogEnable == true ) {
//               WRN_printf(
//                     m_pLog,
//                     "Got a progress msg but not handler set up for it..."
//               );
//            }
//            /* No break here.  Fall through. We handle both of these the same */
//         case MSG_DONE_RECVD_SIG: {
////            if ( MSG_DONE_RECVD_SIG == evt->sig ) {
////               DBG_printf(m_pLog, "Got MSG_DONE_RECVD_SIG");
////            }
//            int offset = CBBasicMsg_read_delimited_from(
//                  (void*)((LrgDataEvt const *) evt)->dataBuf,
//                  basicMsg,
//                  0
//            );
//
//            /* Extract the payload (if exists) since this buffer is going away the moment we get
//             * into a state.  We'll figure out later if it's valid, right before we send an Ack */
//            switch( basicMsg->_msgPayload ) {
//               case _CBNoMsg:
//                  WRN_printf( m_pLog, "No payload detected, this is probably an error" );
//                  break;
//               case _CBStatusPayloadMsg:
////                  DBG_printf( m_pLog, "Status payload detected");
//                  CBStatusPayloadMsg_read_delimited_from(
//                        (void*)((LrgDataEvt const *) evt)->dataBuf,
//                        &(payloadMsgUnion->statusPayload),
//                        offset
//                  );
//                  break;
//               case _CBVersionPayloadMsg:
//                  DBG_printf( m_pLog, "Version payload detected");
//                  CBVersionPayloadMsg_read_delimited_from(
//                        (void*)((LrgDataEvt const *) evt)->dataBuf,
//                        &(payloadMsgUnion->versionPayload),
//                        offset
//                  );
//                  break;
//               case _CBBootModePayloadMsg:
//                  DBG_printf( m_pLog, "BootMode payload detected");
//                  CBBootModePayloadMsg_read_delimited_from(
//                        (void*)((LrgDataEvt const *) evt)->dataBuf,
//                        &(payloadMsgUnion->bootmodePayload),
//                        offset
//                  );
//                  break;
//               default:
//                  WRN_printf( m_pLog, "Unknown payload detected, this is probably an error");
//                  break;
//            }
//
//            if ( MSG_DONE_RECVD_SIG == evt->sig &&
//                  this->m_bDoneLogEnable == true &&
//                  this->m_pDoneHandlerCBFunction ) {
//               this->m_pDoneHandlerCBFunction( *basicMsg, *payloadMsgUnion );
//            }
//            status = CLI_ERR_NONE;
//            break;
//         }
//         case MSG_ACK_TIMED_OUT_SIG:
//            DBG_printf(m_pLog, "Got MSG_ACK_TIMED_OUT_SIG");
//            status = CLI_ERR_MSG_ACK_WAIT_TIMED_OUT;
//            break;
//         case MSG_DONE_TIMED_OUT_SIG:
//            DBG_printf(m_pLog, "Got MSG_DONE_TIMED_OUT_SIG");
//            status = CLI_ERR_MSG_DONE_WAIT_TIMED_OUT;
//            break;
//         default:
//            /* We should never get here */
//            status = CLI_ERR_UNKNOWN;
//            WRN_printf(
//                  m_pLog,
//                  "Unknown event in the queue; signal: %d, client error: 0x%08x",
//                  evt->sig, status
//            );
//            break;
//      }
//
//
//      /* !!! Don't forget to garbage collect the event after processing the
//       * event. After this, any data to which this pointer points to may not be
//       * valid and should not be referenced. This should be done in the calling
//       * function. */
//      QF_gc(evt);
//   }

   return( status );
}

/******************************************************************************/
ClientError_t ClientApi::waitForResp(
      CBBasicMsg *basicMsg,
      CBPayloadMsgUnion_t *payloadMsgUnion,
      uint16_t timeoutSecs
)
{
   int timeout_ms = timeoutSecs * 1000;
   ClientError_t status = CLI_ERR_NONE;
   while (timeout_ms > 0) {                         /* Beginning of the thread forever loop */
      status = pollForResp(basicMsg, payloadMsgUnion);
      if (CLI_ERR_MSG_WAITING_FOR_RESP != status ) {
         return status;
      }
      /* Sleep for 5 ms and decrement timeout counter. */
      boost::this_thread::sleep(boost::posix_time::milliseconds(50));
      timeout_ms -= 5;
   }
   return CLI_ERR_TIMEOUT_WAITING_FOR_RESP;
}

/******************************************************************************/
ClientError_t ClientApi::setReqCallBack(
      CB_ReqMsgHandler_t pCallbackFunction
)
{
   ClientError_t err = CLI_ERR_NONE;

   this->m_pReqHandlerCBFunction = pCallbackFunction;

   if ( NULL == this->m_pReqHandlerCBFunction ) {
      err = CLI_ERR_INVALID_CALLBACK;
      ERR_printf(m_pLog, "Unable to set Req callback, err: 0x%08x",err);
   } else {
      DBG_printf(m_pLog, "Req callback set to addr: 0x%08x",
            this->m_pReqHandlerCBFunction);
      m_bReqLogEnable = true;
   }

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
//   try {
//      CBBasicMsg basicMsg;
//      memset(&basicMsg, 0, sizeof(basicMsg));
//      basicMsg._msgID = 0;
//      basicMsg._msgName = _CBNoMsg;
//      basicMsg._msgPayload = _CBNoMsg;
//      basicMsg._msgReqProg = 0;
//      basicMsg._msgRoute = _CB_NoRoute;
//      basicMsg._msgType = _CB_Req;
//
//      this->m_pReqHandlerCBFunction( basicMsg );
//   } catch ( ... ) {
//      err = CLI_ERR_INVALID_CALLBACK;
//      cerr << "Invalid callback passed in by user" << endl;
//      this->m_pReqHandlerCBFunction = NULL;
//   }

   return( err );
}

/******************************************************************************/
ClientError_t ClientApi::setAckCallBack(
      CB_AckMsgHandler_t pCallbackFunction
)
{
   ClientError_t err = CLI_ERR_NONE;
   this->m_pAckHandlerCBFunction = pCallbackFunction;

   if ( NULL == this->m_pAckHandlerCBFunction ) {
      err = CLI_ERR_INVALID_CALLBACK;
      ERR_printf(m_pLog, "Unable to set Ack callback, err: 0x%08x",err);
   } else {
      DBG_printf(m_pLog, "Ack callback set to addr: 0x%08x",
            this->m_pAckHandlerCBFunction);
      m_bAckLogEnable= true;
   }

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
//   try {
//      CBBasicMsg basicMsg;
//      memset(&basicMsg, 0, sizeof(basicMsg));
//      basicMsg._msgID = 0;
//      basicMsg._msgName = _CBNoMsg;
//      basicMsg._msgPayload = _CBNoMsg;
//      basicMsg._msgReqProg = 0;
//      basicMsg._msgRoute = _CB_NoRoute;
//      basicMsg._msgType = _CB_Ack;
//      this->m_pAckHandlerCBFunction( basicMsg );
//   } catch ( ... ) {
//      err = CLI_ERR_INVALID_CALLBACK;
//      cerr << "Invalid callback passed in by user" << endl;
//      this->m_pReqHandlerCBFunction = NULL;
//   }

   return( err );
}

/******************************************************************************/
ClientError_t ClientApi::setDoneCallBack(
      CB_DoneMsgHandler_t pCallbackFunction
)
{
   ClientError_t err = CLI_ERR_NONE;

   this->m_pDoneHandlerCBFunction = pCallbackFunction;

   if ( NULL == this->m_pDoneHandlerCBFunction ) {
      err = CLI_ERR_INVALID_CALLBACK;
      ERR_printf(m_pLog, "Unable to set Done callback, err: 0x%08x",err);
   } else {
      DBG_printf(m_pLog, "Done callback set to addr: 0x%08x",
            this->m_pDoneHandlerCBFunction);
      m_bDoneLogEnable = true;
   }

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
//   try {
//      CBBasicMsg basicMsg;
//      memset(&basicMsg, 0, sizeof(basicMsg));
//      basicMsg._msgID = 0;
//      basicMsg._msgName = _CBNoMsg;
//      basicMsg._msgPayload = _CBNoMsg;
//      basicMsg._msgReqProg = 0;
//      basicMsg._msgRoute = _CB_NoRoute;
//      basicMsg._msgType = _CB_Ack;
//
//      CBPayloadMsgUnion_t payloadMsgUnion;
//      payloadMsgUnion.statusPayload._errorCode = ERR_NONE;
//      this->m_pDoneHandlerCBFunction( basicMsg, payloadMsgUnion);
//   } catch ( ... ) {
//      err = CLI_ERR_INVALID_CALLBACK;
//      cerr << "Invalid callback passed in by user" << endl;
//      this->m_pReqHandlerCBFunction = NULL;
//   }

   return( err );
}

/******************************************************************************/
void ClientApi::enableMsgCallbacks( void )
{
   m_bReqLogEnable = true;
   m_bAckLogEnable = true;
   m_bProgLogEnable = true;
   m_bDoneLogEnable = true;
}

/******************************************************************************/
void ClientApi::disableMsgCallbacks( void )
{
   m_bReqLogEnable = false;
   m_bAckLogEnable = false;
   m_bProgLogEnable = false;
   m_bDoneLogEnable = false;
}

/******************************************************************************/
ClientApi::ClientApi(
      LogStub *log,
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort
) :   m_pLog(NULL),
      m_pReqHandlerCBFunction(NULL),
      m_pAckHandlerCBFunction(NULL),
      m_pDoneHandlerCBFunction(NULL),
      m_bReqLogEnable(false),
      m_bAckLogEnable(false),
      m_bProgLogEnable(false),
      m_bDoneLogEnable(false),
      m_msgId( 0 ), m_bRequestProg( false ), m_msgRoute( _CB_EthCli )
{
   this->setLogging(log);
   this->qfSetup();                               /* Initialize QF framework. */
   this->setNewConnection(ipAddress, pRemPort, pLocPort);
   l_pComm = NULL;
}

/******************************************************************************/
ClientApi::ClientApi(
         LogStub *log,
         const char *dev_name,
         int baud_rate,
         bool bDFUSEComm
) :   m_pLog(NULL),
      m_pReqHandlerCBFunction(NULL),
      m_pAckHandlerCBFunction(NULL),
      m_pDoneHandlerCBFunction(NULL),
      m_bReqLogEnable(false),
      m_bAckLogEnable(false),
      m_bProgLogEnable(false),
      m_bDoneLogEnable(false),
      m_msgId( 0 ), m_bRequestProg( false ), m_msgRoute( _CB_Serial )
{
   this->setLogging(log);
   this->qfSetup();                               /* Initialize QF framework. */
   try {
      this->setNewConnection(dev_name, baud_rate, bDFUSEComm);
   } catch ( exception &e ) {
      ERR_printf(m_pLog,"Exception trying to open serial connection: %s", e.what());
      throw( &e );
   }
   l_pComm = NULL;
}

/******************************************************************************/
ClientApi::ClientApi( LogStub *log ) :
      m_pLog(NULL),
      m_pReqHandlerCBFunction(NULL),
      m_pAckHandlerCBFunction(NULL),
      m_pDoneHandlerCBFunction(NULL),
      m_bReqLogEnable(false),
      m_bAckLogEnable(false),
      m_bProgLogEnable(false),
      m_bDoneLogEnable(false),
      m_msgId( 0 ), m_bRequestProg( false ), m_msgRoute( _CB_NoRoute )
{

   this->setLogging(log);                                      /* Set logging */
   this->qfSetup();                               /* Initialize QF framework. */
   l_pComm = NULL;

   /* This works */
//   DBG_printf(m_pLog,"Attempting to init serial...");
//   Serial *serial;
//   try {
//      serial = new Serial(
//            "COM32",
//            115200,
//            false
//      );
//
//   } catch (...) {
//      cout << "ERROR: Unable to open serial port "
//            << "COM32" << endl;
//      exit(1); // Do a regular exit since this is critical
//   }
//   DBG_printf(m_pLog,"Attempting to write serial...");
//
//   serial->write_some("Test\n", 5);

   /* This also works.*/
//   DBG_printf(m_pLog,"Attempting to init UDP eth...");
//   Eth *eth;
//   try {
//      eth = new Eth(
//            "172.27.0.75",
//            "1502",
//            "53432"
//      );
//   } catch ( ... ) {
//      ERR_printf("Unable to connect to UDP. Exiting");
//      EXIT_LOG_FLUSH(1); // Do a regular exit since this is critical
//   }
//   DBG_printf(m_pLog,"Attempting to write UDP eth...");
//   eth->write_some("Test\n", 5);

   /* This also works.*/
//   DBG_printf(m_pLog,"Attempting to init UDP eth...");
//   Comm *m_comm;
//   try {
//      m_comm = new Comm(
//            m_pLog,
//            "172.27.0.75",
//            "1502",
//            "53432"
//      );
//   } catch ( ... ) {
//      ERR_printf(m_pLog,"Unable to connect to UDP. Exiting");
//      return; // Do a regular exit since this is critical
//   }
//   DBG_printf(m_pLog,"Attempting to write UDP eth...");
//   m_comm->write_some("Test\n", 5);

}

/******************************************************************************/
ClientApi::~ClientApi(  )
{
   delete[] m_pLog;
   delete[] l_pComm;
}



