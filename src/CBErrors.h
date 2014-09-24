/**
 * @file 	CBErrors.h
 * @brief   Contains all the error codes that can be generated by the board
 *
 * @date   	05/28/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupApp
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBERRORS_H_
#define CBERRORS_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @enum Error codes that can be generated by this board.
 *
 * The error code consists of a 32 bit value:
 *
 * The most significant 16 bits map to the category.
 * The least significant 16 bits map to low level errors as you see fit.
 *
 * For example:
 * Error code - 0x00000004
 * Category   - 0x0000: This is the serial communications category
 * Error      - 0x0004: This is the low level error code that the board can
 * specify.  Each category gets 0xFFFF error codes (65535) dedicated to it.
 *
 */
typedef enum CBErrors
{
   ERR_NONE                                                    = 0x00000000,

   /* HW error category.                         0x00000001 - 0x0000FFFF */
   ERR_SERIAL_HW_TIMEOUT                                       = 0x00000001,
   ERR_SERIAL_MSG_TOO_LONG                                     = 0x00000002,
   ERR_SERIAL_MSG_BASE64_ENC_FAILED                            = 0x00000003,
   ERR_STM32_HW_CRYPTO_FAILED                                  = 0x00000004,

   /* Reset error category                       0x00020000 - 0x0002FFFF */
   ERR_LOW_POWER_RESET                                         = 0x00020000,
   ERR_WINDOW_WATCHDOG_RESET                                   = 0x00020001,
   ERR_INDEPENDENT_WATCHDOG_RESET                              = 0x00020002,
   ERR_SOFTWARE_RESET                                          = 0x00020003,
   ERR_POR_PDR_RESET                                           = 0x00020004,
   ERR_PIN_RESET                                               = 0x00020005,
   ERR_BOR_RESET                                               = 0x00020006,

   /* NOR error category                         0x00030000 - 0x0003FFFF */
   ERR_NOR_ERROR                                               = 0x00030000,
   ERR_NOR_TIMEOUT                                             = 0x00030001,
   ERR_NOR_BUSY                                                = 0x00030002,

   /* Reserved errors                            0xFFFFFFFE - 0xFFFFFFFF */
   ERR_UNIMPLEMENTED                                           = 0xFFFFFFFE,
   ERR_UNKNOWN                                                 = 0xFFFFFFFF
} CBErrorCode;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupApp
 */
#endif                                                         /* CBERRORS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
