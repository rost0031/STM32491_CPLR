/**
 * @file    Job.cpp
 * Class that contains the job that the client needs to do.
 *
 * @date    03/25/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "Job.h"
#include "serial.h"
#include "eth.h"
#include "LogHelper.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_JOB );
/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* Private class prototypes --------------------------------------------------*/

/* Private class methods -----------------------------------------------------*/

/******************************************************************************/
Job::Job( void ) :
      m_pLog(NULL)
{
//   Serial *serial = new Serial("/dev/ttyS1", 115200, false );
   Eth *eth = new Eth( "127.0.0.1", "1502", "7777" );

}

Job::Job( LogStub *log ) :
      m_pLog(NULL)
{
   this->m_pLog = log;
}

/******************************************************************************/
Job::~Job( void )
{
   delete[] this->m_pLog;
}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
