/**
 * @file    ClientApi.h
 * Declarations for class and functions used by the Client API.
 *
 * @date    03/05/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CLIENTAPI_H_
#define CLIENTAPI_H_

/* Includes ------------------------------------------------------------------*/
#include <string>
#include "CallbackTypes.h"
#include "LogStub.h"

#include <boost/thread.hpp>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/
#ifdef _WIN32
#ifdef BUILDING_CLIENT_DLL
#define CLIENT_DLL __declspec(dllexport)
#else
#define CLIENT_DLL __declspec(dllimport)
#endif
#else
#define CLIENT_DLL
#define __stdcall
#endif

#ifdef __cplusplus
}
#endif

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   This function starts the MainMgr AO and the QF framework.
 */
void runMainMgr( void );

/* Exported classes ----------------------------------------------------------*/
/**
 * @class ClientApi
 *
 * @brief This class handles setting up the interface to the client using
 * either a cmd line interface or a graphical interface.
 */
class CLIENT_DLL ClientApi {

private:
   LogStub *m_pLog;         /**< Pointer to LogStub instance used for logging */
   CB_ReqLogHandler_t m_pReqHandlerCBFunction; /**< Callback for handling Req msgs */
   CB_AckLogHandler_t m_pAckHandlerCBFunction; /**< Callback for handling Ack msgs */
   CB_DoneLogHandler_t m_pDoneHandlerCBFunction; /**< Callback for handling Done msgs */

   unsigned int m_msgId;   /* Msg ID incrementing counter for unique msg ids. */
   bool m_bRequestProg;     /* Flag to see if progress messages are requested */
   CBMsgRoute m_msgRoute; /* This is set based on the connection used (UDP vs Serial) */

   boost::thread m_workerThread;          /**< Thread to start MainMgr and QF */

   /* All the different msg structures that exist */
   struct CBBasicMsg          m_basicMsg;
   struct CBStatusPayloadMsg  m_statusPayloadMsg;
   struct CBVersionPayloadMsg m_versionPayloadMsg;
   struct CBRunModePayloadMsg m_runmodePayloadMsg;

   uint8_t dataBuf[1000];
   int dataLen;
   /**
    * @brief   Helper function for QF startup.
    * An intenal helper function that gets called by all constructors to do the
    * actual setup of QF framework.
    * @param   None.
    * @return  None.
    */
   void qfSetup( void );

   /**
    * @brief   Polls for new events in the queue from MainMgr AO.
    * @param   None
    * @return: QEvt pointer to the event that is now in the queue.
    */
   ClientError_t pollForJobDone(
         CBBasicMsg *basicMsg,
         CBMsgName *payloadMsgName,
         CBPayloadMsgUnion_t *payloadMsgUnion
   );

   /**
    * @brief   Blocks while waiting for new events in queue from MainMgr AO.
    * @param:  None
    * @return: QEvt pointer to the event that is now in the queue.
    */
   ClientError_t waitForJobDone(
         CBBasicMsg *basicMsg,
         CBMsgName *payloadMsgName,
         CBPayloadMsgUnion_t *payloadMsgUnion
   );

public:

   /****************************************************************************
    *                    DC3 requests
    ***************************************************************************/
   ClientError_t DC3_getMode(CBErrorCode *status, CBBootMode *mode);



   /****************************************************************************
    *                    Client control functionality
    ***************************************************************************/
   /**
    * @brief   Sets up a new ethernet connection
    * @param[in]   *ipAddress: pointer to the remote IP address string.
    * @param[in]   *pRemPort: pointer to the remote port number string.
    * @param[in]   *pLocPort: pointer to the local port number string.
    * @return  None.
    */
   ClientError_t setNewConnection(
         const char *ipAddress,
         const char *pRemPort,
         const char *pLocPort
   );

   /**
    * @brief   Sets up a new serial connection
    * @param[in]   dev_name: serial device name.  /dev/ttyS10 or COMX
    * @param[in]   baud_rate: serial baud rate.
    * @param[in]   bDFUSEComm: bool that specifies whether to set up serial
    * for DFUSE or regular serial communication.
    *   @arg  TRUE: set up serial for DFUSE
    *   @arg  FALSE: set up serial for regular serial comms.
    * @return  None.
    */
   ClientError_t setNewConnection(
         const char *dev_name,
         int baud_rate,
         bool bDFUSEComm
   );

   /**
    * @breif   Starts the client.
    *
    * This must be called after a connection has been set up and before any
    * commands are executed.
    * @param[in]   None.
    * @return      None.
    */
   void start( void );

   /**
    * @brief   Waits for the MainMgr AO to finish.
    * @param   None.
    * @return  None.
    */
   void waitForStop( void );

   /**
    * @brief   Sets a new LogStub pointer.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @return: None.
    */
   void setLogging( LogStub *log );

   /**
    * @brief   Stops the client api state machine
    * @param   None.
    * @return: None.
    */
   void stop( void );

   /**
    * @brief   Starts a new job
    * @param   None.
    * @return  None.
    */
   void startJob( void );

   /**
    * This method sets a callback to handle Req msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_ReqLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return ClientError_t:
    *    @arg CLI_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   ClientError_t setReqCallBack( CB_ReqLogHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle Ack msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_AckLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return ClientError_t:
    *    @arg CLI_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   ClientError_t setAckCallBack( CB_AckLogHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle Done msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_DoneLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return ClientError_t:
    *    @arg CLI_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   ClientError_t setDoneCallBack( CB_DoneLogHandler_t pCallbackFunction );

   /**
    * Constructor that sets up logging and an ethernet connection.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @param[in]   *ipAddress: pointer to the remote IP address string.
    * @param[in]   *pRemPort: pointer to the remote port number string.
    * @param[in]   *pLocPort: pointer to the local port number string.
    * @return      None.
    */
   ClientApi(
         LogStub *log,
         const char *ipAddress,
         const char *pRemPort,
         const char *pLocPort
   );

   /**
    * Constructor that sets up logging and a serial connection.
    * @param[in]   dev_name: serial device name.  /dev/ttyS10 or COMX
    * @param[in]   baud_rate: serial baud rate.
    * @param[in]   bDFUSEComm: bool that specifies whether to set up serial
    * for DFUSE or regular serial communication.
    *   @arg  TRUE: set up serial for DFUSE
    *   @arg  FALSE: set up serial for regular serial comms.
    * @return      None.
    */
   ClientApi(
         LogStub *log,
         const char *dev_name,
         int baud_rate,
         bool bDFUSEComm
   );

   /**
    * Constructor that sets up logging only.  No connection is made.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @return      None.
    */
   ClientApi( LogStub *log );

   /**
    * Destructor (default) that closes the UDP socket
    *
    * @param       None.
    * @return      None.
    */
   ~ClientApi( void );

};

#endif                                                        /* CLIENTAPI_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
