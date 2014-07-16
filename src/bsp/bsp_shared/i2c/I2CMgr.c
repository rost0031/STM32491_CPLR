/*****************************************************************************
* Model: I2CMgr.qm
* File:  ./I2CMgr_gen.c
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
/*${.::I2CMgr_gen.c} .......................................................*/
/**
 * @file    I2CMgr.c
 * @brief   Declarations for functions for the I2CMgr AO.
 * This state machine handles all I/O on the I2C bus.
 *
 * @note 1: If editing this file, please make sure to update the I2CMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    07/01/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "I2CMgr.h"
#include "project_includes.h"           /* Includes common to entire project. */
#include "bsp.h"          /* For seconds to bsp tick conversion (SEC_TO_TICK) */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/

/**
 * @brief I2CMgr Active Object (AO) "class" that manages the I2C bus.
 * This AO manages the I2C bus and all events associated with it. It
 * has exclusive access to the I2C bus and the ISR handlers will let
 * the AO know that the transfer has completed.  See I2CMgr.qm for
 * diagram and model.
 */
/*${AOs::I2CMgr} ...........................................................*/
typedef struct {
/* protected: */
    QActive super;

    /**< QPC timer Used to timeout I2C transfers if errors occur. */
    QTimeEvt i2cTimerEvt;

    /**< Native QF queue for deferred request events. */
    QEQueue deferredEvtQueue;

    /**< Storage for deferred event queue. */
    QTimeEvt const * deferredEvtQSto[100];

    /**< Flag that indicates whether the bus is free or busy */
    FlagStatus bBusBusy;

    /**< QPC timer Used to time retries for checking if the I2C bus is busy. */
    QTimeEvt i2cBusBusyTimerEvt;

    /**< Keep track of number of retries used to check if the I2C bus is still busy. */
    uint8_t nBusRetries;

    /**< Address on the I2C device to read or write to.  This will be used to store
     the address coming from the events. */
    uint16_t wAddr;

    /**< Specifies which I2C device is currently being handled by this AO.  This should
         be set when a new I2C_READ_START or I2C_WRITE_START events come in.  Those
         events should contain the device for which they are meant for. */
    I2C_Device_t iDevice;

    /**< Which I2C bus this AO is responsible for.  This variable is set on
         startup and is used to index into the structure that holds all the
         I2C bus settings. */
    I2C_Bus_t iBus;

    /**< Counter used to manually timeout some I2C operations.  Though we supposed to
         not do blocking operations like this, it's unavoidable in this case since
         the I2C ISRs won't post events until they are cleared, which happens after
         here in the AO so nothing moves forward.  With all the delays introduced by
         just event handling, there should be no blocking in reality but just in case,
         there will still be timeout events launched from these loops if this counter
         gets to 0. */
    uint32_t nI2CLoopTimeout;

    /**< QPC timer Used to time I2C DMA operations. */
    QTimeEvt i2cDMATimerEvt;

    /**< Number of bytes to read or write to the I2C device */
    uint16_t nLen;
} I2CMgr;

/* protected: */
static QState I2CMgr_initial(I2CMgr * const me, QEvt const * const e);

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
static QState I2CMgr_Active(I2CMgr * const me, QEvt const * const e);

/**
 * @brief   This state indicates that the I2C is currently busy and cannot
 * process incoming data; incoming events will be deferred in this state and
 * handled once the AO goes back to Idle state.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState I2CMgr_Busy(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_BusBeingUsed(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_Reading(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitFor_I2C_EV5_R(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitFor_I2C_EV6_R(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitForDMAData(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitFor_I2C_EV5(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitFor_I2C_EV6(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitFor_I2C_EV8_MSB(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitFor_I2C_EV8_LSB(I2CMgr * const me, QEvt const * const e);
static QState I2CMgr_WaitForBusFree(I2CMgr * const me, QEvt const * const e);

/**
 * @brief This state indicates that the I2C bus is currently idle and the
 * incoming msg can be handled.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState I2CMgr_Idle(I2CMgr * const me, QEvt const * const e);


/* Private defines -----------------------------------------------------------*/
#define MAX_BUS_RETRIES    10 /**< Max number of retries for I2C bus for busy flag */
#define MAX_I2C_TIMEOUT 0xF00 /**< Max number of retries for I2C bus for busy flag */
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static I2CMgr l_I2CMgr;           /* the single instance of the active object */

