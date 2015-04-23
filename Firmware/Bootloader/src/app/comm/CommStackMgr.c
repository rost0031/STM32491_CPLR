/*****************************************************************************
* Model: CommStackMgr.qm
* File:  ./CommStackMgr_gen.c
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
/*${.::CommStackMgr_gen~} ..................................................*/
/**
 * @file    CommStackMgr.c
 * Declarations for functions for the CommStackMgr AO.  This state
 * machine handles all incoming messages and their distribution for the
 * bootloader.
 *
 * Note: If editing this file, please make sure to update the CommStackMgr.qm
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

/* Includes ------------------------------------------------------------------*/
#include "CommStackMgr.h"
#include "project_includes.h"           /* Includes common to entire project. */
#include "bsp.h"                              /* For time to ticks conversion */
#include "I2C1DevMgr.h"                                  /* For I2C Evt types */
#include "time.h"
#include "stm32f4x7_eth.h"
#include "serial.h"                               /* For serial functionality */
#include "LWIPMgr.h"                        /* For UDP ethernet functionality */
#include "base64_wrapper.h"                            /* For base64 encoding */
#include "SerialMgr.h"
#include "LWIPMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE;                 /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_COMM );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
typedef void (*pFunction)(void);


/**
 * \brief CommStackMgr "class"
 */
/*${AOs::CommStackMgr} .....................................................*/
typedef struct {
/* protected: */
    QActive super;

    /**< Local timer for timing out msg processing. */
    QTimeEvt msgTimerEvt;

    /**< Local buffer to store incoming msgs */
    uint8_t* dataBuf[CB_MAX_MSG_LEN];

    /**< Common struct for CBBasicMsg.  This can be re-used in most cases since once the
     * request has been unwrapped, all the data from that struct has been stored locally */
    struct CBBasicMsg basicMsg;

    /**< Keeps track of how far into the dataBuf basicMsg was.  Sometimes, there are
     * additional payload msgs after the BasicMsg in the buffer and this variable allows
     * the next deserialization step to know where to find them */
    uint16_t basicMsgOffset;

    /**< Keeps track of where the message came from */
    CBMsgRoute msgRoute;

    /**< Keep track of the ID of the msg */
    uint32_t msgId;

    /**< Keep track of whether Prog responses were requested by the sender of the msg */
    bool msgReqProg;

    /**< Keep track of the CLI_RECEIVED_SIG event source in order to send it back the
     * same comm channel */
    CBMsgRoute cliEvtSrc;

    /**< Keep track of the CLI_RECEIVED_SIG event destination.
     * TODO: This isn't currently used but hold on to it just in case. Figure out if
     * this is even needed.*/
    CBMsgRoute cliEvtDst;

    /**< Keep track of errors that may occur in the AO */
    CBErrorCode errorCode;

    /**< Keep track of what the payload msg is supposed to be upon reception.  This is
     * necessary because basicMsg struct gets modified to send an Ack and this variable
     * will be used to extract the rest of the payload from the data buffer where it's
     * still being stored. */
    CBMsgName msgPayloadName;

    /**< Union of all the possible payload msgs.  This gets populated by the recieved msg
     * processing and later reused to send Prog and Done msgs. */
    CBPayloadMsgUnion_t payloadMsgUnion;
} CommStackMgr;

/* protected: */
static QState CommStackMgr_initial(CommStackMgr * const me, QEvt const * const e);

/**
 * This state is a catch-all Active state.  If any signals need
 * to be handled that do not cause state transitions and are
 * common to the entire AO, they should be handled here.
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState CommStackMgr_Active(CommStackMgr * const me, QEvt const * const e);

/**
 * @brief	Idle state that allows new messages to be received.
 * The Idle state allows for handling of new incoming msgs. Msgs in other states
 * will be either deferred or ignored.
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState CommStackMgr_Idle(CommStackMgr * const me, QEvt const * const e);

/**
 * @brief	Busy state for msg processing.
 * This state handles msg processing and indicates that the system is busy and
 * cannot process another msg at this time.
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState CommStackMgr_BusyWithMsg(CommStackMgr * const me, QEvt const * const e);

/**
 * @brief	State that ensures the system stays in the Busy state while processing.
 * This state just self-posts an event to start the msg processing. Its primary
 * reason for existance is to make sure that the state machine ends up inside the
 * busy state for proper error handling (if any occur).
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
static QState CommStackMgr_ValidateMsg(CommStackMgr * const me, QEvt const * const e);


/* Private defines -----------------------------------------------------------*/
#define LWIP_ALLOWED
#define USER_FLASH_FIRST_PAGE_ADDRESS                              0x08020000
#define BOOT_APP_TIMEOUT                                           30000

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;

