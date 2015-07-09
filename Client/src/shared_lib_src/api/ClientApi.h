/**
 * @file    ClientApi.h
 * Declarations for class and functions used by the Client API.
 *
 * @date    03/05/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CLIENTAPI_H_
#define CLIENTAPI_H_

/* Includes ------------------------------------------------------------------*/
#include <string>
#include "CallbackTypes.h"
#include "LogStub.h"

#include <boost/thread.hpp>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/
#ifdef _WIN32
#ifdef BUILDING_CLIENT_DLL
#define CLIENT_DLL __declspec(dllexport)
#else
#define CLIENT_DLL __declspec(dllimport)
#endif
#else
#define CLIENT_DLL
#define __stdcall
#endif

#ifdef __cplusplus
}
#endif

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/
/**
 * @class ClientApi
 *
 * @brief This class handles setting up the interface to the client using
 * either a cmd line interface or a graphical interface.
 */
class CLIENT_DLL ClientApi {

private:
   LogStub *m_pLog;         /**< Pointer to LogStub instance used for logging */
   CB_ReqMsgHandler_t m_pReqHandlerCBFunction; /**< Callback for handling Req msgs */
   CB_AckMsgHandler_t m_pAckHandlerCBFunction; /**< Callback for handling Ack msgs */
   CB_DoneMsgHandler_t m_pDoneHandlerCBFunction; /**< Callback for handling Done msgs */

   bool m_bReqLogEnable; /**< flag to enable/disable callback call for Req msgs */
   bool m_bAckLogEnable; /**< flag to enable/disable callback call for Ack msgs */
   bool m_bProgLogEnable; /**< flag to enable/disable callback call for Prog msgs */
   bool m_bDoneLogEnable; /**< flag to enable/disable callback call for Done msgs */

   unsigned int m_msgId;   /* Msg ID incrementing counter for unique msg ids. */
   bool m_bRequestProg;     /* Flag to see if progress messages are requested */
   CBMsgRoute m_msgRoute; /* This is set based on the connection used (UDP vs Serial) */

   boost::thread m_workerThread;          /**< Thread to start MainMgr and QF */

   /* All the different msg structures that exist */
   struct CBBasicMsg          m_basicMsg;
   struct CBStatusPayloadMsg  m_statusPayloadMsg;
   struct CBVersionPayloadMsg m_versionPayloadMsg;
   struct CBBootModePayloadMsg m_bootmodePayloadMsg;
   struct CBFlashMetaPayloadMsg m_flashMetaPayloadMsg;
   struct CBFlashDataPayloadMsg m_flashDataPayloadMsg;
   struct CBI2CDataPayloadMsg m_i2cDataPayloadMsg;

   uint8_t dataBuf[1000];
   int dataLen;

   /**
    * @brief   Polls for new events in the queue from MainMgr AO.
    * @param [out] *basicMsg: CBBasicMsg pointer to the basic msg struct that
    * will contain the data on output.
    * @param [out] *payloadMsgUnion: CBPayloadMsgUnion_t pointer to the union
    * of all the payload msgs that will contain the correct portion of itself
    * filled on output.  This union is indexed by the contents of the
    * basicMsg._msgPayload.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t pollForResp(
         CBBasicMsg *basicMsg,
         CBPayloadMsgUnion_t *payloadMsgUnion
   );

   /**
    * @brief   Blocks while waiting for new events in queue from MainMgr AO.
    * @param [out] *basicMsg: CBBasicMsg pointer to the basic msg struct that
    * will contain the data on output.
    * @param [out] *payloadMsgUnion: CBPayloadMsgUnion_t pointer to the union
    * of all the payload msgs that will contain the correct portion of itself
    * filled on output.  This union is indexed by the contents of the
    * basicMsg._msgPayload.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t waitForResp(
         CBBasicMsg *basicMsg,
         CBPayloadMsgUnion_t *payloadMsgUnion,
         uint16_t timeoutSecs
   );

public:

   /****************************************************************************
    *                    DC3 requests
    ***************************************************************************/
   /**
    * @brief   Blocking cmd to get the current boot mode of DC3.
    * @param [out] *status: CBErrorCode pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [out] *mode: CBBootMode pointer to the returned boot mode from the
    * DC3 board.
    *    @arg  _CB_Bootloader: the DC3 board is currently in bootloader mode.
    *    @arg  _CB_Application: the DC3 board is currently in application mode.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_getMode( CBErrorCode *status, CBBootMode *mode );

   /**
    * @brief   Blocking cmd to set the current boot mode of DC3.
    * @param [out] *status: CBErrorCode pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [in] mode: CBBootMode to the mode to set the DC3 board to.
    *    @arg  _CB_Bootloader: the DC3 board is currently in bootloader mode.
    *    @arg  _CB_Application: the DC3 board is currently in application mode.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_setMode(CBErrorCode *status, CBBootMode mode);

   /**
    * @brief   Blocking cmd to get the current boot mode of DC3.
    * @param [out] *status: CBErrorCode pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [in] type: CBBootMode that specifies where the FW image will be
    * flashed to
    *    @arg  _CB_Application: flash the FW image to the application space.
    * @param [in] *filename: const char pointer to a path and file where the
    * FW image file can be found.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    *
    */
   APIError_t DC3_flashFW(
         CBErrorCode *status,
         CBBootMode type,
         const char *filename
   );

