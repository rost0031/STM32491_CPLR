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
/* System includes */
#include <map>

/* App includes */
#include "EnumMaps.hpp"
#include "Logging.hpp"
#include "KTree.hpp"

/* Lib includes */
#include "DC3CommApi.h"
#include "ClientModules.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief String values for ModuleId_t enums
 */
template<> std::map<ModuleId_t, std::string> enumStrings<ModuleId_t>::data = {
      { MODULE_GEN     , "Gen"   },
      { MODULE_SER     , "Ser"   },
      { MODULE_UDP     , "Udp"   },
      { MODULE_MGR     , "Mgr"   },
      { MODULE_LOG     , "Log"   },
      { MODULE_FWL     , "FwL"   },
      { MODULE_API     , "Api"   },
      { MODULE_EXT     , "Ext"   },
      { MODULE_COM     , "Com"   },
};

/**
 * @brief String values for ModuleSrc_t enums
 */
template<> std::map<ModuleSrc_t, std::string> enumStrings<ModuleSrc_t>::data = {
      { SRC_CLI_EXT    , "Ext"   },
      { SRC_CLI_LIB    , "Lib"   },
      { SRC_DC3_APPL   , "App"   },
      { SRC_DC3_BOOT   , "Btl"   },
};

/**
 * @brief String values for DC3MsgName_t enums
 */
template<> std::map<DC3MsgName_t, std::string> enumStrings<DC3MsgName_t>::data = {
      { _DC3NoMsg                , "None"             },
      { _DC3StatusPayloadMsg     , "StatusPayload"    },
      { _DC3GetVersionMsg        , "GetVersion"       },
      { _DC3VersionPayloadMsg    , "VersionPayload"   },
      { _DC3GetBootModeMsg       , "GetBootMode"      },
      { _DC3SetBootModeMsg       , "SetBootMode"      },
      { _DC3BootModePayloadMsg   , "BootModePayload"  },
};

/**
 * @brief String values for DC3MsgType_t enums
 */
template<> std::map<DC3MsgType_t, std::string> enumStrings<DC3MsgType_t>::data = {
      { _DC3_NoMsgType  , "None"    },
      { _DC3_Req        , "Req"     },
      { _DC3_Ack        , "Ack"     },
      { _DC3_Prog       , "Prog"    },
      { _DC3_Done       , "Done"    },
      { _DC3_MaxMsgType , "Invalid" },
};

/**
 * @brief String values for DC3MsgRoute_t enums
 */
template<> std::map<DC3MsgRoute_t, std::string> enumStrings<DC3MsgRoute_t>::data = {
      { _DC3_NoRoute    , "None"    },
      { _DC3_Serial     , "Serial"  },
      { _DC3_EthSys     , "TCPSys"  },
      { _DC3_EthLog     , "TCPLog"  },
      { _DC3_EthCli     , "UDPCli"  },
      { _DC3_MaxRoutes  , "Invalid" },
};

/**
 * @brief String values for DC3BootMode_t enums
 */
template<> std::map<DC3BootMode_t, std::string> enumStrings<DC3BootMode_t>::data = {
      { _DC3_NoBootMode    , "Invalid"       },
      { _DC3_SysRomBoot    , "SystemROM"     },
      { _DC3_Bootloader    , "Bootloader"    },
      { _DC3_Application   , "Application"   },
      { _DC3_MaxBootModes  , "Invalid"       },
};

/**
 * @brief String values for DC3I2CDevice_t enums
 */
template<> std::map<DC3I2CDevice_t, std::string> enumStrings<DC3I2CDevice_t>::data = {
      { _DC3_EEPROM     , "EEPROM" },
      { _DC3_SNROM      , "SNROM" },
      { _DC3_EUIROM     , "EUIROM" },
      { _DC3_MaxI2CDev  , "Invalid" },
};

/**
 * @brief String values for DC3AccessType_t enums
 */
