/**
 * @file    ClientApi.cpp
 * Definitions for class and functions used by the Client API.
 *
 * @date    03/25/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "ClientApi.h"
#include "Job.h"
#include "serial.h"
#include "eth.h"
#include "ClientShared.h"
#include "MainMgr.h"
#include "cencode.h"
#include "cdecode.h"
#include "base64_wrapper.h"
#include "CBCommApi.h"
#include "LogHelper.h"
#include "CBSharedMsgTypes.h"
#include "qf_port.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_API );

/* Private typedefs ----------------------------------------------------------*/
/* storage for event pools... */
static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvent)];
    uint8_t e2[sizeof(ExitEvt)];
} l_smlPoolSto[10];                     /* storage for the small event pool */

static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t *e1[sizeof(LrgDataEvt)];
} l_medPoolSto[15];                    /* storage for the medium event pool */

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/* Global pointers to device objects and cmdline parser */
Serial          *serial;
Eth             *ethernet;

/* Private variables and Local objects ---------------------------------------*/
static QEvent const *l_MainMgrQueueSto[10];
static QSubscrList l_subscrSto[MAX_PUB_SIG];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
const void ClientApi::setMsgCallBack(
      CB_MsgHandler_t pCallbackFunction
)
{
   this->setMsgCallBack( pCallbackFunction );
}

/******************************************************************************/
const void ClientApi::setAckCallBack(
      CB_MsgHandler_t pCallbackFunction
)
{
   this->setAckCallBack( pCallbackFunction );
}

/******************************************************************************/
const void ClientApi::setInternalLogCallBack(
      CB_LogHandler_t pCallbackFunction
)
{
   this->setInternalLogCallBack( pCallbackFunction );
}


/******************************************************************************/
void ClientApi::run( void )
{
   /* start the active objects... */
   QActive_start(
         AO_MainMgr,
         MAIN_MGR_PRIORITY,
         l_MainMgrQueueSto,
         Q_DIM(l_MainMgrQueueSto),
         (void *)0,
         0,
         (QEvent *)0,
         "MainMgr"
   );

   printf("Started QF Framework.  Client Running...\n");
   QF_run();                              /* run the QF application */
   printf("After exit is called\n");
}

/******************************************************************************/
ClientApi::ClientApi( LogStub *log ) :
      m_pLog(NULL)
{

   this->m_pLog = log;
   DBG_printf(this->m_pLog,"Logging setup successful\n");

   /* instantiate all active objects */
   MainMgr_ctor( this->m_pLog );

   /* initialize the framework and the underlying RT kernel */
   QF_init();

   QS_OBJ_DICTIONARY(l_smlPoolSto);
   QS_OBJ_DICTIONARY(l_medPoolSto);
   QS_OBJ_DICTIONARY(l_MainMgrQueueSto);

   /* init publish-subscribe */
   QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));

   /* initialize event pools... */
   QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
   QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

   /* This works */
//   DBG_printf(this->m_pLog,"Attempting to init serial...\n");
//   Serial *serial;
//   try {
//      serial = new Serial(
//            "COM32",
//            115200,
//            false
//      );
//
//   } catch (...) {
//      cout << "ERROR: Unable to open serial port "
//            << "COM32" << endl;
//      exit(1); // Do a regular exit since this is critical
//   }
//   DBG_printf(this->m_pLog,"Attempting to write serial...\n");
//
//   serial->write_some("Test\n", 5);

   /* This also works.*/
   DBG_printf(this->m_pLog,"Attempting to init UDP eth...\n");
   Eth *eth;
   try {
      eth = new Eth(
            "172.27.0.75",
            "1502",
            "53432"
      );
   } catch ( ... ) {
      cout << "ERROR: Unable to connect to UDP " << endl;
      exit(1); // Do a regular exit since this is critical
   }
   DBG_printf(this->m_pLog,"Attempting to write UDP eth...\n");
   eth->write_some("Test\n", 5);
}

/******************************************************************************/
ClientApi::~ClientApi(  )
{
   delete[] this->m_pLog;
   delete[] serial;
}

