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

/* Boost includes */
#include <boost/algorithm/string.hpp>

/* App includes */
#include "Logging.hpp"
#include "Help.hpp"
#include "EnumMaps.hpp"
#include "CBCommApi.h"
#include "ClientApi.h"

/* Namespaces ----------------------------------------------------------------*/
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

void ARG_parseCBBootMode(
       CBBootMode* value,
      const string& arg,
      const string& cmd,
      const string& appName,
      const vector<string>& args
);




//void ARG_parseSetMode(
//      ClientApi* client,
//      const string& cmd,
//      const string& appName,
//      const po::variables_map& vm
//);

void ARG_parseSetMode(
      ClientApi* client,
      const string& cmd,
      const string& appName
);



/* Exported classes ----------------------------------------------------------*/

/**
 * @brief   Storage for all the maps.
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
 * @brief   Utility type
 * Created automatically. Should not be used directly.
 */
template<typename T> struct enumRefHolder1
{
    T& enumVal1;
    enumRefHolder1(T& enumVal1): enumVal1(enumVal1) {}
};

/**
 * @brief   Utility type
 * Created automatically. Should not be used directly.
 */
template<typename T> struct enumConstRefHolder1
{
    T const& enumVal1;
    enumConstRefHolder1(T const& enumVal1): enumVal1(enumVal1) {}
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

   std::stringstream ss;
   ss << "value " << str << " not found in any of the allowed strings";
   throw std::invalid_argument(ss.str());
//   return (begin->first);
//   return(-1);
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
      throw e;
   }
   return false;
}

/**
* @class Template class that converts enums to strings and back.
*
* Use the ability of function to deduce their template type without being
* explicitly told to create the correct type of enumRefHolder<T>
*/
//template<typename T>
//enumConstRefHolder1<T>  enumToString1(T const& e) {return enumConstRefHolder1<T>(e);}
//
//template<typename T>
//enumRefHolder1<T>       enumFromString1(T& e)     {return enumRefHolder1<T>(e);}

#endif                                                       /* ARGPARSE_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
