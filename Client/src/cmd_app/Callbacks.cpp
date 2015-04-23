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
#include <boost/format.hpp>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_LOG );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void prettyPrintBasicMsg( struct CBBasicMsg basicMsgStruct, stringstream& ss );

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
static void prettyPrintBasicMsg( struct CBBasicMsg basicMsgStruct, stringstream& ss )
{
//   ss << "------------------------------ "
//         << setw(4) << enumToString(basicMsgStruct._msgType)
//         << " Msg Contents ------------------------------" << endl;
//
//   ss << "|" << setw(15) << "Name"
//         << "|" << setw(5)  << "Id"
//         << "|" << setw(5)  << "Type"
//         << "|" << setw(15) << "Payload"
//         << "|" << setw(9)  << "Progress?"
//         << "|" << setw(10) << "Route" << endl;
//
//   ss << "|" << setw(15) << enumToString(basicMsgStruct._msgName)
//         << "|" << setw(5)  << basicMsgStruct._msgID
//         << "|" << setw(5)  << enumToString(basicMsgStruct._msgType)
//         << "|" << setw(15) << enumToString(basicMsgStruct._msgPayload)
//         << "|" << setw(11)  << (basicMsgStruct._msgReqProg == 0 ? "No" : "Yes")
//         << "|" << setw(10)  << enumToString(basicMsgStruct._msgRoute) << "|";

   ss << setw(30) << setfill('-') << " "
         << setw(4) << setfill(' ') << enumToString(basicMsgStruct._msgType)
         << " Msg Contents " << setw(30) << setfill('-') << "-"<< endl;

   ss << "|" << setw(15) << setfill(' ') << "Name"
         << "|" << setw(6)  << "Id"
         << "|" << setw(10)  << "Type"
         << "|" << setw(15) << "Payload"
         << "|" << setw(15)  << "Progress?"
         << "|" << setw(10) << "Route"  << "|" << endl;

   ss << "|" << setw(15) << enumToString(basicMsgStruct._msgName)
         << "|" << setw(6)  << basicMsgStruct._msgID
         << "|" << setw(10)  << enumToString(basicMsgStruct._msgType)
         << "|" << setw(15) << enumToString(basicMsgStruct._msgPayload)
         << "|" << setw(15)  << (basicMsgStruct._msgReqProg == 0 ? "No" : "Yes")
         << "|" << setw(10)  << enumToString(basicMsgStruct._msgRoute) << "|";

}

/******************************************************************************/
void CLI_ReqCallback(
      struct CBBasicMsg basicMsgStruct
)
{
//   cout << "In CLI_ReqCallback callback" << endl;
   LOG_out << "Sending Req msg:";

//         this->m_basicMsg._msgName     = _CBGetBootModeMsg;
//   this->m_basicMsg._msgPayload  = _CBNoMsg;
//   this->m_basicMsg._msgType     = _CB_Req;
//
//   /* Common settings for most messages */
//   this->m_basicMsg._msgID       = this->m_msgId;
//   this->m_basicMsg._msgReqProg  = (unsigned long)this->m_bRequestProg;
//   this->m_basicMsg._msgRoute    = this->m_msgRoute;
//   CON_print(ss.str());
}

/******************************************************************************/
void CLI_AckCallback(
      struct CBBasicMsg basicMsgStruct
)
{
//   cout << "in CLI_AckCallback "<< endl;
   LOG_out << "Received Ack msg:";
   stringstream ss;
   prettyPrintBasicMsg(basicMsgStruct, ss);

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
   ss << "------------------- Done Msg Contents ------------------" << endl;
   ss << "Name" << "  " <<  "Id"<< "  " <<  "Type" << "  " <<  "Payload"
         << "  " <<  "Progress?"<< "  " <<  "Route" << endl;
   ss << basicMsgStruct._msgName << "  " <<  basicMsgStruct._msgID << "  "
         << basicMsgStruct._msgType << "  " <<  basicMsgStruct._msgPayload
         << "  " <<  basicMsgStruct._msgReqProg  << "  " <<  basicMsgStruct._msgRoute;
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

/******************************************************************************/
void CLI_DC3LogCallback(
      const char *msg
)
{
   /* Break apart the log msg from DC3 so we can make it look like the rest of
    * the log messages. Its format looks like this:
    * DBG-01:00:58:069-CommStackMgr_Idle():385:No payload detected
    * */
   string message(msg);

   size_t pos = 0;

   /* Get the debug level of the message */
   DBG_LEVEL_T dbgLvl = CON;
   string dbgLvlStr = "";
   string delim = "-";
   if ( (pos = message.find( delim )) != string::npos) {

      dbgLvlStr = message.substr(0, pos);

      if ( string::npos != dbgLvlStr.find("DBG") ) {
         dbgLvl = DBG;
         message.erase(0, pos + delim.length());
      } else if ( string::npos != dbgLvlStr.find("LOG") ) {
         dbgLvl = LOG;
         message.erase(0, pos + delim.length());
      } else if ( string::npos != dbgLvlStr.find("WRN") ) {
         dbgLvl = WRN;
         message.erase(0, pos + delim.length());
      } else if ( string::npos != dbgLvlStr.find("ERR") ) {
         dbgLvl = ERR;
         message.erase(0, pos + delim.length());
      } else if ( string::npos != dbgLvlStr.find("ISR") ) {
         dbgLvl = ISR;
         message.erase(0, pos + delim.length());
      } else {
         WRN_out << "Unable to figure out debug level of a DC3 log msg";
      }
//      DBG_out << "Found debug level: " << dbgLvlStr;
   }

   /* Get the DC3 internal timestamp of the message */
   string timestamp = "";
   delim = "-";
   if ( (pos = message.find( delim )) != string::npos) {
      timestamp = message.substr(0, pos);
      message.erase(0, pos + delim.length());
//      DBG_out << "Found timestamp: " << timestamp;
   }

   /* Get the DC3 internal function name of the message */
   string funcName = "";
   delim = "():";
   if ( (pos = message.find( delim )) != string::npos) {
      funcName = message.substr(0, pos);
      message.erase(0, pos + delim.length());
//      DBG_out << "Found funcName: " << funcName;
   }

   /* Get the DC3 internal line number of the message */
   string lineNumber = "";
   int lineNum = 0;
   delim = ":";
   if ( (pos = message.find( delim )) != string::npos) {
      lineNumber = message.substr(0, pos);
      lineNum = atoi(lineNumber.c_str());
      message.erase(0, pos + delim.length());
//      DBG_out << "Found lineNumber: " << lineNum;
   }

   /* The rest is the message but we need to trim the newline */
   delim = "\n";
   string msgStr = "";
   if ( (pos = message.find( delim )) != string::npos) {
      msgStr = message.substr(0, pos);
//      DBG_out << "Found rest of msg:" << msgStr;
   } else {
      msgStr = message;
   }

   DC3_out( dbgLvl, funcName.c_str(), lineNum, timestamp.c_str(), msgStr.c_str() );
}


/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
