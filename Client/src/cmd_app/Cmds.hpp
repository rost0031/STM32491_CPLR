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
#include "CBCommApi.h"
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
 * @param [out] *mode: CBBootMode pointer where to store the retrieved mode
 * @return  ApiError_t:
 *    @arg API_ERR_NONE: if no error occurred
 *    @arg API_ERR_XXXX: other error codes indicating the error that occurred.
 */
APIError_t CMD_runGetMode(
      ClientApi* client,
      CBErrorCode* statusDC3,
      CBBootMode* mode
);

/* Exported classes ----------------------------------------------------------*/


#endif                                                           /* CMDS_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
