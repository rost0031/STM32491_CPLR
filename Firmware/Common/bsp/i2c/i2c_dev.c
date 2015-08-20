/**
 * @file 	i2c_dev.c
 * @brief   Data for devices on the I2C Busses.
 *
 * @date   	10/28/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c_defs.h"
#include "i2c.h"
#include "i2c_dev.h"
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "stm32f4xx_dma.h"                           /* For STM32 DMA support */
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */
#include "I2C1DevMgr.h"                    /* For access to the I2C1DevMgr AO */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_I2C_DEV ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief An internal structure that holds settings for I2C devices on all I2C
 * busses.
 */
I2C_DevSettings_t s_I2C_Dev[_DC3_MaxI2CDev] =
{
      {
            /* "External" device settings */
            _DC3_EEPROM,                    /**< i2c_dev */
            I2CBus1,                   /**< i2c_bus */
            1,                         /**< i2c_dev_addr_size */
            0xA0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x00,                      /**< i2c_mem_addr */
            0x00,                      /**< i2c_mem_min_addr */
            0xFF,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE,          /**< i2c_mem_page_size */
      },
      {
            /* "External" device settings */
            _DC3_SNROM,                    /**< i2c_dev */
            I2CBus1,                   /**< i2c_bus */
            1,                         /**< i2c_dev_addr_size */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x80,                      /**< i2c_mem_addr */
            0x80,                      /**< i2c_mem_min_addr */
            0x8F,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE,          /**< i2c_mem_page_size */
      },
      {
            /* "External" device settings */
            _DC3_EUIROM,                   /**< i2c_dev */
            I2CBus1,                   /**< i2c_bus */
            1,                         /**< i2c_dev_addr_size */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x98,                      /**< i2c_mem_addr */
            0x98,                      /**< i2c_mem_min_addr */
            0x9F,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE,          /**< i2c_mem_page_size */
      },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
const uint8_t I2C_getDevAddrSize( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_dev_addr_size );
}

/******************************************************************************/
const uint16_t I2C_getDevAddr( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_dev_addr );
}

/******************************************************************************/
const uint8_t I2C_getMemAddrSize( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_addr_size );
}

/******************************************************************************/
const uint16_t I2C_getMemAddr( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_addr );
}

/******************************************************************************/
void I2C_setI2CMemAddr( const DC3I2CDevice_t iDev, const uint16_t addr )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   s_I2C_Dev[iDev].i2c_mem_addr = 0x00FF & addr;
}

/******************************************************************************/
const uint16_t I2C_getMinMemAddr( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_min_addr );
}

/******************************************************************************/
const uint16_t I2C_getMaxMemAddr( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_max_addr );
}

/******************************************************************************/
const uint8_t I2C_getPageSize( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_page_size );
}

/******************************************************************************/
const I2C_Bus_t I2C_getBus( const DC3I2CDevice_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_bus );
}

/******************************************************************************/
const DC3Error_t I2C_calcPageWriteSizes(
      uint8_t* const writeSizeFirstPage,
      uint8_t* const writeSizeLastPage,
      uint8_t* const writeTotalPages,
      const uint16_t i2cMemAddr,
      const uint16_t bytesToWrite,
      const uint16_t pageSize
)
{
   /* Check if we are going to write over the end of the eeprom */
   if (i2cMemAddr + bytesToWrite > I2C_getMaxMemAddr( _DC3_EEPROM )) {
      return ( ERR_I2C1DEV_MEM_OUT_BOUNDS );
   }

   /* Calculate space available in the first page */
   uint8_t pageSpace = (((i2cMemAddr/(pageSize)) + 1) * (pageSize)) - i2cMemAddr;

   /* Calculate how many bytes to write in the first page */
   *writeSizeFirstPage = MIN(pageSpace, bytesToWrite);

   /* Calculate how many bytes to write on the last page*/
   *writeSizeLastPage = (bytesToWrite - *writeSizeFirstPage) % pageSize;

   /* Calculate how many pages we are going to be writing (full and partial) */
   *writeTotalPages = 0;
   if (bytesToWrite > *writeSizeFirstPage) {
      *writeTotalPages = ((bytesToWrite - *writeSizeFirstPage)/(pageSize)) + 2;
      if (*writeTotalPages > 1 && *writeSizeLastPage == 0) {
         *writeTotalPages -= 1;
      }
   } else {
      *writeTotalPages = 1;
   }
   return( ERR_NONE );
}

