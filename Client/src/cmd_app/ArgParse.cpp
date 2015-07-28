/**
 * @file    ArgParse.cpp
 * Parsing of secondary arguments input via Menu and CmdLine interface.
 *
 * @date    07/06/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
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
MODULE_NAME( MODULE_EXT );

/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief All allowed strings for specifying CBBootMode enums
 */
template<> std::map<CBBootMode, std::vector<std::string>> allowedStrings<CBBootMode>::m_allowedStrings = {
      { _CB_NoBootMode   , {"None","invalid"} },
      { _CB_Bootloader   , {"b", "bo", "bt", "boo", "boot", "bootloader", "bootldr", "btldr" } },
      { _CB_Application  , {"a", "ap", "app", "appl", "application", "scips", "scipsiii", "scips3" } }
};

/**
 * @brief All allowed strings for specifying CBMsgRoute enums
 */
template<> std::map<CBMsgRoute, std::vector<std::string>> allowedStrings<CBMsgRoute>::m_allowedStrings = {
      { _CB_NoRoute   , {"None","no route"} },
      { _CB_Serial   , {"se", "serial", "ser"} },
      { _CB_EthSys  , {"sy", "sys", "sys_eth", "system_eth", "system_ethernet", "eth_sys", "ethernet_system", "eth_system"} },
      { _CB_EthLog  , {"l","lo","log", "log_eth", "logging_eth", "logging_ethernet", "eth_log", "ethernet_logging", "eth_logging"} },
      { _CB_EthCli  , {"c", "cl","cli", "cli_eth", "client_eth", "client_ethernet", "eth_cli", "ethernet_client", "eth_client"} }
};

/**
 * @brief All allowed strings for specifying CBI2CDevices enums
 */
template<> std::map<CBI2CDevices, std::vector<std::string>> allowedStrings<CBI2CDevices>::m_allowedStrings = {
      { _CB_EEPROM   , {"e","ee","eep","eeprom","eprom"} },
      { _CB_SNROM   , {"snrom", "sn", "ser_rom", "srom", "serial_rom", "serial_number_rom", } },
      { _CB_EUIROM  , {"euirom", "eui", "ui", "uie", "uierom", "eurom", "uirom", } },
};

/**
 * @brief All allowed strings for specifying CBAccessType enums
 */
template<> std::map<CBAccessType, std::vector<std::string>> allowedStrings<CBAccessType>::m_allowedStrings = {
      { _CB_ACCESS_BARE, {"b", "bare","metal", "bare_metal", "baremetal"} },
      { _CB_ACCESS_QPC , {"q", "qp", "qpc", } },
      { _CB_ACCESS_FRT , {"f", "fr", "frt", "freertos", "frtos", "free"} },

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

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
