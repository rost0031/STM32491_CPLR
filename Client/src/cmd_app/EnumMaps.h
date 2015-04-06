/**
 * @file    EnumMaps.cpp
 * Template class that provides enum to string and back functionality.
 *
 * @date    04/06/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ENUMMAPS_H_
#define ENUMMAPS_H_

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
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
   return str << enumStrings<T>::data[data.enumVal];
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
    static auto begin  = std::begin(enumStrings<T>::data);
    static auto end    = std::end(enumStrings<T>::data);

    auto find   = std::find(begin, end, value);
    if (find != end)
    {
        data.enumVal = static_cast<T>(std::distance(begin, find));
    }
    return str;
}
/* Exported functions --------------------------------------------------------*/
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
