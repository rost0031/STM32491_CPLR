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
#include "Help.h"
#include "Logging.h"
#include "EnumMaps.h"
/* Namespaces ----------------------------------------------------------------*/

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

   if( 0 == parsed_cmd.compare("get_mode") ) {
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
      example = appName + " -i 192.168.1.75 --" + parsed_cmd;
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
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
