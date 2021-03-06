/**
 * @file 	main.c
 * @brief   Main for starting up the QPC RTOS.
 *
 * @date   	06/11/2014
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "FreeRTOS.h"                                 /* for FreeRTOS support */

#include "LWIPMgr.h"                               /* for starting LWIPMgr AO */
#include "CommMgr.h"                               /* for starting CommMgr AO */
#include "SerialMgr.h"                           /* for starting SerialMgr AO */
#include "I2CBusMgr.h"                           /* for starting I2CBusMgr AO */
#include "I2C1DevMgr.h"                         /* for starting I2C1DevMgr AO */
#include "cplr.h"                               /* for starting the CPLR task */
#include "SysMgr.h"                                 /* for starting SysMgr AO */

#include "project_includes.h"           /* Includes common to entire project. */
#include "Shared.h"
#include "version.h"

#include "stm32f4xx.h"
#include <stdio.h>
#include "bsp_defs.h"
#include "bsp.h"
#include "db.h"                                       /* for settings support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_GEN ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define THREAD_STACK_SIZE  1024U * 2
#define QM_MEMPOOL_SIZE   16384 /**< Large memory pool since it's going into CCMRAM */

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static QEvt const    *l_CommMgrQueueSto[30];        /**< Storage for CommStackMgr event Queue */
static QEvt const    *l_LWIPMgrQueueSto[200];       /**< Storage for LWIPMgr event Queue */
static QEvt const    *l_SerialMgrQueueSto[200];     /**< Storage for SerialMgr event Queue */
static QEvt const    *l_I2CBusMgrQueueSto[30][MAX_I2C_BUS];    /**< Storage for I2CBusMgr event Queue */
static QEvt const    *l_I2C1DevMgrQueueSto[30];     /**< Storage for I2C1DevMgr event Queue */
static QEvt const    *l_SysMgrQueueSto[10];           /**< Storage for SysMgr event Queue */
static QSubscrList   l_subscrSto[MAX_PUB_SIG];      /**< Storage for subscribe/publish event Queue */

static QEvt const    *l_CPLRQueueSto[10]; /**< Storage for raw QE queue for communicating with CPLR task */

/* Put the memory pool into closely coupled ram (CCMRAM).  It's very fast but
 * the drawback is that you can't DMA to/from this memory. */
CCMRAM_VAR QMPool          glbMemPool;                      /**< Global memory pool pointer */
CCMRAM_VAR static uint8_t  l_memPoolSto[QM_MEMPOOL_SIZE];   /**< memory pool storage */

/* Global-scope objects ----------------------------------------------------*/
QMPool * const p_glbMemPool = (QMPool *)&glbMemPool;  /**< "opaque" MemPool pointer */

/**
 * \union Small Events.
 * This union is a storage for small sized events.
 */
static union SmallEvents {
   void   *e0;                                       /* minimum event size */
   uint8_t e1[sizeof(QEvt)];
   uint8_t e2[sizeof(I2CStatusEvt)];
   uint8_t e3[sizeof(I2CReadReqEvt)];
   uint8_t e4[sizeof(I2CAddrEvt)];
   uint8_t e5[sizeof(I2CReadMemReqEvt)];
   uint8_t e6[sizeof(DBWriteDoneEvt)];
   uint8_t e7[sizeof(DBReadReqEvt)];
} l_smlPoolSto[50];                     /* storage for the small event pool */

/**
 * \union Medium Events.
 * This union is a storage for medium sized events.
 */
static union MediumEvents {
   void   *e0;                                       /* minimum event size */
   uint8_t e1[sizeof(I2CWriteReqEvt)];
   uint8_t e2[sizeof(I2CReadDoneEvt)];
   uint8_t e3[sizeof(DBWriteReqEvt)];
   uint8_t e4[sizeof(DBReadDoneEvt)];
} l_medPoolSto[10];                    /* storage for the medium event pool */

/**
 * \union Large Events.
 * This union is a storage for large sized events.
 */
