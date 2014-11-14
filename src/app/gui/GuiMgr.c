/*****************************************************************************
* Model: GuiMgr.qm
* File:  ./GuiMgr_gen.c
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
/*${.::GuiMgr_gen.c} .......................................................*/
/**
 * @file    GuiMgr.c
 * Definitions for functions for the GuiMgr AO.  This state machine handles
 * all touchscreen, LCD, and GUI events and actions.
 *
 * Note: If editing this file, please make sure to update the GuiMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    11/06/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupGui
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "GuiMgr.h"
#include "project_includes.h"           /* Includes common to entire project. */
#include "GUI.h"
#include "WM.h"                                      /* emWin Windows Manager */
#include "DIALOG.h"
#include "I2CBusMgr.h"
#include "I2C1DevMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE;                 /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_GUI ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/

/**
 * \brief MenuMgr "class"
 */
/*${AOs::GuiMgr} ...........................................................*/
typedef struct {
/* protected: */
    QActive super;

    /**< QPC timer to periodically ask for position updates from the touch screen.
     * This timer is started upon receiving the GUI_TSC_INTERRUPT_SIG, only after
     * checking if the touch screen is still being touched. Once the position data
     * indicates that the touchscreen is no longer being touched, the timer is
     * disarmed until the next time an interrupt occurs. */
    QTimeEvt lcdTimerEvt;

    /* Holds the X position last read from the touch screen. */
    uint32_t _x;

    /* Holds the Y position last read from the touch screen. */
    uint32_t _y;

    /**< This variable keeps track of whether the touchscreen is currently pressed or
     * not.  If pressed, this flag is used to keep setting the timer to request LCD
     * position data from the touch screen controller. */
    bool tsPressed;
} GuiMgr;

/* protected: */
static QState GuiMgr_initial(GuiMgr * const me, QEvt const * const e);

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
static QState GuiMgr_Active(GuiMgr * const me, QEvt const * const e);


/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

static GuiMgr l_GuiMgr;           /* the single instance of the active object */

/* GUI handles */
static WM_HWIN      l_hDlg;
static WM_CALLBACK *l_cb_WM_HBKWIN;
static const GUI_WIDGET_CREATE_INFO l_dialog[] = {
    { &FRAMEWIN_CreateIndirect, "Main Window",
        0,  30,  30, 260, 180, FRAMEWIN_CF_MOVEABLE },
    { &TEXT_CreateIndirect, "Thing 1",
        GUI_ID_TEXT9,  50,  10, 0, 0, TEXT_CF_LEFT },
    { &TEXT_CreateIndirect, "Thing 2",
        GUI_ID_TEXT9,  50,  30, 0, 0, TEXT_CF_LEFT },

    { &TEXT_CreateIndirect, "status 1",
        GUI_ID_TEXT0, 130,  10, 0, 0, TEXT_CF_LEFT },
    { &TEXT_CreateIndirect, "status 2",
        GUI_ID_TEXT1, 130,  30, 0, 0, TEXT_CF_LEFT },

    { &BUTTON_CreateIndirect,"Button",
        GUI_ID_BUTTON0,    160, 130, 80, 30 }
};
/* Global-scope objects ------------------------------------------------------*/
QActive * const AO_GuiMgr = (QActive *)&l_GuiMgr;      /* "opaque" AO pointer */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*..........................................................................*/
static void onMainWndGUI(WM_MESSAGE* pMsg) {
    switch (pMsg->MsgId) {
        case WM_PAINT: {
            GUI_SetBkColor(GUI_GRAY);
            GUI_Clear();
            GUI_SetColor(GUI_BLACK);
            GUI_SetFont(&GUI_Font24_ASCII);
            GUI_DispStringHCenterAt("Window Manager Test", 160, 5);
            break;
        }
        default: {
            WM_DefaultProc(pMsg);
            break;
        }
    }
}

