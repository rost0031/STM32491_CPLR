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

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

template<> char const* enumStrings<DBG_LEVEL_T>::data[] =
   {"DBG", "LOG", "WRN", "ERR", "CON", "ISR"};

template<> char const* enumStrings<ModuleId_t>::data[] =
   {"Gen", "Ser", "Eth", "Mgr", "Log", "Api", "Ext"};

template<> char const* enumStrings<ModuleSrc_t>::data[] =
   {"LibCaller", "Lib", "DC3_ApplFW", "DC3_BootFW"};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
