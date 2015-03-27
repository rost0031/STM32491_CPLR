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
#include <iostream>
#include <stdio.h>
#include "CmdlineParser.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
//Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
//DBG_DEFINE_THIS_MODULE( DBG_MODL_GENERAL ); /* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
int main(int argc, char *argv[])
{
   cout << "Testing Client " << endl;

   CmdlineParser *cmdline = new CmdlineParser(argc, argv);

   return (0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
