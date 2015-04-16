/**
 * @file    udp.h
 * Class that implements boost asio asynchronous, non-blocking UDP IO.
 *
 * @date    08/23/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UDP_H
#define UDP_H

/* Includes ------------------------------------------------------------------*/
#include "ClientShared.h"
#include "MainMgrDefs.h"
#include <unistd.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "LogHelper.h"
#include "LogStub.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/
/**
 * @class Udp
 * @brief This class handles sending and receiving data over UDP.
 */
class Udp {

private:
   LogStub *m_pLog;         /**< Pointer to LogStub instance used for logging */
   char read_msg_[CB_MAX_MSG_LEN];          /**< buffer to hold incoming msgs */
   char write_msg_[CB_MAX_MSG_LEN];       /**< buffer to hold msgs being sent */

   boost::asio::io_service m_io;/**< internal instance of boost's io_service  */
   boost::asio::ip::udp::socket m_socket;/**< internal instance of boost's socket pointer */
   boost::asio::ip::udp::endpoint m_rem_endpoint; /**< internal instance of a remote UDP endpoint */
   boost::asio::ip::udp::endpoint m_loc_endpoint; /**< internal instance of a local UDP endpoint */

   /**
    * @brief Handler for the read_some function.
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
    * @brief This method initiates async data read from the UDP connection and
    * launches the read_handler() when data is there.
    *
    * @param       None.
    * @return      None.
    */
   void read_some( void );

   /**
    * @brief Handler for the write_some() method
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
    * @brief Write some data asynchronously to the UDP connection
    *
    * @param[in]   message: pointer to the buffer containing the data to write.
    * @param[in]   len: number of bytes in the buffer that need to be sent.
    *
    * @return      None.
    */
   void write_some( const char* message, uint16_t len );

   /**
    * @brief   Sets a new LogStub pointer.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @return: None.
    */
   void setLogging( LogStub *log );

   /**
    * @brief Constructor that sets up the upd ethernet socket
    *
    * @param[in]   *ipAddress: pointer to the remote IP address string.
    * @param[in]   *pRemPort: pointer to the remote port number string.
    * @param[in]   *pLocPort: pointer to the local port number string.
    *
    * @return      None.
    */
   Udp( const char *ipAddress, const char *pRemPort, const char *pLocPort );

   /**
    * @brief Destructor (default) that closes the UDP socket
    *
    * @param       None.
    * @return      None.
    */
   ~Udp( void );
};

#endif                                                               /* UDP_H */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
