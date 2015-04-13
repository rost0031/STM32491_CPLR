/**
 * @file    comm.h
 * Wrapper class that abstracts ethernet and serial (and possibly other future)
 * communication channels.
 *
 * @date    04/13/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMM_H_
#define COMM_H_

/* Includes ------------------------------------------------------------------*/
#include "LogStub.h"
#include "serial.h"
#include "eth.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/
/**
* @class Job
*
* @brief This class abstracts various communication channels
*
* This class is responsible for abstracting serial, ethernet (UDP) (and
* possibly others) in with an interface that provides the same interface to all
* the different ways to access the FW.
*/
class comm {

private:
   LogStub *m_pLog; /**< Pointer to LogStub instance used for logging */
   Eth     *m_pEth; /**< Pointer to an Eth object */
   Serial  *m_pSer; /**< Pointer to a Serial object */

public:

    /**
     * @brief  Write some data asynchronously to the setup interface.
     *
     * @param[in]   message: pointer to the buffer containing the data to write.
     * @param[in]   len: number of bytes in the buffer that need to be sent.
     *
     * @return      None.
     */
   void write_some( char* message, uint16_t len );

    /**
     * @brief  Constructor that initializes a serial interface and logging.
     * @param [in] *log: LogStub pointer to the class that has the proper
     *                   callbacks set up for logging.
     * @param[in]   dev_name: serial device name.  /dev/ttyS10 or COMX
     * @param[in]   baud_rate: serial baud rate.
     * @param[in]   bDFUSEComm: bool that specifies whether to set up serial
     * for DFUSE or regular serial communication.
     *   @arg  TRUE: set up serial for DFUSE
     *   @arg  FALSE: set up serial for regular serial comms.
     *
     * @return None.
     */
    comm(
          LogStub *log,
          const char *dev_name,
          int baud_rate,
          bool bDFUSEComm
    );

    /**
     * @brief  Constructor that initializes an ethernet interface and logging.
     * @param [in] *log: LogStub pointer to the class that has the proper
     *                   callbacks set up for logging.
     * @param[in]   *ipAddress: pointer to the remote IP address string.
     * @param[in]   *pRemPort: pointer to the remote port number string.
     * @param[in]   *pLocPort: pointer to the local port number string.
     *
     * @return None.
     */
    comm(
          LogStub *log,
          const char *ipAddress,
          const char *pRemPort,
          const char *pLocPort
    );

    /**
     * @brief  Default destructor.
     */
    ~comm( void );

};

#endif                                                             /* COMM_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
