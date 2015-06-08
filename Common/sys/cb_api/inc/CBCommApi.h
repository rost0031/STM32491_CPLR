/**
 * @file     CBCommApi.h
 *
 * CB Communications API typedefs and enum defs that are related to the
 * generated messages.  This file should be included by any project that wants
 * to communicate with the DC3 board via the debug channel. It includes all the
 * error definitions, timeouts, and messages.
 *
 * @date       03/19/2015
 * @author     Harry Rostovtsev
 * @email      harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBCOMMAPI_H_
#define CBCOMMAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifndef CBMSGS_H_
#define CBMSGS_H_
#include "CBMsgs.h"
#endif

#include "CBErrors.h"
#include "CBTimeouts.h"
#include "dfuse.h"
#include "util.h"
#include "CBSharedDBElements.h"

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief   Max buffer length for communications between client and DC3
 * This is the MAX length of msgs in bytes that can be sent over serial (base64
 * encoded) or UDP from the client. */
#define CB_MAX_MSG_LEN 300

/**
 * @brief   Length of a datetime string
 * The format of this string is always:
 * YYYYMMDDhhmmss
 */
#define CB_DATETIME_LEN 14

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* These typedefs allow the project to use the enums defined in the
 * autogenerated protobuf files  */
typedef enum CBMsgName         CBMsgName;
typedef enum CBMsgType         CBMsgType;
typedef enum CBMsgRoute        CBMsgRoute;
typedef enum CBBootMode        CBBootMode;
typedef enum CBI2CDevices      CBI2CDevices;
typedef enum CBAccessType      CBAccessType;
typedef enum CBRamTest_t       CBRamTest_t;

/**
 * @brief   A Union of all the payload structs.
 * This union allows for some fairly significant space savings in FW since only
 * one payload msg will ever be handled at a time, there's no reason to not
 * allow them to share the space.
 */
typedef union CBPayloadMsgs {
   struct CBStatusPayloadMsg  statusPayload;
   struct CBVersionPayloadMsg versionPayload;
   struct CBBootModePayloadMsg bootmodePayload;
   struct CBFlashMetaPayloadMsg flashMetaPayload;
   struct CBFlashDataPayloadMsg flashDataPayload;
   struct CBI2CDataPayloadMsg   i2cDataPayload;
   struct CBRamTestPayloadMsg   ramTestPayload;
} CBPayloadMsgUnion_t;


/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif                                                        /* CBCOMMAPI_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
