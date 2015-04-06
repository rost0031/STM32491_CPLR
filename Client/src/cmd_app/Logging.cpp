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
#include "LogStub.h"
#include "ClientModules.h"
#include "Callbacks.h"

#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/expressions.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/format/format_fwd.hpp>
#include <boost/log/support/date_time.hpp>
#include <iostream>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
using namespace boost::log;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", DBG_LEVEL_T)
BOOST_LOG_ATTRIBUTE_KEYWORD(counter, "LineCounter", int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", boost::posix_time::ptime)

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
//boost::mutex Logging::m_guard{};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/
/******************************************************************************/
Logging::Logging( void ) :
         m_pLog(NULL)
{
   /* Create a new LogStub instance */
   m_pLog = new LogStub();   /* No exceptions thrown from DLL so this is safe */

   /* Set the callback to use this class' logging function "log".  This can also
    * be done externally but we'll do it internally so main remains clean */
//   this->setLibLogCallBack( this->log );

   this->setLibLogCallBack( CLI_LibLogCallback );

   /* START: simple boost log example */

//
//   logging::core::get()->set_filter(
//         logging::trivial::severity >= logging::trivial::trace
//   );
//
//   src::severity_logger< DBG_LEVEL_T > slg;
//
//   BOOST_LOG_SEV(slg, DBG) << "A regular message";
   /* END: Simple boost log example */


   typedef sinks::asynchronous_sink<sinks::text_ostream_backend> text_sink;
   boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

   boost::shared_ptr<std::ostream> stream {
      &std::clog,
      boost::null_deleter{}
   };
   sink->locked_backend()->add_stream(stream);


   core::get()->add_sink(sink);                             /* Get the logger */

   sources::severity_logger<DBG_LEVEL_T> lg;

   sink->set_filter(                                       /* Set debug level */
         expressions::attr< DBG_LEVEL_T >("Severity") >= LOG
   );

   sink->set_filter(                                       /* Set debug level */
         severity >= LOG
   );

   sink->set_formatter(                                  /* Set output format */
         expressions::stream <<
         expressions::attr<DBG_LEVEL_T>("Severity") << ": "
         << expressions::smessage
    );

   BOOST_LOG(lg) << "note";
   BOOST_LOG_SEV(lg, DBG) << "dbg note";
   BOOST_LOG_SEV(lg, LOG) << "log note";
   BOOST_LOG_SEV(lg, WRN) << "wrn note";
   BOOST_LOG_SEV(lg, ERR) << "err note";
   sink->flush();

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
   cout << "Setting callback in Logging::setMsgCallBack" << endl;
   //   ClientError_t err = m_pLog->setMsgCallBack(pCallbackFunction);
   //   return( err );
}

/******************************************************************************/
ClientError_t Logging::setLibLogCallBack(
      CB_LibLogHandler_t pCallbackFunction
)
{
   DBG_printf(this, "Setting a new callback for library logging\n");
   ClientError_t err = m_pLog->setLibLogCallBack(pCallbackFunction);
   if (CLI_ERR_NONE != err ) {
      cerr << "Failed to set logging callback. Error: 0x" << setfill('0') << setw(8) << std::hex << err << endl;
   }
   return( err );
}

/******************************************************************************/
LogStub* Logging::getLogStubPtr( void )
{
   return( this->m_pLog );
}

/******************************************************************************/
void Logging::log(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      ModuleSrc_t moduleSrc,
      ModuleId_t moduleId,
      char *fmt,
      ...
)
{

   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   boost::posix_time::ptime now = boost::date_time::not_a_date_time;
   now = boost::posix_time::microsec_clock::local_time();
   std::string time = boost::posix_time::to_iso_extended_string( now );

   /* Temporary local buffer and index to compose the msg */
   char tmpBuffer[MAX_LOG_BUFFER_LEN];
   uint8_t tmpBufferIndex = 0;


   /* 2. Get a string version of the calling module name */
   string moduleName;
   switch( moduleId ) {
      case MODULE_GEN: moduleName = "M_GEN"; break;
      case MODULE_SER: moduleName = "M_SER"; break;
      case MODULE_ETH: moduleName = "M_ETH"; break;
      case MODULE_MGR: moduleName = "M_MGR"; break;
      case MODULE_LOG: moduleName = "M_LOG"; break;
      case MODULE_EXT: moduleName = "M_EXT"; break;
      case MODULE_JOB: moduleName = "M_JOB"; break;
      case MODULE_API: moduleName = "M_API"; break;
      default:         moduleName = "M_???"; break;
   }

   /* 3. Get the string version of debug level */
   string logLevel;
   switch (dbgLvl) {
      case DBG: logLevel = "DBG"; break;
      case LOG: logLevel = "LOG"; break;
      case WRN: logLevel = "WRN"; break;
      case ERR: logLevel = "ERR"; break;
      case CON: logLevel = "CON"; break;
      case ISR: logLevel = "ISR"; break;
      default:  logLevel = "???"; break;
   }

   /* 4. Get the module source */
   string moduleSrcName;
   switch (moduleSrc) {
      case SRC_CLI_LIB:  moduleSrcName = "S_LIB"; break;
      case SRC_DC3_APPL: moduleSrcName = "S_DC3_APPL"; break;
      case SRC_DC3_BOOT: moduleSrcName = "S_DC3_BOOT"; break;
      case SRC_CLI_EXT:  moduleSrcName = "S_EXT"; break;
      default:  moduleSrcName = "S_???"; break;
   }


   /* 4. Write the origin, module, debug level, time, function, and line number
    * before the rest of the data gets appended. */
   tmpBufferIndex += snprintf(
         tmpBuffer,
         MAX_LOG_BUFFER_LEN,
         "%s:%s-%s:%s-%s():%d:",
         moduleSrcName.c_str(),
         moduleName.c_str(),
         logLevel.c_str(),
         time.c_str(),
         pFuncName,
         wLineNumber
   );

   /* 5. Pass the va args list to get output to a buffer, making sure to not
    * overwrite the prepended data. Write the actual msg that was sent to the
    * function via va_args. */
   va_list args;
   va_start(args, fmt);

   tmpBufferIndex += vsnprintf(
         &tmpBuffer[tmpBufferIndex],
         MAX_LOG_BUFFER_LEN - tmpBufferIndex, // Account for the part of the buffer that was already written.
         fmt,
         args
   );
   va_end(args);

   /* 4. Print directly to the console now. */
   fwrite(tmpBuffer, tmpBufferIndex, 1, stderr);
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
