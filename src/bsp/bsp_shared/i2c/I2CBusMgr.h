/*****************************************************************************
* Model: I2CBusMgr.qm
* File:  ./I2CBusMgr_gen.h
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
/*${.::I2CBusMgr_gen.h} ....................................................*/
/**
 * @file    I2CBusMgr.c
 * @brief   Declarations for functions for the I2CBusMgr AO.
 * This state machine handles all I/O on the I2C bus.  It can be instantiated
 * several times with a different bus for a parameter.  This AO doesn't handle
 * any actual I2C devices, just generic I2C bus operations such as sending the
 * start/end bit, waiting for events, initiating DMA reads/writes, etc.  I2C
 * devices should be handled in the I2CXDevMgr AOs.  This allows this AO to be
 * generic and reusable.
 *
 * @note 1: If editing this file, please make sure to update the I2CBusMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    10/24/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2CBUSMGR_H_
#define I2CBUSMGR_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "Shared.h"                                   /*  Common Declarations */
#include "i2c.h"                               /* For I2C device declarations */

/* Exported defines ----------------------------------------------------------*/
#define MAX_BUS_RETRIES   100 /**< Max number of retries for I2C bus for busy flag */
#define MAX_I2C_TIMEOUT 10000 /**< Max number of retries for I2C bus for busy flag */
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * \struct Event struct type for transporting I2C data.
 */
/*${Events::I2CDataEvt} ....................................................*/
typedef struct {
/* protected: */
    QEvt super;

    /**< Which I2C device data is from. */
    I2C_Device_t i2cDevice;

    /**< Buffer that holds the data. */
    char bufData[MAX_MSG_LEN];

    /**< Length of data in the buffer. */
    uint16_t wDataLen;

    /**< Address on the I2C device read/written to. */
    uint16_t wAddr;
} I2CDataEvt;

/**
 * \struct Event struct type for requesting I2C data reads and notifying of finished
 * writes.
 */
/*${Events::I2CEvt} ........................................................*/
typedef struct {
/* protected: */
    QEvt super;

    /**< Which I2C device is being accessed. */
    I2C_Device_t i2cDevice;

    /**< Address on the I2C device. */
    uint16_t wAddr;

    /**< Number of bytes to read/written from I2C device. */
    uint16_t wDataLen;
} I2CEvt;


/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief C "constructor" for I2CBusMgr "class".
 * Initializes all the timers and queues used by the AO, sets up a deferral
 * queue, and sets of the first state.
 * @param  [in] i2cBus: I2C_TypeDef * type that specifies which STM32 I2C bus this
 * AO is responsible for.
 * @retval None
 */
/*${AOs::I2CBusMgr_ctor} ...................................................*/
void I2CBusMgr_ctor(I2C_Bus_t iBus);


/**< "opaque" pointer to the Active Object */
extern QActive * const AO_I2CBusMgr;


/**
 * @} end addtogroup groupI2C
 */
#endif                                                        /* I2CBUSMGR_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
