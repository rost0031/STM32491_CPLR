// $Id$
/**
 * @file    serial.cpp
 * Definitions of functions needed for a "server" for ethernet and
 * serial communications
 *
 * @date    02/18/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$

#include "serial.h"
#include "base64_wrapper.h"
#include <iomanip>
#include <sstream>

using namespace std;

extern Serial *serial;

/******************************************************************************/
void Serial::read_handler(
                           const boost::system::error_code& error,
                           size_t bytes_transferred
                         )
{

    std::istream is(&serial_stream_);
    std::string line;
    std::getline(is, line);
    memcpy(read_msg_, line.c_str(), bytes_transferred);
    read_msg_[bytes_transferred]=0;

    /* The last 2 characters of a base64 encoded string are '==' so we check for
     * them and if present, decode and launch message.  Otherwise, we can just
     * print them out since it's just printfs coming over serial console.*/
    if (  0 == line.find("DBG:") ||
          0 == line.find("LOG:")   ||
          0 == line.find("WRN:")   ||
          0 == line.find("ERR:")
       )
    {
        std::cout << "CB: " << read_msg_ << std::endl;
    }
    else
    {
//        /* Construct a new msg event indicating that a msg has been received */
//        MsgEvt *msgEvt = Q_NEW(MsgEvt, MSG_RECEIVED_SIG);
//
//        /* Decode the message from base64 and write it directly to the event. */
//        int decoded_sz = base64_decode(read_msg_, bytes_transferred, msgEvt->msg, MAX_MSG_LEN);
//
//        /* Set the size and source */
//        msgEvt->msg_len = decoded_sz;
//        msgEvt->source = SERIAL;
//
//        /* Publish the event */
//        QF_PUBLISH((QEvent *)msgEvt, AO_CommStackMgr);
    }

    /* Continue reading */
    read_some();
}

/******************************************************************************/
void Serial::read_some( void )
{
   async_read_until(
         m_port,
         serial_stream_,
         "\n",
         boost::bind(
               &Serial::read_handler,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
         )
   );
}

/******************************************************************************/
void Serial::read_handler_DFUSE(
      const boost::system::error_code& error,
      size_t bytes_transferred
)
{
   /* These very confusing several lines are how you get data out of a boost
    * streambuffer. */
   const char* header=boost::asio::buffer_cast<const char*>(serial_stream_.data());
   memcpy(read_msg_, header, bytes_transferred);
   serial_stream_.consume(bytes_transferred);

   if ( 1 == bytes_transferred && 0 == bReadNDFUSEBytes )
   {
      if ( DFUSE_ACK == read_msg_[0] )
      {
         /* If the buffer size is 1, then this is an independent ACK msg*/
//         QEvt *qEvt = Q_NEW(QEvt, DFUSE_ACK_SIG);
//         QF_PUBLISH((QEvent *)qEvt, AO_CommStackMgr);
      }
      else if ( DFUSE_NACK == read_msg_[0] )
      {
//         QEvt *qEvt = Q_NEW(QEvt, DFUSE_NACK_SIG);
//         QF_PUBLISH((QEvent *)qEvt, AO_CommStackMgr);
      }
   }
   else if ( bytes_transferred > 1 )
   {
      /* Reset the flag to read n bytes if it's set */
      if ( bReadNDFUSEBytes > 0 )
      {
         bReadNDFUSEBytes = 0;
      }

      /* Otherwise, this is a terminator to response msg */
//      DfuseMsgEvt *dfuseMsgEvt = Q_NEW(DfuseMsgEvt, DFUSE_RESP_SIG);
//      memcpy(dfuseMsgEvt->msg, read_msg_, bytes_transferred);
//      dfuseMsgEvt->msg_len   = bytes_transferred;
//      QF_PUBLISH((QEvent *)dfuseMsgEvt, AO_CommStackMgr);
   }

   /* Continue reading */
   read_some_DFUSE();
}

/******************************************************************************/
void Serial::read_some_DFUSE( void )
{
   async_read_until(
         m_port,
         serial_stream_,
         DFUSE_delim( bReadNDFUSEBytes ),
         boost::bind(
               &Serial::read_handler_DFUSE,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
         )
   );
}

/******************************************************************************/
void Serial::write_handler(
                              const boost::system::error_code& error,
                              size_t bytes_transferred
                          )
{
   write_msg_[bytes_transferred]=0;
//    std::cout << bytes_transferred << " bytes written: " << write_msg_ << std::endl;
}

/******************************************************************************/
void Serial::write_some( char* message, uint16_t len )
{
   memcpy(write_msg_, message, len);

   m_port.async_write_some(
         boost::asio::buffer(
               write_msg_,
               len
         ),
         boost::bind(
               &Serial::write_handler,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
         )
   );
}

/******************************************************************************/
void Serial::expect_n_bytes( uint16_t bytes )
{
   bReadNDFUSEBytes = bytes;
}

/******************************************************************************/
Serial::Serial(const char *dev_name, int baud_rate, bool bDFUSEComm) : m_io(), m_port(m_io, dev_name)
{

   /* Set the bReadNDFUSEBytes to zero so that we don't accidentally quit
    * reading early */
   bReadNDFUSEBytes = 0;

   /* Set the serial port options explicitly.  This isn't necessary in Linux or
    * Cygwin but for some reason, Windows compi
     * @param[in]   baud_rate: serial baud rate.
     * @param[in]   bDFUSEComm: bool that specifies whether to set up serial
     * for DFUSE or regular serial communication.
     *   @arg  TRUE: set up serial for DFUSE
     *   @arg  FALSE: set up serial for regular serial comms.
     *les don't work if this isn't done.
    * Either way, now that DFUSE serial is in the mix, the parity has to be set
    * differently depending on how the serial is being configured. */
   m_port.set_option( boost::asio::serial_port_base::character_size( 8 ) );
   m_port.set_option( boost::asio::serial_port_base::stop_bits(boost::asio::serial_port::stop_bits::one) );
   m_port.set_option( boost::asio::serial_port_base::baud_rate( baud_rate ) );
   m_port.set_option( boost::asio::serial_port_base::flow_control( boost::asio::serial_port::flow_control::none ) );

   /* These settings depend on whether we are running serial in "regular" or
    * DFUSE mode. */
   if ( bDFUSEComm ) /* DFUSE specific setting and read function */
   {
      m_port.set_option( boost::asio::serial_port_base::parity(boost::asio::serial_port::parity::even ) );   // Even for regular serial
      read_some_DFUSE();
   }
   else /* Regular serial specific setting and read function */
   {
      m_port.set_option( boost::asio::serial_port_base::parity(boost::asio::serial_port::parity::none ) );   // None for regular serial
      read_some();
   }

   /* run the IO service as a separate thread, so the main thread can do stuff */
   boost::thread t(boost::bind(&boost::asio::io_service::run, &m_io));
}

/******************************************************************************/
Serial::~Serial( void )
{
   m_port.close();
}
