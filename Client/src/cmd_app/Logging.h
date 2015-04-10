/**
 * @file    Logging.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGGING_H_
#define LOGGING_H_

/* Includes ------------------------------------------------------------------*/
#include "CBSharedDbgLevels.h"
#include "LogStub.h"
#include "EnumMaps.h"
#include <iomanip>

#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/format/format_fwd.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/thread/thread.hpp>



/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Wrapper around the exit() function that allows log flushing before exit.
 * This macro has to be called instead of the usual exit() function in order to
 * allow the boost log async logging to finish flushing its internal queue.
 *
 * @param [in] returnValue: value to send to the exit() function.
 * @return  None.
 */
#define EXIT_LOG_FLUSH( returnValue ) \
   do { \
      boost::this_thread::sleep(boost::posix_time::milliseconds(100)); \
      exit(returnValue); \
   } while(0)

/**
 * @brief  Stream macro to do DBG level logging
 *
 * This macro should be called to log things to the console with DBG severity
 * level.  It should be used as a stream (aka cout or cerr).
 *
 * @note: Do not use "\n" or "<<endl" at the end of the stream since the boost
 * logger automatically appends it (with no way to turn it off, it seems).
 */
#define DBG_out   BOOST_LOG_SEV(my_logger::get(), DBG)                        \
      << "[" << "Ext" << "]"                                                  \
      << ":["<< maskableEnumToString(this_module_) << "]"                     \
      << __func__ << "():" << __LINE__ << " - "

/**
 * @brief  Stream macro to do LOG level logging
 *
 * This macro should be called to log things to the console with LOG severity
 * level.  It should be used as a stream (aka cout or cerr).
 *
 * @note: Do not use "\n" or "<<endl" at the end of the stream since the boost
 * logger automatically appends it (with no way to turn it off, it seems).
 */
#define LOG_out   BOOST_LOG_SEV(my_logger::get(), LOG)                        \
      << "[" << "Ext" << "]"                                                  \
      << ":["<< maskableEnumToString(this_module_) << "]"                     \
      << __func__ << "():" << __LINE__ << " - "


/**
 * @brief  Stream macro to do WRN level logging
 *
 * This macro should be called to log things to the console with WRN severity
 * level.  It should be used as a stream (aka cout or cerr).
 *
 * @note: Do not use "\n" or "<<endl" at the end of the stream since the boost
 * logger automatically appends it (with no way to turn it off, it seems).
 */
#define WRN_out   BOOST_LOG_SEV(my_logger::get(), WRN)                        \
      << "[" << "Ext" << "]"                                                  \
      << ":["<< maskableEnumToString(this_module_) << "]"                     \
      << __func__ << "():" << __LINE__ << " - "


/**
 * @brief  Stream macro to do ERR level logging
 *
 * This macro should be called to log things to the console with ERR severity
 * level.  It should be used as a stream (aka cout or cerr).
 *
 * @note: Do not use "\n" or "<<endl" at the end of the stream since the boost
 * logger automatically appends it (with no way to turn it off, it seems).
 */
#define ERR_out   BOOST_LOG_SEV(my_logger::get(), ERR)                        \
      << "[" << "Ext" << "]"                                                  \
      << ":["<< maskableEnumToString(this_module_) << "]"                     \
      << __func__ << "():" << __LINE__ << " - "


#define CON_out   BOOST_LOG_SEV(my_menu::get(), CON)

/**
 * @brief   Wrapper around the boost log stream for logging from a callback.
 *
 * This macro should only be used to log data coming from the callback of the
 * client library.  It's designed specifically for that purpose.
 *
 * @note: Do not use "\n" or "<<endl" at the end of the buffer since the boost
 * logger automatically appends it (with no way to turn it off, it seems).
 *
 * @param [in] dbgLvl: DBG_LEVEL_T that specifies the level of logging
 *    @arg DBG: debug messages
 *    @arg LOG: log messages
 *    @arg WRN: warning messages
 *    @arg ERR: error messages
 *    @arg CON: console printout only.  Nothing gets prepended.  This really
 *              shouldn't be used unless you want to have a very difficult
 *              time finding where and why something got printed.
 * @param [in] *funcName: const char pointer to the function name where this
 *              was called.
 * @param [in]  lineNum: line number in the file that this was called.
 * @param [in]  moduleSrc: ModuleSrc_t that specifies where the logging is
 *              coming from
 *    @arg SRC_CLI_EXT: from an external caller of the library
 *    @arg SRC_CLI_LIB: from the library itself
 *    @arg SRC_DC3_APPL: from DC3 Application
 *    @arg SRC_DC3_BOOT: from DC3 Bootloader
 * @param [in]  moduleId: ModuleId_t that specifies which module in the lib
 *              this is coming from.  This only makes sense if
 *              moduleSrc == SRC_CLI_LIB.
 *    @arg MODULE_GEN: General modules.
 *    @arg MODULE_SER: Serial module
 *    @arg MODULE_ETH: Ethernet module
 *    @arg MODULE_MGR: MainMgr AO module
 *    @arg MODULE_LOG: LogStub module
 *    @arg MODULE_EXT: External caller. Should be used by external caller.
 * @param [in] message: buffer that contains the original log message.
 * @return:  None.
 */
