/**
 * @file    ClientErrorCodes.h
 * @brief   Error codes internal to the client.
 *
 * This header contains all error codes used by the client.
 *
 * @date    03/31/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CLIENTERRORCODES_H_
#define CLIENTERRORCODES_H_

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
 * @brief Error codes that can be generated by the client
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
typedef enum ClientErrors
{
   CLI_ERR_NONE                                                = 0x00000000,

   /* Logger error category.                     0x00000001 - 0x0000FFFF */
   CLI_ERR_INVALID_CALLBACK                                    = 0x00000001,


   /* Reserved errors                            0xFFFFFFFE - 0xFFFFFFFF */
   CLI_ERR_UNIMPLEMENTED                                       = 0xFFFFFFFE,
   CLI_ERR_UNKNOWN                                             = 0xFFFFFFFF
} ClientError_t;


/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/

#endif                                                 /* CLIENTERRORCODES_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/