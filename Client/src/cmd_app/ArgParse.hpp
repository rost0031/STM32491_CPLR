/**
 * @file    ArgParse.hpp
 * Parsing of secondary arguments input via Menu and CmdLine interface.
 *
 * @date    07/06/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ARGPARSE_HPP_
#define ARGPARSE_HPP_

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>
#include <typeinfo>

/* Boost includes */
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

/* App includes */
#include "Logging.hpp"
#include "Help.hpp"
#include "EnumMaps.hpp"
#include "DC3CommApi.h"
#include "ClientApi.h"

/* Namespaces ----------------------------------------------------------------*/
namespace po = boost::program_options;
using namespace po;

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


/**
 * @brief   Breaks apart a 'arg=value' pairs that are read in from the cmd line.
 *
 * @param [in] str: string& containing the text to break apart
 * @param [out] tokens: vector that will contain the tokens
 * @param [in] delims: a string of one or more delimiters.  Defaults to
 * whitespace.
 * @return: None
 */
void ARG_split(
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
bool ARG_getValue(
      string& value,
      const string& argName,
      const vector<string>& argVector
);

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
void ARG_checkCmdSpecificHelp(
      const string& cmd,
      const string& appName,
      const po::variables_map& vm,
      const bool isConnSet
);

/**
 * @brief   Function that can parse and check a filename with path.
 *
 * @throw <std::exception> if passed filename doesn't exist or can't be opened
 *
 * @param [out] value: string that will contain verified filename and path
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  None
 */
void ARG_parseFilenameStr(
      string& value,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
);

/**
 * @brief   Function that can parse and check a string hex array
 *
 * @throw <std::exception> if array can't be parsed and converted
 *
 * @param [out] *pDataArr: uint8_t pointer to array where to store the output
 * @param [out] *pDataArrLen: size_t pointer to how long the resulting array is.
 * @param [in] nDataArrMaxSize: size_t of max storage in pDataArr.
 * @param [in] defaultDataStr: string ref to the default string to parse
 * if there's an error parsing the user arg=value pair. This allows this param
 * to be optional.
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  None
 */
void ARG_parseHexArr(
      uint8_t* pDataArr,
      size_t* pDataArrLen,
      const size_t nDataArrMaxSize,
      string& defaultDataStr,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
);

/**
 * @brief   Function that can parse and check a string hex array
 *
 * @throw <std::exception> if array can't be parsed and converted
 *
 * @param [out] *pDataArr: uint8_t pointer to array where to store the output
 * @param [out] *pDataArrLen: size_t pointer to how long the resulting array is.
 * @param [in] nDataArrMaxSize: size_t of max storage in pDataArr.
 * @param [in] valueStr: const string ref to string containing hex values
 *
 * @return  None
 */
void ARG_parseHexStr(
      uint8_t* pDataArr,
      size_t* pDataArrLen,
      const size_t nDataArrMaxSize,
      string& valueStr
);

/**
 * @brief   A template function that interactively requests user input an array
 * of numbers.
 *
 * @throw <std::exception> if user passes in invalid values or format
 *
 * @param [out] *pDataArr: uint8_t pointer to array where to store the output
 * @param [out] *pDataArrLen: size_t pointer to how long the resulting array is.
 * @param [in] nDataArrMaxSize: size_t of max storage in pDataArr.
 * @param [in] msg: const string ref of the message to prompt the user with
 *
 * @return  None
 */
bool ARG_userInputArr(
      uint8_t* pDataArr,
      size_t* pDataArrLen,
      const size_t nDataArrMaxSize,
      const string& msg
);

/**
 * @brief   Storage for all the maps of vectors of allowed strings for enums.
 *
 * This is the type that will hold all the strings. Each enumerate type will
 * declare its own specialization. Any enum that does not have a specialization
 * will generate a compiler error indicating that there is no definition of
 * this variable (as there should be no definition of a generic version).
 */
template<typename T> struct allowedStrings
{
    static std::map<T, std::vector<std::string>> m_allowedStrings;
};


/**
 * @brief   Operator that does the actual conversion from enum to a string.
 * @param [in] str: ostream type to output to.
 * @param [in] data: template param of enum that is getting converted.
 * @return  stream with output in string form.
 *
 * @note: THIS ONLY WORKS IN C++11 and above.
 */
template<typename T> T ARG_getEnumFromAllowedStr(
      std::string& str,
      std::map< T, std::vector<std::string>>& m_allowedStrings
)
{
   // iterate over the map elements
   auto begin  = std::begin(allowedStrings<T>::m_allowedStrings);
   auto end    = std::end(allowedStrings<T>::m_allowedStrings);
   auto itr    = begin;

   for( itr = begin; itr != end; ++itr ) {

      // iterate over the elements of the vector
      auto v_begin  = std::begin(itr->second);
      auto v_end    = std::end(itr->second);
      auto v_itr    = v_begin;

      for( v_itr = v_begin; v_itr != v_end; ++v_itr ) {

         // found match
         if ( boost::iequals((*v_itr), str) ) {
            return( (itr->first) );
         }
      }
   }

   // Throw an exception if string was not found in the map.
   std::stringstream ss;
   ss << "value " << str << " not found in any of the allowed strings";
   throw std::invalid_argument(ss.str());
}

/**
 * @brief   A template function that can parse any enum
 *
 * @note:   Parsable enums must have associated maps created in the *.cpp file.
 *
 * @throw <std::exception> if passed in string is not found in the map of
 * allowed strings for a given enum type.
 *
 * @param [out] *value: T pointer to where to output the parsed value.
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  None
 */
template<typename T> void ARG_parseEnumStr(
      T* value,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
)
{
   // Extract the value from the arg=value pair
   string valueStr = "";

   if( !ARG_getValue( valueStr, arg, args ) ) {
      // if error happened, print cmd specific help and exit
      HELP_printCmdSpecific( cmd, appName );
   }

   // The template function ARG_getEnumFromAllowedStr will throw an exception of
   // the passed in value is not found in the appropriate map.  We'll rethrow it
   // if that happens.
   try {
      *value = ARG_getEnumFromAllowedStr(valueStr, allowedStrings<T>::m_allowedStrings);
   } catch (exception& e) {
      throw;
   }
}

/**
 * @brief   A template function that can parse any enum
 *
 * @note:   Parsable enums must have associated maps created in the *.cpp file.
 *
 * @throw <std::exception> if passed in string is not found in the map of
 * allowed strings for a given enum type.
 *
 * @param [out] *value: T pointer to where to output the parsed value.
 * @param [in] defaultValue: T default value to set if the argument can't be
 * parsed (because it was not included).
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  None
 */
template<typename T> void ARG_parseEnumStr(
      T* value,
      T defaultValue,
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
      *value = defaultValue;
      return;
   }

   // The template function ARG_getEnumFromAllowedStr will throw an exception of
   // the passed in value is not found in the appropriate map.  We'll rethrow it
   // if that happens.
   try {
      *value = ARG_getEnumFromAllowedStr(valueStr, allowedStrings<T>::m_allowedStrings);
   } catch (exception& e) {
      throw;
   }
}

