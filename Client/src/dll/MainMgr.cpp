/*****************************************************************************
* Model: MainMgr.qm
* File:  ./MainMgr_gen.cpp
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
/*${.::MainMgr_gen.cpp} ....................................................*/
/**
 * @file    MainMgr.cpp
 * Definitions for functions for the MainMgr AO.  This file is heavily
 * based on the code auto-generated using MainMgr.qm model.  Please update
 * the model to reflect any manual changes to the code.
 *
 * Note: If editing this file, please make sure to update the MainMgr.qm
 * model.  The generated code from that model should be very similar to the code
 * in this file.
 *
 * @date    03/20/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupClient
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "Shared.h"
#include "MainMgr.h"
#include "base64_wrapper.h"
#include "MsgUtils.h"
#include <cstring>
#include "bsp.h"
#include "dfuse.h"
#include "dfuse_err_convert.h"
#include "ClientApi.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE;

/* All timeouts should be defined in CBTimeouts.h as part of the
 * CBCommAPI that should be visible to the client/coupler */
#define EXIT(error_code)                              \
do {                                                  \
        ExitEvt *exEv = Q_NEW(ExitEvt, EXIT_SIG);     \
        exEv->my_error_code = error_code;             \
        QF_PUBLISH((QEvent *)exEv, AO_MainMgr);  \
   } while (0)

/* Private typedefs ----------------------------------------------------------*/
/**< Active object class */

/**
 * @brief Active Object (AO) "class" that manages the all the processing.
 *
 * This AO manages the all the communications and message transactions between the
 * client and the Coupler Board. See MainMgr.qm for diagram and model.
 */
/*${AOs::MainMgr} ..........................................................*/
typedef struct {
/* protected: */
    QActive super;
    QTimeEvt exitTimerEvt;
    uint8_t exit_code;
    ErrorCode errorCode;
} MainMgr;

/* protected: */
static QState MainMgr_initial(MainMgr * const me, QEvt const * const e);

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
static QState MainMgr_Active(MainMgr * const me, QEvt const * const e);
static QState MainMgr_CleanupBeforeExit(MainMgr * const me, QEvt const * const e);


/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static MainMgr l_MainMgr;            /* the single instance of the MainMgr AO */

/* Global-scope objects ------------------------------------------------------*/
QActive * const AO_MainMgr = (QActive *)&l_MainMgr;    /* "opaque" AO pointer */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief C "constructor" for MainMgr "class".
 * Initializes all the timers and queues used by the AO, sets up a deferral
 * queue, and sets of the first state.
 * @param [in]: none.
 * @retval: none
 */
/*${AOs::MainMgr_ctor} .....................................................*/
void MainMgr_ctor(void) {
    MainMgr *me = &l_MainMgr;
    QActive_ctor(&me->super, (QStateHandler)&MainMgr_initial);
    QTimeEvt_ctor(&me->exitTimerEvt, EXIT_SIG);
}

/**
 * @brief Active Object (AO) "class" that manages the all the processing.
 *
 * This AO manages the all the communications and message transactions between the
 * client and the Coupler Board. See MainMgr.qm for diagram and model.
 */
/*${AOs::MainMgr} ..........................................................*/
/*${AOs::MainMgr::SM} ......................................................*/
static QState MainMgr_initial(MainMgr * const me, QEvt const * const e) {
    /* ${AOs::MainMgr::SM::initial} */
    QS_OBJ_DICTIONARY(&l_MainMgr);
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&l_MainMgr_initial);
    QS_FUN_DICTIONARY(&l_MainMgr_Active);
    cout << "Started MainMgr AO" << endl;
    return Q_TRAN(&MainMgr_Active);
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
/*${AOs::MainMgr::SM::Active} ..............................................*/
static QState MainMgr_Active(MainMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::MainMgr::SM::Active} */
        case Q_ENTRY_SIG: {
            cout << "Entered Active state in MainMgr AO" << endl;
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::MainMgr::SM::Active::MSG_TIMEOUT_EXIT} */
        case MSG_TIMEOUT_EXIT_SIG: {
            cout << "Timed out while waiting for a response for a sent message.  Exiting." << endl;
            EXIT(1);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::MainMgr::SM::Active::EXIT} */
        case EXIT_SIG: {
            me->exit_code = (uint8_t)(((ExitEvt *)e)->my_error_code);
            status_ = Q_TRAN(&MainMgr_CleanupBeforeExit);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::MainMgr::SM::Active::CleanupBeforeExi~} ...........................*/
static QState MainMgr_CleanupBeforeExit(MainMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::MainMgr::SM::Active::CleanupBeforeExi~} */
        case Q_ENTRY_SIG: {
            /* Arm the first time for a long time.  */
            QTimeEvt_postIn(
                &me->exitTimerEvt,
                (QActive *)me,
                SECONDS_TO_BSP_TICKS( CLIENT_MAX_TIMEOUT_SEC_EXIT )
            );
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::MainMgr::SM::Active::CleanupBeforeExi~::EXIT} */
        case EXIT_SIG: {
            //exit( me->exit_code );
            QActive_stop(AO_CommStackMgr);
            QF_stop();
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&MainMgr_Active);
            break;
        }
    }
    return status_;
}


/**
 * @} end addtogroup groupClient
 */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
