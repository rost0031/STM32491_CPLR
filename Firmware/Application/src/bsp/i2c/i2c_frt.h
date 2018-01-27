/**
 * @file    i2c_frt.h
 * @brief   I2C Device interface for FreeRTOS.
 *
 * @date    03/13/2015
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupI2C
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_FRT_H_
#define I2C_FRT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "i2c_defs.h"
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   A blocking function to read I2C data that should be called from
 * FreeRTOS threads.
 *
 * This function sends and event to the I2CxDevMgr AO to read data from an I2C
 * device and then suspends the calling task.  Once the AO is finished processing
 * the request, it will put an event with the data (or error) into the raw queue
 * and wake the task.  This function then resumes and retrieves the data out of
 * the queue, parses it, fills in the appropriate buffers and status and returns.
 *
 * @param [in] iDev: DC3I2CDevice_t type specifying the I2C Device.
 *    @arg _DC3_EEPROM: 256 bytes of main EEPROM memory
 *    @arg _DC3_SNROM: SN RO EEPROM memory that contains 128 bit unique id
 *    @arg _DC3_EUIROM: EUI RO EEPROM memory that contains 64 bit MAC address.
 * @param [in] offset: uint8_t offset from beginning of the device where to read
 *             the data from.  Lower level functions will calculate the
 *             appropriate memory addresses.
 * @param [in] bytesToRead: uint8_t variable specifying how many bytes to read
 * @param [in] nBufferSize: size of storage pointed to by *pBuffer.  Used for
 *             error checking.
 * @param [out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @param [out] *pBytesRead: number of bytes read as returned from the
 *             I2CxDevMgr AO that performs the actual read operation.
 * @return DC3Error_t: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
const DC3Error_t I2C_readDevMemFRT(
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      const uint16_t nBytesToRead,
      const uint16_t nBufferSize,
      uint8_t* const pBuffer,
      uint16_t* pBytesRead
);

/**
 * @brief   A blocking function to write I2C data that should be called from
 * FreeRTOS threads.
 *
 * This function sends and event to the I2CxDevMgr AO to write data to an I2C
 * device and then suspends the calling task.  Once the AO is finished processing
 * the request, it will put an event with the data (or error) into the raw queue
 * and wake the task.  This function then resumes and retrieves the data out of
 * the queue, parses it, fills in the appropriate buffers and status and returns.
 *
 * @param [in] iDev: DC3I2CDevice_t type specifying the I2C Device.
 *    @arg _DC3_EEPROM: 256 bytes of main EEPROM memory
 * @param [in] offset: uint8_t offset from beginning of the device where to write
 *             the data to.  Lower level functions will calculate the
 *             appropriate memory addresses.
 * @param [in] bytesToWrite: uint8_t variable specifying how many bytes to write
 * @param [in] nBufferSize: size of storage pointed to by *pBuffer.  Used for
 *             error checking.
 * @param [in] *pBuffer: uint8_t pointer to the buffer containing data to write.
 * @param [out] *pBytesWritten: number of bytes written as returned from the
 *             I2CxDevMgr AO that performs the actual write operation.
 * @return DC3Error_t: status of the write operation
 *    @arg ERR_NONE: if no errors occurred
 */
const DC3Error_t I2C_writeDevMemFRT(
      const DC3I2CDevice_t iDev,
      const uint16_t offset,
      const uint16_t nBytesToWrite,
      const uint16_t nBufferSize,
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

#endif                                                          /* I2C_FRT_H_ */
/******** Copyright (C) 2015 Harry Rostovtsev. All rights reserved *****END OF FILE****/
