/**
 * @file    ArgParse.cpp
 * Parsing of secondary arguments input via Menu and CmdLine interface.
 *
 * @date    07/06/2015
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>
#include <vector>
#include <iterator>
#include <map>
#include <fstream>

/* Boost includes */
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/range/algorithm/remove_if.hpp>

/* App includes */
#include "ArgParse.hpp"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
namespace po = boost::program_options;
using namespace po;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
CLI_MODULE_NAME( CLI_DBG_MODULE_ARG );

/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief All allowed strings for specifying DC3BootMode enums
 */
template<> std::map<DC3BootMode_t, std::vector<std::string>> allowedStrings<DC3BootMode_t>::m_allowedStrings = {
      { _DC3_NoBootMode   , {"None","invalid"} },
      { _DC3_Bootloader   , {"b", "bo", "bt", "boo", "boot", "bootloader", "bootldr", "btldr" } },
      { _DC3_Application  , {"a", "ap", "app", "appl", "application", "scips", "scipsiii", "scips3" } }
};

/**
 * @brief All allowed strings for specifying DC3MsgRoute enums
 */
template<> std::map<DC3MsgRoute_t, std::vector<std::string>> allowedStrings<DC3MsgRoute_t>::m_allowedStrings = {
      { _DC3_NoRoute   , {"None","no route"} },
      { _DC3_Serial   , {"se", "serial", "ser"} },
      { _DC3_EthSys  , {"sy", "sys", "sys_eth", "system_eth", "system_ethernet", "eth_sys", "ethernet_system", "eth_system"} },
      { _DC3_EthLog  , {"l","lo","log", "log_eth", "logging_eth", "logging_ethernet", "eth_log", "ethernet_logging", "eth_logging"} },
      { _DC3_EthCli  , {"c", "cl","cli", "cli_eth", "client_eth", "client_ethernet", "eth_cli", "ethernet_client", "eth_client"} }
};

/**
 * @brief All allowed strings for specifying DC3I2CDevices enums
 */
template<> std::map<DC3I2CDevice_t, std::vector<std::string>> allowedStrings<DC3I2CDevice_t>::m_allowedStrings = {
      { _DC3_EEPROM   , {"e","ee","eep","eeprom","eprom"} },
      { _DC3_SNROM   , {"snrom", "sn", "ser_rom", "srom", "serial_rom", "serial_number_rom", } },
      { _DC3_EUIROM  , {"euirom", "eui", "ui", "uie", "uierom", "eurom", "uirom", } },
};

/**
 * @brief All allowed strings for specifying DC3AccessType enums
 */
template<> std::map<DC3AccessType_t, std::vector<std::string>> allowedStrings<DC3AccessType_t>::m_allowedStrings = {
      { _DC3_ACCESS_BARE, {"b", "bare","metal", "bare_metal", "baremetal"} },
      { _DC3_ACCESS_QPC , {"q", "qp", "qpc", } },
      { _DC3_ACCESS_FRT , {"f", "fr", "frt", "freertos", "frtos", "free"} },

};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void ARG_split(
      const std::string& str,
      std::vector<string>& tokens,
      const std::string& delims
)
{
   // Skip delimiters at beginning.
   string::size_type lastPos = str.find_first_not_of(delims, 0);
   // Find first "non-delimiter".
   string::size_type pos     = str.find_first_of(delims, lastPos);

   while (string::npos != pos || string::npos != lastPos) {
      // Foundtoken, add to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));

      // Skip delimiters.
      lastPos = str.find_first_not_of(delims, pos);

      // Find next "non-delimiter"
      pos = str.find_first_of(delims, lastPos);
   }
}

/******************************************************************************/
bool ARG_getValue(
      string& value,
      const string& argName,
      const vector<string>& argVector
)
{
   vector<string>::const_iterator begin = argVector.begin();
   vector<string>::const_iterator end = argVector.end();
   vector<string>::const_iterator itr;

   for( itr = begin; itr != end; ++itr ) {
      if ( (*itr).find("=") ) {
         vector<string> arg_pair;
         ARG_split( (*itr), arg_pair, "=" );
         if (2 != arg_pair.size()) {                       // invalid arg format
            return false;
         } else if ( boost::iequals(arg_pair.at(0), argName) ) {
            // Do all the checking for whether it exists top level
            value = arg_pair.at(1);
            return true;
         }
      }
   }
   return false;
}

