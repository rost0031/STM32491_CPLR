/**
 * @file    console_output.c
 * @brief   Serial output to the UART.
 *
 * This file contains the definitions for debug and output functions over the
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
/* Includes ------------------------------------------------------------------*/
#include "console_output.h"
#include "qp_port.h"                                        /* for QP support */
#include "DC3Signals.h"
#include "DC3Errors.h"
#include "Shared.h"                                   /*  Common Declarations */
#include "time.h"
#include "SerialMgr.h"
#include "LWIPMgr.h"
#include "qp_port.h"                                               /* QP-port */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_DBG ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
const char invalidStr[] = "Invalid";

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief Common function that gets called by the CON_output* functions to
 * output a dbg/log/wrn/err to DMA serial console that takes in a va_list args.
 *
 * Basic console output function which should be called by the various macro
 * functions to do the actual output to serial.  Takes in parameters that allow
 * easy logging level specification, file, function name, line number, etc.
 * These are prepended in front of the data that was actually sent in to be
 * printed.
 *
 * Function performs the following steps:
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
static void CON_vOutputMsg(
      const DC3DbgLevel_t dbgLvl,
      const char *pFuncName,
      const uint16_t wLineNumber,
      const char *fmt,
      va_list argp,
      const stm32Time_t time
);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
static void CON_vOutputMsg(
      const DC3DbgLevel_t dbgLvl,
      const char *pFuncName,
      const uint16_t wLineNumber,
      const char *fmt,
      va_list argp,
      const stm32Time_t time
)
{

   /* 1. Construct a new msg event pointer and allocate storage in the QP event
    * pool.  Allocate with margin so we can fall back on regular slow printfs if
    * there's a problem */
   LrgDataEvt *lrgDataEvt = Q_NEW(LrgDataEvt, DBG_LOG_SIG);
   lrgDataEvt->dataLen = 0;
   lrgDataEvt->src = _DC3_NoRoute;
   lrgDataEvt->dst = _DC3_NoRoute;

   /* 2. Based on the debug level specified by the calling macro, decide what to
    * prepend (if anything). */
   lrgDataEvt->dataLen += snprintf(
         (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
         DC3_MAX_MSG_LEN,
         "%s-%02d:%02d:%02d:%03d-%s():%d:",
         CON_dbgLvlToStr(dbgLvl),
         time.hour_min_sec.RTC_Hours,
         time.hour_min_sec.RTC_Minutes,
         time.hour_min_sec.RTC_Seconds,
         (int)time.sub_sec,
         pFuncName,
         wLineNumber
   );

   /* 3. Print the actual user supplied data to the buffer and set the length */
   lrgDataEvt->dataLen += vsnprintf(
         (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
         DC3_MAX_MSG_LEN - lrgDataEvt->dataLen, // Account for the part of the buffer that was already written.
         fmt,
         argp
   );

   /* 4. Publish the event*/
   QF_PUBLISH((QEvent *)lrgDataEvt, 0);
}

/******************************************************************************/
void CON_output(
      DC3DbgLevel_t dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
)
{
   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   stm32Time_t time = TIME_getTime();

   /* 2. Call the variadic function to output the msg */
   va_list args;
   va_start(args, fmt);
   CON_vOutputMsg( dbgLvl, pFuncName, wLineNumber, fmt, args, time );
   va_end(args);
}

/******************************************************************************/
void CON_outputWithHexStr(
      const DC3DbgLevel_t dbgLvl,
      const char* pFuncName,
      const uint16_t wLineNumber,
      const uint8_t* const pBuffer,
      const size_t bufferSize,
      char *fmt,
      ...
)
{
   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   stm32Time_t time = TIME_getTime();

   /* 2. Call the variadic function to output the basic msg */
   va_list args;
   va_start(args, fmt);
   CON_vOutputMsg( dbgLvl, pFuncName, wLineNumber, fmt, args, time );
   va_end(args);

   /* 3. In separate events, output the hex string 16 bytes at a time */
   uint8_t numbersPerRow = 16;
   uint8_t nEventsNeeded = (bufferSize / numbersPerRow) + ( bufferSize % numbersPerRow != 0 ? 1 : 0 );
   uint8_t currNumber = 0;

   for ( uint8_t i = 0; i < nEventsNeeded; i++ ) {
      /* Allocate a new event for each line that we are printing and reset its
       * guts to defaults. */
      LrgDataEvt *lrgDataEvt = Q_NEW(LrgDataEvt, DBG_LOG_SIG);
      lrgDataEvt->dataLen = 0;
      lrgDataEvt->src = _DC3_NoRoute;
      lrgDataEvt->dst = _DC3_NoRoute;

      /* Print the preamble every line so we don't confuse the client (which
       * parses the output if connected over serial) or the user who is used
       * to seeing debug level, time, etc printed on every line. */
      lrgDataEvt->dataLen += snprintf(
            (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
            DC3_MAX_MSG_LEN,
            "%s-%02d:%02d:%02d:%03d-%s():%d:[%04x]: ",
            CON_dbgLvlToStr(dbgLvl),
            time.hour_min_sec.RTC_Hours,
            time.hour_min_sec.RTC_Minutes,
            time.hour_min_sec.RTC_Seconds,
            (int)time.sub_sec,
            pFuncName,
            wLineNumber,
            currNumber
      );

      /* Go from current number until either the end of a "page" or end of the
       * buffer, whichever happens to be smaller this iteration*/
      size_t j;
      for( j = currNumber; j < MIN(bufferSize, currNumber + numbersPerRow); j++ ) {
         lrgDataEvt->dataLen += snprintf(
               (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
               DC3_MAX_MSG_LEN - lrgDataEvt->dataLen, // Account for the part of the buffer that was already written.
               "0x%02x ", pBuffer[j]
         );
      }

      /* Append a newline after the line is printed to the output buffer */
      lrgDataEvt->dataLen += snprintf(
            (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
            DC3_MAX_MSG_LEN - lrgDataEvt->dataLen, // Account for the part of the buffer that was already written.
            "\n"
      );

      /* Update the counter so we know where to resume printing. */
      currNumber = j;

      /* Publish the event with the finished line */
      QF_PUBLISH((QEvt *)lrgDataEvt, 0);
   }
}


/******************************************************************************/
void CON_slow_output(
      DC3DbgLevel_t dbgLvl,
      const char *pFuncName,
      uint16_t wLineNumber,
      char *fmt,
      ...
)
{
   /* 1. Get the time first so the printout of the event is as close as possible
    * to when it actually occurred */
   stm32Time_t time = TIME_getTime();

   /* Temporary local buffer and index to compose the msg */
   char tmpBuffer[DC3_MAX_MSG_LEN];
   uint8_t tmpBufferIndex = 0;

   /* 2. Based on the debug level specified by the calling macro, decide what to
    * prepend (if anything). */
   switch (dbgLvl) {
      case _DC3_DBG:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               DC3_MAX_MSG_LEN,
               "DBG-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case _DC3_LOG:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               DC3_MAX_MSG_LEN,
               "LOG-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case _DC3_WRN:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               DC3_MAX_MSG_LEN,
               "WRN-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case _DC3_ERR:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               DC3_MAX_MSG_LEN,
               "ERR-SLOW!-%02d:%02d:%02d:%03d-%s():%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               pFuncName,
               wLineNumber
         );
         break;
      case _DC3_ISR:
         tmpBufferIndex += snprintf(
               tmpBuffer,
               DC3_MAX_MSG_LEN,
               "D-ISR!-%02d:%02d:%02d:%03d-:%d:",
               time.hour_min_sec.RTC_Hours,
               time.hour_min_sec.RTC_Minutes,
               time.hour_min_sec.RTC_Seconds,
               (int)time.sub_sec,
               wLineNumber
         );
         break;
      case _DC3_CON: // This is not used so don't prepend anything
      default:
         break;
   }

   /* 3. Pass the va args list to get output to a buffer, making sure to not
    * overwrite the prepended data. */
   va_list args;
   va_start(args, fmt);

   tmpBufferIndex += vsnprintf(
         &tmpBuffer[tmpBufferIndex],
         DC3_MAX_MSG_LEN - tmpBufferIndex, // Account for the part of the buffer that was already written.
         fmt,
         args
   );
   va_end(args);

   /* 4. Print directly to the console now. THIS IS A SLOW OPERATION! */
   fwrite(tmpBuffer, tmpBufferIndex, 1, stderr);
}

/******************************************************************************/
DC3Error_t CON_hexToStr(
      const uint8_t* hexData,
      uint16_t hexDataLen,
      char* strDataBuffer,
      uint16_t strDataBufferSize,
      uint16_t* strDataLen,
      uint8_t outputNColumns,
      const char sep,
      const bool bPrintX
)
{
   DC3Error_t status = ERR_NONE;

   if ( NULL == hexData ) {
      ERR_printf("Passed in a NULL buffer\n");
      return( ERR_MEM_NULL_VALUE );
   }

   if ( 0 == hexDataLen ) {
      ERR_printf("Passed in zero length data buffer for input\n");
      return( ERR_MEM_BUFFER_LEN );
   }

   if ( 0 == strDataBufferSize ) {
      ERR_printf("Passed in zero length data buffer for output\n");
      return( ERR_MEM_BUFFER_LEN );
   } else if ( bPrintX == true && strDataBufferSize < 5 * hexDataLen ) {
      WRN_printf("Output buffer too small to contain all the data.\n");
      WRN_printf("Should be at least %d long\n", 5 * hexDataLen);
      WRN_printf("Continuing but the resulting data will be incomplete\n");
      status = ERR_MEM_BUFFER_LEN;
   } else if ( bPrintX == false && strDataBufferSize < 3 * hexDataLen ) {
      WRN_printf("Output buffer too small to contain all the data.\n");
      WRN_printf("Should be at least %d long\n", 3 * hexDataLen);
      WRN_printf("Continuing but the resulting data will be incomplete\n");
      status = ERR_MEM_BUFFER_LEN;
   }
   /* Index used to keep track of how far into the buffer we've printed */
   *strDataLen = 0;

   for ( uint16_t i = 0; i < hexDataLen; i++ ) {

      /* Let user zero num of columns but if they do, just give them back one
       * long row of data without any linebreaks. */
      if ( 0 != outputNColumns && i % outputNColumns == 0 && i != 0 ) {
         *strDataLen += snprintf(
               &strDataBuffer[*strDataLen],
               strDataBufferSize - *strDataLen,
               "\n"
         );
      }
      /* Print the actual number after checking if we need to print 0x in front*/
      if ( true == bPrintX ) {
         *strDataLen += snprintf(
               &strDataBuffer[*strDataLen],
               strDataBufferSize - *strDataLen,
               "0x%02x%c",
               hexData[i],
               sep
         );
      } else {
         *strDataLen += snprintf(
               &strDataBuffer[*strDataLen],
               strDataBufferSize - *strDataLen,
               "%02x%c",
               hexData[i],
               sep
         );
      }

      if ( *strDataLen >= strDataBufferSize ) {
         return( ERR_MEM_BUFFER_LEN );
      }
   }
   return( status );
}

/******************************************************************************/
//void CON_slow_outputWithHexStr(
//      DC3DbgLevel_t dbgLvl,
//      const DC3MsgRoute_t src,
//      const DC3MsgRoute_t dst,
//      const char* pFuncName,
//      uint16_t wLineNumber,
//      const uint8_t* const pBuffer,
//      const size_t bufferSize,
//      char *fmt,
//      ...
//)
//{
//   /* 1. Get the time first so the printout of the event is as close as possible
//    * to when it actually occurred */
//   stm32Time_t time = TIME_getTime();
//
//   /* 2. Construct a new msg event pointer and allocate storage in the QP event
//    * pool.  Allocate with margin so we can fall back on regular slow printfs if
//    * there's a problem */
//   LrgDataEvt *lrgDataEvt = Q_NEW(LrgDataEvt, DBG_LOG_SIG);
//   lrgDataEvt->dataLen = 0;
//   lrgDataEvt->src = src;
//   lrgDataEvt->dst = dst;
//
//   /* 3. Based on the debug level specified by the calling macro, decide what to
//    * prepend (if anything). */
//   lrgDataEvt->dataLen += snprintf(
//         (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
//         DC3_MAX_MSG_LEN,
//         "%s-%02d:%02d:%02d:%03d-%s():%d:",
//         CON_dbgLvlToStr(dbgLvl),
//         time.hour_min_sec.RTC_Hours,
//         time.hour_min_sec.RTC_Minutes,
//         time.hour_min_sec.RTC_Seconds,
//         (int)time.sub_sec,
//         pFuncName,
//         wLineNumber
//   );
//
//   /* 4. Pass the va args list to get output to a buffer */
//   va_list args;
//   va_start(args, fmt);
//
//   /* 5. Print the actual user supplied data to the buffer and set the length */
//   lrgDataEvt->dataLen += vsnprintf(
//         (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
//         DC3_MAX_MSG_LEN - lrgDataEvt->dataLen, // Account for the part of the buffer that was already written.
//         fmt,
//         args
//   );
//   va_end(args);
//
//   /* 6. Publish the event*/
//   QF_PUBLISH((QEvent *)lrgDataEvt, 0);
//
//   /* 7. In separate events, output the hex string 16 bytes at a time */
//   uint8_t numbersPerRow = 16;
//   uint8_t nEventsNeeded = (bufferSize / numbersPerRow) + ( bufferSize % numbersPerRow != 0 ? 1 : 0 );
//   uint8_t currNumber = 0;
//
//   for ( uint8_t i = 0; i < nEventsNeeded; i++ ) {
//      /* Allocate a new event for each line that we are printing and reset its
//       * guts to defaults. */
//      lrgDataEvt = Q_NEW(LrgDataEvt, DBG_LOG_SIG);
//      lrgDataEvt->dataLen = 0;
//      lrgDataEvt->src = src;
//      lrgDataEvt->dst = dst;
//
//      /* Print the preamble every line so we don't confuse the client (which
//       * parses the output if connected over serial) or the user who is used
//       * to seeing debug level, time, etc printed on every line. */
//      lrgDataEvt->dataLen += snprintf(
//            (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
//            DC3_MAX_MSG_LEN,
//            "%s-%02d:%02d:%02d:%03d-%s():%d: Page %04x: ",
//            CON_dbgLvlToStr(dbgLvl),
//            time.hour_min_sec.RTC_Hours,
//            time.hour_min_sec.RTC_Minutes,
//            time.hour_min_sec.RTC_Seconds,
//            (int)time.sub_sec,
//            pFuncName,
//            wLineNumber,
//            currNumber
//      );
//
//      /* Go from current number until either the end of a "page" or end of the
//       * buffer, whichever happens to be smaller this iteration*/
//      size_t j;
//      for( j = currNumber; j < MIN(bufferSize, currNumber + numbersPerRow); j++ ) {
//         lrgDataEvt->dataLen += snprintf(
//               (char *)&lrgDataEvt->dataBuf[lrgDataEvt->dataLen],
//               DC3_MAX_MSG_LEN - lrgDataEvt->dataLen, // Account for the part of the buffer that was already written.
//               "0x%02x ", pBuffer[j]
//         );
//      }
//
//      currNumber += j;
//
//      QF_PUBLISH((QEvt *)lrgDataEvt, 0);
//   }
//}

/******************************************************************************/
const char* const CON_accessToStr( const DC3AccessType_t acc )
{
   switch ( acc ) {
      case _DC3_ACCESS_BARE:           return("BARE");      break;
      case _DC3_ACCESS_QPC:            return("QPC");       break;
      case _DC3_ACCESS_FRT:            return("FRT");       break;
      case _DC3_ACCESS_NONE:                    /* Intentionally fall through */
      default:                         return(invalidStr);  break;
   }
}

/******************************************************************************/
const char* const CON_dbElemToStr( const DC3DBElem_t elem )
{
   switch ( elem ) {
      case _DC3_DB_MAGIC_WORD:          return("DB_MAGIC_WORD");          break;
      case _DC3_DB_VERSION:             return("DB_VERSION");             break;
      case _DC3_DB_MAC_ADDR:            return("DB_MAC_ADDR");            break;
      case _DC3_DB_IP_ADDR:             return("DB_IP_ADDR");             break;
      case _DC3_DB_SN:                  return("DB_SN");                  break;
      case _DC3_DB_BOOT_MAJ:            return("DB_BOOT_MAJ");            break;
      case _DC3_DB_BOOT_MIN:            return("DB_BOOT_MIN");            break;
      case _DC3_DB_BOOT_BUILD_DATETIME: return("DB_BOOT_BUILD_DATETIME"); break;
      case _DC3_DB_APPL_MAJ:            return("DB_APPL_MAJ");            break;
      case _DC3_DB_APPL_MIN:            return("DB_APPL_MIN");            break;
      case _DC3_DB_APPL_BUILD_DATETIME: return("DB_APPL_BUILD_DATETIME"); break;
      case _DC3_DB_FPGA_MAJ:            return("DB_FPGA_MAJ");            break;
      case _DC3_DB_FPGA_MIN:            return("DB_FPGA_MIN");            break;
      case _DC3_DB_FPGA_BUILD_DATETIME: return("DB_FPGA_BUILD_DATETIME"); break;
      case _DC3_DB_DBG_MODULES:         return("DB_DBG_MODULES");         break;
      case _DC3_DB_DBG_DEVICES:         return("DB_DBG_DEVICES");         break;
      default:                          return(invalidStr);               break;
   }
}

/******************************************************************************/
const char* const CON_msgNameToStr( const DC3MsgName_t msg )
{
   /* Make sure to take off the "_DC3" prefix and the "Msg" from the returned
    * strings.  This saves flash space */
   switch ( msg ) {
      case _DC3NoMsg:                  return("None");                  break;
      case _DC3StatusPayloadMsg:       return("StatusPayload");         break;
      case _DC3GetVersionMsg:          return("GetVersion");            break;
      case _DC3VersionPayloadMsg:      return("VersionPayload");        break;
      case _DC3GetBootModeMsg:         return("GetBootMode");           break;
      case _DC3SetBootModeMsg:         return("SetBootMode");           break;
      case _DC3BootModePayloadMsg:     return("BootModePayload");       break;
      case _DC3FlashMsg:               return("Flash");                 break;
      case _DC3FlashMetaPayloadMsg:    return("FlashMetaPayload");      break;
      case _DC3FlashDataPayloadMsg:    return("FlashDataPayload");      break;
      case _DC3I2CReadMsg:             return("I2CRead");               break;
      case _DC3I2CWriteMsg:            return("I2CWrite");              break;
      case _DC3I2CDataPayloadMsg:      return("I2CDataPayload");        break;
      case _DC3RamTestMsg:             return("RamTest");               break;
      case _DC3RamTestPayloadMsg:      return("RamTestPayload");        break;
      case _DC3DbgEnableEthMsg:        return("DbgEnableEth");          break;
      case _DC3DbgDisableEthMsg:       return("DbgDisableEth");         break;
      case _DC3DbgEnableSerMsg:        return("DbgEnableSer");          break;
      case _DC3DbgDisableSerMsg:       return("DbgDisableSer");         break;
      case _DC3DbgRstDefaultMsg:       return("DbgRstDefault");         break;
      case _DC3DbgEnableMsg:           return("DbgEnable");             break;
      case _DC3DbgDisableMsg:          return("DbgDisable");            break;
      case _DC3DbgGetCurrentMsg:       return("DbgGetCurrent");         break;
      case _DC3DbgSetCurrentMsg:       return("DbgSetCurrent");         break;
      case _DC3DbgPayloadMsg:          return("DbgPayload");            break;
      case _DC3DBFullResetMsg:         return("DBFullReset");           break;
      case _DC3DBGetElemMsg:           return("DBGetElem");             break;
      case _DC3DBSetElemMsg:           return("DBSetElem");             break;
      case _DC3DBDataPayloadMsg:       return("DBDataPayload");         break;

      /* Add more message name translations here*/
      default:                         return(invalidStr);              break;
   }
}

/******************************************************************************/
const char* const CON_i2cDevToStr( const DC3I2CDevice_t iDev )
{
   switch ( iDev ) {
      case _DC3_EEPROM: return("EEPROM");    break;
      case _DC3_SNROM:  return("SN_ROM");    break;
      case _DC3_EUIROM: return("EUI_ROM");   break;
      default:          return(invalidStr);  break;
   }
}

/******************************************************************************/
const char* const CON_i2cOpToStr( const I2C_Operation_t iOp )
{
   switch ( iOp ) {
      case I2C_OP_MEM_READ:   return("I2C_MEM_READ");    break;
      case I2C_OP_MEM_WRITE:  return("I2C_MEM_WRITE");   break;
      case I2C_OP_REG_READ:   return("I2C_REG_READ");    break;
      case I2C_OP_REG_WRITE:  return("I2C_REG_WRITE");   break;
      default:                return(invalidStr);        break;
   }
}

/******************************************************************************/
const char* const CON_i2cBusToStr( const I2C_Bus_t iBus )
{
   switch ( iBus ) {
      case I2CBus1:  return("I2CBus1");   break;
      default:       return(invalidStr);  break;
   }
}

/******************************************************************************/
const char* const CON_dbgLvlToStr( const DC3DbgLevel_t lvl )
{
   switch ( lvl ) {
      case _DC3_DBG: return("DBG"); break;
      case _DC3_LOG: return("LOG"); break;
      case _DC3_WRN: return("WRN"); break;
      case _DC3_ERR: return("ERR"); break;
      case _DC3_ISR: return("ISR"); break;
      case _DC3_CON:                            /* Intentionally fall through */
      default:       return("CON");  break;
   }
}

/******************************************************************************/
const char* const CON_dbOpToStr( const DB_Operation_t op )
{
   switch ( op ) {

      case DB_OP_READ:     return("DB_OP_READ");      break;
      case DB_OP_WRITE:    return("DB_OP_WRITE");     break;
      case DB_OP_INTERNAL: return("DB_OP_INTERNAL");  break;
      case DB_OP_MAX:                           /* Intentionally fall through */
      case DB_OP_NONE:                          /* Intentionally fall through */
      default:             return("Invalid");         break;
   }
}

/**
 * @} end group groupConOut
 */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
