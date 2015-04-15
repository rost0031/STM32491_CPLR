/**
 * @file    udp.cpp
 * Class that implements boost asio asynchronous, non-blocking UDP IO.
 *
 * @date    08/23/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
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
   if ( !error ) {
      /* Construct a new msg event indicating that a msg has been received */
      MsgEvt *msgEvt = Q_NEW(MsgEvt, MSG_RECEIVED_SIG);

      /* Copy data and set the size and source */
      memcpy( msgEvt->msg, read_msg_, bytes_transferred );
      msgEvt->msg_len = bytes_transferred;
      msgEvt->msg_src = _CB_EthCli;

      /* Publish the event */
      QF_PUBLISH((QEvent *)msgEvt, AO_MainMgr);
   } else {
      printf("Error reading UDP data\n");
   }

   /* Continue reading */
   read_some();
}

/******************************************************************************/
void Udp::read_some( void )
{
   m_socket.async_receive_from(
         boost::asio::buffer(read_msg_, CB_MAX_MSG_LEN),
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
   if (error)
   {
      std::cout << "Send - " << error.message() << std::endl;
      std::cout << bytes_transferred << " bytes written: " << write_msg_ << std::endl;
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
Udp::Udp( const char *ipAddress, const char *pRemPort, const char *pLocPort )
   : m_io(),
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

   read_some();

   // run the IO service as a separate thread, so the main thread can do others
   boost::thread t(boost::bind(&boost::asio::io_service::run, &m_io));
}

/******************************************************************************/
Udp::~Udp(  )
{
   m_socket.close();
}