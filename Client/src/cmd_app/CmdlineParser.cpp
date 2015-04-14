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
#define CHARS_PER_LINE 80     /**<! Max characters to print per line for help */

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
void printCmdSpecificHelp(  const std::string& description,
                            const std::string& prototype,
                            const std::string& example)
{
   stringstream ss;
   ss << endl << endl;
   for (uint8_t i=0; i < CHARS_PER_LINE; i++) {
      ss << "=";
   }
   ss << endl;
   ss << "Command Description: " << endl;
   std::copy(description.begin(), description.end(),
         ff_ostream_iterator(ss, "", CHARS_PER_LINE));
   ss << endl << endl;
   ss << "Command Prototype(s): " << endl;
   std::copy(prototype.begin(), prototype.end(),
         ff_ostream_iterator(ss, "", CHARS_PER_LINE));
   ss << endl << endl;

   ss << "Command Example(s): " << endl;
   std::copy(example.begin(), example.end(),
         ff_ostream_iterator(ss, "", CHARS_PER_LINE));
   ss << endl;
   for (uint8_t i=0; i < CHARS_PER_LINE; i++) {
      ss << "=";
   }
   ss << endl;

   CON_print(ss.str());

   DBG_out << "printCmdSpecificHelp requested";
}
/* Private class prototypes --------------------------------------------------*/
/* Private class methods -----------------------------------------------------*/
/******************************************************************************/
int CmdlineParser::parse( int argc, char** argv )
{
   std::string appName = argv[0];               /* Store the application name */
   unsigned found = appName.find_last_of("/\\");
   appName = appName.substr(found+1);

   /* Strin vars to use for detailed help */
   string description;
   string prototype;
   string example;

   po::options_description desc("Allowed options");

      try {
      desc.add_options()
            /* Common options always required */
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

            /* Options only required if running in non-interactive mode */
            ("flash_fw", po::value<vector<string>>(&m_command)->multitoken(),
               "Flash FW on the DC3"
               "Example: '--flash_fw file=../some/path/DC3Appl.bin fw=DC3Appl'"
               "Example: '--flash_fw file=../some/path/DC3Boot.bin fw=DC3Boot'")

            ("get_mode", po::value<vector<string>>(&m_command)->zero_tokens(),
               "Get current operating mode of the DC3. (Bootloader or Application) "
               "Example: '--get_mode' ")
      ; // End of add_options()

      DBG_out << "Parsing cmdline arguments...";
      /* parse regular options */

      po::parsed_options parsed = po::parse_command_line(argc, argv, desc);
      po::store( parsed, m_vm);
      po::notify(m_vm);

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


      /* Clear out the argument map */
      m_parsed_args.clear();

      /* Figure out which command is being specified */
      if (m_vm.count("get_mode")) {
         m_parsed_cmd = "get_mode";
         DBG_out << "get_mode";

         for(uint8_t i = 0; i < m_vm["get_mode"].as< vector<string> >().size(); i++) {
            DBG_out << m_vm["get_mode"].as< vector<string> >()[i];
            if (std::string::npos != m_vm["get_mode"].as< vector<string> >()[i].find("help")) {
               DBG_out << "get_mode help requested";
               /* This is a dumb workaround for multitoken since the --help switch appears as one of the
                * arguments.  You have to do this anytime you are not using a zero_tokens() in the cmdline*/
               description = m_parsed_cmd + " command sends a request to the "
                     "DC3 to get its current operating mode.  The "
                     "possible return values are ";
               prototype = appName + " [connection options] --" + m_parsed_cmd;
               example = appName + " -i 192.168.1.75 --" + m_parsed_cmd;

               printCmdSpecificHelp(description, prototype, example);
               EXIT_LOG_FLUSH(0);
            }
         }






//         /* Check if command specific help was requested */
//         if (m_vm.count("help")) {
//            DBG_out << "get_mode help requested";
//            string description = m_parsed_cmd + " command sends a request to the "
//                  "DC3 to get its current operating mode.  The "
//                  "possible return values are ";
//            string prototype = appName + " [connection options] --" + m_parsed_cmd;
//            string example = appName + " -i 192.168.1.75 --" + m_parsed_cmd;
//
//            printCmdSpecificHelp(description, prototype, example);
//            EXIT_LOG_FLUSH(0);
//         }

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


      /* Allow user to override the interactive (menu) mode and run in single
       * cmd mode.  This is for scripting purposes */
      if ( m_vm.count("mode") ) {
         this->m_bInteractiveRunMode = false;
         DBG_out << "Overriding interactive mode and running in single cmd mode";

      }



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