template<> std::map<DC3AccessType_t, std::string> enumStrings<DC3AccessType_t>::data = {
      { _DC3_ACCESS_NONE   , "Invalid" },
      { _DC3_ACCESS_BARE   , "BARE" },
      { _DC3_ACCESS_QPC    , "QPC" },
      { _DC3_ACCESS_FRT    , "FRT" },
};


/**
 * @brief String values for DC3RamTest_t enums
 */
template<> std::map<DC3RamTest_t, std::string> enumStrings<DC3RamTest_t>::data = {
      { _DC3_RAM_TEST_NONE       , "None" },
      { _DC3_RAM_TEST_DATA_BUS   , "DataBusTest" },
      { _DC3_RAM_TEST_ADDR_BUS   , "AddrBusTest" },
      { _DC3_RAM_TEST_DEV_INT    , "DeviceTest" },
      { _DC3_RAM_TEST_MAX        , "InvalidTest" },
};

/**
 * @brief String values for MenuAction_t enums
 */
template<> std::map<MenuAction_t, std::string> enumStrings<MenuAction_t>::data = {
      { MENU_NO_ACTION                    , "No menu action" },
      { MENU_RAM_TEST                     , "Menu Ram Test" },
      { MENU_I2C_READ_TEST_DEF            , "Menu I2C Read Test (default params)" },
      { MENU_I2C_READ_TEST_CUS            , "Menu I2C Read Test (custom params)" },
      { MENU_I2C_WRITE_TEST_DEF           , "Menu I2C Write Test (default params)" },
      { MENU_I2C_WRITE_TEST_CUS           , "Menu I2C Write Test (custom params)" },
      { MENU_GET_MODE                     , "Menu Get DC3 mode" },
      { MENU_SET_APPL                     , "Menu Set DC3 mode to Application/SCIPS" },
      { MENU_SET_BOOT                     , "Menu Set DC3 mode to Bootloader" },
      { MENU_DC3_EN_ETH_DBG               , "Menu enable DC3 debugging output over ethernet" },
      { MENU_DC3_DIS_ETH_DBG              , "Menu disable DC3 debugging output over ethernet" },
      { MENU_DC3_EN_SER_DBG               , "Menu enable DC3 debugging output over serial" },
      { MENU_DC3_DIS_SER_DBG              , "Menu disable DC3 debugging output over serial" },
      { MENU_DC3_GET_DBG_MODULES          , "Menu get DC3 debug module states" },

      { MENU_DC3_EN_GEN_DBG_MODULE        , "Menu enable DC3 general module debugging state" },
      { MENU_DC3_DIS_GEN_DBG_MODULE       , "Menu disable DC3 general module debugging state" },
      { MENU_DC3_EN_SER_DBG_MODULE        , "Menu enable DC3 serial module debugging state" },
      { MENU_DC3_DIS_SER_DBG_MODULE       , "Menu disable DC3 serial module debugging state" },
      { MENU_DC3_EN_TIME_DBG_MODULE       , "Menu enable DC3 time module debugging state" },
      { MENU_DC3_DIS_TIME_DBG_MODULE      , "Menu disable DC3 time module debugging state" },
      { MENU_DC3_EN_ETH_DBG_MODULE        , "Menu enable DC3 ethernet module debugging state" },
      { MENU_DC3_DIS_ETH_DBG_MODULE       , "Menu disable DC3 ethernet module debugging state" },
      { MENU_DC3_EN_I2C_DBG_MODULE        , "Menu enable DC3 I2C Bus module debugging state" },
      { MENU_DC3_DIS_I2C_DBG_MODULE       , "Menu disable DC3 I2C Bus module debugging state" },
      { MENU_DC3_EN_I2C_DEV_DBG_MODULE    , "Menu enable DC3 I2C Device module debugging state" },
      { MENU_DC3_DIS_I2C_DEV_DBG_MODULE   , "Menu disable DC3 I2C Device module debugging state" },
      { MENU_DC3_EN_COMM_DBG_MODULE       , "Menu enable DC3 CommMgr AO module debugging state" },
      { MENU_DC3_DIS_COMM_DBG_MODULE      , "Menu disable DC3 CommMgr AO module debugging state" },
      { MENU_DC3_EN_CPLR_DBG_MODULE       , "Menu enable DC3 Coupler Main module debugging state" },
      { MENU_DC3_DIS_CPLR_DBG_MODULE      , "Menu disable DC3 Coupler Main module debugging state" },
      { MENU_DC3_EN_DB_DBG_MODULE         , "Menu enable DC3 Database & Settings module debugging state" },
      { MENU_DC3_DIS_DB_DBG_MODULE        , "Menu disable DC3 Database & Settings module debugging state" },
      { MENU_DC3_EN_FLASH_DBG_MODULE      , "Menu enable DC3 FlashMgr AO and driver module debugging state" },
      { MENU_DC3_DIS_FLASH_DBG_MODULE     , "Menu disable DC3 FlashMgr AO and driver module debugging state" },
      { MENU_DC3_EN_SYS_DBG_MODULE        , "Menu enable DC3 SysMgr AO module debugging state" },
      { MENU_DC3_DIS_SYS_DBG_MODULE       , "Menu disable DC3 SysMgr AO module debugging state" },
      { MENU_DC3_EN_NOR_DBG_MODULE        , "Menu enable DC3 NOR module debugging state" },
      { MENU_DC3_DIS_NOR_DBG_MODULE       , "Menu disable DC3 NOR module debugging state" },
      { MENU_DC3_EN_SDRAM_DBG_MODULE      , "Menu enable DC3 SDRAM module debugging state" },
      { MENU_DC3_DIS_SDRAM_DBG_MODULE     , "Menu disable DC3 SDRAM module debugging state" },
      { MENU_DC3_EN_DBGS_DBG_MODULE       , "Menu enable DC3 Debug system module debugging state" },
      { MENU_DC3_DIS_DBGS_DBG_MODULE      , "Menu disable DC3 Debug system module debugging state" },
};