static CommStackMgr l_CommStackMgr; /* the single instance of the Interstage active object */

/* Global-scope objects ----------------------------------------------------*/
QActive * const AO_CommStackMgr = (QActive *)&l_CommStackMgr;  /* "opaque" AO pointer */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief C "constructor" for CommStackMgr "class".
 * Initializes all the timers and queues used by the AO and sets of the
 * first state.
 * @param  None
 * @param  None
 * @retval None
 */
/*${AOs::CommStackMgr_cto~} ................................................*/
void CommStackMgr_ctor(void) {
    CommStackMgr *me = &l_CommStackMgr;
    QActive_ctor(&me->super, (QStateHandler)&CommStackMgr_initial);
    QTimeEvt_ctor(&me->msgTimerEvt, CLI_MSG_TIMEOUT_SIG);
}

/**
 * @brief	Wrapper around serial and UDP send data function.
 * This function acts as a simple wrapper around serial and UDP ethernet send data
 * functions to allow easy posting of events to send out data over those interfaces.
 * @param [in] *evt: LrgDataEvt pointer to event to send.
 * @return: CBErrorCode indicating status of operation.
 */
/*${AOs::Comm_sendToClien~} ................................................*/
CBErrorCode Comm_sendToClient(LrgDataEvt* evt) {
    CBErrorCode status = ERR_NONE;
    if ( _CB_Serial == evt->dst ) {
        uint8_t encDataBuf[CB_MAX_MSG_LEN];
        memset(encDataBuf, 0, sizeof(encDataBuf));
        uint16_t encDataLen = base64_encode(
              (char *)(evt->dataBuf),
              evt->dataLen,
              (char *)encDataBuf,
              CB_MAX_MSG_LEN
        );

        if(encDataLen < 1) {
            status = ERR_SERIAL_MSG_BASE64_ENC_FAILED;
            ERR_printf(
                "Base64 encoding ran out of space, error: 0x%08x\n",
                status
            );
            /* Set so we don't error out copying.  Client should know how to
             * to properly handle this */
            encDataLen = 1;
        }

        /* Modify the contents of the event with the new encoded data and send
         * it regardless of whether we*/
        evt->dataLen = encDataLen;
        MEMCPY( evt->dataBuf, encDataBuf, evt->dataLen);
        evt->dst = _CB_Serial;
        evt->src = _CB_Serial;

        /* Directly post to the LWIPMgr AO. */
        QACTIVE_POST( AO_SerialMgr, (QEvt *)(evt), 0 );

    } else { /* Always attempt to send over UDP to prevent leaking the event */
        /* No modifications needed to the UDP send so just post it directly to that AO.*/
        /* Directly post to the LWIPMgr AO. */
        QACTIVE_POST( AO_LWIPMgr, (QEvt *)(evt), 0 );
    }

    /* If we got here, src or dst was not set up properly. */
    return status;
}

/**
 * \brief CommStackMgr "class"
 */
/*${AOs::CommStackMgr} .....................................................*/
/*${AOs::CommStackMgr::SM} .................................................*/
static QState CommStackMgr_initial(CommStackMgr * const me, QEvt const * const e) {
    /* ${AOs::CommStackMgr::SM::initial} */
    (void)e;        /* suppress the compiler warning about unused parameter */

    QS_OBJ_DICTIONARY(&l_CommStackMgr);
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&CommStackMgr_initial);
    QS_FUN_DICTIONARY(&CommStackMgr_Active);

    QActive_subscribe((QActive *)me, SER_RECEIVED_SIG);
    return Q_TRAN(&CommStackMgr_Idle);
}

