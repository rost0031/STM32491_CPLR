/**
 * @file    bsp.cpp
 * @brief   This file contains the "Board Support Package" functions for the
 * client.
 *
 * @date   03/23/2015
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupBSP
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE

#ifdef Q_SPY
    enum {
        PHILO_STAT = QS_USER
    };
    static uint8_t const l_clock_tick = 0U;
#endif


/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
static uint32_t l_rnd;                                         /* random seed */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/******************************************************************************/
void BSP_init(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("Client.  QEP ver: %s, QF ver: %s\n",
           QEP_getVersion(),
           QF_getVersion());

    BSP_randomSeed(1234U);
    Q_ALLEGE(QS_INIT(argv[0]));
    QS_RESET();
    QS_OBJ_DICTIONARY(&l_clock_tick);   /* must be called *after* QF_init() */
    QS_USR_DICTIONARY(PHILO_STAT);
}

/******************************************************************************/
void BSP_terminate(int16_t result)
{
    (void)result;
    QF_stop();
}

/******************************************************************************/
uint32_t BSP_random(void)
{  /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3*7*11*13*23);
    return (l_rnd >> 8);
}

/******************************************************************************/
void BSP_randomSeed(uint32_t seed)
{
    l_rnd = seed;
}

/******************************************************************************/
/*                  Various callbacks needed for QPC                          */
/******************************************************************************/

/******************************************************************************/
void QF_onStartup(void)
{
    QF_setTickRate(BSP_TICKS_PER_SEC);         /* set the desired tick rate */
}

/******************************************************************************/
void QF_onCleanup(void)
{
    QS_EXIT();                                    /* perfomr the QS cleanup */
}

/******************************************************************************/
void QF_onClockTick(void)
{
    QF_TICK(&l_clock_tick);         /* perform the QF clock tick processing */
}

/******************************************************************************/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line)
{
    cerr << "Assertion failed in "<< file << " on line "<< line << endl;
    QF_stop();
}

/******************************************************************************/
/*                  Various callbacks needed for QSPY                         */
/******************************************************************************/
/******************************************************************************/
#ifdef Q_SPY                                         /* define QS callbacks */

#include "qspy.h"

static uint8_t l_running;


/******************************************************************************/
static void *idleThread(void *par)         /* the expected P-Thread signature */
{
    (void)par;

    l_running = (uint8_t)1;
    while (l_running) {
        uint16_t nBytes = 256;
        uint8_t const *block;
        struct timeval timeout = { 0 };             /* timeout for select() */

        QF_CRIT_ENTRY(dummy);
        block = QS_getBlock(&nBytes);
        QF_CRIT_EXIT(dummy);

        if (block != (uint8_t *)0) {
            QSPY_parse(block, nBytes);
        }
        timeout.tv_usec = 10000;
        select(0, 0, 0, 0, &timeout);                  /* sleep for a while */
    }
    return 0;                                             /* return success */
}