/******************************************************************************/
void ARG_checkCmdSpecificHelp(
      const string& cmd,
      const string& appName,
      const po::variables_map& vm,
      const bool isConnSet
)
{
   if (vm.count("help") || !isConnSet) {  // Check for command specific help req
      HELP_printCmdSpecific( cmd, appName );
   }

   vector<string>::const_iterator begin = vm[cmd].as<vector<string>>().begin();
   vector<string>::const_iterator end = vm[cmd].as<vector<string>>().end();
   vector<string>::const_iterator itr;
   // Check for command specific help req
   if( find(begin, end, "--help") != end ||
       find(begin, end, "help") != end  || !isConnSet ) {
      HELP_printCmdSpecific( cmd, appName );
   }
}

/******************************************************************************/
void ARG_parseFilenameStr(
      string& value,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
)
{
   // Extract the value from the arg=value pair
   string valueStr = "";
   bool isExtracted = ARG_getValue( valueStr, arg, args );

   if( !isExtracted ) {   // if error happened, print cmd specific help and exit
      HELP_printCmdSpecific( cmd, appName );
   }

   ifstream fw_file (                  // open file stream to read in file
         valueStr.c_str(),
         ios::in | ios::binary | ios::ate
   );
   if (fw_file) {                           // if the file exists, read it
      fw_file.close();                                       // Close file
      value = valueStr;
   } else {
      std::stringstream ss;
      ss << "Unable to open file " << valueStr << "... check your path and filename";
      throw std::invalid_argument(ss.str());
   }
}

/******************************************************************************/
void ARG_parseHexArr(
      uint8_t* pDataArr,
      size_t* pDataArrLen,
      const size_t nDataArrMaxSize,
      string& defaultDataStr,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
)
{
   // Extract the value from the arg=value pair
   string valueStr = "";

   if( !ARG_getValue( valueStr, arg, args ) ) {
      // if error happened, set the default value to output and return
      ARG_parseHexStr(pDataArr, pDataArrLen, nDataArrMaxSize, defaultDataStr);
      return;
   }

   // The template function ARG_getEnumFromAllowedStr will throw an exception of
   // the passed in value is not found in the appropriate map.  We'll rethrow it
   // if that happens.
   try {
      ARG_parseHexStr(pDataArr, pDataArrLen, nDataArrMaxSize, valueStr);
   } catch (exception& e) {
      throw;
   }
}

/******************************************************************************/
void ARG_parseHexStr(
      uint8_t* pDataArr,
      size_t* pDataArrLen,
      const size_t nDataArrMaxSize,
      string& valueStr
)
{
   // check the string for surrounding [], (), {}, '', <>, or "" and strip them
   boost::erase_all(valueStr, "[" );
   boost::erase_all(valueStr, "]" );
   boost::erase_all(valueStr, "{" );
   boost::erase_all(valueStr, "}" );
   boost::erase_all(valueStr, "(" );
   boost::erase_all(valueStr, ")" );
   boost::erase_all(valueStr, "\"" );
   boost::erase_all(valueStr, "'" );

   // tokenize the string using , ; . or spaces
   typedef boost::tokenizer<boost::char_separator<char> > hexTokenizer;
   boost::char_separator<char> tokenSep(",;. ", "", boost::drop_empty_tokens);

   hexTokenizer tok(valueStr, tokenSep);

   int index = 0;
   for(hexTokenizer::iterator curTok=tok.begin(); curTok != tok.end(); ++curTok) {
      uint8_t currNum = 0;
      ARG_parseNumber( &currNum, *curTok );
      pDataArr[index++] = currNum;
   }
   *pDataArrLen = index;
}