/*..........................................................................*/
static void onDialogGUI(WM_MESSAGE * pMsg) {
    switch (pMsg->MsgId) {
        case WM_INIT_DIALOG: {
            break;
        }
        case WM_NOTIFY_PARENT: {
            switch (pMsg->Data.v) {
                case WM_NOTIFICATION_RELEASED: {  /* react only if released */
                    switch (WM_GetId(pMsg->hWinSrc)) {
                        case GUI_ID_BUTTON0: {
                                     /* static PAUSE event for the Table AO */
                            static QEvent const pauseEvt = { GUI_TEST_SIG, 0 };
                            QACTIVE_POST(AO_GuiMgr, &pauseEvt, &l_onDialogGUI);
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        default: {
            WM_DefaultProc(pMsg);
            break;
        }
    }
}


/**
 * @brief C "constructor" for GuiMgr "class".
 * Initializes all the timers and queues used by the AO and sets of the
 * first state.
 * @param  None
 * @param  None
 * @retval None
 */
/*${AOs::GuiMgr_ctor} ......................................................*/
void GuiMgr_ctor(void) {
    GuiMgr *me = &l_GuiMgr;
    QActive_ctor(&me->super, (QStateHandler)&GuiMgr_initial);
    QTimeEvt_ctor( &me->lcdTimerEvt, GUI_LCD_POS_UPDATE_TIMER_SIG );
}

/**
 * \brief MenuMgr "class"
 */
/*${AOs::GuiMgr} ...........................................................*/
/*${AOs::GuiMgr::SM} .......................................................*/
static QState GuiMgr_initial(GuiMgr * const me, QEvt const * const e) {
    /* ${AOs::GuiMgr::SM::initial} */
    (void)e;        /* suppress the compiler warning about unused parameter */

    /* Init some variables to a sane state. */
    me->tsPressed = false;
    me->_x = 0;
    me->_y = 0;

    /* Activate the use of memory device feature */
    WM_SetCreateFlags(WM_CF_MEMDEV);

    /* Init the STemWin GUI Library */
    GUI_Init();

    /* Display a test message to verify LCD is working */
    //GUI_DispStringAt("Hello World!", 0, 0);

    QS_OBJ_DICTIONARY(&l_GuiMgr);
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&GuiMgr_initial);
    QS_FUN_DICTIONARY(&GuiMgr_Active);
    return Q_TRAN(&GuiMgr_Active);
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
/*${AOs::GuiMgr::SM::Active} ...............................................*/
static QState GuiMgr_Active(GuiMgr * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${AOs::GuiMgr::SM::Active} */
        case Q_ENTRY_SIG: {
            DBG_printf("GuiMgr successfully started\n");

            /* Post all the timers and disarm them right away so it can be
             * rearmed at any point without worrying asserts. */
            QTimeEvt_postIn(
                &me->lcdTimerEvt,
                (QActive *)me,
                SEC_TO_TICKS( LL_MAX_TIME_SEC_LCD_UPDATE )
            );
            QTimeEvt_disarm(&me->lcdTimerEvt);

            l_cb_WM_HBKWIN = WM_SetCallback(WM_HBKWIN, &onMainWndGUI);

                         /* create the diaglog box and return right away... */
            l_hDlg = GUI_CreateDialogBox(
                l_dialog, // Pointer to widget
                GUI_COUNTOF(l_dialog), // Number of widgets
                &onDialogGUI, //Callback to call on completion
                0,    // Parent
                0,    // X0 position
                0     // Y0 position
            );

            WM_Exec();                 /* update the screen and invoke WM callbacks */
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::GuiMgr::SM::Active::GUI_TSC_INTERRUPT} */
        case GUI_TSC_INTERRUPT_SIG: {
            DBG_printf("Got GUI_TSC_INTERRUPT\n");

            /* This flag is watched by the GUI_LCD_POS_DATA to see if we need to continue to
             * request TSC position data.  This interrupt signal kicks the process off.*/
            me->tsPressed = true;

            /* Direct post an event to ourself to request LCD position update data
             * from I2C1DevMgr AO. The reason for posting this event to this AO instead
             * of posting directly to the I2C1Dev AO is so all the timer logic can be
             * handled in the GUI_LCD_POS_DATA_SIG handler. */
            static QEvt const qEvt = { GUI_LCD_POS_UPDATE_TIMER_SIG, 0U, 0U };
            QACTIVE_POST(AO_GuiMgr, &qEvt, me);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::GuiMgr::SM::Active::GUI_LCD_POS_UPDATE_TIMER} */
        case GUI_LCD_POS_UPDATE_TIMER_SIG: {
            /* Direct post an event to request LCD position update data from I2C1DevMgr AO. */
            static QEvt const qEvt = { TSC_POS_READ_SIG, 0U, 0U };
            QACTIVE_POST(AO_I2C1DevMgr, &qEvt, me);
            status_ = Q_HANDLED();
            break;
        }
        /* ${AOs::GuiMgr::SM::Active::GUI_LCD_POS_DATA} */
        case GUI_LCD_POS_DATA_SIG: {
            /*
            char tmp[120];
            memset(tmp, 0, sizeof(tmp));
            uint16_t tmpLen = 0;
            CBErrorCode err = CON_hexToStr(
                (const uint8_t *)((I2CBusDataEvt const *)e)->dataBuf, // data to convert
                ((I2CBusDataEvt const *)e)->dataLen, // length of data to convert
                tmp,                                 // where to write output
                sizeof(tmp),                         // max size of output buffer
                &tmpLen,                             // size of the resulting output
                0,                                   // no columns
                ' '                                  // separator
            );
            if ( ERR_NONE != err ) {
                WRN_printf("Got an error converting hex array to string.  Error: 0x%08x\n", err);
            }
            DBG_printf("Read LCD POS array: %s\n", tmp);
            */

            /* Update the TSC position and notify the GUI/LCD system. */
            GUI_PID_STATE TS_State;
            uint32_t xDiff, yDiff , x , y;
            x = ((I2CBusDataEvt const *)e)->dataBuf[2] | ((I2CBusDataEvt const *)e)->dataBuf[1] << 8;
            y = ((I2CBusDataEvt const *)e)->dataBuf[4] | ((I2CBusDataEvt const *)e)->dataBuf[3] << 8;

            DBG_printf("X=%0x, Y=%0x\n", x, y);
            if(((x == 0xFFFF ) && (y == 0xFFFF ))|| ((x == 0 ) && (y == 0 ))) {
                me->tsPressed = false;
            }

            if ( me->tsPressed == true ) {
                x = ((I2CBusDataEvt const *)e)->dataBuf[2] | ((I2CBusDataEvt const *)e)->dataBuf[1] << 8;
                y = ((I2CBusDataEvt const *)e)->dataBuf[4] | ((I2CBusDataEvt const *)e)->dataBuf[3] << 8;
                xDiff = x > me->_x ? (x - me->_x) : (me->_x - x);
                yDiff = y > me->_y ? (y - me->_y) : (me->_y - y);
                if ((xDiff + yDiff > 2)) {
                    me->_x = x;
                    me->_y = y;
                }
                /* TSC is still being touched, rearm the timer to keep updating TSC
                 * position. */
                QTimeEvt_rearm(
                    &me->lcdTimerEvt,
                    SEC_TO_TICKS( LL_MAX_TIME_SEC_LCD_UPDATE )
                );
            } else {
                /* TSC not being touched.  Turn off the timer that triggers
                 * GUI_LCD_POS_UPDATE_TIMER signals which request TSC position data.*/
                QTimeEvt_disarm(&me->lcdTimerEvt);
            }

            TS_State.x = me->_x;
            TS_State.y = me->_y;
            TS_State.Pressed = me->tsPressed;
            TS_State.Layer = 0;

            if(
                (TS_State.x >= 0 ) &&
                (TS_State.x < LCD_GetXSize()) &&
                (TS_State.y >= 0 ) &&
                (TS_State.y < LCD_GetYSize())
            ) {
                GUI_TOUCH_StoreStateEx(&TS_State);
            }

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
 * @} end addtogroup groupGui
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/