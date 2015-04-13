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

/**<! Attribute value tag type */
struct severity_tag;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LOG_setDbgLvl( DBG_LEVEL_T dbgLvl )
{
   boost::log::core::get()->set_filter( severity >= dbgLvl );
}
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/


/**<! The formatting logic for the severity level */
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

   sink->set_filter( severity >= DBG );                    /* Set debug level */

   sink->set_filter(!expressions::has_attr(menu_stream));

   boost::log::core::get()->add_global_attribute(
         "TimeStamp",
         boost::log::attributes::local_clock()
   );
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


   core::get()->add_sink(sink_menu);                        /* Get the logger */

   sink_menu->set_filter( severity >= CON );               /* Set debug level */

   sink_menu->set_filter(expressions::has_attr(menu_stream));

   formatter format_menu = expressions::stream
         << expressions::smessage;
   sink_menu->set_formatter(format_menu);

   sink_menu->flush();

   return menu_log;
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
