/**
 * @file    Logging.cpp
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

/* Includes ------------------------------------------------------------------*/
#include "Logging.h"
#include <boost/date_time.hpp>
#include <stdio.h>
#include <stdarg.h>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define MAX_MSG_BUFFER_LEN 1024  /**<Max buffer length for formatting messages*/

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void CON_output(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      char *fmt,
      ...
)
{
   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   boost::posix_time::ptime now = boost::date_time::not_a_date_time;
   now = boost::posix_time::microsec_clock::universal_time();
   std::string time = boost::posix_time::to_iso_extended_string( now );

   /* Temporary local buffer and index to compose the msg */
   char tmpBuffer[MAX_MSG_BUFFER_LEN];
   uint8_t tmpBufferIndex = 0;

   /* 2. Based on the debug level specified by the calling macro, decide what to
    * prepend (if anything). */
   switch (dbgLvl) {
      case DBG:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_BUFFER_LEN,
               "DBG:%s-%s():%d:",
               time.c_str(),
               pFuncName,
               wLineNumber
         );
         break;
      case LOG:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_BUFFER_LEN,
               "LOG:%s-%s():%d:",
               time.c_str(),
               pFuncName,
               wLineNumber
         );
         break;
      case WRN:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_BUFFER_LEN,
               "WRN:%s-%s():%d:",
               time.c_str(),
               pFuncName,
               wLineNumber
         );
         break;
      case ERR:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               MAX_MSG_BUFFER_LEN,
               "ERR:%s-%s():%d:",
               time.c_str(),
               pFuncName,
               wLineNumber
         );
         break;
      case ISR:
      case CON: // This is not used so don't prepend anything
      default:
         break;
   }

   /* 3. Pass the va args list to get output to a buffer, making sure to not
    * overwrite the prepended data. */
   va_list args;
   va_start(args, fmt);

   tmpBufferIndex += vsnprintf(
         &tmpBuffer[tmpBufferIndex],
         MAX_MSG_BUFFER_LEN - tmpBufferIndex, // Account for the part of the buffer that was already written.
         fmt,
         args
   );
   va_end(args);

   /* 4. Print directly to the console now. THIS IS A SLOW OPERATION! */
   fwrite(tmpBuffer, tmpBufferIndex, 1, stderr);
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/
/******************************************************************************/
Logging::Logging( void ) :
   m_pLog(NULL)
{
   cout << "Called Logging constructor" << endl;
   m_pLog = new LogStub();
}

/******************************************************************************/
Logging::~Logging( void ) {
   delete[] this->m_pLog;
}

/******************************************************************************/
ClientError_t Logging::setMsgCallBack(
      CB_MsgHandler_t pCallbackFunction
)
{
   cout << "Called Logging::setMsgCallBack" << endl;
   ClientError_t err = m_pLog->setMsgCallBack(pCallbackFunction);
   return( err );
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
