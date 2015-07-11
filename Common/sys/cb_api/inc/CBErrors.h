/**
 * @file 	CBErrors.h
 * @brief   Contains all the error codes that can be generated by the board
 *
 * @date   	05/28/2014
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupApp
 * @{
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBERRORS_H_
#define CBERRORS_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @enum Error codes that can be generated by this board.
 *
 * The error code consists of a 32 bit value:
 *
 * The most significant 16 bits map to the category.
 * The least significant 16 bits map to low level errors as you see fit.
 *
 * For example:
 * Error code - 0x00000004
 * Category   - 0x0000: This is the serial communications category
 * Error      - 0x0004: This is the low level error code that the board can
 * specify.  Each category gets 0xFFFF error codes (65535) dedicated to it.
 *
 */
typedef enum CBErrors
{
   ERR_NONE                                                    = 0x00000000,

   /* General HW error category.                 0x00000001 - 0x0000FFFF */
   ERR_SERIAL_HW_TIMEOUT                                       = 0x00000001,
   ERR_SERIAL_MSG_TOO_LONG                                     = 0x00000002,
   ERR_SERIAL_MSG_BASE64_ENC_FAILED                            = 0x00000003,
   ERR_STM32_HW_CRYPTO_FAILED                                  = 0x00000004,
   ERR_LOW_POWER_RESET                                         = 0x00000005,
   ERR_WINDOW_WATCHDOG_RESET                                   = 0x00000006,
   ERR_INDEPENDENT_WATCHDOG_RESET                              = 0x00000007,
   ERR_SOFTWARE_RESET                                          = 0x00000008,
   ERR_POR_PDR_RESET                                           = 0x00000009,
   ERR_PIN_RESET                                               = 0x0000000A,
   ERR_BOR_RESET                                               = 0x0000000B,
   ERR_SDRAM_DATA_BUS                                          = 0x0000000C,
   ERR_SDRAM_ADDR_BUS                                          = 0x0000000D,
   ERR_SDRAM_DEVICE_INTEGRITY                                  = 0x0000000E,

   /* Flash error category                       0x00010000 - 0x0001FFFF */
   ERR_FLASH_IMAGE_SIZE_INVALID                                = 0x00010000,
   ERR_FLASH_IMAGE_TYPE_INVALID                                = 0x00010001,
   ERR_FLASH_BUSY                                              = 0x00010002,
   ERR_FLASH_READ                                              = 0x00010003,
   ERR_FLASH_PGS                                               = 0x00010004,
   ERR_FLASH_PGP                                               = 0x00010005,
   ERR_FLASH_PGA                                               = 0x00010006,
   ERR_FLASH_WRP                                               = 0x00010007,
   ERR_FLASH_PROGRAM                                           = 0x00010008,
   ERR_FLASH_OPERATION                                         = 0x00010009,
   ERR_FLASH_READ_VERIFY_FAILED                                = 0x0001000A,
   ERR_FLASH_WRITE_INCOMPLETE                                  = 0x0001000B,
   ERR_FLASH_SECTOR_ADDR_NOT_FOUND                             = 0x0001000C,
   ERR_FLASH_ERASE_TIMEOUT                                     = 0x0001000D,
   ERR_FLASH_INVALID_IMAGE_CRC                                 = 0x0001000E,
   ERR_FLASH_INVALID_DATETIME_LEN                              = 0x0001000F,
   ERR_FLASH_INVALID_DATETIME                                  = 0x00010010,
   ERR_FLASH_WAIT_FOR_DATA_TIMEOUT                             = 0x00010011,
   ERR_FLASH_INVALID_FW_PACKET_CRC                             = 0x00010012,
   ERR_FLASH_WRITE_TIMEOUT                                     = 0x00010013,
   ERR_FLASH_INVALID_IMAGE_CRC_AFTER_FLASH                     = 0x00010014,
   ERR_SDRAM_DATA_BUS_TEST_TIMEOUT                             = 0x00010015,
   ERR_SDRAM_ADDR_BUS_TEST_TIMEOUT                             = 0x00010016,
   ERR_SDRAM_DEVICE_INTEGRITY_TEST_TIMEOUT                     = 0x00010017,

   /* NOR error category                         0x00030000 - 0x0003FFFF */
   ERR_NOR_ERROR                                               = 0x00030000,
   ERR_NOR_TIMEOUT                                             = 0x00030001,
   ERR_NOR_BUSY                                                = 0x00030002,

   /* CommMgr error category                     0x00040000 - 0x0004FFFF */
   ERR_COMM_UNKNOWN_MSG_SOURCE                                 = 0x00040000,
   ERR_COMM_INVALID_MSG_LEN                                    = 0x00040001,
   ERR_COMM_UNIMPLEMENTED_MSG                                  = 0x00040002,
   ERR_COMM_FLASHMGR_TIMEOUT                                   = 0x00040003,
   ERR_COMM_INVALID_APPL_CRC                                   = 0x00040004,
   ERR_COMM_INVALID_APPL_SIZE                                  = 0x00040005,
   ERR_COMM_INVALID_APPL_CRC_MISMATCH                          = 0x00040006,
   ERR_COMM_INVALID_BOOTMODE_REQUESTED                         = 0x00040007,
   ERR_COMM_I2C_READ_CMD_TIMEOUT                               = 0x00040008,

   /* Application CommMgr error category          0x00050000 - 0x0005FFFF */
   ERR_MENU_NODE_STORAGE_ALLOC_NULL                            = 0x00050000,
   ERR_MENU_TEXT_STORAGE_ALLOC_NULL                            = 0x00050001,
   ERR_MENU_UNKNOWN_CMD                                        = 0x00050002,
   ERR_MENU_CURRENT_NODE_NULL                                  = 0x00050003,
   ERR_MENU_CMD_NOT_FOUND_AT_THIS_MENU                         = 0x00050004,

   /* I2CBus error category                      0x00060000 - 0x0006FFFF */
   ERR_I2CBUS_BUSY                                             = 0x00060000,
   ERR_I2CBUS_RCVRY_SDA_STUCK_LOW                              = 0x00060001,
   ERR_I2CBUS_RCVRY_EV5_NOT_REC                                = 0x00060002,
   ERR_I2CBUS_RCVRY_EV6_NOT_REC                                = 0x00060003,
   ERR_I2CBUS_EV5_TIMEOUT                                      = 0x00060004,
   ERR_I2CBUS_EV5_NOT_REC                                      = 0x00060005,
   ERR_I2CBUS_INVALID_PARAMS_FOR_7BIT_ADDR                     = 0x00060006,
   ERR_I2CBUS_INVALID_PARAMS_FOR_SEND_DATA                     = 0x00060007,
   ERR_I2CBUS_EV6_TIMEOUT                                      = 0x00060008,
   ERR_I2CBUS_EV6_NOT_REC                                      = 0x00060009,
   ERR_I2CBUS_EV8_TIMEOUT                                      = 0x0006000A,
   ERR_I2CBUS_EV8_NOT_REC                                      = 0x0006000B,
   ERR_I2CBUS_INVALID_PARAMS_FOR_BUS_CHECK_FREE                = 0x0006000C,
   ERR_I2CBUS_RXNE_FLAG_TIMEOUT                                = 0x0006000D,
   ERR_I2CBUS_STOP_BIT_TIMEOUT                                 = 0x0006000E,
   ERR_I2CBUS_WRITE_BYTE_TIMEOUT                               = 0x0006000F,

   /* I2C1Dev error category                     0x00070000 - 0x0007FFFF */
   ERR_I2C1DEV_CHECK_BUS_TIMEOUT                               = 0x00070000,
   ERR_I2C1DEV_EV5_TIMEOUT                                     = 0x00070001,
   ERR_I2C1DEV_EV6_TIMEOUT                                     = 0x00070002,
   ERR_I2C1DEV_EV8_TIMEOUT                                     = 0x00070003,
   ERR_I2C1DEV_READ_MEM_TIMEOUT                                = 0x00070004,
   ERR_I2C1DEV_WRITE_MEM_TIMEOUT                               = 0x00070005,
   ERR_I2C1DEV_READ_REG_TIMEOUT                                = 0x00070006,
   ERR_I2C1DEV_WRITE_REG_TIMEOUT                               = 0x00070007,
   ERR_I2C1DEV_ACK_DIS_TIMEOUT                                 = 0x00070008,
   ERR_I2C1DEV_ACK_EN_TIMEOUT                                  = 0x00070009,
   ERR_I2C1DEV_MEM_OUT_BOUNDS                                  = 0x0007000A,
   ERR_I2C1DEV_0_BYTE_REQUEST                                  = 0x0007000B,
   ERR_I2C1DEV_OVERFLOW_REQUEST                                = 0x0007000C,
   ERR_I2C1DEV_INVALID_DEVICE                                  = 0x0007000D,
   ERR_I2C1DEV_READ_ONLY_DEVICE                                = 0x0007000E,
   ERR_I2C1DEV_ACCESS_OVER_END_OF_DEVICE_MEM                   = 0x0007000F,
   ERR_I2C1DEV_INVALID_OPERATION                               = 0x00070010,


   /* Settings Database error category           0x00080000 - 0x0008FFFF */
   ERR_DB_NOT_INIT                                             = 0x00080000,
   ERR_DB_VER_MISMATCH                                         = 0x00080001,
   ERR_DB_ELEM_NOT_FOUND                                       = 0x00080002,
   ERR_DB_ELEM_IS_READ_ONLY                                    = 0x00080003,
   ERR_DB_ELEM_IS_NOT_IN_EEPROM                                = 0x00080004,
   ERR_DB_ACCESS_TIMEOUT                                       = 0x00080005,
   ERR_DB_ELEM_SIZE_OVERFLOW                                   = 0x00080006,
   ERR_DB_INVALID_DATETIME_LENGTH                              = 0x00080007,
   ERR_DB_INVALID_MAGIC_WORD_LENGTH                            = 0x00080008,
   ERR_DB_DATETIME_MISMATCH                                    = 0x00080009,
   ERR_DB_INVALID_MAJ_VER_LENGTH                               = 0x0008000A,
   ERR_DB_INVALID_MIN_VER_LENGTH                               = 0x0008000B,
   ERR_DB_INVALID_MAJ_VER_MISMATCH                             = 0x0008000C,
   ERR_DB_INVALID_MIN_VER_MISMATCH                             = 0x0008000D,

   /* I2C Device general error category          0x00090000 - 0x0009FFFF */
   ERR_I2C_DEV_INVALID_DEVICE                                  = 0x00090000,
   ERR_I2C_DEV_EEPROM_MEM_ADDR_BOUNDARY                        = 0x00090001,
   ERR_I2C_DEV_IS_READ_ONLY                                    = 0x00090002,

   /* Msg device error category                  0x000A0000 - 0x000AFFFF */
   ERR_MSG_ROUTE_INVALID                                       = 0x000A0000,
   ERR_MSG_UNKNOWN_BASIC                                       = 0x000A0001,
   ERR_MSG_UNEXPECTED_PAYLOAD                                  = 0x000A0002,
   ERR_MSG_UNSUPPORTED_IN_BOOTLOADER                           = 0x000A0003,
   ERR_MSG_UNSUPPORTED_IN_APPLICATION                          = 0x000A0004,

   /* Memory error category                      0x000B0000 - 0x000BFFFF */
   ERR_MEM_NULL_VALUE                                          = 0x000B0000,
   ERR_MEM_BUFFER_LEN                                          = 0x000B0001,

   /* Reserved errors                            0xFFFFFFFE - 0xFFFFFFFF */
   ERR_UNIMPLEMENTED                                           = 0xFFFFFFFE,
   ERR_UNKNOWN                                                 = 0xFFFFFFFF
} CBErrorCode;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupApp
 */
#endif                                                         /* CBERRORS_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
