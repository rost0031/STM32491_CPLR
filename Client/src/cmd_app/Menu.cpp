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
APIError_t MENU_run( ClientApi *client )
{

   Ktree *currMenuNode = NULL;

   string input;
   Ktree *root = new Ktree( "top", "Menu" );
   currMenuNode = root;

   root->addChild( "DBG", "Debug control" );
   root->addChild( "SYS", "System tests" );

   root->findChild("SYS")->addChild( "I2C", "I2C tests" );
   root->findChild("SYS")->findChild("I2C")->addChild( "REE", "Read EEPROM on I2C", MENU_I2C_READ_TEST );
   root->findChild("SYS")->findChild("I2C")->addChild( "WEE", "Write EEPROM on I2C", MENU_I2C_WRITE_TEST );

   root->findChild("SYS")->addChild( "MEM", "Memory tests" );
   root->findChild("SYS")->findChild("MEM")->addChild("RAM", "Test RAM", MENU_RAM_TEST );
   root->findChild("SYS")->findChild("MEM")->addChild("RAM1", "Test RAM again" );


   MENU_finalize( root, 1 ); // Finalize the menu node numbers

//   root->printNode(0);
//   dbgMenu->printNode(1);
//   sysTestMenu->printNode(1);

//   root->deleteChild("DBG");

//   root->printNode(0);

   root->printTree();
   MENU_printHelp();

   while ( 1 ) { // Loop forever until a user uses the menu to quit.

      APIError_t status = API_ERR_NONE; // keep track of API status


      CON_print(" *** Enter a command: ***");
      cin >> input; // Read input again at the end

      if( boost::iequals(input, "exit") || boost::iequals(input, "quit") ||
          boost::iequals(input, "x") || boost::iequals(input, "q")  ) {
         CON_print(" *** Leaving the menu... *** ");
         return API_ERR_NONE;
      } else if( boost::iequals(input, "help") || boost::iequals(input, "?") ||
                 boost::iequals(input, "h") ) {
         MENU_printHelp();
      } else if( boost::iequals(input, "up") || boost::iequals(input, "u") ) {
         // go up a level
         Ktree *node = currMenuNode->getParent();
         if ( NULL != node ) {
            currMenuNode = node;
            MENU_printMenuExpAtCurrNode( currMenuNode, root );
         } else {
            CON_print(" *** Already at the top of the menu *** ");
            MENU_printMenuExpAtCurrNode( currMenuNode, root );
         }
      } else if( boost::iequals(input, "top") || boost::iequals(input, "t") ) {
         // go to the top of the tree
         currMenuNode = root;
         MENU_printMenuExpAtCurrNode( currMenuNode, root );
      } else if( boost::iequals(input, "all") || boost::iequals(input, "a") ) {
            root->printTree();                         // print entire menu tree
      } else if( boost::iequals(input, "print") || boost::iequals(input, "p") ) {
         // print current menu level
         MENU_printMenuExpAtCurrNode( currMenuNode, root );
      } else {
         Ktree *node = currMenuNode->findChild(input);
         if ( NULL != node ) {
            if ( !node->isLeaf() ) {
               currMenuNode = node;
            } else {
               // Input is valid, extract the MENU_ACTION and handle it.

               // Save the parent of the menu leaf so the user can still
               // orient themselves in the menu tree
               currMenuNode = node->getParent();

               // Extract the action associated with the menu item
               MenuAction_t menuAction = node->m_menuAction;

               status = MENU_parseAndExecAction( menuAction, client );
            }

            // print the menu in both cases
            MENU_printMenuExpAtCurrNode( currMenuNode, root );

         } else {

            stringstream ssi(input);
            unsigned int menuNode = 0;
            if ( !(ssi >> menuNode).fail() && (ssi >> std::ws).eof() ) {
               // input a number
               Ktree* node = root->findChildInTree( menuNode );
               if ( NULL != node ) {
                  if (node->isLeaf()) {
                     // Input is valid, extract the MENU_ACTION and handle it.

                     // Save the parent of the menu leaf so the user can still
                     // orient themselves in the menu tree
                     currMenuNode = node->getParent();

                     // Extract the action associated with the menu item
                     MenuAction_t menuAction = node->m_menuAction;

                     status = MENU_parseAndExecAction( menuAction, client );

                  } else {
                     currMenuNode = node;
                  }
                  MENU_printMenuExpAtCurrNode( currMenuNode, root );
               } else {
                  stringstream ss;
                  ss << " *** Menu node number '" << menuNode << "' not found in the menu tree ***";
                  CON_print(ss.str());
                  root->printTree();
               }

            } else { // input not a number
               stringstream ss;
               ss << " *** Menu selector '" << input << "' not found among the menu options ***";
               CON_print(ss.str());
               MENU_printMenuLevel( currMenuNode );
               MENU_printMenuExpAtCurrNode( currMenuNode, root );
            }
         }
      }
   }

   return API_ERR_NONE;
}

