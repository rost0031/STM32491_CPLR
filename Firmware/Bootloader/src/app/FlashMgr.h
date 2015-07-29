/*****************************************************************************
* Model: FlashMgr.qm
* File:  ./FlashMgr_gen.h
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/*${.::FlashMgr_gen.h} .....................................................*/
/**
 * @file    FlashMgr.h
 * Declarations for functions for the FlashMgr AO.  This state
 * machine handles all FW flashing operations
 *
 * Note: If editing this file, please make sure to update the FlashMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    04/29/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupBsp
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASHMGR_H_
#define FLASHMGR_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "DC3CommApi.h"              /* For API for communicating with client */
#include "DC3Signals.h"             /* For QP signals available to the system */

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**< Event type that allows transport of FW data */
/*${Events::FWDataEvt} .....................................................*/
typedef struct {
/* protected: */
    QEvt super;

    /**< FW Data packet CRC */
    uint32_t dataCRC;

    /**< Length of data buffer */
    uint16_t dataLen;

    /**< Data buffer */
    uint8_t dataBuf[DC3_MAX_MSG_LEN];

    /**< FW data packet sequence number */
    uint16_t seqCurr;
} FWDataEvt;

/**< Event type that transports metadata about the FW upgrade */
/*${Events::FWMetaEvt} .....................................................*/
typedef struct {
/* protected: */
    QEvt super;

    /**< FW image CRC */
    uint32_t imageCRC;

    /**< FW image size */
    uint32_t imageSize;

    /**< FW image type */
    DC3BootMode_t imageType;

    /**< FW image major version number */
    uint8_t imageMaj;

    /**< FW image minor version number */
    uint8_t imageMin;

    /**< FW image datetime */
    uint8_t imageDatetime[DC3_DATETIME_LEN];

    /**< FW image datetime length */
    uint8_t imageDatetimeLen;

    /**< Status of the FW flash completion.  Only used when sending back from FlashMgr */
    DC3Error_t errorCode;

    /**< Total number of FW data packets expected */
    uint16_t imageNumPackets;
} FWMetaEvt;

/**< Event type that transports metadata about the FW upgrade */
/*${Events::FlashStatusEvt} ................................................*/
typedef struct {
/* protected: */
    QEvt super;

    /**< Status of the FlashMgr operation completion. */
    DC3Error_t errorCode;
} FlashStatusEvt;

/**< Event type that transports data about the RAM test */
/*${Events::RamStatusEvt} ..................................................*/
typedef struct {
/* protected: */
    QEvt super;

    /**< Status of the RAM operation. */
    DC3Error_t errorCode;

    /**< Which test is running if no error or failed if error. */
    DC3RamTest_t test;

    /**< Address where the test is running if no error or failed at if error. */
    uint32_t addr;
} RamStatusEvt;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief C "constructor" for FlashMgr "class".
 * Initializes all the timers and queues used by the AO and sets of the
 * first state.
 * @param  None
 * @param  None
 * @retval None
 */
/*${AOs::FlashMgr_ctor} ....................................................*/
void FlashMgr_ctor(void);


/**< "opaque" pointer to the Active Object */
extern QActive * const AO_FlashMgr;


/**
 * @} end addtogroup groupBsp
 */

#endif                                                         /* FLASHMGR_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
