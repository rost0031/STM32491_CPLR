/**
 * @file    sdram.c
 * @brief   Basic driver for MT48LC2M3B2B5-7E SDRAM memory on the FMC bus on the
 * STM324x9I-EVAL board.
 *
 * @date    09/24/2014
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupSDRAM
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "sdram.h"
#include "qp_port.h"                                        /* for QP support */
#include "stm32f4xx_it.h"
#include "project_includes.h"
#include "Shared.h"
#include "stm32f4xx_fmc.h"                         /* For STM32F4 FMC support */
#include "stm32f4xx_rcc.h"                         /* For STM32F4 RCC support */
#include "stm32f4xx_gpio.h"                       /* For STM32F4 GPIO support */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_SDRAM ); /* For dbg system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/**
 * @brief   A large buffer in SDRAM for testing.
 */
__attribute__((section(".sdram"))) uint32_t sdRamTestBuffer[10000];

/* Private function prototypes -----------------------------------------------*/
static void SDRAM_slowPrintMemRegion( uint32_t startAddr, uint32_t nBytes );
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
void SDRAM_Init( void )
{
   /* GPIO configuration for FMC SDRAM bank */
   SDRAM_GPIOInit();

   /* Enable FMC clock */
   RCC_AHB3PeriphClockCmd( RCC_AHB3Periph_FMC, ENABLE );

   /* FMC Configuration ------------------------------------------------------*/
   FMC_SDRAMInitTypeDef        FMC_SDRAMInitStructure;
   FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure;
   /* FMC SDRAM Bank configuration */
   /* Timing configuration for 90 Mhz of SD clock frequency (180Mhz/2) */
   /* TMRD: 2 Clock cycles */
   FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay    = 2;
   /* TXSR: min=70ns (7x11.11ns) */
   FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;
   /* TRAS: min=42ns (4x11.11ns) max=120k (ns) */
   FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime      = 4;
   /* TRC:  min=70 (7x11.11ns) */
   FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay        = 7;
   /* TWR:  min=1+ 7ns (1+1x11.11ns) */
   FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime    = 2;
   /* TRP:  20ns => 2x11.11ns */
   FMC_SDRAMTimingInitStructure.FMC_RPDelay              = 2;
   /* TRCD: 20ns => 2x11.11ns */
   FMC_SDRAMTimingInitStructure.FMC_RCDDelay             = 2;

   /* FMC SDRAM control configuration */
   FMC_SDRAMInitStructure.FMC_Bank               = FMC_Bank1_SDRAM;
   /* Row addressing: [7:0] */
   FMC_SDRAMInitStructure.FMC_ColumnBitsNumber   = FMC_ColumnBits_Number_8b;
   /* Column addressing: [10:0] */
   FMC_SDRAMInitStructure.FMC_RowBitsNumber      = FMC_RowBits_Number_11b;
   FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth  = FMC_SDMemory_Width_32b;
   FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
   /* CL: Cas Latency = 3 clock cycles */
   FMC_SDRAMInitStructure.FMC_CASLatency         = FMC_CAS_Latency_3;
   FMC_SDRAMInitStructure.FMC_WriteProtection    = FMC_Write_Protection_Disable;
   FMC_SDRAMInitStructure.FMC_SDClockPeriod      = FMC_SDClock_Period_2;
   FMC_SDRAMInitStructure.FMC_ReadBurst          = FMC_Read_Burst_Enable;
   FMC_SDRAMInitStructure.FMC_ReadPipeDelay      = FMC_ReadPipe_Delay_1;
   FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct  = &FMC_SDRAMTimingInitStructure;

   /* FMC SDRAM bank initialization */
   FMC_SDRAMInit(&FMC_SDRAMInitStructure);

   /* FMC SDRAM device initialization sequence */
   SDRAM_InitSequence();
}