/******************************************************************************/
void MENU_finalize( Ktree* node, unsigned int number )
{
   vector<Ktree*>::iterator it = node->children_begin();
   for( it = node->children_begin(); it != node->children_end(); ++it ) {
      (*it)->m_number = number;
      if ( !(*it)->isLeaf() ) {
         MENU_finalize( (*it), ((*it)->m_number * 10) );
      }
      ++number;
   }
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
void MENU_printMenuLevel( Ktree* node )
{
   stringstream ss;
   node->childrenToStream( ss );
   CON_print(ss.str());
}

/******************************************************************************/
void MENU_printMenuExpAtCurrNode( Ktree* node, Ktree* root )
{
   if( NULL == node ) {
      WRN_out << "Passed in a null menu node";
      return;
   }

   KtreeNav *kNav = new KtreeNav();
   kNav->findAncestry( node );

   // increment to start with since we'll skip checking the top node and start
   // with the children.
   kNav->m_index++;

   // stream the root node first. This makes the recusive function much simpler
   stringstream ss;
   ss << "********************************************************************************" << endl;
   root->nodeToStream( ss );
   MENU_nodeAncestryToStream( root, kNav, ss );
   ss << "********************************************************************************" << endl;
   CON_print(ss.str());

   delete kNav;
}

/******************************************************************************/
void MENU_nodeAncestryToStream( Ktree* node, KtreeNav* kNav, stringstream& ss)
{
   // Iterate over all the children of the current node
   vector<Ktree*>::iterator it = node->children_begin();
   for( it = node->children_begin(); it != node->children_end(); ++it ) {
      (*it)->nodeToStream( ss ); // always print the child node

      if ( !node->isLeaf() && ( kNav->m_index < kNav->m_number_of_elems ) ) {
         // don't check if leaf node since nowhere else to descend
         if ( kNav->m_ancestry.at(kNav->m_index) == (*it) ) {
            // if node matches the node in the nav, descend recursively

            // check if this is the last node (leaf) because if it is, don't
            // recurse any lower
            kNav->m_index++;
            MENU_nodeAncestryToStream( (*it), kNav, ss);
         }
      }
   }
}

/******************************************************************************/
APIError_t MENU_parseAndExecAction(
      MenuAction_t menuAction,
      ClientApi* client
)
{
   APIError_t status = API_ERR_NONE; // Keep track of API errors
   CBErrorCode statusDC3 = ERR_NONE; // Keep track of DC3 errors

   stringstream ss;

   switch( menuAction ) {
      case MENU_RAM_TEST: {
         // no extra arguments or options are required from the user for this
         CBRamTest_t test = _CB_RAM_TEST_NONE;
         uint32_t addr = 0x00000000;
         CON_print(" *** Running memory test of external RAM of DC3... ***");
         if( API_ERR_NONE == (status = client->DC3_ramTest(&statusDC3, &test, &addr))) {
            ss.clear();
            ss << " *** Ram test of DC3 ";
            if (ERR_NONE == statusDC3) {
               ss << "completed with no errors.";
            } else {
               ss << "failed with ERROR: 0x" << setw(8) << setfill('0') << hex << statusDC3
                     << " with test " << enumToString(test) << " failing at addr "
                     << "0x" << setw(8) << setfill('0') << hex << addr
                     << dec;
            }
            ss << " ***";
            CON_print(ss.str());
         } else {
            ERR_out << " *** Got DC3 error " << "0x" << std::hex
               << status << std::dec << " when trying to run RAM test. ***";
         }
         break;
      }
      case MENU_NO_ACTION:
         WRN_out << enumToString(menuAction) <<  " associated with this menu selection";
         break;
      default:
         ERR_out << "Unknown action (" << menuAction << ")";
         break;
   }


   return status;
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
