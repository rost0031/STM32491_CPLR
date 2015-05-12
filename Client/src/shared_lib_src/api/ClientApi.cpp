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
/* Private defines -----------------------------------------------------------*/
#define TIME_POLLING_MSEC  1  /**< Number of ms to wait between polling queue */

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**< A single global pointer to a comm class which handles all communications
 * for the client. The reason it's not a local class pointer is this class is
 * part of the API and the header cannot point to the comm class since it's not
 * part of the API. */
static Comm *l_pComm;

/**< A single global pointer to a boost thread-safe queue which handles all
 * received communications from the DC3 board. The reason it's not a local class
 * pointer is this class is part of the API and the header cannot point to the
 * comm class since it's not part of the API. */
boost::lockfree::queue<MsgData_t> queue(128);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******************************************************************************/
ClientError_t ClientApi::DC3_getMode(CBErrorCode *status, CBBootMode *mode)
{
   this->enableMsgCallbacks();

   /* These will be used for responses */
   CBBasicMsg basicMsg;
   CBPayloadMsgUnion_t payloadMsgUnion;

   /* Common settings for most messages */
   this->m_basicMsg._msgID       = this->m_msgId;
   this->m_basicMsg._msgReqProg  = (unsigned long)this->m_bRequestProg;
   this->m_basicMsg._msgRoute    = this->m_msgRoute;

   /* Settings specific to this message */
   this->m_basicMsg._msgType     = _CB_Req;
   this->m_basicMsg._msgName     = _CBGetBootModeMsg;
   this->m_basicMsg._msgPayload  = _CBNoMsg;

   size_t size = CB_MAX_MSG_LEN;
   uint8_t *buffer = new uint8_t(size);                    /* Allocate buffer */
   unsigned int bufferLen = 0;
   bufferLen = CBBasicMsg_write_delimited_to(&m_basicMsg, buffer, 0);
   l_pComm->write_some((char *)buffer, bufferLen);                /* Send Req */

   delete buffer;                                            /* Delete buffer */

   memset(&basicMsg, 0, sizeof(basicMsg));
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
   ClientError_t clientStatus = waitForResp(                  /* Wait for Ack */
         &basicMsg,
         &payloadMsgUnion,
         HL_MAX_TOUT_SEC_CLI_WAIT_FOR_ACK
   );

   if ( CLI_ERR_NONE != clientStatus ) {                    /* Check response */
      ERR_printf(m_pLog,
            "Waiting for Ack received client Error: 0x%08x", clientStatus);
      return clientStatus;
   }

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

   memset(&basicMsg, 0, sizeof(basicMsg));
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
   clientStatus = waitForResp(                           /* Wait for Done msg */
         &basicMsg,
         &payloadMsgUnion,
         HL_MAX_TOUT_SEC_CLI_WAIT_FOR_SIMPLE_MSG_DONE
   );
   if ( CLI_ERR_NONE != clientStatus ) {                    /* Check response */
      ERR_printf(m_pLog,
            "Waiting for Done received client Error: 0x%08x", clientStatus);
      return clientStatus;
   } else {
      *status = (CBErrorCode)payloadMsgUnion.bootmodePayload._errorCode;
      *mode = payloadMsgUnion.bootmodePayload._bootMode;
   }

   return clientStatus;
}

