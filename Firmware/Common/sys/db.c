/**
 * @file		db.c
 * @brief   A simple generic database of settings that worked on any memory type.
 *
 * @date    Feb 24, 2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard.  All rights reserved. 
 *
 * @addtogroup groupSettings
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"
#include "dbg_cntrl.h"
#include <stddef.h>
#include "i2c_dev.h"                               /* for I2C device mappings */
#include "db.h"
#include "ipAndMac.h"                                  /* for default IP addr */
#include "flash.h"
#include <stdio.h>
#include "I2C1DevMgr.h"    /* For access to the I2C1Dev AO for posting events */
#include "SysMgr.h"                           /* For access to DB event types */
#include "version.h"                        /* For access to default versions */

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DC3_DBG_MODL_DB );  /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/**< Macro to get size of the element stored in the EEPROM memory */
#define DB_SIZE_OF_ELEM(s,m)     ((size_t) sizeof(((s *)0)->m))

/**< Macro to get the offset of the element stored in the EEPROM memory */
#define DB_LOC_OF_ELEM(s,m)      offsetof(s, m)
/* Private variables and Local objects ---------------------------------------*/

/**< Array to specify where all the DB elements reside */
static const SettingsDB_Desc_t settingsDB[_DC3_DB_MAX_ELEM] = {
      { _DC3_DB_MAGIC_WORD,          DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, dbMagicWord), DB_LOC_OF_ELEM(SettingsDB_t, dbMagicWord)},
      { _DC3_DB_VERSION,             DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, dbVersion)  , DB_LOC_OF_ELEM(SettingsDB_t, dbVersion)  },
      { _DC3_DB_MAC_ADDR,            DB_UI_ROM,  6                                         , 2                                        },
      { _DC3_DB_IP_ADDR,             DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, ipAddr)     , DB_LOC_OF_ELEM(SettingsDB_t, ipAddr)     },
      { _DC3_DB_SN,                  DB_SN_ROM,  16                                        , 0                                        },
      { _DC3_DB_BOOT_MAJ,            DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, bootMajVer) , DB_LOC_OF_ELEM(SettingsDB_t, bootMajVer) },
      { _DC3_DB_BOOT_MIN,            DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, bootMinVer) , DB_LOC_OF_ELEM(SettingsDB_t, bootMinVer) },
      { _DC3_DB_BOOT_BUILD_DATETIME, DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, bootBuildDT), DB_LOC_OF_ELEM(SettingsDB_t, bootBuildDT)},
      { _DC3_DB_APPL_MAJ,            DB_FLASH ,  FLASH_APPL_MAJ_VER_LEN                    , FLASH_APPL_MAJ_VER_ADDR                  },
      { _DC3_DB_APPL_MIN,            DB_FLASH ,  FLASH_APPL_MIN_VER_LEN                    , FLASH_APPL_MIN_VER_ADDR                  },
      { _DC3_DB_APPL_BUILD_DATETIME, DB_FLASH ,  FLASH_APPL_BUILD_DATETIME_LEN             , FLASH_APPL_BUILD_DATETIME_ADDR           },
      { _DC3_DB_FPGA_MAJ,            DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, fpgaMajVer) , DB_LOC_OF_ELEM(SettingsDB_t, fpgaMajVer) },
      { _DC3_DB_FPGA_MIN,            DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, fpgaMinVer) , DB_LOC_OF_ELEM(SettingsDB_t, fpgaMinVer) },
      { _DC3_DB_FPGA_BUILD_DATETIME, DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, fpgaBuildDT), DB_LOC_OF_ELEM(SettingsDB_t, fpgaBuildDT)},
      { _DC3_DB_DBG_MODULES,         DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, dbgModules) , DB_LOC_OF_ELEM(SettingsDB_t, dbgModules) },
      { _DC3_DB_DBG_DEVICES,         DB_EEPROM,  DB_SIZE_OF_ELEM(SettingsDB_t, dbgDevices) , DB_LOC_OF_ELEM(SettingsDB_t, dbgDevices) },
};

