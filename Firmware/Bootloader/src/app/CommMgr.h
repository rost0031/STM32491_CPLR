/*****************************************************************************
* Model: CommMgr.qm
* File:  ./CommMgr_gen.h
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
/*${.::CommMgr_gen.h} ......................................................*/
/**
 * @file    CommMgr.h
 * Declarations for functions for the CommMgr AO.  This state
 * machine handles all incoming messages and their distribution for the
 * bootloader.
 *
 * Note: If editing this file, please make sure to update the CommMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    05/27/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupComm
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMMMGR_H_
#define COMMMGR_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "DC3CommApi.h"              /* For API for communicating with client */
#include "DC3Signals.h"             /* For QP signals available to the system */

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief C "constructor" for CommMgr "class".
 * Initializes all the timers and queues used by the AO and sets of the
 * first state.
 * @param  None
 * @param  None
 * @retval None
 */
/*${AOs::CommMgr_ctor} .....................................................*/
void CommMgr_ctor(void);


/**
 * @brief	Wrapper around serial and UDP send data function.
 * This function acts as a simple wrapper around serial and UDP ethernet send data
 * functions to allow easy posting of events to send out data over those interfaces.
 * @param [in] *evt: LrgDataEvt pointer to event to send.
 * @return: DC3Error_t indicating status of operation.
 */
/*${AOs::Comm_sendToClien~} ................................................*/
DC3Error_t Comm_sendToClient(LrgDataEvt* evt);


/**< "opaque" pointer to the Active Object */
extern QActive * const AO_CommMgr;


/* Don't declare the MsgEvt type here since it needs to be visible to LWIP,
 * which is a shared port common to any ethernet project.  Instead, it's
 * declared in Shared.h, which is included by LWIPMgr.h */
/* $declare(Events) */

/**
 * @} end addtogroup groupComm
 */

#endif                                                          /* COMMMGR_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
