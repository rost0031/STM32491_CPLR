/**
 * @file 	flash.c
 * @brief   Contains implementation of the flash memory manipulation functions
 * specific to DC3 (STM32F4 with 2MB of flash)
 *
 * @date   	04/29/2015
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "flash.h"
#include "project_includes.h"
#include <stdio.h>
#include "version.h"
#include "DC3CommApi.h"

/* Compile-time called macros ------------------------------------------------*/
DBG_DEFINE_THIS_MODULE( DBG_MODL_FLASH );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* This stores the fw version and build date to the special section of flash of
 * the bootloader located in the last 1K of flash. */
__attribute__((__section__(".btldrInfo"))) const char flash_btldr_ver[32] = FW_VER;
__attribute__((__section__(".btldrInfo"))) const char flash_btldr_date[32] = BUILD_DATE;

/**<! Array to look up sectors */
const uint32_t sectorArray[ADDR_FLASH_SECTORS] = {
      ADDR_FLASH_SECTOR_0,
      ADDR_FLASH_SECTOR_1,
      ADDR_FLASH_SECTOR_2,
      ADDR_FLASH_SECTOR_3,
      ADDR_FLASH_SECTOR_4,
      ADDR_FLASH_SECTOR_5,
      ADDR_FLASH_SECTOR_6,
      ADDR_FLASH_SECTOR_7,
      ADDR_FLASH_SECTOR_8,
      ADDR_FLASH_SECTOR_9,
      ADDR_FLASH_SECTOR_10,
      ADDR_FLASH_SECTOR_11,
      ADDR_FLASH_SECTOR_12,
      ADDR_FLASH_SECTOR_13,
      ADDR_FLASH_SECTOR_14,
      ADDR_FLASH_SECTOR_15,
      ADDR_FLASH_SECTOR_16,
      ADDR_FLASH_SECTOR_17,
      ADDR_FLASH_SECTOR_18,
      ADDR_FLASH_SECTOR_19,
      ADDR_FLASH_SECTOR_20,
      ADDR_FLASH_SECTOR_21,
      ADDR_FLASH_SECTOR_22,
      ADDR_FLASH_SECTOR_23
};

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Convert STM32 Flash_Status type to DC3Error_t corresponding code.
  * @param [in] const FLASH_Status STM32 Flash_Status error code
  * @return: DC3Error_t:
  *   @arg ERR_NONE: flash operation finished
  *   other error codes for failure.
  */
static const DC3Error_t FLASH_statusToErrorCode( const FLASH_Status flashStatus );

/**
  * @brief  Gets the sector base address given an address
  * @param [in] const uint32_t address for which to look up the sector
  * @retval The sector base address of a given address
  */
static const uint32_t FLASH_addrToSectorAddr( const uint32_t addr );

/**
  * @brief  Gets the NEXT sector base address given an address
  * @param [in] const uint32_t address for which to look up the sector
  * @retval The sector base address of a given address
  */
static const uint32_t FLASH_getNextSectorAddr( const uint32_t addr );

/**
  * @brief  Gets the STM32 value for sector of a given base address of that
  * sector
  * @param [in] const uint32_t sector base address for which to look up the STM32
  * sector
  * @retval The STM32 flash sector of a given address
  */
static const uint16_t FLASH_sectorAddrToFlashSect( const uint32_t addr );


/**
 * @brief   Read a uint8_t from flash
 * @param [in] addr: const uint32_t address where to read from
 * @return  data: uint8_t value stored at the specified address
 */
static const uint8_t FLASH_readUint8( const uint32_t addr );

/**
 * @brief   Write a uint8_t to the flash
 * @param [in] addr: const uint32_t address where to write data to
 * @param [in] data: const uint8_t value to write to the specified address
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
static const DC3Error_t FLASH_writeUint8( const uint32_t addr, const uint8_t data );

/**
 * @brief   Read a uint32_t from flash
 * @param [in] addr: const uint32_t address where to read from
 * @return  data: uint32_t value stored at the specified address
 */
static const uint32_t FLASH_readUint32( const uint32_t addr );

