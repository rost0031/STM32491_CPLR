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
#include "ClientShared.h"
#include "MainMgr.h"
#include "MainMgrDefs.h"
#include "cencode.h"
#include "cdecode.h"
#include "base64_wrapper.h"
#include "CBCommApi.h"
#include "LogHelper.h"
#include "CBSharedMsgTypes.h"
#include "qf_port.h"
#include "comm.h"

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
/* Private variables and Local objects ---------------------------------------*/
static QEvent const *l_MainMgrQueueSto[10];
static QSubscrList l_subscrSto[MAX_PUB_SIG];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void runMainMgr( void )
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
   QF_run();                              /* run the QF application */
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******************************************************************************/
void ClientApi::setNewConnection(
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort
)
{
   Comm* my_comm = new Comm(this->m_pLog, ipAddress, pRemPort, pLocPort);
   MainMgr_setConn(my_comm);
}

/******************************************************************************/
void ClientApi::setNewConnection(
      const char *dev_name,
      int baud_rate,
      bool bDFUSEComm
)
{
   Comm* my_comm = new Comm(this->m_pLog, dev_name, baud_rate, bDFUSEComm);
   MainMgr_setConn(my_comm);
}

/******************************************************************************/
void ClientApi::qfSetup( void )
{
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
}

/******************************************************************************/
void ClientApi::run( void )
{
   this->m_workerThread = boost::thread( runMainMgr );
   DBG_printf(this->m_pLog,"QF Framework started successfully.");
}

/******************************************************************************/
void ClientApi::waitForDone( void )
{
   DBG_printf(this->m_pLog,"Waiting for MainMgr to be finished.");
   this->m_workerThread.join();
   DBG_printf(this->m_pLog,"Thread finished. Returning\n");
}

/******************************************************************************/
void ClientApi::setLogging( LogStub *log )
{
   this->m_pLog = log;
   DBG_printf(this->m_pLog,"Logging setup successful.");
}

/******************************************************************************/
void ClientApi::stop( void )
{
   DBG_printf(this->m_pLog,"Stopping client.");
   QEvt* evt = Q_NEW(QEvt, EXIT_SIG);
   QF_PUBLISH((QEvt *)evt, AO_MainMgr);

}

/******************************************************************************/
ClientApi::ClientApi(
      LogStub *log,
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort
) :  m_pLog(NULL)
{
   this->setLogging(log);
   this->qfSetup();                               /* Initialize QF framework. */
   this->setNewConnection(ipAddress, pRemPort, pLocPort);
}

/******************************************************************************/
ClientApi::ClientApi(
         LogStub *log,
         const char *dev_name,
         int baud_rate,
         bool bDFUSEComm
) :  m_pLog(NULL)
{
   this->setLogging(log);
   this->qfSetup();                               /* Initialize QF framework. */
   this->setNewConnection(dev_name, baud_rate, bDFUSEComm);
}

/******************************************************************************/
ClientApi::ClientApi( LogStub *log ) :
      m_pLog(NULL)
{

   this->setLogging(log);                                      /* Set logging */
   this->qfSetup();                               /* Initialize QF framework. */

   /* This works */
//   DBG_printf(this->m_pLog,"Attempting to init serial...");
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
//   DBG_printf(this->m_pLog,"Attempting to write serial...");
//
//   serial->write_some("Test\n", 5);

   /* This also works.*/
//   DBG_printf(this->m_pLog,"Attempting to init UDP eth...");
//   Eth *eth;
//   try {
//      eth = new Eth(
//            "172.27.0.75",
//            "1502",
//            "53432"
//      );
//   } catch ( ... ) {
//      ERR_printf("Unable to connect to UDP. Exiting");
//      EXIT_LOG_FLUSH(1); // Do a regular exit since this is critical
//   }
//   DBG_printf(this->m_pLog,"Attempting to write UDP eth...");
//   eth->write_some("Test\n", 5);

   /* This also works.*/
   DBG_printf(this->m_pLog,"Attempting to init UDP eth...");
   Comm *m_comm;
   try {
      m_comm = new Comm(
            this->m_pLog,
            "172.27.0.75",
            "1502",
            "53432"
      );
   } catch ( ... ) {
      ERR_printf(this->m_pLog,"Unable to connect to UDP. Exiting");
      return; // Do a regular exit since this is critical
   }
   DBG_printf(this->m_pLog,"Attempting to write UDP eth...");
   m_comm->write_some("Test\n", 5);

}

/******************************************************************************/
ClientApi::~ClientApi(  )
{
   delete[] this->m_pLog;
}