/* Global-scope objects ----------))------------------------------------------*/
QActive * const AO_I2CMgr = (QActive *)&l_I2CMgr;      /* "opaque" AO pointer */
extern I2C_BusSettings_t s_I2C_Bus[MAX_I2C_BUS];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief C "constructor" for I2CMgr "class".
 * Initializes all the timers and queues used by the AO, sets up a deferral
 * queue, and sets of the first state.
 * @param  [in] i2cBus: I2C_TypeDef * type that specifies which STM32 I2C bus this
 * AO is responsible for.
 * @retval None
 */
/*${AOs::I2CMgr_ctor} ......................................................*/
void I2CMgr_ctor(I2C_Bus_t iBus) {
    I2CMgr *me = &l_I2CMgr;
    me->iBus = iBus;

    QActive_ctor( &me->super, (QStateHandler)&I2CMgr_initial );
    QTimeEvt_ctor( &me->i2cTimerEvt, I2C_TIMEOUT_SIG );
    QTimeEvt_ctor( &me->i2cBusBusyTimerEvt, I2C_BUS_RETRY_TIMEOUT_SIG );
    QTimeEvt_ctor( &me->i2cDMATimerEvt, I2C_DMA_TIMEOUT_SIG );

    /* Initialize the deferred event queue and storage for it */
    QEQueue_init(
        &me->deferredEvtQueue,
        (QEvt const **)( me->deferredEvtQSto ),
        Q_DIM(me->deferredEvtQSto)
    );

    dbg_slow_printf("Constructor\n");
}

/**
 * @brief I2CMgr Active Object (AO) "class" that manages the I2C bus.
 * This AO manages the I2C bus and all events associated with it. It
 * has exclusive access to the I2C bus and the ISR handlers will let
 * the AO know that the transfer has completed.  See I2CMgr.qm for
 * diagram and model.
 */
/*${AOs::I2CMgr} ...........................................................*/
/*${AOs::I2CMgr::SM} .......................................................*/
static QState I2CMgr_initial(I2CMgr * const me, QEvt const * const e) {
    /* ${AOs::I2CMgr::SM::initial} */
    (void)e;        /* suppress the compiler warning about unused parameter */

    QS_OBJ_DICTIONARY(&l_I2CMgr);
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&I2CMgr_initial);
    QS_FUN_DICTIONARY(&I2CMgr_Active);
    QS_FUN_DICTIONARY(&I2CMgr_Idle);
    QS_FUN_DICTIONARY(&I2CMgr_Busy);
    QS_FUN_DICTIONARY(&I2CMgr_Read);

    QActive_subscribe((QActive *)me, I2C_READ_START_SIG);
    QActive_subscribe((QActive *)me, I2C_WRITE_START_SIG);
    QActive_subscribe((QActive *)me, I2C_TIMEOUT_SIG);
    QActive_subscribe((QActive *)me, I2C_BUS_RETRY_TIMEOUT_SIG);
    QActive_subscribe((QActive *)me, I2C_CHECK_EV_SIG);
    QActive_subscribe((QActive *)me, I2C_READ_DONE_SIG);
    QActive_subscribe((QActive *)me, I2C_DMA_TIMEOUT_SIG);
    return Q_TRAN(&I2CMgr_Idle);
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
/*${AOs::I2CMgr::SM::Active} ...............................................*/
static QState I2CMgr_Active(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active} */
        case Q_ENTRY_SIG: {
            /* Post all the timers and disarm them right away so it can be
             * rearmed at any point without worrying asserts. */
            QTimeEvt_postIn(
                &me->i2cTimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( LL_MAX_TIMEOUT_SERIAL_DMA_BUSY_SEC )
            );
            QTimeEvt_disarm(&me->i2cTimerEvt);

            QTimeEvt_postIn(
                &me->i2cBusBusyTimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( LL_MAX_TIMEOUT_I2C_READ_OP_SEC )
            );
            QTimeEvt_disarm(&me->i2cBusBusyTimerEvt);

            QTimeEvt_postIn(
                &me->i2cDMATimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( LL_MAX_TIMEOUT_I2C_READ_OP_SEC )
            );
            QTimeEvt_disarm(&me->i2cDMATimerEvt);

            /* Initialize the I2C devices and associated busses */
            I2C_BusInit( me->iBus );
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
 * @brief   This state indicates that the I2C is currently busy and cannot
 * process incoming data; incoming events will be deferred in this state and
 * handled once the AO goes back to Idle state.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::I2CMgr::SM::Active::Busy} .........................................*/
