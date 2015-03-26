/**
 * @file    Callbacks.cpp
 * This file contains all the callbacks used by the Client.  This allows
 * different implementation of how to print and log info from the coupler
 * and the client.
 *
 * @date    03/26/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include <iostream>                              /* For cout and endl support */
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
void CLI_MsgCallback( char* message, int len )
{
   cout << "In CLI_MsgCallback callback" << endl;
}

/******************************************************************************/
void CLI_AckCallback( char* message, int len )
{
   cout << "in CLI_AckCallback, len=" << len << endl;
}

/******************************************************************************/
void CLI_LogCallback(
      DBG_LEVEL_T logLevel,
      CBErrorCode err,
      char *message
)
{
    cout << "in CLI_LogCallback, err=" << err << endl;
}

/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
