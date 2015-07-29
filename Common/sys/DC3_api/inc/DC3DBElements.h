/**
 * @file     DC3DBElements.h
 *
 * Specifies the all the elements the settings DB can handle.
 *
 * @date       05/15/2015
 * @author     Harry Rostovtsev
 * @email      harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DC3DBELEMENTS_H_
#define DC3DBELEMENTS_H_

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * All the different settings that can exist in the settings database
 */
typedef enum DB_Elements {
   DB_MAGIC_WORD = 0,      /**< Magic word that specifies if a DB exists or
                                needs to be init to a default. */
   DB_VERSION,             /**< Keeps track of DB version to allow upgrades and
                                additions */
   DB_MAC_ADDR,            /**< Mac address stored in the special UID section of
                                the RO EEPROM */
   DB_IP_ADDR,             /**< IP address stored in main EEPROM */
   DB_SN,                  /**< Serial number stored in the special SN section
                                of RO EEPROM */
   DB_BOOT_MAJ,            /**< Major version of bootloader FW image */
   DB_BOOT_MIN,            /**< Minor version of bootloader FW image */
   DB_BOOT_BUILD_DATETIME, /**< Build datetime of the bootloader FW image */
   DB_APPL_MAJ,            /**< Major version of application FW image */
   DB_APPL_MIN,            /**< Minor version of application FW image */
   DB_APPL_BUILD_DATETIME, /**< Build datetime of the application FW image */
   DB_FPGA_MAJ,            /**< Major version of FPGA FW image */
   DB_FPGA_MIN,            /**< Minor version of FPGA FW image */
   DB_FPGA_BUILD_DATETIME, /**< Build datetime of the FPGA FW image */

   /* Add more elements here.  If adding elements after code is released, bump
    * the DB_VERSION up. */

   DB_MAX_ELEM   /**< Max number of elements that can be stored.  ALWAYS LAST */
} DB_Elem_t;


/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif                                                    /* DC3DBELEMENTS_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
