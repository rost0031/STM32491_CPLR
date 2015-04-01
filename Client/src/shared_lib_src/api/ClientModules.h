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
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CLIENTMODULES_H_
#define CLIENTMODULES_H_

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
/* Exported types ------------------------------------------------------------*/

/**
 * @brief Various system modules for which debugging can be enabled/disabled.
 *
 * These are limited to 32 bit and must be maskable.  These enum masks will be
 * used for both naming the various modules and for checking whether their
 * debug capabilities have been enabled.
 */
typedef enum DBG_MODULES {
   MODULE_GENERAL  = 0x00000001, /**< General module debugging (main, bsp, etc) */
   MODULE_SERIAL   = 0x00000002, /**< Serial module debugging. */
   MODULE_ETH      = 0x00000008, /**< Ethernet module debugging. */
   MODULE_MAINMGR  = 0x00000010, /**< MainMgr AO module debugging. */
} ModuleId_t;

/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/

#endif                                                    /* CLIENTMODULES_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
