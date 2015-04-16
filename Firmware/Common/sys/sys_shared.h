/**
 * @file 	sys_shared.h
 * @brief   Contains all the shared SYS types, defines, etc.
 *
 * This file contains all the shared (between bootloader and application) system
 * settings, macros, types, etc. This is to allow common code usage for SYS
 * portion of both bootloader and application FW.
 *
 * @date   	04/15/2015
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SYS_SHARED_H_
#define SYS_SHARED_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @brief STM32 optimized MEMCPY.
 * This STM32 optimized MEMCPY is much faster than the regular one provided by
 * standard libs.  It's specifically tuned for arm cortex M3 processors and
 * written in very fast assembly.  Use it instead of regular memcpy */
#define MEMCPY(dst,src,len)            MEM_DataCopy(dst,src,len)
#define SMEMCPY(dst,src,len)           MEM_DataCopy(dst,src,len)

/**
 * @brief MAX macro with type safety
 * @param [in] a: first value to check
 * @param [in] b: second value to check
 * @return: value which is bigger.
 */
#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

/**
 * @brief MIN macro with type safety
 * @param [in] a: first value to check
 * @param [in] b: second value to check
 * @return: value which is smaller.
 */
#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


#ifndef CB_UNUSED_ARG
#define CB_UNUSED_ARG(x) (void)x
#endif
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#endif                                                       /* BSP_SHARED_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
