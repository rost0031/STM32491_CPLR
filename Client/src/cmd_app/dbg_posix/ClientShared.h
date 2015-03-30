/**
 * @file 	ClientShared.h
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
#ifndef CLIENTSHARED_H_
#define CLIENTSHARED_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdlib>
#include <cstdio>
#include <cstring>                            /* for memcpy() and memset() */
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include "ClientTimeouts.h"
#include "CBSharedMsgTypes.h"

/* This is necessary since C++ code is pulling in a library compiled as C */
extern "C" {
#include "qp_port.h"
#include "CBCommApi.h"
}

/* Exported defines ----------------------------------------------------------*/
/* These two macros are handy for debugging and will be disabled if NDEBUG is
 * set. This is the regular debug print function which includes more info */
#ifdef NDEBUG
#define DEBUG 0
#else
#define DEBUG 1
#endif

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* These are the priorities of all the Shared Active Objects.  The priorities are lowest at zero. */
enum AO_Priorities {
	NEVER_USE_ZERO_PRIORITY	= 0,  // Never use this.  It breaks everything
	FWFLASHMGR_PRIORITY,
	MAIN_MGR_PRIORITY,
	MAX_SHARED_PRIORITY  // This should always be at the end of this enum list since other modules will reference it
};

enum MainMgrSignals {
	/* Q_USER_SIG is the first signal that user applications are allowed to use.
	 * Should always be at the very top of the very first enum list
	 */
   MSG_SEND_OUT_SIG = Q_USER_SIG,
	MSG_RECEIVED_SIG,
	MSG_TIMEOUT_EXIT_SIG,
   MSG_START_SENDING_SUPERSTATE_SIG,
   MSG_START_GETTING_SETTINGS_SIG,
   MSG_SEND_NEXT_FILE_SIG,
   MSG_DFUSE_SEND_SIG,
   MSG_DFUSE_RECV_SIG,
   DFUSE_CONNECT_SIG,
   DFUSE_NACK_SIG,
   DFUSE_ACK_SIG,
   DFUSE_RESP_SIG,
   DFUSE_CMD_DONE_SIG,
   DFUSE_CMD_FAILED_SIG,
   DFUSE_CMD_TIMEOUT_SIG,
	EXIT_SIG,
	START_PROCESSING_SIG,
   MAX_SHARED_SIG,
	MAX_PUB_SIG,
};

/* Exported constants --------------------------------------------------------*/
extern std::ofstream nullDev;

/* Exported functions --------------------------------------------------------*/

#endif                                                     /* CLIENTSHARED_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
