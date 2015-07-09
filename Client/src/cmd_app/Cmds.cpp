/**
 * @file    Cmds.cpp
 * Various commands that can be sent from the cmd client
 *
 * @date    07/02/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
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

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
namespace po = boost::program_options;
using namespace po;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
APIError_t CMD_runRamTest( ClientApi* client )
{
   APIError_t statusAPI = API_ERR_NONE;
   CBErrorCode statusDC3 = ERR_NONE;

   CBRamTest_t test = _CB_RAM_TEST_NONE;  // Test which can fail
   uint32_t addr = 0x00000000;            // Address at which a test can fail

   CON_print("*** Starting ram_test cmd to test of external RAM of DC3... ***");

   stringstream ss;
   ss << "*** "; // Prepend so start and end of command output are easily visible

   if( API_ERR_NONE == (statusAPI = client->DC3_ramTest(&statusDC3, &test, &addr))) {
      ss << "Finished memory test of external RAM of DC3. Command ";
      if (ERR_NONE == statusDC3) {
         ss << "completed with no errors. No RAM errors found.";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << statusDC3
               << " with test " << enumToString(test) << " failing at addr "
               << "0x" << setw(8) << setfill('0') << hex << addr
               << dec;
      }
   } else {
      ss << "Unable to finish memory test of external RAM of DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << std::hex << statusAPI << std::dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return statusAPI;
}

/******************************************************************************/
APIError_t CMD_runGetMode(
      ClientApi* client,
      CBErrorCode* statusDC3,
      CBBootMode* mode
)
{
   APIError_t statusAPI = API_ERR_NONE;

   CON_print("*** Starting get_mode cmd to get the boot mode of DC3... ***");

   stringstream ss;
   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_getMode(statusDC3, mode))) {

      ss << "Finished get_mode cmd. Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. DC3 is currently in " << enumToString(*mode)
               << " boot mode";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }

   } else {
      ss << "Unable to finish get_mode cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runSetMode(
      ClientApi* client,
      CBErrorCode* statusDC3,
      CBBootMode mode
)
{
   APIError_t statusAPI = API_ERR_NONE;

   CON_print("*** Starting set_mode cmd to set the boot mode of DC3... ***");

   stringstream ss;
   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_setMode(statusDC3, mode)) ) {

      ss << "Finished set_mode cmd. Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. DC3 should now be in " << enumToString(mode)
               << " boot mode";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }
   } else {
      ss << "Unable to finish set_mode cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runFlash(
      ClientApi* client,
      CBErrorCode* statusDC3,
      CBBootMode type,
      const string& file
)
{
   APIError_t statusAPI = API_ERR_NONE;

   // Only allow Application FW image for now.
   if ( _CB_Application != type ) {
      ERR_out << "Only " << enumToString(_CB_Application) << " FW images are currently supported.";
      return( API_ERR_UNIMPLEMENTED );
   }

   CON_print("*** Starting flash cmd to flash FW on DC3... ***");

   stringstream ss;
   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_flashFW(statusDC3, type, file.c_str() )) ) {
      ss << "Finished flash cmd. Command ";
      if (ERR_NONE == *statusDC3) {
         ss << "completed with no errors. DC3 " << enumToString(type)
               << " FW image should now be flashed.";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << *statusDC3 << dec;
      }
   } else {
      ss << "Unable to finish flash cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << hex << statusAPI << dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
