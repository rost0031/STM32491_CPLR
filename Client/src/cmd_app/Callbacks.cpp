/**
 * @file    Callbacks.cpp
 * This file contains all the callbacks used by the Client.  This allows
 * different implementation of how to print and log info from the coupler
 * and the client.
 *
 * @date    03/26/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include <iostream>                              /* For cout and endl support */
#include <stdarg.h>
#include "Callbacks.h"
#include "Logging.h"
#include "LogStub.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void CLI_MsgCallback( char* message, int len )
{
   cout << "In CLI_MsgCallback callback" << endl;
}

/******************************************************************************/
void CLI_AckCallback( char* message, int len )
{
   cout << "in CLI_AckCallback, len=" << len << endl;
}

/******************************************************************************/
//void CLI_DC3LogMsgCallback(
//      DBG_LEVEL_T logLevel,
//      CBErrorCode err,
//      char *message
//)
//{
//    cout << "in CLI_DC3LogMsgCallback, err=" << err << endl;
//}

//sources::severity_logger<DBG_LEVEL_T> lg;

/******************************************************************************/
void CLI_LibLogCallback(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      ModuleSrc_t moduleSrc,
      ModuleId_t moduleId,
      char *fmt,
      ...
)
{

//   sources::severity_logger<DBG_LEVEL_T> lg;

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

//   BOOST_LOG_SEV(lg::get(), dbgLvl) << tmpBuffer;

//   switch( dbgLvl ) {
//      case DBG:
//         DBG_printf_b << tmpBuffer;
//         break;
//
//      case LOG: LOG_printf_b << tmpBuffer; break;
//      case WRN: WRN_printf_b << tmpBuffer; break;
//      case ERR: ERR_printf_b << tmpBuffer; break;
//      case CON: ERR_printf_b << tmpBuffer; break;
//      case ISR: ERR_printf_b << tmpBuffer; break;
//      default:  BOOST_LOG_TRIVIAL(debug) << tmpBuffer; break;
//   }
   BOOST_LOG_SEV(my_logger::get(), dbgLvl) << tmpBuffer;
}

/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