/******************************************************************************/
bool ARG_userInputArr(
      uint8_t* pDataArr,
      size_t* pDataArrLen,
      const size_t nDataArrMaxSize,
      const string& msg
)
{
   stringstream ss;
   ss << msg << endl; // print the explanation msg and ask for the actual value
   ss << "Input a comma separated array of hex or decimal numbers. Prepend hex "
         << endl << "numbers with '0x'. Type 'cancel' to abort:";
   CON_print( ss.str() );

   string input;
   cin >> input;                             // Read input from user as a string

   if ( boost::iequals(input, "cancel") ) {
      return false;
   }

   try {
      ARG_parseHexStr(pDataArr, pDataArrLen, nDataArrMaxSize, input);
   } catch ( exception &e ) {
      WRN_out << "Caught exception trying to parse array.  Aborting...";
      return false;
   }

   // If we got here, everything parsed ok.
   return true;
}

/******************************************************************************/
bool ARG_userAccessType(
      DC3AccessType_t* pAccessType,
      const string& msg
)
{
   stringstream ss;
   ss << msg << endl; // print the explanation msg and ask for the actual value
   ss << "Type the number next the access type you would like to use: " << endl
      << "Type 'cancel' to abort or 'default' to use the default access type (QPC)" << endl
      << "1 - QPC: access using QPC events (default)" << endl
      << "2 - FRT: access using FreeRTOS queues (only available in Application)" << endl
      << "3 - Bare metal: access using bare metal access.  This is risky if other things are running." << endl;
   CON_print( ss.str() );

   string input;
   cin >> input;                             // Read input from user as a string

   if ( boost::iequals(input, "cancel") ) {
      return false;
   } else if ( boost::iequals(input, "default") ) {
      *pAccessType = _DC3_ACCESS_QPC;
   } else if ( boost::iequals(input, "1") ) {
      *pAccessType = _DC3_ACCESS_QPC;
   } else if ( boost::iequals(input, "2") ) {
      *pAccessType = _DC3_ACCESS_FRT;
   } else if ( boost::iequals(input, "3") ) {
      *pAccessType = _DC3_ACCESS_BARE;
   } else {
      WRN_out << "Unknown input: '" << input << "'  Ignoring...";
      return false;
   }

   // If we got here, everything parsed ok.
   return true;
}