/**
 * @brief DC3DbgLevel_t enum to strings map
 */
template<> std::map<DC3DbgLevel_t, std::string> enumStrings<DC3DbgLevel_t>::data = {
      { _DC3_DBG   , "DBG" },
      { _DC3_LOG   , "LOG" },
      { _DC3_WRN   , "WRN" },
      { _DC3_ERR   , "ERR" },
      { _DC3_CON   , "CON" },
      { _DC3_ISR   , "ISR" },
};

/**
 * @brief DC3DbgModule_t enum to strings map
 */
template<> std::map<DC3DbgModule_t, std::string> enumStrings<DC3DbgModule_t>::data = {
      { DC3_DBG_MODL_GEN      , "GENERAL Module"   },
      { DC3_DBG_MODL_SER      , "SERIAL Module"    },
      { DC3_DBG_MODL_TIME     , "TIME Module"      },
      { DC3_DBG_MODL_ETH      , "ETH Module"       },
      { DC3_DBG_MODL_I2C      , "I2C BUS Module"   },
      { DC3_DBG_MODL_I2C_DEV  , "I2C DEV Module"   },
      { DC3_DBG_MODL_NOR      , "NOR/FPGA Module"  },
      { DC3_DBG_MODL_SDRAM    , "SDRAM Module"     },
      { DC3_DBG_MODL_DBG      , "DEBUG_OUT Module" },
      { DC3_DBG_MODL_COMM     , "COMMMGR Module"   },
      { DC3_DBG_MODL_CPLR     , "CPLR Main Module" },
      { DC3_DBG_MODL_DB       , "DATABASE Module"  },
      { DC3_DBG_MODL_FLASH    , "FLASH Module"     },
      { DC3_DBG_MODL_SYS      , "SYSMGR Module"    },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private class methods -----------------------------------------------------*/


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
