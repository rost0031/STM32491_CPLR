/**
 * @file 	i2c_frt.c
 * @brief   I2C Device interface for FreeRTOS.
 *
 * @date    03/13/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c_frt.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"
#include "Shared.h"
#include "I2C1DevMgr.h"                    /* For access to the I2C1DevMgr AO */
#include "cplr.h"
#include "i2c_dev.h"
#include "i2c.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_I2C_DEV ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
const DC3Error_t I2C_readDevMemFRT(
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      uint8_t* const pBuffer,
      const uint16_t nBufferSize,
      uint16_t* pBytesRead,
      const uint16_t nBytesToRead
)
{
   DC3Error_t status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   /* Check buffer sizes */
   if( nBytesToRead > nBufferSize ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_readDevMemFRT_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   if ( NULL == pBuffer ) {
      status = ERR_MEM_NULL_VALUE;
      goto I2C_readDevMemFRT_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   /* Issue a non-blocking call to read I2C */
   status = I2C_readDevMemEVT(
         iDev,                                        // DC3I2CDevice_t iDev,
         offset,                                      // uint16_t offset,
         nBytesToRead,                                // uint16_t bytesToRead,
         _DC3_ACCESS_FRT,                             // AccessType_t accType,
         (QActive *)NULL                              // QActive* callingAO
   );

   if( ERR_NONE != status ) {
      goto I2C_readDevMemFRT_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   /* Suspend the task.  Once something has been put into the queue, the AO that
    * put it there, will wake up this task. */
   DBG_printf("Suspending Task\n");
   vTaskSuspend( xHandle_CPLR );

   DBG_printf("Task resumed\n");
   /* The task has been awakened by the I2CXDevMgr AO because it has put an
    * event into the raw queue */
   QEvt const *evtI2CDone = QEQueue_get(&CPLR_evtQueue);
   if (evtI2CDone != (QEvt *)0 ) {
      LOG_printf("Found expected event in queue\n");
      switch( evtI2CDone->sig ) {
         case I2C1_DEV_READ_DONE_SIG:
            DBG_printf("Got I2C1_DEV_READ_DONE_SIG\n");
            *pBytesRead = ((I2CReadDoneEvt *) evtI2CDone)->bytes;
            status = ((I2CReadDoneEvt *) evtI2CDone)->status;
            MEMCPY(pBuffer,
                  ((I2CReadDoneEvt *) evtI2CDone)->dataBuf,
                  *pBytesRead
            );

            break;
         default:
            WRN_printf("Unknown signal %d\n", evtI2CDone->sig);
            break;
      }
      QF_gc(evtI2CDone);         /* Don't forget to garbage collect the event */
   } else {
      WRN_printf("Expected event not found in queue\n");
   }

I2C_readDevMemFRT_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         _DC3_ACCESS_FRT,
         "Error 0x%08x reading I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
const DC3Error_t I2C_writeDevMemFRT(
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      const uint8_t* const pBuffer,
      const uint16_t nBufferSize,
      uint16_t *pBytesWritten,
      const uint16_t nBytesToWrite
)
{
   DC3Error_t status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   /* Check buffer sizes */
   if( nBytesToWrite > nBufferSize ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_writeDevMemFRT_ERR_HANDLER; /* Stop and jump to error handling */
   }

   if ( NULL == pBuffer ) {
      status = ERR_MEM_NULL_VALUE;
      goto I2C_writeDevMemFRT_ERR_HANDLER; /* Stop and jump to error handling */
   }

   /* Issue a non-blocking call to read I2C */
   status = I2C_writeDevMemEVT(
         iDev,                                        // DC3I2CDevice_t iDev,
         offset,                                      // uint16_t offset,
         nBytesToWrite,                               // uint16_t bytesToRead,
         _DC3_ACCESS_FRT,                             // AccessType_t accType,
         (QActive *)NULL,                             // QActive* callingAO
         pBuffer                                      // uint8_t* pBuffer
   );

   if( ERR_NONE != status ) {
      goto I2C_writeDevMemFRT_ERR_HANDLER; /* Stop and jump to error handling */
   }

   /* Suspend the task.  Once something has been put into the queue, the AO that
    * put it there, will wake up this task. */
   DBG_printf("Suspending Task\n");
   vTaskSuspend( xHandle_CPLR );
   DBG_printf("Task resumed\n");

   /* The task has been awakened by the I2CXDevMgr AO because it has put an
    * event into the raw queue */
   QEvt const *evtI2CDone = QEQueue_get(&CPLR_evtQueue);
   if (evtI2CDone != (QEvt *)0 ) {
      LOG_printf("Found expected event in queue\n");
      switch( evtI2CDone->sig ) {
         case I2C1_DEV_WRITE_DONE_SIG:
            DBG_printf("Got I2C1_DEV_WRITE_DONE_SIG\n");
            *pBytesWritten = ((I2CWriteDoneEvt *) evtI2CDone)->bytes;
            status = ((I2CWriteDoneEvt *) evtI2CDone)->status;
            break;
         default:
            WRN_printf("Unknown signal %d\n", evtI2CDone->sig);
            break;
      }
      QF_gc(evtI2CDone);         /* Don't forget to garbage collect the event */
   } else {
      WRN_printf("Expected event not found in queue\n");
   }

I2C_writeDevMemFRT_ERR_HANDLER:   /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         _DC3_ACCESS_FRT,
         "Error 0x%08x writing I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/

