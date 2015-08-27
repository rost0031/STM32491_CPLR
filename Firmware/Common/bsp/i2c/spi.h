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
#include "spi_defs.h"

/* Exported defines ----------------------------------------------------------*/
#define SPI_FLAG_MAX_RETRIES  255 /**< Max number of times to poll SPI HW flags */

/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @brief   Macro to determine if an I2C bus is defined in the system
 * @param [in] BUS:  I2C_Bus_t type I2C bus specifier.
 * @retval
 *    1: Bus exists and is valid
 *    0: Bus doesn't exist or isn't defined
 */
#define IS_SPI_BUS(BUS) ((BUS) == SPIBus1)

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint8_t spi1RxBuffer[];          /**< Exported to I2CMgr I2C RX buffer */
extern uint8_t spi1TxBuffer[];          /**< Exported to I2CMgr I2C TX buffer */
/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Initialize specified SPI bus
 *
 * This function initializes GPIO, clocks, and all the associated hardware for
 * a specified SPI bus interface.
 *
 * @param [in]  iBus: SPI_Bus_t identifier for SPI bus to initialize
 *    @arg SPIBus5: SPI bus 5
 * @return: None
 */
void SPI_BusInit( SPI_Bus_t iBus );

/**
 * @brief   DeInitialize specified SPI bus
 *
 * This function initializes GPIO used by I2C for input, disables clocks (except
 * for the SYSCFG clock), and all the associated hardware for a specified I2C
 * bus interface.
 *
 * @param [in]  iBus: SPI_Bus_t identifier for SPI bus to de-initialize
 *    @arg SPIBus5: SPI bus 5
 * @return: None
 */
void SPI_BusDeInit( SPI_Bus_t iBus );

/**
 * @brief   Set the NSS pin for specified SPI bus high
 *
 * @param [in]  iBus: SPI_Bus_t identifier for SPI bus set the NSS pin high for
 *    @arg SPIBus5: SPI bus 5
 * @return: None
 */
void SPI_NSS_high( SPI_Bus_t iBus );

/**
 * @brief   Set the NSS pin for specified SPI bus low
 *
 * @param [in]  iBus: SPI_Bus_t identifier for SPI bus set the NSS pin low for
 *    @arg SPIBus5: SPI bus 5
 * @return: None
 */
void SPI_NSS_low( SPI_Bus_t iBus );

/**
 * @brief   Send a buffer over SPI
 *
 * @param [in] iBus: SPI_Bus_t identifier for SPI bus
 *    @arg SPIBus5: SPI bus 5
 * @param [in] bytesToTX: const uint16_t number of bytes to transmit
 * @param [in] bufferTXSize: const uint16_t size of the transmit buffer
 * @param [in] pBufferTX: const uint8_t const pointer to buffer where data to
 * be transmitted is stored.
 * @param [out] pBytesTXed: uint16_t pointer to how many bytes were transmitted.
 * @return: DC3Error_t status of the operation
 *    @arg  ERR_NONE: success
 *    other error codes if error occurred
 */
DC3Error_t SPI_send(
      const SPI_Bus_t iBus,
      const uint16_t bytesToTX,
      const uint16_t bufferTXSize,
      const uint8_t* const pBufferTX,
      uint16_t* pBytesTXed
);

/**
 * @brief   Transceive (Send and Receive) a buffer over SPI
 *
 * @param [in] iBus: SPI_Bus_t identifier for SPI bus
 *    @arg SPIBus5: SPI bus 5
 * @param [in] bytesToTX: const uint16_t number of bytes to transmit
 * @param [in] bufferTXSize: const uint16_t size of the transmit buffer
 * @param [in] pBufferTX: const uint8_t const pointer to buffer where data to
 * be transmitted is stored.
 * @param [out] pBytesTXed: uint16_t pointer to how many bytes were transmitted.
 * @param [in] bytesToRX: const uint16_t number of bytes to receive
 * @param [in] bufferRXSize: const uint16_t size of the receive buffer
 * @param [in] pBufferRX: uint8_t const pointer to buffer where data to store
 * received data.
 * @param [out] pBytesRXed: uint16_t pointer to how many bytes were received.
 * @return: DC3Error_t status of the operation
 *    @arg  ERR_NONE: success
 *    other error codes if error occurred
 */
DC3Error_t SPI_transceive(
      const SPI_Bus_t iBus,
      const uint16_t bytesToTX,
      const uint16_t bufferTXSize,
      const uint8_t* const pBufferTX,
      uint16_t* pBytesTXed,
      const uint16_t bytesToRX,
      const uint16_t bufferRXSize,
      uint8_t* const pBufferRX,
      uint16_t* pBytesRXed
);

/******************************************************************************/
/***                      Callback functions for SPI                        ***/
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
