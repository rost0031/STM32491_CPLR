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
/* Exported defines ----------------------------------------------------------*/

/* This is the MAX length of msgs in bytes that can be sent over serial to the
 * RFID board */
#define CB_MAX_MSG_LEN 300

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif                                                 /* CBSHAREDMSGTYPES_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