static union LargeEvents {
   void   *e0;                                       /* minimum event size */
   uint8_t e1[sizeof(EthEvt)];
   uint8_t e2[sizeof(LrgDataEvt)];
} l_lrgPoolSto[100];                    /* storage for the large event pool */


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*............................................................................*/
int main(void)
{
   /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/
       startup_stm32f429_439xx.s/startup_stm32f401xx.s) before to branch to
       application main. To reconfigure the default setting of SystemInit()
       function, refer to system_stm32f4xx.c file
    */


   /* Enable debugging for select modules - Note: this has no effect in rel
    * builds since all DBG level logging is disabled and only LOG and up msgs
    * will get printed. */
   DBG_setDefaults();

   /* initialize the Board Support Package */
   BSP_init();

   dbg_slow_printf("Initialized BSP\n");
   log_slow_printf("Starting Application version %s built on %s\n", FW_VER, BUILD_DATE);

   /* Instantiate the Active objects by calling their "constructors"         */
   dbg_slow_printf("Initializing AO constructors\n");

   /* Make sure the comm interfaces are up first */
   SerialMgr_ctor();
   LWIPMgr_ctor();

   /* Iterate though the available I2C busses on the system and call the ctor()
    * for each instance of the I2CBusMgr AO for each bus. */
   for( uint8_t i = 0; i < MAX_I2C_BUS; ++i ) {
      I2CBusMgr_ctor( i );      /* Start this instance of AO for this bus. */
   }

   I2C1DevMgr_ctor();
   CommMgr_ctor();
   SysMgr_ctor();

   dbg_slow_printf("Initializing QF\n");
   QF_init();       /* initialize the framework and the underlying RT kernel */

   /* object dictionaries... */
   dbg_slow_printf("Initializing object dictionaries for QSPY\n");
   QS_OBJ_DICTIONARY(l_smlPoolSto);
   QS_OBJ_DICTIONARY(l_medPoolSto);
   QS_OBJ_DICTIONARY(l_lrgPoolSto);
   QS_OBJ_DICTIONARY(l_SerialMgrQueueSto);
   QS_OBJ_DICTIONARY(l_LWIPMgrQueueSto);
   QS_OBJ_DICTIONARY(l_I2CBusMgrQueueSto);
   QS_OBJ_DICTIONARY(l_I2C1DevMgrQueueSto);
   QS_OBJ_DICTIONARY(l_CommMgrQueueSto);
   QS_OBJ_DICTIONARY(l_SysMgrQueueSto);

   QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));     /* init publish-subscribe */

   /* initialize the general memory pool */
   dbg_slow_printf("Initializing general purpose memory pool in CCMRAM\n");
   QMPool_init(
         p_glbMemPool,
         l_memPoolSto,
         sizeof(l_memPoolSto),
         DC3_MAX_MEM_BLK_SIZE
   );

   /* initialize event pools... */
   dbg_slow_printf("Initializing event storage pools\n");
   QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
   QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));
   QF_poolInit(l_lrgPoolSto, sizeof(l_lrgPoolSto), sizeof(l_lrgPoolSto[0]));

   /* initialize the raw queues */
   QEQueue_init(&CPLR_evtQueue, l_CPLRQueueSto, Q_DIM(l_CPLRQueueSto));

   /* Start Active objects */
   dbg_slow_printf("Starting Active Objects\n");

   QACTIVE_START(AO_SerialMgr,
         SERIAL_MGR_PRIORITY,                                    /* priority */
         l_SerialMgrQueueSto, Q_DIM(l_SerialMgrQueueSto),       /* evt queue */
         (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
         (QEvt *)0,                               /* no initialization event */
         "SerialMgr"                                     /* Name of the task */
   );

   QACTIVE_START(AO_LWIPMgr,
         ETH_PRIORITY,                                           /* priority */
         l_LWIPMgrQueueSto, Q_DIM(l_LWIPMgrQueueSto),           /* evt queue */
         (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
         (QEvt *)0,                               /* no initialization event */
         "LWIPMgr"                                       /* Name of the task */
   );

   /* Iterate though the available I2C busses on the system and start an
    * instance of the I2CBusMgr AO for each bus.
    * WARNING!!!: make sure that the priorities for them are all together since
    * this loop will iterates through them and will take another AO's priority.
    * You will end up with clashing priorities for your AOs.*/
   for( uint8_t i = 0; i < MAX_I2C_BUS; ++i ) {
      QACTIVE_START(AO_I2CBusMgr[i],
            I2CBUS1MGR_PRIORITY + i,                                /* priority */
            l_I2CBusMgrQueueSto[i], Q_DIM(l_I2CBusMgrQueueSto[i]), /* evt queue */
            (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
            (QEvt *)0,                               /* no initialization event */
            "I2CBusMgr"                                     /* Name of the task */
      );
   }

   QACTIVE_START(AO_I2C1DevMgr,
         I2C1DEVMGR_PRIORITY,                                    /* priority */
         l_I2C1DevMgrQueueSto, Q_DIM(l_I2C1DevMgrQueueSto),     /* evt queue */
         (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
         (QEvt *)0,                               /* no initialization event */
         "I2CDevMgr"                                     /* Name of the task */
   );

   xTaskCreate(
         CPLR_Task,
         ( const char * ) "CPLRTask",                    /* Name of the task */
         THREAD_STACK_SIZE,                         /* per-thread stack size */
         NULL,                             /* arguments to the task function */
         CPLR_PRIORITY,                                          /* priority */
         ( xTaskHandle * ) &xHandle_CPLR                      /* Task handle */
   );

   QACTIVE_START(AO_CommMgr,
         COMM_MGR_PRIORITY,                                      /* priority */
         l_CommMgrQueueSto, Q_DIM(l_CommMgrQueueSto),           /* evt queue */
         (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
         (QEvt *)0,                               /* no initialization event */
         "CommMgr"                                       /* Name of the task */
   );

   QACTIVE_START(AO_SysMgr,
         SYS_MGR_PRIORITY,                                       /* priority */
         l_SysMgrQueueSto, Q_DIM(l_SysMgrQueueSto),             /* evt queue */
         (void *)0, THREAD_STACK_SIZE,              /* per-thread stack size */
         (QEvt *)0,                               /* no initialization event */
         "SysMgr"                                        /* Name of the task */
   );

   log_slow_printf("Starting QPC. All logging from here on out shouldn't show 'SLOW'!!!\n\n");
   QF_run();                                       /* run the QF application */

   return(0);
}

/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
