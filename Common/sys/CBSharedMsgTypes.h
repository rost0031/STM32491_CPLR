/**
 * @file     CBSharedMsgTypes.h
 *
 * Message types that are shared between all FW and client but are dependent on
 * QPC.  This is separate from the CBApi since we don't want to have a
 * dependency on QPC there.
 *
 * @date       03/24/2015
 * @author     Harry Rostovtsev
 * @email      harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBSHAREDMSGTYPES_H_
#define CBSHAREDMSGTYPES_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                       /* For QPC support */

/* Exported defines ----------------------------------------------------------*/

/* This is the MAX length of msgs in bytes that can be sent over serial to the
 * RFID board */
#define CB_MAX_MSG_LEN 300

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @brief Struct for messages.
 * TODO: this should be depricated and LrgDataEvt used instead
 * Uses CommStackSignals for signal names.  These events are responsible
 * for getting data from Eth/Serial to CommStackMgr
 */
typedef struct MsgEvtTag {
/* protected: */
    QEvt super;
    CBMsgRoute msg_src;                            /**< Source of the message */
    uint16_t msg_len;                           /**< Length of the msg buffer */
    char msg[CB_MAX_MSG_LEN];      /**< Buffer that holds the data of the msg */
} MsgEvt;

/**
 * @brief Event type for transferring large data.
 */
typedef struct LrgDataEvtTag {
/* protected: */
    QEvt       super;
    CBMsgRoute src;                                   /**< Source of the data */
    CBMsgRoute dst;                              /**< Destination of the data */
    uint16_t   dataLen;                    /**< Length of the data in dataBuf */
    uint8_t    dataBuf[CB_MAX_MSG_LEN];       /**< Buffer that holds the data */
} LrgDataEvt;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif                                                 /* CBSHAREDMSGTYPES_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
