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
#include <cctype>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

/* Local App includes */
//#include "CmdlineParser.h"
#include "Help.h"
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

namespace po = boost::program_options;
using namespace po;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define MAX_FW_IMAGE_SIZE 1500000
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
   pLogStub->disableLogForLibModule(MODULE_SER);    /* Disable serial logging */
   pLogStub->disableLogForLibModule(MODULE_MGR);   /* Disable MainMgr logging */
   LOG_setDbgLvl(DBG); /* Set logging level */

   /* 4. Set up the client api and initialize its logging.  Users can call the
    * constructor that also sets up the communication at the same time but if
    * something goes wrong and an exception is thrown, the program will log it
    * but attempt to continue since exceptions can't be thrown across dll
    * boundaries. This way is safer because it allows for explicit error
    * checking via error codes from class functions. */
   ClientApi *client = new ClientApi( pLogStub );

   /* 5. Set callbacks for Req, Ack, and Done msg types. */
   if ( CLI_ERR_NONE != (status = client->setReqCallBack(CLI_ReqCallback)) ) {
      WRN_out << "Failed to set library callback function for Req msgs.";
   }

   if ( CLI_ERR_NONE != (status = client->setAckCallBack(CLI_AckCallback)) ) {
      WRN_out << "Failed to set library callback function for Ack msgs.";
   }

   if ( CLI_ERR_NONE != (status = client->setDoneCallBack(CLI_DoneCallback)) ) {
      WRN_out << "Failed to set library callback function for Done msgs.";
   }

   bool    m_dfuse = false;         /**< Serial will be used for DFU commands */

   std::vector<string> m_command;   /**< Vector that will hold the command along
                                   with optional (ordered) arguments in the rest
                                   of  the indexes. */
   string  m_parsed_cmd;           /**< String containing the parsed command */

   std::map<string, string> m_parsed_args;    /**< Map containing parsed args */
   po::variables_map m_vm;                      /**< boost program options vm */

   /* Storage for various args collected from cmdline */
   string  m_ip_address;                        /**< IP address to connect to */
   string  m_remote_port;                          /**< IP port to connect to */
   string  m_local_port;                         /**< IP port to connect from */
   string  m_serial_dev;                     /**< serial device to connect to */
   int     m_serial_baud;                  /**< serial device baudrate to use */
   po::options_description desc("Global options");

   stringstream ss;        /**< Stream for some helpful comments to help user */

   try {
      desc.add_options()
         /* Common options always required */
         ("help,h", "produce help message")
         ("version,v", "print version of client") // TODO: not implemented

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
         ("flash", po::value<vector<string>>(&m_command)->multitoken(),
            "Flash FW to the DC3"
            "Example: '--flash file=../some/path/DC3Appl.bin fw=DC3Appl'"
            "Example: '--flash file=../some/path/DC3Boot.bin fw=DC3Boot'")

         ("get_mode", po::value<vector<string>>(&m_command)->zero_tokens(),
            "Get current operating mode of the DC3. (Bootloader or Application) "
            "Example: '--get_mode' ")
      ; // End of add_options()

      DBG_out << "Parsing cmdline arguments...";
      /* parse regular options */

      po::parsed_options parsed = po::parse_command_line(argc, argv, desc);
      po::store( parsed, m_vm);
      po::notify(m_vm);

      /* Clear out the argument map */
      m_parsed_args.clear();

      /* Serial and IP connections are mutually exclusive so treat them as such
       * on the cmdline. */
      if (m_vm.count("ip_address") && !m_vm.count("serial_dev")) {
         LOG_out << "UDP connection to: "
               << m_vm["ip_address"].as<string>() << ":"
               << m_vm["remote_port"].as<string>()
               << "from local port "
                  << m_vm["local_port"].as<string>() << endl;

         status = client->setNewConnection(
               m_vm["ip_address"].as<string>().c_str(),
               m_vm["remote_port"].as<string>().c_str(),
               m_vm["local_port"].as<string>().c_str()
         );

         if ( CLI_ERR_NONE != status ) {
            ERR_out << "Unable to open UDP connection. Error: 0x"
                  << setfill('0') << setw(8) << hex << status;
            EXIT_LOG_FLUSH(1);
         }
      } else if (m_vm.count("serial_dev") && !m_vm.count("ip_address")) {
         LOG_out << "Serial connection on "
               << m_vm["serial_dev"].as<string>()
               << " with "
               << m_vm["serial_baud"].as<int>()
               << " baud rate"
               << ((m_dfuse == false) ? "without" : "with") << "DFUSE";

         status = client->setNewConnection(
               m_vm["serial_dev"].as<string>().c_str(),
               m_vm["serial_baud"].as<int>(),
               m_dfuse
         );

         if ( CLI_ERR_NONE != status ) {
            ERR_out << "Unable to open Serial connection. Error: 0x"
                  << setfill('0') << setw(8) << hex << status << dec;
            CON_print("!!! Make sure you're using the correct serial port and nothing else is using it !!!");
            EXIT_LOG_FLUSH(1);
         }
      } else if (m_vm.count("serial_dev") && m_vm.count("ip_address")) {
         /* User specified specified both connection options. Let's not allow
          * that. */
         ss.clear();
         ss << desc << endl;
         ss << "To get detailed help for any command, add a --help after the "
               "command." << endl;
         ss << "You only get ethernet OR serial device. Not both. Specify via: "
               "-i <ip address> OR -s <COMx>.  (See help above)";
         CON_print(ss.str());
         EXIT_LOG_FLUSH(1);
      } else  {
         /* User didn't specify any connection options. */
         ss.clear();
         ss << desc << endl;
         ss << "To get detailed help for any command, add a --help after the "
               "command." << endl;
         ss << "No IP address or serial device specified.  Please specify using"
               "-i <ip address> or -s <COMx>.  (See help above)";
         CON_print(ss.str());
         EXIT_LOG_FLUSH(1);
      }

      /**< Used for status of commands sent to the DC3 board */
      CBErrorCode statusDC3;

      /* Figure out which command is being specified.  If a command is found,
       * execute and exit.  If no command is found, start the menu thread and
       * go into interactive mode. */
      if (m_vm.count("get_mode")) {                /* "get_mode" cmd handling */
         m_parsed_cmd = "get_mode";

         if (m_vm.count("help")) {     /* Check for command specific help req */
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         CBBootMode mode = _CB_NoBootMode;       /**< Store the bootmode here */

         /* Execute (and block) on this command */
         if( CLI_ERR_NONE == (status = client->DC3_getMode(&statusDC3, &mode))) {
            ss.clear();
            ss << "Got back DC3 bootmode: " << enumToString(mode);
            if (ERR_NONE == statusDC3) {
               ss << " with no errors.";
            } else {
               ss << " with ERROR: 0x" << setw(8) << setfill('0') << hex << statusDC3 << dec;
            }
            CON_print(ss.str());
         } else {
            ERR_out << "Got DC3 error " << "0x" << std::hex
               << status << std::dec << " when trying to get bootmode.";
         }
      } else if (m_vm.count("flash")) {               /* "flash" cmd handling */
         m_parsed_cmd = "flash";

         vector<string>::const_iterator begin = m_vm["flash"].as<vector<string>>().begin();
         vector<string>::const_iterator end = m_vm["flash"].as<vector<string>>().end();
         vector<string>::const_iterator itr;
          /* Check for command specific help req */
         if( find(begin, end, "--help") != end || find(begin, end, "help") != end ) {
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         CBBootMode type = _CB_NoBootMode;
         string filename = "";
         for( itr = begin; itr != end; ++itr ) {
            if ( (*itr).find("=") ) {
               vector<string> arg_pair;

               CMD_tokenize( (*itr), arg_pair, "=" );
               if (2 != arg_pair.size()) {              /* invalid arg format */
                  ERR_out << "ERROR: found a = sign in argument but wrong number of params";
                  HELP_printCmdSpecific( m_parsed_cmd, appName );
               } else if ( 0 == arg_pair.at(0).compare("file")) { /* file arg */
                  DBG_out << "Attempting to find file: " << arg_pair.at(1);

                  ifstream fw_file (      /* open file stream to read in file */
                        arg_pair.at(1).c_str(),
                        ios::in | ios::binary | ios::ate
                  );

                  if (fw_file) {               /* if the file exists, read it */
                     DBG_out << "File exists: " << arg_pair.at(1);
                     filename = arg_pair.at(1);
                     fw_file.close();                           /* Close file */
                  } else {
                     ERR_out << "Unable to open file " << arg_pair.at(1);
                     HELP_printCmdSpecific( m_parsed_cmd, appName );
                  }

               } else if ( 0 == arg_pair.at(0).compare("type")) { /* type arg */
                  DBG_out << "FW image type specified as: " << arg_pair.at(1);
                  if ( 0 == arg_pair.at(1).compare("Bootloader")) {
                     //TODO: either implement or remove option.
                     ERR_out << "Bootloader flashing is unimplemented currently";
                     HELP_printCmdSpecific( m_parsed_cmd, appName );
                  } else if ( 0 == arg_pair.at(1).compare("Application")) {
                     type = _CB_Application;
                  } else {
                     ERR_out << "Unknown type of FW image specified: " << arg_pair.at(1);
                     HELP_printCmdSpecific( m_parsed_cmd, appName );
                  }
               }
            }
         }

         /* Check if everything has been parsed correctly */
         if ( _CB_NoBootMode == type || 0 == filename.compare("") ) {
            ERR_out << "Failed to parse args. ";
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         /* If we got here, we have a valid filename/path and valid FW image
          * type. Execute (and block) on this command */
         status = client->DC3_flashFW(&statusDC3, type, filename.c_str());
         if( CLI_ERR_NONE == status) {
            ss.clear();
            ss << "FW flashing of DC3 ";
            if (ERR_NONE == statusDC3) {
               ss << "completed successfully.";
            } else {
               ss << "FAILED with ERROR: 0x" << setw(8) << setfill('0') << hex << statusDC3 << dec;
            }
            CON_print(ss.str());
         } else {
            ERR_out << "Got client error " << "0x" << std::hex
               << status << std::dec << " when trying to flash FW.";
         }
      }

      else { // end of checking for cmdline requested commands.
         /* If we are here, no commands were requested so */
         ERR_out << "TODO: implement menu here";
         EXIT_LOG_FLUSH(0);
      }
      /* Now check if the user requested general help */
      if (m_vm.count("help")) {
         stringstream ss;

         ss << desc << endl;
         ss << "To get detailed help for any command, add a --help after the command." << endl;
         CON_print(ss.str());
         EXIT_LOG_FLUSH(0);
      } else if (m_vm.count("version")) {
         stringstream ss;
         ss << "CmdLine Client App version: " << "TODO" << endl;
         ss << "CmdLine Client Lib version: " << "TODO" << endl;
         ss << "Boost Library version: " << BOOST_LIB_VERSION << endl;
         CON_print(ss.str());
         EXIT_LOG_FLUSH(0);
      }

   } catch(po::error& e) {
      string error = e.what();
      ERR_out << "Exception " << e.what() << " while parsing cmdline arguments.";
      EXIT_LOG_FLUSH(1);
   } catch (...) {
      ERR_out << "Some Unknown error occurred while parsing cmdline arguments.";
      EXIT_LOG_FLUSH(1);
   }

   /* Tell the client to stop and wait for it */
   DBG_out << "Exiting with client status 0x"
         << setw(8) << setfill('0') << hex << status << dec;
   EXIT_LOG_FLUSH( status );
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
