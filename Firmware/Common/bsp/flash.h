/**
 * @file    flash.c
 * @brief   Contains declarations of the flash memory manipulation functions
 * specific to DC3 (STM32F4 with 2MB of flash)
 *
 * @date    04/29/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASH_H_
#define FLASH_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_flash.h"
#include "Shared.h"
#include "project_includes.h"
//#include "flash_info.h"
#include "DC3CommApi.h"

/* Exported defines ----------------------------------------------------------*/
/* These defines specify where the various flash regions start and end on the
 * STM32F4xx 2MB flash chip that is being used by the DC3 */
#define FLASH_BOOT_START_ADDR             ((uint32_t)0x08000000)
#define FLASH_BOOT_LAST_ADDR              ((uint32_t)0x0803FFFF)
#define FLASH_LAST_ADDR                   ((uint32_t)0x081FFFFF)

#define FLASH_END_ADDR_SECTOR             ADDR_FLASH_SECTOR_23  /**< End address of user Flash area */

/**
 * @brief   Size of the Application FW image
 */
#define FLASH_APPL_START_ADDR             ((uint32_t)0x08040000)

#define FLASH_APPL_MAJ_VER_ADDR           ((uint32_t)0x081FFFE8)
#define FLASH_APPL_MAJ_VER_LEN            1

#define FLASH_APPL_MIN_VER_ADDR           ((uint32_t)0x081FFFEA)
#define FLASH_APPL_MIN_VER_LEN            1

#define FLASH_APPL_BUILD_DATETIME_ADDR    ((uint32_t)0x081FFFEC)
#define FLASH_APPL_BUILD_DATETIME_LEN     DC3_DATETIME_LEN

#define FLASH_APPL_SIZE_ADDR              ((uint32_t)0x081FFFE0)
#define FLASH_APPL_SIZE_LEN               4

#define FLASH_APPL_CRC_ADDR               ((uint32_t)0x081FFFE4)
#define FLASH_APPL_CRC_LEN                4

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /**< Addr of Sect 0,  16  KB */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /**< Addr of Sect 1,  16  KB */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /**< Addr of Sect 2,  16  KB */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /**< Addr of Sect 3,  16  KB */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /**< Addr of Sect 4,  64  KB */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /**< Addr of Sect 5,  128 KB */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /**< Addr of Sect 6,  128 KB */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /**< Addr of Sect 7,  128 KB */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /**< Addr of Sect 8,  128 KB */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /**< Addr of Sect 9,  128 KB */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /**< Addr of Sect 10, 128 KB */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /**< Addr of Sect 11, 128 KB */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x08100000) /**< Addr of Sect 12, 16  KB */
#define ADDR_FLASH_SECTOR_13    ((uint32_t)0x08104000) /**< Addr of Sect 13, 16  KB */
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08108000) /**< Addr of Sect 14, 16  KB */
#define ADDR_FLASH_SECTOR_15    ((uint32_t)0x0810C000) /**< Addr of Sect 15, 16  KB */
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x08110000) /**< Addr of Sect 16, 64  KB */
#define ADDR_FLASH_SECTOR_17    ((uint32_t)0x08120000) /**< Addr of Sect 17, 128 KB */
#define ADDR_FLASH_SECTOR_18    ((uint32_t)0x08140000) /**< Addr of Sect 18, 128 KB */
#define ADDR_FLASH_SECTOR_19    ((uint32_t)0x08160000) /**< Addr of Sect 19, 128 KB */
#define ADDR_FLASH_SECTOR_20    ((uint32_t)0x08180000) /**< Addr of Sect 20, 128 KB */
#define ADDR_FLASH_SECTOR_21    ((uint32_t)0x081A0000) /**< Addr of Sect 21, 128 KB */
#define ADDR_FLASH_SECTOR_22    ((uint32_t)0x081C0000) /**< Addr of Sect 22, 128 KB */
#define ADDR_FLASH_SECTOR_23    ((uint32_t)0x081E0000) /**< Addr of Sect 23, 128 KB */

