/**
 * @file    console_output.h
 * @brief   Serial output to the UART.
 *
 * This file contains the declarations for debug and output functions over the
 * serial DMA console and over regular serial blocking.
 *
 * @date   06/09/2014
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupConOut
 * @{
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONSOLE_OUTPUT_H_
#define CONSOLE_OUTPUT_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "Shared.h"
#include "dbg_cntrl.h"                                   /* For debug control */
#include "i2c_defs.h"
#include "db.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Function that gets called by the XXX_printf() macros to output a
 * dbg/log/wrn/err to DMA serial console.
 *
 * Basic console output function which should be called by the various macro
 * functions to do the actual output to serial.  Takes in parameters that allow
 * easy logging level specification, file, function name, line number, etc.
 * These are prepended in front of the data that was actually sent in to be
 * printed.
 *
 * Function performs the following steps:
 *    -# Gets the timestamp.  This timestamp represents when the call was
 *    actually made since by the time it's output, time can/will have passed.
 *    -# Constructs a new msg event pointer and allocates storage in the QP
 *    event pool.
 *    -# Decides the output format based on which macro was called DBG_printf(),
 *    LOG_printf(), WRN_printf(), ERR_printf(), or CON_printf() and writes it to
 *    the event pointer msg buffer and sets the length in the event pointer.
 *    -# Pass the va args list to get output to a buffer, making sure to not
 *    overwrite the prepended data.
 *    -# Append the actual user supplied data to the buffer and set the length.
 *    -# Publish the event and return.  The event will be handled (queued or
 *     executed) by SerialMgr AO when it is able to do.  See @SerialMgr
 *     documentation for details.
 *
 * @note 1: Do not call this function directly.  Instead, call on of the
 * DBG/LOG/WRN/ERR/CON_printf() macros.  Printout from these looks something
 * like:
 * DBG_LEVEL-HH:MM:SS:XXX-SomeFunctionName():fileLineNumber: User message here
 *
 * @note 2: Instead of directly printing to the serial console, it creates a
 * SerDataEvt and sends the data to be output to serial via DMA.  This prevents
 * slow downs due to regular printf() to serial.
 *
 * @param  [in] dbgLvl: a DC3DbgLevel_t variable that specifies the logging
 * level to use.
 *   @arg DBG: Lowest level of debugging.  Everything above this level is
 *   printed.  Disabled in Release builds.  Prints "DBG" in place of "DBG_LEVEL".
 *   @arg LOG: Basic logging. Everything above this level is printed.
 *   Disabled in Release builds. Prints "LOG" in place of "DBG_LEVEL".
 *   @arg WRN: Warnings.  Everything above this level is printed. Enabled in
 *   Release builds. Prints "WRN" in place of "DBG_LEVEL".
 *   @arg ERR: Errors. Enabled in all builds. Prints "ERR" in place of "DBG_LEVEL".
 *   @arg CON: Regular output to the console without prepending anything.
 *   Enabled in all builds. Just the "User message here" will be printed.  This
 *   is meant to output serial menu items.
 *
 * @param [in] pFuncName: const char* pointer to the function name where the
 * macro was called from.
 *
 * @param [in] wLineNumber: line number where the macro was called from.
 *
 * @param [in] fmt: const char* pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
void CON_output(
      DC3DbgLevel_t dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
);

