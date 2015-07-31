/*****************************************************************************
* Model: SerialMgr.qm
* File:  ./SerialMgr_gen.c
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
/*${.::SerialMgr_gen.c} ....................................................*/
/**
 * @file    SerialMgr.c
 * @brief   Declarations for functions for the SerialMgr AO.
 * This state machine handles all output to the serial port.
 *
 * @note 1: If editing this file, please make sure to update the SerialMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    06/02/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupSerial
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "SerialMgr.h"
#include "project_includes.h"         /* Includes common to entire project. */
#include "bsp.h"        /* For seconds to bsp tick conversion (SEC_TO_TICK) */
#include "serial.h"                           /* For low level serial support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE;                 /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_SER ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/

/**
 * @brief SerialMgr Active Object (AO) "class" that manages the debug serial port.
 * Upon receiving an event it copies the data to the UART DMA buffer and activates
 * DMA to do the actual outputting to serial, freeing up the system to continue to
 * do real work. The interrupt at the end of the DMA process disables the DMA and
 * lets the AO know that the transfer has completed.  See SerialMgr.qm for diagram
 * and model.
 */
/*${AOs::SerialMgr} ........................................................*/
typedef struct {
/* protected: */
    QActive super;

    /**< QPC timer Used to timeout serial transfers if errors occur. */
    QTimeEvt serialTimerEvt;

    /**< Native QF queue for deferred request events. */
    QEQueue deferredEvtQueue;

    /**< Storage for deferred event queue. */
    QTimeEvt const * deferredEvtQSto[200];

    /**< Keep track of whether serial debug is enabled or disabled.  Starts out enabled
     * but an event can enable it. */
    bool isSerialDbgEnabled;
} SerialMgr;

/* protected: */
static QState SerialMgr_initial(SerialMgr * const me, QEvt const * const e);

/**
 * @brief This state is a catch-all Active state.
 * If any signals need to be handled that do not cause state transitions and
 * are common to the entire AO, they should be handled here.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status_: QState type that specifies where the state
 * machine is going next.
 */
static QState SerialMgr_Active(SerialMgr * const me, QEvt const * const e);

/**
 * @brief This state indicates that the DMA is currently idle and the
 * incoming msg can be sent out to the DMA buffer to be printed out to the
 * serial port.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState SerialMgr_Idle(SerialMgr * const me, QEvt const * const e);

/**
 * @brief   This state indicates that the DMA is currently busy outputting to
 * the serial port and cannot process incoming data; incoming events will be
 * deferred in this state and handled once the AO goes back to Idle state.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState SerialMgr_Busy(SerialMgr * const me, QEvt const * const e);


/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static SerialMgr l_SerialMgr; /* the single instance of the active object */

/* Global-scope objects ----------------------------------------------------*/
QActive * const AO_SerialMgr = (QActive *)&l_SerialMgr;  /* "opaque" AO pointer */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief C "constructor" for SerialMgr "class".
 * Initializes all the timers and queues used by the AO, sets up a deferral
 * queue, and sets of the first state.
 * @param [in] enSerialDbg: bool that specifies whether to enable debugging output
 * over serial on startup of the AO.
 * @retval None
 */
/*${AOs::SerialMgr_ctor} ...................................................*/
void SerialMgr_ctor(bool en) {
    SerialMgr *me = &l_SerialMgr;
    QActive_ctor(&me->super, (QStateHandler)&SerialMgr_initial);
    QTimeEvt_ctor(&me->serialTimerEvt, UART_DMA_TIMEOUT_SIG);

    /* Initialize the deferred event queue and storage for it */
    QEQueue_init(
        &me->deferredEvtQueue,
        (QEvt const **)( me->deferredEvtQSto ),
        Q_DIM(me->deferredEvtQSto)
    );

    /* Let the caller determine whether serial debugging is enabled by default or not. */
    me->isSerialDbgEnabled = en;
}

/**
 * @brief Check if serial debug is enabled
 *
 * @param	None
 * @retval 	bool:
 *   @arg true : serial debug is enabled
 *   @arg false: serial debug is disabled
 */
/*${AOs::SerialMgr_isDbgE~} ................................................*/
bool SerialMgr_isDbgEnabled(void) {
    SerialMgr *me = &l_SerialMgr;
    return( me->isSerialDbgEnabled );
}

/**
 * @brief Enable or disable debug over serial
 *
 * @param [in] en: bool that specifies if debug is to be enabled (true) or
 * disabled (false)
 * @retval 	None
 */
/*${AOs::SerialMgr_setDbg~} ................................................*/
void SerialMgr_setDbgEnabled(bool en) {
    SerialMgr *me = &l_SerialMgr;
    me->isSerialDbgEnabled = en;
}

/**
 * @brief SerialMgr Active Object (AO) "class" that manages the debug serial port.
 * Upon receiving an event it copies the data to the UART DMA buffer and activates
 * DMA to do the actual outputting to serial, freeing up the system to continue to
 * do real work. The interrupt at the end of the DMA process disables the DMA and
 * lets the AO know that the transfer has completed.  See SerialMgr.qm for diagram
 * and model.
 */
/*${AOs::SerialMgr} ........................................................*/
/*${AOs::SerialMgr::SM} ....................................................*/
static QState SerialMgr_initial(SerialMgr * const me, QEvt const * const e) {
    /* ${AOs::SerialMgr::SM::initial} */
    (void)e;        /* suppress the compiler warning about unused parameter */

    QS_OBJ_DICTIONARY(&l_SerialMgr);
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&SerialMgr_initial);
    QS_FUN_DICTIONARY(&SerialMgr_Active);
    QS_FUN_DICTIONARY(&SerialMgr_Idle);
    QS_FUN_DICTIONARY(&SerialMgr_Busy);

    QActive_subscribe((QActive *)me, UART_DMA_START_SIG);
    QActive_subscribe((QActive *)me, DBG_LOG_SIG);
    QActive_subscribe((QActive *)me, DBG_MENU_SIG);
    QActive_subscribe((QActive *)me, UART_DMA_DONE_SIG);
    QActive_subscribe((QActive *)me, UART_DMA_TIMEOUT_SIG);
    QActive_subscribe((QActive *)me, UART_DMA_DBG_TOGGLE_SIG);
    return Q_TRAN(&SerialMgr_Idle);
}