/******************************************************************************/
void SDRAM_GPIOInit(void)
{
   /* Enable GPIOs clock */
   RCC_AHB1PeriphClockCmd(
         RCC_AHB1Periph_GPIOC |
         RCC_AHB1Periph_GPIOD |
         RCC_AHB1Periph_GPIOE |
         RCC_AHB1Periph_GPIOF |
         RCC_AHB1Periph_GPIOG |
         RCC_AHB1Periph_GPIOH |
         RCC_AHB1Periph_GPIOI,
         ENABLE
   );

   /*-- GPIOs Configuration --------------------------------------------------*/
   /*
 +-------------------+------------------+------------------+-------------------+
 +                       SDRAM pins assignment                                 +
 +-------------------+------------------+------------------+-------------------+
 | PD0  <-> FMC_D2   | PE0  <-> FMC_NBL0| PF0  <-> FMC_A0  | PG0 <-> FMC_A10   |
 | PD1  <-> FMC_D3   | PE1  <-> FMC_NBL1| PF1  <-> FMC_A1  | PG1 <-> FMC_A11   |
 | PD8  <-> FMC_D13  | PE7  <-> FMC_D4  | PF2  <-> FMC_A2  | PG4 <-> FMC_A14   |
 | PD9  <-> FMC_D14  | PE8  <-> FMC_D5  | PF3  <-> FMC_A3  | PG5 <-> FMC_A15   |
 | PD10 <-> FMC_D15  | PE9  <-> FMC_D6  | PF4  <-> FMC_A4  | PG8 <-> FC_SDCLK  |
 | PD14 <-> FMC_D0   | PE10 <-> FMC_D7  | PF5  <-> FMC_A5  | PG15 <-> FMC_NCAS |
 | PD15 <-> FMC_D1   | PE11 <-> FMC_D8  | PF11 <-> FC_NRAS |-------------------+
 +-------------------| PE12 <-> FMC_D9  | PF12 <-> FMC_A6  |
                     | PE13 <-> FMC_D10 | PF13 <-> FMC_A7  |
                     | PE14 <-> FMC_D11 | PF14 <-> FMC_A8  |
                     | PE15 <-> FMC_D12 | PF15 <-> FMC_A9  |
 +-------------------+------------------+------------------+
 | PH2 <-> FMC_SDCKE0| PI4 <-> FMC_NBL2 |
 | PH3 <-> FMC_SDNE0 | PI5 <-> FMC_NBL3 |
 | PH5 <-> FMC_SDNW  |------------------+
 +-------------------+
 +-------------------+------------------+
 +   32-bits Mode: D31-D16              +
 +-------------------+------------------+
 | PH8 <-> FMC_D16   | PI0 <-> FMC_D24  |
 | PH9 <-> FMC_D17   | PI1 <-> FMC_D25  |
 | PH10 <-> FMC_D18  | PI2 <-> FMC_D26  |
 | PH11 <-> FMC_D19  | PI3 <-> FMC_D27  |
 | PH12 <-> FMC_D20  | PI6 <-> FMC_D28  |
 | PH13 <-> FMC_D21  | PI7 <-> FMC_D29  |
 | PH14 <-> FMC_D22  | PI9 <-> FMC_D30  |
 | PH15 <-> FMC_D23  | PI10 <-> FMC_D31 |
 +------------------+-------------------+

 +-------------------+
 +  Pins remapping   +
 +-------------------+
 | PC0 <-> FMC_SDNWE |
 | PC2 <-> FMC_SDNE0 |
 | PC3 <-> FMC_SDCKE0|
 +-------------------+
    */

   /* Common GPIO configuration */
   GPIO_InitTypeDef GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

   /* GPIOD configuration */
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0  |GPIO_Pin_1  |GPIO_Pin_8 | GPIO_Pin_9 |
         GPIO_Pin_10 |GPIO_Pin_14 |GPIO_Pin_15;

   GPIO_Init(GPIOD, &GPIO_InitStructure);

   /* GPIOE configuration */
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_7  | GPIO_Pin_8  |
         GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
         GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

   GPIO_Init(GPIOE, &GPIO_InitStructure);

   /* GPIOF configuration */
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource11 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  |
         GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_11 | GPIO_Pin_12 |
         GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

   GPIO_Init(GPIOF, &GPIO_InitStructure);

   /* GPIOG configuration */
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource8 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource15 , GPIO_AF_FMC);


   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 |
         GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_15;

   GPIO_Init(GPIOG, &GPIO_InitStructure);

   /* GPIOH configuration */
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource2 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource3 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource5 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource8 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource9 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource10 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource11 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource12 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource13 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource14 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOH, GPIO_PinSource15 , GPIO_AF_FMC);


   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_5  | GPIO_Pin_8  |
         GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
         GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

   GPIO_Init(GPIOH, &GPIO_InitStructure);

   /* GPIOI configuration */
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource0 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource1 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource2 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource3 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource4 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource5 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource6 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource7 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource9 , GPIO_AF_FMC);
   GPIO_PinAFConfig(GPIOI, GPIO_PinSource10 , GPIO_AF_FMC);

   GPIO_InitStructure.GPIO_Pin =
         GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
         GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
         GPIO_Pin_9 | GPIO_Pin_10;

   GPIO_Init(GPIOI, &GPIO_InitStructure);
}

