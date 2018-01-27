/**
 * @file    Help.cpp
 * Contains all the extended help for all commands so they don't take up space
 * in main.cpp.
 *
 * @date    04/24/2015
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2015 Harry Rostovtsev. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
/* App includes */
#include "Help.hpp"
#include "Logging.hpp"
#include "EnumMaps.hpp"
#include "CliDbgModules.hpp"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
CLI_MODULE_NAME( CLI_DBG_MODULE_HLP );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define CHARS_PER_LINE 80     /**<! Max characters to print per line for help */

/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
void HELP_printCmdSpecific(
      const std::string& parsed_cmd,
      const std::string& appName
)
{
   /* String vars to use for detailed help */
   string description;
   string prototype;
   string example;
   vector<string> cmd_arg_options;
   /* Yeah, it's a little annoying that you can't just append streams like the
    * rest of the strings.  I'll eventually figure out how to properly implement
    * a template + operator in the Enum Maps but until then, this is how you do
    * it.*/
   stringstream ss_params;

   if( 0 == parsed_cmd.compare("get_mode") ) {              // get_mode cmd help
      description = parsed_cmd + " command sends a request to the DC3 to get "
            "its current boot mode. The command returns the status of the "
            "request and the current boot mode. The possible return for: "
            "status: 0x00000000 for success or other codes for failure.\n"
            "bootmode: \n";
      ss_params << "[" << enumToString(_DC3_NoBootMode) << "|"
            << enumToString(_DC3_SysRomBoot) << "|"
            << enumToString(_DC3_Bootloader) << "|"
            << enumToString(_DC3_Application) << "]";
      description += ss_params.str();

      cmd_arg_options.push_back(" * None");

      prototype = appName + " [connection options] --" + parsed_cmd;
      example = appName + " -i 207.27.0.75 --" + parsed_cmd;
   } else  if( 0 == parsed_cmd.compare("set_mode") ) {      // set_mode cmd help
      description = parsed_cmd + " command sends a request to the DC3 to set "
            "its current boot mode.";
      description +=
            "The command returns the status of the "
            "request. The possible return for: "
            "status: 0x00000000 for success or other codes for failure.\n";

      ss_params.str(string());
      ss_params <<" * [mode=" << enumToString(_DC3_Bootloader) << "|"
                << enumToString(_DC3_Application) << "]";
      cmd_arg_options.push_back(ss_params.str());

      cmd_arg_options.push_back(" --- Note that instead of typing out the entire "
            "word, you can pretty safely use abbreviations. For example, instead "
            "of 'Application', you can type something as short as 'app' or 'a'. "
            "Capitalization doesn't matter.");


      prototype = appName + " [connection options] --" + parsed_cmd + "[mode=";
      prototype += enumToString(_DC3_Application);
      prototype += "|";
      prototype += enumToString(_DC3_Bootloader);
      prototype += "]";

      example = appName + " -i 207.27.0.75 --" + parsed_cmd + " mode=";
      example += enumToString(_DC3_Application);
   } else if (  0 == parsed_cmd.compare("flash") ) {           // flash cmd help
      description = parsed_cmd + " command initiates a FW upgrade on the DC3 "
            "board. You have to specify the location of the FW upgrade *bin "
            "file and which FW image is going to be upgraded. File can be "
            "specified using a relative path to " + appName + " or a full path. "
            "The options for the FW image type are: ";

      ss_params.str(string());
      ss_params << " * [type=" << enumToString(_DC3_Application) << "]";
      cmd_arg_options.push_back(ss_params.str());

      cmd_arg_options.push_back(" --- Note that instead of typing out the entire word, you "
            "can pretty safely use abbreviations. Instead of Application, you can "
            "type something as short as 'app' or 'a'. Capitalization doesn't matter.");

      cmd_arg_options.push_back(" * [file=path/to/filename.bin]");
      cmd_arg_options.push_back(" --- Note that the filename must follow the "
            "format of DC3<Name>_vYY.ZZ_YYYYMMDDhhmmss.bin because the Bootloader "
            "expects the version (vYY.ZZ) and build datetime "
            "(YYYYMMDDhhmmss) to be in the filename.");


      prototype = appName + " [connection options] --" + parsed_cmd
            + " [file=<relative path to *.bin file>] " + "[type=";
      prototype += enumToString(_DC3_Application);
      prototype += "|";
      prototype += enumToString(_DC3_Bootloader);
      prototype += "]";

      example = appName + " -i 207.27.0.75 --" + parsed_cmd +
            " file=../../DC3Appl_v01.03_20150715142540.bin " + "type=";
      example += enumToString(_DC3_Application);
   } else if (  0 == parsed_cmd.compare("read_i2c") ) {     // read_i2c cmd help
      description = parsed_cmd + " command initiates an I2C device read. You "
            "have to specify the I2C device (dev=), number of bytes to read "
            "(bytes=), and an offset from the start of device memory (start=). ";

      ss_params.str(string());
      ss_params << "* [dev=<" << enumToString(_DC3_EEPROM)
                << "|" << enumToString(_DC3_SNROM)
                << "|" << enumToString(_DC3_EUIROM) << ">]";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_EEPROM) << " is a 256 byte R/W "
            "device that contains a database of various settings stored in a "
            "binary format. Some settings include an IP address last assigned "
            "to the DC3 and the version and build datetime of the Bootloader FW "
            "image.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_SNROM) << " is a 16 byte RO "
            "device that contains a unique 64 bit serial number.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_EUIROM) << " is an 8 byte RO "
            "device that contains a unique 48 bit number that is used as the MAC "
            "address for the DC3.";
      cmd_arg_options.push_back(ss_params.str());

      cmd_arg_options.push_back("* [start=<0 - N>] where N depends on the device.");
      cmd_arg_options.push_back("* {bytes=<1 - N>} where N depends on the device and "
            "'start' param. Note that this parameter is optional if 'data=' parameter "
            "is specified. If 'data=' is not specified, 'bytes=' should be specified.");

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_EEPROM) << " max start is 255 (0xFF)";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_SNROM) << " max start is 15 (0x0F)";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_EUIROM) << " max start is 7 (0x07)";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << " start + bytes should be less than the maximum "
            "storage size of the devices";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << "* {acc=<(" << enumToString(_DC3_ACCESS_QPC) << ")"
                << "|" << enumToString(_DC3_ACCESS_FRT)
                << "|" << enumToString(_DC3_ACCESS_BARE) << ">}";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_ACCESS_QPC) << " - data is accessed "
            "via the QPC event driven framework.  This is the default option.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_ACCESS_FRT) << " - data is accessed "
            "via the FreeRTOS events. This option is only available when the DC3 "
            "is in " << enumToString(_DC3_Application) << " mode.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_ACCESS_FRT) << " - data is accessed "
            "via bare-metal, bypassing all event interfaces except for the delivery "
            "to and from the active object that handles that particular HW "
            "interface. This option should only be used for debugging.";
      cmd_arg_options.push_back(ss_params.str());


      prototype = appName + " [connection options] --" + parsed_cmd
            + " [start=<0 - 128>] {bytes=<0-128>}";
      ss_params.str(string());
      ss_params << "[dev=<" << enumToString(_DC3_EEPROM)
                << "|" << enumToString(_DC3_SNROM)
                << "|" << enumToString(_DC3_EUIROM) << ">]";
      prototype += ss_params.str();
      prototype += " ";
      ss_params.str(string());
      ss_params << "{acc=<(" << enumToString(_DC3_ACCESS_QPC) << ")"
                << "|" << enumToString(_DC3_ACCESS_FRT)
                << "|" << enumToString(_DC3_ACCESS_BARE) << ">}";
      prototype += ss_params.str();

      example = appName + " -i 207.27.0.75 --" + parsed_cmd +
            " start=0 bytes=20 " + "dev=";
      example += enumToString(_DC3_EEPROM);

   } else if (  0 == parsed_cmd.compare("write_i2c") ) {   // write_i2c cmd help

      description = parsed_cmd + " command initiates an I2C device write. You "
            "have to specify the I2C device (dev=), number of bytes to read "
            "(bytes=), and an offset from the start of device memory (start=). "
            "You can also optionally specify the data to write. If you do not, "
            "an array of increasing numbers will be automatically generated and "
            "used.";

      ss_params.str(string());
      ss_params << "* [dev=<" << enumToString(_DC3_EEPROM)<< ">]";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_EEPROM) << " is a 256 byte R/W "
            "device that contains a database of various settings stored in a "
            "binary format. Some settings include an IP address last assigned "
            "to the DC3 and the version and build datetime of the Bootloader FW "
            "image.";
      cmd_arg_options.push_back(ss_params.str());

      cmd_arg_options.push_back("* [start=<0 - N>] where N depends on the device.");
      cmd_arg_options.push_back("* {bytes=<1 - N>} where N depends on the device and "
            "'start' param. Note that this parameter is optional if 'data=' parameter "
            "is specified. If 'data=' is not specified, 'bytes=' should be specified.");

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_EEPROM) << " max start is 255 (0xFF)";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << " start + bytes should be less than the maximum "
            "storage size of the devices";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << "* {acc=<(" << enumToString(_DC3_ACCESS_QPC) << ")"
                << "|" << enumToString(_DC3_ACCESS_FRT)
                << "|" << enumToString(_DC3_ACCESS_BARE) << ">}";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_ACCESS_QPC) << " - data is accessed "
            "via the QPC event driven framework.  This is the default option.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_ACCESS_FRT) << " - data is accessed "
            "via the FreeRTOS events. This option is only available when the DC3 "
            "is in " << enumToString(_DC3_Application) << " mode.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << " --- " << enumToString(_DC3_ACCESS_FRT) << " - data is accessed "
            "via bare-metal, bypassing all event interfaces except for the delivery "
            "to and from the active object that handles that particular HW "
            "interface. This option should only be used for debugging.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << "* {data=\"0x00,0x05 0x34; 0xab 32 \"}";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << "--- the optional data should be formatted as follows: ";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << "~~~~ The array should be surrounded by '' or \"\".  [] can "
            "also be used but spaces cannot be used as separators then.  \"\" and "
            "'' are the safe options.";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << "~~~~ Numbers can be separated by , ; . or spaces if surrounded by '' or \"\"";
      cmd_arg_options.push_back(ss_params.str());

      ss_params.str(string());
      ss_params << "~~~~ Numbers can be specified to be hex (prepend 0x) or dec";
      cmd_arg_options.push_back(ss_params.str());

      prototype = appName + " [connection options] --" + parsed_cmd
            + " [start=<0 - 128>] {bytes=<0-128>}";
      ss_params.str(string());
      ss_params << "[dev=<" << enumToString(_DC3_EEPROM) << ">]";
      prototype += ss_params.str();
      prototype += " ";
      ss_params.str(string());
      ss_params << "{acc=<(" << enumToString(_DC3_ACCESS_QPC) << ")"
                << "|" << enumToString(_DC3_ACCESS_FRT)
                << "|" << enumToString(_DC3_ACCESS_BARE) << ">}";
      prototype += ss_params.str();
      prototype += "{data=\"<0-0xFF>,<0-0xFF>,<0-0xFF>,<0-0xFF>, ... ,<0-0xFF>\"}";

      example = appName + " -i 207.27.0.75 --" + parsed_cmd +
            " start=0 bytes=4 " + "dev=";
      example += enumToString(_DC3_EEPROM);
      example += " data=\"0x00,0x01,0x02,0x03\"";

   } else if( 0 == parsed_cmd.compare("ram_test") ) {       // ram_test cmd help
      description = parsed_cmd + " command sends a request to the DC3 to run "
            "a test on the external RAM. The RAM test checks the integrity of "
            "the address lines, the data lines, and finally, the integrity of "
            "the RAM chips themselves. This is a long test and may take a "
            "while. The command returns the status of the ram test and if any "
            "failures occurred, the test which failed and the address at which "
            "the failure occurred. The possible return for: "
            "status: 0x00000000 for success or other codes for failure.\n"
            "addr: 0x00000000 if no failures and hex value of the memory "
            "address the error occurred at. \n"
            "test: ";
      ss_params << "[" << enumToString(_DC3_RAM_TEST_DATA_BUS) << "|"
            << enumToString(_DC3_RAM_TEST_ADDR_BUS) << "|"
            << enumToString(_DC3_RAM_TEST_DEV_INT) << "]";
      description += ss_params.str();
      prototype = appName + " [connection options] --" + parsed_cmd;
      example = appName + " -i 207.27.0.75 --" + parsed_cmd;
   } else {
      ERR_out << "Unable to find cmd specific help for " << parsed_cmd;
      EXIT_LOG_FLUSH(0);
   }

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

   ss << "Command argument(s) and option(s): " << endl;
   ss << "Arguments surrounded in: " << endl
         << "[] are required" << endl
         << "{} are optional" << endl;
   ss << "Argument values surrounded in: " << endl
         << "<> are allowed options" << endl
         << "() are the default if none is selected" << endl
         << "\"\" are string types that may contain spaces" << endl
         << "Argument values separated by | indicate all the allowed options" << endl;

   // iterate over the elements of the vector
   for( vector<string>::iterator v_itr = cmd_arg_options.begin(); v_itr != cmd_arg_options.end(); ++v_itr ) {
      std::copy((*v_itr).begin(), (*v_itr).end(),
         ff_ostream_iterator(ss, "", CHARS_PER_LINE));
         ss << endl;
   }
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
   EXIT_LOG_FLUSH(0);
}

/******************************************************************************/
void CMD_tokenize(
      const std::string& str,
      std::vector<string>& tokens,
      const std::string& delims
)
{
   // Skip delimiters at beginning.
   string::size_type lastPos = str.find_first_not_of(delims, 0);
   // Find first "non-delimiter".
   string::size_type pos     = str.find_first_of(delims, lastPos);

   while (string::npos != pos || string::npos != lastPos) {
      // Foundtoken, add to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));

      // Skip delimiters.
      lastPos = str.find_first_not_of(delims, pos);

      // Find next "non-delimiter"
      pos = str.find_first_of(delims, lastPos);
   }
}

/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Harry Rostovtsev. All rights reserved *****END OF FILE****/
