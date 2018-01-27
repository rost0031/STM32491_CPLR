/**
 * @file    udp.cpp
 * Class that implements boost asio asynchronous, non-blocking UDP IO.
 *
 * @date    08/23/2013
 * @author  Harry Rostovtsev
 * @email   rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "udp.h"
#include "LogHelper.h"

/* Namespaces ----------------------------------------------------------------*/
/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_UDP );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private class methods -----------------------------------------------------*/

/******************************************************************************/
void Udp::read_handler(
      const boost::system::error_code& error,
      size_t bytes_transferred
)
{
   if ( !error ) {                             /* Data received with no error */

      /* Construct a new msg event indicating that a msg has been received */
      MsgData_t msg;
      memset(&msg, 0, sizeof(msg));
      msg.src = _DC3_EthCli;
      msg.dst = _DC3_EthCli;
      msg.dataLen = bytes_transferred;
      memcpy( msg.dataBuf, read_msg_, msg.dataLen );

      /* Put the data into the queue for ClientApi to read */
      if(!this->m_pQueue->push(msg)) {
         ERR_printf( m_pLog, "Unable to push data into queue.");
      }
   } else {
      ERR_printf(this->m_pLog, "Unable to read UDP data");
   }

   read_some();                                           /* Continue reading */
}

/******************************************************************************/
void Udp::read_some( void )
{
   m_socket.async_receive_from(
         boost::asio::buffer(read_msg_, DC3_MAX_MSG_LEN),
         m_loc_endpoint,
         boost::bind(
               &Udp::read_handler,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
         )
   );
}

/******************************************************************************/
void Udp::write_handler(
      const boost::system::error_code& error,
      size_t bytes_transferred
)
{
   write_msg_[bytes_transferred]=0;
   if (error) {
      ERR_printf(
            this->m_pLog,
            "Send error: %s.  %d bytes written: %s",
            error.message().c_str(),
            bytes_transferred,
            write_msg_
      );
   }
}

/******************************************************************************/
void Udp::write_some(const char* message, uint16_t len)
{
   memcpy(write_msg_, message, len);
   m_socket.async_send_to(
         boost::asio::buffer( write_msg_, len ),
         m_rem_endpoint,
         boost::bind(
               &Udp::write_handler,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
         )
   );
}

/******************************************************************************/
void Udp::setLogging( LogStub *log )
{
   this->m_pLog = log;
   DBG_printf(this->m_pLog,"Logging setup successful.");
}

/******************************************************************************/
Udp::Udp(
      const char *ipAddress,
      const char *pRemPort,
      const char *pLocPort,
      boost::lockfree::queue<MsgData_t> *pQueue
)  : m_pQueue(NULL),
     m_io(),
     m_socket( m_io ),
     m_rem_endpoint(boost::asio::ip::address::from_string(ipAddress), atoi(pRemPort)),
     m_loc_endpoint(boost::asio::ip::udp::v4(), atoi(pLocPort))

{
   boost::system::error_code myError;

   m_socket.open( boost::asio::ip::udp::v4(), myError);
   if (myError) {
      std::cout << "Open - " << myError.message() << std::endl;
      exit(1);
   }

   m_socket.bind( m_loc_endpoint, myError );
   if (myError) {
      std::cout << "Bind - " << myError.message() << std::endl;
      exit(1);
   }

   this->m_pQueue = pQueue;                  /* Set the pointer to the queue */

   read_some();

   // run the IO service as a separate thread, so the main thread can do others
   boost::thread t(boost::bind(&boost::asio::io_service::run, &m_io));
}

/******************************************************************************/
Udp::~Udp(  )
{
   m_socket.close();
}
