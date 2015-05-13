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
DBG_DEFINE_THIS_MODULE( DBG_MODL_I2C_DEV ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief An internal structure that holds settings for I2C devices on all I2C
 * busses.
 */
I2C_DevSettings_t s_I2C_Dev[MAX_I2C_DEV] =
{
      {
            /* "External" device settings */
            EEPROM,                    /**< i2c_dev */
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
            SN_ROM,                    /**< i2c_dev */
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
            EUI_ROM,                   /**< i2c_dev */
            I2CBus1,                   /**< i2c_bus */
            1,                         /**< i2c_dev_addr_size */
            0xB0,                      /**< i2c_dev_addr */

            /* Internal device settings */
            1,                         /**< i2c_mem_addr_size */
            0x98,                      /**< i2c_mem_addr */
            0x98,                      /**< i2c_mem_min_addr */
            0x9F,                      /**< i2c_mem_max_addr */
            EEPROM_PAGE_SIZE / 2,      /**< i2c_mem_page_size */
      },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
const uint8_t I2C_getDevAddrSize( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_dev_addr_size );
}

/******************************************************************************/
const uint16_t I2C_getDevAddr( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_dev_addr );
}

/******************************************************************************/
const uint8_t I2C_getMemAddrSize( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_addr_size );
}

/******************************************************************************/
const uint16_t I2C_getMemAddr( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_addr );
}

/******************************************************************************/
void I2C_setI2CMemAddr( const I2C_Dev_t iDev, const uint16_t addr )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   s_I2C_Dev[iDev].i2c_mem_addr = 0x00FF & addr;
}

/******************************************************************************/
const uint16_t I2C_getMinMemAddr( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_min_addr );
}

/******************************************************************************/
const uint16_t I2C_getMaxMemAddr( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_max_addr );
}

/******************************************************************************/
const uint8_t I2C_getPageSize( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_mem_page_size );
}

/******************************************************************************/
const I2C_Bus_t I2C_getBus( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   return( s_I2C_Dev[iDev].i2c_bus );
}

/******************************************************************************/
const char* const I2C_devToStr( const I2C_Dev_t iDev )
{
   /* Check inputs */
   assert_param( IS_I2C_DEVICE( iDev ) );

   switch ( iDev ) {
      case EEPROM: return("EEPROM"); break;
      case SN_ROM: return("SN_ROM"); break;
      case EUI_ROM: return("EUI_ROM"); break;
      default: return(""); break;
   }
}

/******************************************************************************/
const CBErrorCode I2C_calcPageWriteSizes(
      uint8_t* const writeSizeFirstPage,
      uint8_t* const writeSizeLastPage,
      uint8_t* const writeTotalPages,
      const uint16_t i2cMemAddr,
      const uint16_t bytesToWrite,
      const uint16_t pageSize
)
{
   /* Check if we are going to write over the end of the eeprom */
   if (i2cMemAddr + bytesToWrite > I2C_getMaxMemAddr( EEPROM )) {
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
   } else {
      *writeTotalPages = 1;
   }
   return( ERR_NONE );
}

