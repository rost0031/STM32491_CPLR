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


/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/* Exported classes ----------------------------------------------------------*/
/**
 * @class ClientApi
 *
 * @brief This class handles setting up the interface to the client using
 * either a cmd line interface or a graphical interface.
 */
class CLIENT_DLL ClientApi {

private:

   CB_MsgHandler_t m_pMsgHandlerCBFunction;
   CB_MsgHandler_t m_pAckHandlerCBFunction;
   CB_LogHandler_t m_pInternalLogHandlerCBFunction;

public:

   /**
    * Kicks of the command to run.
    * @param[in]   None.
    * @return      None.
    */
   void run();

   /**
    * Constructor that sets up the upd ethernet socket
    * @param [in]  None.
    * @return      None.
    */
   ClientApi( );

   /**
    * Destructor (default) that closes the UDP socket
    *
    * @param       None.
    * @return      None.
    */
   ~ClientApi( void );

   /**
    * This method sets a callback to handle all and any general Redwood msgs.
    *
    * @param  [in]  pCallbackFunction: a cbRedwoodMsgHandler pointer to the
    * callback function that is implemented outside the library.
    *
    * @return None:
    */
   const void setMsgCallBack( CB_MsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle just the Ack msgs from Redwood since
    * the user may or may not want to see these.
    *
    * @param  [in]  pCallbackFunction: a cbRedwoodMsgHandler pointer to the
    * callback function that is implemented outside the library.
    *
    * @return None:
    */
   const void setAckCallBack( CB_MsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle the internal logging from the client.
    *
    * @param  [in]  pCallbackFunction: a cbRedwoodLogHandler pointer to the
    * callback function that is implemented outside the library.
    *
    * @return None:
    */
   const void setInternalLogCallBack( CB_LogHandler_t pCallbackFunction );

};

#endif                                                        /* CLIENTAPI_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
