/**
 * @file    CallbackTypes.h
 * Definitions of different callback types
 *
 * @date    03/24/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */
#ifndef CALLBACKTYPES_H_
#define CALLBACKTYPES_H_

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

/* Generated C headers should be included inside of extern C enclosure */
#include "CBCommApi.h"
#ifndef CBMSGS_H_
#define CBMSGS_H_
#include "CBMsgs.h"
#endif

#ifdef __cplusplus
}
#endif

#include "LogLevels.h"

/**
 * \typedef for a call back function that will handle messages coming back from
 * the laminator.
 */
typedef void (*cbRedwoodMsgHandler)(
      char *message,
      int len
);

/**
 * \typedef for a call back function that will handle messages coming back from
 * the laminator.
 */
typedef void (*cbRedwoodLogHandler)(
      LogLevel_t logLevel,
      ErrorCode err,
      char *message
);

#endif                                                    /* CALLBACKTYPES_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
