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
#include "base64_wrapper.h"

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
APIError_t Comm::write_some( char* message, uint16_t len )
{
   if ( this->m_pSer ) {
      /* Serial messages have to be base64 encoded */
      char enDataBuf[CB_MAX_MSG_LEN];
      int encDataLen = base64_encode( message, len, enDataBuf, CB_MAX_MSG_LEN );

      if(encDataLen < 1) {
         ERR_printf(this->m_pLog,"Encoding failed\n");
         return API_ERR_SER_MSG_BASE64_ENC_FAILED;
      }
      this->m_pSer->write_some( enDataBuf, encDataLen );
   } else if ( this->m_pUdp ) {
      this->m_pUdp->write_some( message, len );
   } else {
      ERR_printf(this->m_pLog, "No comm interface have been set up.");
      return API_ERR_COMM_NOT_SET;
   }
   return API_ERR_NONE;
}

/******************************************************************************/
void Comm::setLogging( LogStub *log )
{
   this->m_pLog = log;
   DBG_printf(this->m_pLog,"Logging setup successful.");
   if ( this->m_pSer ) {
      this->m_pSer->setLogging( log);
   } else if ( this->m_pUdp ) {
      this->m_pUdp->setLogging( log );
   } else {
      WRN_printf(this->m_pLog, "No active connections to set logging for");
   }
}

/******************************************************************************/
Comm::Comm(
      LogStub *log,
      const char *dev_name,
      int baud_rate,
      bool bDFUSEComm,
      boost::lockfree::queue<MsgData_t> *pQueue
) : m_pLog(NULL), m_pUdp(NULL), m_pSer(NULL)
{
   this->m_pLog = log;
   this->m_pSer = new Serial( dev_name, baud_rate, bDFUSEComm, pQueue);
   this->m_pSer->setLogging( log );
}

/******************************************************************************/
Comm::Comm(
      LogStub *log,
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort,
      boost::lockfree::queue<MsgData_t> *pQueue
) : m_pLog(NULL), m_pUdp(NULL), m_pSer(NULL)
{
   this->m_pLog = log;
   this->m_pUdp = new Udp( ipAddress, pRemPort, pLocPort, pQueue);
   this->m_pUdp->setLogging( log );
}

/******************************************************************************/
Comm::~Comm( void )
{
   delete[] this->m_pLog;
   delete[] this->m_pSer;
   delete[] this->m_pUdp;
}
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
