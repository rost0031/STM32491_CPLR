/**
 * @file		db.h
 * @brief   A database of settings that works on any memory type.
 *
 * This is an implementation of a database of settings that will be stored by
 * the system.  The intention is to store these settings in the I2C EEPROM, this
 * implementation abstracts away the ability to locate and size the different
 * elements in the different sections of the EEPROM as well as other locations.
 * (See Note). It also provides interfaces to allow DB access using blocking
 * (bare metal) and non-blocking (QP event based).
 *
 * The database consists of a structure that maps all the different types to a
 * memory location in memory and allows non-linear access to read/update the
 * values without manually calculating offsets and sizes.
 *
 * @note: While most of the settings are stored in the main section of the
 * EEPROM, there are a few settings that reside in completely separate RO
 * sections of the EEPROM, such as the UID and SN.  Other settings may be GPIO
 * controlled, such as the board position in the barrel and the processor
 * position on the coupler board.
 *
 * @date 	Feb 24, 2015
 * @author	Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard.  All rights reserved. 
 *
 * @addtogroup groupSettings
 * @{
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DB_H_
#define DB_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Shared.h"
#include "i2c_defs.h"                                /* for I2C functionality */
#include "CBCommApi.h"

/* Exported defines ----------------------------------------------------------*/
#define MAX_DB_ELEM_SIZE   64               /**< Max size of an element in DB */

 /**< Magic word that will be stored at the head of the DB.  If this is not
 * there, the DB needs to be initialized to a default and updated after. */
#define DB_MAGIC_WORD_DEF   0xdefec8db

/**< Current version of the DB.  This needs to be bumped once the FW is out in
 * the field and any changes are made to the DB. This is to allow for a smooth
 * upgrade of the DB. */
#define DB_VERSION_DEF      0x0001

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
* I2C Operations available on the system.
*/
typedef enum DB_Operations {
   DB_OP_NONE  = 0,                                 /**< No current operation */
   DB_OP_READ,                                  /**< Reading from settings DB */
   DB_OP_WRITE,                                   /**< Writing to settings DB */
   DB_OP_INTERNAL,                        /**< Internal only setting used for DB
                                              validation. No response is sent */
   /* Insert more I2C operations here... */
   DB_OP_MAX
} DB_Operation_t;

/**
 * DB elements that can be retrieved.
 */
typedef enum {
   DB_EEPROM = 0,           /**< Setting is located in the main EEPROM memory */
   DB_SN_ROM,         /**< Setting is located in the RO SNR section of EEPROM */
   DB_UI_ROM,        /**< Setting is located in the RO UI64 section of EEPROM */

   /* .. insert more I2C devices before GPIO so they are all together .. */

   DB_GPIO,                          /**< Setting is specified via a GPIO pin */
   DB_FLASH                  /**< Setting is located in the main FLASH memory */
} DB_ElemLoc_t;

/**
 * Description type of each DB element (location, etc).
 */
typedef struct {
   DB_Elem_t    elem;  /**< Specifies which settings element is being described */
   DB_ElemLoc_t  loc;  /**< Specifies how the db element is stored */
   size_t       size;  /**< Specifies the size of the db element */
   uint32_t    offset; /**< Specifies the offset from beginning of EEPROM memory
                            where the element is stored */
} SettingsDB_Desc_t;

/**
 * DB element structure mapping that resides in the main memory of the EEPROM
 */
typedef struct {
   uint32_t dbMagicWord; /**< Magic word that specifies whether a DB exists.  If
                              this is not present, we have to initialize the DB
                              in EEPROM memory to some default.*/
   uint16_t dbVersion;   /**< Version of the database */
   uint8_t ipAddr[4];  /**< 4 values of the 111.222.333.444 ip address in hex */
   uint8_t bootMajVer; /**< Major version byte of the bootloader FW image */
   uint8_t bootMinVer; /**< Major version byte of the bootloader FW image */
   uint8_t bootBuildDT[CB_DATETIME_LEN]; /**< Build datetime of the bootloader
                                              FW image */
   uint8_t fpgaMajVer; /**< Major version byte of the FPGA FW image */
   uint8_t fpgaMinVer; /**< Major version byte of the FPGA FW image */
   uint8_t fpgaBuildDT[CB_DATETIME_LEN]; /**< Build datetime of the FPGA FW
                                              image */
} SettingsDB_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief   Get a string representation of a DB element.
 *
 * @param  [in] elem: DB_Elem_t that specifies what element to retrieve.
 *    @arg DB_MAGIC_WORD: only used to validate that the DB even exists.
 *    @arg DB_VERSION: version of the DB.  To be used for future upgrades.
 *    @arg DB_MAC_ADDR: MAC address stored in the RO part of DB.
 *    @arg DB_IP_ADDR: IP address stored in the RW part of DB.
 *    @arg DB_SN: Serial number stored in the RO part of DB.
 * @return str: char* representation of DB element if found,
 *             "" if not found.
 */
