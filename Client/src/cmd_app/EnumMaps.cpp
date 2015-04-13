/**
 * @file    EnumMaps.cpp
 * Template class that provides enum to string and back functionality.
 *
 * @date    04/06/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "EnumMaps.h"

#include "CBSharedDbgLevels.h"
#include "ClientModules.h"
#include "Logging.h"
#include <map>
#include <boost/assign.hpp>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief ModuleId_t -> string map
 * Non-consecutive enums cannot use the template code since the const char
 * array is indexed using the enum values which causes the lookup to exceed
 * array bounds.
 *
 * TODO: This should really be replaced with an implementation of enumToString
 * that uses a map instead of const char* array.
 */
std::map<ModuleId_t, std::string> m_ModuleId = boost::assign::map_list_of
      ( MODULE_GEN   , "Gen" )
      ( MODULE_SER   , "Ser" )
      ( MODULE_ETH   , "Eth" )
      ( MODULE_MGR   , "Mgr" )
      ( MODULE_LOG   , "Log" )
      ( MODULE_JOB   , "Job" )
      ( MODULE_API   , "Api" )
      ( MODULE_EXT   , "Ext" )
      ( MODULE_COM   , "Com" )
      ; //End of m_ModuleId map

/**
 * @brief ModuleSrc_t -> string map
 * Non-consecutive enums cannot use the template code since the const char
 * array is indexed using the enum values which causes the lookup to exceed
 * array bounds.
 *
 * TODO: This should really be replaced with an implementation of enumToString
 * that uses a map instead of const char* array.
 */
std::map<ModuleSrc_t, std::string> m_ModuleSrc = boost::assign::map_list_of
      ( SRC_CLI_EXT   , "Ext" )
      ( SRC_CLI_LIB   , "Lib" )
      ( SRC_DC3_APPL  , "App" )
      ( SRC_DC3_BOOT  , "Btl" )
      ; //End of m_ModuleSrc map

/**
 * @brief String values for DBG_LEVEL_T enums
 * String values for consecutive enums can be placed into const char* arrays
 * and a template will autogenerate the necessary enumToString function based
 * on the existence of this ::data[] array.
 * */
template<> char const* enumStrings<DBG_LEVEL_T>::data[] =
   {"DBG", "LOG", "WRN", "ERR", "CON", "ISR"};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
const std::string getModuleIdStr( const ModuleId_t a )
{
    map<ModuleId_t, std::string>::iterator it = m_ModuleId.find(a);
    if (it != m_ModuleId.end() ) {
       return it->second;
    } else {
       std::ostringstream s;
       s << a << " is not a valid ModuleId_t type";
       return s.str();
    }
}

/******************************************************************************/
const std::string getModuleSrcStr( const ModuleSrc_t a )
{
    map<ModuleSrc_t, std::string>::iterator it = m_ModuleSrc.find(a);

    if (it != m_ModuleSrc.end() ) {
       return it->second;
    } else {
       std::ostringstream s;
       s << a << " is not a valid ModuleId_t type";
       return s.str();
    }
}

/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