/******************************************************************************/
uint8_t QS_onStartup(void const *arg)
{
    static uint8_t qsBuf[4*1024];                 // 4K buffer for Quantum Spy
    QS_initBuf(qsBuf, sizeof(qsBuf));

    QSPY_config((QP_VERSION >> 8),  // version
                QS_OBJ_PTR_SIZE,    // objPtrSize
                QS_FUN_PTR_SIZE,    // funPtrSize
                QS_TIME_SIZE,       // tstampSize
                Q_SIGNAL_SIZE,      // sigSize,
                QF_EVENT_SIZ_SIZE,  // evtSize
                QF_EQUEUE_CTR_SIZE, // queueCtrSize
                QF_MPOOL_CTR_SIZE,  // poolCtrSize
                QF_MPOOL_SIZ_SIZE,  // poolBlkSize
                QF_TIMEEVT_CTR_SIZE,// tevtCtrSize
                (void *)0,          // matFile,
                (void *)0,
                (QSPY_CustParseFun)0); // customized parser function

    QS_FILTER_ON(QS_ALL_RECORDS);

//    QS_FILTER_OFF(QS_QEP_STATE_EMPTY);
//    QS_FILTER_OFF(QS_QEP_STATE_ENTRY);
//    QS_FILTER_OFF(QS_QEP_STATE_EXIT);
//    QS_FILTER_OFF(QS_QEP_STATE_INIT);
//    QS_FILTER_OFF(QS_QEP_INIT_TRAN);
//    QS_FILTER_OFF(QS_QEP_INTERN_TRAN);
//    QS_FILTER_OFF(QS_QEP_TRAN);
//    QS_FILTER_OFF(QS_QEP_IGNORED);
//    QS_FILTER_OFF(QS_QEP_DISPATCH);
//    QS_FILTER_OFF(QS_QEP_UNHANDLED);

    QS_FILTER_OFF(QS_QF_ACTIVE_ADD);
    QS_FILTER_OFF(QS_QF_ACTIVE_REMOVE);
    QS_FILTER_OFF(QS_QF_ACTIVE_SUBSCRIBE);
//    QS_FILTER_OFF(QS_QF_ACTIVE_UNSUBSCRIBE);
    QS_FILTER_OFF(QS_QF_ACTIVE_POST_FIFO);
//    QS_FILTER_OFF(QS_QF_ACTIVE_POST_LIFO);
    QS_FILTER_OFF(QS_QF_ACTIVE_GET);
    QS_FILTER_OFF(QS_QF_ACTIVE_GET_LAST);
    QS_FILTER_OFF(QS_QF_EQUEUE_INIT);
    QS_FILTER_OFF(QS_QF_EQUEUE_POST_FIFO);
    QS_FILTER_OFF(QS_QF_EQUEUE_POST_LIFO);
    QS_FILTER_OFF(QS_QF_EQUEUE_GET);
    QS_FILTER_OFF(QS_QF_EQUEUE_GET_LAST);
    QS_FILTER_OFF(QS_QF_MPOOL_INIT);
    QS_FILTER_OFF(QS_QF_MPOOL_GET);
    QS_FILTER_OFF(QS_QF_MPOOL_PUT);
    QS_FILTER_OFF(QS_QF_PUBLISH);
    QS_FILTER_OFF(QS_QF_NEW);
    QS_FILTER_OFF(QS_QF_GC_ATTEMPT);
    QS_FILTER_OFF(QS_QF_GC);
    QS_FILTER_OFF(QS_QF_TICK);
    QS_FILTER_OFF(QS_QF_TIMEEVT_ARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_AUTO_DISARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM_ATTEMPT);
    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_REARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_POST);
    QS_FILTER_OFF(QS_QF_CRIT_ENTRY);
    QS_FILTER_OFF(QS_QF_CRIT_EXIT);
    QS_FILTER_OFF(QS_QF_ISR_ENTRY);
    QS_FILTER_OFF(QS_QF_ISR_EXIT);

    QS_RESET();

    {
        pthread_attr_t attr;
        struct sched_param param;
        pthread_t idle;

        /* SCHED_FIFO corresponds to real-time preemptive priority-based
        * scheduler.
        * NOTE: This scheduling policy requires the superuser priviledges
        */
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        param.sched_priority = sched_get_priority_min(SCHED_FIFO);

        pthread_attr_setschedparam(&attr, &param);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (pthread_create(&idle, &attr, &idleThread, 0) != 0) {
               /* Creating the p-thread with the SCHED_FIFO policy failed.
               * Most probably this application has no superuser privileges,
               * so we just fall back to the default SCHED_OTHER policy
               * and priority 0.
               */
            pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
            param.sched_priority = 0;
            pthread_attr_setschedparam(&attr, &param);
            Q_ALLEGE(pthread_create(&idle, &attr, &idleThread, 0) == 0);
        }
        pthread_attr_destroy(&attr);
    }

    return (uint8_t)1;
}

/******************************************************************************/
void QS_onCleanup(void)
{
    l_running = (uint8_t)0;
    QSPY_stop();
}

/******************************************************************************/
void QS_onFlush(void)
{
    uint16_t nBytes = 1024;
    uint8_t const *block;
    QF_CRIT_ENTRY(dummy);
    while ((block = QS_getBlock(&nBytes)) != (uint8_t *)0) {
        QF_CRIT_EXIT(dummy);
        QSPY_parse(block, nBytes);
        nBytes = 1024;
    }
}

/******************************************************************************/
QSTimeCtr QS_onGetTime(void)
{
    return (QSTimeCtr)clock();                                   // see NOTE01
}

/******************************************************************************/
void QSPY_onPrintLn(void)
{
    fputs(QSPY_line, stdout);
    fputc('\n', stdout);
}

/*****************************************************************************
* NOTE01:
* clock() is the most portable facility, but might not provide the desired
* granularity. Other, less-portable alternatives are clock_gettime(), rdtsc(),
* or gettimeofday().
*/

#endif                                                             /* Q_SPY */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
