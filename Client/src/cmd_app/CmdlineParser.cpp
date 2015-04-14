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
int CmdlineParser::parse( int argc, char** argv )
{
   po::options_description desc("Allowed options");

   desc.add_options()
         ("help,h", "produce help message")
         ("version,v", "print version of client") // TODO: not implemented
         ("mode,m", "Override the default interactive mode and allow single cmd operation")

         ("ip_address,i", po::value<string>(&m_ip_address),
            "Set remote IP address to connect to (Required if serial_dev not set)")

         ("remote_port,p", po::value<string>(&m_remote_port)->default_value("1502"),
            "Set remote port number")

         ("local_port,l", po::value<string>(&m_local_port)->default_value("50249"),
            "Set local port number")

         ("serial_dev,s", po::value<string>(&m_serial_dev),
            "Set the name of the serial device to connect to instead of "
            "using an IP connection (in the form of /dev/ttySx on cygwin/linux, "
            "and COMX in windows. Required if remote_ip_address not set)")

         ("serial_baud,b", po::value<int>(&m_serial_baud)->default_value(115200),
            "Set the baud rate of the serial device")
   ; // End of add_options()

   DBG_out << "Parsing cmdline arguments...";
   /* parse regular options */

   po::parsed_options parsed = po::parse_command_line(argc, argv, desc);
   po::store( parsed, m_vm);
   po::notify(m_vm);

   try {
      /* Check for general help request first */
      if (m_vm.count("help")) {
         stringstream ss;

         ss << desc << endl;
         ss << "To get detailed help for any command, add a --help after the command." << endl;
         CON_print(ss.str());
         EXIT_LOG_FLUSH(0);
      }

      if (m_vm.count("version")) {
         stringstream ss;
         ss << "CmdLine Client App version: " << "TODO" << endl;
         ss << "CmdLine Client Lib version: " << "TODO" << endl;
         ss << "Boost Library version: " << BOOST_LIB_VERSION << endl;
         CON_print(ss.str());
         EXIT_LOG_FLUSH(0);
      }

      /* Allow user to override the interactive (menu) mode and run in single
       * cmd mode.  This is for scripting purposes */
      if ( m_vm.count("mode") ) {
         this->m_bInteractiveRunMode = false;
         DBG_out << "Overriding interactive mode and running in single cmd mode";
      }

      /* Serial and IP connections are mutually exclusive so treat them as such
       * on the cmdline. */
      if (m_vm.count("ip_address") && !m_vm.count("serial_dev")) {

         this->m_conn_mode = _CB_EthCli;

         stringstream ss;

         ss << "UDP connection to: "
               << m_vm["ip_address"].as<string>() << ":"
               << m_vm["remote_port"].as<string>()
               << "from local port "
                  << m_vm["local_port"].as<string>() << endl;

         CON_print(ss.str());

      } else if (m_vm.count("serial_dev") && !m_vm.count("ip_address")) {
         this->m_conn_mode = _CB_Serial;

         stringstream ss;

         ss << "Serial connection on "
               << m_vm["serial_dev"].as<string>()
               << " with "
               << m_vm["serial_baud"].as<int>()
               << " baud rate";

         if (this->m_dfuse) {
            ss << " with DFUSE parameters" << endl;
         } else {
            ss << " without DFUSE parameters" << endl;
         }
         CON_print(ss.str());

      } else  {
         /* User didn't specify any connection options.  Attempt using default
          * IP address on the default port. */
         this->m_conn_mode = _CB_EthCli;
         this->m_ip_address = "172.27.0.75";
         LOG_out << "No IP address or serial device specified.";
         LOG_out << "Using default ip address "
               << this->m_ip_address << " to attempt connect connection.";
      }

      /* Clear out the argument map */
      m_parsed_args.clear();

   } catch(po::error& e) {
      string error = e.what();
      ERR_out << "Exception " << e.what() << " while parsing cmdline arguments.";
      EXIT_LOG_FLUSH(1);
   } catch (...) {
      ERR_out << "Some Unknown error occurred while parsing cmdline arguments.";
      EXIT_LOG_FLUSH(1);
   }

   return( 0 );
}

/******************************************************************************/
bool CmdlineParser::isConnEth( void )
{
   if (m_vm.count("ip_address") && !m_vm.count("serial_dev")) {
      return true;
   } else {
      return false;
   }
}

/******************************************************************************/
bool CmdlineParser::isConnSer( void )
{
   if (m_vm.count("serial_dev") && !m_vm.count("ip_address")) {
      return true;
   } else {
      return false;
   }
}

/******************************************************************************/
void CmdlineParser::getEthConnOpts(
      string& ipAddress,
      string& remPort,
      string& locPort
)
{
   ipAddress = this->m_ip_address;
   remPort = this->m_remote_port;
   locPort = this->m_local_port;
}

/******************************************************************************/
void CmdlineParser::getSerConnOpts(
      string& devName,
      int *baudRate,
      bool *dfuseFlag
)
{
   devName = this->m_serial_dev;
   *baudRate = this->m_serial_baud;
   *dfuseFlag = this->m_dfuse;
}

/******************************************************************************/
CmdlineParser::CmdlineParser( void ) :
      m_bInteractiveRunMode(true), m_dfuse(false)
{
   DBG_out << "Instantiated CmdlineParser.  Ready to parse cmdline args.";
}

/******************************************************************************/
CmdlineParser::~CmdlineParser( void )
{

}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
