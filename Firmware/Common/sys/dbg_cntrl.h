/**
 * @file   dbg_cntrl.h
 * @brief  Declarations for debug and output functions over DMA serial console.
 * @date   09/09/2014
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupDbgCntrl
 * @{
 * <b> Introduction </b>
 *
 * The 2 sets of macros defined here should be used for all output to the serial
 * debug console.  The 2 categories of printf-like macros are:
 *
 *    -# XXX_printf()
 *    -# xxx_slow_printf().
 *
 * <b> XXX_printf macro description </b>
 *
 * The XXX_printf() macros should only be used after the BSP has been properly
 * initialized and the RTOS (QPC) running.  These macros are non-blocking and
 * return almost instantly, causing very little speed decrease in the running of
 * the actual application.  Instead of waiting until the data is done printing
 * over the serial port, these macros copy the data to an event which gets
 * published and received by SerialMgr Active Object (AO).  SerialMgr AO then
 * either:
 *    - Sets up a DMA write out to the serial console and the data then proceeds
 *    to get output.
 *    - Queues up the event if it's currently busy doing a previous DMA serial
 *    write and takes care of it when it's finished.
 *
 * @note 1: Since the buffer space in SerialMgr is finite, if you have too many
 * consecutive calls to a XXX_printf() macro, you can overrun the queue.  This
 * will cause an Q_assert.
 *
 * @note 2: SerialMgr AO must be up and the BSP must have finished configuring
 * DMA serial interface.  Before this point, only the xxx_slow_debug() macros
 * should be used.
 *
 * @note 3: Some XXX_printf macros are automatically compiled out when doing a
 * release build.
 *
 * <b> XXX_printf macro usage and examples </b>
 *
 * Use the following macros for printf style debugging.  Some of them will be
 * automatically disabled via macros when a "rel" build is done (as opposed to
 * a dbg or a spy build).
 * Examples of how to use:
 * @code
 * int i = 0;
 *
 * DBG_printf("Debug print test %d\n",i);
 * LOG_printf("Logging print test %d\n",i);
 * WRN_printf("Warning print test %d\n",i);
 * ERR_printf("Error print test %d\n",i);
 * MENU_printf("Console print test %d\n",i);
 *
 * will output:
 * DBG-00:04:09:00459-function_name():219: Debug print test 0
 * LOG-00:04:09:00459-function_name():219: Logging print test 0
 * WRN-00:04:09:00459-function_name():219: Warning print test 0
 * ERR-00:04:09:00459-function_name():219: Error print test 0
 * Console print test 0
 * @endcode
 *
 * <b> xxx_slow_printf macro description </b>
 *
 * These macros print printing directly to the serial console and are fairly
 * slow.  Use the DBG/LOG/WRN/ERR_printf macros instead.  These macros should
 * only in the initializations before the QPC RTOS is running and should
 * not be used after.
 *
 * <b> xxx_slow_printf macro usage and examples </b>
 *
 * Use the following functions for printf style debugging only before the AOs of
 * the QPC have been started.  These output directly to the serial console
 * without using DMA and will adversely affect the performance of the system.
 * These will be automatically disabled via macros when a "rel" build is done
 * (as opposed to a dbg or a spy build). xxx_slow_printf will print a lot more info
 * than isr_debug_printf
 * Examples of how to use:
 * @code
 * int i = 0;
 *
 * dbg_slow_printf("Debug print test\n");
 * dbg_slow_printf("Debug print test %d\n",i);
 * log_slow_printf("Logging print test %d\n",i);
 * wrn_slow_printf("Warning print test %d\n",i);
 * err_slow_printf("Error print test %d\n",i);
 *
 *
 * will output:
 * DBG-SLOW!-00:04:09:00459-function_name():219: Debug print test
 * DBG-SLOW!-00:04:09:00459-function_name():219: Debug print test 0
 * LOG-SLOW!-00:04:09:00459-function_name():219: Logging print test 0
 * WRN-SLOW!-00:04:09:00459-function_name():219: Warning print test 0
 * ERR-SLOW!-00:04:09:00459-function_name():219: Error print test 0
 * @endcode
 *
 *
 * @code
 * isr_debug_slow_printf("ISR Debug print test\n");
 * isr_debug_slow_printf("ISR Debug print test %d\n", i);
 *
 * will output:
 * D ISR Debug print test
 * D ISR Debug print test 0
 *
 * @endcode
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DBG_CNTRL_H_
#define DBG_CNTRL_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "DC3CommApi.h"                          /* For the shared dbg levels */
#include "bsp_shared.h"

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief Flag passed in by the makefile to determine whether to compile in the
 * subset of output functions which depend on this flag.  This, in turn sets or
 * unsets the @def DEBUG flag.  This flag determines whether some of the
 * XXX_printf() macros will be compiled in or not.
 */