/**
 * This state is a catch-all Active state.  If any signals need
 * to be handled that do not cause state transitions and are
 * common to the entire AO, they should be handled here.
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::CommStackMgr::SM::Active} .........................................*/
static QState CommStackMgr_Active(CommStackMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::CommStackMgr::SM::Active} */
        case Q_ENTRY_SIG: {
            /* Arm and disarm*/
            QTimeEvt_postIn(
                &me->msgTimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( LL_MAX_TOUT_SEC_CLI_MSG_PROCESS )
            );
            QTimeEvt_disarm(&me->msgTimerEvt);
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
 * @brief	Idle state that allows new messages to be received.
 * The Idle state allows for handling of new incoming msgs. Msgs in other states
 * will be either deferred or ignored.
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::CommStackMgr::SM::Active::Idle} ...................................*/
static QState CommStackMgr_Idle(CommStackMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::CommStackMgr::SM::Active::Idle} */
        case Q_ENTRY_SIG: {
            /* Clear out all the variables on entry to Idle state.  By this point, any error
             * feedback should have already happened. */
            me->errorCode       = ERR_NONE;
            me->msgId           = 0;
            me->cliEvtSrc       = _CB_NoRoute;
            me->cliEvtDst       = _CB_NoRoute;
            me->msgRoute        = _CB_NoRoute;
            me->msgPayloadName  = _CBNoMsg;
            me->msgReqProg      = false;

            memset(&me->basicMsg, 0, sizeof(me->basicMsg));
            memset(&me->payloadMsgUnion, 0, sizeof(me->payloadMsgUnion));
            memset(me->dataBuf, 0, sizeof(me->dataBuf));
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::CommStackMgr::SM::Active::Idle::SER_RECEIVED} */
        case SER_RECEIVED_SIG: {
            DBG_printf("SER_RECIEVED\n");

            LrgDataEvt *cliEvt = Q_NEW(LrgDataEvt, CLI_RECEIVED_SIG);
            cliEvt->dataLen = base64_decode(
                (char *)((LrgDataEvt const *) e)->dataBuf,
                ((LrgDataEvt const *) e)->dataLen,
                (char *)cliEvt->dataBuf,
                CB_MAX_MSG_LEN
            );

            cliEvt->src = ((LrgDataEvt const *) e)->src;
            cliEvt->dst = ((LrgDataEvt const *) e)->dst;

            QACTIVE_POST(
                AO_CommStackMgr,
                (QEvt *)(cliEvt),
                AO_CommStackMgr
            );
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::CommStackMgr::SM::Active::Idle::CLI_RECEIVED} */
        case CLI_RECEIVED_SIG: {
            DBG_printf("CLI_RECIEVED\n");

            /* Extract the basicMsg first */
            memset(&(me->basicMsg), 0, sizeof(me->basicMsg));
            me->basicMsgOffset = CBBasicMsg_read_delimited_from(
                (void*)((LrgDataEvt const *) e)->dataBuf,
                &(me->basicMsg),
                0
            );

            /* Store the event source and destination before entering a new state where that
             * information will be lost. */
            me->cliEvtSrc = ((LrgDataEvt const *) e)->src;
            me->cliEvtDst = ((LrgDataEvt const *) e)->dst;

            /* Store the basic msg elements locally since they are needed to send back all the
             * ack, prog, and done replies. */
            me->msgRoute   = me->basicMsg._msgRoute;
            me->msgId      = me->basicMsg._msgID;
            me->msgReqProg = (bool)(me->basicMsg._msgReqProg);
            me->msgPayloadName = me->basicMsg._msgPayload; // save this since Ack will overwrite it

            /* Extract the payload (if exists) since this buffer is going away the moment we get
             * into a state.  We'll figure out later if it's valid, right before we send an Ack */
            switch( me->msgPayloadName ) {
                case _CBNoMsg:
                    DBG_printf("No payload detected\n");
                    break;
                case _CBStatusPayloadMsg:
                    WRN_printf("Status payload detected, this is probably an error\n");
                    break;
                case _CBVersionPayloadMsg:
                    WRN_printf("Version payload detected, this is probably an error\n");
                    break;
                case _CBBootModePayloadMsg:
                    DBG_printf("BootMode payload detected\n");
                    break;
                default:
                    WRN_printf(
                        "Unknown payload detected: %d, this is probably an error\n",
                        me->msgPayloadName
                    );
                    break;
            }
            status_ = Q_TRAN(&CommStackMgr_ValidateMsg);
            break;
        }
        default: {
            status_ = Q_SUPER(&CommStackMgr_Active);
            break;
        }
    }
    return status_;
}

