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
#include "MainMgr.h"
#include "MainMgrDefs.h"
#include "cencode.h"
#include "cdecode.h"
#include "base64_wrapper.h"
#include "LogHelper.h"
#include "comm.h"
#include <boost/thread/thread.hpp>
#include "msg_utils.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_API );

/* Private typedefs ----------------------------------------------------------*/
/* storage for event pools... */
static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvent)];
    uint8_t e2[sizeof(ExitEvt)];
} l_smlPoolSto[10];                     /* storage for the small event pool */

static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t *e1[sizeof(LrgDataEvt)];
} l_medPoolSto[15];                    /* storage for the medium event pool */

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static QEvt const *l_MainMgrQueueSto[10];
static QEvt const *l_cliQueueSto[10]; /**< Storage for raw QE queue for
                                                 communicating with ClientApi */
static QSubscrList l_subscrSto[MAX_PUB_SIG];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void runMainMgr( void )
{
   /* start the active objects... */
   QActive_start(
         AO_MainMgr,
         MAIN_MGR_PRIORITY,
         l_MainMgrQueueSto,
         Q_DIM(l_MainMgrQueueSto),
         (void *)0,
         0,
         (QEvent *)0,
         "MainMgr"
   );
   QF_run();                              /* run the QF application */
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******************************************************************************/
ClientError_t ClientApi::DC3_getMode(CBErrorCode *status, CBBootMode *mode)
{
   /* Settings specific to this message */
   this->m_basicMsg._msgName     = _CBGetBootModeMsg;
   this->m_basicMsg._msgPayload  = _CBNoMsg;
   this->m_basicMsg._msgType     = _CB_Req;

   /* Common settings for most messages */
   this->m_basicMsg._msgID       = this->m_msgId;
   this->m_basicMsg._msgReqProg  = (unsigned long)this->m_bRequestProg;
   this->m_basicMsg._msgRoute    = this->m_msgRoute;


   LrgDataEvt *evt = Q_NEW(LrgDataEvt, MSG_SEND_OUT_SIG);
   evt->dataLen = CBBasicMsg_write_delimited_to(&m_basicMsg, evt->dataBuf, 0);
   evt->dst = m_msgRoute;
   evt->src = m_msgRoute;
   QACTIVE_POST(AO_MainMgr, (QEvt *)(evt), AO_MainMgr);

   char *tmp = (char *)malloc(sizeof(char) * 1000);
   uint16_t tmpStrLen;
   ClientError_t convertStatus = MSG_hexToStr(
         evt->dataBuf,
         evt->dataLen,
         tmp,
         1000,
         &tmpStrLen,
         0,
         '-',
         true
   );

   DBG_printf(this->m_pLog,"ConStatus: 0x%x, sending a buffer with: %s", convertStatus, tmp);
   free(tmp);

   CBBasicMsg basicMsg;
   CBMsgName payloadMsgName = _CBNoMsg;
   CBPayloadMsgUnion_t payloadMsgUnion;
   memset(&payloadMsgUnion, 0, sizeof(payloadMsgUnion));

   ClientError_t clientStatus = waitForJobDone(
         &basicMsg,
         &payloadMsgName,
         &payloadMsgUnion
   );

   if ( CLI_ERR_NONE == clientStatus ) {
      *status = (CBErrorCode)payloadMsgUnion.runmodePayload._errorCode;
      *mode = payloadMsgUnion.runmodePayload._bootMode;
   }

   return clientStatus;
}

/******************************************************************************/
ClientError_t ClientApi::setNewConnection(
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort
)
{
   Comm* my_comm;
   try {
      my_comm = new Comm(this->m_pLog, ipAddress, pRemPort, pLocPort);
   } catch  ( exception &e ) {
      ERR_printf(
            this->m_pLog,"Exception trying to open UDP connection: %s",
            e.what()
      );
      return( CLI_ERR_UDP_EXCEPTION_CAUGHT );
   }

   MainMgr_setConn(my_comm);
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
   Comm* my_comm;
   try {
      my_comm = new Comm(this->m_pLog, dev_name, baud_rate, bDFUSEComm);
   } catch ( exception &e ) {
      ERR_printf(
            this->m_pLog,
            "Exception trying to open serial connection: %s",
            e.what()
      );
      return( CLI_ERR_SER_EXCEPTION_CAUGHT );
   }

   MainMgr_setConn(my_comm);
   m_msgRoute = _CB_Serial;
   return( CLI_ERR_NONE );
}

/******************************************************************************/
void ClientApi::qfSetup( void )
{
   /* instantiate all active objects */
   MainMgr_ctor( this->m_pLog );

   /* initialize the framework and the underlying RT kernel */
   QF_init();

   QS_OBJ_DICTIONARY(l_smlPoolSto);
   QS_OBJ_DICTIONARY(l_medPoolSto);
   QS_OBJ_DICTIONARY(l_MainMgrQueueSto);

   /* init publish-subscribe */
   QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));

   /* initialize event pools... */
   QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
   QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

   /* initialize the raw queues */
   QEQueue_init(&cliQueue, l_cliQueueSto, Q_DIM(l_cliQueueSto));
}