/**
 * Array to lookup the I2C device for the DB elements which are stored on some
 * I2C device
 */
static const DC3I2CDevice_t DB_I2C_devices[] = {
      _DC3_EEPROM,
      _DC3_SNROM,
      _DC3_EUIROM,
};

/**< Default settings that belong in the main memory of the EEPROM */
static const SettingsDB_t DB_defaultEepromSettings = {
      .dbMagicWord = DB_MAGIC_WORD_DEF,
      .dbVersion   = DB_VERSION_DEF,
      .ipAddr = {STATIC_IPADDR0, STATIC_IPADDR1, STATIC_IPADDR2, STATIC_IPADDR3},
#if CPLR_BOOT
      .bootMajVer = FW_VER_MAJOR,
      .bootMinVer = FW_VER_MINOR,
      .bootBuildDT = BUILD_DATE,
#else
      .bootMajVer = 0,
      .bootMinVer = 0,
      .bootBuildDT = {0},
#endif
      .fpgaMajVer = 0,
      .fpgaMinVer = 0,
      .fpgaBuildDT = {0},
      .dbgModules = DB_DBG_MODULES_DEF,
      .dbgDevices = DB_DBG_DEVICES_DEF
};

/* Private function prototypes -----------------------------------------------*/
const DC3Error_t DB_initToDefault( const DC3AccessType_t accessType );
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
const DC3Error_t DB_isValid( const DC3AccessType_t accessType )
{
   DC3Error_t status = ERR_NONE;            /* keep track of success/failure */

   uint32_t db_magicWord = 0;
   status = DB_read(
         _DC3_DB_MAGIC_WORD,
         _DC3_ACCESS_BARE,
         sizeof(db_magicWord),
         (uint8_t *)&db_magicWord
   );

   if ( ERR_NONE != status ) {
      goto DB_isValid_ERR_HANDLE;          /* Stop and jump to error handling */
   } else {
      if ( db_magicWord != DB_MAGIC_WORD_DEF ) {
         wrn_slow_printf(
               "Magic word in DB (0x%08x) doesn't match expected (0x%08x)\n",
               db_magicWord,
               DB_MAGIC_WORD_DEF
         );
         return( ERR_DB_NOT_INIT);
      }
   }

   uint16_t db_version = 0;
   status = DB_read(
         _DC3_DB_VERSION,
         _DC3_ACCESS_BARE,
         sizeof(db_version),
         (uint8_t *)&db_version
   );

   if ( ERR_NONE != status ) {
      goto DB_isValid_ERR_HANDLE;          /* Stop and jump to error handling */
   } else {
      if ( db_version != DB_VERSION_DEF ) {

         wrn_slow_printf(
               "DB version in EEPROM (0x%04x) doesn't match expected (0x%04x)\n",
               db_version,
               DB_VERSION_DEF
         );
         return( ERR_DB_VER_MISMATCH);
      }
   }

DB_isValid_ERR_HANDLE:            /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT( status, accessType,
         "Validating the settings DB: Error 0x%08x\n",
         status );
   return( status );
}

