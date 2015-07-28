/**
 * @file    serial.cpp
 * Class that implements boost asio asynchronous, non-blocking serial IO.
 *
 * @date    02/18/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "serial.h"
#include "base64_wrapper.h"
#include <iomanip>
#include <sstream>
#include "LogHelper.h"
/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_SER );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private class methods -----------------------------------------------------*/

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

    /* If we don't find any of the strings below in the message, assume it's a
     * base64 encoded string.  Otherwise, just print it since it's probably a
     * logging message from DC3. */
    if (
          string::npos == line.find("DBG") &&
          string::npos == line.find("LOG") &&
          string::npos == line.find("WRN") &&
          string::npos == line.find("ERR") &&
          string::npos == line.find("ISR")
    )
    {

       /* Construct a new msg event indicating that a msg has been received */
       MsgData_t msg;
       memset(&msg, 0, sizeof(msg));

       /* Decode the message from base64 and write it directly to the event. */
       msg.dataLen = base64_decode(
             read_msg_,
             bytes_transferred,
             (char *)msg.dataBuf,
             DC3_MAX_MSG_LEN
       );

       /* Set the size and source */
       msg.src = _DC3_EthCli;
       msg.dst = _DC3_EthCli;

       /* Put the data into the queue for ClientApi to read */
       if(!this->m_pQueue->push(msg)) {
          ERR_printf( m_pLog, "Unable to push data into queue.");
       }

    } else {
       DC3_printf(this->m_pLog, read_msg_);
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
   const char* header =
         boost::asio::buffer_cast<const char*>( serial_stream_.data() );

   memcpy(read_msg_, header, bytes_transferred);
   serial_stream_.consume(bytes_transferred);

   if ( 1 == bytes_transferred && 0 == bReadNDFUSEBytes ) {
      if ( DFUSE_ACK == read_msg_[0] )  {
         /* If the buffer size is 1, then this is an independent ACK msg*/
//         QEvt *qEvt = Q_NEW(QEvt, DFUSE_ACK_SIG);
//         QF_PUBLISH((QEvent *)qEvt, AO_CommStackMgr);
      } else if ( DFUSE_NACK == read_msg_[0] ) {
//         QEvt *qEvt = Q_NEW(QEvt, DFUSE_NACK_SIG);
//         QF_PUBLISH((QEvent *)qEvt, AO_CommStackMgr);
      }
   } else if ( bytes_transferred > 1 ) {
      /* Reset the flag to read n bytes if it's set */
      if ( bReadNDFUSEBytes > 0 ) {
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
void Serial::setLogging( LogStub *log )
{
   this->m_pLog = log;
   DBG_printf(this->m_pLog,"Logging setup successful.");
}

/******************************************************************************/
Serial::Serial(
      const char *dev_name,
      int baud_rate,
      bool bDFUSEComm,
      boost::lockfree::queue<MsgData_t> *pQueue
)  :
      m_pQueue(NULL),
      m_io(),
      m_port(m_io, dev_name)
{

   /* Set the bReadNDFUSEBytes to zero so that we don't accidentally quit
    * reading early */
   bReadNDFUSEBytes = 0;

   /* Set the serial port options explicitly.  This isn't necessary in Linux or
    * Cygwin but for some reason, Windows compiles don't work if this isn't done.
    * Either way, now that DFUSE serial is in the mix, the parity has to be set
    * differently depending on how the serial is being configured. */
   m_port.set_option( boost::asio::serial_port_base::character_size( 8 ) );
   m_port.set_option( boost::asio::serial_port_base::stop_bits(boost::asio::serial_port::stop_bits::one) );
   m_port.set_option( boost::asio::serial_port_base::baud_rate( baud_rate ) );
   m_port.set_option( boost::asio::serial_port_base::flow_control( boost::asio::serial_port::flow_control::none ) );

   this->m_pQueue = pQueue;                   /* Set the pointer to the queue */

   /* These settings depend on whether we are running serial in "regular" or
    * DFUSE mode. */
   if ( bDFUSEComm ) {/* DFUSE specific setting and read function */
      m_port.set_option(
            boost::asio::serial_port_base::parity(
                  boost::asio::serial_port::parity::even
            )
      );   // Even for regular serial
      read_some_DFUSE();
   } else {/* Regular serial specific setting and read function */
      m_port.set_option(
            boost::asio::serial_port_base::parity(
                  boost::asio::serial_port::parity::none
            )
      );   // None for regular serial
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

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
