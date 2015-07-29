/*****************************************************************************
* Model: LWIPMgr.qm
* File:  ./LWIPMgr_gen.h
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
/*${.::LWIPMgr_gen.h} ......................................................*/
/**
 * @file  LWIPMgr.h
 * @brief This file contains the declarations of the LWIPMgr Active Object and
 * its state machines.
 *
 * LWIPMgr is responsible for handling all communications with the ethernet
 * driver.  It is the only state machine that has exclusive access to the
 * ethernet hardware and facilitates sharing by allowing other state machines
 * to publish events to the QPC RTOS.  QPC queues the events to be handled by
 * the LWIPMgr state machine in the FIFO order.
 *
 * Note: If editing this file, please make sure to update the LWIPMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date       06/09/2014
 * @author     Harry Rostovtsev
 * @email      harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupLWIP_QPC_Eth
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LWIPMGR_H_
#define LWIPMGR_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"           /* Includes common to entire project. */
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/*! \enum LWIPMgr Signals
 * Signals used by LWIPMgr.  These should start from MAX_SHARED_SIG
 */
enum LWIPMgrSignals {
    ETH_UDP_SEND_SIG = MAX_SHARED_SIG,
    ETH_DBG_TOGGLE_SIG,
    ETH_LOG_TCP_SEND_SIG,
    ETH_SYS_TCP_SEND_SIG,
    ETH_TCP_DATA_RECV_SIG,
    TCP_DONE_SIG,
    TCP_TIMEOUT_SIG,
    MAX_PUB_SIG,                                  /* the last published signal */
};


/**
 * \struct Event struct type for getting data from Eth driver to LWIPMgr.
 */
/*${Events::EthEvt} ........................................................*/
typedef struct {
/* protected: */
    QEvt super;

    /**< Where the msg came from so it can be routed back to the sender. */
    DC3MsgRoute_t msg_src;

    /**< Length of the msg buffer. */
    uint16_t msg_len;

    /**< Buffer that holds the data of the msg. */
    char msg[DC3_MAX_MSG_LEN];
} EthEvt;


/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief C "constructor" for LWIP "class".
 * Initializes all the timers and queues used by the AO and sets of the
 * first state.
 * @param  None
 * @param  None
 * @retval None
 */
/*${AOs::LWIPMgr_ctor} .....................................................*/
void LWIPMgr_ctor(void);


/**< "opaque" pointer to the Active Object */
extern QActive * const AO_LWIPMgr;


/**
 * @brief    Send UDP msg over ethernet.
 * This function wraps a buffer, length, destination, and source and creates a
 * correct event to post to the LWIPMgr AO to send over UDP.
 *
 * @param [in] *dataBuf: const uint8_t pointer to the buffer of data to send.
 * @param [in]  dataLen: length of data to send.
 *
 * @return DC3Error_t: status of send
 */
DC3Error_t ETH_SendUdp(
      const uint8_t* const dataBuf,
      const uint16_t const dataLen
);

/**
 * @}
 * end addtogroup groupLWIP_QPC_Eth
 */
#endif                                                          /* LWIPMGR_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
