/**
 * @file 	bsp_shared.h
 * @brief   Contains all the shared BSP types, defines, etc.
 *
 * This file contains all the shared (between bootloader and application) BSP
 * and device settings This is necessary to make sure there is no overlap in
 * enumerations between	the shared code and the code pulling the shared code in.
 *
 * @date   	04/15/2015
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BSP_SHARED_H_
#define BSP_SHARED_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @brief   Device Access type
 * This type allows caller to specify whether a particular device will be
 * accessed via DMA or by old byte-at-a-time blocking methods.
 * @note: not all the devices support this.
 */
typedef enum Access {
   DEV_ACC_BLK  = 0,                    /**< Access device one byte at a time */
   DEV_ACC_DMA,                                    /**< Access device via DMA */
   /* Insert more I2C access types here... */
} DevAccess_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#endif                                                       /* BSP_SHARED_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