/**
 * @brief   A template function that can parse any number
 *
 * @throw <std::exception> if passed in string is not one of alpha-numeric chars
 * or a - sign.
 *
 * @param [out] *value: T pointer to where to output the parsed value.
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  None
 */
template<typename T> void ARG_parseNumber( T* value, std::string str )
{
   if ( ( typeid(T) == typeid( int ) ) || ( typeid(T) == typeid( unsigned int ) ) ||
         ( typeid(T) == typeid( uint8_t ) ) || ( typeid(T) == typeid( uint16_t ) ) ||
         ( typeid(T) == typeid( uint32_t ) ) || ( typeid(T) == typeid( int8_t ) )  ||
         ( typeid(T) == typeid( int16_t ) )|| ( typeid(T) == typeid( int32_t ) ) ||
         ( typeid(T) == typeid( size_t ) )
   ) {
      if( (str.find_first_not_of( "0123456789abcdefABCDEFxX-" ) != string::npos) ) {
         std::stringstream ss;
         ss << "String '"<< str << "' is not numeric as expected";
         throw std::invalid_argument(ss.str());
      }
      char *end;
      *value = strtol(str.c_str(), &end, 0);
   }
}

/**
 * @brief   A template function that can parse any "arg=value" for numeric types
 *
 * @throw <std::exception> if passed in string is not found in the map of
 * allowed strings for a given enum type.
 *
 * @param [out] *value: T pointer to where to output the parsed value.
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  None
 */
template<typename T> void ARG_parseNumStr(
      T* value,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
)
{
   // Extract the value from the arg=value pair
   string valueStr = "";
   if( !ARG_getValue( valueStr, arg, args ) ) {
      // if error happened, print cmd specific help and exit
      HELP_printCmdSpecific( cmd, appName );
   }

   try {
      ARG_parseNumber(value, valueStr);
   } catch ( exception &e ) {
      throw;
   }
}

/**
 * @brief   A template function that can parse any "arg=value" for numeric types
 *
 * @throw <std::exception> if passed in string is not found in the map of
 * allowed strings for a given enum type.
 *
 * @param [out] *value: T pointer to where to output the parsed value.
 * @param [in] defaultValueStr: const string ref to default string value to use
 * if the user value can't be parsed.
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  None
 */
template<typename T> void ARG_parseNumStr(
      T* value,
      const string& defaultValueStr,
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
      ARG_parseNumber(value, defaultValueStr);
      return;
   }

   try {
      ARG_parseNumber(value, valueStr);
   } catch ( exception &e ) {
      throw;
   }
}

/**
 * @brief   A template function that interactively requests user input a number
 *
 * @throw <std::exception> if user passes in invalid value
 *
 * @param [out] *value: T pointer to where to output the parsed value.
 * @param [in] min: T that specifies the minimum value allowed
 * @param [in] max: T that specifies the maximum value allowed
 * @param [in] msg: const string ref of the message to prompt the user with
 *
 * @return  None
 */
template<typename T> bool ARG_userInputNum(
      T* value,
      T min,
      T max,
      const string& msg
)
{
   // Allows the generated template functions to use logging facilities
   CLI_MODULE_NAME( CLI_DBG_MODULE_ARG );

   stringstream ss;
   ss << msg << endl; // print the explanation msg and ask for the actual value
   ss << "Input a number between " << min << " and " << max << " or type 'cancel' to abort:";
   CON_print( ss.str() );

   string input;
   cin >> input;                             // Read input from user as a string

   if ( boost::iequals(input, "cancel") ) {
      return false;
   }

   try {
      ARG_parseNumber(value, input);
   } catch ( exception &e ) {
      WRN_out << "Caught exception trying to parse number.  Aborting...";
      return false;
   }

   // Don't allow negatives in our numeric input
   if ( *value < min || *value > max ) {
      WRN_out << "Value " << *value << " must be no less than " << min
            << " and no greater than " << max << ". Aborting...";
      return false;
   }

   // If we got here, everything parsed ok.
   return true;
}

/* Exported classes ----------------------------------------------------------*/



#endif                                                       /* ARGPARSE_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