static QState I2CMgr_Busy(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy} */
        case Q_ENTRY_SIG: {
            /* Post a timer on entry */
            QTimeEvt_rearm(
                &me->i2cTimerEvt,
                SEC_TO_TICKS( LL_MAX_TIMEOUT_I2C_BUSY_SEC )
            );

            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm( &me->i2cTimerEvt ); /* Disarm timer on exit */
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::I2C_TIMEOUT} */
        case I2C_TIMEOUT_SIG: {
            ERR_printf("I2C timeout occurred\n");
            status_ = Q_TRAN(&I2CMgr_Idle);
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::I2C_READ_START, I2C_WRITE_START} */
        case I2C_READ_START_SIG: /* intentionally fall through */
        case I2C_WRITE_START_SIG: {
            if (QEQueue_getNFree(&me->deferredEvtQueue) > 0) {
               /* defer the request - this event will be handled
                * when the state machine goes back to Idle state */
               QActive_defer((QActive *)me, &me->deferredEvtQueue, e);
            } else {
               /* notify the request sender that the request was ignored.. */
               ERR_printf("Unable to defer I2C request\n");
            }
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_Active);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed} ...........................*/
static QState I2CMgr_BusBeingUsed(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed} */
        case Q_ENTRY_SIG: {
            DBG_printf("Generating I2C start\n");

            /* Send START condition */
            I2C_GenerateSTART(s_I2C_Bus[me->iBus].i2c_bus, ENABLE);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed} */
        case Q_EXIT_SIG: {
            I2C_GenerateSTOP(s_I2C_Bus[me->iBus].i2c_bus, ENABLE);
            DBG_printf("Generating I2C stop\n");
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_Busy);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading} ..................*/
static QState I2CMgr_Reading(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::I2C_READ_DONE} */
        case I2C_READ_DONE_SIG: {
            DBG_printf("Read finished successfully: read %d bytes\n", ((I2CDataEvt const *)e)->wBufferLen);
            status_ = Q_TRAN(&I2CMgr_Idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_BusBeingUsed);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV5_R} */
