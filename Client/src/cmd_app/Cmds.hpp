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
 * @param [out] *statusDC3: DC3Error_t status returned from DC3.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @return  ApiError_t:
 *    @arg API_ERR_NONE: if no error occurred
 *    @arg API_ERR_XXXX: other error codes indicating the error that occurred.
 */
APIError_t CMD_runRamTest(  ClientApi* client, DC3Error_t* statusDC3 );

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

/**
 * @brief   Wrapper around the UI for get_dbg_modules command
 *
 * @param [in] *client: ClientApi pointer to the API object to provide access
 * to the DC3
 * @param [out] *statusDC3: DC3Error_t status returned from DC3.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @note: unless this variable is set to ERR_NONE at the completion, the
 * results of other returned data should not be trusted.
 * @param [out] *dbgModules: pointer to where to store the bitfield indicating
 * the state of the dbg modules enable/disable states.
 *
 * @return: APIError_t status of the client executing the command.
 *    @arg  API_ERR_NONE: success
 *    other error codes if failures.
 */
APIError_t CMD_runGetDbgModules(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint32_t* dbgModules
);

/**
 * @brief   Wrapper around the UI for set_dbg_modules command.
 *
 * This function allows to both enable and disable debugging for individual
 * modules.  This is controlled by the bEnable flag, where:
 *    @arg true:  enable
 *    @arg false: disable
 *
 * as well as controlling individual debug modules vs replacing the
 * entire bitfield set. This is controlled by bOverWrite flag, where:
 *    @arg false: leave the other debug modules as they are and only change
 *                the passed in field
 *    @arg true:  overwrite the entire debug bitfield with the passed in
 *                value composed of ORed DC3DbgModule_t fields.
 *
 * @note: the bEnable flag is ignored if bOverWrite is set to true since this
 * option just sets the bitfield to whatever is passed in.
 *
 * @param [out] *status: DC3Error_t pointer to the returned status of from
 * the DC3 board.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @note: unless this variable is set to ERR_NONE at the completion, the
 * results of other returned data should not be trusted.
 *
 * @param [in|out] dbgModuleSet: uint32_t pointer specifying which module to
 * enable/disable (in) and the state of all dbg modules (out) upon return.
 *
 * @param [in] bEnable: bool flag that controls whether to enable or disable
 * the passed in bitfield.
 *    @arg true:  enable
 *    @arg false: disable
 *
 * @param [in] bOverWrite: bool flag that controls whether individual debug
 * module is enabled/disabled vs replacing the entire bitfield.
 *    @arg false: leave the other debug modules as they are and only change
 *                the passed in field
 *    @arg true:  overwrite the entire debug bitfield with the passed in
 *                value composed of ORed DC3DbgModule_t fields.
 *
 * @return: APIError_t status of the client executing the command.
 *    @arg  API_ERR_NONE: success
 *    other error codes if failure.
 */
APIError_t CMD_runSetDbgModule(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint32_t* dbgModuleSet,
      bool bEnable,
      bool bOverWrite
);

/**
 * @brief   Wrapper around the UI for set_dbg_device command.
 *
 * This function allows to  enable and disable debugging over serial and ethernet
 * This is controlled by the bEnable flag, where:
 *    @arg true:  enable
 *    @arg false: disable
 *
 * @param [out] *status: DC3Error_t pointer to the returned status of from
 * the DC3 board.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @note: unless this variable is set to ERR_NONE at the completion, the
 * results of other returned data should not be trusted.
 *
 * @param [in] device: DC3MsgRoute_t specifying which device to enable/disable
 * debugging output on.
 *
 * @param [in] bEnable: bool flag that controls whether to enable or disable
 * the debugging output on the specified device
 *    @arg true:  enable
 *    @arg false: disable
 *
 * @return: APIError_t status of the client executing the command.
 *    @arg  API_ERR_NONE: success
 *    other error codes if failure.
 */
APIError_t CMD_runSetDbgDevice(
      ClientApi* client,
      DC3Error_t* statusDC3,
      DC3MsgRoute_t device,
      bool bEnable
);

/**
 * @brief   Wrapper around the UI for reset_db command.
 *
 * @param [out] *status: DC3Error_t pointer to the returned status of from
 * the DC3 board.
 *    @arg  ERR_NONE: success.
 *    other error codes if failure.
 * @note: unless this variable is set to ERR_NONE at the completion, the
 * results of other returned data should not be trusted.
 *
 * @return: APIError_t status of the client executing the command.
 *    @arg  API_ERR_NONE: success
 *    other error codes if failure.
 */
APIError_t CMD_runResetDB(
      ClientApi* client,
      DC3Error_t* statusDC3
);
/* Exported classes ----------------------------------------------------------*/


#endif                                                           /* CMDS_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
