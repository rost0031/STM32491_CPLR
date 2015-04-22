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
MODULE_NAME( MODULE_LOG );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void CLI_ReqCallback(
      struct CBBasicMsg basicMsgStruct
)
{
//   cout << "In CLI_ReqCallback callback" << endl;
   LOG_out << "Sending Req msg:";
   stringstream ss;
   ss << "------------------- Req Msg Contents -------------------";
   CON_print(ss.str());
}

/******************************************************************************/
void CLI_AckCallback(
      struct CBBasicMsg basicMsgStruct
)
{
//   cout << "in CLI_AckCallback "<< endl;
   LOG_out << "Received Ack msg:";
   stringstream ss;
   ss << "------------------- Ack Msg Contents -------------------";
   CON_print(ss.str());
}

/******************************************************************************/
void CLI_DoneCallback(
      struct CBBasicMsg basicMsgStruct,
      CBMsgName payloadMsgName,
      CBPayloadMsgUnion_t payloadMsgUnion
)
{
//   cout << "in CLI_DoneCallback "<< endl;
   LOG_out << "Received Done msg:";
   stringstream ss;
   ss << "------------------- Done Msg Contents ------------------";
   CON_print(ss.str());
}

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

   LIB_out( dbgLvl, pFuncName, wLineNumber, moduleSrc, moduleId, tmpBuffer );
}

/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
