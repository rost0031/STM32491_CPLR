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
#include "EnumMaps.h"


#include <iostream>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
using namespace boost::log;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT )


BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", DBG_LEVEL_T)
BOOST_LOG_ATTRIBUTE_KEYWORD(counter, "LineCounter", int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_stream, "LogStream", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(menu_stream, "MenuStream", std::string)

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

// Attribute value tag type
struct severity_tag;

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, DBG_LEVEL_T lvl)
{
    static const char* const str[] =
    {
        "DBG",
        "LOG",
        "WRN",
        "ERR",
        "CON",
        "ISR"
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}


/******************************************************************************/
BOOST_LOG_GLOBAL_LOGGER_INIT(my_logger, logger_t)
{
   logger_t lg;

   typedef sinks::asynchronous_sink<sinks::text_ostream_backend> text_sink;
   boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

   boost::shared_ptr<std::ostream> stream {
      &std::clog,
      boost::null_deleter{}
   };
   sink->locked_backend()->add_stream(stream);


   core::get()->add_sink(sink);                             /* Get the logger */

   sink->set_filter(                                       /* Set debug level */
         severity >= DBG
   );

   sink->set_filter(!expressions::has_attr(menu_stream));

   boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
   boost::log::add_common_attributes();
   formatter format = expressions::stream
         << expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << " - "
         << expressions::attr<DBG_LEVEL_T, severity_tag>("Severity")  << ": "
         << expressions::smessage;
   sink->set_formatter(format);
   sink->flush();

   return lg;
}

/******************************************************************************/
BOOST_LOG_GLOBAL_LOGGER_INIT(my_menu, logger_t)
{
   logger_t menu_log;

   typedef sinks::asynchronous_sink<sinks::text_ostream_backend> text_sink_menu;
   boost::shared_ptr<text_sink_menu> sink_menu = boost::make_shared<text_sink_menu>();

   boost::shared_ptr<std::ostream> stream {
      &std::clog,
      boost::null_deleter{}
   };
   sink_menu->locked_backend()->add_stream(stream);


   core::get()->add_sink(sink_menu);                             /* Get the logger */

   sink_menu->set_filter(                                       /* Set debug level */
         severity >= CON
   );

   sink_menu->set_filter(expressions::has_attr(menu_stream));

   formatter format_menu = expressions::stream
         << expressions::smessage;
   sink_menu->set_formatter(format_menu);

   sink_menu->flush();

   return menu_log;
}
/******************************************************************************/
Logging::Logging( void ) :
         m_pLog(NULL)
{
   /* Create a new LogStub instance */
   m_pLog = new LogStub();   /* No exceptions thrown from DLL so this is safe */

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

//   sources::severity_logger_mt<DBG_LEVEL_T>& lg = logger::get();
   LOG_out << "Testing logging from Logging constructor";



   /* Set the callback to use this class' logging function "log".  This can also
    * be done externally but we'll do it internally so main remains clean */
   //   this->setLibLogCallBack( this->log );

   this->setLibLogCallBack( CLI_LibLogCallback );

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
   DBG_out << "Setting callback in Logging::setMsgCallBack";
   //   ClientError_t err = m_pLog->setMsgCallBack(pCallbackFunction);
   //   return( err );
}

/******************************************************************************/
ClientError_t Logging::setLibLogCallBack(
      CB_LibLogHandler_t pCallbackFunction
)
{
   DBG_out << "Setting a new callback for library logging";
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
//   fwrite(tmpBuffer, tmpBufferIndex, 1, stderr);
//   BOOST_LOG_SEV(my_logger, dbgLvl) << tmpBuffer;
//   ERR_printf_b << tmpBuffer;
   BOOST_LOG_SEV(my_logger::get(), dbgLvl) << tmpBuffer;
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