/**
 * @brief	Busy state for msg processing.
 * This state handles msg processing and indicates that the system is busy and
 * cannot process another msg at this time.
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::CommStackMgr::SM::Active::BusyWithMsg} ............................*/
static QState CommStackMgr_BusyWithMsg(CommStackMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::CommStackMgr::SM::Active::BusyWithMsg} */
        case Q_ENTRY_SIG: {
            /* Arm the timer so if the message can't be processed for some reason, we can get
             * back to idle state.  This timer may be re-armed if some messages require more
             * time to process than others. */
            QTimeEvt_rearm(
                &me->msgTimerEvt,
                SEC_TO_TICKS( LL_MAX_TOUT_SEC_CLI_MSG_PROCESS )
            );

            /* Compose and send Ack response.  We can re-use the current structure since only
             * one non-essential field changes. */
            me->basicMsg._msgType    = _CB_Ack;
            me->basicMsg._msgPayload = _CBNoMsg; // This was saved earlier
            /* The rest of the fields are the same */

            /* Serialize the msg into a buffer of a fresh event. This event will be passed to
             * a function which will post it directly to the correct AO. */
            LrgDataEvt *evt = Q_NEW(LrgDataEvt, CLI_SEND_DATA_SIG);
            /* The src and dst are swapped on purpose since we have to tell the message to go
             * to where it originally came from. */
            evt->dst = me->cliEvtSrc;
            evt->src = me->cliEvtDst;
            evt->dataLen = CBBasicMsg_write_delimited_to(&(me->basicMsg), evt->dataBuf, 0);
            me->errorCode = Comm_sendToClient( evt );
            if ( ERR_NONE != me->errorCode ) {
                WRN_printf("Possible error sending Ack, attempting to continue. Error: 0x%08x\n", me->errorCode);
            }
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::CommStackMgr::SM::Active::BusyWithMsg} */
        case Q_EXIT_SIG: {
            /* Set the fields of the msg common for all Done msgs regardless of status or msg */
            me->basicMsg._msgType    = _CB_Done;
            me->basicMsg._msgRoute   = me->msgRoute;
            me->basicMsg._msgID      = me->msgId;
            me->basicMsg._msgReqProg = (unsigned long)me->msgReqProg;

            /* The command specific fields should have already been set up by their respective
             * exit transitions */
            LrgDataEvt *evt = Q_NEW(LrgDataEvt, CLI_SEND_DATA_SIG);
            evt->dataLen = CBBasicMsg_write_delimited_to(&me->basicMsg, evt->dataBuf, 0);
            /* The src and dst are swapped on purpose since we have to tell the message to go
             * to where it originally came from. */
            evt->dst = me->cliEvtSrc;
            evt->src = me->cliEvtDst;
            evt->dataLen = CBBasicMsg_write_delimited_to(&(me->basicMsg), evt->dataBuf, 0);

            /* Append payload msg if needed */
            switch( me->msgPayloadName ) {
                case _CBStatusPayloadMsg:
                    DBG_printf("Sending status payload\n");
                    evt->dataLen = CBStatusPayloadMsg_write_delimited_to(
                        (void*)&(me->payloadMsgUnion.statusPayload),
                        evt->dataBuf,
                        evt->dataLen
                    );
                    break;
                case _CBVersionPayloadMsg:
                    DBG_printf("Sending version payload\n");
                    evt->dataLen = CBVersionPayloadMsg_write_delimited_to(
                        (void*)&(me->payloadMsgUnion.versionPayload),
                        evt->dataBuf,
                        evt->dataLen
                    );
                    break;
                case _CBBootModePayloadMsg:
                    DBG_printf("Sending bootMode payload with bootmode: %d\n", me->payloadMsgUnion.runmodePayload._bootMode);
                    evt->dataLen = CBRunModePayloadMsg_write_delimited_to(
                        (void*)&(me->payloadMsgUnion.runmodePayload),
                        evt->dataBuf,
                        evt->dataLen
                    );
                    break;
                case _CBNoMsg:
                    WRN_printf("Not sending payload as part of Done msg.\n");
                    break;
                default:
                    WRN_printf("Unknown payload detected %d, this is probably an error\n",
                        me->msgPayloadName
                    );
                    break;
            }

            /* Send the Done msg buffer to the client */
            me->errorCode = Comm_sendToClient( evt );
            if ( ERR_NONE != me->errorCode ) {
                WRN_printf("Possible error sending Done, attempting to continue. Error: 0x%08x\n", me->errorCode);
            }

            /* Disarm timer on exit */
            QTimeEvt_disarm(&me->msgTimerEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::CommStackMgr::SM::Active::BusyWithMsg::CLI_MSG_TIMEOUT} */
        case CLI_MSG_TIMEOUT_SIG: {
            ERR_printf("Timeout trying to process %d basic msg, error: 0x%08x\n", me->errorCode);
            status_ = Q_TRAN(&CommStackMgr_Idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&CommStackMgr_Active);
            break;
        }
    }
    return status_;
}