   /**
    * @brief   Blocking cmd to read I2C device on the DC3.
    * @param [out] *status: CBErrorCode pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    * @param [out] *pBytesRead: pointer to number of bytes read from I2C device.
    * @param [out] *pBuffer: pointer to buffer where data will be stored.
    * @param [in] bufferSize: size_t size of *pBuffer storage area.
    * @param [in] bytes: number of bytes to read
    * @param [in] start: where to start reading from
    * @param [in] dev: CBI2CDevices type that specifies the I2C device to read
    *    @arg _CB_EEPROM: read from the EEPROM on I2C bus 1.
    *    @arg _CB_SNROM: read from the RO SerialNumber ROM on I2C bus 1.
    *    @arg _CB_EUIROM: read from the RO Unique number ROM on I2C bus 1.
    * @param [in] acc: CBAccessType type that specifies the access to use to get
    * at the I2C bus.
    *    @arg _CB_ACCESS_BARE: bare metal access. Blocking and slow. For testing only.
    *    @arg _CB_ACCESS_QPC: use event driven QPC access.
    *    @arg _CB_ACCESS_FRT: use event driven FreeRTOS access.  Available in
    *    Application only.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failures.
    */
   APIError_t DC3_readI2C(
         CBErrorCode *status,
         uint16_t *pBytesRead,
         uint8_t *pBuffer,
         const int bufferSize,
         const int bytes,
         const int start,
         const CBI2CDevices dev,
         const CBAccessType acc
   );

   /**
    * @brief   Blocking cmd to write I2C device on the DC3.
    * @param [out] *status: CBErrorCode pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    * @param [out] *pBuffer: pointer to buffer where data to write is stored.
    * @param [in] bytes: number of bytes to write
    * @param [in] start: where to start writing to (offset)
    * @param [in] dev: CBI2CDevices type that specifies the I2C device to read
    *    @arg _CB_EEPROM: read from the EEPROM on I2C bus 1.
    *    @arg _CB_SNROM: read from the RO SerialNumber ROM on I2C bus 1.
    *    @arg _CB_EUIROM: read from the RO Unique number ROM on I2C bus 1.
    * @param [in] acc: CBAccessType type that specifies the access to use to get
    * at the I2C bus.
    *    @arg _CB_ACCESS_BARE: bare metal access. Blocking and slow. For testing only.
    *    @arg _CB_ACCESS_QPC: use event driven QPC access.
    *    @arg _CB_ACCESS_FRT: use event driven FreeRTOS access.  Available in
    *    Application only.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failures.
    */
   APIError_t DC3_writeI2C(
         CBErrorCode *status,
         const uint8_t* const pBuffer,
         const int bytes,
         const int start,
         const CBI2CDevices dev,
         const CBAccessType acc
   );