/******************************************************************************/
const DC3Error_t DB_initToDefault( const DC3AccessType_t accessType )
{
   DC3Error_t status = ERR_NONE;            /* keep track of success/failure */

   switch( accessType ) {
      case _DC3_ACCESS_BARE:
         status = DB_write(
               _DC3_DB_MAGIC_WORD,
               accessType,
               sizeof(DB_defaultEepromSettings.dbMagicWord),
               (uint8_t *)&DB_defaultEepromSettings.dbMagicWord
         );
         if ( ERR_NONE != status ) {
            goto DB_initToDefault_ERR_HANDLE;
         }

         status = DB_write(
               _DC3_DB_VERSION,
               accessType,
               sizeof(DB_defaultEepromSettings.dbVersion),
               (uint8_t *)&DB_defaultEepromSettings.dbVersion
         );
         if ( ERR_NONE != status ) {
            goto DB_initToDefault_ERR_HANDLE;
         }

         status = DB_write(
               _DC3_DB_IP_ADDR,
               accessType,
               sizeof(DB_defaultEepromSettings.ipAddr),
               (uint8_t *)&DB_defaultEepromSettings.ipAddr
         );
         if ( ERR_NONE != status ) {
            goto DB_initToDefault_ERR_HANDLE;
         }

         break;                               /* end of case _DC3_ACCESS_BARE */

      case _DC3_ACCESS_QPC:{;
         /* Create the event and directly post it to the right AO. */
         I2CWriteReqEvt *i2cWriteReqEvt  = Q_NEW(I2CWriteReqEvt, I2C1_DEV_RAW_MEM_WRITE_SIG);
         i2cWriteReqEvt->i2cDev          = DB_getI2CDev(_DC3_DB_MAGIC_WORD);
         i2cWriteReqEvt->start           = DB_getElemOffset(_DC3_DB_MAGIC_WORD);
         i2cWriteReqEvt->bytes           = sizeof(DB_defaultEepromSettings);
         i2cWriteReqEvt->accessType      = accessType;
         MEMCPY(i2cWriteReqEvt->dataBuf, &DB_defaultEepromSettings, sizeof(DB_defaultEepromSettings));
         QACTIVE_POST(AO_I2C1DevMgr, (QEvt *)(i2cWriteReqEvt), SysMgr_AO);
         goto DB_initToDefault_ERR_HANDLE; /* Stop and jump to error handling */
      }
      break;                                        /* end of case ACCESS_QPC */
      case  _DC3_ACCESS_FRT:
         status = ERR_UNIMPLEMENTED;
         goto DB_initToDefault_ERR_HANDLE; /* Stop and jump to error handling */
         break;                                /* end of case ACCESS_FREERTOS */

      default:
         status = ERR_UNIMPLEMENTED;
         goto DB_initToDefault_ERR_HANDLE; /* Stop and jump to error handling */
         break;                                        /* end of default case */

   }                                               /* End of switch statement */

DB_initToDefault_ERR_HANDLE:      /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT( status, accessType,
         "Initializing DB to default: Error 0x%08x\n",
         status );
   return( status );
}

/******************************************************************************/
const DC3Error_t DB_read(
      const DC3DBElem_t elem,
      const DC3AccessType_t accessType,
      const size_t bufSize,
      uint8_t* pBuffer
)
{
   DC3Error_t status = ERR_NONE;            /* keep track of success/failure */

   /* 1. Sanity checks of buffer sizes and memory allocations. Only check this
    * for bare metal access since QPC and FRT accesses will post events with
    * their own buffers */
   if ( _DC3_ACCESS_BARE == accessType ) {
      if ( bufSize < settingsDB[elem].size ) {
         status = ERR_MEM_BUFFER_LEN;
         goto DB_read_ERR_HANDLE;          /* Stop and jump to error handling */
      }

      if ( NULL == pBuffer ) {
         status = ERR_MEM_NULL_VALUE;
         goto DB_read_ERR_HANDLE;          /* Stop and jump to error handling */
      }
   }

   /* 2. Find where the element lives */
   DB_ElemLoc_t loc = settingsDB[elem].loc;

   /* 3. Call the location dependent functions to retrieve the data from DB */
   switch( loc ) {
      case DB_EEPROM:                           /* Intentionally fall through */
      case DB_SN_ROM:                           /* Intentionally fall through */
      case DB_UI_ROM:
         if ( _DC3_ACCESS_BARE == accessType ) {
            status = I2C_readDevMemBLK(
                  DB_I2C_devices[loc],          // DC3I2CDevice_t iDev,
                  settingsDB[elem].offset,      // uint16_t offset,
                  settingsDB[elem].size,        // uint16_t bytesToRead,
                  accessType,                   // DC3AccessType_t accType,
                  pBuffer,                      // uint8_t* pBuffer,
                  bufSize                       // uint8_t  bufSize
            );
         } else {
            /* Create the event and directly post it to the right AO. */
            I2CReadReqEvt *i2cReadReqEvt  = Q_NEW(I2CReadReqEvt, I2C1_DEV_RAW_MEM_READ_SIG);
            i2cReadReqEvt->i2cDev         = DB_getI2CDev(loc);
            i2cReadReqEvt->start          = settingsDB[elem].offset;
            i2cReadReqEvt->bytes          = settingsDB[elem].size;
            i2cReadReqEvt->accessType     = accessType;
            QACTIVE_POST(AO_I2C1DevMgr, (QEvt *)(i2cReadReqEvt), SysMgr_AO);
         }
         break;
      case DB_GPIO:
         status = ERR_UNIMPLEMENTED;
         goto DB_read_ERR_HANDLE;          /* Stop and jump to error handling */
         break;
      case DB_FLASH:
         ; // need this or we get "a label can only be part of a statement and
           // a declaration is not a statement" compile error
         uint16_t resultLen = 0;
         status = DB_readFlash(
               elem,
               accessType,
               bufSize,
               pBuffer,
               &resultLen
         );
         break;
         /* Add more locations here. Anything that fails will go to the default
          * case and get logged as an error. */
      default:
         status = ERR_DB_ELEM_NOT_FOUND;
         goto DB_read_ERR_HANDLE;          /* Stop and jump to error handling */
         break;
   }

DB_read_ERR_HANDLE:               /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT( status, accessType,
         "Getting element %s (%d) from DB: Error 0x%08x\n",
         CON_dbElemToStr( elem ), elem, status );
   return( status );
}

