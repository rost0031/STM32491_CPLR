/**
 * @file    Cmds.cpp
 * Various commands that can be sent from the cmd client
 *
 * @date    07/02/2015
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>
#include <vector>
#include <iterator>

/* Boost includes */
#include <boost/program_options.hpp>

/* App includes */
#include "Cmds.hpp"
#include "ArgParse.hpp"
#include "CliDbgModules.hpp"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
namespace po = boost::program_options;
using namespace po;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
CLI_MODULE_NAME( CLI_DBG_MODULE_CMD );

/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/**
 * @brief   Output dbgModules bitfield to a human readable stringstream
 * @param [in|out]: stringstream ref to output to.
 * @param [in] dbgModuleSet: uint32_t bitfield representation of the dbgModule
 * states.
 *
 * @return: None
 */
static void CMD_dbgModulesToStream( stringstream& ss, uint32_t dbgModules);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
static void CMD_dbgModulesToStream( stringstream& ss, uint32_t dbgModules)
{
   ss << setw(24) << setfill(' ') << " -- Debug Module name --"            <<  "   " << "-- State --" << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_GEN )      <<  "     :     " << ( dbgModules & DC3_DBG_MODL_GEN   ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_SER )      <<  "     :     " << ( dbgModules & DC3_DBG_MODL_SER   ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_TIME )     <<  "     :     " << ( dbgModules & DC3_DBG_MODL_TIME  ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_ETH )      <<  "     :     " << ( dbgModules & DC3_DBG_MODL_ETH   ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_I2C )      <<  "     :     " << ( dbgModules & DC3_DBG_MODL_I2C   ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_I2C_DEV )  <<  "     :     " << ( dbgModules & DC3_DBG_MODL_I2C_DEV ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_NOR )      <<  "     :     " << ( dbgModules & DC3_DBG_MODL_NOR   ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_SDRAM )    <<  "     :     " << ( dbgModules & DC3_DBG_MODL_SDRAM ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_DBG )      <<  "     :     " << ( dbgModules & DC3_DBG_MODL_DBG   ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_COMM )     <<  "     :     " << ( dbgModules & DC3_DBG_MODL_COMM  ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_CPLR )     <<  "     :     " << ( dbgModules & DC3_DBG_MODL_CPLR  ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_DB )       <<  "     :     " << ( dbgModules & DC3_DBG_MODL_DB    ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_FLASH )    <<  "     :     " << ( dbgModules & DC3_DBG_MODL_FLASH ? " On" : " Off" ) << endl;
   ss << setw(20) << setfill(' ') << enumToString( DC3_DBG_MODL_SYS )      <<  "     :     " << ( dbgModules & DC3_DBG_MODL_SYS   ? " On" : " Off" ) << endl;
}

/******************************************************************************/
APIError_t CMD_runRamTest( ClientApi* client, DC3Error_t* statusDC3 )
{
   APIError_t statusAPI = API_ERR_NONE;

   DC3RamTest_t test = _DC3_RAM_TEST_NONE;  // Test which can fail
   uint32_t addr = 0x00000000;            // Address at which a test can fail

   string cmd = "ram_test"; // This is the name of the command we are running
   stringstream ss;
   ss << "*** Starting " << cmd << " command to test of external RAM of DC3... ***";

   CON_print(ss.str());
   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   if( API_ERR_NONE == (statusAPI = client->DC3_ramTest(statusDC3, &test, &addr))) {
      ss << "Finished " << cmd << ". Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. No RAM errors found.";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3
               << " with test " << enumToString(test) << " failing at addr "
               << "0x" << setw(8) << setfill('0') << hex << addr
               << dec;
      }
   } else {
      ss << "Unable to complete " << cmd << " command due to API error: "
            << "0x" << setw(8) << setfill('0') << std::hex << statusAPI << std::dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return statusAPI;
}

