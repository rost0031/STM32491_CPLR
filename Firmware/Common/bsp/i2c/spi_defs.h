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
 * \enum SPI_Bus_t
 * SPI Busses available on the system.
 */
typedef enum SPI_Busses {
   SPIBus5  = 0,                                        /**< SPI Bus 5 (SPI5) */
   MAX_SPI_BUS      /**< Maximum number of available SPI Busses on the system */
} SPI_Bus_t;

/**
 * \struct SPI_BusSettings_t
 * Most of the settings that are needed to set up all the hardware for each
 * SPI Bus.
 */
typedef struct SPI_BusSettings
{

   SPI_Bus_t               spi_sys_bus;     /**< System specifier for I2C Bus */

   /* SPI bus settings */
   SPI_TypeDef*      spi_bus;                           /**< STM32 SPI bus id */
   const uint32_t    spi_clk;                            /**< STM32 SPI clock */

   /* SPI interrupt settings */

   /* SPI GPIO settings for MISO */
   GPIO_TypeDef*       miso_port;                      /**< SPI MISO pin port */
   const uint16_t      miso_pin;                            /**< SPI MISO pin */
   const uint16_t      miso_af_pin_source;  /**< SPI MISO alt function source */
   const uint16_t      miso_af;                    /**< SPI MISO alt function */
   const uint32_t      miso_clk;                     /**< SPI MISO GPIO clock */

   /* SPI GPIO settings for MOSI */
   GPIO_TypeDef*       mosi_port;                      /**< SPI MOSI pin port */
   const uint16_t      mosi_pin;                            /**< SPI MOSI pin */
   const uint16_t      mosi_af_pin_source;  /**< SPI MOSI alt function source */
   const uint16_t      mosi_af;                    /**< SPI MOSI alt function */
   const uint32_t      mosi_clk;                     /**< SPI MOSI GPIO clock */

   /* SPI GPIO settings for SCK */
   GPIO_TypeDef*       sck_port;                        /**< SPI SCK pin port */
   const uint16_t      sck_pin;                              /**< SPI SCK pin */
   const uint16_t      sck_af_pin_source;    /**< SPI SCK alt function source */
   const uint16_t      sck_af;                      /**< SPI SCK alt function */
   const uint32_t      sck_clk;                       /**< SPI SCK GPIO clock */

   /* SPI GPIO settings for NSS */
   GPIO_TypeDef*       nss_port;                        /**< SPI NSS pin port */
   const uint16_t      nss_pin;                              /**< SPI NSS pin */
   const uint16_t      nss_af_pin_source;    /**< SPI NSS alt function source */
   const uint16_t      nss_af;                      /**< SPI NSS alt function */
   const uint32_t      nss_clk;                       /**< SPI NSS GPIO clock */

   /* Common SPI DMA settings */
   DMA_TypeDef*        spi_dma;                           /**< SPI DMA device */
   const uint32_t      spi_dma_channel;                  /**< SPI DMA channel */
   const uint32_t      spi_dma_dr_addr;               /**< SPI DMA DR address */
   const uint32_t      spi_dma_clk;                   /**< SPI DMA Clk source */

   /* TX SPI DMA settings */
   DMA_Stream_TypeDef* spi_dma_tx_stream;          /**< SPI DMA stream for TX */
   const IRQn_Type     spi_dma_tx_irq_num;         /**< SPI DMA TX IRQ number */
   const ISR_Priority  spi_dma_tx_irq_prio;      /**< SPI DMA TX IRQ priority */
   const uint32_t      spi_dma_tx_flags;                /**< SPI DMA TX Flags */

   /* RX SPI DMA settings */
   DMA_Stream_TypeDef* spi_dma_rx_stream;          /**< SPI DMA stream for RX */
   const IRQn_Type     spi_dma_rx_irq_num;         /**< SPI DMA RX IRQ number */
   const ISR_Priority  spi_dma_rx_irq_prio;      /**< SPI DMA RX IRQ priority */
   const uint32_t      spi_dma_rx_flags;                /**< SPI DMA RX Flags */

   /* Buffer management */
   uint8_t*            pRxBuffer;                     /**< SPI RX data buffer.*/
   uint16_t            nRxIndex;          /**< SPI data RX buffer used length.*/
   uint8_t*            pTxBuffer;                     /**< SPI TX data buffer.*/
   uint16_t            nTxIndex;          /**< SPI TX data buffer used length.*/

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
