/**
 * @file    Help.cpp
 * Contains all the extended help for all commands so they don't take up space
 * in main.cpp.
 *
 * @date    04/24/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
/* App includes */
#include "Help.hpp"
#include "Logging.hpp"
#include "EnumMaps.hpp"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

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
void HELP_printCmdSpecific(
      const std::string& parsed_cmd,
      const std::string& appName
)
{
   DBG_out << "printCmdSpecificHelp requested";

   /* String vars to use for detailed help */
   string description;
   string prototype;
   string example;
   /* Yeah, it's a little annoying that you can't just append streams like the
    * rest of the strings.  I'll eventually figure out how to properly implement
    * a template + operator in the Enum Maps but until then, this is how you do
    * it.*/
   stringstream ss_params;

   if( 0 == parsed_cmd.compare("get_mode") ) {           /* get_mode cmd help */
      description = parsed_cmd + " command sends a request to the DC3 to get "
            "its current boot mode. The command returns the status of the "
            "request and the current boot mode. The possible return for: "
            "status: 0x00000000 for success or other codes for failure.\n"
            "bootmode: \n";
      ss_params << "[" << enumToString(_CB_NoBootMode) << "|"
            << enumToString(_CB_SysRomBoot) << "|"
            << enumToString(_CB_Bootloader) << "|"
            << enumToString(_CB_Application) << "]";
      description += ss_params.str();
      prototype = appName + " [connection options] --" + parsed_cmd;
      example = appName + " -i 207.27.0.75 --" + parsed_cmd;
   } else  if( 0 == parsed_cmd.compare("set_mode") ) {   /* set_mode cmd help */
      description = parsed_cmd + " command sends a request to the DC3 to set "
            "its current boot mode. Options are:";
      ss_params << "[" << enumToString(_CB_Bootloader) << "|"
                << enumToString(_CB_Application) << "]. ";
      description += ss_params.str();
      description +=
            "The command returns the status of the "
            "request. The possible return for: "
            "status: 0x00000000 for success or other codes for failure.\n";

      prototype = appName + " [connection options] --" + parsed_cmd + " mode=";
      prototype += ss_params.str();
      example = appName + " -i 207.27.0.75 --" + parsed_cmd + " mode=";
      example += ss_params.str();
   } else if (  0 == parsed_cmd.compare("flash") ) {        /* flash cmd help */
      description = parsed_cmd + " command initiates a FW upgrade on the DC3 "
            "board. You have to specify the location of the FW upgrade *bin "
            "file and which FW image is going to be upgraded. File can be "
            "specified using a relative path to " + appName + " or a full path. "
            "The options for the FW image type are: ";
      ss_params << "[" << enumToString(_CB_Bootloader) << "|"
            << enumToString(_CB_Application) << "]";
      description += ss_params.str();

      prototype = appName + " [connection options] --" + parsed_cmd
            + " file=<relative path to *.bin file> " + "type=";
      prototype += ss_params.str();

      example = appName + " -i 207.27.0.75 --" + parsed_cmd +
            " file=../../DC3Appl.bin " + "type=Application";
   } else if (  0 == parsed_cmd.compare("read_i2c") ) {  /* read_i2c cmd help */
      description = parsed_cmd + " command initiates an I2C device read.  You "
            "have to specify the I2C device (dev=), number of bytes to read "
            "(bytes=), and a start offset (start=)."
            "The options for the I2C devices are: ";
      ss_params << "[" << enumToString(_CB_EEPROM) << "|"
            << enumToString(_CB_SNROM) << "|" << enumToString(_CB_EUIROM) << "]";
      description += ss_params.str();

      description += ". Number of bytes is specified with 'bytes=<0 < int <= "
            "128>'.  Offset from the start is specified with 'start=<0 < int "
            "<= 128>'.  Depending on the device selected, size restrictions "
            "apply to prevent reading past the memory boundaries.  EEPROM is 128"
            " bytes, SNROM is 8 bytes, and EUIROM is 8 bytes.  start + bytes "
            "must be less than the remaining memory.  There is also an optional "
            "arg acc=<> which can be used to specify the type of access the board "
            "attempts to the I2C device. By default, event driven (QPC) will be "
            "used but acc= can be used to specify BARE and FRT (available in "
            "Application only";

      prototype = appName + " [connection options] --" + parsed_cmd
            + " start=<0 - 128> bytes=<0-128>" + "dev=";
      prototype += ss_params.str();

      example = appName + " -i 207.27.0.75 --" + parsed_cmd +
            " start=0 bytes=20" + "dev=EEPROM";
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

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
