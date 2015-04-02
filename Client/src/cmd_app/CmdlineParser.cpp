/**
 * @file    CmdlineParser.cpp
 * Class that contains the command line parser.
 *
 * @date    03/26/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "CmdlineParser.h"
#include "Logging.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
namespace po = boost::program_options;
using namespace po;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/

/******************************************************************************/
CmdlineParser::CmdlineParser( Logging* logger ) :
            bInteractiveRunMode(true)
{
   this->log = logger;
   DBG_printf(this->log, "Logging for CmdlineParser class enabled\n");
}

/******************************************************************************/
int CmdlineParser::parse( int argc, char** argv )
{
   /* Make a local store cmd line args */
   this->argc = argc;
   this->argv = argv;

   po::options_description desc("Allowed options");

   desc.add_options()
         ("help,h", "produce help message")
         ("version,v", "print version of client")
         ("mode,m", "Override the default interactive mode and allow single cmd operation")

   ; // End of add_options()

   DBG_printf(this->log,"Parsing...\n");
   /* parse regular options */
   po::variables_map vm;
   po::parsed_options parsed = po::parse_command_line(argc, argv, desc);
   po::store( parsed, vm);
   po::notify(vm);

   DBG_printf(this->log,"Finished parsing.\n");
   if ( vm.count("mode") ) {

      this->bInteractiveRunMode = false;

      cout << "bInteractiveRunMode was set to false" << endl;
   }
   printf("4\n");
   /* Clear out the argument map */
//   parsed_args.clear();

   try {
      if (vm.count("help")) {
         /* If the user specified help but no commands */
         cout << desc << endl;
         cout << "To get detailed help for any command, add a --help after the command." << endl;
         exit(0);
      }
   } catch(po::error& e) {
      string error = e.what();
      ERR_printf(this->log,"Exception %s while parsing cmdline arguments.\n", error.c_str() );
      return(1);
   } catch (...) {
      ERR_printf(this->log, "Some Unknown error occurred while parsing cmdline arguments\n");
      return(1);
   }

   std::cout << "Boost version: " << BOOST_LIB_VERSION << std::endl;
}

/******************************************************************************/
CmdlineParser::~CmdlineParser( void )
{

}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
