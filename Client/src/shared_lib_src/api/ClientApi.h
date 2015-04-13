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
   boost::thread m_workerThread;          /**< Thread to start MainMgr and QF */

   /**
    * @brief   Helper function for QF startup.
    * An intenal helper function that gets called by all constructors to do the
    * actual setup of QF framework.
    * @param   None.
    * @return  None.
    */
   void qfSetup( void );
public:

   /**
    * @brief   Sets up a new ethernet connection
    * @param[in]   *ipAddress: pointer to the remote IP address string.
    * @param[in]   *pRemPort: pointer to the remote port number string.
    * @param[in]   *pLocPort: pointer to the local port number string.
    * @return  None.
    */
   void setNewConnection(
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
   void setNewConnection(
         const char *dev_name,
         int baud_rate,
         bool bDFUSEComm
   );

   /**
    * Kicks of the command to run.
    * @param[in]   None.
    * @return      None.
    */
   void run( void );

   /**
    * @brief   Waits for the MainMgr AO to finish.
    * @param   None.
    * @return  None.
    */
   void waitForDone( void );

   /**
    * @brief   Sets a new LogStub pointer.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @return: None.
    */
   void setLogging( LogStub *log );

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