/**
 * @brief   Write a uint32_t to the flash
 * @param [in] addr: const uint32_t address where to write data to
 * @param [in] data: const uint32_t value to write to the specified address
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
static const DC3Error_t FLASH_writeUint32( const uint32_t addr, const uint32_t data );

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
static const DC3Error_t FLASH_statusToErrorCode( const FLASH_Status flashStatus )
{
   DC3Error_t status = ERR_NONE;
   switch( flashStatus ) {
      case FLASH_BUSY:              status = ERR_FLASH_BUSY; break;
      case FLASH_ERROR_RD:          status = ERR_FLASH_READ; break;
      case FLASH_ERROR_PGS:         status = ERR_FLASH_PGS; break;
      case FLASH_ERROR_PGP:         status = ERR_FLASH_PGP; break;
      case FLASH_ERROR_PGA:         status = ERR_FLASH_PGA; break;
      case FLASH_ERROR_WRP:         status = ERR_FLASH_WRP; break;
      case FLASH_ERROR_PROGRAM:     status = ERR_FLASH_PROGRAM; break;
      case FLASH_ERROR_OPERATION:   status = ERR_FLASH_OPERATION; break;
      case FLASH_COMPLETE:
      default:
         status = ERR_NONE; break;
   }
   return( status );
}

/******************************************************************************/
static const uint32_t FLASH_addrToSectorAddr( const uint32_t addr )
{
  uint32_t sectorAddr = 0;

  for( uint8_t i=1; i < ADDR_FLASH_SECTORS; i++ ) {
     if( addr >= sectorArray[i-1] && addr < sectorArray[i] ) {
        return( sectorArray[i-1] );
     }
  }
  /* We should never get here unless a very invalid address gets passed in */
  return sectorAddr;
}

/******************************************************************************/
static const uint32_t FLASH_getNextSectorAddr( const uint32_t addr )
{
  uint32_t sectorAddr = 0;

  for( uint8_t i=1; i < ADDR_FLASH_SECTORS; i++ ) {
     if( addr >= sectorArray[i-1] && addr < sectorArray[i] ) {
        return( sectorArray[i] );
     }
  }
  /* We should never get here unless a very invalid address gets passed in */
  return sectorAddr;
}

/******************************************************************************/
static const uint16_t FLASH_sectorAddrToFlashSect( const uint32_t addr )
{
   uint16_t sector = 0;
   switch( addr ) {
      case ADDR_FLASH_SECTOR_0:  sector = FLASH_Sector_0;  break;
      case ADDR_FLASH_SECTOR_1:  sector = FLASH_Sector_1;  break;
      case ADDR_FLASH_SECTOR_2:  sector = FLASH_Sector_2;  break;
      case ADDR_FLASH_SECTOR_3:  sector = FLASH_Sector_3;  break;
      case ADDR_FLASH_SECTOR_4:  sector = FLASH_Sector_4;  break;
      case ADDR_FLASH_SECTOR_5:  sector = FLASH_Sector_5;  break;
      case ADDR_FLASH_SECTOR_6:  sector = FLASH_Sector_6;  break;
      case ADDR_FLASH_SECTOR_7:  sector = FLASH_Sector_7;  break;
      case ADDR_FLASH_SECTOR_8:  sector = FLASH_Sector_8;  break;
      case ADDR_FLASH_SECTOR_9:  sector = FLASH_Sector_9;  break;
      case ADDR_FLASH_SECTOR_10: sector = FLASH_Sector_10; break;
      case ADDR_FLASH_SECTOR_11: sector = FLASH_Sector_11; break;
      case ADDR_FLASH_SECTOR_12: sector = FLASH_Sector_12; break;
      case ADDR_FLASH_SECTOR_13: sector = FLASH_Sector_13; break;
      case ADDR_FLASH_SECTOR_14: sector = FLASH_Sector_14; break;
      case ADDR_FLASH_SECTOR_15: sector = FLASH_Sector_15; break;
      case ADDR_FLASH_SECTOR_16: sector = FLASH_Sector_16; break;
      case ADDR_FLASH_SECTOR_17: sector = FLASH_Sector_17; break;
      case ADDR_FLASH_SECTOR_18: sector = FLASH_Sector_18; break;
      case ADDR_FLASH_SECTOR_19: sector = FLASH_Sector_19; break;
      case ADDR_FLASH_SECTOR_20: sector = FLASH_Sector_20; break;
      case ADDR_FLASH_SECTOR_21: sector = FLASH_Sector_21; break;
      case ADDR_FLASH_SECTOR_22: sector = FLASH_Sector_22; break;
      case ADDR_FLASH_SECTOR_23: /* Intentionally fall through */
      default:
         sector = FLASH_Sector_23;break;
   }

   return sector;
}

