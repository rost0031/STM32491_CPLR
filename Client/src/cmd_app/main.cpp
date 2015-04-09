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

/* Local App includes */
#include "CmdlineParser.h"
#include "Logging.h"

/* Lib includes */
#include "Job.h"
#include "CBSharedDbgLevels.h"
#include "Callbacks.h"
#include "ClientModules.h"
#include "LogStub.h"
#include "ClientApi.h"
#include "EnumMaps.h"


/* Namespaces ----------------------------------------------------------------*/
using namespace std;
using namespace boost::log;

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

   /* Test of enum to string capability */
   std::cout << enumToString(DBG) << "\n";

   Logging *logger;

   try {
      logger = new Logging();
   } catch ( ... ) {
      cerr << "Failed to set up logging.  Exiting" << endl;
      EXIT_LOG_FLUSH(1);
   }

//   sources::severity_logger<DBG_LEVEL_T> lg;
   LOG_out << "Testing logging via boost from main";



   MENU_print("Testing menu NEW output 1");
   MENU_print("Testing menu NEW output 2");
   MENU_print("Testing menu NEW output 3");
   MENU_print("Testing menu NEW output 4");
   MENU_print("Testing menu NEW output 5");
   MENU_print("Testing menu NEW output 6");

//   BOOST_LOG_TRIVIAL(fatal) << "Test";


//   LOG_printf(logger, "Successfully set up logging\n");

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
      ERR_out << "Failed to parse cmdline args. Exiting";
      EXIT_LOG_FLUSH(1);
   }

   /* Set up the client api and initialize its logging */
   ClientApi *client = new ClientApi(logger->getLogStubPtr());

   client->run();

   Job *job = new Job(logger->getLogStubPtr());


   EXIT_LOG_FLUSH(0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
