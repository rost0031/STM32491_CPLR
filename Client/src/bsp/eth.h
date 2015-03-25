// $Id$
/**
 * @file    eth.h
 * Declarations of functions needed for a "server" for udp communication
 *
 * @date    08/23/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$
#ifndef UDP_H
#define UDP_H

#include <unistd.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "CBCliShared.h"

/**
 * @class Eth
 *
 * @brief This class handles sending and receiving data over UDP for a
 * any Redwood clients.  It SHOULD be OS independent since it uses Boost.
 */
class Eth {

    char read_msg_[MAX_MSG_LEN];/**< buffer to hold incoming msgs */
    char write_msg_[MAX_MSG_LEN];/**< buffer to hold msgs being sent */

    boost::asio::io_service m_io;/**< internal instance of boost's io_service  */
    boost::asio::ip::udp::socket m_socket;/**< internal instance of boost's socket pointer */
    boost::asio::ip::udp::endpoint m_rem_endpoint; /**< internal instance of a remote UDP endpoint */
    boost::asio::ip::udp::endpoint m_loc_endpoint; /**< internal instance of a local UDP endpoint */

private:
    /**
     * Handler for the read_some function.
     *
     * @param[in|out]   error: Error that can occur during a read
     * @param[in|out]   bytes_transferred: number of bytes transferred
     *
     * @return      None.
     */
    void read_handler(
                        const boost::system::error_code& error,
                        size_t bytes_transferred
                     );

    /**
     * This method initiates async data read from the UDP connection and
     * launches the read_handler() when data is there.
     *
     * @param       None.
     * @return      None.
     */
    void read_some( void );

    /**
     * Handler for the write_some() method
     *
     * @param[in|out]   error: Error that can occur during a write
     * @param[in|out]   bytes_transferred: number of bytes transferred
     *
     * @return      None.
     */
    void write_handler(
                         const boost::system::error_code& error,
                         size_t bytes_transferred
                      );

public:
    /**
     * Write some data asynchronously to the UDP connection
     *
     * @param[in]   message: pointer to the buffer containing the data to write.
     * @param[in]   len: number of bytes in the buffer that need to be sent.
     *
     * @return      None.
     */
    void write_some( const char* message, uint16_t len );

    /**
     * Constructor that sets up the upd ethernet socket
     *
     * @param[in]   *ipAddress: pointer to the remote IP address string.
     * @param[in]   *pRemPort: pointer to the remote port number string.
     * @param[in]   *pLocPort: pointer to the local port number string.
     *
     * @return      None.
     */
    Eth( const char *ipAddress, const char *pRemPort, const char *pLocPort );

    /**
     * Destructor (default) that closes the UDP socket
     *
     * @param       None.
     * @return      None.
     */
    ~Eth( void );
};

#endif                                                               /* UDP_H */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
