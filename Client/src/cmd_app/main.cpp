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
   string appName = argv[0];
   unsigned found = appName.find_last_of("/\\");
   appName = appName.substr(found+1);
   LOG_out << "Starting " << appName;

   /* 1. Create a new LogStub instance.  This allows setting of logging
    * callbacks to the rest of the client library.  Safe to do this without
    * try/catch since no exceptions are thrown from library. */
    LogStub *pLogStub = new LogStub();

   /* 2. Set the callback functions which allow the library to log to this
    * cmdline client. */
   status = pLogStub->setLibLogCallBack( CLI_LibLogCallback );
   if ( CLI_ERR_NONE != status ) {
      ERR_out << "Failed to set library logging callback function. Exiting...";
      EXIT_LOG_FLUSH(1);
   }

   /* 3. Enable and disable logging for various modules in the library */
   pLogStub->enableLogForAllLibModules();

//   DBG_out << "DBG test before setting ";
//   LOG_out << "LOG test before setting ";
//   WRN_out << "WRN test before setting ";
//   ERR_out << "ERR test before setting ";

   LOG_setDbgLvl(DBG); /* Set logging level */
//   DBG_out << "DBG test after setting ";
//   LOG_out << "LOG test after setting ";
//   WRN_out << "WRN test after setting ";
//   ERR_out << "ERR test after setting ";
//
//
//   MENU_print("Testing menu NEW output 1");
//   MENU_print("Testing menu NEW output 2");
//   MENU_print("Testing menu NEW output 3");
//   MENU_print("Testing menu NEW output 4");
//   MENU_print("Testing menu NEW output 5");
//   MENU_print("Testing menu NEW output 6");

   /* 3. Create a new CmdlineParser instance. */
   CmdlineParser *cmdline = new CmdlineParser();

   /* 4. Attempt to parse the cmdline arguments. */
   if( 0 != cmdline->parse(argc, argv) ) {
      ERR_out << "Failed to parse cmdline args. Exiting";
      EXIT_LOG_FLUSH(1);
   }

   /* 5. Set up the client api and initialize its logging.  Users can call the
    * constructor that also sets up the communication at the same time but if
    * something goes wrong and an exception is thrown, the program will log it
    * but attempt to continue since exceptions can't be thrown across dll
    * boundaries. This way is safer because it allows for explicit error
    * checking via error codes from class functions. */
   ClientApi *client = new ClientApi( pLogStub );

   /* 6. Set the connection.  This is done separately so that the ClientApi
    * class can catch any exceptions that happen and convert them to error
    * codes that the functions can return (and constructors can't). */
   if( cmdline->isConnEth() ) {
      string ipAddr, remPort, locPort;
      cmdline->getEthConnOpts(ipAddr, remPort, locPort);
      DBG_out << "Got " << ipAddr << ", " << remPort << ", " << locPort;

      status = client->setNewConnection(
            ipAddr.c_str(),
            remPort.c_str(),
            locPort.c_str()
      );

      if ( CLI_ERR_NONE != status ) {
         ERR_out << "Unable to open UDP connection. Error: 0x"
               << setfill('0') << setw(8) << hex << status;
         EXIT_LOG_FLUSH(1);
      }
   } else if ( cmdline->isConnSer() ) {
      string serDev;
      int baudRate = 0;
      bool dfuseFlag = true;
      cmdline->getSerConnOpts(serDev, &baudRate, &dfuseFlag);
      DBG_out << "Got " << serDev << ", " << baudRate << ", " << dfuseFlag;

      status = client->setNewConnection(
            serDev.c_str(),
            baudRate,
            dfuseFlag
      );

      if ( CLI_ERR_NONE != status ) {
         ERR_out << "Unable to open Serial connection. Error: 0x"
               << setfill('0') << setw(8) << hex << status;
         CON_print("!!! Make sure you're using the correct serial port and nothing else is using it !!!");
         EXIT_LOG_FLUSH(1);
      }
   }

   /* 7. Start the client.  This starts job processing and can now accept
    * requests for work to be done as well as the responses from the DC3. */
   client->run(); /* Set the client running */

//   struct CBBasicMsg _CBBasicMsg;
//   uint8_t _buffer[20];
//   int offset;
//
//   _CBBasicMsg._msgID = 0;
//   _CBBasicMsg._msgName = _CBGetBootModeMsg;
//   _CBBasicMsg.
//   CBBasicMsg_write_delimited_to(&_CBBasicMsg, (void *)_buffer, 0);


   DBG_out << "Waiting until the state machine is up and running...";
   boost::this_thread::sleep(boost::posix_time::milliseconds(10));
   DBG_out << "Starting a test job.";
   client->startJob();

   DBG_out << "Waiting for test job to finish...";
   client->waitForJobDone();

   DBG_out << "Test job finished. Exiting.";
   boost::this_thread::sleep(boost::posix_time::milliseconds(50));
   client->stop();
   DBG_out<< "Waiting for MainMgr AO to finish";
   boost::this_thread::sleep(boost::posix_time::milliseconds(50));
   client->waitForStop();

   DBG_out<< "Exiting normally";
   EXIT_LOG_FLUSH(0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