/******************************************************************************/
const DC3Error_t DB_write(
      const DC3DBElem_t elem,
      const DC3AccessType_t accessType,
      const size_t bufSize,
      const uint8_t* const pBuffer
)
{
   DC3Error_t status = ERR_NONE; /* keep track of success/failure of operations. */

   /* 1. Sanity checks of buffer sizes and memory allocations. */
   if ( bufSize < settingsDB[elem].size ) {
      status = ERR_MEM_BUFFER_LEN;
      goto DB_write_ERR_HANDLE;          /* Stop and jump to error handling */
   }

   if ( NULL == pBuffer ) {
      status = ERR_MEM_NULL_VALUE;
      goto DB_write_ERR_HANDLE;          /* Stop and jump to error handling */
   }

   /* 2. Find where the element lives */
   DB_ElemLoc_t loc = settingsDB[elem].loc;

   /* 3. Call the location dependent functions to write the data to DB */
   switch( loc ) {
      case DB_EEPROM:
         if ( _DC3_ACCESS_BARE == accessType ) {
            status = I2C_writeDevMemBLK(
                  DB_I2C_devices[loc],                // DC3I2CDevice_t iDev,
                  settingsDB[elem].offset,            // uint16_t offset,
                  settingsDB[elem].size,              // uint16_t bytesToWrite,
                  accessType,                         // DC3AccessType_t accType,
                  pBuffer,                            // uint8_t* pBuffer,
                  bufSize                             // uint8_t  bufSize
            );
         } else {
            /* Create the event and directly post it to the right AO. */
            I2CWriteReqEvt *i2cWriteReqEvt = Q_NEW(I2CWriteReqEvt, I2C1_DEV_RAW_MEM_WRITE_SIG);
            i2cWriteReqEvt->i2cDev         = DB_getI2CDev(loc);
            i2cWriteReqEvt->start          = settingsDB[elem].offset;
            i2cWriteReqEvt->bytes          = settingsDB[elem].size;
            i2cWriteReqEvt->accessType     = accessType;
            MEMCPY(i2cWriteReqEvt->dataBuf, pBuffer, i2cWriteReqEvt->bytes);
            QACTIVE_POST(AO_I2C1DevMgr, (QEvt *)(i2cWriteReqEvt), SysMgr_AO);
         }
         break;
      case DB_SN_ROM:                           /* Fall through intentionally */
      case DB_UI_ROM:                           /* Fall through intentionally */
      case DB_GPIO:                             /* Fall through intentionally */
      case DB_FLASH:                            /* Fall through intentionally */
         status = ERR_DB_ELEM_IS_READ_ONLY;
         goto DB_write_ERR_HANDLE;
         break;
         /* Add more locations here. Anything that fails will go to the default
          * case and get logged as an error. */
      default:
         status = ERR_DB_ELEM_NOT_FOUND;
         goto DB_write_ERR_HANDLE;
         break;
   }

DB_write_ERR_HANDLE:              /* Handle any error that may have occurred. */
   ERR_COND_OUTPUT( status, accessType,
         "Writing element %s (%d) to DB: Error 0x%08x\n",
         CON_dbElemToStr( elem ), elem, status );
   return( status );
}