/******************************************************************************/
CBErrorCode I2C_readDevMemEVT(
      const I2C_Dev_t iDev,
      const uint16_t offset,
      const uint16_t bytesToRead,
      const AccessType_t accType,
      const QActive* const callingAO
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   QSignal sig = 0;               /* Signal which will be used in the event to
                                     be posted. This may change based on which
                                     AO will be posted to depending on which I2C
                                     device was passed in */
   QActive* aoToPostTo = 0;       /* AO to which to directly post the even. This
                                     will change depending on which I2C device
                                     was passed in */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   switch( iDev ) {
      case EEPROM:                              /* Intentionally fall through */
      case SN_ROM:                              /* Intentionally fall through */
      case EUI_ROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToRead > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_readDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         sig = I2C1_DEV_RAW_MEM_READ_SIG;
         aoToPostTo = AO_I2C1DevMgr;
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_readDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   /* Create the event and directly post it to the right AO. */
   I2CReadReqEvt *i2cReadReqEvt  = Q_NEW(I2CReadReqEvt, sig);
   i2cReadReqEvt->i2cDev         = iDev;
   i2cReadReqEvt->addr           = I2C_getMemAddr( iDev ) + offset;
   i2cReadReqEvt->bytes          = bytesToRead;
   i2cReadReqEvt->accessType     = accType;
   QACTIVE_POST(aoToPostTo, (QEvt *)(i2cReadReqEvt), callingAO);


I2C_readDevMemEVT_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
         "Error 0x%08x reading I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
CBErrorCode I2C_writeDevMemEVT(
      const I2C_Dev_t iDev,
      const uint16_t offset,
      const uint16_t bytesToWrite,
      const AccessType_t accType,
      const QActive* const callingAO,
      const uint8_t* const pBuffer
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */
   QSignal sig = 0;               /* Signal which will be used in the event to
                                     be posted. This may change based on which
                                     AO will be posted to depending on which I2C
                                     device was passed in */
   QActive* aoToPostTo = 0;       /* AO to which to directly post the even. This
                                     will change depending on which I2C device
                                     was passed in */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   switch( iDev ) {
      case EEPROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToWrite > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_writeDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         sig = I2C1_DEV_RAW_MEM_WRITE_SIG;
         aoToPostTo = AO_I2C1DevMgr;
         break;

      case SN_ROM:                              /* Intentionally fall through */
      case EUI_ROM:
         status = ERR_I2C_DEV_IS_READ_ONLY;
         goto I2C_writeDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_writeDevMemEVT_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   /* Create the event and directly post it to the right AO. */
   I2CWriteReqEvt *i2cWriteReqEvt   = Q_NEW(I2CWriteReqEvt, sig);
   i2cWriteReqEvt->i2cDev           = iDev;
   i2cWriteReqEvt->addr             = I2C_getMemAddr( iDev ) + offset;
   i2cWriteReqEvt->bytes            = bytesToWrite;
   i2cWriteReqEvt->accessType       = accType;
   MEMCPY(
         i2cWriteReqEvt->dataBuf,
         pBuffer,
         i2cWriteReqEvt->bytes
   );
   QACTIVE_POST(aoToPostTo, (QEvt *)(i2cWriteReqEvt), callingAO);


I2C_writeDevMemEVT_ERR_HANDLER:   /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
         "Error 0x%08x writing I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
/* Blocking Functions - Don't call unless before threads/AOs started or after */
/* they crashed                                                               */
/******************************************************************************/

/******************************************************************************/
CBErrorCode I2C_readDevMemBLK(
      const I2C_Dev_t iDev,
      const uint16_t offset,
      const uint16_t bytesToRead,
      const AccessType_t accType,
      uint8_t* pBuffer,
      const uint8_t  bufSize
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   if ( bufSize < bytesToRead ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_readDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
   }

   switch( iDev ) {
      case EEPROM:                              /* Intentionally fall through */
      case SN_ROM:                              /* Intentionally fall through */
      case EUI_ROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToRead > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_readDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_readDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   status = I2C_readBufferBLK(
         I2C_getBus( iDev),                 // I2C_Bus_t iBus,
         I2C_getDevAddr( iDev ),            // uint8_t i2cDevAddr,
         I2C_getMemAddr( iDev ) + offset,   // uint16_t i2cMemAddr,
         I2C_getMemAddrSize( iDev ),        // uint8_t i2cMemAddrSize,
         pBuffer,                           // uint8_t* pBuffer,
         bytesToRead                        // uint16_t bytesToRead
   );

I2C_readDevMemBLK_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
         "Error 0x%08x reading I2C device %s on %s at mem addr 0x%02x\n",
         status,
         I2C_devToStr(iDev),
         I2C_busToStr( I2C_getBus(iDev) ),
         I2C_getMemAddr( iDev ) + offset
   );
   return( status );
}

/******************************************************************************/
CBErrorCode I2C_writeDevMemBLK(
      const I2C_Dev_t iDev,
      const uint16_t offset,
      const uint16_t bytesToWrite,
      const AccessType_t accType,
      const uint8_t* const pBuffer,
      const uint8_t  bufSize
)
{
   CBErrorCode status = ERR_NONE; /* Keep track of the errors that may occur.
                                     This gets returned at the end of the
                                     function */

   /* Check inputs, requested read boundaries, and figure out which signals will
    * be used to post the event to which AO. */
   if ( bufSize < bytesToWrite ) {
      status = ERR_MEM_BUFFER_LEN;
      goto I2C_writeDevMemBLK_ERR_HANDLER; /* Stop and jump to error handling */
   }

   switch( iDev ) {
      case EEPROM:
         /* These 3 devices are actually part of the same EEPROM chip (and thus
          * on the same I2C bus) but have different bus addresses.  They can all
          * be handled by this case. */
         if ( offset + bytesToWrite > I2C_getMaxMemAddr( iDev ) ) {
            status = ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY;
            goto I2C_writeDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         }
         break;
      case EUI_ROM:                             /* Intentionally fall through */
      case SN_ROM:
         status = ERR_I2C_DEV_IS_READ_ONLY;
         goto I2C_writeDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
      default:
         status = ERR_I2C_DEV_INVALID_DEVICE;
         goto I2C_writeDevMemBLK_ERR_HANDLER;  /* Stop and jump to error handling */
         break;
   }

   status = I2C_writeBufferBLK(
         I2C_getBus( iDev),                     // I2C_Bus_t iBus,
         I2C_getDevAddr( iDev ),                // uint8_t i2cDevAddr,
         I2C_getMemAddr( iDev ) + offset,       // uint16_t i2cMemAddr,
         I2C_getMemAddrSize( iDev ),            // uint8_t i2cMemAddrSize,
         pBuffer,                               // uint8_t* pBuffer,
         bytesToWrite,                          // uint16_t bytesToWrite
         I2C_getPageSize( iDev )                // uint16_t pageSize
   );

I2C_writeDevMemBLK_ERR_HANDLER:    /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT(
         status,
         accType,
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

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/

