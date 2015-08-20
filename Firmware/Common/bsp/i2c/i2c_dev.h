/**
 * @file    i2c_dev.h
 * @brief   Data for devices on the I2C Busses.
 *
 * @date    10/28/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_DEV_H_
#define I2C_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "bsp_defs.h"
#include "i2c_defs.h"
#include "Shared.h"
#include "stm32f4xx_i2c.h"                           /* For STM32 DMA support */

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/**
 * @brief   Macro to determine if an I2C operation is defined in the system
 * @param [in] OP:  I2C_Operation_t type I2C operation specifier.
 * @retval
 *    1: Device operation and is valid
 *    0: Device operation doesn't exist or isn't valid
 */
#define IS_I2C_OP( OP )                                                       \
(                                                                             \
   (OP) == I2C_OP_MEM_READ  ||                                                \
   (OP) == I2C_OP_MEM_WRITE ||                                                \
   (OP) == I2C_OP_REG_READ  ||                                                \
   (OP) == I2C_OP_REG_READ                                                    \
)

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Get the device address size for the specified I2CBus1 device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint8_t device address size of the requested device.
 */
const uint8_t I2C_getDevAddrSize( const DC3I2CDevice_t iDev );

/**
 * @brief   Get the device address for the specified I2CBus1 device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t device address on the I2C Bus.
 */
const uint16_t I2C_getDevAddr( const DC3I2CDevice_t iDev );

/**
 * @brief   Get the internal memory address size for the specified I2CBus1 device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint8_t internal memory address size of the requested device.
 */
const uint8_t I2C_getMemAddrSize( const DC3I2CDevice_t iDev );

/**
 * @brief   Get the last accessed internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory address on the I2C Device.
 */
const uint16_t I2C_getMemAddr( const DC3I2CDevice_t iDev );

/**
 * @brief   Set the last accessed internal memory address for the specified
 * I2CBus1 device.
 *
 * This is to keep track of where the last access left off so continuous reads
 * don't have to keep telling the device where to read from.  Saves 2 I2C bus
 * operations per access.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @param [in]  addr: uint16_t last accessed memory address.
 * @return: None
 */
void I2C_setI2CMemAddr( const DC3I2CDevice_t iDev, const uint16_t addr );

/**
 * @brief   Get the first accessible internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t first memory address on the I2C device.
 */
const uint16_t I2C_getMinMemAddr( const DC3I2CDevice_t iDev );

/**
 * @brief   Get the last accessible internal memory address for the specified
 * I2CBus1 device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t last memory address on the I2C device.
 */
const uint16_t I2C_getMaxMemAddr( const DC3I2CDevice_t iDev );

/**
 * @brief   Get the page size for memory for the specified I2C device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory page size on the I2C device.
 */
const uint8_t I2C_getPageSize( const DC3I2CDevice_t iDev );

/**
 * @brief   Get the I2C bus of specified I2C device.
 *
 * @param [in]  iDev: DC3I2CDevice_t identifier to choose device
 *    @arg EEPROM: get the EEPROM device address
 *    @arg SN_ROM: get the SN_ROM device address
 *    @arg UIE_ROM: get the UIE_ROM device address
 * @return: uint16_t memory page size on the I2C device.
 */
const I2C_Bus_t I2C_getBus( const DC3I2CDevice_t iDev );

/**
 * @brief   Calculate how to properly write large data over page boundaries in EEPROM.
 *
 * @param [out] *writeSizeFirstPage: uint8_t pointer to how many bytes to write
 *              on the first page of the EEPROM.
 * @param [out] *writeSizeLastPage: uint8_t pointer to how many bytes to write
 *              on the last page of the EEPROM.
 * @param [out] *writeTotalPages: uint8_t pointer to how many total pages will
 *              be written to the EEPROM.
 * @param [in] i2cMemAddr: internal memory address of the device on the I2C bus.
 *    @arg 1: a 1 byte address.
 *    @arg 2: a 2 byte address.
 *    No other sizes will be handled.
 * @param [in] bytesToWrite: uint8_t variable specifying how many bytes to write
 * @param [in] pageSize: uint16_t specifying the size of the page of the target
 * device.
 * @return DC3Error_t: status of the write operation
 *    @arg ERR_NONE: if no errors occurred
 */