/**
 * @brief	State that ensures the system stays in the Busy state while processing.
 * This state just self-posts an event to start the msg processing. Its primary
 * reason for existance is to make sure that the state machine ends up inside the
 * busy state for proper error handling (if any occur).
 *
 * @param  [in|out] me: Pointer to the state machine
 * @param  [in|out]  e:  Pointer to the event being processed.
 * @return status: QState type that specifies where the state
 * machine is going next.
 */
/*${AOs::CommStackMgr::SM::Active::BusyWithMsg::ValidateMsg} ...............*/
static QState CommStackMgr_ValidateMsg(CommStackMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::CommStackMgr::SM::Active::BusyWithMsg::ValidateMsg} */
        case Q_ENTRY_SIG: {
            /* Post to self to leave the state and continue processing the rest of the msg */
            QEvt *evt = Q_NEW(QEvt, MSG_PROCESS_SIG);
            QACTIVE_POST(AO_CommStackMgr, (QEvt *)(evt), AO_CommStackMgr);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::CommStackMgr::SM::Active::BusyWithMsg::ValidateMsg::MSG_PROCESS} */
        case MSG_PROCESS_SIG: {
            DBG_printf("MSG_PROCESS\n");
            /* ${AOs::CommStackMgr::SM::Active::BusyWithMsg::ValidateMsg::MSG_PROCESS::[GetBootMode?]} */
            if (_CBGetBootModeMsg == me->basicMsg._msgName) {
                me->errorCode = ERR_NONE;

                DBG_printf("_CBGetBootModeMsg decoded, attempting to decode payload (if exists)\n");

                /* Compose Done response.  We can re-use the current structure and it will be used by
                 * the exit action of the parent state to send the msg.  Here, we only set up fields
                 * that are specific to this response. We can also destructively change the payload
                 * name since we are sending a response right after this. */
                me->msgPayloadName = _CBBootModePayloadMsg;

                /* Don't change the basicMsg name since it should be the same in all cases. */
                me->basicMsg._msgPayload = me->msgPayloadName;
                me->payloadMsgUnion.runmodePayload._errorCode = me->errorCode;
                me->payloadMsgUnion.runmodePayload._bootMode  = _CB_Bootloader; // This is the bootloader
                DBG_printf("Setting bootMode payload with bootmode: %d\n", me->payloadMsgUnion.runmodePayload._bootMode);
                status_ = Q_TRAN(&CommStackMgr_Idle);
            }
            /* ${AOs::CommStackMgr::SM::Active::BusyWithMsg::ValidateMsg::MSG_PROCESS::[else]} */
            else {
                me->errorCode = ERR_MSG_UNKNOWN_BASIC;
                ERR_printf(
                    "Unknown msg (%d) with msgId=%d. Sending back error: 0x%08x\n",
                    me->basicMsg._msgName, me->msgId, me->errorCode);

                /* Compose Done response.  We can re-use the current structure and it will be used by
                 * the exit action of the parent state to send the msg.  Here, we only set up fields
                 * that are specific to this response. We can also destructively change the payload
                 * name since we are sending a response right after this. */
                me->msgPayloadName = _CBStatusPayloadMsg;

                /* Don't change the basicMsg name since we don't actually know what it is but add a
                 * status payload msg to let the user know the error. */
                me->basicMsg._msgPayload = me->msgPayloadName;
                me->payloadMsgUnion.statusPayload._errorCode = me->errorCode;



                status_ = Q_TRAN(&CommStackMgr_Idle);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&CommStackMgr_BusyWithMsg);
            break;
        }
    }
    return status_;
}


/**
 * @} end addtogroup groupComm
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
