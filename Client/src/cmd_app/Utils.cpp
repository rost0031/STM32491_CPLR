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
#include "Utils.hpp"
#include "Help.hpp"
#include "Logging.hpp"
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
   if (vm.count("help") || !isConnSet) {  /* Check for command specific help req */
      HELP_printCmdSpecific( cmd, appName );
   }

   vector<string>::const_iterator begin = vm[cmd].as<vector<string>>().begin();
   vector<string>::const_iterator end = vm[cmd].as<vector<string>>().end();
   vector<string>::const_iterator itr;
   /* Check for command specific help req */
   if( find(begin, end, "--help") != end || find(begin, end, "help") != end  || !isConnSet) {
      HELP_printCmdSpecific( cmd, appName );
   }
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