#define LIB_out( dbgLvl, funcName, lineNum, moduleSrc, moduleId, message )    \
   BOOST_LOG_SEV(my_logger::get(), dbgLvl)                                    \
   << "[" << setw(3) << maskableEnumToString(moduleSrc) << "]"                \
   << ":[" << setw(3) << maskableEnumToString(moduleId) << "]"                \
   << funcName << "():" << lineNum << " - "<< message;

/**
 * @brief  Wrapper around the boost log that outputs a naked string to cout.
 *
 * This macro should be called to print naked lines for use in the menu and
 * such.  This macro is unfortunately not usable as a stream.  Instead, it's up
 * to the caller to create a string to pass into this macro.
 *
 * @note: Do not use "\n" or "<<endl" at the end of the stream since the boost
 * logger automatically appends it (with no way to turn it off, it seems).
 *
 * @param [in] message: string type that contains the data to print.
 * @return: None.
 */
#define MENU_print(message)                                                   \
   if (true) {                                                                \
      BOOST_LOG_SCOPED_LOGGER_TAG(my_menu::get(), "MenuStream", "conMenu");   \
      BOOST_LOG_SEV(my_menu::get(), CON) << message;                          \
   } else ((void) 0)

/**
 * @brief   Just an alias for MENU_print() macro.
 *
 * This macro should be called to print naked lines for use in the menu and
 * such.  This macro is unfortunately not usable as a stream.  Instead, it's up
 * to the caller to create a string to pass into this macro.
 *
 * @note: Do not use "\n" or "<<endl" at the end of the stream since the boost
 * logger automatically appends it (with no way to turn it off, it seems).
 *
 * @param [in] message: string type that contains the data to print.
 * @return: None.
 */
#define CON_print(message) MENU_print(message)

/**
 * @brief   Wrapper around the Logging::log() function for DBG logging.
 *
 * @param [in] logInstance: Logging instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return:  None.
 */
