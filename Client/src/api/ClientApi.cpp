/**
 * @file    ClientApi.cpp
 * Definitions for class and functions used by the Client API.
 *
 * @date    02/04/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */

#include "ClientApi.h"
#include "CmdlineParser.h"
#include "serial.h"
#include "eth.h"
#include "Shared.h"
#include "CommStackMgr.h"
#include "cencode.h"
#include "cdecode.h"
#include "base64_wrapper.h"

#include "MsgUtils.h"

#ifndef RFID_BOARD_API_H_
#define RFID_BOARD_API_H_
#include "rfid_board_api.h"
#endif

#include "Redwood_API.h"

/* Global pointers to device objects and cmdline parser */
Serial          *serial;
Eth             *ethernet;
CmdlineParser   *cmdline_parser;

/* Local-scope objects -----------------------------------------------------*/
static QEvent const *l_MainMgrQueueSto[10];
static QSubscrList l_subscrSto[MAX_PUB_SIG];

/* storage for event pools... */
static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvent)];
    uint8_t e2[sizeof(ExitEvt)];
} l_smlPoolSto[10];                     /* storage for the small event pool */

static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t *e1[sizeof(MsgEvt)];
    uint8_t *e2[sizeof(DfuseMsgEvt)];
} l_medPoolSto[15];                    /* storage for the medium event pool */


/******************************************************************************/
void ClientApi::run( void )
{
   /* instantiate all active objects */
   MainMgr_ctor();

   /* initialize the framework and the underlying RT kernel */
   QF_init();

   QS_OBJ_DICTIONARY(l_smlPoolSto);
   QS_OBJ_DICTIONARY(l_medPoolSto);
   QS_OBJ_DICTIONARY(l_CommStackMgrQueueSto);

   /* init publish-subscribe */
   QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));

   /* initialize event pools... */
   QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
   QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

   /* start the active objects... */
   QActive_start(  AO_MainMgr, COMM_STACK_PRIORITY,
         l_MainMgrQueueSto, Q_DIM(l_MainMgrQueueSto),
         (void *)0, 0, (QEvent *)0);

   printf("Started QF Framework.  Client Running...\n");
   QF_run();                              /* run the QF application */
   printf("After exit is called\n");
}

/******************************************************************************/
ClientApi::ClientApi( int argc, char** argv )
{
   /* Parse command line arguments and make them globally accessible */
   cmdline_parser = new CmdlineParser(argc, argv);

}

/******************************************************************************/
ClientApi::~ClientApi(  )
{
   delete[] cmdline_parser;
   delete[] serial;
}

/******************************************************************************/
const void ClientApi::setRedwoodMsgCallBack(
      cbRedwoodMsgHandler pCallbackFunction
)
{
   cmdline_parser->setRedwoodMsgCallBack( pCallbackFunction );
}

/******************************************************************************/
const void ClientApi::setRedwoodAckCallBack(
      cbRedwoodMsgHandler pCallbackFunction
)
{
   cmdline_parser->setRedwoodAckCallBack( pCallbackFunction );
}

/******************************************************************************/
const void ClientApi::setInternalLogCallBack(
      cbRedwoodLogHandler pCallbackFunction
)
{
   cmdline_parser->setInternalLogCallBack( pCallbackFunction );
}
