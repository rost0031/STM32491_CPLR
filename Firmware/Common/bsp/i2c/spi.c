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
            SPIBus5,                   /**< spi_sys_bus */

            /* I2C bus settings */
            SPI5,                      /**< spi_bus */
            RCC_APB2Periph_SPI5,       /**< spi_clk */

            /* SPI interrupt settings */

            /* SPI GPIO settings for MISO */
            GPIOF,                     /**< miso_port */
            GPIO_Pin_8,                /**< miso_pin */
            GPIO_PinSource8,           /**< miso_af_pin_source */
            GPIO_AF_SPI5,              /**< miso_af */
            RCC_AHB1Periph_GPIOF,      /**< miso_clk */

            /* SPI GPIO settings for MOSI */
            GPIOF,                     /**< mosi_port */
            GPIO_Pin_9,                /**< mosi_pin */
            GPIO_PinSource9,           /**< mosi_af_pin_source */
            GPIO_AF_SPI5,              /**< mosi_af */
            RCC_AHB1Periph_GPIOF,      /**< mosi_clk */

            /* SPI GPIO settings for SCK */
            GPIOF,                     /**< sck_port */
            GPIO_Pin_7,                /**< sck_pin */
            GPIO_PinSource7,           /**< sck_af_pin_source */
            GPIO_AF_SPI5,              /**< sck_af */
            RCC_AHB1Periph_GPIOF,      /**< sck_clk */

            /* SPI GPIO settings for NSS */
            GPIOF,                     /**< nss_port */
            GPIO_Pin_6,                /**< nss_pin */
            GPIO_PinSource6,           /**< nss_af_pin_source */
            GPIO_AF_SPI5,              /**< nss_af */
            RCC_AHB1Periph_GPIOF,      /**< nss_clk */

            /* Common SPI DMA settings */
            DMA2,                      /**< spi_dma */
            DMA_Channel_2,             /**< spi_dma_channel */
            (uint32_t)&(SPI5->DR),     /**< spi_dma_dr_addr */
            RCC_AHB1Periph_DMA2,       /**< spi_dma_clk */

            /* TX SPI DMA settings */
            DMA2_Stream4,              /**< spi_dma_tx_stream */
            DMA2_Stream4_IRQn,         /**< spi_dma_tx_irq_num */
            DMA2_Stream4_PRIO,         /**< spi_dma_tx_irq_prio */
            DMA_FLAG_TCIF4 |
            DMA_FLAG_FEIF4 |
            DMA_FLAG_DMEIF4 |
            DMA_FLAG_TEIF4 |
            DMA_FLAG_HTIF4,            /**< spi_dma_tx_flags */

            /* RX SPI DMA settings */
            DMA2_Stream3,              /**< spi_dma_rx_stream */
            DMA2_Stream3_IRQn,         /**< spi_dma_rx_irq_num */
            DMA2_Stream3_PRIO,         /**< spi_dma_rx_irq_prio */
            DMA_FLAG_TCIF3 |
            DMA_FLAG_FEIF3 |
            DMA_FLAG_DMEIF3 |
            DMA_FLAG_TEIF3 |
            DMA_FLAG_HTIF3,            /**< spi_dma_rx_flags */

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
   //   SPI_BusDeInit( iBus );

   /* Enable the SPI bus clock */
   RCC_APB2PeriphClockCmd(s_SPI_Bus[iBus].spi_clk, ENABLE);

   /* Enable GPIO pin clocks */
   RCC_AHB1PeriphClockCmd(
         s_SPI_Bus[iBus].sck_clk | s_SPI_Bus[iBus].miso_clk | s_SPI_Bus[iBus].mosi_clk,
         ENABLE
   );

   /* Enable DMA clock */
   //   RCC_AHB1PeriphClockCmd(SPIx_DMA_CLK, ENABLE);

   /* SPI GPIO Configuration --------------------------------------------------*/
   /* GPIO Deinitialisation */
   GPIO_DeInit(s_SPI_Bus[iBus].sck_port);
   GPIO_DeInit(s_SPI_Bus[iBus].miso_port);
   GPIO_DeInit(s_SPI_Bus[iBus].mosi_port);

   /* Connect SPI pins to AF5 */
   GPIO_PinAFConfig( s_SPI_Bus[iBus].sck_port, s_SPI_Bus[iBus].sck_af_pin_source, s_SPI_Bus[iBus].sck_af );
   GPIO_PinAFConfig( s_SPI_Bus[iBus].miso_port, s_SPI_Bus[iBus].miso_af_pin_source, s_SPI_Bus[iBus].miso_af );
   GPIO_PinAFConfig( s_SPI_Bus[iBus].mosi_port, s_SPI_Bus[iBus].mosi_af_pin_source, s_SPI_Bus[iBus].mosi_af );

   GPIO_InitTypeDef GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

   /* SPI SCK pin configuration */
   GPIO_InitStructure.GPIO_Pin = s_SPI_Bus[iBus].sck_pin;
   GPIO_Init(s_SPI_Bus[iBus].sck_port, &GPIO_InitStructure);

   /* SPI  MISO pin configuration */
   GPIO_InitStructure.GPIO_Pin = s_SPI_Bus[iBus].miso_pin;
   GPIO_Init(s_SPI_Bus[iBus].miso_port, &GPIO_InitStructure);

   /* SPI  MOSI pin configuration */
   GPIO_InitStructure.GPIO_Pin = s_SPI_Bus[iBus].mosi_pin;
   GPIO_Init(s_SPI_Bus[iBus].mosi_port, &GPIO_InitStructure);

   /* SPI configuration -------------------------------------------------------*/
   SPI_I2S_DeInit(s_SPI_Bus[iBus].spi_bus);

   SPI_InitTypeDef  SPI_InitStructure;
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 0;

   SPI_Init( s_SPI_Bus[iBus].spi_bus, &SPI_InitStructure );
   SPI_Cmd( s_SPI_Bus[iBus].spi_bus, ENABLE );

   /* DMA configuration -------------------------------------------------------*/
   //   DMA_InitTypeDef DMA_InitStructure;
   //   /* Deinitialize DMA Streams */
   //   DMA_DeInit(SPIx_TX_DMA_STREAM);
   //   DMA_DeInit(SPIx_RX_DMA_STREAM);
   //
   //   /* Configure DMA Initialization Structure */
   //   DMA_InitStructure.DMA_BufferSize = BUFFERSIZE ;
   //   DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
   //   DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
   //   DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
   //   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   //   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   //   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   //   DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(SPIx->DR)) ;
   //   DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
   //   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   //   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   //   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   //   /* Configure TX DMA */
   //   DMA_InitStructure.DMA_Channel = SPIx_TX_DMA_CHANNEL ;
   //   DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
   //   DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aTxBuffer ;
   //   DMA_Init(SPIx_TX_DMA_STREAM, &DMA_InitStructure);
   //   /* Configure RX DMA */
   //   DMA_InitStructure.DMA_Channel = SPIx_RX_DMA_CHANNEL ;
   //   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
   //   DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aRxBuffer ;
   //   DMA_Init(SPIx_RX_DMA_STREAM, &DMA_InitStructure);

}

