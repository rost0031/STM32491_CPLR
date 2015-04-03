// $Id$
/**
 * @file    serial.h
 * Declarations of functions needed for a "server" for ethernet and
 * serial communications
 *
 * @date    02/18/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$


#ifndef SERIAL_H
#define SERIAL_H

/* Includes ------------------------------------------------------------------*/
#include "ClientShared.h"
#include <unistd.h>
#include <iostream>
#include "boost/asio.hpp"
#include "boost/asio/serial_port.hpp"
#include "boost/system/error_code.hpp"
#include "boost/system/system_error.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include "dfuse.h"

/**
 * @class Serial
 *
 * @brief This class handles sending and receiving data over serial for a
 * any Redwood clients.  It SHOULD be OS independent since it uses Boost.
 */
class Serial {

private:
    char read_msg_[CB_MAX_MSG_LEN];/**< buffer to hold incoming msgs */
    char write_msg_[CB_MAX_MSG_LEN];/**< buffer to hold msgs being sent */

    uint8_t bReadNDFUSEBytes; /**< This variable will hold how many bytes to
    read on the DFUSE serial bus because ST is inconsistent about when they send
    and don't send ACKs/NACKs.  Pretty much every command sends an ACK when it's
    finished except for READ_MEM.  This variable needs to get set when sending
    how many bytes to read (last step of the READ_MEM DFUSE cmd) so that this
    class will know when to stop reading.  This variable is ignored when set to
    0. */
    boost::asio::streambuf serial_stream_;

    boost::asio::io_service m_io;/**< internal instance of boost's io_service  */
    boost::asio::serial_port m_port;/**< internal instance of boost's serial port pointer */

    /**
     * Handler for the read_some function.
     *
     * @param[in|out]   error: Error that can occur during a read
     * @param[in|out]   bytes_transferred: number of bytes transferred
     *
     * @return      None.
     */
    void read_handler(  const boost::system::error_code& error, size_t bytes_transferred);

    /**
     * This method initiates async data read from the serial port and launches
     * the read_handler() when data is there.
     *
     * @param       None.
     * @return      None.
     */
    void read_some(void);

    /**
     * Handler for the read_some_DFUSE function.
     *
     * @param[in|out]   error: Error that can occur during a read
     * @param[in|out]   bytes_transferred: number of bytes transferred
     *
     * @return      None.
     */
    void read_handler_DFUSE(  const boost::system::error_code& error, size_t bytes_transferred);

    /**
     * This method initiates async data read DFUSE data from the serial port and
     * launches the read_handler_DFUSE() when data is there.
     *
     * @param       None.
     * @return      None.
     */
    void read_some_DFUSE(void);

    /**
     * Handler for the write_some() method
     *
     * @param[in|out]   error: Error that can occur during a write
     * @param[in|out]   bytes_transferred: number of bytes transferred
     *
     * @return      None.
     */
    void write_handler(  const boost::system::error_code& error, size_t bytes_transferred);

public:
    /**
     * Write some data asynchronously to the serial port
     *
     * @param[in]   message: pointer to the buffer containing the data to write.
     * @param[in]   len: number of bytes in the buffer that need to be sent.
     *
     * @return      None.
     */
    void write_some(char* message, uint16_t len);

    /**
     * Tell the serial bus (when using DFUSE) how many bytes to expect to read.
     * This is a hack to get around ST's inconsistent use of ACKs/NACKs.
     *
     * @param[in]   bytes: number of bytes to expect to read.
     *
     * @return      None.
     */
    void expect_n_bytes( uint16_t bytes );

    /**
     * Constructor that sets up serial for regular or DFUSE communication.
     *
     * @param[in]   dev_name: serial device name.  /dev/ttyS10 or COMX
     * @param[in]   baud_rate: serial baud rate.
     * @param[in]   bDFUSEComm: bool that specifies whether to set up serial
     * for DFUSE or regular serial communication.
     *   @arg  TRUE: set up serial for DFUSE
     *   @arg  FALSE: set up serial for regular serial comms.
     *
     * @return      None.
     */
    Serial(const char *dev_name, int baud_rate, bool bDFUSEComm);

    /**
     * Destructor that cleans up serial port.
     *
     * @param[in]   None.
     * @return      None.
     */
    ~Serial( void );
};

/**
 * @class DFUSE_delim
 *
 * @brief This template class handles figures out if a DFUSE ACK or NACK has
 * been found in the serial data.  It is used by the async_read for figuring
 * out when DFUSE has finished transmitting.
 */
class DFUSE_delim
{
public:

   explicit DFUSE_delim( uint16_t nBytesToRead ) : _nBytesToRead( nBytesToRead ) {}

   template <typename Iterator> std::pair<Iterator, bool> operator()(
         Iterator begin,
         Iterator end
   ) const
   {
      Iterator i = begin;
      uint8_t j = 0;
      for (i = begin, j = 0; i != end; i++, j++) {
         if ( _nBytesToRead == 0 ) {/* Look for ACK/NACK */

            /* If a match happens, pre-increment the iterator so ACK/NACK gets
             * returned as part of the data stream */
            if ( DFUSE_ACK == *i ) {
               return( std::make_pair(++i, true) );
            } else if ( DFUSE_NACK == *i ) {
               return( std::make_pair(++i, true) );
            }
         } else { /* Count until enough bytes have been read */
            if (j == _nBytesToRead ) {
               return( std::make_pair(++i, true) );
            }
         }
      }
      return( std::make_pair(i, false) );
   }

private:
   uint16_t _nBytesToRead;

};

/**
 * @brief This type allows the DFUSE_delim to be used inside of a bind() call.
 * This is just a hack around the crappy boost library limitations.  They claim
 * that any type can be used but they lie.
 */
namespace boost {
   namespace asio {
      template <> struct is_match_condition< DFUSE_delim >
         : public boost::true_type {};
   }
}

#endif                                                            /* SERIAL_H */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
