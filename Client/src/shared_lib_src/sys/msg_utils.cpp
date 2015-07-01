/**
 * @file    msg_utils.cpp
 * Utilities that are helpful for dealing with msgs and buffers of hex data.
 *
 * @date    04/21/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */
/* Includes ------------------------------------------------------------------*/
#include "msg_utils.h"
#include "LogHelper.h"
#include <cstring>
#include "LogStub.h"
/* Namespaces ----------------------------------------------------------------*/

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_MSG );
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/******************************************************************************/
APIError_t MSG_hexToStr(
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
   APIError_t status = API_ERR_NONE;

   if ( NULL == hexData ) {
      return( API_ERR_MEM_NULL_VALUE );
   }

   if ( 0 == hexDataLen ) {
      return( API_ERR_MEM_BUFFER_LEN );
   }

   if ( 0 == strDataBufferSize ) {
      return( API_ERR_MEM_BUFFER_LEN );
   } else if ( bPrintX == true && strDataBufferSize < 5 * hexDataLen ) {
      status = API_ERR_MEM_BUFFER_LEN;
   } else if ( bPrintX == false && strDataBufferSize < 3 * hexDataLen ) {
      status = API_ERR_MEM_BUFFER_LEN;
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
         return( API_ERR_MEM_BUFFER_LEN );
      }
   }
   return( status );

}


/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
