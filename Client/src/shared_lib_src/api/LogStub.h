/**
 * @file    Logging.h
 * This class is responsible for logging and output.
 *
 * This class contains functions and methods which should be passed in as
 * callbacks to all the other classes in the dll and the program so that there's
 * consistent logging/debugging system throughout the entire program.
 *
 * @date    03/31/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGSTUB_H_
#define LOGSTUB_H_

/* Includes ------------------------------------------------------------------*/
#include "CBSharedDbgLevels.h"
#include "ClientModules.h"
#include "ClientErrorCodes.h"
#include "CallbackTypes.h"
#include <cstddef>

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


/* Exported defines ----------------------------------------------------------*/
#define MAX_LOG_BUFFER_LEN 1024  /**<Max buffer length for formatting messages*/
/* Exported macros -----------------------------------------------------------*/

/**
 * @brief   Defines a module for grouping debugging functionality.
 *
 * @description
 * Macro to be placed at the top of each C/C++ module to define the single
 * instance of the module name string to be used in printing of debugging
 * information.
 *
 * @param[in] @c name_: ModuleId_t enum representing the module.
 *
 * @note 1: This macro should __not__ be terminated by a semicolon.
 * @note 2: This macro MUST be present in the file if DBG_printf() or
 * LOG_printf() functions are called.  The code will not compile without this.
 */
#define DBG_DEFINE_THIS_MODULE( name_ ) \
      static ModuleId_t const Q_ROM DBG_this_module_ = name_;

/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/* Exported classes ----------------------------------------------------------*/
/**
 * @class LogStub
 *
 * @brief This class handles all internal logging from the client.
 *
 * This class gets instantiated by the ClientApi class.  It gets initialized by
 * passing in callback functions from the caller of the Client dll/so and then
 * gets passed in to all the other modules of the client dll/so.  All other
 * modules can then use this class to do transparent logging.
 */
class CLIENT_DLL LogStub {

private:
   CB_MsgHandler_t m_pMsgHandlerCBFunction; /**< Callback for handling Done and Progress msgs */
   CB_MsgHandler_t m_pAckHandlerCBFunction; /**< Callback for handling Ack msgs */
   CB_LibLogHandler_t m_pLibLogHandlerCBFunction;  /**< Callback for handling library logging */
   bool m_AckLoggingEnabled; /**< Allows enabling/disabling of logging of Ack msgs */
   bool m_MsgLoggingEnabled; /**< Allows enabling/disabling of logging of Done and Prog msgs */
public:



   /**
    * @brief   Default constructor.
    * This constructor doesn't set up any logging.  There will be complete
    * silence from the Client library unless "" is called to set up the callback
    * functions for logging.
    *
    * @param   None.
    * @return  None.
    */
   LogStub( void );

   /**
    * @brief   Default destructor
    */
   ~LogStub( void );

   /**
    * This method sets a callback to handle all and any general Done and Prog msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_MsgHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return ClientError_t:
    *    @arg CLI_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   ClientError_t setMsgCallBack( CB_MsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle all and any general Ack msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_MsgHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return ClientError_t:
    *    @arg CLI_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   ClientError_t setAckCallBack( CB_MsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle the internal logging from the client.
    *
    * @param  [in]  pCallbackFunction: a CB_LibLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return ClientError_t:
    *    @arg CLI_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   ClientError_t setLibLogCallBack( CB_LibLogHandler_t pCallbackFunction );

   void LOG_printf(
         DBG_LEVEL_T dbgLvl,
         const char *pFuncName,
         int wLineNumber,
         ModuleId_t module,
         char *fmt,
         ...
   );
};

#endif                                                          /* LOGSTUB_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
