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
//   std::map<T, std::vector<std::string>>::const_iterator begin = allowedStrings<T>::m_allowedStrings.begin();
//   std::map<T, std::vector<std::string>>::const_iterator end = allowedStrings<T>::m_allowedStrings.end();
//   std::map<T, std::vector<std::string>>::const_iterator itr;

   auto begin  = std::begin(allowedStrings<T>::m_allowedStrings);
   auto end    = std::end(allowedStrings<T>::m_allowedStrings);
   auto itr    = begin;

   for( itr = begin; itr != end; ++itr ) {

      std::vector<std::string> tmpVector = itr->second;

      std::vector<std::string>::const_iterator v_begin = tmpVector.begin();
      std::vector<std::string>::const_iterator v_end = tmpVector.end();
      std::vector<std::string>::const_iterator v_itr;

//      std::vector<std::string>::const_iterator v_begin = itr->begin();
//      std::vector<std::string>::const_iterator v_end = itr->end();
//      std::vector<std::string>::const_iterator v_itr;

      for( v_itr = v_begin; v_itr != v_end; ++v_itr ) {
         if ( boost::iequals((*v_itr), str) ) {
            return( itr->first );
         }
      }
   }

   return (begin->first);

   // These two can be made easier to read in C++11
   // using std::begin() and std::end()
   //


//   if (find != end) {
//      data.enumVal = static_cast<T>(std::distance(begin, find));
//   }
//   return str;
}

/**
* @class Template class that converts enums to strings and back.
*
* Use the ability of function to deduce their template type without being
* explicitly told to create the correct type of enumRefHolder<T>
*/
template<typename T>
enumConstRefHolder1<T>  enumToString1(T const& e) {return enumConstRefHolder1<T>(e);}

template<typename T>
enumRefHolder1<T>       enumFromString1(T& e)     {return enumRefHolder1<T>(e);}

#endif                                                       /* ARGPARSE_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
