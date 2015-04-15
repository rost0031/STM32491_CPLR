/**
 * @file    MainMgrDefs.h
 * Signal enums and priorities used by MainMgr AO.
 *
 * @date    04/13/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupClient
 * @{
 */
#ifndef MAINMGRDEFS_H_
#define MAINMGRDEFS_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @brief Priorities for AOs
 * These are the priorities of all the Shared Active Objects.  The priorities
 * are lowest at zero.
 */
enum AO_Priorities {
   NEVER_USE_ZERO_PRIORITY = 0,  // Never use this.  It breaks everything
   MAIN_MGR_PRIORITY,
   MAX_SHARED_PRIORITY  // This should always be at the end of this enum list since other modules will reference it
};

/**
 * @brief   Signals used by MainMgr AO
 */
enum MainMgrSignals {
   /* Q_USER_SIG is the first signal that user applications are allowed to use.
    * Should always be at the very top of the very first enum list
    */
   MSG_SEND_OUT_SIG = Q_USER_SIG,
   MSG_RECEIVED_SIG,
   MSG_TIMEOUT_EXIT_SIG,
   MSG_DFUSE_SEND_SIG,
   MSG_DFUSE_RECV_SIG,
   DFUSE_CONNECT_SIG,
   DFUSE_NACK_SIG,
   DFUSE_ACK_SIG,
   DFUSE_RESP_SIG,
   DFUSE_CMD_DONE_SIG,
   DFUSE_CMD_FAILED_SIG,
   DFUSE_CMD_TIMEOUT_SIG,
   TEST_JOB_SIG,
   TEST_JOB_DONE_SIG,
   EXIT_SIG,
   MAX_SHARED_SIG,
   MAX_PUB_SIG,
};

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @} end addtogroup groupClient
 */

#endif                                                      /* MAINMGRDEFS_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