/******************************************************************************/
void ClientApi::start( void )
{
   this->m_workerThread = boost::thread( runMainMgr );
   DBG_printf(this->m_pLog,"QF Framework started successfully.");
}

/******************************************************************************/
void ClientApi::waitForStop( void )
{
   DBG_printf(this->m_pLog,"Waiting for MainMgr to be finished.");
   this->m_workerThread.join();
   DBG_printf(this->m_pLog,"Thread finished. Returning\n");
}

/******************************************************************************/
void ClientApi::setLogging( LogStub *log )
{
   this->m_pLog = log;
   DBG_printf(this->m_pLog,"Logging setup successful.");
}

/******************************************************************************/
void ClientApi::stop( void )
{
   DBG_printf(this->m_pLog,"Stopping client.");
   QEvt* evt = Q_NEW(QEvt, EXIT_SIG);
   QF_PUBLISH((QEvt *)evt, AO_MainMgr);

}

/******************************************************************************/
void ClientApi::startJob( void )
{
   DBG_printf(this->m_pLog,"Starting job.");
   /* Create the event and directly post it to the right AO. */
   QEvt* evt = Q_NEW(QEvt, TEST_JOB_SIG);
   QACTIVE_POST(AO_MainMgr, (QEvt *)(evt), AO_MainMgr);
}