/******************************************************************************/
APIError_t CMD_runGetMode(
      ClientApi* client,
      DC3Error_t* statusDC3,
      DC3BootMode_t* mode
)
{
   APIError_t statusAPI = API_ERR_NONE;
   string cmd = "get_mode"; // This is the name of the command we are running

   stringstream ss;
   ss << "*** Starting " << cmd << " command to get the boot mode of DC3... ***";

   CON_print(ss.str());
   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_getMode(statusDC3, mode))) {

      ss << "Finished " << cmd << ". Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. DC3 is currently in " << enumToString(*mode)
               << " boot mode";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }

   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runSetMode(
      ClientApi* client,
      DC3Error_t* statusDC3,
      DC3BootMode_t mode
)
{
   APIError_t statusAPI = API_ERR_NONE;
   string cmd = "set_mode"; // This is the name of the command we are running

   stringstream ss;
   ss << "*** Starting " << cmd << " command to to set the boot mode of DC3... ***";
   CON_print(ss.str());
   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_setMode(statusDC3, mode)) ) {

      ss << "Finished " << cmd << " cmd. Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. DC3 should now be in " << enumToString(mode)
               << " boot mode";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }
   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runFlash(
      ClientApi* client,
      DC3Error_t* statusDC3,
      DC3BootMode_t type,
      const string& file
)
{
   APIError_t statusAPI = API_ERR_NONE;

   // Only allow Application FW image for now.
   if ( _DC3_Application != type ) {
      ERR_out << "Only " << enumToString(_DC3_Application) << " FW images are currently supported.";
      return( API_ERR_UNIMPLEMENTED );
   }

   string cmd = "flash"; // This is the name of the command we are running

   stringstream ss;
   ss << "*** Starting " << cmd << " command to flash "
         << enumToString(_DC3_Application) << " FW image to the DC3... ***";
   CON_print(ss.str());
   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_flashFW(statusDC3, type, file.c_str() )) ) {
      ss << "Finished " << cmd << ". Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. DC3 " << enumToString(type)
               << " FW image should now be flashed.";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }
   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runReadI2C(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint16_t* pBytesRead,
      uint8_t* pBuffer,
      const size_t nMaxBufferSize,
      const size_t nBytesToRead,
      const size_t nStart,
      const DC3I2CDevice_t dev,
      const DC3AccessType_t  acc
)
{
   APIError_t statusAPI = API_ERR_NONE;
   string cmd = "read_i2c"; // This is the name of the command we are running

   stringstream ss;
   ss << "*** Starting " << cmd << " command to read an I2C device on DC3... ***";
   CON_print(ss.str());
   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_readI2C( statusDC3, pBytesRead,
            pBuffer, nMaxBufferSize, nBytesToRead, nStart, dev, acc) )) {
      ss << "Finished " << cmd << ". Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. ***" << endl << "*** Read " << *pBytesRead << " bytes: [ ";
         for ( unsigned int i = 0; i < *pBytesRead; i++ ) {
            ss << "0x" << hex << setfill('0') << setw(2) << unsigned(pBuffer[i]) << " ";
         }
         ss << "]" << dec;
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }
   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runWriteI2C(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint8_t* pBuffer,
      const size_t nBytesToWrite,
      const size_t nStart,
      const DC3I2CDevice_t dev,
      const DC3AccessType_t  acc
)
{
   APIError_t statusAPI = API_ERR_NONE;

   string cmd = "write_i2c"; // This is the name of the command we are running
   stringstream ss;

   // Conditional printout since this command does so many different things
   ss << "*** Starting " << cmd << " command to write to an I2C device on DC3... ***";
   CON_print(ss.str());
   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_writeI2C( statusDC3, pBuffer,
         nBytesToWrite, nStart, dev, acc ))) {
      ss << "Finished " << cmd << " cmd. Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. ***" << endl << "*** Wrote " << nBytesToWrite << " bytes: [ ";
         for ( unsigned int i = 0; i < nBytesToWrite; i++ ) {
            ss << "0x" << hex << setfill('0') << setw(2) << unsigned(pBuffer[i]) << " ";
         }
         ss << "]" << dec;
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }
   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runGetDbgModules(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint32_t* dbgModules
)
{
   APIError_t statusAPI = API_ERR_NONE;


   stringstream ss;
   string cmd = "get_dbg_modules"; // This is the name of the command we are running
   // Conditional printout since this command does so many different things
   ss << "*** Starting " << cmd << " command to get the state of debug modules on DC3... ***";
   CON_print(ss.str());
   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_getDbgModules(statusDC3, dbgModules))) {

      ss << "Finished get_dbg_modules cmd. Command " << endl;
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. ***" << endl;
         ss << "*** DC3 debug module states are as follows ***" << endl;
         CMD_dbgModulesToStream( ss, *dbgModules);
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }

   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runSetDbgModule(
      ClientApi* client,
      DC3Error_t* statusDC3,
      uint32_t* dbgModuleSet,
      bool bEnable,
      bool bOverWrite
)
{
   APIError_t statusAPI = API_ERR_NONE;
   stringstream ss;
   string cmd = "set_dbg_modules"; // This is the name of the command we are running
   // Conditional printout since this command does so many different things
   ss << "*** Starting " << cmd << " command to ";

   if (bOverWrite) {
      ss << "overwrite the dbg modules states bitfield ";
   } else {
      if (bEnable) {
         ss << "enable ";
      } else {
         ss << "disable ";
      }
      ss << "the " << enumToString((DC3DbgModule_t) *dbgModuleSet) << " debug module ";
   }
   ss << "on DC3... ***";
   CON_print(ss.str());

   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_setDbgModules(statusDC3, dbgModuleSet, bEnable, bOverWrite))) {

      ss << "Finished " << cmd << ". Command " << endl;
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. ***" << endl;
         ss << "*** DC3 debug module states are as follows ***" << endl;
         CMD_dbgModulesToStream( ss, *dbgModuleSet);
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }

   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}