/**
 * @brief Function that gets called by the XXX_printfHexStr() macros to output a
 * dbg/log/wrn/err to DMA serial console, along with a pretty print of a given
 * hex array.
 *
 * Basic console output function which should be called by the various macro
 * functions to do the actual output to serial.  Takes in parameters that allow
 * easy logging level specification, file, function name, line number, etc.
 * These are prepended in front of the data that was actually sent in to be
 * printed.
 *
 * Function performs the following steps:
 *    -# Gets the timestamp.  This timestamp represents when the call was
 *    actually made since by the time it's output, time can/will have passed.
 *    -# Constructs a new msg event pointer and allocates storage in the QP
 *    event pool.
 *    -# Decides the output format based on which macro was called DBG_printf(),
 *    LOG_printf(), WRN_printf(), ERR_printf(), or CON_printf() and writes it to
 *    the event pointer msg buffer and sets the length in the event pointer.
 *    -# Pass the va args list to get output to a buffer, making sure to not
 *    overwrite the prepended data.
 *    -# Append the actual user supplied data to the buffer and set the length.
 *    -# Publish the event and return.  The event will be handled (queued or
 *     executed) by SerialMgr AO when it is able to do.  See @SerialMgr
 *     documentation for details.
 *    -# Prints out the hex string 16 bytes at a time along with a "page number"
 *    to help readability.
 *
 * @note 1: Do not call this function directly.  Instead, call on of the
 * DBG/LOG/WRN/ERR/CON_printfHexStr() macros.  Printout from these looks
 * something like:
 * DBG_LEVEL-HH:MM:SS:XXX-SomeFunctionName():fileLineNumber:User message here
 * DBG_LEVEL-HH:MM:SS:XXX-SomeFunctionName():fileLineNumber:[0000] 16 bytes
 * DBG_LEVEL-HH:MM:SS:XXX-SomeFunctionName():fileLineNumber:[0010] 16 bytes
 * ...
 * etc
 * ...
 *
 * or a real life example:
 *
 * DBG-00:06:40:361-I2C1DevMgr_Busy():462:Attempting to write 52 bytes:
 * DBG-00:06:40:361-I2C1DevMgr_Busy():462:[0000]: 0xdb 0xc8 0xfe 0xde 0x01 0x00 0xac 0x1b 0x00 0x4b 0x00 0x01 0x32 0x30 0x31 0x35
 * DBG-00:06:40:361-I2C1DevMgr_Busy():462:[0010]: 0x30 0x38 0x31 0x39 0x31 0x37 0x30 0x34 0x33 0x36 0x00 0x00 0x00 0x00 0x00 0x00
 * DBG-00:06:40:361-I2C1DevMgr_Busy():462:[0020]: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xe9 0x3a 0x00 0x00
 * DBG-00:06:40:361-I2C1DevMgr_Busy():462:[0030]: 0x03 0x00 0x00 0x00
 *
 * @note 2: when printing to a @CON debug level, no data is prepended to the front
 * of the buffer.  This should be used to do menu output to the console.
 *
 * @note 3: Instead of directly printing to the serial console, it creates a
 * SerDataEvt and sends the data to be output to serial via DMA.  This prevents
 * slow downs due to regular printf() to serial.
 *
 * @param  [in] dbgLvl: a DC3DbgLevel_t variable that specifies the logging
 * level to use.
 *   @arg DBG: Lowest level of debugging.  Everything above this level is
 *   printed.  Disabled in Release builds.  Prints "DBG" in place of "DBG_LEVEL".
 *   @arg LOG: Basic logging. Everything above this level is printed.
 *   Disabled in Release builds. Prints "LOG" in place of "DBG_LEVEL".
 *   @arg WRN: Warnings.  Everything above this level is printed. Enabled in
 *   Release builds. Prints "WRN" in place of "DBG_LEVEL".
 *   @arg ERR: Errors. Enabled in all builds. Prints "ERR" in place of "DBG_LEVEL".
 *   @arg CON: Regular output to the console without prepending anything.
 *   Enabled in all builds. Just the "User message here" will be printed.  This
 *   is meant to output serial menu items.
 *
 * @param [in] pFuncName: const char* pointer to the function name where the
 * macro was called from.
 *
 * @param [in] wLineNumber: line number where the macro was called from.
 *
 * @param [in] fmt: const char* pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
void CON_outputWithHexStr(
      const DC3DbgLevel_t dbgLvl,
      const char* pFuncName,
      const uint16_t wLineNumber,
      const uint8_t* const pBuffer,
      const size_t bufferSize,
      char *fmt,
      ...
);

/**
 * @brief Function called by the slow_xxx_printf() macros.
 *
 * Basic console output function which should be called by the various macro
 * functions to do the actual output to serial.  Takes in parameters that allow
 * easy logging level specification, file, function name, line number, etc.
 * These are prepended in front of the data that was actually sent in to be
 * printed.
 * 1. Gets the timestamp.  This timestamp represents when the call was actually
 * made since by the time it's output, time can/will have passed.
 * 2. Decides the output format based on which macro was called
 * slow_dbg_printf(), slow_log_printf(), slow_wrn_printf(), slow_err_printf(),
 * or isr_slow_debug_printf() and writes it to a temporary buffer.
 * 3. Pass the va args list to get output to a buffer, making sure to not
 * overwrite the prepended data.
 * 4. Performs a blocking write of the buffer to the serial port without using
 * DMA and returns.
 *
 * @note 1: when printing to a @CON debug level, nothing is output.
 *
 * @note 2: This function prints directly printing to the serial console and is
 * fairly slow.  Use the xxx_slow_printf macros instead.  This function is only
 * called by the slow macros which have to be called in the initializations
 * before the QPC RTOS is running and should not be used after.
 *
 * @param  [in] dbgLvl: a DC3DbgLevel_t variable that specifies the logging
 * level to use.
 *   @arg DBG: Lowest level of debugging.  Everything above this level is
 *   printed.  Disabled in Release builds.
 *   @arg LOG: Basic logging. Everything above this level is printed.
 *   Disabled in Release builds.
 *   @arg WRN: Warnings.  Everything above this level is printed. Enabled in
 *   Release builds.
 *   @arg ERR: Errors. Enabled in all builds.
 *
 * @param [in] *pFuncName: pointer to the function name where the macro was
 * called from.
 *
 * @param [in] wLineNumber: line number where the macro was called from.
 *
 * @param [in] *fmt: const char pointer to the data to be printed using the
 * va_args type argument list.
 *
 * @param [in] ... : the variable list of arguments from above.  This allows
 * the function to be called like any xprintf() type function.
 * @return None
 */
