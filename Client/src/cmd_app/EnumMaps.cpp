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
      ( MODULE_UDP   , "Udp" )
      ( MODULE_MGR   , "Mgr" )
      ( MODULE_LOG   , "Log" )
      ( MODULE_FWL   , "FwL" )
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

/**
 * @brief String values for CBMsgName enums
 * String values for consecutive enums can be placed into const char* arrays
 * and a template will autogenerate the necessary enumToString function based
 * on the existence of this ::data[] array.
 * */
template<> char const* enumStrings<CBMsgName>::data[] =  {
         "None",                          /**< _CBNoMsg */
         "StatusPayload",                 /**< _CBStatusPayloadMsg */
         "GetVersion",                    /**< _CBGetVersionMsg */
         "VersionPayload",                /**< _CBVersionPayloadMsg */
         "GetBootMode",                   /**< _CBGetBootModeMsg */
         "SetBootMode",                   /**< _CBSetBootModeMsg */
         "BootModePayload"                /**< _CBBootModePayloadMsg */
};

/**
 * @brief String values for CBMsgType enums
 * String values for consecutive enums can be placed into const char* arrays
 * and a template will autogenerate the necessary enumToString function based
 * on the existence of this ::data[] array.
 * */
template<> char const* enumStrings<CBMsgType>::data[] =  {
         "None",                          /**< _CB_NoMsgType */
         "Req",                           /**< _CB_Req */
         "Ack",                           /**< _CB_Ack */
         "Prog",                          /**< _CB_Prog */
         "Done",                          /**< _CB_Done */
         "Invalid"                        /**< _CB_MaxMsgType */
};

/**
 * @brief String values for CBMsgRoute enums
 * String values for consecutive enums can be placed into const char* arrays
 * and a template will autogenerate the necessary enumToString function based
 * on the existence of this ::data[] array.
 * */
template<> char const* enumStrings<CBMsgRoute>::data[] =  {
         "None",                          /**< _CB_NoRoute */
         "Serial",                        /**< _CB_Serial */
         "TCPSys",                        /**< _CB_EthSys */
         "TCPLog",                        /**< _CB_EthLog */
         "UDPCli",                        /**< _CB_EthCli */
         "Invalid"                        /**< _CB_MaxRoutes */
};

/**
 * @brief String values for CBMsgRoute enums
 * String values for consecutive enums can be placed into const char* arrays
 * and a template will autogenerate the necessary enumToString function based
 * on the existence of this ::data[] array.
 * */
template<> char const* enumStrings<CBBootMode>::data[] =  {
         "Invalid",                       /**< _CB_NoBootMode */
         "SystemROM",                     /**< _CB_SysRomBoot */
         "Bootloader",                    /**< _CB_Bootloader */
         "Application",                   /**< _CB_Application */
         "Invalid"                        /**< _CB_MaxBootModes */
};

/**
 * @brief String values for CBI2CDevices enums
 * String values for consecutive enums can be placed into const char* arrays
 * and a template will autogenerate the necessary enumToString function based
 * on the existence of this ::data[] array.
 * */
template<> char const* enumStrings<CBI2CDevices>::data[] =  {
         "EEPROM",                /**< _CB_EEPROM */
         "SNROM",                 /**< _CB_SNROM */
         "EUIROM",                /**< _CB_EUIROM */
         "Invalid device",        /**< _CB_MaxI2CDev */
};

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
