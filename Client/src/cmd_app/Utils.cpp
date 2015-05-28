/**
 * @file    Utils.cpp
 * Various utilities for the cmd client
 *
 * A place to collect all the various utility classes and functions used by the
 * Client cmd line app.
 *
 * @date    05/27/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>
#include <vector>
#include <iterator>

/* App includes */
#include "Utils.hpp"

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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void UTIL_checkForCmdSpecificHelp(
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
void UTIL_splitArgs(
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
bool UTIL_getArgValue(
      string& value,
      const string& argName,
      const string& cmd,
      const string& appName,
      const po::variables_map& vm
)
{
   vector<string>::const_iterator begin = vm[cmd].as<vector<string>>().begin();
   vector<string>::const_iterator end = vm[cmd].as<vector<string>>().end();
   vector<string>::const_iterator itr;

   for( itr = begin; itr != end; ++itr ) {
      if ( (*itr).find("=") ) {
         vector<string> arg_pair;

         UTIL_splitArgs( (*itr), arg_pair, "=" );
         if (2 != arg_pair.size()) {                       // invalid arg format
            HELP_printCmdSpecific( cmd, appName );
         } else if ( 0 == arg_pair.at(0).compare(argName)) {
            // Do all the checking for whether it exists top level
            value = arg_pair.at(1);
            return true;
         }
      }
   }
   return false;
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
