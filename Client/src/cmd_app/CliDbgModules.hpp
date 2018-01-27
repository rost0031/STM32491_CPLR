/**
 * @file    CliDbgModules.hpp
 * @brief   Identifications for different modules.
 *
 * This header contains identifying enumerations for all the modules that exist
 * in the client exe application (not to be confused with the Dbg modules that
 * exist in the DC3 Api).  All logging from within the client exe will send
 * along one of these identifiers to allow the caller to enable/disable logging
 * based on the modules.
 *
 * @date    08/03/2015
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CLIDBGMODULES_H_
#define CLIDBGMODULES_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Defines a module for grouping debugging functionality.
 *
 * @description
 * Macro to be placed at the top of each C/C++ module to define the single
 * instance of the module name string to be used in printing of debugging
 * information.
 *
 * @param[in] @c name_: CliDbgModuleId_t enum representing the module.
 *
 * @note 1: This macro should __not__ be terminated by a semicolon.
 * @note 2: This macro MUST be present in the file if DBG_out, LOG_out, WRN_out,
 * or ERR_out macros are called.  The code will not compile without this.
 */
#define CLI_MODULE_NAME( name_ ) \
      static CliDbgModuleId_t const this_module_ = name_;

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Various system modules for which debugging can be enabled/disabled.
 *
 * These are limited to 32 bit and must be maskable.  These enum masks will be
 * used for both naming the various modules and for checking whether their
 * debug capabilities have been enabled.
 */
typedef enum CLI_DBG_MODULES {
   CLI_DBG_MODULE_GEN   = 0x00000001, /**< General module debugging (main ) */
   CLI_DBG_MODULE_ARG   = 0x00000002, /**< ArgParse module debugging. */
   CLI_DBG_MODULE_CBS   = 0x00000004, /**< CallBacks and related module debugging. */
   CLI_DBG_MODULE_LOG   = 0x00000008, /**< Logging module debugging */
   CLI_DBG_MODULE_CMD   = 0x00000010, /**< CMDs module debugging */
   CLI_DBG_MODULE_MNU   = 0x00000020, /**< Menu module debugging */
   CLI_DBG_MODULE_KTR   = 0x00000040, /**< KTree module debugging*/
   CLI_DBG_MODULE_HLP   = 0x00000080, /**< Help module debugging */
} CliDbgModuleId_t;

/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/

#endif                                                    /* CLIDBGMODULES_H_ */
/******** Copyright (C) 2015 Harry Rostovtsev. All rights reserved *****END OF FILE****/
