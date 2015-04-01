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
#include "Job.h"
#include "Logging.h"
#include "CBSharedDbgLevels.h"
#include "Callbacks.h"
/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
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

   int tmp = 6;
   CON_output(
         DBG,
         __func__,
         __LINE__,
         "Testing logging function %d\n",
         tmp
   );

//   CmdlineParser *cmdline = new CmdlineParser(argc, argv);

   //   Job *job = new Job();

   Logging *log = new Logging();
//   log->setMsgCallBack( CLI_MsgCallback );

//   log->m_pLog->m_pMsgHandlerCBFunction("Test\n", 5);

   log->setLibLogCallBack( CLI_LibLogCallback );

   cout << "Testing Client some more " << endl;
   printf("Hello\n");
   return (0);
}


/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
