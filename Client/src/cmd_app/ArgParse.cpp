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

/* Boost includes */
#include <boost/algorithm/string.hpp>

/* App includes */
#include "ArgParse.hpp"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private variables and Local objects ---------------------------------------*/


template<> std::map<CBBootMode, std::vector<std::string>> allowedStrings<CBBootMode>::m_allowedStrings = {
      { _CB_NoBootMode   , {"None","invalid"} },
      { _CB_Bootloader   , {"boot", "bootloader", "bootldr", "btldr" } },
      { _CB_Application  , {"app", "application", "appl", "scips", "scipsiii", "scips3" } }
}; //End of map


template<> std::map<CBMsgRoute, std::vector<std::string>> allowedStrings<CBMsgRoute>::m_allowedStrings = {
      { _CB_NoRoute   , {"None","no route"} },
      { _CB_Serial   , {"serial", "ser"} },
      { _CB_EthSys  , {"sys", "sys_eth", "system_eth", "system_ethernet", "eth_sys", "ethernet_system", "eth_system"} },
      { _CB_EthLog  , {"log", "log_eth", "logging_eth", "logging_ethernet", "eth_log", "ethernet_logging", "eth_logging"} },
      { _CB_EthCli  , {"cli", "cli_eth", "client_eth", "client_ethernet", "eth_cli", "ethernet_client", "eth_client"} }
}; //End of m_ModuleSrc map




/* Private function prototypes -----------------------------------------------*/

/**
 * @brief   Breaks apart a 'arg=value' pairs that are read in from the cmd line.
 *
 * @param [in] str: string& containing the text to break apart
 * @param [out] tokens: vector that will contain the tokens
 * @param [in] delims: a string of one or more delimiters.  Defaults to
 * whitespace.
 * @return: None
 */
static void ARG_split(
      const std::string& str,
      std::vector<string>& tokens,
      const std::string& delims
);

/**
 * @brief   Extract the value from an arg=value pair.
 * @param [out] value: string ref to where to store the output of the value
 * @param [in] appName: const string ref that contains the application name.
 * Used for printing command specific help
 */
static bool ARG_getValue(
      string& value,
      const string& argName,
      const vector<string>& argVector
);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
static void ARG_split(
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
static bool ARG_getValue(
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
void ARG_parseCBBootMode(
      CBBootMode* value,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
)
{
   // Extract the value from the arg=value pair
//   CBBootMode mode = _CB_NoBootMode;
   string valueStr = "";
   bool isExtracted = ARG_getValue( valueStr, arg, args );

   if( !isExtracted ) {   // if error happened, print cmd specific help and exit
      HELP_printCmdSpecific( cmd, appName );
   }


   try {
      *value = getEnumFromAllowedStr(valueStr, allowedStrings<CBBootMode>::m_allowedStrings);
      DBG_out << "*value =" << *value << " with string value: " << enumToString(*value);
   } catch (exception& e) {
      ERR_out << "Caught exception: " << e.what();
   }

//   int someVal = getEnumFromAllowedStr(valueStr, allowedStrings<CBBootMode>::m_allowedStrings);
//   DBG_out << "*someVal =" << someVal << " with string value: " << enumToString((CBBootMode)someVal);


   // be very forgiving with what we allow for arguments
//   if(   boost::iequals(valueStr, "bootloader") ||
//         boost::iequals(valueStr, "boot") ||
//         boost::iequals(valueStr, "btldr") ) {
//      *value = _CB_Bootloader;
//   } else if ( boost::iequals(valueStr, "application") ||
//               boost::iequals(valueStr, "app") ||
//               boost::iequals(valueStr, "appl") ||
//               boost::iequals(valueStr, "scipsiii") ||
//               boost::iequals(valueStr, "scips") ) {
//      *value = _CB_Application;
//   } else {
//      ERR_out << "Only " << enumToString(_CB_Bootloader) << " and "
//            << enumToString(_CB_Application) << " boot modes supported.";
//      HELP_printCmdSpecific( cmd, appName );
//   }
}


///******************************************************************************/
//template< typename T> void ARG_parseEnum(
//      typename T* value,
//      const string& arg,
//      const string& cmd,
//      const string& appName,
//      const vector<string>& args
//)
//{
//   // Extract the value from the arg=value pair
////   CBBootMode mode = _CB_NoBootMode;
//   string valueStr = "";
//   bool isExtracted = ARG_getValue( valueStr, arg, args );
//
//   if( !isExtracted ) {   // if error happened, print cmd specific help and exit
//      HELP_printCmdSpecific( cmd, appName );
//   }
//
//   // be very forgiving with what we allow for arguments
//   if(   boost::iequals(value, "bootloader") ||
//         boost::iequals(value, "boot") ||
//         boost::iequals(value, "btldr") ) {
//      *value = _CB_Bootloader;
//   } else if ( boost::iequals(value, "application") ||
//               boost::iequals(value, "app") ||
//               boost::iequals(value, "appl") ||
//               boost::iequals(value, "scipsiii") ||
//               boost::iequals(value, "scips") ) {
//      *value = _CB_Application;
//   } else {
//      ERR_out << "Only " << enumToString(_CB_Bootloader) << " and "
//            << enumToString(_CB_Application) << " boot modes supported.";
//      HELP_printCmdSpecific( cmd, appName );
//   }
//}

/******************************************************************************/
void ARG_parseSetMode(
      ClientApi* client,
      const string& cmd,
      const string& appName
)
{

}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
