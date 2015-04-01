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

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/**
 * @brief   Wrapper around the Logging::log() function for DBG logging.
 *
 * @param [in] logInstance: Logging instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
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
/* Exported functions --------------------------------------------------------*/
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