/******************************************************************************/
void SPI_BusDeInit( SPI_Bus_t iBus )
{

}

/******************************************************************************/
void SPI_NSS_high( SPI_Bus_t iBus )
{
   GPIO_WriteBit( s_SPI_Bus[iBus].nss_port, s_SPI_Bus[iBus].nss_pin, Bit_SET );
}

/******************************************************************************/
void SPI_NSS_low( SPI_Bus_t iBus )
{
   GPIO_WriteBit( s_SPI_Bus[iBus].nss_port, s_SPI_Bus[iBus].nss_pin, Bit_RESET );
}

/******************************************************************************/
DC3Error_t SPI_send(
      const SPI_Bus_t iBus,
      const uint16_t bytesToTX,
      const uint16_t bufferTXSize,
      const uint8_t* const pBufferTX,
      uint16_t* pBytesTXed
)
{
   DC3Error_t status = ERR_NONE;             /* keep track of success/failure */

   uint16_t spi_retry = 0;
   *pBytesTXed = 0;

   SPI_NSS_low( iBus );    /* Assert the NSS pin low to become SPI bus master */

   for ( (*pBytesTXed) = 0; (*pBytesTXed) < bytesToTX; (*pBytesTXed)++ )
   {
      /* When SPI Tx buffer is empty, send data.  Make sure we don't get
       * stuck in an inf loop while waiting for HW */
      spi_retry = 0;
      while ( SET != SPI_I2S_GetFlagStatus( s_SPI_Bus[iBus].spi_bus, SPI_I2S_FLAG_TXE ) ) {
         if ( SPI_FLAG_MAX_RETRIES <= spi_retry++ ) {
            status = ERR_SPI_TXE_FLAG_TIMEOUT;
            goto SPI_send_ERR_HANDLE;
         }
      }
      SPI_I2S_SendData( s_SPI_Bus[iBus].spi_bus, pBufferTX[ *pBytesTXed ] );

      /* Get the dummy byte coming back when SPI Rx buffer is empty. Make sure
       * we don't get stuck in an inf loop while waiting for HW */
      spi_retry = 0;
      while ( SET != SPI_I2S_GetFlagStatus( s_SPI_Bus[iBus].spi_bus, SPI_I2S_FLAG_RXNE ) ) {
         if ( SPI_FLAG_MAX_RETRIES <= spi_retry++ )  {
            status = ERR_SPI_RXE_FLAG_TIMEOUT;
            goto SPI_send_ERR_HANDLE;
         }
      }
      SPI_I2S_ReceiveData( s_SPI_Bus[iBus].spi_bus ); /* Dummy byte so no need to store it */
   }

SPI_send_ERR_HANDLE:              /* Handle any error that may have occurred. */

   /* Always release the bus upon exit */
   SPI_NSS_high( iBus ); /* Assert the NSS pin high to release SPI bus master */

   ERR_COND_OUTPUT( status, _DC3_ACCESS_BARE,
      "Sending %d bytes on SPI bus %s from a buffer of size %d via %s: Error 0x%08x\n",
      bytesToTX, CON_spiBusToStr( iBus ), bufferTXSize,
      CON_accessToStr(_DC3_ACCESS_BARE), status );
   return( status );
}