/******************************************************************************/
void SDRAM_InitSequence( void )
{
   FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
   uint32_t tmpr = 0;
   uint32_t timeout = SDRAM_TIMEOUT;

   /* Step 3 -----------------------------------------------------------------*/
   /* Configure a clock configuration enable command */
   FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
   FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
   FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
   FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
   /* Wait until the SDRAM controller is ready */
   while((FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
   {
      timeout--;
   }
   /* Send the command */
   FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

   /* Step 4 -----------------------------------------------------------------*/
   /* Insert 100 ms delay */
   BSP_Delay(10);

   /* Step 5 -----------------------------------------------------------------*/
   /* Configure a PALL (precharge all) command */
   FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
   FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
   FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
   FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

   /* Wait until the SDRAM controller is ready */
   timeout = SDRAM_TIMEOUT;
   while((FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
   {
      timeout--;
   }
   /* Send the command */
   FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

   /* Step 6 -----------------------------------------------------------------*/
   /* Configure a Auto-Refresh command */
   FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
   FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
   FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 8;
   FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;

   /* Wait until the SDRAM controller is ready */
   timeout = SDRAM_TIMEOUT;
   while((FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
   {
      timeout--;
   }
   /* Send the command */
   FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

   /* Step 7 -----------------------------------------------------------------*/
   /* Program the external memory mode register */
   tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
         SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
         SDRAM_MODEREG_CAS_LATENCY_3           |
         SDRAM_MODEREG_OPERATING_MODE_STANDARD |
         SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

   /* Configure a load Mode register command*/
   FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
   FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
   FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
   FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;

   /* Wait until the SDRAM controller is ready */
   timeout = SDRAM_TIMEOUT;
   while((FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
   {
      timeout--;
   }
   /* Send the command */
   FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

   /* Step 8 -----------------------------------------------------------------*/

   /* Set the refresh rate counter */
   /* (15.62 us x Freq) - 20 */
   /* Set the device refresh counter */
   FMC_SetRefreshCount( 1385 );

   /* Wait until the SDRAM controller is ready */
   timeout = SDRAM_TIMEOUT;
   while((FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET) && (timeout > 0))
   {
      timeout--;
   }
}

/******************************************************************************/
void SDRAM_WriteBuffer(
      uint32_t *pBuffer,
      uint32_t uwWriteAddress,
      uint32_t uwBufferSize
)
{
   __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;

   /* Disable write protection */
   FMC_SDRAMWriteProtectionConfig(FMC_Bank1_SDRAM, DISABLE);

   /* Wait until the SDRAM controller is ready */
   while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
   {
   }

   /* While there is data to write */
   for (; uwBufferSize != 0; uwBufferSize--) {
      /* Transfer data to the memory */
      *(uint32_t *) (SDRAM_BANK_ADDR + write_pointer) = *pBuffer++;

      /* Increment the address*/
      write_pointer += 4;
   }
}

/******************************************************************************/
void SDRAM_ReadBuffer(
      uint32_t* pBuffer,
      uint32_t uwReadAddress,
      uint32_t uwBufferSize
)
{
   __IO uint32_t write_pointer = (uint32_t)uwReadAddress;


   /* Wait until the SDRAM controller is ready */
   while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
   {
   }

   /* Read data */
   for(; uwBufferSize != 0x00; uwBufferSize--) {
      *pBuffer++ = *(__IO uint32_t *)(SDRAM_BANK_ADDR + write_pointer );

      /* Increment the address*/
      write_pointer += 4;
   }
}

/******************************************************************************/
void SDRAM_TestDestructive( void )
{
#define BUFFER_SIZE        ((uint32_t)0x0100)
#define WRITE_READ_ADDR    ((uint32_t)0x0800)

   uint32_t aTxBuffer[BUFFER_SIZE];
   uint32_t aRxBuffer[BUFFER_SIZE];

   uint32_t tmpIndex = 0;
   uint32_t uwOffset = 0x250F;

   /* 1. Fill the buffer to write */
   DBG_printf("Filling buffer...\n");
   for (tmpIndex = 0; tmpIndex < BUFFER_SIZE; tmpIndex++ ) {
      aTxBuffer[tmpIndex] = tmpIndex + uwOffset;
   }

   /* 2. Write buffer to SDRAM */
   DBG_printf("Writing buffer...\n");
   SDRAM_WriteBuffer(aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
   DBG_printf("Write finished\n");

   /* 3. Read data from the SDRAM */
   DBG_printf("Reading buffer...\n");
   SDRAM_ReadBuffer(aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
   DBG_printf("Read finished\n");

   /* 4. Check buffers against each other */
   DBG_printf("Checking buffers...\n");
   uint32_t uwIndex = 0;
   __IO uint32_t uwWriteReadStatus = 0;
   for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++) {
      if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex]) {
         ERR_printf("Written and read back values don't match: wrote 0x%04x and read back 0x%04x\n", aTxBuffer[uwIndex], aRxBuffer[uwIndex] );
         uwWriteReadStatus = uwIndex + 1;
      }
   }
   DBG_printf("Result of buffer check: %d\n", uwWriteReadStatus );

   /* Test a buffer placed directly into SDRAM by the linker */
   for (uint16_t uwIndexLarge = 0; uwIndexLarge < 10000 ; uwIndexLarge++) {
      sdRamTestBuffer[uwIndexLarge] = uwIndexLarge;
   }

  uint32_t uwRamBufferAddr = (uint32_t)sdRamTestBuffer; /* should be 0xC00xxxxx */

  /* Get main stack pointer value */
  uint32_t MSPValue = __get_MSP(); /* should be 0xC00xxxxx */
  uint16_t tmpRamIndex = 5553;
  DBG_printf("Copied data to large SDRAM buffer.  Value at %u is %u\n",tmpRamIndex , sdRamTestBuffer[tmpRamIndex]);
  DBG_printf("Stack pointer addr: 0x%08x, SDRAM buffer addr: 0x%08x\n", MSPValue, uwRamBufferAddr);

}

/******************************************************************************/
uint32_t SDRAM_testDataBus( const uint32_t addr )
{
   LOG_printf("Testing RAM data bus...\n");
   for( uint32_t p = 1; p != 0; p <<= 1 ) {
      *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr) = p; // Write pattern to the address

      if (  *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr ) != p ) {
         ERR_printf(
               "RAM data bus test failed. Expected: 0x%08x, read: 0x%08x\n",
               p,
               *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr )
         );
         return p;
      }
   }

   LOG_printf("RAM data bus test finished with no errors.\n");
   return 0;
}

/******************************************************************************/
uint32_t SDRAM_testAddrBus( const uint32_t addr, const uint32_t nBytes )
{
   LOG_printf("Testing RAM address bus...\n");
   uint32_t addrMask = nBytes/sizeof(uint8_t) - 1;

   uint8_t testPattern = (uint8_t) 0xAA;
   uint8_t antiPattern = (uint8_t) 0x55;

   uint32_t offset = 0;
   /* Write the default pattern to each of the power of two offsets */
   for( offset = 1; (offset & addrMask ) != 0; offset <<= 1 ) {
      *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + offset) = testPattern;
   }

   /* Check for address bits stuck high */
   uint32_t testOffset = 0;
   *(uint8_t *)(SDRAM_BANK_ADDR + addr + testOffset) = antiPattern;

   LOG_printf("Testing for address lines stuck high...\n");

   for( offset = 1; (offset & addrMask) != 0; offset <<= 1 ) {
      if ( *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + offset ) != testPattern ) {
         ERR_printf(
               "RAM address bus stuck-high test failed. Expected: 0x%08x, read: 0x%08x\n",
               testPattern,
               *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + offset )
         );
         SDRAM_slowPrintMemRegion(SDRAM_BANK_ADDR + addr, 128);

         return( SDRAM_BANK_ADDR + addr + offset );
      }
   }

   LOG_printf("Stuck-high test finished with no errors.\n");
   LOG_printf("Testing for address lines stuck low or shorted...\n");

   *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + testOffset) = testPattern;

   /* Check for address bits stuck low or shorted */
   for( testOffset = 1; (testOffset & addrMask) != 0; testOffset <<= 1 ) {
      *(uint8_t *)(SDRAM_BANK_ADDR + addr + testOffset) = antiPattern;

      if ( *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + 0) != testPattern ) {
         ERR_printf(
               "RAM address bus stuck-low test failed. Expected: 0x%02x, read: 0x%02x\n",
               testPattern,
               *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + 0 )
         );
         SDRAM_slowPrintMemRegion(SDRAM_BANK_ADDR + addr, 128);
         return( SDRAM_BANK_ADDR + addr + 0 );
      }

      for( offset = 1; (offset & addrMask) != 0; offset <<= 1 ) {
         if ( offset != testOffset &&
               *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + offset) != testPattern ) {
            ERR_printf(
                  "RAM address bus stuck-low test failed. Expected: 0x%02x, read: 0x%02x\n",
                  testPattern,
                  *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + offset )
            );
            SDRAM_slowPrintMemRegion(SDRAM_BANK_ADDR + addr + offset, 128);
            return( SDRAM_BANK_ADDR + addr + testOffset );
         }
      }

      *(__IO uint8_t *)(SDRAM_BANK_ADDR + addr + testOffset) = testPattern;
   }
   LOG_printf("Stuck-low test finished with no errors.\n");
   LOG_printf("RAM address bus test finished with no errors.\n");
   return 0;
}