/******************************************************************************/
ClientError_t ClientApi::DC3_setMode(CBErrorCode *status, CBBootMode mode)
{
   this->enableMsgCallbacks();

   /* These will be used for responses */
   CBBasicMsg basicMsg;
   CBPayloadMsgUnion_t payloadMsgUnion;

   /* Common settings for most messages */
   this->m_basicMsg._msgID       = this->m_msgId;
   this->m_basicMsg._msgReqProg  = (unsigned long)this->m_bRequestProg;
   this->m_basicMsg._msgRoute    = this->m_msgRoute;

   /* Settings specific to this message */
   this->m_basicMsg._msgType     = _CB_Req;
   this->m_basicMsg._msgName     = _CBSetBootModeMsg;
   this->m_basicMsg._msgPayload  = _CBBootModePayloadMsg;

   this->m_bootmodePayloadMsg._bootMode = mode;
   this->m_bootmodePayloadMsg._errorCode = ERR_NONE; // This field is ignored in Req msgs.

   size_t size = CB_MAX_MSG_LEN;
   uint8_t *buffer = new uint8_t(size);                    /* Allocate buffer */
   unsigned int bufferLen = 0;
   bufferLen = CBBasicMsg_write_delimited_to(&m_basicMsg, buffer, 0);
   bufferLen = CBBootModePayloadMsg_write_delimited_to(&m_bootmodePayloadMsg, buffer, bufferLen);
   l_pComm->write_some((char *)buffer, bufferLen);                /* Send Req */

   delete buffer;                                            /* Delete buffer */

   memset(&basicMsg, 0, sizeof(basicMsg));
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
   ClientError_t clientStatus = waitForResp(                  /* Wait for Ack */
         &basicMsg,
         &payloadMsgUnion,
         HL_MAX_TOUT_SEC_CLI_WAIT_FOR_ACK
   );

   if ( CLI_ERR_NONE != clientStatus ) {                    /* Check response */
      ERR_printf(m_pLog,
            "Waiting for Ack received client Error: 0x%08x", clientStatus);
      return clientStatus;
   }

   memset(&basicMsg, 0, sizeof(basicMsg));
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
   clientStatus = waitForResp(                           /* Wait for Done msg */
         &basicMsg,
         &payloadMsgUnion,
         HL_MAX_TOUT_SEC_CLI_WAIT_FOR_SIMPLE_MSG_DONE
   );
   if ( CLI_ERR_NONE != clientStatus ) {                    /* Check response */
      ERR_printf(m_pLog,
            "Waiting for Done received client Error: 0x%08x", clientStatus);
      return clientStatus;
   } else {
      *status = (CBErrorCode)payloadMsgUnion.statusPayload._errorCode;
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

   /* These will be used for responses */
   CBBasicMsg basicMsg;
   CBPayloadMsgUnion_t payloadMsgUnion;

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

   /* Buffer and counter to use for sending messages. We could allocated when
    * needed but it's a lot slower */
   uint8_t buffer[CB_MAX_MSG_LEN];
   unsigned int bufferLen;

   /* 1. Send the Flashmsg wih FlashMetaPayloadMsg */
   memset(buffer, 0, sizeof(buffer));
   bufferLen = 0;

   bufferLen = CBBasicMsg_write_delimited_to(&m_basicMsg, buffer, 0);
   bufferLen = CBFlashMetaPayloadMsg_write_delimited_to(&m_flashMetaPayloadMsg, buffer, bufferLen);
   l_pComm->write_some((char *)buffer, bufferLen);                /* Send Req */

   /* 2. Wait for Ack */
   memset(&basicMsg, 0, sizeof(basicMsg));
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
   clientStatus = waitForResp(                                /* Wait for Ack */
         &basicMsg,
         &payloadMsgUnion,
         HL_MAX_TOUT_SEC_CLI_WAIT_FOR_ACK
   );

   if ( CLI_ERR_NONE != clientStatus ) {                    /* Check response */
      ERR_printf(m_pLog,
            "Waiting for Ack received client Error: 0x%08x", clientStatus);
      return clientStatus;
   }

   /* 3. Wait for Done with status payload */
   memset(&basicMsg, 0, sizeof(basicMsg));
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
   clientStatus = waitForResp(                               /* Wait for Done */
         &basicMsg,
         &payloadMsgUnion,
         LL_MAX_TOUT_SEC_CLI_FW_META_WAIT
   );

   if ( CLI_ERR_NONE != clientStatus ) {                    /* Check response */
      ERR_printf(m_pLog,
            "Waiting for Done received client Error: 0x%08x", clientStatus);
      return clientStatus;
   } else {
      *status = (CBErrorCode)payloadMsgUnion.statusPayload._errorCode;
      if ( ERR_NONE != *status ) {
         ERR_printf(m_pLog, "Status from DC3: 0x%08x", *status);
         return clientStatus;
      }
   }

   /* 4. Cycle through the FW image and send out FW data packets until done. */
   size_t bytesTransferred = 0;
   uint16_t nPacketSeqNum = 0;
   while ( bytesTransferred < this->m_flashMetaPayloadMsg._imageSize ) {

      this->m_msgId++;                  /* Increment msg id for every new send*/
      nPacketSeqNum++; /* Increment right away so the first packet is 1 not 0 */

      /* Set up the basic msg */
      this->m_basicMsg._msgID       = this->m_msgId;
      this->m_basicMsg._msgReqProg  = 0;
      this->m_basicMsg._msgRoute    = this->m_msgRoute;
      this->m_basicMsg._msgType     = _CB_Req;
      this->m_basicMsg._msgName     = _CBFlashMsg;
      this->m_basicMsg._msgPayload  = _CBFlashDataPayloadMsg;

      uint32_t crc = 0;
      /* Set up the payload */
      memset(&m_flashDataPayloadMsg, 0, sizeof(m_flashDataPayloadMsg));
      m_flashDataPayloadMsg._dataBuf_len = fw->getChunkAndCRC( chunkSize, (uint8_t *)(m_flashDataPayloadMsg._dataBuf), &crc );
      m_flashDataPayloadMsg._dataCrc = crc;
      m_flashDataPayloadMsg._seqCurr = nPacketSeqNum;

      /* Only log every 100th packet since it gets way too chatty otherwise */
      if ( nPacketSeqNum % 100 == 0 ) {
         LOG_printf(m_pLog,
               "Sending FW data packet %d of %d total...",
               nPacketSeqNum, this->m_flashMetaPayloadMsg._imageNumPackets);
      }

      /* 5. Send the Flashmsg wih FlashDataPayloadMsg */
      memset(buffer, 0, sizeof(buffer));
      bufferLen = 0;
      bufferLen = CBBasicMsg_write_delimited_to(&m_basicMsg, buffer, 0);
      bufferLen = CBFlashDataPayloadMsg_write_delimited_to(&m_flashDataPayloadMsg, buffer, bufferLen);
//      DBG_printf(m_pLog, "BufferLen is %d", bufferLen);
      l_pComm->write_some((char *)buffer, bufferLen);             /* Send Req */

      /* 6. Wait for Ack */
      memset(&basicMsg, 0, sizeof(basicMsg));
      memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));
      clientStatus = waitForResp(                            /* Wait for Done */
            &basicMsg,
            &payloadMsgUnion,
            HL_MAX_TOUT_SEC_CLI_WAIT_FOR_ACK
      );

      if ( CLI_ERR_NONE != clientStatus ) {                 /* Check response */
         ERR_printf(m_pLog,
               "Waiting for Ack received client Error: 0x%08x", clientStatus);
         return clientStatus;
      }

      /* 7. Wait for Done */
      memset(&basicMsg, 0, sizeof(basicMsg));
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
      bytesTransferred += m_flashDataPayloadMsg._dataBuf_len;

      if ( nPacketSeqNum == m_flashMetaPayloadMsg._imageNumPackets ) {               /* Last packet */
         DBG_printf(
               m_pLog,
               "This should be the last packet (%d of %d total)...",
               nPacketSeqNum, this->m_flashMetaPayloadMsg._imageNumPackets
         );
         DBG_printf(
               m_pLog,
               "bytesTransferred: %d (of %d total), nPacketSeqNum %d (of %d total)",
               bytesTransferred, m_flashMetaPayloadMsg._imageSize,
               nPacketSeqNum, m_flashMetaPayloadMsg._imageNumPackets
         );
      }
   }
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
void ClientApi::setLogging( LogStub *log )
{
   m_pLog = log;
   DBG_printf(m_pLog,"Logging setup successful.");
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
      /* Sleep for TIME_POLLING_MSEC ms and decrement timeout counter. */
      boost::this_thread::sleep(boost::posix_time::milliseconds(TIME_POLLING_MSEC));
      timeout_ms -= TIME_POLLING_MSEC;
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
   l_pComm = NULL;
}

/******************************************************************************/
ClientApi::~ClientApi(  )
{
   delete[] m_pLog;
   delete[] l_pComm;
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/


