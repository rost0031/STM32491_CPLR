/**
 * @file 	Shared.h
 * @brief   Contains all the Shared Events, Priorities, and any other needed
 * 			declarations for the Shared state machines.  This is necessary to
 * 			make sure there is no overlap in enumerations between	the shared
 * 			code and the code pulling the shared code in.
 *
 * @date   	06/23/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SHARED_H_
#define SHARED_H_

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "CBErrors.h"                               /* for system error codes */
#include "CBSignals.h"                                  /* for system signals */
#include "CBCommApi.h"                                  /* for CB API support */
#include "sys_shared.h"
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * Active Object priorities.
 * These are the priorities of all the Active Objects in the system.  The
 * priorities are lowest at zero.
 * @note 1: Never use priority 0.
 */
enum AO_Priorities {
   NEVER_USE_ZERO_PRIORITY = 0,   /**< Never use this.  It breaks everything. */

   SYS_MGR_PRIORITY,                             /**< Priority of MenuMgr AO. */
   COMM_MGR_PRIORITY,                       /**< Priority of CommStackMgr AO. */

   I2C1DEVMGR_PRIORITY,                       /**< Priority of I2C1DevMgr AO. */
   SERIAL_MGR_PRIORITY,                        /**< Priority of SerialMgr AO. */


   CPLR_PRIORITY,                              /**< Priority of Coupler task. */



   I2CBUS1MGR_PRIORITY,                        /**< Priority of I2CBusMgr AO. */
   /* If more I2C busses are used, make sure that the priorities for them are
    * all together since there's a loop that iterates through them and will
    * take up the next priority if one is not added here.  You will end up with
    * clashing priorities for your AOs.*/

   ETH_PRIORITY,       /**< Priority of LWIP AO which handles ethernet comms. */
};

/**
 * @brief   Specify access type.
 * Some functions allow user to specify whether access is performed via event
 * driven (QP) or via direct access to HW using slow, blocking function calls.
 *
 * @note: Once the threads/Active Objects (AOs) have been started (end of main),
 * access should be limited to event driven interface.  In the event of a major
 * crash, and before all the threads/AOs have been started, access should be
 * the blocking kind.
 */
typedef enum AccessType {
   ACCESS_NONE       = 0,  /**< No access type.  Used as a default value  */
   ACCESS_BARE_METAL,      /**< Blocking access where no RTOS services are
                                running. All returns are done via passed in
                                buffer */
   ACCESS_QPC,             /**< Non-blocking access performed from QPC Active
                                Objects. All returns are done via events that
                                requesting AO should know how to handle */
   ACCESS_FREERTOS,        /**< Non-blocking access performed from a FreeRTOS
                                thread.  All returns are done via a QEQueue that
                                the requesting thread should wait on */
   ACCESS_MAX              /**< Max value in the type */
} AccessType_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#endif                                                           /* SHARED_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
