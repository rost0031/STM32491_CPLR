/**
 * @file    Menu.hpp
 * Implementation of the menu system for the client.
 *
 * @date    06/11/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MENU_HPP_
#define MENU_HPP_

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>
#include <string>
#include <vector>

/* App includes */

/* Lib includes */
#include "ClientApi.h"
#include "ClientErrorCodes.h"

/* Namespaces ----------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Loop that runs the menu until user exits.
 * @param [in] *client: ClientApi pointer to the client object.  This should be
 * instantiated outside and already set up for connection at this point.
 * @return ClientError_t:
 *    @arg CLI_ERR_NONE: if user ordered an exit
 *    other codes if errors occur in the client itself.
 */
ClientError_t MENU_run( ClientApi* client );

/**
 * @brief   Print help for the menu interface
 * @param   None
 * @return  None
 */
void MENU_printHelp( void );
/* Exported classes ----------------------------------------------------------*/



#endif                                                           /* MENU_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
