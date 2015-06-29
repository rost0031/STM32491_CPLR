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
#include <iostream>

/* App includes */
#include "Ktree.hpp"

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
 * @brief   Finalize the numbers of the menu nodes.
 * This recursive function should be called after all the menu items have been
 * added to the menu and if any menu items are ever removed from the menu. It
 * re-orders the numbers of the menu nodes so that there's a hierarchical number
 * system used for the tree.  IE:
 *
 * Root 0
 *   Node 1
 *     Node 11
 *     Node 12
 *       Node 121
 *       Node 122
 *     Node 13
 *     Node 14
 *       Node 141
 *       Node 142
 *  Node 2
 *    Node 21
 *    Node 22
 *    Node 23
 *       Node 231
 *       Node 232
 *    Node 24
 *    etc...
 *
 * @param [in] *node: Ktree pointer to root node of the menu
 * @param [in] number: unsigned int number from which to start numbering from
 * @note This should be 1 unless there's a particularly good reason otherwise
 * @return  None.
 */
void MENU_finalize( Ktree* node, unsigned int number );

/**
 * @brief   Print help for the menu interface
 * @param   None
 * @return  None
 */
void MENU_printHelp( void );

/**
 * @brief   Output passed in menu node.
 */
void MENU_printMenuLevel( Ktree* node );

void MENU_printMenuExpAtCurrNode( Ktree* node, Ktree* root );

void MENU_nodeAncestryToStream( Ktree* node, KtreeNav* kNav, std::stringstream& ss);

void MENU_nodeAncestryToStream(
      Ktree* node,
      std::vector<Ktree*>::iterator& ancestry_it,
      std::stringstream& ss
);
/* Exported classes ----------------------------------------------------------*/



#endif                                                           /* MENU_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