/******************************************************************************/
bool ARG_userDbElem(
      DC3DBElem_t* pDbElem,
      const bool write,
      const string& msg
)
{
   stringstream ss;
   ss << msg << endl; // print the explanation msg and ask for the actual value
   ss << "Type the number of the  DB element you would like to access: " << endl
      << "Type 'cancel' to abort" << endl;
   if( write ) {
      ss << " 1 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_MAGIC_WORD)
         << ": the database magic word that allows self checks to see if DB is valid" << endl
         << " 2 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_VERSION)
         << ": version of DB settings" << endl
         << " 4 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_IP_ADDR)
         << ": IP address stored in the DB" << endl
         << " 6 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_BOOT_MAJ)
         << ": Major version of the Bootloader stored in DB" << endl
         << " 7 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_BOOT_MIN)
         << ": Minor version of the Bootloader stored in DB" << endl
         << " 8 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_BOOT_BUILD_DATETIME)
         << ": Build datetime of the Bootloader stored in DB" << endl
         << "12 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_FPGA_MAJ)
         << ": Major version of the FPGA stored in the DB" << endl
         << "13 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_FPGA_MIN)
         << ": Minor version of the FPGA stored in the DB" << endl
         << "14 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_FPGA_BUILD_DATETIME)
         << ": Build datetime of the FPGA stored in the DB" << endl
         << "15 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_DBG_MODULES)
         << ": Debug module enable/disable settings stored in the DB" << endl
         << "16 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_DBG_DEVICES)
         << ": Debug device enable/disable settings stored in the DB" << endl;
   } else {
      ss << " 1 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_MAGIC_WORD)
         << ": the database magic word that allows self checks to see if DB is valid" << endl
         << " 2 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_VERSION)
         << ": version of DB settings" << endl
         << " 3 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_MAC_ADDR)
         << ": MAC address stored in the RO section of the DB" << endl
         << " 4 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_IP_ADDR)
         << ": IP address stored in the DB" << endl
         << " 5 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_SN)
         << ": Serial number stored in the RO section of the DB" << endl
         << " 6 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_BOOT_MAJ)
         << ": Major version of the Bootloader stored in DB" << endl
         << " 7 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_BOOT_MIN)
         << ": Minor version of the Bootloader stored in DB" << endl
         << " 8 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_BOOT_BUILD_DATETIME)
         << ": Build datetime of the Bootloader stored in DB" << endl
         << " 9 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_APPL_MAJ)
         << ": Major version of the Application stored in RO section of the DB" << endl
         << "10 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_APPL_MIN)
         << ": Minor version of the Application stored in RO section of the DB" << endl
         << "11 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_APPL_BUILD_DATETIME)
         << ": Build datetime of the Application stored in RO section of the DB" << endl
         << "12 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_FPGA_MAJ)
         << ": Major version of the FPGA stored in the DB" << endl
         << "13 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_FPGA_MIN)
         << ": Minor version of the FPGA stored in the DB" << endl
         << "14 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_FPGA_BUILD_DATETIME)
         << ": Build datetime of the FPGA stored in the DB" << endl
         << "15 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_DBG_MODULES)
         << ": Debug module enable/disable settings stored in the DB" << endl
         << "16 - " << setw(22) << setfill(' ') << enumToString(_DC3_DB_DBG_DEVICES)
         << ": Debug device enable/disable settings stored in the DB" << endl;
   }

   CON_print( ss.str() );

   string input;
   cin >> input;                             // Read input from user as a string

   if ( boost::iequals(input, "cancel") ) {
      return false;
   } else if ( boost::iequals(input, "1") ) {
      *pDbElem = _DC3_DB_MAGIC_WORD;
      return true;
   } else if ( boost::iequals(input, "2") ) {
      *pDbElem = _DC3_DB_VERSION;
      return true;
   } else if ( boost::iequals(input, "3") ) {
      *pDbElem = _DC3_DB_MAC_ADDR;
      if(write) {
         ERR_out << enumToString(*pDbElem) << " DB element is Read Only. Aborting. ";
      } else {
         return true;
      }
   } else if ( boost::iequals(input, "4") ) {
      *pDbElem = _DC3_DB_IP_ADDR;
      return true;
   } else if ( boost::iequals(input, "5") ) {
      *pDbElem = _DC3_DB_SN;
      if(write) {
         ERR_out << enumToString(*pDbElem) << " DB element is Read Only. Aborting. ";
      } else {
         return true;
      }
   } else if ( boost::iequals(input, "6") ) {
      *pDbElem = _DC3_DB_BOOT_MAJ;
      return true;
   } else if ( boost::iequals(input, "7") ) {
      *pDbElem = _DC3_DB_BOOT_MIN;
      return true;
   } else if ( boost::iequals(input, "8") ) {
      *pDbElem = _DC3_DB_BOOT_BUILD_DATETIME;
      return true;
   } else if ( boost::iequals(input, "9") ) {
      *pDbElem = _DC3_DB_APPL_MAJ;
      if(write) {
         ERR_out << enumToString(*pDbElem) << " DB element is Read Only. Aborting. ";
      } else {
         return true;
      }
   } else if ( boost::iequals(input, "10") ) {
      *pDbElem = _DC3_DB_APPL_MIN;
      if(write) {
         ERR_out << enumToString(*pDbElem) << " DB element is Read Only. Aborting. ";
      } else {
         return true;
      }
   } else if ( boost::iequals(input, "11") ) {
      *pDbElem = _DC3_DB_APPL_BUILD_DATETIME;
      if(write) {
         ERR_out << enumToString(*pDbElem) << " DB element is Read Only. Aborting. ";
      } else {
         return true;
      }
   } else if ( boost::iequals(input, "12") ) {
      *pDbElem = _DC3_DB_FPGA_MAJ;
      return true;
   } else if ( boost::iequals(input, "13") ) {
      *pDbElem = _DC3_DB_FPGA_MIN;
      return true;
   } else if ( boost::iequals(input, "14") ) {
      *pDbElem = _DC3_DB_FPGA_BUILD_DATETIME;
      return true;
   } else if ( boost::iequals(input, "15") ) {
      *pDbElem = _DC3_DB_DBG_MODULES;
      return true;
   } else if ( boost::iequals(input, "16") ) {
      *pDbElem = _DC3_DB_DBG_DEVICES;
      return true;
   } else {
      WRN_out << "Unknown input: '" << input << "'  Ignoring...";
      return false;
   }

   // If we got here, everything parsed ok.
   return true;
}
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Harry Rostovtsev. All rights reserved *****END OF FILE****/
