/**
 * @file    EnumMaps.hpp
 * Template class that provides enum to string and back functionality.
 *
 * @date    04/06/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ENUMMAPS_HPP_
#define ENUMMAPS_HPP_

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <typeinfo>

/* Boost includes */
#include <boost/fusion/container/map.hpp>

/* Api includes */
#include "ClientModules.h"

/* App includes */
#include "Logging.hpp"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
//using namespace boost::fusion;
/**
 * @brief   Storage for all the maps.
 *
 * This is the type that will hold all the strings. Each enumerate type will
 * declare its own specialization. Any enum that does not have a specialization
 * will generate a compiler error indicating that there is no definition of
 * this variable (as there should be no definition of a generic version).
 */
template<typename T> struct enumStrings
{
    static char const* data[];
//    static boost::fusion::map<T, std::string> m_map;
};

/**
 * @brief   Utility type
 * Created automatically. Should not be used directly.
 */
template<typename T> struct enumRefHolder
{
    T& enumVal;
    enumRefHolder(T& enumVal): enumVal(enumVal) {}
};

/**
 * @brief   Utility type
 * Created automatically. Should not be used directly.
 */
template<typename T> struct enumConstRefHolder
{
    T const& enumVal;
    enumConstRefHolder(T const& enumVal): enumVal(enumVal) {}
};


/**
 * This function returns a string representation of the ModuleId_t enum.
 *
 * @param  a: A ModuleId_t enum
 * @return: a string representation of the ModuleId_t enum
 */
//std::ostream getModuleIdStr( const ModuleId_t a );

/**
 * @brief   Operator that does the actual conversion from enum to a string.
 * @param [in] str: ostream type to output to.
 * @param [in] data: template param of enum that is getting converted.
 * @return  stream with output in string form.
 */
template<typename T> std::ostream& operator<<(
      std::ostream& str,
      enumConstRefHolder<T> const& data
)
{
   return( str << enumStrings<T>::data[data.enumVal] );
}

/**
 * @brief   Operator that does the actual conversion from enum to a string.
 * @param [in] str: ostream type to output to.
 * @param [in] data: template param of enum that is getting converted.
 * @return  stream with output in string form.
 *
 * @note: THIS ONLY WORKS IN C++11 and above.
 */
template<typename T> std::istream& operator>>(
      std::istream& str,
      enumRefHolder<T> const& data
)
{
    std::string value;
    str >> value;

    // These two can be made easier to read in C++11
    // using std::begin() and std::end()
    //
    auto begin  = std::begin(enumStrings<T>::data);
    auto end    = std::end(enumStrings<T>::data);
    auto find   = std::find(begin, end, value);

    if (find != end) {
        data.enumVal = static_cast<T>(std::distance(begin, find));
    }
    return str;
}
/* Exported functions --------------------------------------------------------*/
const std::string getModuleIdStr( const ModuleId_t a );
const std::string getModuleSrcStr( const ModuleSrc_t a );

/*! \p getParamString : Figures out which getString function to call based on
 * the type passed in.  The types are defined in the Redwood API redwood_msgs.h
 * file.
 *
 * \param [in] value: an enum of a type that exists in redwood_msgs.h.
 * \tparam: Any enum type that exists in redwood_msgs.h.  Can be one of the
 * following:
 *   @RespType
 *
 * @code
 * string val = getParamString< WhichStepMtr > (  _InterstageDrive );
 * cout << "val is " << val << endl;
 * int invalid_val = getParamValue< WhichStepMtr > (  4 );
 * cout << "invalid_val is " << invalid_val << endl;
 * @endcode
 *
 * Output:
 * val is "_InterstageDrive"
 * invalid_val is "Unknown type"
 *
 * "val" is 4 since _InterstageDrive corresponds to a value of 4 in the
 * WhichStepMtr enum set.
 * "invalid_val" passed in an invalid string so the out put is a string
 * "Unknown type ..." - This won't compile even.
 */
template<typename T> const std::string maskableEnumToString( T value )
{
   if ( typeid(T) == typeid( ModuleId_t ) )   {
      return( getModuleIdStr( (ModuleId_t) value) );
   } else if ( typeid(T) == typeid( ModuleSrc_t ) ) {
      return( getModuleSrcStr( (ModuleSrc_t) value) );
   } else {
      std::ostringstream oss(NULL);
      oss << typeid(T).name() <<"(" << value << ")" << "unknown";
      return  oss.str();
   }
};

/* Exported classes ----------------------------------------------------------*/
/**
* @class Template class that converts enums to strings and back.
*
* Use the ability of function to deduce their template type without being
* explicitly told to create the correct type of enumRefHolder<T>
*/
template<typename T>
enumConstRefHolder<T>  enumToString(T const& e) {return enumConstRefHolder<T>(e);}

template<typename T>
enumRefHolder<T>       enumFromString(T& e)     {return enumRefHolder<T>(e);}

#endif                                                         /* ENUMMAPS_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