void CON_slow_output(
      DC3DbgLevel_t dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
);

/**
 * @brief   Convert a uint8_t hex array to a string array.
 *
 * @note: passed in buffer for string output should be at least 5x the size of
 * the original data.
 *
 * @param [in] hexData: const char* pointer to the buffer that contains the hex
 * data to convert.
 * @param [in] hexDataLen: uint16_t length of data in the hexData buffer.
 * @param [in,out] strDataBuffer: char* pointer to the buffer where to write the
 * result.  This should be allocated by the caller.
 * @param [in] strDataBufferSize: uint16_t max length of the strDataBuffer.
 * @param [in,out] strDataLen: uint16_t* pointer to the size of the data in the
 * strDataBuffer after it has been written.
 * @param [in] outputNColumns: uint8_t number of columns to break up the
 * resulting string into with newlines. If set to zero, a single long row is
 * returned without any newlines.
 * @param [in] sep: char that will be used to separate all the printed hex numbers.
 * @param [in] bPrintX: bool that specifies whether to print 0x in front of each
 * hex number.
 * @return DC3Error_t: ERR_NONE if OK.
 */
DC3Error_t CON_hexToStr(
      const uint8_t* hexData,
      uint16_t hexDataLen,
      char* strDataBuffer,
      uint16_t strDataBufferSize,
      uint16_t* strDataLen,
      uint8_t outputNColumns,
      const char sep,
      const bool bPrintX
);

/**
 * @brief   Get a string representation of a system access type.
 *
 * @param  [in] elem: DC3AccessType_t that specifies the access to retrieve.
 * @return str: char* representation of if found,
 *             "Invalid" if not found.
 */
const char* const CON_accessToStr( const DC3AccessType_t acc );

/**
 * @brief   Get a string representation of a DB element.
 *
 * @param  [in] elem: DC3DBElem_t that specifies what element to convert
 * to string.
 * @return str: char* representation of DB element if found,
 *             "Invalid" if not found.
 */
const char* const CON_dbElemToStr( const DC3DBElem_t elem );

/**
 * @brief   Get a string representation of a DC3 API Messages Name
 *
 * @param  [in] msg: DC3MsgName_t that specifies what message name to convert
 * to string.
 * @return str: char* representation of DC3MsgName_t msg if found,
 *             "Invalid" if not found.
 */
const char* const CON_msgNameToStr( const DC3MsgName_t msg );

/**
 * @brief   Get a string representation of a DC3 I2C device
 *
 * @param  [in] msg: DC3I2CDevice_t that specifies what I2C device to convert
 * to string.
 * @return str: char* representation of DC3I2CDevice_t msg if found,
 *             "Invalid" if not found.
 */
const char* const CON_i2cDevToStr( const DC3I2CDevice_t iDev );

/**
 * @brief   Get a string representation of a DC3 I2C operation
 *
 * @param  [in] msg: I2C_Operation_t that specifies what I2C operation to convert
 * to string.
 * @return str: char* representation of I2C_Operation_t msg if found,
 *             "Invalid" if not found.
 */
const char* const CON_i2cOpToStr( const I2C_Operation_t iOp );

/**
 * @brief   Get a string representation of a DC3 I2C Bus
 *
 * @param  [in] msg: I2C_Bus_t that specifies what I2C Bus to convert
 * to string.
 * @return str: char* representation of I2C_Bus_t msg if found,
 *             "Invalid" if not found.
 */
const char* const CON_i2cBusToStr( const I2C_Bus_t iBus );

/**
 * @brief   Get a string representation of a DC3 Debug level
 *
 * @param  [in] msg: DC3DbgLevel_t that specifies what debug level to convert
 * to string.
 * @return str: char* representation of DC3DbgLevel_t msg if found,
 *             "CON" if not found.
 */
const char* const CON_dbgLvlToStr( const DC3DbgLevel_t lvl );

/**
 * @brief   Get a string representation of a DC3 DB operation
 *
 * @param  [in] msg: DB_Operation_t that specifies what DB operation to convert
 * to string.
 * @return str: char* representation of DB_Operation_t msg if found,
 *             "Invalid" if not found.
 */
const char* const CON_dbOpToStr( const DB_Operation_t op );

/**
 * @} end group groupConOut
 */

#endif                                                   /* CONSOLE_OUTPUT_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
