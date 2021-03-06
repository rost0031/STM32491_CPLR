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
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <stdio.h>
#include <stdarg.h>

/* Boost includes */
#include <boost/date_time.hpp>

/* Lib includes */
#include "LogStub.h"
#include "ApiDbgModules.h"

/* App includes */
#include "Callbacks.hpp"
#include "EnumMaps.hpp"
#include "Logging.hpp"
#include "CliDbgModules.hpp"

#include <iostream>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
using namespace boost::log;

/* Compile-time called macros ------------------------------------------------*/
CLI_MODULE_NAME( CLI_DBG_MODULE_LOG )

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", DC3DbgLevel_t)
BOOST_LOG_ATTRIBUTE_KEYWORD(counter, "LineCounter", int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_stream, "LogStream", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(menu_stream, "MenuStream", std::string)

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**<! Attribute value tag type */
struct severity_tag;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void LOG_setDbgLvl( DC3DbgLevel_t dbgLvl )
{
   boost::log::core::get()->set_filter( severity >= dbgLvl );
}

/******************************************************************************/
void LOG_msgToStream(
      struct DC3BasicMsg *pBasicMsg,
      DC3PayloadMsgUnion_t *pPayloadUnion,
      std::stringstream& ss
)
{
   ss << setw(25) << setfill('-')
         << " " << enumToString(pBasicMsg->_msgName)
         << setw(4) << setfill(' ') << enumToString(pBasicMsg->_msgType)
         << " Msg Contents " << setw(25) << setfill('-') << "-"<< endl;

   ss << "|" << setw(15) << setfill(' ') << "Name"
         << "|" << setw(15) << "Payload"
         << "|" << setw(6)  << "Id"
         << "|" << setw(10)  << "Type"
         << "|" << setw(15)  << "Progress?"
         << "|" << setw(10) << "Route"  << "|" << endl;

   ss << "|" << setw(15) << enumToString(pBasicMsg->_msgName)
         << "|" << setw(15) << enumToString(pBasicMsg->_msgPayload)
         << "|" << setw(6)  << pBasicMsg->_msgID
         << "|" << setw(10)  << enumToString(pBasicMsg->_msgType)
         << "|" << setw(15)  << (pBasicMsg->_msgReqProg == 0 ? "No" : "Yes")
         << "|" << setw(10)  << enumToString(pBasicMsg->_msgRoute) << "|" << endl;

   if (NULL != pPayloadUnion && _DC3NoMsg != pBasicMsg->_msgPayload ) {
      ss << setw(25) << setfill(' ') << "|" << endl;
      ss << setw(25) << setfill(' ') << "*" << "---> ";
      switch( pBasicMsg->_msgPayload ) {
         case _DC3StatusPayloadMsg:
            ss << "ErrorCode: " << hex << "0x" << setw(8) << setfill('0')
            << pPayloadUnion->statusPayload._errorCode << dec << endl;
            break;
         case _DC3VersionPayloadMsg:
            break;
         case _DC3BootModePayloadMsg:
            ss << setw(15) << setfill(' ') << "ErrorCode: "
            << hex << "0x" << setw(8) << setfill('0')
            << pPayloadUnion->bootmodePayload._errorCode << dec << endl;
            ss << setw(25) << setfill(' ') << "*" << "---> ";
            ss << setw(15) << setfill(' ') << "Boot Mode: "
            << enumToString(pPayloadUnion->bootmodePayload._bootMode) << endl;
            break;
         default:
            break;
      }
   }
   ss << setw(79) << setfill('-') << "-";

}

/******************************************************************************/
void LOG_payloadMsgToStream(
      DC3MsgName_t payloadMsgName,
      DC3PayloadMsgUnion_t *pPayloadUnion,
      std::stringstream& ss
)
{

}


/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/


/**<! The formatting logic for the severity level */
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, DC3DbgLevel_t lvl)
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

   sink->set_filter( severity >= _DC3_DBG );               /* Set debug level */

   sink->set_filter(!expressions::has_attr(menu_stream));

   boost::log::core::get()->add_global_attribute(
         "TimeStamp",
         boost::log::attributes::local_clock()
   );
   boost::log::add_common_attributes();
   formatter format = expressions::stream
         << expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << " - "
         << expressions::attr<DC3DbgLevel_t, severity_tag>("Severity")  << ": "
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


   core::get()->add_sink(sink_menu);                        /* Get the logger */

   sink_menu->set_filter( severity >= _DC3_CON );          /* Set debug level */

   sink_menu->set_filter(expressions::has_attr(menu_stream));

   formatter format_menu = expressions::stream
         << expressions::smessage;
   sink_menu->set_formatter(format_menu);

   sink_menu->flush();

   return menu_log;
}


/******** Copyright (C) 2015 Harry Rostovtsev. All rights reserved *****END OF FILE****/
