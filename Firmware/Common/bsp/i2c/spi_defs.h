/**
 * @file    spi_defs.h
 * @brief   Defines and types for SPI busses and devices.
 *
 * These have to be kept separate from the spi.h and spi_dev.h so that both can
 * include this without circular dependencies.
 *
 * @date    08/24/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupSPI
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_DEFS_H_
#define SPI_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "stm32f4xx_spi.h"                           /* For STM32 SPI support */
#include "bsp_defs.h"
#include "Shared.h"

/* Exported defines ----------------------------------------------------------*/
#define MAX_SPI_LEN 512                  /**< Max SPI trans/rec size in bytes */

/* Exported types ------------------------------------------------------------*/

/**
 * \enum I2C_Bus_t
 * I2C Busses available on the system.
 */
typedef enum SPI_Busses {
   SPIBus3  = 0,                                        /**< SPI Bus 3 (SPI3) */
// SPIBus5  = 1,                                        /**< SPI Bus 5 (SPI5) */
   MAX_SPI_BUS      /**< Maximum number of available SPI Busses on the system */
} SPI_Bus_t;

/**
 * \struct I2C_BusSettings_t
 * Most of the settings that are needed to set up all the hardware for each
 * I2C Bus.
 */
typedef struct SPI_BusSettings
{
   SPI_Bus_t               spi_sys_bus;     /**< System specifier for I2C Bus */



} SPI_BusSettings_t;


/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupSPI
 */

#ifdef __cplusplus
}
#endif

#endif                                                         /* SPI_DEFS_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
