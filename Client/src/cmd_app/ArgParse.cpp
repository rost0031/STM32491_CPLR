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
      { _CB_Bootloader   , {"boot", "bootloader", "bootldr", "btldr" } },
      { _CB_Application  , {"app", "application", "appl", "scips", "scipsiii", "scips3" } }
};

/**
 * @brief All allowed strings for specifying CBMsgRoute enums
 */
template<> std::map<CBMsgRoute, std::vector<std::string>> allowedStrings<CBMsgRoute>::m_allowedStrings = {
      { _CB_NoRoute   , {"None","no route"} },
      { _CB_Serial   , {"serial", "ser"} },
      { _CB_EthSys  , {"sys", "sys_eth", "system_eth", "system_ethernet", "eth_sys", "ethernet_system", "eth_system"} },
      { _CB_EthLog  , {"log", "log_eth", "logging_eth", "logging_ethernet", "eth_log", "ethernet_logging", "eth_logging"} },
      { _CB_EthCli  , {"cli", "cli_eth", "client_eth", "client_ethernet", "eth_cli", "ethernet_client", "eth_client"} }
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

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