const char* const DB_elemToStr( const DB_Elem_t elem );

/**
 * @brief   Check if Settings DB in EEPROM is valid.
 *
 * This function checks for a magic number at the beginning of the EEPROM and
 * the version to see if they match what's expected.
 *
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
const CBErrorCode DB_isValid( const AccessType_t accessType );

/**
 * @brief   Initialize the settings DB in EEPROM to a stored default.
 *
 * This function initializes the settings DB in EEPROM to a stored default.
 * This should only happen if it is found to be invalid by DB_isValid().
 *
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
const CBErrorCode DB_initToDefault( const AccessType_t accessType  );

/**
 * @brief   Get an element from the settings DB.
 *
 * This function retrieves an element from the database of settings.
 *
 * @param  [in] elem: DB_Elem_t that specifies what element to retrieve.
 *    @arg DB_MAGIC_WORD: only used to validate that the DB even exists.
 *    @arg DB_VERSION: version of the DB.  To be used for future upgrades.
 *    @arg DB_MAC_ADDR: MAC address stored in the RO part of DB.
 *    @arg DB_IP_ADDR: IP address stored in the RW part of DB.
 *    @arg DB_SN: Serial number stored in the RO part of DB.
 *
 * @param  [out] *pBuffer: uint8_t pointer to a buffer where to store the
 *                         retrieved element.
 * @param  [in] bufSize: size of the pBuffer.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
const CBErrorCode DB_getElemBLK(
      const DB_Elem_t elem,
      uint8_t* pBuffer,
      const size_t bufSize,
      const AccessType_t accessType
);

/**
 * @brief   Set an element in the settings DB.
 *
 * This function sets an element to the database of settings.
 *
 * @note: Some elements in the DB cannot be set since they are in RO part of the
 * DB (RO EEPROM, GPIO, Flash, etc).
 *
 * @param  [in] elem: DB_Elem_t that specifies what element to set.
 *    @arg DB_MAGIC_WORD: only used to validate that the DB even exists.
 *    @arg DB_VERSION: version of the DB.  To be used for future upgrades.
 *    @arg DB_IP_ADDR: IP address stored in the RW part of DB.
 *
 * @param  [in] *pBuffer: uint8_t pointer to a buffer where to element to be set
 *                         is.
 * @param  [in] bufSize: size of the pBuffer.
 * @param  [in] accessType: AccessType_t that specifies how the function is being
 * accessed.
 *    @arg ACCESS_BARE_METAL: blocking access that is slow.  Don't use once the
 *                            RTOS is running.
 *    @arg ACCESS_QPC:        non-blocking, event based access.
 *    @arg ACCESS_FREERTOS:   non-blocking, but waits on queue to know the status.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
const CBErrorCode DB_setElemBLK(
      const DB_Elem_t elem,
      const uint8_t* const pBuffer,
      const size_t bufSize,
      const AccessType_t accessType
);

/**
 * @brief   Get the DB element location in the DB
 *
 * @param [in] elem: DB_Elem_t that specifies the DB element
 * @return DB_ElemLoc_t:
 *    @arg DB_EEPROM: element lives in the R/W EEPROM
 *    @arg DB_SN_ROM: element lives in the R/O SNR section of EEPROM
 *    @arg DB_UI_ROM: element lives in the R/O UI64 section of EEPROM
 *    @arg DB_GPIO:   element is specified via a GPIO pin
 *    @arg DB_FLASH:  element lives in the main FLASH memory
 */
const DB_ElemLoc_t DB_getElemLoc( const DB_Elem_t elem );

/**
 * @brief   Get the DB element offset from the beginnging of its device
 *
 * @param [in] elem: DB_Elem_t that specifies the DB element
 * @return uint32_t: offset from beginning of the element's device
 */
const uint32_t DB_getElemOffset( const DB_Elem_t elem );