/******************************************************************************/
ClientError_t ClientApi::pollForJobDone(
      CBBasicMsg *basicMsg,
      CBMsgName *payloadMsgName,
      CBPayloadMsgUnion_t *payloadMsgUnion
)
{
   /* This code indicates to the caller that the function is didn't find any
    * events in the queue this call. */
   ClientError_t status = CLI_ERR_MSG_WAITING_FOR_RESP;

   /* Check if there's data in the queue and process it if there. */
   QEvt const *evt = QEQueue_get(&cliQueue);
   if ( evt != (QEvt *)0 ) { /* Check whether an event is present in queue */

      DBG_printf(this->m_pLog,"Job finish found in queue.");

      switch( evt->sig ) {        /* Identify the event by its signal enum */
         case TEST_JOB_DONE_SIG:
            DBG_printf(this->m_pLog, "Got TEST_JOB_DONE_SIG");
            break;
         case MSG_ACK_RECVD_SIG:
            DBG_printf(this->m_pLog, "Got MSG_ACK_RECVD_SIG");
            CBBasicMsg_read_delimited_from(
                  (void*)((LrgDataEvt const *) evt)->dataBuf,
                  basicMsg,
                  0
            );
            if (this->m_pAckHandlerCBFunction) {
               this->m_pAckHandlerCBFunction( *basicMsg );
            }
            DBG_printf(this->m_pLog, "Got an Ack msg");
            break;
         case MSG_PROG_RECVD_SIG:
            DBG_printf(this->m_pLog, "Got MSG_PROG_RECVD_SIG");
            DBG_printf(
                  this->m_pLog,
                  "Got a progress msg but not handler set up for it..."
            );
            break;
         case MSG_DONE_RECVD_SIG: {
            DBG_printf(this->m_pLog, "Got MSG_DONE_RECVD_SIG");
            int offset = CBBasicMsg_read_delimited_from(
                  (void*)((LrgDataEvt const *) evt)->dataBuf,
                  basicMsg,
                  0
            );

            *payloadMsgName = basicMsg->_msgPayload;
            /* Extract the payload (if exists) since this buffer is going away the moment we get
             * into a state.  We'll figure out later if it's valid, right before we send an Ack */
            switch( *payloadMsgName ) {
               case _CBNoMsg:
                  WRN_printf( this->m_pLog, "No payload detected, this is probably an error" );
                  break;
               case _CBStatusPayloadMsg:
                  DBG_printf( this->m_pLog, "Status payload detected");
                  CBStatusPayloadMsg_read_delimited_from(
                        (void*)((LrgDataEvt const *) evt)->dataBuf,
                        &(payloadMsgUnion->statusPayload),
                        offset
                  );
                  break;
               case _CBVersionPayloadMsg:
                  DBG_printf( this->m_pLog, "Version payload detected");
                  break;
               case _CBBootModePayloadMsg:
                  DBG_printf( this->m_pLog, "BootMode payload detected");
                  break;
               default:
                  WRN_printf( this->m_pLog, "Unknown payload detected, this is probably an error");
                  break;
            }

            if (this->m_pDoneHandlerCBFunction) {
               this->m_pDoneHandlerCBFunction( *basicMsg, *payloadMsgName, *payloadMsgUnion );
            }
            status = CLI_ERR_NONE;
            break;
         }
         case MSG_ACK_TIMED_OUT_SIG:
            DBG_printf(this->m_pLog, "Got MSG_ACK_TIMED_OUT_SIG");
            status = CLI_ERR_MSG_ACK_WAIT_TIMED_OUT;
            break;
         case MSG_DONE_TIMED_OUT_SIG:
            DBG_printf(this->m_pLog, "Got MSG_DONE_TIMED_OUT_SIG");
            status = CLI_ERR_MSG_DONE_WAIT_TIMED_OUT;
            break;
         default:
            /* We should never get here */
            status = CLI_ERR_UNKNOWN;
            WRN_printf(
                  this->m_pLog,
                  "Unknown event in the queue; signal: %d, client error: 0x%08x",
                  evt->sig, status
            );
            break;
      }


      /* !!! Don't forget to garbage collect the event after processing the
       * event. After this, any data to which this pointer points to may not be
       * valid and should not be referenced. This should be done in the calling
       * function. */
      QF_gc(evt);
   }

   return( status );
}

/******************************************************************************/
ClientError_t ClientApi::waitForJobDone(
      CBBasicMsg *basicMsg,
      CBMsgName *payloadMsgName,
      CBPayloadMsgUnion_t *payloadMsgUnion
)
{
   ClientError_t status = CLI_ERR_NONE;
   DBG_printf(this->m_pLog,"Waiting for job to finish.");
   for (;;) {                         /* Beginning of the thread forever loop */

      status = pollForJobDone(basicMsg, payloadMsgName, payloadMsgUnion);
      if (CLI_ERR_MSG_WAITING_FOR_RESP != status ) {
         return status;
      }
      /* Sleep for 5 ms*/
      boost::this_thread::sleep(boost::posix_time::milliseconds(5));
   }
}

