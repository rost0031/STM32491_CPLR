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

#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
using boost::assign::map_list_of;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

//const boost::unordered_map<DBG_LEVEL_T, const char*> enumToStr = map_list_of
//      (DBG, "DBG")
//      (LOG, "LOG")
//      (WRN, "WRN");

//template<typename T> struct map_init_helper
//{
//    T& data;
//    map_init_helper(T& d) : data(d) {}
//    map_init_helper& operator() (typename T::key_type const& key, typename T::mapped_type const& value)
//    {
//        data[key] = value;
//        return *this;
//    }
//};
//
//template<typename T> map_init_helper<T> map_init(T& item)
//{
//    return map_init_helper<T>(item);
//}


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
int main(int argc, char *argv[])
{
   ClientError_t status = CLI_ERR_NONE;               /* Keep track of status */

   Logging *logger;

//   cout << "Enum DBG = " << enumToStr.at(DBG) << endl;
   std::cout << enumToString(DBG) << "\n";

//   DBG_LEVEL_T tmp = LOG;
//   std::stringstream line("DBG");
//
//   line >> enumFromString(tmp);
//
//   std::cout << "LOG: " << LOG << " : " << enumToString(tmp) << "\n";



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

   /* Set up the client api and initialize its logging */
   ClientApi *client = new ClientApi(logger->getLogStubPtr());

   client->run();

   Job *job = new Job(logger->getLogStubPtr());


   return (0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
