/**
 * @file 	spi.h
 * @brief   Basic driver for SPI bus.
 *
 * @date   	08/24/2015
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupSPI
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_H_
#define SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"                                 /* For STM32F4 support */
#include "bsp.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Macro to determine if an I2C bus is defined in the system
 * @param [in] BUS:  I2C_Bus_t type I2C bus specifier.
 * @retval
 *    1: Bus exists and is valid
 *    0: Bus doesn't exist or isn't defined
 */
#define IS_SPI_BUS(BUS) ((BUS) == I2CBus1)

/**
 * @brief   Macro as a wrapper for the I2C Timeout callback.
 * This macro is a wrapper around the I2C_TIMEOUT_UserCallbackRaw() function.
 * This wrapper grabs the function name and line where the callback was called
 * from an passes it to the callback.
 * @param [in] bus: I2C_Bus_t type that specifies what i2c bus the error occurred on.
 * @param [in] error: DC3Error_t that specifies the error that occurred.
 * @return None
 */
#define I2C_TIMEOUT_UserCallback( bus, error ) \
      I2C_TIMEOUT_UserCallbackRaw(bus, error, __func__, __LINE__);

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint8_t spi1RxBuffer[];          /**< Exported to I2CMgr I2C RX buffer */
extern uint8_t spi1TxBuffer[];          /**< Exported to I2CMgr I2C TX buffer */
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Initialize specified I2C bus
 *
 * This function initializes GPIO, clocks, and all the associated hardware for
 * a specified I2C bus interface.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus to initialize
 *    @arg
 * @return: None
 */
void SPI_BusInit( SPI_Bus_t iBus );

/**
 * @brief   DeInitialize specified I2C bus
 *
 * This function initializes GPIO used by I2C for input, disables clocks (except
 * for the SYSCFG clock), and all the associated hardware for a specified I2C
 * bus interface.
 *
 * @param [in]  iBus: I2C_Bus_t identifier for I2C bus to de-initialize
 *    @arg
 * @return: None
 */
void SPI_BusDeInit( SPI_Bus_t iBus );


/******************************************************************************/
/***                      Callback functions for I2C                        ***/
/******************************************************************************/


/**
 * @}
 * end addtogroup groupSPI
 */

#ifdef __cplusplus
}
#endif

#endif                                                              /* SPI_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
