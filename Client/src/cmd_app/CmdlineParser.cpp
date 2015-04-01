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

/* Namespaces ----------------------------------------------------------------*/
using namespace std;
namespace po = boost::program_options;
using namespace po;

/* Compile-time called macros ------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/

/******************************************************************************/
CmdlineParser::CmdlineParser( int argc, char** argv ) :
      bInteractiveRunMode(true)
{
   /* Make a local store cmd line args */
   this->argc = argc;
   this->argv = argv;

   po::options_description desc("Allowed options");
   printf("Entry\n");
   try
   {
      printf("1\n");
      /* Try and keep these in alphabetical order */
      desc.add_options()

            ("help,h", "produce help message")

            ("cmd_mode,c", po::value<vector<string>>(&command)->zero_tokens(),
             "Override the default interactive operation and allow single cmd operation")

      ; //End of add_options()
printf("2\n");
      /* parse regular options */
      po::variables_map vm;
      po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
      po::store( parsed, vm);
      po::notify(vm);
printf("3\n");
      if ( vm.count("cmd_mode") ) {

         this->bInteractiveRunMode = false;

         cout << "bInteractiveRunMode was set to false" << endl;
      }
printf("4\n");
      /* Clear out the argument map */
      parsed_args.clear();

      if (vm.count("help")) {
         /* If the user specified help but no commands */
         cout << desc << endl;
         cout << "To get detailed help for any command, add a --help after the command." << endl;
         exit(0);
      }
   } catch(po::error& e) {
        cerr << "ERROR: " << e.what() << endl <<
                "Some Unknown ProgramOptions error occurred while parsing cmdline arguments" << endl << endl;
        cout << desc << endl;
        exit(1);
    } catch (...) {
        cerr << "ERROR: Some Unknown error occurred while parsing cmdline arguments" << endl << endl;
        cout << desc << endl;
        exit(1);
    }

    std::cout << "Boost version: " << BOOST_LIB_VERSION << std::endl;
}

/******************************************************************************/
CmdlineParser::~CmdlineParser( void )
{

}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
