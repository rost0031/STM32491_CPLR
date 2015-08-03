/**
 * @file    Callbacks.hpp
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
#ifndef CALLBACKS_HPP_
#define CALLBACKS_HPP_

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <map>

/* Api includes */
#include "DC3CommApi.h"

/* Lib includes */
#include "ApiDbgModules.h"


/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Callback for an Req msg going to the coupler board.
 *
 * This is a callback which should be instantiated to handle Req msgs going to
 * the coupler board. This performs a pretty-print of data going out in the Req
 * msg.
 *
 * @param [in] basicMsgStruct: DC3BasicMsg struct of raw data contained in the
 * Req msg.
 * @return: None.
 */
void CLI_ReqCallback(
      struct DC3BasicMsg basicMsgStruct
);

/**
 * @brief   Callback for an Ack msg coming from the coupler board.
 *
 * This is a callback which should be instantiated to handle Ack msgs coming
 * from the coupler board. This performs a pretty-print of data going out in the
 * Ack msg.
 *
 * @param [in] basicMsgStruct: DC3BasicMsg struct of raw data contained in the
 * Ack msg.
 * @return: None.
 */
void CLI_AckCallback(
      struct DC3BasicMsg basicMsgStruct
);

/**
 * @brief   Callback for an Done msg coming from the coupler board.
 *
 * This is a callback which should be instantiated to handle Done msgs coming
 * from the coupler board. This performs a pretty-print of data in the Done msg.
 *
 * @param [in] basicMsgStruct: DC3BasicMsg struct of raw data contained in the Ack msg.
 * @param [in] payloadMsgUnion: DC3PayloadMsgUnion_t uniont that contains the union
 * of all possible payload msgs.
 *
 * @return: None.
 */
void CLI_DoneCallback(
      struct DC3BasicMsg basicMsgStruct,
      DC3PayloadMsgUnion_t payloadMsgUnion
);

/**
 * @brief   Callback for printing debug statements from the library.
 *
 * This is a callback which should be instantiated to handle primary output from
 * the DC3 Api library.
 *
 * @param [in] dbgLevel: DC3DbgLevel_t that specifies the log priority of the msg
 *    @arg _DC3_DBG: Lowest level of debugging.  Everything printed.
 *    @arg _DC3_LOG: Basic logging.
 *    @arg _DC3_WRN: Warnings.  Non-critical errors that may have occurred that
 *                   allow  operations to continue.
 *    @arg _DC3_ERR: Critical errors. Operations will stop if these occurred.
 *    @arg _DC3_CON: This is reserved for printing to the console as part of
 *                   regular operation and nothing will be prepended. The
 *                   client has no use for this. - DO NOT USE.
 *    @arg _DC3_ISR: This debug msg came from an ISR. The client has no use for
 *                   this. - DO NOT USE.
 *
 * @param [in] *pFuncName: const char * string specifying the calling function's
 *              name.
 * @param [in]  wLineNumber: int line number where the call occurred.
 * @param [in]  module: ModuleId_t that specifies which module called it.
 * @param [in] *fmt: char* fmt va args type argument.
 * @param [in]  ...: additional va args type arguments.
 *
 * @return: None.
 */
void CLI_LibLogCallback(
      DC3DbgLevel_t dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      ApiDbgModuleSrc_t moduleSrc,
      ApiDbgModuleId_t moduleId,
      char *fmt,
      ...
);

void CLI_DC3LogCallback(
     const char *msg
);
/* Exported classes ----------------------------------------------------------*/

#endif                                                      /* CALLBACKS_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
