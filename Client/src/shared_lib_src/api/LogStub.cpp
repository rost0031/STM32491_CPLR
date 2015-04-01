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
   m_pInternalLogHandlerCBFunction(NULL),
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
ClientError_t LogStub::setInternalLogCallBack(
      CB_LogHandler_t pCallbackFunction
)
{
   this->m_pInternalLogHandlerCBFunction = pCallbackFunction;
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