static QState I2CMgr_WaitFor_I2C_EV5_R(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV5_R} */
        case Q_ENTRY_SIG: {
            /* Post an event to check for EV5 event */
            QEvt *qEvt = Q_NEW(QEvt, I2C_CHECK_EV_SIG);
            QF_PUBLISH((QEvt *)qEvt, AO_I2CMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV5_R::I2C_CHECK_EV} */
        case I2C_CHECK_EV_SIG: {
            /* Check if EV5 has happened.  If it has, go on to the next state.  Otherwise,
             * try again until number of retries is out */
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV5_R::I2C_CHECK_EV::[EV5Happened?]} */
            if (I2C_CheckEvent(s_I2C_Bus[me->iBus].i2c_bus, I2C_EVENT_MASTER_MODE_SELECT)) {
                DBG_printf("Selecting slave I2C Device\n");

                /* Set the direction to receive */
                I2C_SetDirection( me->iBus,  I2C_Direction_Receiver);

                /* Send slave Address for read */
                I2C_Send7bitAddress(
                    s_I2C_Bus[me->iBus].i2c_bus,           /* This is always the bus used in this ISR */
                    s_I2C_Bus[me->iBus].i2c_cur_dev_addr,  /* Look up the current device address for this bus */
                    s_I2C_Bus[me->iBus].bTransDirection    /* Direction of data on this bus */
                );

                /* Reset the maximum number of times to poll the I2C bus for an event */
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT * 10000;
                status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV6_R);
            }
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV5_R::I2C_CHECK_EV::[else]} */
            else {
                me->nI2CLoopTimeout--;                 /* Decrement counter */
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV5_R::I2C_CHECK_EV::[else]::[Retriesleft?]} */
                if (me->nI2CLoopTimeout != 0) {
                    status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV5_R);
                }
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV5_R::I2C_CHECK_EV::[else]::[else]} */
                else {
                    ERR_printf("Timeout waiting for I2C_EVENT_MASTER_MODE_SELECT (EV5)\n");
                    status_ = Q_TRAN(&I2CMgr_Idle);
                }
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_Reading);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV6_R} */
static QState I2CMgr_WaitFor_I2C_EV6_R(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV6_R} */
        case Q_ENTRY_SIG: {
            /* Post an event to check for EV5 event */
            QEvt *qEvt = Q_NEW(QEvt, I2C_CHECK_EV_SIG);
            QF_PUBLISH((QEvt *)qEvt, AO_I2CMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV6_R::I2C_CHECK_EV} */
        case I2C_CHECK_EV_SIG: {
            /* Check if EV6 has happened.  If it has, go on to the next state.  Otherwise,
             * try again until number of retries is out */
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV6_R::I2C_CHECK_EV::[EV6Happened?]} */
            if (I2C_CheckEvent( s_I2C_Bus[me->iBus].i2c_bus, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED )) {
                DBG_printf("Starting to read the I2C Device data...\n");

                /* Reset the maximum number of times to poll the I2C bus for an event */
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT;
                status_ = Q_TRAN(&I2CMgr_WaitForDMAData);
            }
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV6_R::I2C_CHECK_EV::[else]} */
            else {
                me->nI2CLoopTimeout--;                 /* Decrement counter */
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV6_R::I2C_CHECK_EV::[else]::[Retriesleft?]} */
                if (me->nI2CLoopTimeout != 0) {
                    status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV6_R);
                }
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitFor_I2C_EV6_R::I2C_CHECK_EV::[else]::[else]} */
                else {
                    ERR_printf("Timeout waiting for I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED (EV6)\n");
                    status_ = Q_TRAN(&I2CMgr_Idle);
                }
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_Reading);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitForDMAData} ..*/
static QState I2CMgr_WaitForDMAData(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitForDMAData} */
        case Q_ENTRY_SIG: {
            /* Post a timer on entry */
            QTimeEvt_rearm(
                &me->i2cDMATimerEvt,
                SEC_TO_TICKS( LL_MAX_TIMEOUT_I2C_DMA_READ_SEC )
            );

            /* Start the DMA read operation */
            I2C_DMARead(
                me->iBus,
                me->wAddr,
                me->nLen
            );
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitForDMAData} */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm( &me->i2cDMATimerEvt );
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::Reading::WaitForDMAData::I2C_DMA_TIMEOUT} */
        case I2C_DMA_TIMEOUT_SIG: {
            ERR_printf("Timeout while waiting for DMA read timeout\n");
            status_ = Q_TRAN(&I2CMgr_Idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_Reading);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV5} ..........*/
