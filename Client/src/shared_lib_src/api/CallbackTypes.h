/**
 * @file    CallbackTypes.h
 * Definitions of different callback types
 *
 * @date    03/25/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
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

/* Includes ------------------------------------------------------------------*/
/* Generated C headers should be included inside of extern C enclosure */
#include "CBCommApi.h"
#include "CBSharedDbgLevels.h"
#include "ClientModules.h"
#ifdef __cplusplus
}
#endif


/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @brief   Callback type for an Req msg going to the coupler board.
 *
 * This is a callback function type definition which should be instantiated to
 * handle Req msgs going to the coupler board. This allows different handling of
 * logging outgoing Req msgs depending if the client is commandline, menu
 * driven, or a a gui.
 *
 * @param [in] basicMsg: CBBasicMsg struct of raw data contained in the Req msg.
 * @param [in] basicMsgMap: std map that contains text values of the basicMsg
 * struct, ready to print
 * @return: None.
 */
typedef void (*CB_ReqMsgHandler_t)(
      struct CBBasicMsg basicMsgStruct
);

/**
 * @brief   Callback type for an Ack msg coming from the coupler board.
 *
 * This is a callback function type definition which should be instantiated to
 * handle Ack msgs coming from the coupler board. This allows different handling
 * depending if the client is commandline, menu driven, or a a gui.
 *
 * @param [in] basicMsg: CBBasicMsg struct of raw data contained in the Ack msg.
 * @param [in] basicMsgMap: std map that contains text values of the basicMsg
 * struct, ready to print
 * @return: None.
 */
typedef void (*CB_AckMsgHandler_t)(
      struct CBBasicMsg basicMsgStruct
);

/**
 * @brief   Callback type for an Done msg coming from the coupler board.
 *
 * This is a callback function type definition which should be instantiated to
 * handle Done msgs coming from the coupler board. This allows different
 * handling depending if the client is commandline, menu driven, or a a gui.
 *
 * @param [in] basicMsgStruct: CBBasicMsg struct of raw data contained in the Ack msg.
 * @param [in] payloadMsgUnion: CBPayloadMsgUnion_t uniont that contains the union
 * of all possible payload msgs.
 *
 * @return: None.
 */
typedef void (*CB_DoneMsgHandler_t)(
      struct CBBasicMsg basicMsgStruct,
      CBPayloadMsgUnion_t payloadMsgUnion
);

/**
 * @brief   Callback type for a debug/log msg coming from the coupler board.
 *
 * This is a callback function type definition which should be instantiated to
 * handle logging and debugging msgs coming from the coupler board.  This allows
 * different handling depending if the client is commandline, menu driven, or a
 * a gui.
 *
 * @param *buffer: char pointer to buffer containing the message to be printed.
 */
typedef void (*CB_DC3LogHandler_t)(
      const char *buffer
);

/**
 * @brief   Callback type for a debug/log msg coming from the coupler board.
 *
 * This is a callback function type definition which should be instantiated to
 * handle logging and debugging msgs coming from the coupler board.  This allows
 * different handling depending if the client is commandline, menu driven, or a
 * a gui.
 *
 * @param [in] dbgLevel: DBG_LEVEL_T that specifies the log priority of the msg
 *    @arg DBG: Lowest level of debugging.  Everything printed.
 *    @arg LOG: Basic logging.
 *    @arg WRN: Warnings.  Non-critical errors that may have occurred that allow
 *              operations to continue.
 *    @arg ERR: Critical errors. Operations will stop if these occurred.
 *    @arg CON: This is reserved for printing to the console as part of regular
 *              operation and nothing will be prepended.  The client has no use
 *              for this.
 *    @arg ISR: This debug msg came from an ISR
 *
 * @param [in] *pFuncName: const char * string specifying the calling function's
 *              name.
 * @param [in]  wLineNumber: int line number where the call occurred.
 * @param [in]  module: ModuleId_t that specifies which module called it.
 * @param [in] *fmt: char* fmt va args type argument.
 * @param [in]  ...: additional va args type arguments.
 */
typedef void (*CB_LibLogHandler_t)(
      DBG_LEVEL_T logLevel,
      const char *pFuncName,
      int wLineNumber,
      ModuleSrc_t moduleSrc,
      ModuleId_t moduleId,
      char *fmt,
      ...
);

#endif                                                    /* CALLBACKTYPES_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
