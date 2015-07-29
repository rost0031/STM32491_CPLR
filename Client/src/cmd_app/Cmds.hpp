/**
 * @file    Cmds.hpp
 * Various commands that can be sent from the cmd client
 *
 * @date    07/02/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CMDS_HPP_
#define CMDS_HPP_

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>

/* App includes */
#include "Logging.hpp"
#include "Help.hpp"
#include "EnumMaps.hpp"
#include "DC3CommApi.h"
#include "ClientApi.h"

/* Namespaces ----------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Wrapper around the UI for RAM test
 *
 * @param [in] *client: ClientApi pointer to the api object to provide access to
 * the DC3
 * @return  ApiError_t:
 *    @arg API_ERR_NONE: if no error occurred
 *    @arg API_ERR_XXXX: other error codes indicating the error that occurred.
 */
APIError_t CMD_runRamTest(  ClientApi* client );

/**
 * @brief   Wrapper around the UI for get_mode command
 *
 * @param [in] *client: ClientApi pointer to the API object to provide access to
 * the DC3
 * @param [out] *statusDC3: DC3Error_t status returned from DC3.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @param [out] *mode: DC3BootMode_t pointer where to store the retrieved mode
 * @return  ApiError_t:
 *    @arg API_ERR_NONE: if no error occurred
 *    @arg API_ERR_XXXX: other error codes indicating the error that occurred.
 */
APIError_t CMD_runGetMode(
      ClientApi* client,
      DC3Error_t* statusDC3,
      DC3BootMode_t* mode
);

/**
 * @brief   Wrapper around the UI for set_mode command
 *
 * @param [in] *client: ClientApi pointer to the API object to provide access to
 * the DC3
 * @param [out] *statusDC3: DC3Error_t status returned from DC3.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @param [in] mode: DC3BootMode_t to set the DC3 mode to.
 * @return  ApiError_t:
 *    @arg API_ERR_NONE: if no error occurred
 *    @arg API_ERR_XXXX: other error codes indicating the error that occurred.
 */
APIError_t CMD_runSetMode(
      ClientApi* client,
      DC3Error_t*
      statusDC3,
      DC3BootMode_t mode
);

/**
 * @brief   Wrapper around the UI for flash command
 *
 * @param [in] *client: ClientApi pointer to the API object to provide access to
 * the DC3
 * @param [out] *statusDC3: DC3Error_t status returned from DC3.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @param [in] type: DC3BootMode_t type of fw image
 * @param [in] file: string that contains a verified path to the FW image file.
 * @return  ApiError_t:
 *    @arg API_ERR_NONE: if no error occurred
 *    @arg API_ERR_XXXX: other error codes indicating the error that occurred.
 */
APIError_t CMD_runFlash(
      ClientApi* client,
      DC3Error_t* statusDC3,
      DC3BootMode_t type,
      const string& file
);

/**
 * @brief   Wrapper around the UI for read_i2c command
 *
 * @param [in] *client: ClientApi pointer to the API object to provide access
 * to the DC3
 * @param [out] *statusDC3: DC3Error_t status returned from DC3.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @note: unless this variable is set to ERR_NONE at the completion, the
 * results of other returned data should not be trusted.
 * @param [out] *pBytesRead: pointer to number of bytes read from I2C device.
 * @param [out] *pBuffer: pointer to buffer where data will be stored.
 * @param [in] nMaxBufferSize: size_t size of *pBuffer storage area.
 * @param [in] nBytesToRead: number of bytes to read
 * @param [in] nStart: where to start reading from
 * @param [in] dev: DC3I2CDevice_t type that specifies the I2C device to read
 *    @arg _DC3_EEPROM: read from the EEPROM on I2C bus 1.
 *    @arg _DC3_SNROM: read from the RO SerialNumber ROM on I2C bus 1.
 *    @arg _DC3_EUIROM: read from the RO Unique number ROM on I2C bus 1.
 * @param [in] acc: DC3AccessType_t  type that specifies the access to use to get
 * at the I2C bus.
 *    @arg _DC3_ACCESS_BARE: bare metal access. Blocking and slow. For testing only.
 *    @arg _DC3_ACCESS_QPC: use event driven QPC access.
 *    @arg _DC3_ACCESS_FRT: use event driven FreeRTOS access.  Available in
 *    Application only.
 *
 * @return: APIError_t status of the client executing the command.
 *    @arg  API_ERR_NONE: success
 *    other error codes if failures.
 */
APIError_t CMD_runReadI2C(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint16_t* pBytesRead,
      uint8_t* pBuffer,
      const size_t nMaxBufferSize,
      const size_t nBytesToRead,
      const size_t nStart,
      const DC3I2CDevice_t dev,
      const DC3AccessType_t  acc
);

/**
 * @brief   Wrapper around the UI for write_i2c command
 *
 * @param [in] *client: ClientApi pointer to the API object to provide access
 * to the DC3
 * @param [out] *statusDC3: DC3Error_t status returned from DC3.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @note: unless this variable is set to ERR_NONE at the completion, the
 * results of other returned data should not be trusted.
 *
 * @param [in] *pBuffer: pointer to data to write.
 * @param [in] nBytesToWrite: number of bytes to write
 * @param [in] nStart: where to start reading from
 * @param [in] dev: DC3I2CDevice_t type that specifies the I2C device to read
 *    @arg _DC3_EEPROM: read from the EEPROM on I2C bus 1.
 *    @arg _DC3_SNROM: read from the RO SerialNumber ROM on I2C bus 1.
 *    @arg _DC3_EUIROM: read from the RO Unique number ROM on I2C bus 1.
 * @param [in] acc: DC3AccessType_t  type that specifies the access to use to get
 * at the I2C bus.
 *    @arg _DC3_ACCESS_BARE: bare metal access. Blocking and slow. For testing only.
 *    @arg _DC3_ACCESS_QPC: use event driven QPC access.
 *    @arg _DC3_ACCESS_FRT: use event driven FreeRTOS access.  Available in
 *    Application only.
 *
 * @return: APIError_t status of the client executing the command.
 *    @arg  API_ERR_NONE: success
 *    other error codes if failures.
 */
APIError_t CMD_runWriteI2C(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint8_t* pBuffer,
      const size_t nBytesToWrite,
      const size_t nStart,
      const DC3I2CDevice_t dev,
      const DC3AccessType_t  acc
);
/* Exported classes ----------------------------------------------------------*/


#endif                                                           /* CMDS_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