/******************************************************************************/
const DC3Error_t FLASH_getSectorsToErase(
      uint32_t *sectorArrayLoc,
      uint8_t *nSectors,
      const uint8_t sectorArraySize,
      const DC3BootMode flashImageLoc,
      const uint32_t flashImageSize
)
{
   DC3Error_t status = ERR_NONE;

   /* Make sure the buffer is not null */
   if ( NULL == sectorArray ) {
      status = ERR_MEM_NULL_VALUE;
      ERR_printf("Sector array is NULL. Error: 0x%08x\n", status);
      return( status );
   }

   /* Make sure the buffer is long enough to fit enough sectors */
   if( sectorArraySize < ADDR_FLASH_SECTORS ) {
      status = ERR_MEM_BUFFER_LEN;
      ERR_printf("Sector array is not long enough. Error: 0x%08x\n", status);
      return( status );
   }

   uint32_t startAddr = 0;
   uint32_t currAddr = 0;
   /* Check for a valid image type and set the correct starting address */
   if( _DC3_Application == flashImageLoc ) {
      startAddr = FLASH_APPL_START_ADDR;
   } else if( _DC3_Bootloader == flashImageLoc ) {
      startAddr = FLASH_BOOT_START_ADDR;
   } else {
      status = ERR_FLASH_IMAGE_TYPE_INVALID;
      return( status );
   }

   /* Set the current address to the start address so that start address can be
    * used to offset the flash image size. */
   currAddr = startAddr;

   *nSectors = 0; /* Clear the counter pointer so caller knows how many filled */

   while( currAddr <= startAddr + flashImageSize ) {
      /* Get the sector address where the current address resides*/
      currAddr = FLASH_addrToSectorAddr( currAddr );

      if ( 0 == currAddr ) {
         status = ERR_FLASH_SECTOR_ADDR_NOT_FOUND;
         ERR_printf(
               "Sector not found given addr 0x%08x.  Error: 0x%08x\n",
               currAddr, status
         );
      }
      DBG_printf("Addr at %d: 0x%08x\n", *nSectors, currAddr);
      sectorArrayLoc[(*nSectors)++] = currAddr;

      /* Get the address of the next sector */
      currAddr = FLASH_getNextSectorAddr(currAddr);
   }

   /* If we are doing application, make sure to also erase the very last sector
    * since we store information about the application image there. */
   if( _DC3_Application == flashImageLoc ) {
      sectorArrayLoc[(*nSectors)++] = FLASH_END_ADDR_SECTOR;
   }
   return( status );
}

/******************************************************************************/
const DC3Error_t FLASH_eraseSector( const uint32_t sectorAddr )
{
   DC3Error_t status = ERR_NONE;

   /* These flags have to be cleared before any operation can be done on the
    * flash memory */
   //	DBG_printf("Clearing flash flags. Status before clear: %x\n",FLASH->SR );
   FLASH_ClearFlag(
         FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
         FLASH_FLAG_WRPERR | FLASH_FLAG_OPERR | FLASH_FLAG_EOP
   );
   //	DBG_printf("Status after clear: %x\n",FLASH->SR );
   WRN_printf("*** Erasing Flash sector addr 0x%08x ***\n", sectorAddr);

   /*Look up the STM32 value of the sector given the sector address */
   uint16_t sector = FLASH_sectorAddrToFlashSect( sectorAddr );

   DBG_printf("Erasing sector (0x%04x) with base addr: 0x%08x...\n",
         sector, sectorAddr);
   /* This is a blocking operation but it's only for a single sector so should
    * not take that long */
   FLASH_Status flash_status = FLASH_EraseSector(sector, VoltageRange_3);
   status = FLASH_statusToErrorCode( flash_status );

   if (ERR_NONE != status) {
         ERR_printf("Flash error %d ( error 0x%08x) while erasing sector "
               "0x%04x (base addr: 0x%08x)\n",
               flash_status, status, sector, sectorAddr);
         DBG_printf("FLASH SR:%lx\n", FLASH->SR);
   }

   return( status );
}

