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
#include "CBSharedDbgLevels.h"
#include "CBCommApi.h"

/* Lib includes */
#include "ClientModules.h"


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
 * @param [in] basicMsgStruct: CBBasicMsg struct of raw data contained in the
 * Req msg.
 * @return: None.
 */
void CLI_ReqCallback(
      struct CBBasicMsg basicMsgStruct
);

/**
 * @brief   Callback for an Ack msg coming from the coupler board.
 *
 * This is a callback which should be instantiated to handle Ack msgs coming
 * from the coupler board. This performs a pretty-print of data going out in the
 * Ack msg.
 *
 * @param [in] basicMsgStruct: CBBasicMsg struct of raw data contained in the
 * Ack msg.
 * @return: None.
 */
void CLI_AckCallback(
      struct CBBasicMsg basicMsgStruct
);


/**
 * @brief   Callback for an Done msg coming from the coupler board.
 *
 * This is a callback which should be instantiated to handle Done msgs coming
 * from the coupler board. This performs a pretty-print of data in the Done msg.
 *
 * @param [in] basicMsgStruct: CBBasicMsg struct of raw data contained in the Ack msg.
 * @param [in] payloadMsgUnion: CBPayloadMsgUnion_t uniont that contains the union
 * of all possible payload msgs.
 *
 * @return: None.
 */
void CLI_DoneCallback(
      struct CBBasicMsg basicMsgStruct,
      CBPayloadMsgUnion_t payloadMsgUnion
);

//void CLI_LogCallback(
//      DBG_LEVEL_T logLevel,
//      CBErrorCode err,
//      char *message
//);

void CLI_LibLogCallback(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      ModuleSrc_t moduleSrc,
      ModuleId_t moduleId,
      char *fmt,
      ...
);

void CLI_DC3LogCallback(
     const char *msg
);
/* Exported classes ----------------------------------------------------------*/

#endif                                                      /* CALLBACKS_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/