/******************************************************************************/
const DC3Error_t DB_chkElem(
      const DC3DBElem_t elem,
      const DC3AccessType_t accessType,
      const size_t bufSize,
      const uint8_t* const pBuffer
)
{
   DC3Error_t status = ERR_NONE;

   if ( _DC3_ACCESS_BARE == accessType ) {
      // TODO: implement this later if needed
      status = ERR_UNIMPLEMENTED;
      goto DB_chkElem_ERR_HANDLE;
   }

   if( bufSize > MAX_DB_ELEM_SIZE ) {                              /* too big */
      /* This should be considered a pretty critical error. */
      status = ERR_DB_ELEM_SIZE_OVERFLOW;
      goto DB_chkElem_ERR_HANDLE;
   } else if( 0 == bufSize ) {                                   /* too small */
      /* This should be considered a pretty critical error. */
      status = ERR_DB_ELEM_SIZE_UNDERFLOW;
      goto DB_chkElem_ERR_HANDLE;
   }

   /* Using QP's native memory pool for "dynamic" memory allocation. This is
    * very fast CCM memory but you can't DMA to/from it.  Be careful using GOTOs
    * from here until the garbage collection happens. */
   uint8_t* dataBuf = (uint8_t *)QMPool_get(p_glbMemPool, 1U);
   if ( NULL == dataBuf ) {
      status = ERR_MEM_NULL_VALUE;
      goto DB_chkElem_ERR_HANDLE;
   }

   /* Get the DB element's default value */
   if (ERR_NONE != (status = DB_getEepromDefaultElem( elem, accessType,
         MAX_DB_ELEM_SIZE, dataBuf))) {
      /* Make sure to jump to the ERR_HANDLE that performs GC since we now have
       * an allocated buffer */
      goto DB_chkElemWithGC_ERR_HANDLE;
   }

   /* Compare the data passed in to the default value read */
   if( !DB_isArraysMatch( pBuffer, dataBuf, DB_getElemSize(elem) ) ) {
      WRN_printf("DB element %s (%d) value doesn't match default:\n",
            CON_dbElemToStr(elem), elem);
      DBG_printfHexStr(dataBuf, DB_getElemSize(elem),
            "Default Value (%d bytes):\n", DB_getElemSize(elem));
      DBG_printfHexStr(pBuffer, DB_getElemSize(elem),
            "Stored  Value (%d bytes):\n", DB_getElemSize(elem));

      /* Have to handle the elements explicitly here since the error assigned
       * might be used to reset or do other things with the data outside of this
       * function */
      switch( elem ) {
         case _DC3_DB_MAGIC_WORD:   status = ERR_DB_NOT_INIT;              break;
         case _DC3_DB_VERSION:      status = ERR_DB_VER_MISMATCH;          break;
         case _DC3_DB_IP_ADDR:      status = ERR_NONE;                     break;
#if CPLR_BOOT                               /* Only compile in the bootloader */
         case _DC3_DB_BOOT_MAJ:                 /* Intentionally fall through */
         case _DC3_DB_BOOT_MIN:                 /* Intentionally fall through */
         case _DC3_DB_BOOT_BUILD_DATETIME: status = ERR_DB_CHK_ELEM_NEEDS_RESET;  break;
#endif
         case _DC3_DB_FPGA_MAJ:                 /* Intentionally fall through */
         case _DC3_DB_FPGA_MIN:                 /* Intentionally fall through */
         case _DC3_DB_FPGA_BUILD_DATETIME: status = ERR_UNIMPLEMENTED;     break;
         case _DC3_DB_DBG_MODULES: status = ERR_DB_CHK_DBG_MODULES_MISMATCH; break;
         case _DC3_DB_DBG_DEVICES: status = ERR_DB_CHK_DBG_DEVICES_MISMATCH; break;
         default: status = ERR_DB_ELEM_NOT_FOUND; break;
      }
   }

DB_chkElemWithGC_ERR_HANDLE:      /* Error handle tag with garbage collection */
   /* Garbage collect the temp data buffer */
   QMPool_put(p_glbMemPool, dataBuf);

DB_chkElem_ERR_HANDLE:            /* Handle any error that may have occurred. */
   WRN_COND_OUTPUT( status, accessType,
         "Element mismatch checking element %s (%d) in DB against default: Error 0x%08x\n",
         CON_dbElemToStr( elem ), elem, status );
   return( status );
}