/******************************************************************************/
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
)
{
   DC3Error_t status = ERR_NONE;             /* keep track of success/failure */

   uint16_t spi_retry = 0;


   /* Check buffer sizes and params */
   if ( bytesToTX != bytesToRX ) {
      status = ERR_SPI_RX_TX_LENGTH_MISMATCH;
      goto SPI_transceive_ERR_HANDLE;
   }

   if ( bufferTXSize < bytesToTX || bufferRXSize < bytesToRX ) {
      status = ERR_MEM_BUFFER_LEN;
      goto SPI_transceive_ERR_HANDLE;
   }

   if ( pBufferTX == NULL || pBufferRX == NULL ) {
      status = ERR_MEM_NULL_VALUE;
      goto SPI_transceive_ERR_HANDLE;
   }

   SPI_NSS_low( iBus );    /* Assert the NSS pin low to become SPI bus master */

   *pBytesTXed = 0;
   *pBytesRXed = 0;

   for ( (*pBytesTXed) = 0; (*pBytesTXed) < bytesToTX; (*pBytesTXed)++, (*pBytesRXed)++ )
   {
      /* TX a byte */
      spi_retry = 0;
      while ( SET != SPI_I2S_GetFlagStatus( s_SPI_Bus[iBus].spi_bus, SPI_I2S_FLAG_TXE ) ) {
         if ( SPI_FLAG_MAX_RETRIES <= spi_retry++ ) {
            status = ERR_SPI_TXE_FLAG_TIMEOUT;
            goto SPI_transceive_ERR_HANDLE;
         }
      }
      SPI_I2S_SendData( s_SPI_Bus[iBus].spi_bus, pBufferTX[ *pBytesTXed ] );

      /* RX a byte */
      spi_retry = 0;
      while ( SET != SPI_I2S_GetFlagStatus( s_SPI_Bus[iBus].spi_bus, SPI_I2S_FLAG_RXNE ) ) {
         if ( SPI_FLAG_MAX_RETRIES <= spi_retry++ )  {
            status = ERR_SPI_RXE_FLAG_TIMEOUT;
            goto SPI_transceive_ERR_HANDLE;
         }
      }
      pBufferRX[*pBytesRXed] = SPI_I2S_ReceiveData( s_SPI_Bus[iBus].spi_bus );
   }

SPI_transceive_ERR_HANDLE:        /* Handle any error that may have occurred. */

   /* Always release the bus upon exit */
   SPI_NSS_high( iBus ); /* Assert the NSS pin high to release SPI bus master */

   ERR_COND_OUTPUT( status, _DC3_ACCESS_BARE,
      "Transceiving (tx %d and rx %d) bytes on SPI bus %s from buffers of size %d (rx) and %d (tx) via %s: Error 0x%08x\n",
      bytesToTX, bytesToRX, CON_spiBusToStr( iBus ), bufferTXSize, bufferRXSize,
      CON_accessToStr(_DC3_ACCESS_BARE), status );
   return( status );
}

/**
 * @}
 * end addtogroup groupI2C
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