#ifndef NDEBUG
 #define DEBUG 1
 /* If debug build, enable some module debug by default and enable debug output
  * over all debug devices */
 #define DBG_MODULES_DEF     (                                                \
       DC3_DBG_MODL_GEN   | DC3_DBG_MODL_I2C_DEV | DC3_DBG_MODL_FLASH |       \
       DC3_DBG_MODL_SYS   | DC3_DBG_MODL_DB      | DC3_DBG_MODL_ETH   |       \
       DC3_DBG_MODL_NOR   | DC3_DBG_MODL_SDRAM   | DC3_DBG_MODL_COMM          \
  )

 #define DBG_DEVICES_DEF    ( _DC3_DBG_DEV_SER | _DC3_DBG_DEV_ETH )

#else
#define DEBUG 0
/* If release build, disable all module debug by default and only enable debug
 * output over ethernet since it only outputs if someone actually connects to it */
 #define DBG_MODULES_DEF    ( 0 )
 #define DBG_DEVICES_DEF    ( _DC3_DBG_DEV_ETH )
#endif

/**< For debugging weird problems.  Uncomment this to turn all debugging output
 * to the regular slow and blocking version.  This will also disable debugging
 * output to ethernet.
 */
//#define SLOW_PRINTF

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
CCMRAM_VAR extern uint32_t glbDbgModuleConfig;    /**< Allow global access to debug info */
CCMRAM_VAR extern uint8_t  glbDbgDeviceConfig;    /**< Allow global access to debug info */

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/**
 * @brief   Defines a module for grouping debugging functionality.
 *
 * @description
 * Macro to be placed at the top of each C/C++ module to define the single
 * instance of the module name string to be used in printing of debugging
 * information.
 *
 * @param[in] @c name_: DC3DbgModule_t enum representing the module.
 * @return  None
 *
 * @note 1: This macro should __not__ be terminated by a semicolon.
 * @note 2: This macro MUST be present in the file if DBG_printf() or
 * LOG_printf() functions are called.  The code will not compile without this.
 */
#define DBG_DEFINE_THIS_MODULE( name_ ) \
      static DC3DbgModule_t const Q_ROM DBG_this_module_ = name_;

/**
 * @brief   Enable debugging output for a given module.
 *
 * @param [in] @c name_: DC3DbgModule_t enum representing the module.
 * @return  None
 */
#define DBG_ENABLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgModuleConfig |= name_;

/**
 * @brief   Disable debugging output for a given module.
 *
 * @param [in] @c name_: DC3DbgModule_t enum representing the module.
 * @return  None
 */
#define DBG_DISABLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgModuleConfig &= ~name_;

/**
 * @brief   Toggle debugging output for a given module.
 *
 * @param [in] @c name_: DC3DbgModule_t enum representing the module.
 * @return  None
 */
#define DBG_TOGGLE_DEBUG_FOR_MODULE( name_ ) \
      glbDbgModuleConfig ^= name_;

/**
 * @brief   Toggle debugging output for a given module.
 *
 * @param [in] @c name_: DC3DbgModule_t enum representing the module.
 * @return  None
 */
#define DBG_CHECK_DEBUG_FOR_MODULE( name_ ) \
      ( glbDbgModuleConfig & name_ )

/**
 * @brief   Disable debugging output for all modules.
 * @param   None
 * @return  None
 */
#define DBG_DISABLE_DEBUG_FOR_ALL_MODULES( ) \
      glbDbgModuleConfig = 0x00000000;

/**
 * @brief   Set debugging output for all modules, overwriting current.
 * This is used to set the debugging to settings from the DB.
 * @param   [in] @c dbgSetting_: uint32_t bitfield representing ORring of all
 * enabled modules.
 * @return  None
 */
#define DBG_SET_DEBUG_FOR_ALL_MODULES( dbgSetting_ ) \
      glbDbgModuleConfig = dbgSetting_;

