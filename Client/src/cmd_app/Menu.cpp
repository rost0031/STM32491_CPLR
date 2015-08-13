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
#include "Cmds.hpp"
#include "ArgParse.hpp"
#include "CliDbgModules.hpp"

/* Lib includes */

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
CLI_MODULE_NAME( CLI_DBG_MODULE_MNU );

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

   root->findChild("DBG")->addChild( "DC3", "Debug output control on the (DC3)" );
   root->findChild("DBG")->findChild("DC3")->addChild( "DEV", "Debug device output control");
   root->findChild("DBG")->findChild("DC3")->findChild( "DEV" )->addChild( "EDE", "(E)nable (D)ebug output over (e)thernet", MENU_DC3_EN_ETH_DBG );
   root->findChild("DBG")->findChild("DC3")->findChild( "DEV" )->addChild( "DDE", "(D)isable (D)ebug output over (e)thernet", MENU_DC3_DIS_ETH_DBG );
   root->findChild("DBG")->findChild("DC3")->findChild( "DEV" )->addChild( "EDS", "(E)nable (D)ebug output over (s)erial", MENU_DC3_EN_SER_DBG );
   root->findChild("DBG")->findChild("DC3")->findChild( "DEV" )->addChild( "DDS", "(D)isable (D)ebug output over (s)erial", MENU_DC3_DIS_SER_DBG );

   root->findChild("DBG")->findChild("DC3")->addChild( "MOD", "Debug module control");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "GET", "(Get) DC3 debug module status", MENU_DC3_GET_DBG_MODULES );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "CPL", "DC3 Coupler Main module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "CPL" )->addChild( "E", "Enable DC3 COUPLER MAIN debug module", MENU_DC3_EN_CPLR_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "CPL" )->addChild( "D", "Disable DC3 COUPLER MAIN debug module", MENU_DC3_DIS_CPLR_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "GEN", "General (main, bsp, etc) module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "GEN" )->addChild( "E", "Enable GENERAL debug module", MENU_DC3_EN_GEN_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "GEN" )->addChild( "D", "Disable GENERAL debug module", MENU_DC3_DIS_GEN_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "SER", "Serial module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "SER" )->addChild( "E", "Enable SERIAL debug module", MENU_DC3_EN_SER_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "SER" )->addChild( "D", "Disable SERIAL debug module", MENU_DC3_DIS_SER_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "TIM", "Time and timer module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "TIM" )->addChild( "E", "Enable TIME debug module", MENU_DC3_EN_TIME_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "TIM" )->addChild( "D", "Disable TIME debug module", MENU_DC3_DIS_TIME_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "ETH", "Ethernet module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "ETH" )->addChild( "E", "Enable ETHERNET debug module", MENU_DC3_EN_ETH_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "ETH" )->addChild( "D", "Disable ETHERNET debug module", MENU_DC3_DIS_ETH_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "I2B", "I2C Bus module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "I2B" )->addChild( "E", "Enable I2C Bus debug module", MENU_DC3_EN_I2C_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "I2B" )->addChild( "D", "Disable I2C Bus debug module", MENU_DC3_DIS_I2C_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "I2D", "I2C Device module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "I2D" )->addChild( "E", "Enable I2C Device debug module", MENU_DC3_EN_I2C_DEV_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "I2D" )->addChild( "D", "Disable I2C Device debug module", MENU_DC3_DIS_I2C_DEV_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "CMM", "CommMgr and Comm module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "CMM" )->addChild( "E", "Enable COMM debug module", MENU_DC3_EN_COMM_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "CMM" )->addChild( "D", "Disable COMM debug module", MENU_DC3_DIS_COMM_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "DBS", "Database and Settings module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "DBS" )->addChild( "E", "Enable DB & Settings debug module", MENU_DC3_EN_DB_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "DBS" )->addChild( "D", "Disable DB & Settings debug module", MENU_DC3_DIS_DB_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "SYS", "SYS and SyshMgr module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "SYS" )->addChild( "E", "Enable SYS debug module", MENU_DC3_EN_SYS_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "SYS" )->addChild( "D", "Disable SYS debug module", MENU_DC3_DIS_SYS_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "NOR", "NOR module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "NOR" )->addChild( "E", "Enable NOR debug module", MENU_DC3_EN_NOR_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "NOR" )->addChild( "D", "Disable NOR debug module", MENU_DC3_DIS_NOR_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "RAM", "SDRAM module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "RAM" )->addChild( "E", "Enable SDRAM debug module", MENU_DC3_EN_NOR_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "RAM" )->addChild( "D", "Disable SDRAM debug module", MENU_DC3_DIS_NOR_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "DBG", "Debug control (DBG) module");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "DBG" )->addChild( "E", "Enable DBG debug module", MENU_DC3_EN_DBGS_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "DBG" )->addChild( "D", "Disable DBG debug module", MENU_DC3_DIS_DBGS_DBG_MODULE );

   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->addChild( "FLS", "FLASH and FlashMgr module (Bootloader Only)");
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "FLS" )->addChild( "E", "Enable FLASH debug module", MENU_DC3_EN_FLASH_DBG_MODULE );
   root->findChild("DBG")->findChild("DC3")->findChild( "MOD" )->findChild( "FLS" )->addChild( "D", "Disable FLASH debug module", MENU_DC3_DIS_FLASH_DBG_MODULE );

   root->findChild("DBG")->addChild( "CLI", "Debug output control on the (Cli)ent" );

   root->addChild( "SYS", "System tests" );

   root->findChild("SYS")->addChild( "MEM", "(Mem)ory tests" );
   root->findChild("SYS")->findChild("MEM")->addChild("RAM", "Test RAM", MENU_RAM_TEST );

   root->findChild("SYS")->addChild( "MDE", "DC3 mode commands" );
   root->findChild("SYS")->findChild("MDE")->addChild( "GET", "(Get) DC3 boot mode", MENU_GET_MODE );
   root->findChild("SYS")->findChild("MDE")->addChild( "SEA", "(Se)t DC3 boot mode to (A)pplication", MENU_SET_APPL );
   root->findChild("SYS")->findChild("MDE")->addChild( "SEB", "(Se)t DC3 boot mode to (B)ootloader", MENU_SET_BOOT );

   root->findChild("SYS")->addChild( "I2C", "I2C tests" );
   root->findChild("SYS")->findChild("I2C")->addChild( "REE", "(R)ead (EE)PROM on I2C" );
   root->findChild("SYS")->findChild("I2C")->findChild("REE")->addChild( "DEF", "Read EEPROM on I2C with (def)ault start and number of bytes", MENU_I2C_READ_TEST_DEF );
   root->findChild("SYS")->findChild("I2C")->findChild("REE")->addChild( "CUS", "Read EEPROM on I2C with (cus)tom start and number of bytes", MENU_I2C_READ_TEST_CUS );
   root->findChild("SYS")->findChild("I2C")->addChild( "WEE", "Write EEPROM on I2C" );
   root->findChild("SYS")->findChild("I2C")->findChild("WEE")->addChild( "DEF", "Write EEPROM on I2C with (def)ault start and number of bytes", MENU_I2C_WRITE_TEST_DEF );
   root->findChild("SYS")->findChild("I2C")->findChild("WEE")->addChild( "CUS", "Write EEPROM on I2C with (cus)tom start and number of bytes", MENU_I2C_WRITE_TEST_CUS );

   root->findChild("SYS")->addChild( "DBS", "Database Settings commands" );
   root->findChild("SYS")->findChild("DBS")->addChild( "RST", "(R)e(s)e(t) DC3 settings Database", MENU_DB_RESET );

   // Finalize the menu node numbers.  This has to be called after all the items
   // have been added to the menu.  This function numbers all the nodes to allow
   // direct access to them without going through the entire menu tree.
   MENU_finalize( root, 1 );

   root->printTree();
   MENU_printHelp();

   while ( 1 ) { // Loop forever until a user uses the menu to quit.

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

         // try and find the node based on the selector user put in
         Ktree *node = currMenuNode->findChild(input);

         if ( NULL == node ) {
            // if no results are found, check if user entered a number to go to
            // node directly
            stringstream ssi(input);
            unsigned int menuNode = 0;
            if ( !(ssi >> menuNode).fail() && (ssi >> std::ws).eof() ) {
               // input was a number, find the right node recursively
               node = root->findChildInTree( menuNode );
            }
         }

         // Check again if the node is still null since numeric input could have
         // selected it and it could be valid now.
         if ( NULL == node ) {
            stringstream ss;
            ss << " *** Menu selector '" << input << "' not found among the menu options ***";
            CON_print(ss.str());
         } else {
            if ( !node->isLeaf() ) {
               currMenuNode = node;

               // Only print menu during navigation.
               MENU_printMenuExpAtCurrNode( currMenuNode, root ); // new
            } else {
               // Input is valid, extract the MENU_ACTION and handle it.

               // Save the parent of the menu leaf so the user can still
               // orient themselves in the menu tree
               currMenuNode = node->getParent();

               // Extract the action associated with the menu item
               MenuAction_t menuAction = node->m_menuAction;

               MENU_parseAndExecAction( menuAction, client );
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
   DC3Error_t statusDC3 = ERR_NONE; // Keep track of DC3 errors
   stringstream ss;

   switch( menuAction ) {
      case MENU_RAM_TEST:
         status = CMD_runRamTest( client, &statusDC3 );
         break;
      case MENU_GET_MODE:
         DC3BootMode_t mode;
         status = CMD_runGetMode( client, &statusDC3, &mode );
         break;
      case MENU_SET_APPL:
         status = CMD_runSetMode( client, &statusDC3, _DC3_Application );
         break;
      case MENU_SET_BOOT:
         status = CMD_runSetMode( client, &statusDC3, _DC3_Bootloader );
         break;
      case MENU_I2C_READ_TEST_DEF:                 // intentionally fall through
      case MENU_I2C_WRITE_TEST_DEF: {;        // local scope and ; for label fix

         // common vars and setup
         int bytes = 16, start = 0;
         size_t nMaxBufferSize = 20;

         DC3I2CDevice_t dev = _DC3_EEPROM;
         DC3AccessType_t acc = _DC3_ACCESS_QPC;   // set to a default arg of QPC

         uint8_t *buffer = new uint8_t[nMaxBufferSize];

         if ( MENU_I2C_READ_TEST_DEF == menuAction ) {
            uint16_t bytesRead = 0;
            status = CMD_runReadI2C( client, &statusDC3, &bytesRead, buffer,
                  nMaxBufferSize, bytes, start, dev, acc );
         } else {
            // form the default array of the length user wants but still have
            // our code parse it instead of just making the data array directly.
            // This allows the same code path to be taken for both default and
            // user arrays.
            string defaultArrayStr = "[";
            stringstream ss_tmp;
            for (int i = 0; i < bytes; i++ ) {
               ss_tmp << "0x" << hex << setfill('0') << setw(2) << unsigned(i);
               if (i < (bytes - 1) ) {
                  ss_tmp << ",";  // append a comma unless it's the last element
               }
            }
            defaultArrayStr.append(ss_tmp.str());
            defaultArrayStr.append("]");

            size_t dataLen = 0;
            try {
               ARG_parseHexStr( buffer, &dataLen, nMaxBufferSize, defaultArrayStr );
            } catch (exception &e) {
               ERR_out << "Caught exception parsing data array: " << e.what();
            }

            bytes = dataLen;        // update the bytes with how many were read

            status = CMD_runWriteI2C( client, &statusDC3, buffer,
                  bytes, start, dev, acc );
         }

         delete[] buffer;

         break;
      }
      case MENU_I2C_READ_TEST_CUS:                 // intentionally fall through
      case MENU_I2C_WRITE_TEST_CUS:{;         // local scope and ; for label fix
         int bytes = 0, start = 0;
         size_t nMaxBufferSize = 0;

         DC3I2CDevice_t dev = _DC3_EEPROM;
         DC3AccessType_t acc = _DC3_ACCESS_QPC;   // set to a default arg of QPC


         // TODO: the current magic numbers should be looked up from a common
         // header file eventually when we refactor the lib to include info
         // about device sizes
         if( !ARG_userInputNum( &start, 0, 255, "Offset to start access from...") ) {
            stringstream ss;
            ss << "*** Not performing operation '" << enumToString(menuAction)
                  << "'. ***";
            CON_print(ss.str());
            break;
         }

         // Don't allocate array here since invalid inputs might cause it to not
         // be used.
         uint8_t *buffer;

         if ( MENU_I2C_READ_TEST_CUS == menuAction ) {

            // Don't allow to read more than there are in the device
            if( !ARG_userInputNum( &bytes, 1, 256 - start, "How many bytes to access...") ) {
               stringstream ss;
               ss << "*** Not performing operation '" << enumToString(menuAction)
                        << "'. ***";
               CON_print(ss.str());
               break;
            }

            nMaxBufferSize = 1000;
            buffer = new uint8_t[nMaxBufferSize];

            uint16_t bytesRead = 0;
            status = CMD_runReadI2C( client, &statusDC3, &bytesRead, buffer,
                  nMaxBufferSize, bytes, start, dev, acc );

            delete[] buffer;

         } else {

            nMaxBufferSize = 1000;
            buffer = new uint8_t[nMaxBufferSize];
            size_t bytesInArray = 0;

            if( !ARG_userInputArr( buffer, &bytesInArray, nMaxBufferSize, "Array of data to write..." ) ) {
               stringstream ss;
               ss << "*** Not performing operation '" << enumToString(menuAction)
                        << "'. ***";
               CON_print(ss.str());
               delete[] buffer;        // Don't forget to delete buffer on error
               break;
            }

            bytes = bytesInArray;

            status = CMD_runWriteI2C( client, &statusDC3, buffer,
                  bytes, start, dev, acc );

            delete[] buffer;         // Don't forget to delete buffer on success
         }

         break;
      }

      case MENU_DC3_GET_DBG_MODULES:{;
         uint32_t dbgModules = 0;
         status = CMD_runGetDbgModules( client, &statusDC3, &dbgModules );
         break;
      }

      case MENU_DC3_EN_GEN_DBG_MODULE:             // Fall through intentionally
      case MENU_DC3_EN_SER_DBG_MODULE:             // Fall through intentionally
      case MENU_DC3_EN_TIME_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_EN_ETH_DBG_MODULE:             // Fall through intentionally
      case MENU_DC3_EN_I2C_DBG_MODULE:             // Fall through intentionally
      case MENU_DC3_EN_I2C_DEV_DBG_MODULE:         // Fall through intentionally
      case MENU_DC3_EN_COMM_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_EN_CPLR_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_EN_DB_DBG_MODULE:              // Fall through intentionally
      case MENU_DC3_EN_FLASH_DBG_MODULE:           // Fall through intentionally
      case MENU_DC3_EN_SYS_DBG_MODULE:             // Fall through intentionally
      case MENU_DC3_EN_NOR_DBG_MODULE:             // Fall through intentionally
      case MENU_DC3_EN_SDRAM_DBG_MODULE:           // Fall through intentionally
      case MENU_DC3_EN_DBGS_DBG_MODULE: {;    // local scope and ; for label fix
         uint32_t dbgModuleSet = 0;
         if ( MENU_DC3_EN_GEN_DBG_MODULE == menuAction)           dbgModuleSet = DC3_DBG_MODL_GEN;
         else if ( MENU_DC3_EN_SER_DBG_MODULE == menuAction)      dbgModuleSet = DC3_DBG_MODL_SER;
         else if ( MENU_DC3_EN_TIME_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_TIME;
         else if ( MENU_DC3_EN_ETH_DBG_MODULE== menuAction)       dbgModuleSet = DC3_DBG_MODL_ETH;
         else if ( MENU_DC3_EN_I2C_DBG_MODULE== menuAction)       dbgModuleSet = DC3_DBG_MODL_I2C;
         else if ( MENU_DC3_EN_I2C_DEV_DBG_MODULE == menuAction)  dbgModuleSet = DC3_DBG_MODL_I2C_DEV;
         else if ( MENU_DC3_EN_COMM_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_COMM;
         else if ( MENU_DC3_EN_CPLR_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_CPLR;
         else if ( MENU_DC3_EN_DB_DBG_MODULE== menuAction)        dbgModuleSet = DC3_DBG_MODL_DB;
         else if ( MENU_DC3_EN_FLASH_DBG_MODULE== menuAction)     dbgModuleSet = DC3_DBG_MODL_FLASH;
         else if ( MENU_DC3_EN_SYS_DBG_MODULE== menuAction)       dbgModuleSet = DC3_DBG_MODL_SYS;
         else if ( MENU_DC3_EN_NOR_DBG_MODULE== menuAction)       dbgModuleSet = DC3_DBG_MODL_NOR;
         else if ( MENU_DC3_EN_SDRAM_DBG_MODULE== menuAction)     dbgModuleSet = DC3_DBG_MODL_SDRAM;
         else if ( MENU_DC3_EN_DBGS_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_DBG;
         // Run the cmd with bEnable = true indicating an enable and
         // bOverWrite = false indicating to only touch that field and not the
         // rest
         status = CMD_runSetDbgModule(
               client,
               &statusDC3,
               &dbgModuleSet,
               true,
               false
         );
      }
      break;

      case MENU_DC3_DIS_GEN_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_DIS_SER_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_DIS_TIME_DBG_MODULE:           // Fall through intentionally
      case MENU_DC3_DIS_ETH_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_DIS_I2C_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_DIS_I2C_DEV_DBG_MODULE:        // Fall through intentionally
      case MENU_DC3_DIS_COMM_DBG_MODULE:           // Fall through intentionally
      case MENU_DC3_DIS_CPLR_DBG_MODULE:           // Fall through intentionally
      case MENU_DC3_DIS_DB_DBG_MODULE:             // Fall through intentionally
      case MENU_DC3_DIS_FLASH_DBG_MODULE:          // Fall through intentionally
      case MENU_DC3_DIS_SYS_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_DIS_NOR_DBG_MODULE:            // Fall through intentionally
      case MENU_DC3_DIS_SDRAM_DBG_MODULE:          // Fall through intentionally
      case MENU_DC3_DIS_DBGS_DBG_MODULE: {;   // local scope and ; for label fix
         uint32_t dbgModuleSet = 0;
         if ( MENU_DC3_EN_GEN_DBG_MODULE == menuAction)           dbgModuleSet = DC3_DBG_MODL_GEN;
         else if ( MENU_DC3_DIS_SER_DBG_MODULE == menuAction)     dbgModuleSet = DC3_DBG_MODL_SER;
         else if ( MENU_DC3_DIS_TIME_DBG_MODULE== menuAction)     dbgModuleSet = DC3_DBG_MODL_TIME;
         else if ( MENU_DC3_DIS_ETH_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_ETH;
         else if ( MENU_DC3_DIS_I2C_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_I2C;
         else if ( MENU_DC3_DIS_I2C_DEV_DBG_MODULE == menuAction) dbgModuleSet = DC3_DBG_MODL_I2C_DEV;
         else if ( MENU_DC3_DIS_COMM_DBG_MODULE== menuAction)     dbgModuleSet = DC3_DBG_MODL_COMM;
         else if ( MENU_DC3_DIS_CPLR_DBG_MODULE== menuAction)     dbgModuleSet = DC3_DBG_MODL_CPLR;
         else if ( MENU_DC3_DIS_DB_DBG_MODULE== menuAction)       dbgModuleSet = DC3_DBG_MODL_DB;
         else if ( MENU_DC3_DIS_FLASH_DBG_MODULE== menuAction)    dbgModuleSet = DC3_DBG_MODL_FLASH;
         else if ( MENU_DC3_DIS_SYS_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_SYS;
         else if ( MENU_DC3_DIS_NOR_DBG_MODULE== menuAction)      dbgModuleSet = DC3_DBG_MODL_NOR;
         else if ( MENU_DC3_DIS_SDRAM_DBG_MODULE== menuAction)    dbgModuleSet = DC3_DBG_MODL_SDRAM;
         else if ( MENU_DC3_DIS_DBGS_DBG_MODULE== menuAction)     dbgModuleSet = DC3_DBG_MODL_DBG;
         // Run the cmd with bEnable = false indicating a disable and
         // bOverWrite = false indicating to only touch that field and not the
         // rest
         DBG_out << "Disabling " << enumToString((DC3DbgModule_t)dbgModuleSet) << " module";
         status = CMD_runSetDbgModule(
               client,
               &statusDC3,
               &dbgModuleSet,
               false,
               false
         );
      }
      break;

      case MENU_DC3_EN_ETH_DBG:
         status = CMD_runSetDbgDevice( client, &statusDC3, _DC3_EthCli, true );
         break;
      case MENU_DC3_DIS_ETH_DBG:
         status = CMD_runSetDbgDevice( client, &statusDC3, _DC3_EthCli, false );
         break;
      case MENU_DC3_EN_SER_DBG:
         status = CMD_runSetDbgDevice( client, &statusDC3, _DC3_Serial, true );
         break;
      case MENU_DC3_DIS_SER_DBG:
         status = CMD_runSetDbgDevice( client, &statusDC3, _DC3_Serial, false );
         break;
      case MENU_DB_RESET:
         status = CMD_runResetDB( client, &statusDC3 );
         break;
      case MENU_NO_ACTION:;
         WRN_out << enumToString(menuAction) <<  " associated with this menu selection";
         break;
      default:
         ERR_out << "Unknown menu action item: (" << menuAction << ") " << enumToString(menuAction);
         break;
   }

   return status;
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
