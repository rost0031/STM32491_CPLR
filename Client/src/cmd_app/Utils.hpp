/**
 * @file    Utils.hpp
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UTILS_HPP_
#define UTILS_HPP_

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>
#include <typeinfo>

/* Boost includes */
#include <boost/program_options.hpp>

/* App includes */
#include "Logging.hpp"
#include "Help.hpp"
#include "EnumMaps.hpp"
#include "CBCommApi.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

namespace po = boost::program_options;
using namespace po;

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Check a vector type boost arg list for cmd specific help.
 * This utility function checks a vector type boost arg list for cmd specific
 * help and prints it if it exists.
 * @note: currently, the cmd specific help exits after quitting.
 *
 * @param [in] cmd: ref to a string containing the parsed command.
 * @param [in] appName: ref to a string containing the name of the application.
 * @param [in] vm: boost program options variables map that contains all the
 * parsed arguments.
 * @param [in] isConnSet: bool that specifies whether the connection options
 * have been specified.
 * @return  None.
 */
void UTIL_checkForCmdSpecificHelp(
      const string& cmd,
      const string& appName,
      const po::variables_map& vm,
      const bool isConnSet
);

/**
 * @brief   Breaks apart a 'arg=value' pairs that are read in from the cmd line.
 *
 * @param [in] str: string& containing the text to break apart
 * @param [out] tokens: vector that will contain the tokens
 * @param [in] delims: a string of one or more delimiters.  Defaults to
 * whitespace.
 * @return: None
 */
void UTIL_splitArgs(
      const std::string& str,
      std::vector<string>& tokens,
      const std::string& delims
);


/**
 * @brief   Template function that extracts the value from an arg=value pair.
 */
bool UTIL_getArgValue(
      string& value,
      const string& argName,
      const string& cmd,
      const string& appName,
      const po::variables_map& vm
);

///**
// *
// */
//template<typename T>
//const bool getArgValueTYPE(
//      T& value,
//      const string& argName,
//      const string& cmd,
//      const string& appName,
//      const po::variables_map& vm
//)
//{
//
//   string tmpValue = "";
//
//   UTIL_getArgValue(tmpValue, argName, cmd, appName, vm);
//
//   // Some args are optional and we don't want to error out
//   if ( 0 == tmpValue.compare("") ) {
//      return false;
//   }
//
//   // Have to check types here.  It's ugly but it works.
//   if ( typeid(value) == string ) {
//      value = tmpValue;
//   } else if ( typeid(value) == CBBootMode ) {
//      if (0 == tmpValue.compare("Bootloader")) {
//         value = _CB_Bootloader;
//      } else if (0 == tmpValue.compare("Application")) {
//         value = _CB_Application;
//      } else if (0 == tmpValue.compare("SysRomBoot")) {
//         value = _CB_SysRomBoot;
//      }  else {
//         value = _CB_MaxBootModes;
//      }
//   }
//
//   return true;
//}


/* Exported classes ----------------------------------------------------------*/


#endif                                                          /* UTILS_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