/**
 * @brief   Enable debugging output over a given device.
 *
 * @param [in] @c dev_: DC3DbgDeviceSetting_t enum representing the device.
 * @return  None
 */
#define DBG_ENABLE_DEVICE( dev_ ) \
      glbDbgDeviceConfig |= dev_;

/**
 * @brief   Disable debugging output over a given device.
 *
 * @param [in] @c name_: DC3DbgDeviceSetting_t enum representing the device.
 * @return  None
 */
#define DBG_DISABLE_DEVICE( dev_ ) \
      glbDbgDeviceConfig &= ~dev_;

/**
 * @brief   Toggle debugging output for a given module.
 *
 * @param [in] @c name_: DC3DbgDeviceSetting_t enum representing the device.
 * @return  None
 */
#define DBG_IS_DEVICE_ENABLED( dev_ ) \
      ( glbDbgDeviceConfig & dev_ )

/**
 * @brief   Disable debugging output for a given module.
 *
 * @param [in] @c name_: DC3DbgDeviceSetting_t enum representing the device.
 * @return  None
 */
#define DBG_DISABLE_ALL_DEVICES( ) \
      glbDbgDeviceConfig = 0;

/**
 * @brief   Set debugging output for all modules, overwriting current.
 * This is used to set the debugging to settings from the DB.
 * @param   [in] @c dbgDevices_: uint8_t bitfield representing ORring of all
 * enabled devices.
 * @return  None
 */
#define DBG_SET_DEBUG_FOR_ALL_DEVICES( dbgDevices_ ) \
      glbDbgDeviceConfig = dbgDevices_;

/**
 * @brief   Conditional error output
 *
 * @description
 * Macro that can be used to conditionally print an error.  If the passed in
 * status is not ERR_NONE (no error), the macro will use the passed in
 * accessType to figure out how to (slow and blocking or fast and
 * non-blocking/non-invasively) to output the passed in string.
 *
 * @param [in] status: status to parse and print if not ERR_NONE.
 *    @arg ERR_NONE: nothing will be output if this is passed in
 *    else: passed in string will be printed.
 * @param [in] accessType: AccessType_t enum representing how the caller wants
 *                         to output the error msg if needed.
 *    @arg ACCESS_BARE_METAL: slow blocking output.
 *    @arg ACCESS_QPC: fast, non-blocking
 *    @arg ACCESS_FREERTOS: fast, non-blocking
 * @param [in] fmt: printf like argument string with any % modifiers that print can handle.
 * @param [in] ...: printf like variable argument list.
 *
 * @note 1: This macro should be treated just like a conditional printf
 * @note 2: This macro DOES NOT perform a return.  This is left up to the user
 * since doing a return inside of a macro is bad practice for debugging.
 */