/******************************************************************************/
const DC3Error_t FLASH_validateMetadata(
      const struct DC3FlashMetaPayloadMsg const *fwMetadata
)
{
   DC3Error_t status = ERR_NONE;
   if ( 0 == fwMetadata->_imageCrc || 0xFFFFFFF == fwMetadata->_imageCrc ) {
      status = ERR_FLASH_INVALID_IMAGE_CRC;
   } else if ( fwMetadata->_imageDatetime_len != DC3_DATETIME_LEN ) {
      ERR_printf("Datetime len: %d\n", fwMetadata->_imageDatetime_len);
      status = ERR_FLASH_INVALID_DATETIME_LEN;
   } else if ( fwMetadata->_imageDatetime[0] != '2' &&  fwMetadata->_imageDatetime[0] != '0' ) {
      status = ERR_FLASH_INVALID_DATETIME;
   } else if ( fwMetadata->_imageType == _DC3_Application ) {
      /* Make sure that the image is not bigger than the total available flash */
      if ( 0 == fwMetadata->_imageSize ||
            fwMetadata->_imageSize >= MAX_APPL_FWIMAGE_SIZE ) {
         status = ERR_FLASH_IMAGE_SIZE_INVALID;
      }
   } else {
      status = ERR_FLASH_IMAGE_TYPE_INVALID;
   }

   return status;
}

/******************************************************************************/
const DC3Error_t FLASH_writeBuffer(
      const uint32_t startAddr,
      const uint8_t *buffer,
      const uint16_t size,
      uint16_t *bytesWritten
)
{
   DC3Error_t status = ERR_NONE;
   *bytesWritten = 0;
   uint8_t data_read_back = 0;
   FLASH_Status flash_status;
   uint32_t addr = startAddr;

   /* These flags have to be cleared before any operation can be done on the
    * flash memory */
   FLASH_ClearFlag(
         FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
         FLASH_FLAG_WRPERR |	FLASH_FLAG_OPERR | FLASH_FLAG_EOP
   );

   for ( uint16_t i = 0; i < size; i++, addr++ ) {
      flash_status = FLASH_ProgramByte(addr, buffer[i]);
      if (FLASH_COMPLETE != flash_status) {
         status = FLASH_statusToErrorCode( flash_status );
         ERR_printf("Flash Error %d (error 0x%08x) writing %x to address %lx\n",
               flash_status, status, buffer[i], addr);
         return( status );
      } else {
         data_read_back = *((uint8_t *)addr);
         if (data_read_back != buffer[i]) {
            status = ERR_FLASH_READ_VERIFY_FAILED;
            ERR_printf("Failed to verify write at address %lx : Wrote %2x and "
                  "read back %2ux.  Error: 0x%08x\n",
                  addr, buffer[i], data_read_back, status);
            return( status );
         }
         *bytesWritten += 1; /* Increment the return value for bytes written */
      }
   }

   if (*bytesWritten != size) {
      status = ERR_FLASH_WRITE_INCOMPLETE;
      ERR_printf("Bytes written (%d) != number of bytes requested to be "
            "written (%d). Error: 0x%08x\n", bytesWritten, size, status);
   }

   return (status);
}

/******************************************************************************/
const uint32_t FLASH_readApplCRC( void )
{
   return( FLASH_readUint32( FLASH_APPL_CRC_ADDR ) );
}

/******************************************************************************/
const DC3Error_t FLASH_writeApplCRC( const uint32_t crc )
{
   return( FLASH_writeUint32( FLASH_APPL_CRC_ADDR, crc ) );
}

/******************************************************************************/
const uint32_t FLASH_readApplSize( void ) {
   return( FLASH_readUint32(FLASH_APPL_SIZE_ADDR) );
}

/******************************************************************************/
const DC3Error_t FLASH_writeApplSize( const uint32_t size )
{
   return( FLASH_writeUint32(FLASH_APPL_SIZE_ADDR, size) );
}

/******************************************************************************/
const uint8_t FLASH_readApplMajVer( void )
{
   return( FLASH_readUint8( FLASH_APPL_MAJ_VER_ADDR ) );
}

/******************************************************************************/
const DC3Error_t FLASH_writeApplMajVer( const uint8_t maj )
{
   return( FLASH_writeUint8( FLASH_APPL_MAJ_VER_ADDR, maj ) );
}

/******************************************************************************/
const uint8_t FLASH_readApplMinVer( void )
{
   return( FLASH_readUint8( FLASH_APPL_MIN_VER_ADDR ) );
}

/******************************************************************************/
const DC3Error_t FLASH_writeApplMinVer( const uint8_t maj )
{
   return( FLASH_writeUint8( FLASH_APPL_MIN_VER_ADDR, maj ) );
}

