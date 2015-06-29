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
ClientError_t MENU_run( ClientApi *client )
{
   Ktree *currMenuNode = NULL;

   string input;
   Ktree *root = new Ktree( "top", "Menu" );
   currMenuNode = root;

   root->addChild( "DBG", "Debug control" );
   root->addChild( "SYS", "System tests" );

   root->findChild("SYS")->addChild( "I2C", "I2C tests" );
   root->findChild("SYS")->findChild("I2C")->addChild( "REE", "Read EEPROM on I2C" );
   root->findChild("SYS")->findChild("I2C")->addChild( "WEE", "Write EEPROM on I2C" );

   root->findChild("SYS")->addChild( "MEM", "Memory tests" );
   root->findChild("SYS")->findChild("MEM")->addChild("RAM", "Test RAM" );
   root->findChild("SYS")->findChild("MEM")->addChild("RAM1", "Test RAM again" );


   MENU_finalize( root, 1 ); // Finalize the menu node numbers

//   root->printNode(0);
//   dbgMenu->printNode(1);
//   sysTestMenu->printNode(1);

//   root->deleteChild("DBG");

//   root->printNode(0);

   root->printTree();
   MENU_printHelp();
   while ( 1 ) {

      CON_print(" *** Enter a command: ***");
      cin >> input; // Read input again at the end

      if( boost::iequals(input, "exit") || boost::iequals(input, "quit") ||
          boost::iequals(input, "x") || boost::iequals(input, "q")  ) {
         CON_print(" *** Leaving the menu... *** ");
         return CLI_ERR_NONE;
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
               MENU_printMenuExpAtCurrNode( currMenuNode, root );
            } else {

            }
         } else {

            stringstream ssi(input);
            unsigned int menuNode = 0;
            if ( !(ssi >> menuNode).fail() && (ssi >> std::ws).eof() ) {
               // input a number
               WRN_out << "Input a number: " << menuNode;
               Ktree* node = root->findChildInTree( menuNode );
               if ( NULL != node ) {
                  if (node->isLeaf()) {
                     currMenuNode = node->getParent();
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

   return CLI_ERR_NONE;
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

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
