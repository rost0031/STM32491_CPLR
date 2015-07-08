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

/* App includes */
#include "Cmds.hpp"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

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

   stringstream ss;

   CBRamTest_t test = _CB_RAM_TEST_NONE;  // Test which can fail
   uint32_t addr = 0x00000000;            // Address at which a test can fail

   CON_print("*** Starting ram_test cmd to test of external RAM of DC3... ***");
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
//   CBErrorCode statusDC3 = ERR_NONE;

   // No need to extract the value from the arg=value pair for this cmd.
//   CBBootMode mode = _CB_NoBootMode;       /**< Store the bootmode here */
   stringstream ss;

   CON_print("*** Starting get_mode cmd to get the boot mode of DC3... ***");
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
            << "0x" << setw(8) << setfill('0') << std::hex << statusAPI << std::dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}

/******************************************************************************/
APIError_t CMD_runSetMode(  ClientApi* client )
{
   APIError_t statusAPI = API_ERR_NONE;
   CBErrorCode statusDC3 = ERR_NONE;

   // No need to extract the value from the arg=value pair for this cmd.
   CBBootMode mode = _CB_NoBootMode;       /**< Store the bootmode here */
   stringstream ss;

   CON_print("*** Starting get_mode cmd to get the boot mode of DC3... ***");
   ss << "*** "; // Prepend so start and end of command output are easily visible

   // Execute (and block) on this command
   if( API_ERR_NONE == (statusAPI = client->DC3_getMode(&statusDC3, &mode))) {

      ss << "Finished get_mode cmd. Command ";
      if (ERR_NONE == statusDC3) {
         ss << "completed with no errors. DC3 is currently in " << enumToString(mode)
               << " boot mode";
      } else {
         ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << statusDC3 << dec;
      }

   } else {
      ss << "Unable to finish get_mode cmd to DC3 due to API error: "
            << "0x" << setw(8) << setfill('0') << std::hex << statusAPI << std::dec;
   }

   ss << " ***"; // Append so start and end of command output are easily visible
   CON_print(ss.str());                                      // output to screen

   return( statusAPI );
}
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
