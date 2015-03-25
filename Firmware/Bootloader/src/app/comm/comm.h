/**
 * @file 	comm.h
 * @brief   Parser for various messages that come over any communication busses.
 *
 * @date   	09/29/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupComm
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMM_H_
#define COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Parse msg.
 *
 * @param [in] *pBuffer: uint8_t pointer to the buffer containing the msg.
 * @param [in] bufferLen: uint16_t length of data in buffer.
 * @param [in] msgSource: CBMsgRoute indicating where the msg originated.
 * @return: None
 */
CBErrorCode COMM_parseMsg(
      uint8_t *pBuffer,
      uint16_t bufferLen,
      CBMsgRoute msgSource
);

/**
 * @}
 * end addtogroup groupComm
 */

#ifdef __cplusplus
}
#endif

#endif                                                             /* COMM_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