static QState I2CMgr_WaitFor_I2C_EV5(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV5} */
        case Q_ENTRY_SIG: {
            /* Post an event to check for EV5 event */
            QEvt *qEvt = Q_NEW(QEvt, I2C_CHECK_EV_SIG);
            QF_PUBLISH((QEvt *)qEvt, AO_I2CMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV5::I2C_CHECK_EV} */
        case I2C_CHECK_EV_SIG: {
            /* Check if EV5 has happened.  If it has, go on to the next state.  Otherwise,
             * try again until number of retries is out */
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV5::I2C_CHECK_EV::[EV5Happened?]} */
            if (I2C_CheckEvent(s_I2C_Bus[me->iBus].i2c_bus, I2C_EVENT_MASTER_MODE_SELECT)) {
                DBG_printf("Selecting slave I2C Device\n");

                /* Send slave Address for write */
                I2C_Send7bitAddress(
                    s_I2C_Bus[me->iBus].i2c_bus,            /* This is always the bus used in this ISR */
                    s_I2C_Bus[me->iBus].i2c_cur_dev_addr,   /* Look up the current device address for this bus */
                    s_I2C_Bus[me->iBus].bTransDirection     /* Direction of data on this bus */
                );

                /* Reset the maximum number of times to poll the I2C bus for an event */
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT;
                status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV6);
            }
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV5::I2C_CHECK_EV::[else]} */
            else {
                me->nI2CLoopTimeout--;                 /* Decrement counter */
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV5::I2C_CHECK_EV::[else]::[Retriesleft?]} */
                if (me->nI2CLoopTimeout != 0) {
                    status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV5);
                }
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV5::I2C_CHECK_EV::[else]::[else]} */
                else {
                    ERR_printf("Timeout waiting for I2C_EVENT_MASTER_MODE_SELECT (EV5)\n");
                    status_ = Q_TRAN(&I2CMgr_Idle);
                }
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_BusBeingUsed);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV6} ..........*/
static QState I2CMgr_WaitFor_I2C_EV6(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV6} */
        case Q_ENTRY_SIG: {
            /* Post an event to check for EV5 event */
            QEvt *qEvt = Q_NEW(QEvt, I2C_CHECK_EV_SIG);
            QF_PUBLISH((QEvt *)qEvt, AO_I2CMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV6::I2C_CHECK_EV} */
        case I2C_CHECK_EV_SIG: {
            /* Check if EV6 has happened.  If it has, go on to the next state.  Otherwise,
             * try again until number of retries is out */
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV6::I2C_CHECK_EV::[EV6Happened?]} */
            if (I2C_CheckEvent( s_I2C_Bus[me->iBus].i2c_bus, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
                DBG_printf("Sending internal MSB addr to the I2C Device\n");

                /* Send the MSB of the address first to the I2C device */
                I2C_SendData(
                    s_I2C_Bus[me->iBus].i2c_bus,
                    (uint8_t)((me->wAddr & 0xFF00) >> 8)
                );

                /* Reset the maximum number of times to poll the I2C bus for an event */
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT;
                status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV8_MSB);
            }
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV6::I2C_CHECK_EV::[else]} */
            else {
                me->nI2CLoopTimeout--;                 /* Decrement counter */
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV6::I2C_CHECK_EV::[else]::[Retriesleft?]} */
                if (me->nI2CLoopTimeout != 0) {
                    status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV6);
                }
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV6::I2C_CHECK_EV::[else]::[else]} */
                else {
                    ERR_printf("Timeout waiting for I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED (EV6)\n");
                    status_ = Q_TRAN(&I2CMgr_Idle);
                }
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_BusBeingUsed);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_MSB} ......*/
static QState I2CMgr_WaitFor_I2C_EV8_MSB(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_MSB} */
        case Q_ENTRY_SIG: {
            /* Post an event to check for EV5 event */
            QEvt *qEvt = Q_NEW(QEvt, I2C_CHECK_EV_SIG);
            QF_PUBLISH((QEvt *)qEvt, AO_I2CMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_MSB::I2C_CHECK_EV} */
        case I2C_CHECK_EV_SIG: {
            /* Check if EV6 has happened.  If it has, go on to the next state.  Otherwise,
             * try again until number of retries is out */
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_MSB::I2C_CHECK_EV::[EV8Happened?]} */
            if (I2C_CheckEvent( s_I2C_Bus[me->iBus].i2c_bus, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
                DBG_printf("Sending internal LSB addr to the I2C Device\n");

                /* Send the MSB of the address first to the I2C device */
                I2C_SendData(
                    s_I2C_Bus[me->iBus].i2c_bus,
                    (uint8_t)(me->wAddr & 0xFF00)
                );

                /* Reset the maximum number of times to poll the I2C bus for an event */
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT;
                status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV8_LSB);
            }
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_MSB::I2C_CHECK_EV::[else]} */
            else {
                me->nI2CLoopTimeout--;                 /* Decrement counter */
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_MSB::I2C_CHECK_EV::[else]::[Retriesleft?]} */
                if (me->nI2CLoopTimeout != 0) {
                    status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV8_MSB);
                }
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_MSB::I2C_CHECK_EV::[else]::[else]} */
                else {
                    ERR_printf("Timeout waiting for I2C_EVENT_MASTER_BYTE_TRANSMITTING (EV8)\n");
                    status_ = Q_TRAN(&I2CMgr_Idle);
                }
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_BusBeingUsed);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_LSB} ......*/
static QState I2CMgr_WaitFor_I2C_EV8_LSB(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_LSB} */
        case Q_ENTRY_SIG: {
            /* Post an event to check for EV5 event */
            QEvt *qEvt = Q_NEW(QEvt, I2C_CHECK_EV_SIG);
            QF_PUBLISH((QEvt *)qEvt, AO_I2CMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_LSB::I2C_CHECK_EV} */
        case I2C_CHECK_EV_SIG: {
            /* Check if EV6 has happened.  If it has, go on to the next state.  Otherwise,
             * try again until number of retries is out */
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_LSB::I2C_CHECK_EV::[EV8Happened?]} */
            if (SET == I2C_GetFlagStatus(s_I2C_Bus[me->iBus].i2c_bus, I2C_FLAG_BTF)) {
                DBG_printf("Sending a second START bit to the I2C Device\n");

                /* Send START condition */
                I2C_GenerateSTART(s_I2C_Bus[me->iBus].i2c_bus, ENABLE);

                /* Reset the maximum number of times to poll the I2C bus for an event */
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT;
                status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV5_R);
            }
            /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_LSB::I2C_CHECK_EV::[else]} */
            else {
                me->nI2CLoopTimeout--;                 /* Decrement counter */
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_LSB::I2C_CHECK_EV::[else]::[Retriesleft?]} */
                if (me->nI2CLoopTimeout != 0) {
                    status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV8_LSB);
                }
                /* ${AOs::I2CMgr::SM::Active::Busy::BusBeingUsed::WaitFor_I2C_EV8_LSB::I2C_CHECK_EV::[else]::[else]} */
                else {
                    ERR_printf("Timeout waiting for I2C_EVENT_MASTER_BYTE_TRANSMITTING (EV8)\n");
                    status_ = Q_TRAN(&I2CMgr_Idle);
                }
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_BusBeingUsed);
            break;
        }
    }
    return status_;
}
/*${AOs::I2CMgr::SM::Active::Busy::WaitForBusFree} .........................*/
static QState I2CMgr_WaitForBusFree(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Busy::WaitForBusFree} */
        case Q_ENTRY_SIG: {
            /* Create MsgEvt event to send out the message */
            QEvt *qEvt = Q_NEW(QEvt, I2C_BUS_RETRY_TIMEOUT_SIG);

            /* Post the msgEvt for this AO (CommStackMgr) to handle */
            QF_PUBLISH((QEvt *)qEvt, AO_I2CMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Busy::WaitForBusFree::I2C_BUS_RETRY_TIMEOUT} */
        case I2C_BUS_RETRY_TIMEOUT_SIG: {
            me->nBusRetries++;

            I2C_SoftwareResetCmd(s_I2C_Bus[me->iBus].i2c_bus, ENABLE);
            I2C_SoftwareResetCmd(s_I2C_Bus[me->iBus].i2c_bus, DISABLE);
            DBG_printf("Reset bus\n");

            me->bBusBusy = I2C_GetFlagStatus(s_I2C_Bus[me->iBus].i2c_bus, I2C_FLAG_BUSY);
            /* ${AOs::I2CMgr::SM::Active::Busy::WaitForBusFree::I2C_BUS_RETRY_TIMEOUT::[BusBusy?]} */
            if (SET == me->bBusBusy) {
                DBG_printf("Bus still busy after %d retries\n", me->nBusRetries);
                /* ${AOs::I2CMgr::SM::Active::Busy::WaitForBusFree::I2C_BUS_RETRY_TIMEOUT::[BusBusy?]::[Retriesleft?]} */
                if (MAX_BUS_RETRIES > me->nBusRetries) {
                    DBG_printf("Retrying\n");
                    status_ = Q_TRAN(&I2CMgr_WaitForBusFree);
                }
                /* ${AOs::I2CMgr::SM::Active::Busy::WaitForBusFree::I2C_BUS_RETRY_TIMEOUT::[BusBusy?]::[else]} */
                else {
                    DBG_printf("Out of retries\n");
                    status_ = Q_TRAN(&I2CMgr_Idle);
                }
            }
            /* ${AOs::I2CMgr::SM::Active::Busy::WaitForBusFree::I2C_BUS_RETRY_TIMEOUT::[else]} */
            else {
                DBG_printf("Bus free after %d retries\n", me->nBusRetries);
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT;
                status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV5);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_Busy);
            break;
        }
    }
    return status_;
}

