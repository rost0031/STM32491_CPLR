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
CmdlineParser::CmdlineParser( int argc, char** argv )
{
   /* Make a local store cmd line args */
   this->argc = argc;
   this->argv = argv;


}

/******************************************************************************/
CmdlineParser::~CmdlineParser( void )
{

}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