/******************************************************************************/
const DC3Error_t FLASH_readApplBuildDatetime(
      uint8_t *buffer,
      const uint8_t bufferSize
)
{
   if( NULL == buffer ) {
      return( ERR_MEM_NULL_VALUE );
   }

   if( bufferSize < FLASH_APPL_BUILD_DATETIME_LEN ) {
      return( ERR_MEM_BUFFER_LEN );
   }

   uint32_t addr = FLASH_APPL_BUILD_DATETIME_ADDR;
   for( uint8_t i = 0; i < FLASH_APPL_BUILD_DATETIME_LEN; i++ ) {
      buffer[i] = *((uint8_t *)addr);
   }
   return( ERR_NONE );
}

/******************************************************************************/
const DC3Error_t FLASH_writeApplBuildDatetime(
      const uint8_t *buffer,
      const uint8_t bufferSize
)
{
   if( NULL == buffer ) {
      return( ERR_MEM_NULL_VALUE );
   }

   if( bufferSize < FLASH_APPL_BUILD_DATETIME_LEN ) {
      return( ERR_MEM_BUFFER_LEN );
   }

   uint16_t bytesWritten = 0;
   DC3Error_t status = FLASH_writeBuffer(
         FLASH_APPL_BUILD_DATETIME_ADDR,
         buffer,
         FLASH_APPL_BUILD_DATETIME_LEN,
         &bytesWritten
   );

   return( status );
}

/******************************************************************************/
static const uint8_t FLASH_readUint8( const uint32_t addr )
{
   uint8_t data_read = 0;

   /* These flags have to be cleared before any operation can be done on the
    * flash memory */
//   FLASH_ClearFlag(
//         FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
//         FLASH_FLAG_WRPERR |  FLASH_FLAG_OPERR | FLASH_FLAG_EOP
//   );

   data_read = *((uint8_t *)addr);

   return(data_read);
}

/******************************************************************************/
static const DC3Error_t FLASH_writeUint8(
      const uint32_t addr,
      const uint8_t data
)
{
   uint8_t data_read = 0;
   FLASH_Status flash_status = FLASH_COMPLETE;
   DC3Error_t status = ERR_NONE;

   /* These flags have to be cleared before any operation can be done on the
    * flash memory */
   FLASH_ClearFlag(
         FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
         FLASH_FLAG_WRPERR |  FLASH_FLAG_OPERR | FLASH_FLAG_EOP
   );

   flash_status = FLASH_ProgramByte(addr, data);
   if (FLASH_COMPLETE != flash_status) {
      status = FLASH_statusToErrorCode( flash_status );
      ERR_printf("Flash Error %d writing 0x%02x to addr 0x%08x. Error: 0x%08x\n",
            flash_status, data, addr, status);
      return( status );
   } else {
      data_read = *((uint8_t *)addr);
      if (data_read != data) {
         status = ERR_FLASH_READ_VERIFY_FAILED;
         ERR_printf("Failed to verify write at addr 0x%08x : Wrote 0x%02x and "
               "read back 0x%02x. Error: 0x%08x\n", addr, data, data_read, status);
         return( status );
      }
   }

   return( status );
}

/******************************************************************************/
static const uint32_t FLASH_readUint32( const uint32_t addr )
{
   uint32_t data_read = 0;

   /* These flags have to be cleared before any operation can be done on the
    * flash memory */
//   FLASH_ClearFlag(
//         FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
//         FLASH_FLAG_WRPERR |	FLASH_FLAG_OPERR | FLASH_FLAG_EOP
//   );

   data_read = *((uint32_t *)addr);

   return(data_read);
}

/******************************************************************************/
static const DC3Error_t FLASH_writeUint32(
      const uint32_t addr,
      const uint32_t data
)
{
   uint32_t data_read = 0;
   FLASH_Status flash_status = FLASH_COMPLETE;
   DC3Error_t status = ERR_NONE;

   /* These flags have to be cleared before any operation can be done on the
    * flash memory */
   FLASH_ClearFlag(
         FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR |
         FLASH_FLAG_WRPERR |	FLASH_FLAG_OPERR | FLASH_FLAG_EOP
   );

   flash_status = FLASH_ProgramWord(addr, data);
   if (FLASH_COMPLETE != flash_status) {
      status = FLASH_statusToErrorCode( flash_status );
      ERR_printf("Flash Error %d writing %lx to addr %lx. Error: 0x%08x\n",
            flash_status, data, addr, status);
      return( status );
   } else {
      data_read = *((uint32_t *)addr);
      if (data_read != data) {
         status = ERR_FLASH_READ_VERIFY_FAILED;
         ERR_printf("Failed to verify write at addr %lx : Wrote %2lx and "
               "read back %2lx. Error: 0x%08x\n", addr, data, data_read, status);
         return( status );
      }
   }

   return( status );
}