const DC3Error_t I2C_calcPageWriteSizes(
      uint8_t* const writeSizeFirstPage,
      uint8_t* const writeSizeLastPage,
      uint8_t* const writeTotalPages,
      const uint16_t i2cMemAddr,
      const uint16_t bytesToWrite,
      const uint16_t pageSize
);

/**
 * @brief  Common function to do a read of an I2C device using any access.
 *
 * This function is the common entry point to perform a read of an I2C device
 * using any accessType (BARE, QPC, or FRT).
 *
 * @note:  This function needs to be wrapped in FRT function to listen for the
 * appropriate event that the read is finished.
 *
 * @param  [in] accessType: const DC3AccessType_t that specifies how the
 * function is being accessed.
 *    @arg _DC3_ACCESS_BARE: blocking access that is slow.  Don't use once the
 *                           RTOS is running.
 *    @arg _DC3_ACCESS_QPC:  non-blocking, event based access.
 *    @arg _DC3_ACCESS_FRT:  non-blocking, but waits on queue to know the status.
 *
 * @param [in] iDev: DC3I2CDevice_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 *    @arg SN_ROM: SN RO EEPROM memory that contains 128 bit unique id
 *    @arg EUI_ROM: EUI RO EEPROM memory that contains 64 bit MAC address.
 *
 * @param [in] offset: offset from the beginning of the device where the read
 * should start.  The actual address is figured out internally so the device
 * memory map doesn't need to be exposed to the caller.
 *
 * @param [in] bytesToRead : const uint16_t variable specifying how many bytes
 * to read from the device.
 *
 * @param [in] bufferSize: uint16_t maximum size of storage pointed to by
 * *pBuffer pointer.
 *
 * @param [in|out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * This should only be accessed if the returned status is ERR_NONE.
 *
 * @param [out] *pBytesRead: uint16_t pointer specifying how many bytes were
 * actually read. This should only be accessed if the returned status is
 * ERR_NONE.

 * @return DC3Error_t: status of the read request operation
 *    @arg ERR_NONE: if no errors occurred
 */
const DC3Error_t I2C_readDevMem(
      const DC3AccessType_t accessType,
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      const uint16_t bytesToRead,
      const uint16_t bufferSize,
      uint8_t* const pBuffer,
      uint16_t* pBytesRead
);

/**
 * @brief  Common function to do a write of an I2C device using any access.
 *
 * This function is the common entry point to perform a write of an I2C device
 * using any accessType (BARE, QPC, or FRT).
 *
 * @note:  This function needs to be wrapped in FRT function to listen for the
 * appropriate event that the read is finished.
 *
 * @param  [in] accessType: const DC3AccessType_t that specifies how the
 * function is being accessed.
 *    @arg _DC3_ACCESS_BARE: blocking access that is slow.  Don't use once the
 *                           RTOS is running.
 *    @arg _DC3_ACCESS_QPC:  non-blocking, event based access.
 *    @arg _DC3_ACCESS_FRT:  non-blocking, but waits on queue to know the status.
 *
 * @param [in] iDev: DC3I2CDevice_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 *
 * @param [in] offset: offset from the beginning of the device where the write
 * should start.  The actual address is figured out internally so the device
 * memory map doesn't need to be exposed to the caller.
 *
 * @param [in] bytesToWrite : const uint16_t variable specifying how many bytes
 * to write to the device.
 *
 * @param [in] bufferSize: uint16_t maximum size of storage pointed to by
 * *pBuffer pointer.
 *
 * @param [in|out] *pBuffer: uint8_t pointer to the buffer containing data to be
 * written.
 *
 * @param [out] *pBytesWritten: uint16_t pointer specifying how many bytes were
 * actually written. This should only be accessed if the returned status is
 * ERR_NONE.
 *
 * @return DC3Error_t: status of the write request operation
 *    @arg ERR_NONE: if no errors occurred
 */
const DC3Error_t I2C_writeDevMem(
      const DC3AccessType_t accessType,
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      const uint16_t bytesToWrite,
      const uint16_t bufferSize,
      const uint8_t* const pBuffer,
      uint16_t* pBytesWritten
);

/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                          /* I2C_DEV_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
