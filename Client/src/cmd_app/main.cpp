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
/* System includes */
#include <cctype>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>

/* Boost includes */
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

/* App includes */
#include "Help.hpp"
#include "Logging.hpp"
#include "Callbacks.hpp"
#include "EnumMaps.hpp"
#include "Utils.hpp"
#include "Menu.hpp"
#include "Cmds.hpp"
#include "ArgParse.hpp"

/* Lib includes */
#include "CBSharedDbgLevels.h"
#include "ClientModules.h"
#include "LogStub.h"
#include "ClientApi.h"

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
   // Keep track of status of the API responses
   APIError_t status = API_ERR_NONE;

   string appName = argv[0];
   unsigned found = appName.find_last_of("/\\");
   appName = appName.substr(found+1);
   LOG_out << "Starting " << appName;

   // 1. Create a new LogStub instance.  This allows setting of logging
   // callbacks to the rest of the client library.  Safe to do this without
   // try/catch since no exceptions are thrown from library.
    LogStub *pLogStub = new LogStub();

   // 2. Set the callback functions which allow the library to log to this
   // cmdline client.
   status = pLogStub->setLibLogCallBack( CLI_LibLogCallback );
   if ( API_ERR_NONE != status ) {
      ERR_out << "Failed to set library logging callback function. Exiting...";
      EXIT_LOG_FLUSH(1);
   }

   status = pLogStub->setDC3LogCallBack( CLI_DC3LogCallback );
   if ( API_ERR_NONE != status ) {
      ERR_out << "Failed to set DC3 logging callback function. Exiting...";
      EXIT_LOG_FLUSH(1);
   }

   // 3. Enable and disable logging for various modules in the library
   pLogStub->enableLogForAllLibModules();
   pLogStub->disableLogForLibModule(MODULE_SER);       // Disable serial logging
   pLogStub->disableLogForLibModule(MODULE_MGR);      // Disable MainMgr logging
   LOG_setDbgLvl(DBG); /* Set logging level */

   // 4. Set up the client api and initialize its logging.  Users can call the
   // constructor that also sets up the communication at the same time but if
   // something goes wrong and an exception is thrown, the program will log it
   // but attempt to continue since exceptions can't be thrown across dll
   // boundaries. This way is safer because it allows for explicit error
   // checking via error codes from class functions.
   ClientApi *client = new ClientApi( pLogStub );

   // 5. Set callbacks for Req, Ack, and Done msg types.
   if ( API_ERR_NONE != (status = client->setReqCallBack(CLI_ReqCallback)) ) {
      WRN_out << "Failed to set library callback function for Req msgs.";
   }

   if ( API_ERR_NONE != (status = client->setAckCallBack(CLI_AckCallback)) ) {
      WRN_out << "Failed to set library callback function for Ack msgs.";
   }

   if ( API_ERR_NONE != (status = client->setDoneCallBack(CLI_DoneCallback)) ) {
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
         // Common options always required
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

         // Options only required if running in non-interactive mode
         ("flash", po::value<vector<string>>(&m_command)->multitoken(),
            "Flash FW to the DC3"
            "Example: --flash file=../some/path/DC3Appl.bin type=Application"
            "Example: --flash file=../some/path/DC3Boot.bin type=Bootloader")

         ("get_mode", po::value<vector<string>>(&m_command)->zero_tokens(),
            "Get current operating mode of the DC3. (Bootloader or Application) "
            "Example: --get_mode ")

         ("set_mode", po::value<vector<string>>(&m_command),
            "Set current operating mode of the DC3. (Bootloader or Application) "
            "Example: --set_mode Application "
            "Example: --set_mode Bootloader ")

         ("ram_test", po::value<vector<string>>(&m_command)->zero_tokens(),
            "Run a test of the external RAM on the DC3."
            "Example: --ram_test ")

         ("read_i2c", po::value<vector<string>>(&m_command)->multitoken(),
            "Read data from an I2C device."
            "Example: --read_i2c dev=EEPROM bytes=3 start=0 "
            "Example: --read_i2c dev=EUIROM bytes=8 start=0 "
            "Example: --read_i2c dev=SNROM  bytes=6 start=0 {acc=QPC}"
            "Example: --read_i2c dev=SNROM  bytes=6 start=0 {acc=FRT}"
            "Example: --read_i2c dev=SNROM  bytes=6 start=0 {acc=BARE}")

         ("write_i2c", po::value<vector<string>>(&m_command)->multitoken(),
            "Write data to an I2C device."
            "Example: --write_i2c dev=EEPROM bytes=3 start=0 "
            "Example: --write_i2c dev=EEPROM bytes=8 start=0 {acc=QPC} "
            "Example: --write_i2c dev=EEPROM start=0 {data='0x23 0x43 0xA0 ...'}'"
            "Example: --write_i2c dev=EEPROM start=0 {data='A0 23 4b 3A ...'} {acc=FRT}"
            "Example: --write_i2c dev=EEPROM bytes=6 start=0 {acc=BARE}")
      ; // End of add_options()

      DBG_out << "Parsing cmdline arguments...";

      // parse regular options
      po::parsed_options parsed = po::parse_command_line(argc, argv, desc);
      po::store( parsed, m_vm);
      po::notify(m_vm);

      // Clear out the argument map
      m_parsed_args.clear();

      // Serial and IP connections are mutually exclusive so treat them as such
      // on the cmdline.
      if (m_vm.count("ip_address") && !m_vm.count("serial_dev")) {
         LOG_out << "UDP connection to: "
               << m_vm["ip_address"].as<string>() << ":"
               << m_vm["remote_port"].as<string>()
               << " from local port "
                  << m_vm["local_port"].as<string>() << endl;

         status = client->setNewConnection(
               m_vm["ip_address"].as<string>().c_str(),
               m_vm["remote_port"].as<string>().c_str(),
               m_vm["local_port"].as<string>().c_str()
         );

         if ( API_ERR_NONE != status ) {
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

         if ( API_ERR_NONE != status ) {
            ERR_out << "Unable to open Serial connection. Error: 0x"
                  << setfill('0') << setw(8) << hex << status << dec;
            CON_print("!!! Make sure you're using the correct serial port and nothing else is using it !!!");
            EXIT_LOG_FLUSH(1);
         }
      } else if (m_vm.count("serial_dev") && m_vm.count("ip_address")) {
         // User specified specified both connection options. Let's not allow
         // that. */
         ss.clear();
         ss << desc << endl;
         ss << "To get detailed help for any command, add a --help after the "
               "command." << endl;
         ss << "You only get ethernet OR serial device. Not both. Specify via: "
               "-i <ip address> OR -s <COMx>.  (See help above)";
         CON_print(ss.str());
         EXIT_LOG_FLUSH(1);
      }

      /**< Used for status of commands sent to the DC3 board */
      CBErrorCode statusDC3;

      // Figure out which command is being specified.  If a command is found,
      // execute and exit.  If no command is found, start the menu thread and
      // go into interactive mode.
      if (m_vm.count("get_mode")) {                   // "get_mode" cmd handling
         m_parsed_cmd = "get_mode";

         // Check for command specific help req
         ARG_checkCmdSpecificHelp( m_parsed_cmd, appName, m_vm, client->isConnected() );

         // Store status of DC3 cmd and bootmode even though we don't need it
         // since it's all parsed and handled in CMD_ functions to reduce clutter.
         statusDC3 = ERR_NONE;
         CBBootMode mode = _CB_NoBootMode;

         // No need to extract the value from the arg=value pair for this cmd.

         // All the error handling and output to console happens inside this
         // function so there's no need to do it here
         status = CMD_runGetMode(  client, &statusDC3, &mode );

      } else if (m_vm.count("set_mode")) {            // "set_mode" cmd handling
         m_parsed_cmd = "set_mode";

         // Check for command specific help req
         ARG_checkCmdSpecificHelp( m_parsed_cmd, appName, m_vm, client->isConnected() );

         // Store status of DC3 cmd and bootmode even though we don't need it
         // since it's all parsed and handled in CMD_ functions to reduce clutter.
         statusDC3 = ERR_NONE;
         CBBootMode mode = _CB_NoBootMode;

         try {                      // Extract the value from the arg=value pair
            ARG_parseEnumStr( &mode, "mode", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
         } catch (exception& e) {
            ERR_out << "Caught exception parsing arguments: " << e.what();
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         // All the error handling and output to console happens inside this
         // function so there's no need to do it here
         status = CMD_runSetMode( client, &statusDC3, mode );

      } else if (m_vm.count("flash")) {                  // "flash" cmd handling
         m_parsed_cmd = "flash";

         // Check for command specific help req
         ARG_checkCmdSpecificHelp( m_parsed_cmd, appName, m_vm, client->isConnected() );

         // Store status of DC3 cmd and bootmode even though we don't need it
         // since it's all parsed and handled in CMD_ functions to reduce clutter.
         statusDC3 = ERR_NONE;
         CBBootMode type = _CB_NoBootMode;
         string filename = "";

         try {                      // Extract the value from the arg=value pair
            ARG_parseEnumStr( &type, "type", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
         } catch (exception& e) {
            ERR_out << "Caught exception parsing arguments: " << e.what();
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         try {      // Extract and validate the filename from the arg=value pair
            ARG_parseFilenameStr( filename, "file", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
         } catch (exception& e) {
            ERR_out << "Caught exception parsing arguments: " << e.what();
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         // All the error handling and output to console happens inside this
         // function so there's no need to do it here
         status = CMD_runFlash( client, &statusDC3, type, filename );

      } else if (m_vm.count("read_i2c")) {            // "read_i2c" cmd handling
         m_parsed_cmd = "read_i2c";

         // Check for command specific help req
         ARG_checkCmdSpecificHelp( m_parsed_cmd, appName, m_vm, client->isConnected() );

         // Extract the value from the arg=value pair
         int bytes = -1, start = -1;
         CBI2CDevices dev = _CB_MaxI2CDev;
         CBAccessType acc = _CB_ACCESS_QPC;       // set to a default arg of QPC
         string value = "";

         try {                      // Extract the value from the arg=value pair
            ARG_parseEnumStr( &dev, "dev", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
            // This call passes in a default value for an optional argument
            ARG_parseEnumStr( &acc, _CB_ACCESS_QPC, "acc", m_parsed_cmd,
                  appName, m_vm[m_parsed_cmd].as<vector<string>>() );
            ARG_parseNumStr( &start, "start", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
            ARG_parseNumStr( &bytes, "bytes", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
         } catch (exception& e) {
            ERR_out << "Caught exception parsing arguments: " << e.what();
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         // Don't allow negatives in our numeric input
         if ( start < 0 ) {
            ERR_out << "Invalid start specified: " << start;
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         } else if ( bytes < 0 ) {
            ERR_out << "Invalid number of bytes specified: " << bytes;
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         DBG_out << "Issuing read_i2c cmd with start=" << start
               << " bytes=" << bytes
               << " from dev=" << enumToString(dev)
               << " with acc=" << enumToString(acc) << "...";

         size_t nMaxBufferSize = 1000;
         uint8_t *buffer = new uint8_t[nMaxBufferSize];
         uint16_t bytesRead = 0;

         status = CMD_runReadI2C( client, &statusDC3, &bytesRead, buffer,
               nMaxBufferSize, bytes, start, dev, acc );

         delete[] buffer;

      } else if (m_vm.count("write_i2c")) {           // "read_i2c" cmd handling
         m_parsed_cmd = "write_i2c";

         // Check for command specific help req
         ARG_checkCmdSpecificHelp( m_parsed_cmd, appName, m_vm, client->isConnected() );

         // Extract the value from the arg=value pair
         int bytes = -1, start = -1;
         CBI2CDevices dev = _CB_MaxI2CDev;
         CBAccessType acc = _CB_ACCESS_QPC;       // set to a default arg of QPC

         string value = "";

         try {                      // Extract the value from the arg=value pair
            ARG_parseEnumStr( &dev, "dev", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
            // This call passes in a default value for an optional argument
            ARG_parseEnumStr( &acc, _CB_ACCESS_QPC, "acc", m_parsed_cmd,
                  appName, m_vm[m_parsed_cmd].as<vector<string>>() );
            // This call passes in a default value for an optional argument
            ARG_parseNumStr( &start, "0", "start", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
            // This call passes in a default value for an optional argument
            ARG_parseNumStr( &bytes, "5", "bytes", m_parsed_cmd, appName,
                  m_vm[m_parsed_cmd].as<vector<string>>() );
         } catch (exception& e) {
            ERR_out << "Caught exception parsing arguments: " << e.what();
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         // Don't allow negatives in our numeric input
         if ( start < 0 ) {
            ERR_out << "Invalid start specified: " << start;
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         } else if ( bytes < 0 ) {
            ERR_out << "Invalid number of bytes specified: " << bytes;
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         // create a default array and fill it number of bytes user requested
         size_t maxArraySize = 1000;
         uint8_t *data = new uint8_t[maxArraySize];

         // form the default array of the length user wants but still have our
         // code parse it instead of just making the data array directly.  This
         // allows the same code path to be taken for both default and user
         // arrays.
         string defaultArrayStr = "[";
         stringstream ss_tmp;
         for (int i = 0; i < bytes; i++ ) {
            ss_tmp << "0x" << hex << setfill('0') << setw(2) << unsigned(i);
            if (i < (bytes - 1) ) {
               ss_tmp << ",";     // append a comma unless it's the last element
            }
         }
         defaultArrayStr.append(ss_tmp.str());
         defaultArrayStr.append("]");

         size_t dataLen = 0;
         try {
            ARG_parseHexArr( data, &dataLen, maxArraySize, defaultArrayStr, "data", m_parsed_cmd,
                  appName, m_vm[m_parsed_cmd].as<vector<string>>() );
         } catch (exception &e) {
            ERR_out << "Caught exception parsing arguments: " << e.what();
            HELP_printCmdSpecific( m_parsed_cmd, appName );
         }

         bytes = dataLen;

         stringstream ss_data_out;
         for (int i = 0; i < bytes; i++ ) {
            ss_data_out << "0x" << hex << setfill('0') << setw(2) << unsigned(data[i]);
            if (i < (bytes - 1) ) {
               ss_data_out << ",";     // append a comma unless it's the last element
            }
         }

         DBG_out << "Issuing write_i2c cmd with start=" << start
               << " bytes=" << bytes
               << " to dev=" << enumToString(dev)
               << " with acc=" << enumToString(acc)
               << " data={" << ss_data_out.str() << "} ...";


         status = CMD_runWriteI2C( client, &statusDC3, data, bytes, start, dev, acc );

         delete[] data;

      } else if (m_vm.count("ram_test")) {            // "ram_test" cmd handling
         m_parsed_cmd = "ram_test";

         // Check for command specific help req
         ARG_checkCmdSpecificHelp( m_parsed_cmd, appName, m_vm, client->isConnected() );

         // No need to extract the value from the arg=value pair for this cmd.

         // Execute (and block) on this command
         status = CMD_runRamTest( client );
      }

      // Now check if the user requested general help.  This has to be done
      // after checking for commands.  This allows checking for user requesting
      // help for a specific command and this general help won't override.
      else if (m_vm.count("help")) {
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
      else {                  // end of checking for cmdline requested commands.
         // If we are here, no commands were requested so run the menu and let
         // the user interactively choose what they want to do.
         status = MENU_run( client );
      }

   } catch(po::error& e) {
      string error = e.what();
      ERR_out << "Exception " << e.what() << " while parsing cmdline arguments.";
      EXIT_LOG_FLUSH(1);
   } catch ( exception& ex) {
      ERR_out << "Some Unknown error occurred while parsing cmdline arguments: " << ex.what();
      EXIT_LOG_FLUSH(1);
   }

   // Tell the client to stop and wait for it */
   DBG_out << "Exiting with client status 0x"
         << setw(8) << setfill('0') << hex << status << dec;
   EXIT_LOG_FLUSH( status );
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