#define ERR_COND_OUTPUT(status, accessType, fmt, ...) {                       \
      if ((status) != ERR_NONE) {                                             \
         switch( accessType ) {                                               \
            case _DC3_ACCESS_FRT:                                             \
            case _DC3_ACCESS_QPC:                                             \
               ERR_printf(fmt, ##__VA_ARGS__);                                \
               break;                                                         \
            case _DC3_ACCESS_BARE:                                            \
            default:                                                          \
               err_slow_printf(fmt, ##__VA_ARGS__);                           \
               break;                                                         \
         }                                                                    \
      }                                                                       \
   }

/**
 * @brief   Conditional Warning output
 *
 * @description
 * Macro that can be used to conditionally print an warning.  If the passed in
 * status is not ERR_NONE (no error), the macro will use the passed in
 * accessType to figure out how to (slow and blocking or fast and
 * non-blocking/non-invasively) to output the passed in string.
 *
 * @param [in] status: status to parse and print if not ERR_NONE.
 *    @arg ERR_NONE: nothing will be output if this is passed in
 *    else: passed in string will be printed.
 * @param [in] accessType: AccessType_t enum representing how the caller wants
 *                         to output the error msg if needed.
 *    @arg ACCESS_BARE_METAL: slow blocking output.
 *    @arg ACCESS_QPC: fast, non-blocking
 *    @arg ACCESS_FREERTOS: fast, non-blocking
 * @param [in] fmt: printf like argument string with any % modifiers that print can handle.
 * @param [in] ...: printf like variable argument list.
 *
 * @note 1: This macro should be treated just like a conditional printf
 * @note 2: This macro DOES NOT perform a return.  This is left up to the user
 * since doing a return inside of a macro is bad practice for debugging.
 */
#define WRN_COND_OUTPUT(status, accessType, fmt, ...) {                       \
      if ((status) != ERR_NONE) {                                             \
         switch( accessType ) {                                               \
            case _DC3_ACCESS_FRT:                                             \
            case _DC3_ACCESS_QPC:                                             \
               WRN_printf(fmt, ##__VA_ARGS__);                                \
               break;                                                         \
            case _DC3_ACCESS_BARE:                                            \
            default:                                                          \
               wrn_slow_printf(fmt, ##__VA_ARGS__);                           \
               break;                                                         \
         }                                                                    \
      }                                                                       \
   }

/** @addtogroup groupDbgFast
 * @{
 */

/**
 * @brief Debug print function.
 *
 * This macro behaves just like a printf function but prepends "DBG:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * @note: This macro checks the DEBUG flag (compile time) and global debug
 * control to allow filtering.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * DBG_printf("Debug print test %d\n", i);
 *
 * will output:
 * DBG-00:04:09:459-function_name():219: Debug print test 0
 * @endcode
 *
 * @note: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define DBG_printf(fmt, ...) \
      do { \
            if ( glbDbgModuleConfig & DBG_this_module_ ) { \
               CON_output(_DC3_DBG, __func__, __LINE__, fmt, \
                  ##__VA_ARGS__); \
            } \
         } while (0)
#else
#define DBG_printf(fmt, ...) \
      do { \
            if ( glbDbgModuleConfig & DBG_this_module_ ) { \
               CON_slow_output(_DC3_DBG, __func__, __LINE__, fmt, \
                  ##__VA_ARGS__); \
            } \
         } while (0)

#endif

#ifndef SLOW_PRINTF
#define DBG_printfHexStr(buffer, bufferLen, fmt, ...) \
      do { \
            if ( glbDbgModuleConfig & DBG_this_module_ ) { \
               CON_outputWithHexStr(_DC3_DBG, __func__, __LINE__, buffer, bufferLen, fmt, \
                  ##__VA_ARGS__); \
            } \
         } while (0)
#else
#define DBG_printfHexStr(buffer, bufferLen, fmt, ...) \
      do { \
            if ( glbDbgModuleConfig & DBG_this_module_ ) { \
               CON_outputWithHexStr(_DC3_DBG, __func__, __LINE__, buffer, bufferLen, fmt, \
                  ##__VA_ARGS__); \
            } \
         } while (0)

#endif

/**
 * @brief Logging print function.
 *
 * This macro behaves just like a printf function but prepends "LOG:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * LOG_printf("Logging print test %d\n", i);
 *
 * will output:
 * LOG-00:04:09:459-function_name():219: Logging print test 0
 * @endcode
 *
 * @note: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define LOG_printf(fmt, ...) \
      do { CON_output(_DC3_LOG, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#else
#define LOG_printf(fmt, ...) \
      do { CON_slow_output(_DC3_LOG, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif
/**
 * @brief Warning print function.
 *
 * This macro behaves just like a print function but prepends "WRN:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * WRN_printf("Warning print test %d\n", i);
 *
 * will output:
 * WRN-00:04:09:459-function_name():219: Warning print test 0
 * @endcode
 *
 * @note: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define WRN_printf(fmt, ...) \
      do { CON_output(_DC3_WRN, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#else
#define WRN_printf(fmt, ...) \
      do { CON_slow_output(_DC3_WRN, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif
/**
 * @brief Error print function.
 *
 * This macro behaves just like a printf function but prepends "ERR:", time,
 * function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * WRN_printf("Error print test %d\n", i);
 *
 * will output:
 * ERR-00:04:09:459-function_name():219: Error print test 0
 * @endcode
 *
 * @note: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#ifndef SLOW_PRINTF
#define ERR_printf(fmt, ...) \
      do { CON_output(_DC3_ERR, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#else
#define ERR_printf(fmt, ...) \
      do { CON_slow_output(_DC3_ERR, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif

/**
 * @brief Menu print function.
 *
 * This macro behaves just like a printf function.  It doesn't prepend
 * anything.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * MENU_printf("Menu print test %d\n", i);
 *
 * will output:
 * Menu print test 0
 * @endcode
 *
 * @note 1: This macro is always enabled.
 * @note 2: Use just as a regular printf with all the same flags.
 * @note 3: Don't use this for regular debugging since it will be difficult to
 * track down where this is being called since it prepends no location or
 * temporal data to help you.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
/*
#ifndef SLOW_PRINTF
#define MENU_printf(dst, fmt, ...) \
      do {  CON_output(_DC3_CON, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#else
#define MENU_printf(dst, fmt, ...) \
      do { CON_slow_output(_DC3_CON, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)
#endif
*/
/**
 * @} end group groupDbgFast
 */

/** @addtogroup groupDbgSlow
 * @{
 */

/**
 * @brief Slow debug print function.
 *
 * This macro behaves just like a printf function but prepends "DBG-SLOW!", time,
 * function name, line number, and finally, the user passed in message.
 *
 * @note: This macro checks the DEBUG flag (compile time) and global debug
 * control to allow filtering.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * dbg_slow_printf("Slow debug print test %d\n", i);
 *
 * will output:
 * DBG-SLOW!-00:04:09:459-function_name():219: Slow debug print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def DBG_printf() macro instead.
 * @note 2. This macro is disabled in Rel builds.
 * @note 3. Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define dbg_slow_printf(fmt, ...) \
      do { \
            if ( glbDbgModuleConfig & DBG_this_module_ ) { \
               CON_slow_output(_DC3_DBG, __func__, __LINE__, fmt, \
                     ##__VA_ARGS__); \
            } \
      } while (0)

/**
 * @brief Slow logging print function.
 *
 * This macro behaves just like a printf function but prepends "LOG-SLOW!",
 * time, function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * log_slow_printf("Slow logging print test %d\n", i);
 *
 * will output:
 * LOG-SLOW!-00:04:09:459-function_name():219: Slow logging print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def LOG_printf() macro instead.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define log_slow_printf(fmt, ...) \
      do { CON_slow_output(_DC3_LOG, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief Slow warning print function.
 *
 * This macro behaves just like a printf function but prepends "WRN-SLOW!",
 * time, function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * log_slow_printf("Slow warning print test %d\n", i);
 *
 * will output:
 * WRN-SLOW!-00:04:09:459-function_name():219: Slow warning print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def WRN_printf() macro instead.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define wrn_slow_printf(fmt, ...) \
      do { CON_slow_output(_DC3_WRN, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief Slow error print function.
 *
 * This macro behaves just like a printf function but prepends "ERR-SLOW!",
 * time, function name, line number, and finally, the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * err_slow_printf("Slow error print test %d\n", i);
 *
 * will output:
 * ERR-SLOW!-00:04:09:459-function_name():219: Slow error print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def ERR_printf() macro instead.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define err_slow_printf(fmt, ...) \
      do { CON_slow_output(_DC3_ERR, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief Slow isr debug print function.
 *
 * This macro behaves just like a printf function but prepends "D-ISR",
 * timestamp, and line number only in front of the user passed in message.
 *
 * Usage Example:
 *
 * @code
 * int i = 0;
 * isr_debug_slow_printf("Slow isr debug print test %d\n", i);
 *
 * will output:
 * D-ISR:-00:04:09:459-219:Slow isr debug print test 0
 * @endcode
 *
 * @note 1. Don't use this after the BSP and RTOS has been initialized since it
 * is a blocking call and will affect the speed of operation.  Use
 * @def DBG_printf() macro instead.
 * @note 2: Use just as a regular printf with all the same flags.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 *
 * @return None
 */
#define isr_dbg_slow_printf(fmt, ...) \
      do { CON_slow_output(_DC3_ISR, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/**
 * @brief  Sets the default module debug settings and default devices.
 *
 * This function sets default settings for which debug modules are enabled
 * and over which devices the debugging output is enabled over.
 *
 * This function is safe to call at any time.
 *
 * @note: this function behaves differently depending on whether the build is
 * debug or release
 *
 * @note: this only sets the defaults in memory.  It DOES NOT save the settings
 * to EEPROM.
 *
 * @param  None
 * @return None
 */
void DBG_setDefaults( void );


 #ifdef __cplusplus
}
#endif
/**
 * @} end group groupDbgSlow
 */
#endif                                                        /* DBG_CNTRL_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
