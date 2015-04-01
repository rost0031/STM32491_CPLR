/**
 * @file    Callbacks.h
 * This file contains all the callbacks used by the Client.  This allows
 * different implementation of how to print and log info from the coupler
 * and the client.
 *
 * @date    03/26/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CALLBACKS_H_
#define CALLBACKS_H_

/* Includes ------------------------------------------------------------------*/
#include "CBSharedDbgLevels.h"
#include "CBCommApi.h"
#include "ClientModules.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void CLI_MsgCallback( char* message, int len );

void CLI_AckCallback( char* message, int len );

//void CLI_LogCallback(
//      DBG_LEVEL_T logLevel,
//      CBErrorCode err,
//      char *message
//);

//void CLI_LibLogCallback(
//      DBG_LEVEL_T logLevel,
//      const char *pFuncName,
//      int wLineNumber,
//      ModuleId_t module,
//      char *fmt,
//      ...
//);
/* Exported classes ----------------------------------------------------------*/

#endif                                                        /* CALLBACKS_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
