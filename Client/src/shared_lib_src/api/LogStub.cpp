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
#include <cstring>
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

/**< This "global" variable is used to keep track of all the modules which have
 * debugging enabled throughout the system.
 * @note 1: this var should not be accessed directly by the developer.
 * @note 2: this var should be set/cleared/checked by
 * #DBG_ENABLE_DEBUG_FOR_MODULE(), #DBG_DISABLE_DEBUG_FOR_MODULE(),
 * #DBG_TOGGLE_DEBUG_FOR_MODULE(), #DBG_CHECK_DEBUG_FOR_MODULE() macros only!
 */
uint32_t  glbDbgConfig = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private class methdos -----------------------------------------------------*/

/******************************************************************************/
LogStub::LogStub( void ) :
   m_pLibLogHandlerCBFunction(NULL),
   m_pDC3LogHandlerCBFunction(NULL)
{

}

/******************************************************************************/
LogStub::~LogStub( void ) {}



/******************************************************************************/
APIError_t LogStub::setLibLogCallBack(
      CB_LibLogHandler_t pCallbackFunction
)
{
   APIError_t err = API_ERR_NONE;

   this->m_pLibLogHandlerCBFunction = pCallbackFunction;

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
   string tmp = "Testing Library Logging Callback passed in by user";
   try {
      if ( NULL != this->m_pLibLogHandlerCBFunction ) {
         LOG_printf(this, "Successfully set callback in LogStub");
      } else {
         err = API_ERR_INVALID_CALLBACK;
         cerr << "Invalid Library Logging callback passed in by user" << endl;
      }
   } catch ( ... ) {
      err = API_ERR_INVALID_CALLBACK;
      cerr << "Invalid Library Logging callback passed in by user" << endl;
   }

   return( err );
}

/******************************************************************************/
APIError_t LogStub::setDC3LogCallBack(
      CB_DC3LogHandler_t pCallbackFunction
)
{
   APIError_t err = API_ERR_NONE;

   this->m_pDC3LogHandlerCBFunction = pCallbackFunction;

   /* This is a potentially dangerous call since the user could have passed in
    * garbage instead of a valid pointer */
   string tmp = "Testing DC3 Logging Callback passed in by user";
   try {
      if ( NULL != this->m_pDC3LogHandlerCBFunction ) {
         LOG_printf(this, "Successfully set callback in LogStub");
      } else {
         err = API_ERR_INVALID_CALLBACK;
         cerr << "Invalid DC3 Logging callback passed in by user" << endl;
      }
   } catch ( ... ) {
      err = API_ERR_INVALID_CALLBACK;
      cerr << "Invalid DC3 Logging callback passed in by user" << endl;
   }

   return( err );
}

/******************************************************************************/
void LogStub::enableLogForLibModule( ModuleId_t moduleId )
{
   DBG_ENABLE_DEBUG_FOR_MODULE( moduleId );
}

/******************************************************************************/
void LogStub::disableLogForLibModule( ModuleId_t moduleId )
{
   DBG_DISABLE_DEBUG_FOR_MODULE( moduleId );
}

/******************************************************************************/
void LogStub::enableLogForAllLibModules( void )
{
   DBG_ENABLE_DEBUG_FOR_ALL_MODULES();
}

/******************************************************************************/
void LogStub::disableLogForAllLibModules( void )
{
   DBG_DISABLE_DEBUG_FOR_ALL_MODULES();
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
   char tmpBuffer[MAX_LOG_BUFFER_LEN];
   if ( NULL != this->m_pLibLogHandlerCBFunction ) {

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
      /* No callback registered.  If WRN or greater, just cerr it */
      if (dbgLvl > LOG) {
         cerr << "[" << dbgLvl << "]" << pFuncName << "():" << wLineNumber << ":" << tmpBuffer << endl;
      }
   }
}

/******************************************************************************/
void LogStub::printDC3LogMsg(
      const char *msg
)
{
   if (NULL != this->m_pDC3LogHandlerCBFunction) {
      this->m_pDC3LogHandlerCBFunction( msg );
   }
}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
