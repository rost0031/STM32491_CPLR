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

   status = pLogStub->setDC3LogCallBack( CLI_DC3LogCallback );
   if ( CLI_ERR_NONE != status ) {
      ERR_out << "Failed to set DC3 logging callback function. Exiting...";
      EXIT_LOG_FLUSH(1);
   }

   /* 3. Enable and disable logging for various modules in the library */
   pLogStub->enableLogForAllLibModules();

   LOG_setDbgLvl(DBG); /* Set logging level */

   /* 4. Set up the client api and initialize its logging.  Users can call the
    * constructor that also sets up the communication at the same time but if
    * something goes wrong and an exception is thrown, the program will log it
    * but attempt to continue since exceptions can't be thrown across dll
    * boundaries. This way is safer because it allows for explicit error
    * checking via error codes from class functions. */
   ClientApi *client = new ClientApi( pLogStub );

   /* 5. Set callbacks for Req, Ack, and Done msg types. */
   status = client->setReqCallBack(CLI_ReqCallback);
   if ( CLI_ERR_NONE != status ) {
      WRN_out << "Failed to set library callback function for Req msgs.";
   }
   status = client->setAckCallBack(CLI_AckCallback);
   if ( CLI_ERR_NONE != status ) {
      WRN_out << "Failed to set library callback function for Ack msgs.";
   }
   status = client->setDoneCallBack(CLI_DoneCallback);
   if ( CLI_ERR_NONE != status ) {
      WRN_out << "Failed to set library callback function for Done msgs.";
   }


   /* 6. Create a new CmdlineParser instance. */
   CmdlineParser *cmdline = new CmdlineParser();

   /* 7. Attempt to parse the cmdline arguments. */
   if( 0 != cmdline->parse(argc, argv) ) {
      ERR_out << "Failed to parse cmdline args. Exiting";
      EXIT_LOG_FLUSH(1);
   }

   /* 8. Set the connection.  This is done separately so that the ClientApi
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


   /* 9. Start the client.  This starts processing and can now accept
    * requests for work to be done as well as the responses from the DC3. */
   client->start(); /* Set the client running */

   DBG_out << "Waiting until the state machine is up and running...";
   boost::this_thread::sleep(boost::posix_time::milliseconds(5));


   CBErrorCode statusDC3;
   CBBootMode mode = _CB_NoBootMode;
   ClientError_t statusCli = client->DC3_getMode(&statusDC3, &mode);

   if( CLI_ERR_NONE == statusCli ) {
      DBG_out << "DC3 boot mode is: " << mode << " and status: "
            << statusDC3 << ". Client status: " << "0x" << std::hex
            << statusCli << std::dec;
   } else {
      ERR_out << "Got error " << "0x" << std::hex
            << statusCli << std::dec << " when trying to get boot mode";
   }

//   DBG_out << "Test job finished. Exiting.";
//   boost::this_thread::sleep(boost::posix_time::milliseconds(5));
   client->stop();
   DBG_out<< "Waiting for MainMgr AO to finish";
//   boost::this_thread::sleep(boost::posix_time::milliseconds(5));
   client->waitForStop();

   DBG_out<< "Exiting normally";
   EXIT_LOG_FLUSH(0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
