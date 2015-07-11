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
#include "CBCommApi.h"
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
template<typename T> T getEnumFromAllowedStr(
      std::string& str,
      std::map< T, std::vector<std::string>>& m_allowedStrings //enumRefHolder1<T> const& m_allowedStrings
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
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  T value: enumeration of type T that was parsed
 */
template<typename T> bool ARG_parseEnumStr(
      T* value,
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

   // The template function getEnumFromAllowedStr will throw an exception of
   // the passed in value is not found in the appropriate map.  We'll rethrow it
   // if that happens.
   try {
      *value = getEnumFromAllowedStr(valueStr, allowedStrings<T>::m_allowedStrings);
      return true;
   } catch (exception& e) {
      throw;
   }
   return false;
}

/**
 * @brief   A template function that can parse any enum
 *
 * @note:   Parsable enums must have associated maps created in the *.cpp file.
 *
 * @throw <std::exception> if passed in string is not found in the map of
 * allowed strings for a given enum type.
 *
 * @param [in] arg: argument name of the "arg=value" pair.
 * @param [in] cmd: parsed command.  Used for printing cmd specific help.
 * @param [in] appName: application name. Used for printing cmd specific help.
 * @param [in] args: vector of arguments for the parsed command.  Derived from
 * the program options vm map.
 *
 * @return  T value: enumeration of type T that was parsed
 */
template<typename T> bool ARG_parseNumStr(
      T* value,
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

   if ( ( typeid(T) == typeid( int ) ) || ( typeid(T) == typeid( unsigned int ) ) ||
        ( typeid(T) == typeid( uint8_t ) ) || ( typeid(T) == typeid( uint16_t ) ) ||
        ( typeid(T) == typeid( uint32_t ) ) || ( typeid(T) == typeid( int8_t ) )  ||
        ( typeid(T) == typeid( int16_t ) )|| ( typeid(T) == typeid( int32_t ) ) ||
        ( typeid(T) == typeid( size_t ) )
      ) {
      if( (valueStr.find_first_not_of( "0123456789" ) != string::npos) ) {
         std::stringstream ss;
         ss << "Input '"<< arg << "=" << valueStr << "' is not numeric as expected";
         throw std::invalid_argument(ss.str());
      }
      *value = atoi(valueStr.c_str());
      return true;
   }

   return false;
}

/* Exported classes ----------------------------------------------------------*/



#endif                                                       /* ARGPARSE_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
