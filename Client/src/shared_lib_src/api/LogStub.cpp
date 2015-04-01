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
/* Namespaces ----------------------------------------------------------------*/
using namespace std;
/* Compile-time called macros ------------------------------------------------*/

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
      this->m_pLibLogHandlerCBFunction(LOG, __func__, __LINE__, MODULE_LOGGING, "Testing Msg Callback passed in by user\n");
   } catch ( ... ) {
      err = CLI_ERR_INVALID_CALLBACK;
      cerr << "Invalid Library Logging callback passed in by user" << endl;
   }

   return( err );
}

/******************************************************************************/
void LogStub::LOG_printf(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      ModuleId_t module,
      char *fmt,
      ...
)
{
   if ( NULL != this->m_pLibLogHandlerCBFunction ) {
      char tmpBuffer[MAX_LOG_BUFFER_LEN];
      uint8_t tmpBufferIndex = 0;

      /* 1. Pass the va args list to get output to a buffer */
      va_list args;
      va_start(args, fmt);

      /* 2. Print the actual user supplied data to the buffer and set the length */
      tmpBufferIndex += vsnprintf(
            (char *)&tmpBuffer[tmpBufferIndex],
            MAX_LOG_BUFFER_LEN - tmpBufferIndex, // Account for the part of the buffer that was already written.
            fmt,
            args
      );
      va_end(args);

      /* Call the callback that will pass the data to the caller of the library */
      this->m_pLibLogHandlerCBFunction(
            dbgLvl,
            pFuncName,
            wLineNumber,
            module,
            tmpBuffer
      );
   } else {
      cerr << "Callback is null" << endl;
   }

}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
