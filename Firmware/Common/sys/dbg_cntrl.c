/**
 * @file    dbg_cntrl.c
 * @brief   Module level debugging control.
 *
 * This file contains the definitions for debug and output functions over the
 * serial DMA console.
 *
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupDbgCntrl
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "dbg_cntrl.h"
#include "qp_port.h"                                        /* for QP support */
#include "DC3Signals.h"
#include "DC3Errors.h"
#include "Shared.h"                                   /*  Common Declarations */
#include "time.h"
#include "SerialMgr.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**< This "global" variable is used to keep track of all the modules which have
 * debugging enabled throughout the system.
 * @note 1: this var should not be accessed directly by the developer.
 * @note 2: this var should be set/cleared/checked by
 * #DBG_ENABLE_DEBUG_FOR_MODULE(), #DBG_DISABLE_DEBUG_FOR_MODULE(),
 * #DBG_TOGGLE_DEBUG_FOR_MODULE(), #DBG_CHECK_DEBUG_FOR_MODULE() macros only!
 */
CCMRAM_VAR uint32_t  glbDbgModuleConfig = 0;

/**< This "global" variable is used to keep track of what debug output devices
 * are currently enabled/disabled.
 * @note 1: this var should not be accessed directly by the developer.
 */
CCMRAM_VAR uint8_t   glbDbgDeviceConfig = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void DBG_setDefaults( void )
{
   if (DEBUG) {                                                  // Debug builds

      // Set the debug module settings
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_GEN);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_SER);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_TIME);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_ETH);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_I2C);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_I2C_DEV);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_NOR);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_SDRAM);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_COMM);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_CPLR);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_FLASH);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_SYS);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_DB);
      DBG_ENABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_DBG);

      // Set the devices
      DBG_ENABLE_DEVICE(_DC3_DBG_DEV_SER);
      DBG_ENABLE_DEVICE(_DC3_DBG_DEV_ETH);

   } else {                                                    // Release builds

      // Set the debug module settings
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_GEN);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_SER);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_TIME);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_ETH);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_I2C);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_I2C_DEV);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_NOR);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_SDRAM);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_COMM);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_CPLR);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_FLASH);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_SYS);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_DB);
      DBG_DISABLE_DEBUG_FOR_MODULE(DC3_DBG_MODL_DBG);

      // Set the devices
      DBG_DISABLE_DEVICE(_DC3_DBG_DEV_SER);
      DBG_ENABLE_DEVICE(_DC3_DBG_DEV_ETH);
   }
}

/**
 * @} end addtogroup groupDbgCntrl
 */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