#define DBG_printf(logInstance, fmt, ...) \
      do { logInstance->log(DBG, __func__, __LINE__, SRC_CLI_EXT, this_module_, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief   Wrapper around the Logging::log() function for LOG logging.
 *
 * @param [in] logInstance: Logging instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define LOG_printf(logInstance, fmt, ...) \
      do { logInstance->log(LOG, __func__, __LINE__, SRC_CLI_EXT, this_module_, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief   Wrapper around the Logging::log() function for WRN logging.
 *
 * @param [in] logInstance: Logging instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define WRN_printf(logInstance, fmt, ...) \
      do { logInstance->log(WRN, __func__, __LINE__, SRC_CLI_EXT, this_module_, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief   Wrapper around the Logging::log() function for ERR logging.
 *
 * @param [in] logInstance: Logging instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define ERR_printf(logInstance, fmt, ...) \
      do { logInstance->log(ERR, __func__, __LINE__, SRC_CLI_EXT, this_module_, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/* Exported types ------------------------------------------------------------*/
/**
 * @brief   Logger type that defines a thread-safe severity logger.
 */
typedef boost::log::sources::severity_logger_mt<DBG_LEVEL_T> logger_t;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief   A boost macro that instantiates a thread safe global logger
 *
 * This boost macro that performs some sort of dark magic to initialize
 * and instantiate a global logger. I really have no idea how this works.  It's
 * some sort of template macro that is necessary and is called the first time
 * a logger is accessed. - HR
 *
 * @param [in] my_logger: variable name that is created for the logger to be
 * accessed globally.
 * @param [in] logger_t: type of logger to instantiate.
 * @return: my_logger global instance of the logger.
 */
BOOST_LOG_GLOBAL_LOGGER(my_logger, logger_t)

/**
 * @brief   A boost macro that instantiates a thread safe global naked logger
 *
 * Unlike the other logger that prepends time, line, file, etc... this logger
 * is used to print naked strings to cout.
 *
 * This boost macro that performs some sort of dark magic to initialize
 * and instantiate a global logger. I really have no idea how this works.  It's
 * some sort of template macro that is necessary and is called the first time
 * a logger is accessed. - HR
 *
 * @param [in] my_menu: variable name that is created for the logger to be
 * accessed globally.
 * @param [in] logger_t: type of logger to instantiate.
 * @return: my_menu global instance of the logger.
 */
BOOST_LOG_GLOBAL_LOGGER(my_menu, logger_t)

/* Exported classes ----------------------------------------------------------*/

/**
 * @class Logging
 *
 * @brief This class is a wrapper for the internal library log stubs.
 *
 * This class allows the caller of the Client library to implement any logging
 * by using local callbacks.  It also provides a direct interface to the various
 * control over internal logging the library uses.
 *
 * Examples of how to set up logging:
 *
 * \code{.cpp}
 *  Logging *logger = new Logging();
 *  cerr << "Demonstration of invalid logging callback setting. Expecting error: 0x"
 *       << setfill('0') << setw(8) << std::hex << CLI_ERR_INVALID_CALLBACK << endl;
 *  ClientError_t status = logger->setLibLogCallBack( NULL );
 *  if ( CLI_ERR_NONE != status ) {
 *    cerr << "Failed to set logging callback. Error: 0x"
 *          << setfill('0') << setw(8) << std::hex << status << endl;
 * }
 *
 * cerr << "Demonstration of valid logging callback setting. Expecting error: 0x"
 *      << setfill('0') << setw(8) << std::hex << CLI_ERR_NONE << endl;
 * status = logger->setLibLogCallBack( CLI_LibLogCallback );
 * if ( CLI_ERR_NONE != status ) {
 *    cerr << "Failed to set logging callback. Error: 0x"
 *          << setfill('0') << setw(8) << std::hex << status << endl;
 * } else {
 *    cerr << "Successfully set up logging callback. You should have seen a date"
 *          " and time with some logging printed before this.  Error: 0x"
 *          << setfill('0') << setw(8) << std::hex << status << endl;
 * }
 *
 *
 *  cerr << "Demonstration of valid logging class method callback setting. Expecting error: 0x"
 *           << setfill('0') << setw(8) << std::hex << CLI_ERR_NONE << endl;
 *
 * \endcode
 */
class Logging {

private:

   LogStub *m_pLog;

public:

   ClientError_t setMsgCallBack(
         CB_MsgHandler_t pCallbackFunction
   );

   ClientError_t setLibLogCallBack(
         CB_LibLogHandler_t pCallbackFunction
   );

   /**
    * @brief   Get pointer to LogStub instance
    *
    * This function return the pointer to the LogStub class instance.
    */
   LogStub* getLogStubPtr( void );


   /**
    * @brief   Output logging msg
    *
    * Outputs a logging message to the callback passed in by the user.  Should
    * be used just like printf() in C in addition to providing the log level,
    * __func__, and __LINE__ macros as inputs.
    *
    * @param [in] dbgLvl: DBG_LEVEL_T that specifies the level of logging
    *    @arg DBG: debug messages
    *    @arg LOG: log messages
    *    @arg WRN: warning messages
    *    @arg ERR: error messages
    *    @arg CON: console printout only.  Nothing gets prepended.  This really
    *              shouldn't be used unless you want to have a very difficult
    *              time finding where and why something got printed.
    * @param [in] *pFuncName: const char pointer to the function name where this
    *              was called.
    * @param [in]  wLineNumber: line number in the file that this was called.
    * @param [in]  moduleSrc: ModuleSrc_t that specifies where the logging is
    *              coming from
    *    @arg SRC_CLI_EXT: from an external caller of the library
    *    @arg SRC_CLI_LIB: from the library itself
    *    @arg SRC_DC3_APPL: from DC3 Application
    *    @arg SRC_DC3_BOOT: from DC3 Bootloader
    * @param [in]  moduleId: ModuleId_t that specifies which module in the lib
    *              this is coming from.  This only makes sense if
    *              moduleSrc == SRC_CLI_LIB.
    *    @arg MODULE_GEN: General modules.
    *    @arg MODULE_SER: Serial module
    *    @arg MODULE_ETH: Ethernet module
    *    @arg MODULE_MGR: MainMgr AO module
    *    @arg MODULE_LOG: LogStub module
    *    @arg MODULE_EXT: External caller. Should be used by external caller.
    * @param [in] *fmt: va_arg style argument.
    * @param [in]  ...: va_arg style additional arguments.
    * @return  None.
    */
   static void log(
         DBG_LEVEL_T dbgLvl,
         const char *pFuncName,
         int wLineNumber,
         ModuleSrc_t moduleSrc,
         ModuleId_t moduleId,
         char *fmt,
         ...
   );

   /**
    * @brief   Default constructor.
    * This constructor doesn't set up any logging.  There will be complete
    * silence from the Client library unless "" is called to set up the callback
    * functions for logging.
    *
    * @param   None.
    * @return  None.
    */
   Logging( void );

   /**
    * @brief   Default destructor
    */
   ~Logging( void );

};


#endif                                                          /* LOGGING_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
