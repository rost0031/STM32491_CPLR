/**
 * @file    Menu.cpp
 * Implementation of the menu system for the client.
 *
 * @date    06/11/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <iomanip>
#include <iostream>

/* Boost includes */
#include <boost/algorithm/string.hpp>

/* App includes */
#include "Menu.hpp"
#include "Logging.hpp"
#include "EnumMaps.hpp"
#include "KTree.hpp"

/* Lib includes */

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
ClientError_t MENU_run( ClientApi *client )
{
   Ktree *currMenuNode = NULL;

   string input;
   unsigned int menuNumber = 0;
   Ktree *root = new Ktree( "top", "Menu", menuNumber++ );
   currMenuNode = root;

   root->addChild( "DBG", "Debug control", menuNumber++ );
   root->addChild( "SYS", "System tests", menuNumber++ );

   root->findChild("SYS")->addChild( "I2C", "I2C tests", menuNumber++);
   root->findChild("SYS")->findChild("I2C")->addChild( "REE", "Read EEPROM on I2C", menuNumber++);
   root->findChild("SYS")->findChild("I2C")->addChild( "WEE", "Write EEPROM on I2C", menuNumber++);

   root->findChild("SYS")->addChild( "MEM", "Memory tests", menuNumber++);
   root->findChild("SYS")->findChild("MEM")->addChild("RAM", "Test RAM", menuNumber++);
   root->findChild("SYS")->findChild("MEM")->addChild("RAM1", "Test RAM again", menuNumber++);

//   root->printNode(0);
//   dbgMenu->printNode(1);
//   sysTestMenu->printNode(1);

//   root->deleteChild("DBG");

//   root->printNode(0);

   root->printTree();
   MENU_printHelp();
   while ( 1 ) {

      CON_print("Enter a command: ");
      cin >> input; // Read input again at the end

      if( boost::iequals(input, "exit") || boost::iequals(input, "quit") ||
          boost::iequals(input, "x") || boost::iequals(input, "q")  ) {
         CON_print("Leaving the menu...");
         return CLI_ERR_NONE;
      } else if( boost::iequals(input, "help") || boost::iequals(input, "?") ||
                 boost::iequals(input, "h") ) {
         MENU_printHelp();
      } else if( boost::iequals(input, "up") || boost::iequals(input, "u") ) {
         // go up a level
      } else if( boost::iequals(input, "top") || boost::iequals(input, "t") ) {
         // go to the top of the tree
      } else if( boost::iequals(input, "all") || boost::iequals(input, "a") ) {
            root->printTree();                         // print entire menu tree
      } else if( boost::iequals(input, "print") || boost::iequals(input, "p") ) {
         // print current menu level
      }

   }

   return CLI_ERR_NONE;
}

/******************************************************************************/
void MENU_printHelp( void )
{
   stringstream ss;
   ss << "********************************************************************************" << endl
      << "*****                              Menu Help                               *****" << endl
      << "*****         Top level commands: (work anywhere in the menu)              *****" << endl
      << "***** 'h', 'H', '?' or 'help' : print this menu help                       *****" << endl
      << "***** 'p', 'P', or 'print'    : print the current menu level               *****" << endl
      << "***** 'a', 'A', or 'all'      : print the entire menu tree                 *****" << endl
      << "***** 't', 'T', or 'top'      : go to the top of the menu tree             *****" << endl
      << "***** 'u', 'U', or 'up'       : go up a level from the current submenu     *****" << endl
      << "***** any identifier to select the submenu or menu item on your level      *****" << endl
      << "********************************************************************************" << endl;
   CON_print(ss.str());
}

/******************************************************************************/
void MENU_currLevelToStream( stringstream& ss, Ktree* node )
{
   Ktree *parent = NULL;
   if( NULL != (parent = node->getParent()) ) {
      parent->childrenToStream( ss );
   }

}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