/******************************************************************************/
ClientError_t ClientApi::setReqCallBack(
      CB_ReqMsgHandler_t pCallbackFunction
)
{
   ClientError_t err = CLI_ERR_NONE;

   this->m_pReqHandlerCBFunction = pCallbackFunction;

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
   try {
      CBBasicMsg basicMsg;
      memset(&basicMsg, 0, sizeof(basicMsg));
      basicMsg._msgID = 0;
      basicMsg._msgName = _CBNoMsg;
      basicMsg._msgPayload = _CBNoMsg;
      basicMsg._msgReqProg = 0;
      basicMsg._msgRoute = _CB_NoRoute;
      basicMsg._msgType = _CB_Req;

      this->m_pReqHandlerCBFunction( basicMsg );
   } catch ( ... ) {
      err = CLI_ERR_INVALID_CALLBACK;
      cerr << "Invalid callback passed in by user" << endl;
      this->m_pReqHandlerCBFunction = NULL;
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

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
   try {
      CBBasicMsg basicMsg;
      memset(&basicMsg, 0, sizeof(basicMsg));
      basicMsg._msgID = 0;
      basicMsg._msgName = _CBNoMsg;
      basicMsg._msgPayload = _CBNoMsg;
      basicMsg._msgReqProg = 0;
      basicMsg._msgRoute = _CB_NoRoute;
      basicMsg._msgType = _CB_Ack;
      this->m_pAckHandlerCBFunction( basicMsg );
   } catch ( ... ) {
      err = CLI_ERR_INVALID_CALLBACK;
      cerr << "Invalid callback passed in by user" << endl;
      this->m_pReqHandlerCBFunction = NULL;
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

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
   try {
      CBBasicMsg basicMsg;
      memset(&basicMsg, 0, sizeof(basicMsg));
      basicMsg._msgID = 0;
      basicMsg._msgName = _CBNoMsg;
      basicMsg._msgPayload = _CBNoMsg;
      basicMsg._msgReqProg = 0;
      basicMsg._msgRoute = _CB_NoRoute;
      basicMsg._msgType = _CB_Ack;

      CBMsgName payloadName = _CBStatusPayloadMsg;
      CBPayloadMsgUnion_t payloadMsgUnion;
      payloadMsgUnion.statusPayload._errorCode = ERR_NONE;
      this->m_pDoneHandlerCBFunction( basicMsg, payloadName, payloadMsgUnion);
   } catch ( ... ) {
      err = CLI_ERR_INVALID_CALLBACK;
      cerr << "Invalid callback passed in by user" << endl;
      this->m_pReqHandlerCBFunction = NULL;
   }

   return( err );


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
      m_msgId( 0 ), m_bRequestProg( false ), m_msgRoute( _CB_EthCli )
{
   this->setLogging(log);
   this->qfSetup();                               /* Initialize QF framework. */
   this->setNewConnection(ipAddress, pRemPort, pLocPort);
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
      m_msgId( 0 ), m_bRequestProg( false ), m_msgRoute( _CB_Serial )
{
   this->setLogging(log);
   this->qfSetup();                               /* Initialize QF framework. */
   try {
      this->setNewConnection(dev_name, baud_rate, bDFUSEComm);
   } catch ( exception &e ) {
      ERR_printf(this->m_pLog,"Exception trying to open serial connection: %s", e.what());
      throw( &e );
   }
}

/******************************************************************************/
ClientApi::ClientApi( LogStub *log ) :
      m_pLog(NULL),
      m_pReqHandlerCBFunction(NULL),
      m_pAckHandlerCBFunction(NULL),
      m_pDoneHandlerCBFunction(NULL),
      m_msgId( 0 ), m_bRequestProg( false ), m_msgRoute( _CB_NoRoute )
{

   this->setLogging(log);                                      /* Set logging */
   this->qfSetup();                               /* Initialize QF framework. */

   /* This works */
//   DBG_printf(this->m_pLog,"Attempting to init serial...");
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
//   DBG_printf(this->m_pLog,"Attempting to write serial...");
//
//   serial->write_some("Test\n", 5);

   /* This also works.*/
//   DBG_printf(this->m_pLog,"Attempting to init UDP eth...");
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
//   DBG_printf(this->m_pLog,"Attempting to write UDP eth...");
//   eth->write_some("Test\n", 5);

   /* This also works.*/
//   DBG_printf(this->m_pLog,"Attempting to init UDP eth...");
//   Comm *m_comm;
//   try {
//      m_comm = new Comm(
//            this->m_pLog,
//            "172.27.0.75",
//            "1502",
//            "53432"
//      );
//   } catch ( ... ) {
//      ERR_printf(this->m_pLog,"Unable to connect to UDP. Exiting");
//      return; // Do a regular exit since this is critical
//   }
//   DBG_printf(this->m_pLog,"Attempting to write UDP eth...");
//   m_comm->write_some("Test\n", 5);

}

/******************************************************************************/
ClientApi::~ClientApi(  )
{
   delete[] this->m_pLog;
}



