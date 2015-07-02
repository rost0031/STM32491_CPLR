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
   APIError_t status = API_ERR_NONE;
   CBErrorCode statusDC3 = ERR_NONE;

   CBRamTest_t test = _CB_RAM_TEST_NONE;
   uint32_t addr = 0x00000000;
   stringstream ss;

   CON_print(" *** Running memory test of external RAM of DC3... ***");
   if( API_ERR_NONE == (status = client->DC3_ramTest(&statusDC3, &test, &addr))) {
      ss.clear();
      ss << " *** Ram test of DC3 ";
      if (ERR_NONE == statusDC3) {
         ss << "completed with no errors.";
      } else {
         ss << "failed with ERROR: 0x" << setw(8) << setfill('0') << hex << statusDC3
               << " with test " << enumToString(test) << " failing at addr "
               << "0x" << setw(8) << setfill('0') << hex << addr
               << dec;
      }
      ss << " ***";
      CON_print(ss.str());
   } else {
      ERR_out << " *** Got DC3 error " << "0x" << std::hex
            << status << std::dec << " when trying to run RAM test. ***";
   }

   return status;
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