/******************************************************************************/
const DC3Error_t DB_getEepromDefaultElem(
      const DC3DBElem_t elem,
      const DC3AccessType_t accessType,
      const size_t bufSize,
      uint8_t* pBuffer
)
{
   DC3Error_t status = ERR_NONE;
   size_t elemSize = DB_getElemSize(elem);

   if ( bufSize < elemSize ) {
      status = ERR_MEM_BUFFER_LEN;
      goto DB_getEepromDefaultElem_ERR_HANDLE;
   }

   if ( NULL == pBuffer ) {
      status = ERR_MEM_NULL_VALUE;
      goto DB_getEepromDefaultElem_ERR_HANDLE;
   }

   switch( elem ) {
      case _DC3_DB_MAGIC_WORD:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.dbMagicWord,
               elemSize );
         break;
      case _DC3_DB_VERSION:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.dbVersion,
               elemSize );
         break;
      case _DC3_DB_IP_ADDR:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.ipAddr,
               elemSize );
         break;
      case _DC3_DB_BOOT_MAJ:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.bootMajVer,
               elemSize );
         break;
      case _DC3_DB_BOOT_MIN:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.bootMinVer,
               elemSize );
         break;
      case _DC3_DB_BOOT_BUILD_DATETIME:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.bootBuildDT,
               elemSize );
         break;
      case _DC3_DB_FPGA_MAJ:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.fpgaMajVer,
               elemSize );
         break;
      case _DC3_DB_FPGA_MIN:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.fpgaMinVer,
               elemSize );
         break;
      case _DC3_DB_FPGA_BUILD_DATETIME:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.fpgaBuildDT,
               elemSize );
         break;
      case _DC3_DB_DBG_MODULES:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.dbgModules,
               elemSize );
         break;
      case _DC3_DB_DBG_DEVICES:
         MEMCPY( pBuffer, (uint8_t *)&DB_defaultEepromSettings.dbgDevices,
               elemSize );
         break;
      default:
         status = ERR_DB_ELEM_NOT_FOUND;
         break;
   }

DB_getEepromDefaultElem_ERR_HANDLE:/* Handle any error that may have occurred */
   ERR_COND_OUTPUT( status, accessType,
         "Getting default value for DB element %s (%d): Error 0x%08x \n",
         CON_dbElemToStr( elem ), elem, status );
   return( status );
}

/******************************************************************************/
const DB_ElemLoc_t DB_getElemLoc( const DC3DBElem_t elem )
{
   return( settingsDB[elem].loc );
}

/******************************************************************************/
const uint32_t DB_getElemOffset( const DC3DBElem_t elem )
{
   return( settingsDB[elem].offset );
}