/******************************************************************************/
uint32_t SDRAM_testDevice( const uint32_t addr, const uint32_t nBytes )
{
   LOG_printf("Testing RAM device...\n");

   uint32_t offset = 0;
   uint32_t nWords = nBytes / (sizeof(uint32_t));

   uint32_t testPattern = 0;
   uint32_t antiPattern = 0;

   /* Fill memory with known pattern */
   for( testPattern = 1, offset = 0; offset < nWords; testPattern++, offset += 4 ) {
      *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr + offset) = testPattern; // Write pattern to the address
   }

   /* Check each location and invert it for a second pass */
   for( testPattern = 1, offset = 0; offset < nWords; testPattern++,  offset += 4 ) {
      if( *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr + offset) != testPattern ) {
         ERR_printf(
               "RAM device test failed at addr: 0x%08x. Expected: 0x%08x, read: 0x%08x\n",
               SDRAM_BANK_ADDR + addr + offset,
               testPattern,
               *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr + offset )
         );
         return( SDRAM_BANK_ADDR + addr + offset );
      }

      antiPattern = ~testPattern;
      *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr + offset ) = antiPattern;
   }

   /* Check each location for the inverted pattern and zero it */
   for( testPattern = 1, offset = 0; offset < nWords; testPattern++, offset += 4 ) {
      antiPattern = ~testPattern;
      if( *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr + offset) != antiPattern ) {
         ERR_printf(
               "RAM device test failed at addr: 0x%08x. Expected: 0x%08x, read: 0x%08x\n",
               SDRAM_BANK_ADDR + addr + offset,
               testPattern,
               *(__IO uint32_t *)(SDRAM_BANK_ADDR + addr + offset )
         );
         return( SDRAM_BANK_ADDR + addr + offset );
      }
   }

   LOG_printf("RAM device test finished with no errors.\n");
   return 0;
}


/******************************************************************************/
static void SDRAM_slowPrintMemRegion( uint32_t startAddr, uint32_t nBytes )
{
   dbg_slow_printf("Memory Region %d bytes from addr 0x%08x\n", (unsigned int)nBytes, (unsigned int)startAddr);
   for ( uint32_t i = 0; i <= nBytes; i += 4 ) {
      if ( i%16 == 0 ) {
         printf("\nAddr: 0x%08x : ", (unsigned int)(startAddr + i) );
      }
      printf("%08x ", (unsigned int)*(__IO uint32_t *)(startAddr + i ));
   }
}

/******************************************************************************/
inline void SDRAM_CallbackExample( void )
{
}

/**
 * @}
 * end addtogroup groupSDRAM
 */

/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
