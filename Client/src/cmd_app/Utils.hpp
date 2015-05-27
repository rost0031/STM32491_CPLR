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
#include <cstring>

#include <boost/program_options.hpp>

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
 * @param [in]
 */
void UTIL_checkForCmdSpecificHelp(
      const string& cmd,
      const string& appName,
      const po::variables_map& vm,
      const bool isConnSet
);

/* Exported classes ----------------------------------------------------------*/


#endif                                                          /* UTILS_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
