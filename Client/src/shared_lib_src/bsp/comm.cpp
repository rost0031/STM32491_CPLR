/**
 * @file    comm.cpp
 * Wrapper class that abstracts ethernet and serial (and possibly other future)
 * communication channels.
 *
 * @date    04/13/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "comm.h"
#include "LogHelper.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_COM );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private class methods -----------------------------------------------------*/

/******************************************************************************/
void Comm::write_some( char* message, uint16_t len )
{
   if ( this->m_pSer ) {
      this->m_pSer->write_some( message, len );
   } else if ( this->m_pUdp ) {
      this->m_pUdp->write_some( message, len );
   } else {
      ERR_printf(this->m_pLog, "No comm interface have been set up.");
   }
}

/******************************************************************************/
Comm::Comm(
      LogStub *log,
      const char *dev_name,
      int baud_rate,
      bool bDFUSEComm
) : m_pLog(NULL), m_pUdp(NULL), m_pSer(NULL)
{
   this->m_pLog = log;
   this->m_pSer = new Serial( dev_name, baud_rate, bDFUSEComm);
}

/******************************************************************************/
Comm::Comm(
      LogStub *log,
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort
) : m_pLog(NULL), m_pUdp(NULL), m_pSer(NULL)
{
   this->m_pLog = log;
   this->m_pUdp = new Udp( ipAddress, pRemPort, pLocPort);
}

/******************************************************************************/
Comm::~Comm( void )
{
   delete[] this->m_pLog;
   delete[] this->m_pSer;
   delete[] this->m_pUdp;
}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
