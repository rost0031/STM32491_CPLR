/**
 * @file 	spi.c
 * @brief   Basic driver for SPI bus.
 *
 * @date   	08/24/2015
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 *
 * @addtogroup groupSPI
 * @{
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include "spi_defs.h"
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"                           /* For STM32 DMA support */
#include "stm32f4xx_spi.h"                           /* For STM32 SPI support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_SPI ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

  #define SPIx                           SPI2
  #define SPIx_CLK                       RCC_APB1Periph_SPI2
  #define SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
  #define SPIx_IRQn                      SPI2_IRQn
  #define SPIx_IRQHANDLER                SPI2_IRQHandler

  #define SPIx_SCK_PIN                   GPIO_Pin_1
  #define SPIx_SCK_GPIO_PORT             GPIOI
  #define SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOI
  #define SPIx_SCK_SOURCE                GPIO_PinSource1
  #define SPIx_SCK_AF                    GPIO_AF_SPI2

  #define SPIx_MISO_PIN                  GPIO_Pin_2
  #define SPIx_MISO_GPIO_PORT            GPIOI
  #define SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOI
  #define SPIx_MISO_SOURCE               GPIO_PinSource2
  #define SPIx_MISO_AF                   GPIO_AF_SPI2

  #define SPIx_MOSI_PIN                  GPIO_Pin_3
  #define SPIx_MOSI_GPIO_PORT            GPIOI
  #define SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOI
  #define SPIx_MOSI_SOURCE               GPIO_PinSource3
  #define SPIx_MOSI_AF                   GPIO_AF_SPI2

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief RX Buffer for I2C1 bus
 */
uint8_t          spi1RxBuffer[MAX_SPI_LEN];

/**
 * @brief TX Buffer for I2C1 bus
 */
uint8_t          spi1TxBuffer[MAX_SPI_LEN];

/**
 * @brief An internal structure that holds almost all the settings for the I2C
 * Busses
 */
SPI_BusSettings_t s_SPI_Bus[MAX_SPI_BUS] =
{
      {
            SPIBus3,                   /**< i2c_sys_bus */

            /* I2C bus settings */
            SPI3,                      /**< i2c_bus */


            /* SPI interrupt settings */

            /* SPI GPIO settings for MISO */
            GPIOC,                     /**< miso_port */
            GPIO_Pin_11,               /**< miso_pin */
            GPIO_PinSource11,          /**< miso_af_pin_source */
            GPIO_AF_SPI3,              /**< miso_af */
            RCC_AHB1Periph_GPIOC,      /**< miso_clk */

            /* SPI GPIO settings for MOSI */
            GPIOC,                     /**< mosi_port */
            GPIO_Pin_12,               /**< mosi_pin */
            GPIO_PinSource12,          /**< mosi_af_pin_source */
            GPIO_AF_SPI3,              /**< mosi_af */
            RCC_AHB1Periph_GPIOC,      /**< mosi_clk */

            /* SPI GPIO settings for SCK */
            GPIOC,                     /**< sck_port */
            GPIO_Pin_10,               /**< sck_pin */
            GPIO_PinSource10,          /**< sck_af_pin_source */
            GPIO_AF_SPI3,              /**< sck_af */
            RCC_AHB1Periph_GPIOC,      /**< sck_clk */

            /* SPI GPIO settings for NSS */
            GPIOB,                     /**< nss_port */
            GPIO_Pin_9,                /**< nss_pin */
            GPIO_PinSource9,           /**< nss_af_pin_source */
            GPIO_AF_I2C1,              /**< nss_af */
            RCC_AHB1Periph_GPIOB,      /**< nss_clk */

            /* Common SPI DMA settings */
            DMA1,                      /**< i2c_dma */
            DMA_Channel_1,             /**< i2c_dma_channel */
            (uint32_t)&(I2C1->DR),     /**< i2c_dma_dr_addr */
            RCC_AHB1Periph_DMA1,       /**< i2c_dma_clk */

            /* TX SPI DMA settings */
            DMA1_Stream6,              /**< i2c_dma_tx_stream */
            DMA1_Stream6_IRQn,         /**< i2c_dma_tx_irq_num */
            DMA1_Stream6_PRIO,         /**< i2c_dma_tx_irq_prio */
            DMA_FLAG_TCIF6 |
            DMA_FLAG_FEIF6 |
            DMA_FLAG_DMEIF6 |
            DMA_FLAG_TEIF6 |
            DMA_FLAG_HTIF6,            /**< i2c_dma_tx_flags */

            /* RX SPI DMA settings */
            DMA1_Stream0,              /**< i2c_dma_rx_stream */
            DMA1_Stream0_IRQn,         /**< i2c_dma_rx_irq_num */
            DMA1_Stream0_PRIO,         /**< i2c_dma_rx_irq_prio */
            DMA_FLAG_TCIF0 |
            DMA_FLAG_FEIF0 |
            DMA_FLAG_DMEIF0 |
            DMA_FLAG_TEIF0 |
            DMA_FLAG_HTIF0,            /**< i2c_dma_rx_flags */

            /* Buffer management */
            &spi1RxBuffer[0],          /**< *pRxBuffer */
            0,                         /**< nRxindex */
            &spi1TxBuffer[0],          /**< *pTxBuffer */
            0,                         /**< nTxindex */

      }
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void SPI_BusInit( SPI_Bus_t iBus )
{
   /* Deinit the bus just in case. */
   SPI_BusDeInit( iBus );



}

/******************************************************************************/
void SPI_BusDeInit( SPI_Bus_t iBus )
{

}


/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
