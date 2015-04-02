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
#include "ClientModules.h"
#include "LogStub.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
int main(int argc, char *argv[])
{
   ClientError_t status = CLI_ERR_NONE;               /* Keep track of status */

   Logging *logger;

   try {
      logger = new Logging();
   } catch ( ... ) {
      cerr << "Failed to set up logging.  Exiting" << endl;
      exit(1);
   }

   LOG_printf(logger, "Successfully set up logging\n");

//   ClientError_t status;
//
//   status = logger->setLibLogCallBack( logger->log );
//   if ( CLI_ERR_NONE != status ) {
//      cerr << "Failed to set logging callback. Error: 0x"
//            << setfill('0') << setw(8) << std::hex << status << endl;
//      exit(1);
//   } else {
//      LOG_printf( logger, "Successfully set up logging callback.\n");
//   }

   CmdlineParser *cmdline = new CmdlineParser( logger );
   if( 0 != cmdline->parse(argc, argv) ) {
      ERR_printf(logger, "Failed to parse cmdline args. Exiting\n");
      exit(1);
   }
   Job *job = new Job(logger->getLogStubPtr());


   return (0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