/******************************************************************************/
APIError_t CMD_runSetDbgDevice(
      ClientApi* client,
      DC3Error_t* statusDC3,
      DC3MsgRoute_t device,
      bool bEnable
)
{
   APIError_t statusAPI = API_ERR_NONE;
   stringstream ss;
   string cmd = "set_dbg_device"; // This is the name of the command we are running
   // Conditional printout since this command does so many different things
   ss << "*** Starting " << cmd << " cmd to ";
   if (bEnable) {
      ss << "enable ";
   } else {
      ss << "disable ";
   }

   ss << "debugging output on " << enumToString(device) << " device on the DC3 ***";
   CON_print(ss.str());

   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_setDbgDevice(statusDC3, device, bEnable))) {

      ss << "Finished " << cmd << ". Command " << endl;
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. ***" << endl;
         ss << "*** DC3 debugging output over " << enumToString(device)
               << " is now " << (( true == bEnable ) ? " enabled " : "disabled ");
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }

   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;

   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runResetDB(
      ClientApi* client,
      DC3Error_t* statusDC3
)
{
   APIError_t statusAPI = API_ERR_NONE;
   stringstream ss;
   string cmd = "reset_db";    // This is the name of the command we are running
   ss << "*** Starting "<< cmd << " command to reset settings in DC3 Database to defaults ***";
   CON_print(ss.str());

   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_resetDB(statusDC3))) {

      ss << "Finished " << cmd << ". Command " << endl;
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. ***" << endl;
         ss << "*** DC3 Database is now reset to compiled defaults";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }

   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;

   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runGetDbElem(
      ClientApi* client,
      DC3Error_t* statusDC3,
      const DC3DBElem_t elem,
      const DC3AccessType_t  acc,
      const size_t bufferSize,
      uint8_t* const pBuffer,
      size_t* pBytesInBuffer
)
{
   APIError_t statusAPI = API_ERR_NONE;
   stringstream ss;
   string cmd = "get_db_elem";    // This is the name of the command we are running
   ss << "*** Starting "<< cmd << " command to get a DC3 Database element ***";
   CON_print(ss.str());

   ss.str(std::string()); // It's the only way to actually clear the stringstream

   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_getDbElem(statusDC3, elem, acc, bufferSize, pBuffer, pBytesInBuffer ))) {

      ss << "Finished " << cmd << ". Command " << endl;
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. ***" << endl;
         ss << "*** DB element " << enumToString(elem) << " value = ***" << endl;

         ss << "*** Raw bytes: [ ";
         for ( unsigned int i = 0; i < *pBytesInBuffer; i++ ) {
            ss << "0x" << hex << setfill('0') << setw(2) << unsigned(pBuffer[i]) << " ";
         }
         ss << " ]" << dec;

         // Some elements can be printed as strings
         if( elem == _DC3_DB_BOOT_BUILD_DATETIME ||
             elem == _DC3_DB_APPL_BUILD_DATETIME ||
             elem == _DC3_DB_FPGA_BUILD_DATETIME ) {
            ss << " ***" << endl << "*** As string: " << pBuffer;
         } else if ( elem == _DC3_DB_IP_ADDR ) {
            ss << " ***" << endl << "*** As IP address: [ ";
            for ( unsigned int i = 0; i < *pBytesInBuffer; i++ ) {
               ss << unsigned(pBuffer[i]);
               if ( i < ((*pBytesInBuffer) - 1) ) {
                  ss << ".";
               }
            }
            ss << " ]" << dec;
         } else if ( elem == _DC3_DB_MAC_ADDR ) {
            ss << " ***" << endl << "*** As MAC Address: [ ";
            for ( unsigned int i = 0; i < *pBytesInBuffer; i++ ) {
               ss << hex << setfill('0') << setw(2) << unsigned(pBuffer[i]);
               if ( i < ((*pBytesInBuffer) - 1) ) {
                  ss << ":";
               }
            }
            ss << " ]" << dec;
         }

      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }

   } else {
      ss << "Unable to complete " << cmd << " cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;

   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Harry Rostovtsev. All rights reserved *****END OF FILE****/
