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
            m_bInteractiveRunMode(true)
{
   this->m_log = logger;
   DBG_printf(this->m_log, "Logging for CmdlineParser class enabled\n");
}

/******************************************************************************/
int CmdlineParser::parse( int argc, char** argv )
{
   /* Make a local store cmd line args */
   this->m_argc = argc;
   this->m_argv = argv;

   po::options_description desc("Allowed options");

   desc.add_options()
         ("help,h", "produce help message")
         ("version,v", "print version of client") // TODO: not implemented
         ("mode,m", "Override the default interactive mode and allow single cmd operation")

         ("ip_address,i", po::value<string>(&m_ip_address),
            "Set remote IP address to connect to (Required if serial_dev not set)")

         ("remote_port,p", po::value<string>(&m_remote_port)->default_value("1502"),
            "Set remote port number")

         ("local_port,l", po::value<int>(&m_local_port)->default_value(50249),
            "Set local port number")

         ("serial_dev,s", po::value<string>(&m_serial_dev),
            "Set the name of the serial device to connect to instead of "
            "using an IP connection (in the form of /dev/ttySx on cygwin/linux, "
            "and COMX in windows. Required if remote_ip_address not set)")

         ("serial_baud,b", po::value<int>(&m_serial_baud)->default_value(115200),
            "Set the baud rate of the serial device")
   ; // End of add_options()

   DBG_printf(this->m_log,"Parsing cmdline arguments...\n");
   /* parse regular options */
   po::variables_map vm;
   po::parsed_options parsed = po::parse_command_line(argc, argv, desc);
   po::store( parsed, vm);
   po::notify(vm);

   try {
      /* Check for general help request first */
      if (vm.count("help")) {
         cout << desc << endl;
         cout << "To get detailed help for any command, add a --help after the command." << endl;
         exit(0);
      }

      if (vm.count("version")) {
         cout << "CmdLine Client App version: " << "TODO" << endl;
         cout << "CmdLine Client Lib version: " << "TODO" << endl;
         cout << "Boost Library version: " << BOOST_LIB_VERSION << endl;
         exit(0);
      }

      /* Allow user to override the interactive (menu) mode and run in single
       * cmd mode.  This is for scripting purposes */
      if ( vm.count("mode") ) {
         this->m_bInteractiveRunMode = false;
         DBG_printf(this->m_log,"Overriding interactive mode and running in single cmd mode\n");
      }

      /* Serial and IP connections are mutually exclusive so treat them as such
       * on the cmdline. */
      if (vm.count("ip_address") && !vm.count("serial_dev")) {

         this->m_conn_mode = _CB_EthCli;

         LOG_printf_b << "ip_address was set to "
               << vm["ip_address"].as<string>() << endl;



         if (vm.count("remote_port")) {
            cout << "remote_port was set to "
                  << vm["remote_port"].as<string>() << endl;
         }

      } else if (vm.count("serial_dev") && !vm.count("ip_address")) {
         this->m_conn_mode = _CB_Serial;

         LOG_printf_b << "serial_dev was set to "
               << vm["serial_dev"].as<string>() << endl;

         if (vm.count("serial_baud")) {
            LOG_printf_b << "serial_baud was set to "
                  << vm["serial_baud"].as<int>() << endl;
         }

      } else  {
         /* User didn't specify any connection options.  Attempt using default
          * IP address on the default port. */
         this->m_conn_mode = _CB_EthCli;
         this->m_ip_address = "172.27.0.75";
         LOG_printf(this->m_log,"No IP address or serial device specified.\n");
         LOG_printf(this->m_log,"Using default ip address %s to attempt connect connection.\n", this->m_ip_address.c_str());
      }

      /* Clear out the argument map */
      parsed_args.clear();

   } catch(po::error& e) {
      string error = e.what();
      ERR_printf(this->m_log,"Exception %s while parsing cmdline arguments.\n", error.c_str() );
      return(1);
   } catch (...) {
      ERR_printf(this->m_log, "Some Unknown error occurred while parsing cmdline arguments\n");
      return(1);
   }

   return( 0 );
}

/******************************************************************************/
CmdlineParser::~CmdlineParser( void )
{

}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
