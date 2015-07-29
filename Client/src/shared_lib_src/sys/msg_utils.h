/**
 * @file    msg_utils.h
 * Utilities that are helpful for dealing with msgs and buffers of hex data.
 *
 * @date    04/21/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MSG_UTILS_H_
#define MSG_UTILS_H_

/* Includes ------------------------------------------------------------------*/
#include "ApiShared.h"


/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Event type for transferring large data.
 */
typedef struct MsgEvtTag {
/* protected: */
    DC3MsgRoute_t src;                                   /**< Source of the data */
    DC3MsgRoute_t dst;                              /**< Destination of the data */
    uint16_t   dataLen;                    /**< Length of the data in dataBuf */
    uint8_t    dataBuf[DC3_MAX_MSG_LEN];       /**< Buffer that holds the data */
} MsgData_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief   Convert a uint8_t hex array to a char array.
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
 * @return APIError_t: API_ERR_NONE if OK.
 */
APIError_t MSG_hexToStr(
      const uint8_t* hexData,
      uint16_t hexDataLen,
      char* strDataBuffer,
      uint16_t strDataBufferSize,
      uint16_t* strDataLen,
      uint8_t outputNColumns,
      const char sep,
      const bool bPrintX
);

/* Exported classes ----------------------------------------------------------*/


#endif                                                        /* MSG_UTILS_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