/******************************************************************************/
const DC3Error_t I2C_readDevMem(
      const DC3AccessType_t accessType,
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      const uint16_t bytesToRead,
      const uint16_t bufferSize,
      uint8_t* const pBuffer,
      uint16_t* pBytesRead
)
{
   DC3Error_t status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */

   /* Check I2C device memory boundaries against request */
   switch( iDev ) {
      case _DC3_EUIROM:                         /* Intentionally fall through */
      case _DC3_SNROM:                          /* Intentionally fall through */
      case _DC3_EEPROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToRead > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_readDevMem_ERR_HANDLER; /* Stop and jump to error handling */
         }
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_readDevMem_ERR_HANDLER;    /* Stop and jump to error handling */
         break;
   }

   /* Perform the actual request to read the device. Note that the BLK functions
    * take an actual address while the QPC/FRT accesses take offset and the AO
    * will figure out the correct addresses */
   switch( accessType ) {
      case _DC3_ACCESS_BARE:
         /* Perform a blocking read. Buffer and buffer size will be checked in
          * the only function that requires them (the BLK type functions ) */
         status = I2C_readBufferBLK(
               I2C_getBus( iDev),                 // const I2C_Bus_t iBus,
               I2C_getDevAddr( iDev ),            // const uint8_t i2cDevAddr,
               I2C_getMemAddr( iDev ) + offset,   // const uint16_t i2cMemAddr,
               I2C_getMemAddrSize( iDev ),        // const uint8_t i2cMemAddrSize,
               bytesToRead,                       // const uint16_t bytesToRead,
               bufferSize,                        // const uint16_t bufferSize,
               pBuffer,                           // uint8_t* const pBuffer,
               pBytesRead                         // uint16_t* pBytesRead
         );

         break;
      case _DC3_ACCESS_QPC:                     /* Intentionally fall through */
      case _DC3_ACCESS_FRT:
         ; /* Fix for comments immediately following a case label */
         /* It is the responsibility of the caller of this function with
          * accessType QPC to know what event to listen for in response to this
          * request.  See I2C1DevMgr AO for details. */

         /* Create the event and directly post it to the right AO. */
         I2CReadReqEvt *i2cReadReqEvt  = Q_NEW(I2CReadReqEvt, I2C1_DEV_RAW_MEM_READ_SIG);
         i2cReadReqEvt->i2cDev         = iDev;
         i2cReadReqEvt->start          = offset;
         i2cReadReqEvt->bytes          = bytesToRead;
         i2cReadReqEvt->accessType     = accessType;
         QACTIVE_POST(AO_I2C1DevMgr, (QEvt *)(i2cReadReqEvt), AO_I2C1DevMgr);
         break;
      case _DC3_ACCESS_NONE:                    /* Intentionally fall through */
      default:
         status = ERR_INVALID_ACCESS_TYPE;
         goto I2C_readDevMem_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

I2C_readDevMem_ERR_HANDLER:       /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT( status, accessType,
      "Requesting a read of I2C device %s on %s at mem addr 0x%02x via %s: Error 0x%08x\n",
      CON_i2cDevToStr(iDev), CON_i2cBusToStr( I2C_getBus(iDev) ),
      I2C_getMemAddr( iDev ) + offset, CON_accessToStr(accessType), status );
   return( status );
}

/******************************************************************************/
const DC3Error_t I2C_writeDevMem(
      const DC3AccessType_t accessType,
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      const uint16_t bytesToWrite,
      const uint16_t bufferSize,
      const uint8_t* const pBuffer,
      uint16_t* pBytesWritten
)
{
   DC3Error_t status = ERR_NONE; /* Keep track of the errors that may occur.
                                    This gets returned at the end of the
                                    function */

   /* Check I2C device memory boundaries against request */
   switch( iDev ) {
      case _DC3_EUIROM:                         /* Intentionally fall through */
      case _DC3_SNROM:
         status = ERR_I2C_DEV_IS_READ_ONLY;
         goto I2C_writeDevMem_ERR_HANDLER; /* Stop and jump to error handling */
         break;
      case _DC3_EEPROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToWrite > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_writeDevMem_ERR_HANDLER; /* Stop and jump to error handling */
         }
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_writeDevMem_ERR_HANDLER;    /* Stop and jump to error handling */
         break;
   }

   /* Perform the actual request to read the device */
   switch( accessType ) {
      case _DC3_ACCESS_BARE:


         /* Perform a blocking write */
         status = I2C_writeBufferBLK(
               I2C_getBus( iDev),                 // const I2C_Bus_t iBus,
               I2C_getDevAddr( iDev ),            // const uint8_t i2cDevAddr,
               I2C_getMemAddr( iDev ) + offset,   // const uint16_t i2cMemAddr,
               I2C_getMemAddrSize( iDev ),        // const uint8_t i2cMemAddrSize,
               I2C_getPageSize( iDev ),           // const uint16_t pageSize,
               bytesToWrite,                      // const uint16_t bytesToWrite,
               bufferSize,                        // const uint16_t bufferSize,
               pBuffer,                           // const uint8_t* const pBuffer,
               pBytesWritten
         );

         break;
      case _DC3_ACCESS_QPC:                     /* Intentionally fall through */
      case _DC3_ACCESS_FRT:
         ; /* Fix for comments immediately following a case label */
         /* It is the responsibility of the caller of this function with
          * accessType QPC to know what event to listen for in response to this
          * request.  See I2C1DevMgr AO for details. */

         /* Create the event and directly post it to the right AO. */
         I2CWriteReqEvt *i2cWriteReqEvt   = Q_NEW(I2CWriteReqEvt, I2C1_DEV_RAW_MEM_WRITE_SIG);
         i2cWriteReqEvt->i2cDev           = iDev;
         i2cWriteReqEvt->start            = offset;
         i2cWriteReqEvt->bytes            = bytesToWrite;
         i2cWriteReqEvt->accessType       = accessType;
         MEMCPY(
               i2cWriteReqEvt->dataBuf,
               pBuffer,
               i2cWriteReqEvt->bytes
         );
         QACTIVE_POST(AO_I2C1DevMgr, (QEvt *)(i2cWriteReqEvt), AO_I2C1DevMgr);
         break;
      case _DC3_ACCESS_NONE:                    /* Intentionally fall through */
      default:
         status = ERR_INVALID_ACCESS_TYPE;
         goto I2C_writeDevMem_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

I2C_writeDevMem_ERR_HANDLER:       /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT( status, accessType,
      "Requesting a write of I2C device %s on %s at mem addr 0x%02x via %s: Error 0x%08x\n",
      CON_i2cDevToStr(iDev), CON_i2cBusToStr( I2C_getBus(iDev) ),
      I2C_getMemAddr( iDev ) + offset, CON_accessToStr(accessType), status );
   return( status );
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/