   /**
    * @brief   Blocking cmd to start a test of external RAM of DC3.
    * @param [out] *status: CBErrorCode pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [out] *test: CBRamTest_t pointer to the returned RAM test that
    * failed on the DC3 board.
    *    @arg _CB_RAM_TEST_NONE: no test failed.
    *    @arg _CB_RAM_TEST_DATA_BUS: ram test failed in the data bus integrity
    *                                test
    *    @arg _CB_RAM_TEST_ADDR_BUS: ram test failed in the addr bus integrity
    *                                test
    *    @arg _CB_RAM_TEST_DEV_INT:  ram test failed in the device integrity
    *                                test
    * @param [out] *addr: address at which the ram address saw an error.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_ramTest(
         CBErrorCode *status,
         CBRamTest_t* test,
         uint32_t* addr
   );

   /****************************************************************************
    *                    Client control functionality
    ***************************************************************************/
   /**
    * @brief   Sets up a new ethernet connection
    * @param[in]   *ipAddress: pointer to the remote IP address string.
    * @param[in]   *pRemPort: pointer to the remote port number string.
    * @param[in]   *pLocPort: pointer to the local port number string.
    * @return  None.
    */
   APIError_t setNewConnection(
         const char *ipAddress,
         const char *pRemPort,
         const char *pLocPort
   );

   /**
    * @brief   Sets up a new serial connection
    * @param[in]   dev_name: serial device name.  /dev/ttyS10 or COMX
    * @param[in]   baud_rate: serial baud rate.
    * @param[in]   bDFUSEComm: bool that specifies whether to set up serial
    * for DFUSE or regular serial communication.
    *   @arg  TRUE: set up serial for DFUSE
    *   @arg  FALSE: set up serial for regular serial comms.
    * @return  None.
    */
   APIError_t setNewConnection(
         const char *dev_name,
         int baud_rate,
         bool bDFUSEComm
   );

   /**
    * @brief   Returns true if connection is set
    * @param   None
    * @return  bool
    *    @arg  true: connection has been set
    *    @arg  false: connection has not been set
    */
   bool isConnected( void );

   /**
    * @brief   Sets a new LogStub pointer.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @return: None.
    */
   void setLogging( LogStub *log );

   /**
    * This method sets a callback to handle Req msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_ReqLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setReqCallBack( CB_ReqMsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle Ack msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_AckLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setAckCallBack( CB_AckMsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle Done msgs.
    *
    * @param  [in]  pCallbackFunction: a CB_DoneLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setDoneCallBack( CB_DoneMsgHandler_t pCallbackFunction );

   /**
    * @brief   Enable all the callbacks (if set) for Req, Ack, Prog, and Done msgs.
    *
    * @param       None.
    * @return      None.
    */
   void enableMsgCallbacks( void );

   /**
    * @brief   Disable all the callbacks (if set) for Req, Ack, Prog, and Done msgs.
    *
    * @param       None.
    * @return      None.
    */
   void disableMsgCallbacks( void );

   /**
    * Constructor that sets up logging and an ethernet connection.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @param[in]   *ipAddress: pointer to the remote IP address string.
    * @param[in]   *pRemPort: pointer to the remote port number string.
    * @param[in]   *pLocPort: pointer to the local port number string.
    * @return      None.
    */
   ClientApi(
         LogStub *log,
         const char *ipAddress,
         const char *pRemPort,
         const char *pLocPort
   );

   /**
    * Constructor that sets up logging and a serial connection.
    * @param[in]   dev_name: serial device name.  /dev/ttyS10 or COMX
    * @param[in]   baud_rate: serial baud rate.
    * @param[in]   bDFUSEComm: bool that specifies whether to set up serial
    * for DFUSE or regular serial communication.
    *   @arg  TRUE: set up serial for DFUSE
    *   @arg  FALSE: set up serial for regular serial comms.
    * @return      None.
    */
   ClientApi(
         LogStub *log,
         const char *dev_name,
         int baud_rate,
         bool bDFUSEComm
   );

   /**
    * Constructor that sets up logging only.  No connection is made.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @return      None.
    */
   ClientApi( LogStub *log );

   /**
    * Destructor (default) that closes the UDP socket
    *
    * @param       None.
    * @return      None.
    */
   ~ClientApi( void );

};

#endif                                                        /* CLIENTAPI_H_ */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
