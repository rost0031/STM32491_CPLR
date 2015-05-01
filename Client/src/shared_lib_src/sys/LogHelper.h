/**
 * @file    LogHelper.h
 * @brief   Macros to help with logging.
 *
 * This file contains wrappers around the LogStub::log() function which allow
 * not having to explicitly specify every argument such as debug level, function
 * name, line number, and module name.
 *
 * @date    04/01/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGHELPER_H_
#define LOGHELPER_H_

/* Includes ------------------------------------------------------------------*/
#include "CBSharedDbgLevels.h"
#include "ClientModules.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/**
 * @brief   Enable debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_ENABLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgConfig |= name_;

/**
 * @brief   Disable debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_DISABLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgConfig &= ~name_;

/**
 * @brief   Toggle debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_TOGGLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgConfig ^= name_;

/**
 * @brief   Toggle debugging output for a given module.
 *
 * @param [in] @c name_: DBG_MODL_T enum representing the module.
 */
#define DBG_CHECK_DEBUG_FOR_MODULE( name_ ) \
      ( glbDbgConfig & name_ )
/**
 * @brief   Disable debugging output for all modules.
 */
#define DBG_DISABLE_DEBUG_FOR_ALL_MODULES( ) \
      glbDbgConfig = 0x00000000;

/**
 * @brief   Enable debugging output for all modules.
 */
#define DBG_ENABLE_DEBUG_FOR_ALL_MODULES( ) \
      glbDbgConfig = 0xFFFFFFFF;

/**
 * @brief   Wrapper around the LogStub::log() function for DBG logging.
 *
 * @param [in] logInstance: LogStub instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define DBG_printf(logInstance, fmt, ...)                                     \
   do {                                                                       \
      if ( glbDbgConfig & this_module_ )                                      \
         {logInstance->log( DBG, __func__, __LINE__, SRC_CLI_LIB,             \
               this_module_, fmt, ##__VA_ARGS__);}                            \
      } while (0)

/**
 * @brief   Wrapper around the LogStub::log() function for LOG logging.
 *
 * @param [in] logInstance: LogStub instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define LOG_printf(logInstance, fmt, ...)                                     \
   do {                                                                       \
      if ( glbDbgConfig & this_module_ )                                      \
         {logInstance->log( LOG, __func__, __LINE__, SRC_CLI_LIB,             \
               this_module_, fmt, ##__VA_ARGS__);}                            \
      } while (0)

/**
 * @brief   Wrapper around the LogStub::log() function for WRN logging.
 *
 * @param [in] logInstance: LogStub instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define WRN_printf(logInstance, fmt, ...)                                     \
      do { logInstance->log(WRN, __func__, __LINE__, SRC_CLI_LIB,             \
            this_module_, fmt, ##__VA_ARGS__);                                \
      } while (0)

/**
 * @brief   Wrapper around the LogStub::log() function for ERR logging.
 *
 * @param [in] logInstance: LogStub instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define ERR_printf(logInstance, fmt, ...)                                     \
      do { logInstance->log(ERR, __func__, __LINE__, SRC_CLI_LIB,             \
            this_module_, fmt, ##__VA_ARGS__);                                \
      } while (0)

/**
 * @brief   Wrapper around the LogStub::log() function for ERR logging.
 *
 * @param [in] logInstance: LogStub instance which defines the log function.
 * @param [in] *fmt: va_args style string
 * @param [in] ...: va_args style additional arguments.
 * @return  None.
 */
#define DC3_printf(logInstance, buffer)                                       \
      logInstance->printDC3LogMsg( buffer );                                  \

/**
 * @brief   Print warning "Leaving function %s with error 0x%08x"
 *
 * @description
 * Macro that can be used to conditionally print an error.  If the passed in
 * status is not ERR_NONE (no error), the macro will use the passed in
 * accessType to figure out how to (slow and blocking or fast and
 * non-blocking/non-invasively) to output the passed in string.
 *
 * @param [in] status: status to parse and print if not ERR_NONE.
 *    @arg ERR_NONE: nothing will be output if this is passed in
 *    else: passed in string will be printed.
 * @param [in] accessType: AccessType_t enum representing how the caller wants
 *                         to output the error msg if needed.
 *    @arg ACCESS_BARE_METAL: slow blocking output.
 *    @arg ACCESS_QPC: fast, non-blocking
 *    @arg ACCESS_FREERTOS: fast, non-blocking
 * @param [in] fmt: printf like argument string with any % modifiers that print can handle.
 * @param [in] ...: printf like variable argument list.
 *
 * @note 1: This macro should be treated just like a conditional printf
 * @note 2: This macro DOES NOT perform a return.  This is left up to the user
 * since doing a return inside of a macro is bad practice for debugging.
 */
#define WRN_IF_ERR_OUTPUT(logInstance, status ) {                             \
      if (status != CLI_ERR_NONE) {                                           \
         ERR_printf(logInstance,                                              \
            "Leaving function %s with error 0x%08x", __func__, status);       \
      }                                                                       \
   }


/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint32_t  glbDbgConfig; /**< Allow global access to debug info */

/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/

#endif                                                        /* LOGHELPER_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
