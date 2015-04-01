/**
 * @file    main.cpp
 * @brief   Main for Client for the coupler board (CB).
 *
 * @date    03/23/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* This is needed to make the windows version work. */
#define _WIN32_WINNT 0x400

/* Includes ------------------------------------------------------------------*/
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include "CmdlineParser.h"
#include "Job.h"
#include "Logging.h"
#include "CBSharedDbgLevels.h"
#include "Callbacks.h"
/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
int main(int argc, char *argv[])
{
   cout << "Testing Client " << endl;

   int tmp = 6;
   CON_output(
         DBG,
         __func__,
         __LINE__,
         "Testing logging function %d\n",
         tmp
   );

   Logging *log = new Logging();

   cerr << "Demonstration of invalid logging callback setting. Expecting error: 0x"
            << setfill('0') << setw(8) << std::hex << CLI_ERR_INVALID_CALLBACK << endl;
   ClientError_t status = log->setLibLogCallBack( NULL );
   if ( CLI_ERR_NONE != status ) {
      cerr << "Failed to set logging callback. Error: 0x"
            << setfill('0') << setw(8) << std::hex << status << endl;
   }

   cerr << "Demonstration of valid logging callback setting. Expecting error: 0x"
            << setfill('0') << setw(8) << std::hex << CLI_ERR_NONE << endl;
   status = log->setLibLogCallBack( CLI_LibLogCallback );
   if ( CLI_ERR_NONE != status ) {
      cerr << "Failed to set logging callback. Error: 0x"
            << setfill('0') << setw(8) << std::hex << status << endl;
   } else {
      cerr << "Successfully set up logging callback. You should have seen a date"
            " and time with some logging printed before this.  Error: 0x"
            << setfill('0') << setw(8) << std::hex << status << endl;
   }



   //   CmdlineParser *cmdline = new CmdlineParser(argc, argv);

   //   Job *job = new Job();


   return (0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
