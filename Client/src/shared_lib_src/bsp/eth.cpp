// $Id$
/**
 * @file    eth.cpp
 * Definitions of functions needed for a "server" for ethernet udp communication
 *
 * @date    08/23/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$

#include "eth.h"
#include "base64_wrapper.h"

/******************************************************************************/
void Eth::read_handler(
      const boost::system::error_code& error,
      size_t bytes_transferred
)
{
   if ( !error ) {
      /* Construct a new msg event indicating that a msg has been received */
//      MsgEvt *msgEvt = Q_NEW(MsgEvt, MSG_RECEIVED_SIG);
//
//      /* Copy data and set the size and source */
//      memcpy( msgEvt->msg, read_msg_, bytes_transferred );
//      msgEvt->msg_len = bytes_transferred;
//      msgEvt->source = IP;
//
//      /* Publish the event */
//      QF_PUBLISH((QEvent *)msgEvt, AO_CommStackMgr);
   } else {
      printf("Error reading UDP data\n");
   }

   /* Continue reading */
   read_some();
}

/******************************************************************************/
void Eth::read_some( void )
{
   m_socket.async_receive_from(
         boost::asio::buffer(read_msg_, CB_MAX_MSG_LEN),
         m_loc_endpoint,
         boost::bind(
               &Eth::read_handler,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
         )
   );
}

/******************************************************************************/
void Eth::write_handler(
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
void Eth::write_some(const char* message, uint16_t len)
{
   memcpy(write_msg_, message, len);
   m_socket.async_send_to(
         boost::asio::buffer( write_msg_, len ),
         m_rem_endpoint,
         boost::bind(
               &Eth::write_handler,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
         )
   );
}

/******************************************************************************/
Eth::Eth( const char *ipAddress, const char *pRemPort, const char *pLocPort )
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
Eth::~Eth(  )
{
   m_socket.close();
}
