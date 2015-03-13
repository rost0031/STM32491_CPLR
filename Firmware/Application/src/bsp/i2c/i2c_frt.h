/**
 * @file    i2c_frt.h
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
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 *    @arg SN_ROM: SN RO EEPROM memory that contains 128 bit unique id
 *    @arg EUI_ROM: EUI RO EEPROM memory that contains 64 bit MAC address.
 * @param [in] offset: uint8_t offset from beginning of the device where to read
 *             the data from.  Lower level functions will calculate the
 *             appropriate memory addresses.
 * @param [out] *pBuffer: uint8_t pointer to the buffer to store read data.
 * @param [in] nBufferSize: size of storage pointed to by *pBuffer.  Used for
 *             error checking.
 * @param [out] *pBytesRead: number of bytes read as returned from the
 *             I2CxDevMgr AO that performs the actual read operation.
 * @param [in] bytesToRead: uint8_t variable specifying how many bytes to read
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_readDevMemFRT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint8_t *pBuffer,
      uint16_t nBufferSize,
      uint16_t *pBytesRead,
      uint16_t nBytesToRead
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
 * @param [in] iDev: I2C_Dev_t type specifying the I2C Device.
 *    @arg EEPROM: 256 bytes of main EEPROM memory
 * @param [in] offset: uint8_t offset from beginning of the device where to write
 *             the data to.  Lower level functions will calculate the
 *             appropriate memory addresses.
 * @param [in] *pBuffer: uint8_t pointer to the buffer containing data to write.
 * @param [in] nBufferSize: size of storage pointed to by *pBuffer.  Used for
 *             error checking.
 * @param [out] *pBytesWrite: number of bytes written as returned from the
 *             I2CxDevMgr AO that performs the actual write operation.
 * @param [in] bytesToWrite: uint8_t variable specifying how many bytes to write
 * @return CBErrorCode: status of the write operation
 *    @arg ERR_NONE: if no errors occurred
 */
CBErrorCode I2C_writeDevMemFRT(
      I2C_Dev_t iDev,
      uint16_t offset,
      uint8_t *pBuffer,
      uint16_t nBufferSize,
      uint16_t *pBytesWritten,
      uint16_t nBytesToWrite
);

/**
 * @}
 * end addtogroup groupI2C
 */

#ifdef __cplusplus
}
#endif

#endif                                                          /* I2C_FRT_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