/******************************************************************************/
const size_t DB_getElemSize( const DC3DBElem_t elem )
{
   return( settingsDB[elem].size );
}

/******************************************************************************/
const DC3I2CDevice_t DB_getI2CDev( const DB_ElemLoc_t loc )
{
   return( DB_I2C_devices[loc] );
}

/******************************************************************************/
const DC3Error_t DB_readFlash(
      const DC3DBElem_t elem,
      const DC3AccessType_t accessType,
      const size_t bufSize,
      uint8_t* pBuffer,
      uint16_t* pResultLen
)
{

   DC3Error_t status = ERR_NONE;

   /* 1. Sanity checks of buffer sizes and memory allocations. Only check this
    * for bare metal access since QPC and FRT accesses will post events with
    * their own buffers */
   if ( _DC3_ACCESS_BARE == accessType ) {
      if ( bufSize < settingsDB[elem].size ) {
         status = ERR_MEM_BUFFER_LEN;
         goto DB_readFlash_ERR_HANDLE;     /* Stop and jump to error handling */
      }

      if ( NULL == pBuffer ) {
         status = ERR_MEM_NULL_VALUE;
         goto DB_readFlash_ERR_HANDLE;     /* Stop and jump to error handling */
      }
   }

   if( !DB_IS_ELEM_IN_FLASH(elem) ) {
      status = ERR_DB_ELEM_IS_NOT_IN_FLASH;
      goto DB_readFlash_ERR_HANDLE;        /* Stop and jump to error handling */
   }

   DBReadDoneEvt *evt;
   switch ( accessType ) {
      case _DC3_ACCESS_QPC:               /* Intentionally fall through */
      case _DC3_ACCESS_FRT: {
         evt = Q_NEW(DBReadDoneEvt, DB_FLASH_READ_DONE_SIG);
         evt->dbElem = elem;
         evt->status = ERR_NONE;

         /* Both evt->status and status should be assigned here */
         evt->status = status = FLASH_readBufferUint8(
               settingsDB[elem].offset,
               settingsDB[elem].size,
               MAX_DB_ELEM_SIZE,
               evt->dataBuf,
               (uint16_t *)&(evt->dataLen)
         );
         QACTIVE_POST(AO_SysMgr, (QEvt *)evt, AO_SysMgr);

         break;
      }
      case _DC3_ACCESS_BARE:              /* Intentionally fall through */
      default:
         if ( bufSize < settingsDB[elem].size ) {
            status = ERR_MEM_BUFFER_LEN;
            goto DB_readFlash_ERR_HANDLE;     /* Stop and jump to error handling */
         }

         if ( NULL == pBuffer ) {
            status = ERR_MEM_NULL_VALUE;
            goto DB_readFlash_ERR_HANDLE;     /* Stop and jump to error handling */
         }

         status = FLASH_readBufferUint8(
               settingsDB[elem].offset,
               settingsDB[elem].size,
               bufSize,
               pBuffer,
               pResultLen
         );

         break;
   }

   DB_readFlash_ERR_HANDLE:        /* Handle any error that may have occurred */
   ERR_COND_OUTPUT( status, accessType,
         "Reading from Flash DB element %s (%d): Error 0x%08x \n",
         CON_dbElemToStr( elem ), elem, status );

   return( status );
}

/******************************************************************************/
const bool DB_isArraysMatch(
      const uint8_t*  dt1,
      const uint8_t*  dt2,
      const uint16_t  len
)
{
   /* iterate arrays backwards and check if they match.  The reason for
    * iterating backwards is because most checks will involve changes in the
    * LSByte so this should be faster for most cases */
   for(int i = len - 1; i >= 0; i-- ) {
      if (dt1[i] != dt2[i]) {
         ERR_printf("dt1[%d]: %02x dt2[%d]: %02x\n", i, dt1[i], i, dt2[i]);
         return false;
      }
   }
   return true;
}

/**
 * @}
 * end addtogroup groupSettings
 */

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