/**
 * @brief This state indicates that the I2C bus is currently idle and the
 * incoming msg can be handled.
 *
 * @param  [in,out] me: Pointer to the state machine
 * @param  [in,out] e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::I2CMgr::SM::Active::Idle} .........................................*/
static QState I2CMgr_Idle(I2CMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::I2CMgr::SM::Active::Idle} */
        case Q_ENTRY_SIG: {
            /* recall the request from the private requestQueue */
            I2CDataEvt const *rq = (I2CDataEvt const *) (uint32_t) QActive_recall(
                (QActive *)me,
                &me->deferredEvtQueue
            );

            /* Reset all counters */
            me->nBusRetries = 0;
            DBG_printf("back in Idle\n");
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::I2CMgr::SM::Active::Idle::I2C_READ_START} */
        case I2C_READ_START_SIG: {
            DBG_printf("Got I2C_READ_START\n");

            /* Store the device */
            me->iDevice = ((I2CReqEvt const *)e)->i2cDevice;

            /* Store the address */
            me->wAddr = ((I2CReqEvt const *)e)->wReadAddr;
            me->nLen  = ((I2CReqEvt const *)e)->nReadLen;

            //I2C_SoftwareResetCmd(I2C1, ENABLE);
            //I2C_SoftwareResetCmd(I2C1, DISABLE);

            /* Check if the bus is busy */
            me->bBusBusy = I2C_GetFlagStatus(s_I2C_Bus[me->iBus].i2c_bus, I2C_FLAG_BUSY);
            /* ${AOs::I2CMgr::SM::Active::Idle::I2C_READ_START::[Busbusy?]} */
            if (SET == me->bBusBusy) {
                /*Reset the retries counter when first getting into this state. */
                me->nBusRetries = 0;
                DBG_printf("Bus busy, waiting for free\n");
                status_ = Q_TRAN(&I2CMgr_WaitForBusFree);
            }
            /* ${AOs::I2CMgr::SM::Active::Idle::I2C_READ_START::[else]} */
            else {
                DBG_printf("Bus free\n");

                /* Reset the maximum number of times to poll the I2C bus for an event */
                me->nI2CLoopTimeout = MAX_I2C_TIMEOUT;
                status_ = Q_TRAN(&I2CMgr_WaitFor_I2C_EV5);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&I2CMgr_Active);
            break;
        }
    }
    return status_;
}


/**
 * @} end addtogroup groupI2C
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/