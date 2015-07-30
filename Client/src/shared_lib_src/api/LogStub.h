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
#ifndef LOGSTUB_H_
#define LOGSTUB_H_

/* Includes ------------------------------------------------------------------*/
#include "DC3CommApi.h"
#include "ClientModules.h"
#include "ApiErrorCodes.h"
#include "CallbackTypes.h"
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/
#ifdef _WIN32
#ifdef BUILDING_CLIENT_DLL
#define CLIENT_DLL __declspec(dllexport)
#else
#define CLIENT_DLL __declspec(dllimport)
#endif
#else
#define CLIENT_DLL
#define __stdcall
#endif

#ifdef __cplusplus
}
#endif

/* Exported defines ----------------------------------------------------------*/
#define MAX_LOG_BUFFER_LEN 1024  /**<Max buffer length for formatting messages*/

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/
/**
 * @class LogStub
 *
 * @brief This class handles all internal logging from the client.
 *
 * This class gets instantiated by the ClientApi class.  It gets initialized by
 * passing in callback functions from the caller of the Client dll/so and then
 * gets passed in to all the other modules of the client dll/so.  All other
 * modules can then use this class to do transparent logging.
 */
class CLIENT_DLL LogStub {

private:
   DC3_LibLogHandler_t m_pLibLogHandlerDC3Function;  /**< Callback for handling lib log msgs */
   DC3_DC3LogHandler_t m_pDC3LogHandlerDC3Function;  /**< Callback for handling DC3 log msgs */

public:

   /**
    * @brief   Default constructor.
    * This constructor doesn't set up any logging.  There will be complete
    * silence from the Client library unless "" is called to set up the callback
    * functions for logging.
    *
    * @param   None.
    * @return  None.
    */
   LogStub( void );

   /**
    * @brief   Default destructor
    */
   ~LogStub( void );

   /**
    * @brief   Set a callback to handle the internal logging from the client.
    *
    * @param  [in]  pCallbackFunction: a DC3_LibLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setLibLogCallBack( DC3_LibLogHandler_t pCallbackFunction );

   /**
    * @brief   Set a callback to handle the log msgs coming from DC3 board.
    *
    * @param  [in]  pCallbackFunction: a DC3_DC3LogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setDC3LogCallBack( DC3_DC3LogHandler_t pCallbackFunction );

   /**
    * @brief   Enable logging for a given module of the library.
    * Enable DBG and LOG level messages from a given module of the library.
    * See ModuleId_t for module options.
    *
    * @param [in] moduleId: ModuleId_t that specifies which module to enable
    * logging for.
    * @return: None.
    */
   void enableLogForLibModule( ModuleId_t moduleId );

   /**
    * @brief   Disable logging for a given module of the library.
    * Disable DBG and LOG level messages from a given module of the library.
    * See ModuleId_t for module options.
    *
    * @note: WRN and ERR level messages cannot be silenced but they should not
    * appear unless there's an actual problem.
    *
    * @param [in] moduleId: ModuleId_t that specifies which module to disable
    * logging for.
    * @return: None.
    */
   void disableLogForLibModule( ModuleId_t moduleId );

   /**
    * @brief   Enable logging for all library modules.
    * @param   None.
    * @return  None.
    */
   void enableLogForAllLibModules( void );

   /**
    * @brief   Disable logging for all library modules.
    * @param   None.
    * @return  None.
    */
   void disableLogForAllLibModules( void );

   /**
    * @brief   Output logging msg
    *
    * Outputs a logging message to the callback passed in by the user.  Should
    * be used just like printf() in C in addition to providing the log level,
    * __func__, and __LINE__ macros as inputs.
    *
    * @param [in] dbgLvl: DC3DbgLevel_t that specifies the level of logging
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
   void log(
         DC3DbgLevel_t dbgLvl,
         const char *pFuncName,
         int wLineNumber,
         ModuleSrc_t moduleSrc,
         ModuleId_t moduleId,
         char *fmt,
         ...
   );

   /**
    * @brief   Print log messages from DC3
    * This function prints out DC3 log messages that may come over the channel
    * bus when connected over serial.
    *
    */
   void printDC3LogMsg(
         const char *msg
   );
};

#endif                                                          /* LOGSTUB_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
