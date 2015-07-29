/**
 * @file     DC3DbgLevels.h
 *
 * Specifies the debug levels available to logging functions on the DC3.
 *
 * @date       03/24/2015
 * @author     Harry Rostovtsev
 * @email      harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DC3DBGLEVELS_H_
#define DC3DBGLEVELS_H_

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/*! \enum DBG_LEVEL_T
 * These are the various levels of debug that are available on the system.
 */
typedef enum DBG_LEVEL {
   DBG = 0, /**< Lowest level of debugging.  Everything printed. */
   LOG,     /**< Basic logging. */
   WRN,     /**< Warnings. Always printed. */
   ERR,     /**< Critical errors.  Always printed. */
   CON,     /**< This is reserved for printing to the console as part of
                 regular operation and nothing will be prepended.  Also, the
                 DBG_LEVEL will be checked to issue a MENU signal instead of a
                 LOG signal */
   ISR,     /**< Use this with isr_debug_slow_printf to get smaller printout */
} DC3DbgLevel_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif                                                     /* DC3DBGLEVELS_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