/**
 * @brief This state is a catch-all Active state.
 * If any signals need to be handled that do not cause state transitions and
 * are common to the entire AO, they should be handled here.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status_: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::SerialMgr::SM::Active} ............................................*/
static QState SerialMgr_Active(SerialMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::SerialMgr::SM::Active} */
        case Q_ENTRY_SIG: {
            /* Post a timer and disarm it right away so it can be
             * rearmed at any point */
            QTimeEvt_postIn(
                &me->serialTimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( LL_MAX_TIMEOUT_SERIAL_DMA_BUSY_SEC )
            );
            QTimeEvt_disarm(&me->serialTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::SerialMgr::SM::Active::UART_DMA_DBG_TOG~} */
        case UART_DMA_DBG_TOGGLE_SIG: {
            me->isSerialDbgEnabled = !(me->isSerialDbgEnabled);
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/**
 * @brief This state indicates that the DMA is currently idle and the
 * incoming msg can be sent out to the DMA buffer to be printed out to the
 * serial port.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::SerialMgr::SM::Active::Idle} ......................................*/
static QState SerialMgr_Idle(SerialMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::SerialMgr::SM::Active::Idle} */
        case Q_ENTRY_SIG: {
            /* recall the request from the private requestQueue */
            QActive_recall(
                (QActive *)me,
                &me->deferredEvtQueue
            );
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::SerialMgr::SM::Active::Idle::UART_DMA_START, ~} */
        case UART_DMA_START_SIG: /* intentionally fall through */
        case CLI_SEND_DATA_SIG: /* intentionally fall through */
        case DBG_MENU_SIG: {
            /* Set up the DMA buffer here.  This copies the data from the event to the UART's
             * private buffer as well to avoid someone overwriting it */
            Serial_DMAConfig(
                SERIAL_UART1,
                (char *)((LrgDataEvt const *) e)->dataBuf,
                ((LrgDataEvt const *) e)->dataLen
            );
            status_ = Q_TRAN(&SerialMgr_Busy);
            break;
        }
        /* ${AOs::SerialMgr::SM::Active::Idle::DBG_LOG} */
        case DBG_LOG_SIG: {
            /* ${AOs::SerialMgr::SM::Active::Idle::DBG_LOG::[SerialDbgEnable~} */
            if (true == me->isSerialDbgEnabled) {
                /* Set up the DMA buffer here.  This copies the data from the event to the UART's
                 * private buffer as well to avoid someone overwriting it */
                Serial_DMAConfig(
                    SERIAL_UART1,
                    (char *)((LrgDataEvt const *) e)->dataBuf,
                    ((LrgDataEvt const *) e)->dataLen
                );
                status_ = Q_TRAN(&SerialMgr_Busy);
            }
            /* ${AOs::SerialMgr::SM::Active::Idle::DBG_LOG::[else]} */
            else {
                status_ = Q_HANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&SerialMgr_Active);
            break;
        }
    }
    return status_;
}

/**
 * @brief   This state indicates that the DMA is currently busy outputting to
 * the serial port and cannot process incoming data; incoming events will be
 * deferred in this state and handled once the AO goes back to Idle state.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::SerialMgr::SM::Active::Busy} ......................................*/
static QState SerialMgr_Busy(SerialMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::SerialMgr::SM::Active::Busy} */
        case Q_ENTRY_SIG: {
            /* Post a timer on entry */
            QTimeEvt_rearm(
                &me->serialTimerEvt,
                SEC_TO_TICKS( LL_MAX_TIMEOUT_SERIAL_DMA_BUSY_SEC )
            );

            /* Start the DMA transfer over serial */
            Serial_DMAStartXfer( SERIAL_UART1 );
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::SerialMgr::SM::Active::Busy} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm( &me->serialTimerEvt ); /* Disarm timer on exit */
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::SerialMgr::SM::Active::Busy::UART_DMA_DONE} */
        case UART_DMA_DONE_SIG: {
            status_ = Q_TRAN(&SerialMgr_Idle);
            break;
        }
        /* ${AOs::SerialMgr::SM::Active::Busy::UART_DMA_TIMEOUT} */
        case UART_DMA_TIMEOUT_SIG: {
            err_slow_printf("UART DMA timeout occurred\n");
            status_ = Q_TRAN(&SerialMgr_Idle);
            break;
        }
        /* ${AOs::SerialMgr::SM::Active::Busy::UART_DMA_START, ~} */
        case UART_DMA_START_SIG: /* intentionally fall through */
        case DBG_LOG_SIG: /* intentionally fall through */
        case DBG_MENU_SIG: /* intentionally fall through */
        case CLI_SEND_DATA_SIG: {
            if (QEQueue_getNFree(&me->deferredEvtQueue) > 0) {
               /* defer the request - this event will be handled
                * when the state machine goes back to Idle state */
               QActive_defer((QActive *)me, &me->deferredEvtQueue, e);
            } else {
               /* notify the request sender that the request was ignored.. */
               err_slow_printf("Unable to defer UART_DMA_START request\n");
            }
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&SerialMgr_Active);
            break;
        }
    }
    return status_;
}


/**
 * @} end addtogroup groupSerial
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