#define ADDR_FLASH_SECTORS       24 /**< Number of sectors in the flash */
#define MAX_APPL_FWIMAGE_SIZE    0x0001AFFFF /**< Max size of fw image (~1.76 MB) */

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Find and fill array of sector base addresses.
 *
 * This is a helper function that looks up and fills the array of sector base
 * addresses based on which FW image type and its size.  It takes in a pointer
 * to an array and fills it with sector base addresses,
 *
 * @param [out] *sectorArrayLoc: uint32_t pointer to the array to fill with
 * sector base addresses that are found to erase.
 * @param [out] *nSectors: uint8_t pointer to how many sector base addresses are
 * found.
 * @param [in] sectorArraySize: const uint8_t size of sectorArrayLoc buffer.
 * This is for sanity checking.
 * @param [in] flashImageLoc: const DC3BootMode_t that specifies which flash area
 * to find sectors to erase for.
 * @param [in] flashImageSize: const uint32_t that specifies the size of the FW
 * image.
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_getSectorsToErase(
      uint32_t *sectorArrayLoc,
      uint8_t *nSectors,
      const uint8_t sectorArraySize,
      const DC3BootMode_t flashImageLoc,
      const uint32_t flashImageSize
);

/**
 * @brief   Erase flash section reserved for the specified FW image.
 * @param [in] sectorAddr: const uint32_t sector base address that specifies
 * which sector to erase.
 * @return	DC3Error_t status:
 *    @arg  ERR_NONE: success
 * 	@arg  other error codes if error occurred
 */
const DC3Error_t FLASH_eraseSector( const uint32_t sectorAddr );

/**
 * @brief   Validate FW image metadata.
 * @param [in] *fwMetadata: const const struct DC3FlashMetaPayloadMsg const pointer
 * to the fw image metadata.
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_validateMetadata(
      const struct DC3FlashMetaPayloadMsg const *fwMetadata
);

/**
 * @brief   Writes a buffer of "size" bytes starting from the "startAddr".
 * @param [in] startAddr: const uint32_t address at which to start write.
 * @param [in] *buffer: const char pointer to the buffer containing data to
 * write
 * @param [in] size: const uint16_t number of bytes in the buffer that will be
 * written to flash.
 * @param [out] *bytesWritten: uint16_t pointer to how many bytes were actually
 * written.
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_writeBuffer(
      const uint32_t startAddr,
      const uint8_t *buffer,
      const uint16_t size,
      uint16_t *bytesWritten
);

/**
 * @brief   Read the CRC of the Application FW image stored at the very end of
 * the Application flash section.
 * @param	None
 * @return	crc: uint32_t CRC stored at that address location
 */
const uint32_t FLASH_readApplCRC( void );

/**
 * @brief   Write the application CRC to the very end of the Application flash.
 * @param	crc: uint32_t crc that is to be written
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_writeApplCRC( const uint32_t crc );

/**
 * @brief   Read the size stored at the very end of the Application flash.
 * @param   None
 * @return	size: uint32_t application size stored at that address location
 */
const uint32_t FLASH_readApplSize( void );

/**
 * @brief   Write the application size to the very end of the Application flash.
 * @param [in] size: const uint32_t size that is to be written
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_writeApplSize( const uint32_t size );

/**
 * @brief   Read the major version stored at the very end of the Application flash.
 * @param   None
 * @return  maj: uint8_t application major version stored at that address location
 */
const uint8_t FLASH_readApplMajVer( void );

/**
 * @brief   Write the application major version to the very end of the
 * Application flash.
 * @param [in] maj: const uint8_t major version that is to be written
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_writeApplMajVer( const uint8_t maj );

/**
 * @brief   Read the minor version stored at the very end of the Application flash.
 * @param   None
 * @return  maj: uint8_t application minor version stored at that address location
 */
const uint8_t FLASH_readApplMinVer( void );

/**
 * @brief   Write the application minor version to the very end of the
 * Application flash.
 * @param [in] maj: const uint8_t minor version that is to be written
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_writeApplMinVer( const uint8_t maj );

/**
 * @brief   Read the build datetime stored at the very end of the Application flash.
 * @param [out] *buffer: uint8_t pointer to buffer where the data will be stored
 * on return.
 * @param [in] bufferSize: const uint8_t size of the buffer.
 *    @note: This buffer should be at minimum 14 (FLASH_APPL_BUILD_DATETIME_LEN)
 *    bytes.
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_readApplBuildDatetime(
      uint8_t *buffer,
      const uint8_t bufferSize
);

/**
 * @brief   Write the build datetime stored at the very end of the Application flash.
 * @param [out] *buffer: const uint8_t pointer to buffer where the build
 * datetime resides.
 * @param [in] bufferSize: const uint8_t size of the buffer.
 *    @note: This buffer should be at minimum 14 (FLASH_APPL_BUILD_DATETIME_LEN)
 *    bytes.
 * @return  DC3Error_t status:
 *    @arg  ERR_NONE: success
 *    @arg  other error codes if error occurred
 */
const DC3Error_t FLASH_writeApplBuildDatetime(
      const uint8_t *buffer,
      const uint8_t bufferSize
);

#endif                                                            /* FLASH_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
