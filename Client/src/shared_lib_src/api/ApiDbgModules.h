/**
 * @file    ClientModules.h
 * @brief   Identifications for different modules.
 *
 * This header contains identifying enumerations for all the modules that exist
 * in the client.  All logging from within the library will send along one of
 * these identifiers to allow the caller to enable/disable logging based on the
 * modules.
 *
 * @date    03/31/2015
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APIDBGMODULES_H_
#define APIDBGMODULES_H_

#ifdef __cplusplus
extern "C" {
#endif

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
 * @param[in] @c name_: ModuleId_t enum representing the module.
 *
 * @note 1: This macro should __not__ be terminated by a semicolon.
 * @note 2: This macro MUST be present in the file if DBG_printf() or
 * LOG_printf() functions are called.  The code will not compile without this.
 */
#define MODULE_NAME( name_ ) \
      static ApiDbgModuleId_t const this_module_ = name_;

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Various system modules for which debugging can be enabled/disabled.
 *
 * These are limited to 32 bit and must be maskable.  These enum masks will be
 * used for both naming the various modules and for checking whether their
 * debug capabilities have been enabled.
 */
typedef enum API_DBG_MODULES {
   MODULE_GEN   = 0x00000001, /**< General module debugging (main, bsp, etc) */
   MODULE_SER   = 0x00000002, /**< Serial module debugging. */
   MODULE_UDP   = 0x00000004, /**< UDP ethernet module debugging. */
   MODULE_LOG   = 0x00000008, /**< LogStub module */
   MODULE_FWL   = 0x00000010, /**< FW loader module */
   MODULE_COM   = 0x00000020, /**< Comm interface module */
   MODULE_API   = 0x00000040, /**< ClientApi module */
   MODULE_MSG   = 0x00000080, /**< Msg interface module */
} ApiDbgModuleId_t;

/**
 * @brief Sources of modules available in the Client API.
 *
 * These are limited to 32 bit and must be maskable.  These enum masks will be
 * used for identifying the source of various messages and debug statements
 */
typedef enum DBG_SOURCES {
   CLI_SRC_EXT  = 0x00000001, /**< Caller of the client library is the source */
   CLI_SRC_API  = 0x00000002, /**< Client library is the source */
   CLI_SRC_DC3  = 0x00000003, /**< DC3 board is the source */
} ApiDbgModuleSrc_t;

/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/

#endif                                                    /* APIDBGMODULES_H_ */
