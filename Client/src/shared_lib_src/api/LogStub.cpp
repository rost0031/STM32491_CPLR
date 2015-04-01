/**
 * @file    LogStub.cpp
 * This class is responsible for logging and output.
 *
 * This class is a logging stub that keeps track of the logging callbacks
 * passed in by the caller of the library and uses them to do the logging output
 * from all the modules.
 *
 * @date    03/31/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "LogStub.h"
#include "CallbackTypes.h"
#include <iostream>
#include <string>
#include <stdarg.h>
#include "LogHelper.h"
#include "ClientModules.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_LOG );
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private class methdos -----------------------------------------------------*/

/******************************************************************************/
LogStub::LogStub( void ) :
   m_pMsgHandlerCBFunction(NULL),
   m_pAckHandlerCBFunction(NULL),
   m_pLibLogHandlerCBFunction(NULL),
   m_AckLoggingEnabled(false),
   m_MsgLoggingEnabled(false)
{

}

/******************************************************************************/
LogStub::~LogStub( void ) {}

/******************************************************************************/
ClientError_t LogStub::setMsgCallBack(
      CB_MsgHandler_t pCallbackFunction
)
{
   ClientError_t err = CLI_ERR_NONE;

   this->m_pMsgHandlerCBFunction = pCallbackFunction;

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
   string tmp = "Testing Msg Callback passed in by user";
   try {
      this->m_pMsgHandlerCBFunction("Testing Msg Callback passed in by user", tmp.length());
   } catch ( ... ) {
      err = CLI_ERR_INVALID_CALLBACK;
      cerr << "Invalid callback passed in by user" << endl;
   }

   return( err );
}

/******************************************************************************/
ClientError_t LogStub::setAckCallBack(
      CB_MsgHandler_t pCallbackFunction
)
{
   this->m_pAckHandlerCBFunction = pCallbackFunction;
}

/******************************************************************************/
ClientError_t LogStub::setLibLogCallBack(
      CB_LibLogHandler_t pCallbackFunction
)
{
   ClientError_t err = CLI_ERR_NONE;

   this->m_pLibLogHandlerCBFunction = pCallbackFunction;

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
   string tmp = "Testing Library Logging Callback passed in by user";
   try {
      if ( NULL != this->m_pLibLogHandlerCBFunction ) {
         LOG_printf(this, "Successfully set callback in LogStub\n");
      } else {
         err = CLI_ERR_INVALID_CALLBACK;
         cerr << "Invalid Library Logging callback passed in by user" << endl;
      }
   } catch ( ... ) {
      err = CLI_ERR_INVALID_CALLBACK;
      cerr << "Invalid Library Logging callback passed in by user" << endl;
   }

   return( err );
}

/******************************************************************************/
void LogStub::log(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      ModuleSrc_t moduleSrc,
      ModuleId_t moduleId,
      char *fmt,
      ...
)
{
   if ( NULL != this->m_pLibLogHandlerCBFunction ) {
      char tmpBuffer[MAX_LOG_BUFFER_LEN];
      uint8_t tmpBufferIndex = 0;

      /* 3. Pass the va args list to get output to a buffer */
      va_list args;
      va_start(args, fmt);

      /* 4. Put the rest of the message behind the stuff already in the buffer*/
      tmpBufferIndex += vsnprintf(
            (char *)&tmpBuffer[tmpBufferIndex],
            MAX_LOG_BUFFER_LEN - tmpBufferIndex, // Account for the part of the buffer that was already written.
            fmt,
            args
      );
      va_end(args);

      /* 5. Call the callback that will pass the data to the caller of the library */
      this->m_pLibLogHandlerCBFunction(
            dbgLvl,
            pFuncName,
            wLineNumber,
            moduleSrc,
            moduleId,
            tmpBuffer
      );
   } else {
      cerr << "Callback is null" << endl;
   }

}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
