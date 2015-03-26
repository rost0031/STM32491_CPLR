/**
 * @file 	ClientTimeouts.h
 * Contains all the Shared Events, Priorities, and any other needed declarations
 * for the CB Clients. This is necessary to make sure there is no overlap in
 * enumerations between the shared code and the code pulling the shared code in.
 *
 * @date   	03/23/2015
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CLIENTTIMEOUTS_H_
#define CLIENTTIMEOUTS_H_

/* Includes ------------------------------------------------------------------*/
#include "CBCommApi.h"

/* Exported defines ----------------------------------------------------------*/
#define BSP_TICKS_PER_SEC   1000U

#ifdef WIN32COMPILE
#define WIN32_TIME_MULT       0.1
#else
#define WIN32_TIME_MULT       1
#endif

/**
 * Timeout naming conventions:
 * Timeouts should be named in the following fashion:
 *
 * 1. Which AO or functional block is using it (WHO)?
 * 2. Is it a maximum, minimum, ave, etc?
 * 3. Is it a timeout (TOUT) or just used to time something (TIME)?
 * 4. Units: seconds (SEC), minutes (MIN), or other.
 * 5. Name of the timeout and who/how/etc it's used.
 *
 * {WHO}_{MAX/MIN/AVE}_{TOUT/TIME}_{SEC/MIN/MS}_NAME_AND_USE
 *
 * Example:  HL_MAX_TOUT_SEC_QUICK_HEATER_CHECK
 *
 * HL: indicates that this is a high level timeout and is used by a top level
 * Active Object and is visible to the printer.
 * MAX: this is the maximum amount of time that is allowed for the action being
 * timed with this timeout.
 * TIMEOUT: this is a timeout and not simply used to time how long something
 * takes or a polling time.
 * SEC: this timeout is using units of "seconds"
 * NAME: QUICK_HEATER_CHECK - this is the function of this timeout.  It is used
 * to make sure that the check of temperature will not take too long.
 * */

/** \name Coupler Client Timeouts and Times
 * These are the timeouts used by all the state machines in the client. They are
 * based on the FW timeouts from CBTimeouts.h.
 *@{*/

   /** \name MainMgr AO Timeouts and Times.
    * Timeouts used by MainMgr AO.
    *@{*/
   #define MAINMGR_MAX_TIME_SEC_EXIT_DELAY                                    0.1
   /*@} MainMgr AO Timeouts and Times */

/*@} Coupler Board Timeout Maximums*/

/* Exported macros -----------------------------------------------------------*/
/**
 * Convert seconds to BSP ticks used by timers in QP.
 *
 * @param  seconds: number of seconds that is desired
 * @return ticks: number of ticks to actually arm the QP timer on the client.
 */
#define SEC_TO_TICKS( seconds )   \
   (uint32_t)( (1.0 * seconds) * BSP_TICKS_PER_SEC * WIN32_TIME_MULT)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif                                                   /* CLIENTTIMEOUTS_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