/**
 * @brief   Get the DB element size in DB
 *
 * @param [in] elem: DB_Elem_t that specifies the DB element
 * @return size_t: size of the element in DB
 */
const size_t DB_getElemSize( const DB_Elem_t elem );

/**
 * @brief   Get the specific I2C device on which a specified DB element is
 *          stored.
 *
 * @note:   This should only be used only after checking if the DB element lives
 *          on I2C and not anywhere else.
 *
 * @param [in] loc: DB_ElemLoc_t that specifies location where the DB element
 * lives.
 * @return I2C_Dev_t:
 *    @arg EEPROM: eeprom device
 *    @arg SN_ROM: R/O SN eeprom section
 *    @arg EUI_ROM: R/O EUI eeprom section
 */
const I2C_Dev_t DB_getI2CDev( const DB_ElemLoc_t loc );

/**
 * @brief   Get an element stored in the FLASH section of settings DB.
 *
 * This function retrieves an element from the database portion of DB that is
 * stored exclusively in FLASH (and is read only).
 *
 * @param  [in] elem: DB_Elem_t that specifies what element to retrieve.
 *    @arg DB_APPL_MAJ_VER: Major version of the Application FW image.
 *    @arg DB_APPL_MIN_VER: Minor version of the Application FW image.
 *    @arg DB_APPL_BUILD_DATETIME: Build datetime of the application FW image.
 *
 * @param  [in] bufSize: size of the pBuffer.
 * @param  [out] *pBuffer: uint8_t pointer to a buffer where to store the
 *                         retrieved element.
 * @param  [out] resultLen: uint8_t pointer to the length of data stored in the
 *                         buffer on return.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    other errors if found.
 */
const CBErrorCode DB_readEEPROM(
      const DB_Elem_t elem,
      const uint8_t bufferSize,
      uint8_t* const buffer,
      uint8_t* resultLen
);

/**
 * @brief   Checks if the passed in db magic word matches the compiled in one.
 *
 * This function simply checks if the compiled in DB magic word matches the one
 * passed in as a parameter.
 * @param  [in] dbMagicWord: uint32_t magic number
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    @arg ERR_DB_NOT_INIT: if there is a mismatch
 */
const CBErrorCode DB_isMagicWordValid( const uint32_t dbMagicWord );

/**
 * @brief   Checks if the passed in db version matches the compiled in one.
 *
 * This function simply checks if the compiled in DB version matches the one
 * passed in as a parameter.

 * @param  [in] dbMagicWord: uint32_t magic number
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if no errors occurred
 *    @arg ERR_DB_NOT_INIT: if there is a mismatch
 */
const CBErrorCode DB_isVersionValid( const uint16_t dbVersion );

/**
 * @brief   Checks if the passed in db version matches the compiled in one.
 *
 * This function simply checks if the compiled in DB version matches the one
 * passed in as a parameter.
 * @param  [in] dt1: uint8_t pointer to array of CB_DATETIME_LEN length that
 * contains the first datetime string.
 * @param  [in] dt2: uint8_t pointer to array of CB_DATETIME_LEN length that
 * contains the second datetime string.
 * @return CBErrorCode: status of the read operation
 *    @arg ERR_NONE: if exact match
 *    @other errors if differences found
 */
const CBErrorCode DB_checkDTMatch(
      const uint8_t* const dt1,
      const uint8_t* const dt2
);

/**
 * @brief   Checks 2 arrays of same length to see if their contents match.
 *
 * This function simply checks if 2 arrays of the same length have the same
 * contents by iterating backwards through them. The reason for iterating
 * backwards is because most checks will involve changes in the LSByte so this
 * should be faster for most cases.  For example, in datetime array,
 * YYYYMMDDhhmmss, the seconds and minutes of a build are more likely to differ
 * than the year and the month and the check will finish faster.  In others, it
 * doesn't matter so the worst case is still the same.
 *
 * @param  [in] dt1: uint8_t pointer to array 1
 * @param  [in] dt2: uint8_t pointer to array 2
 * @param  [in] len: uint16_t length of both arrays.
 *    @note: if the lengths of both arrays are not the same, the results will
 *    probably be incorrect.
 *
 * @return boo:
 *    @arg true: if exact match
 *    @arg false: if differences found
 */
const bool DB_isArraysMatch(
      const uint8_t*  dt1,
      const uint8_t*  dt2,
      const uint16_t  len
);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * end addtogroup groupSettings
 */
#endif                                                               /* DB_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
