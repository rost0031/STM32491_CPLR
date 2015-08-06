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
   DC3_ReqMsgHandler_t m_pReqHandlerDC3Function; /**< Callback for handling Req msgs */
   DC3_AckMsgHandler_t m_pAckHandlerDC3Function; /**< Callback for handling Ack msgs */
   DC3_DoneMsgHandler_t m_pDoneHandlerDC3Function; /**< Callback for handling Done msgs */

   bool m_bReqLogEnable; /**< flag to enable/disable callback call for Req msgs */
   bool m_bAckLogEnable; /**< flag to enable/disable callback call for Ack msgs */
   bool m_bProgLogEnable; /**< flag to enable/disable callback call for Prog msgs */
   bool m_bDoneLogEnable; /**< flag to enable/disable callback call for Done msgs */

   unsigned int m_msgId;   /* Msg ID incrementing counter for unique msg ids. */
   bool m_bRequestProg;     /* Flag to see if progress messages are requested */
   DC3MsgRoute_t m_msgRoute; /* This is set based on the connection used (UDP vs Serial) */

   boost::thread m_workerThread;          /**< Thread to start MainMgr and QF */

   /* All the different msg structures that exist */
   struct DC3BasicMsg            m_basicMsg;
   struct DC3StatusPayloadMsg    m_statusPayloadMsg;
   struct DC3VersionPayloadMsg   m_versionPayloadMsg;
   struct DC3BootModePayloadMsg  m_bootmodePayloadMsg;
   struct DC3FlashMetaPayloadMsg m_flashMetaPayloadMsg;
   struct DC3FlashDataPayloadMsg m_flashDataPayloadMsg;
   struct DC3I2CDataPayloadMsg   m_i2cDataPayloadMsg;
   struct DC3DbgPayloadMsg       m_dbgPayloadMsg;

   uint8_t dataBuf[1000];
   int dataLen;

   /**
    * @brief   Polls for new events in the queue from MainMgr AO.
    * @param [out] *basicMsg: DC3BasicMsg pointer to the basic msg struct that
    * will contain the data on output.
    * @param [out] *payloadMsgUnion: DC3PayloadMsgUnion_t pointer to the union
    * of all the payload msgs that will contain the correct portion of itself
    * filled on output.  This union is indexed by the contents of the
    * basicMsg._msgPayload.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t pollForResp(
         DC3BasicMsg *basicMsg,
         DC3PayloadMsgUnion_t *payloadMsgUnion
   );

   /**
    * @brief   Blocks while waiting for new events in queue from MainMgr AO.
    * @param [out] *basicMsg: DC3BasicMsg pointer to the basic msg struct that
    * will contain the data on output.
    * @param [out] *payloadMsgUnion: DC3PayloadMsgUnion_t pointer to the union
    * of all the payload msgs that will contain the correct portion of itself
    * filled on output.  This union is indexed by the contents of the
    * basicMsg._msgPayload.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t waitForResp(
         DC3BasicMsg *basicMsg,
         DC3PayloadMsgUnion_t *payloadMsgUnion,
         uint16_t timeoutSecs
   );

public:

   /****************************************************************************
    *                    DC3 requests
    ***************************************************************************/
   /**
    * @brief   Blocking cmd to get the current boot mode of DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [out] *mode: DC3BootMode_t pointer to the returned boot mode from the
    * DC3 board.
    *    @arg  _DC3_Bootloader: the DC3 board is currently in bootloader mode.
    *    @arg  _DC3_Application: the DC3 board is currently in application mode.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_getMode( DC3Error_t *status, DC3BootMode_t *mode );

   /**
    * @brief   Blocking cmd to set the current boot mode of DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [in] mode: DC3BootMode_t to the mode to set the DC3 board to.
    *    @arg  _DC3_Bootloader: the DC3 board is currently in bootloader mode.
    *    @arg  _DC3_Application: the DC3 board is currently in application mode.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_setMode(DC3Error_t *status, DC3BootMode_t mode);

   /**
    * @brief   Blocking cmd to get the current boot mode of DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [in] type: DC3BootMode_t that specifies where the FW image will be
    * flashed to
    *    @arg  _DC3_Application: flash the FW image to the application space.
    * @param [in] *filename: const char pointer to a path and file where the
    * FW image file can be found.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    *
    */
   APIError_t DC3_flashFW(
         DC3Error_t *status,
         DC3BootMode_t type,
         const char *filename
   );

   /**
    * @brief   Blocking cmd to read I2C device on the DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
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
    * @param [in] dev: DC3I2CDevice_t type that specifies the I2C device to read
    *    @arg _DC3_EEPROM: read from the EEPROM on I2C bus 1.
    *    @arg _DC3_SNROM: read from the RO SerialNumber ROM on I2C bus 1.
    *    @arg _DC3_EUIROM: read from the RO Unique number ROM on I2C bus 1.
    * @param [in] acc: DC3AccessType_t  type that specifies the access to use to get
    * at the I2C bus.
    *    @arg _DC3_ACCESS_BARE: bare metal access. Blocking and slow. For testing only.
    *    @arg _DC3_ACCESS_QPC: use event driven QPC access.
    *    @arg _DC3_ACCESS_FRT: use event driven FreeRTOS access.  Available in
    *    Application only.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failures.
    */
   APIError_t DC3_readI2C(
         DC3Error_t *status,
         uint16_t *pBytesRead,
         uint8_t *pBuffer,
         const int bufferSize,
         const int bytes,
         const int start,
         const DC3I2CDevice_t dev,
         const DC3AccessType_t  acc
   );

   /**
    * @brief   Blocking cmd to write I2C device on the DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    * @param [out] *pBuffer: pointer to buffer where data to write is stored.
    * @param [in] bytes: number of bytes to write
    * @param [in] start: where to start writing to (offset)
    * @param [in] dev: DC3I2CDevice_t type that specifies the I2C device to read
    *    @arg _DC3_EEPROM: read from the EEPROM on I2C bus 1.
    *    @arg _DC3_SNROM: read from the RO SerialNumber ROM on I2C bus 1.
    *    @arg _DC3_EUIROM: read from the RO Unique number ROM on I2C bus 1.
    * @param [in] acc: DC3AccessType_t  type that specifies the access to use to get
    * at the I2C bus.
    *    @arg _DC3_ACCESS_BARE: bare metal access. Blocking and slow. For testing only.
    *    @arg _DC3_ACCESS_QPC: use event driven QPC access.
    *    @arg _DC3_ACCESS_FRT: use event driven FreeRTOS access.  Available in
    *    Application only.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failures.
    */
   APIError_t DC3_writeI2C(
         DC3Error_t *status,
         const uint8_t* const pBuffer,
         const int bytes,
         const int start,
         const DC3I2CDevice_t dev,
         const DC3AccessType_t  acc
   );

   /**
    * @brief   Blocking cmd to start a test of external RAM of DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [out] *test: DC3RamTest_t pointer to the returned RAM test that
    * failed on the DC3 board.
    *    @arg _DC3_RAM_TEST_NONE: no test failed.
    *    @arg _DC3_RAM_TEST_DATA_BUS: ram test failed in the data bus integrity
    *                                test
    *    @arg _DC3_RAM_TEST_ADDR_BUS: ram test failed in the addr bus integrity
    *                                test
    *    @arg _DC3_RAM_TEST_DEV_INT:  ram test failed in the device integrity
    *                                test
    * @param [out] *addr: address at which the ram address saw an error.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_ramTest(
         DC3Error_t *status,
         DC3RamTest_t* test,
         uint32_t* addr
   );


   /**
    * @brief   Blocking cmd to get the DBG module status from DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [out] *dbgModules: uint32_t pointer where to store the OR'ed  dbg
    * module enables.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_getDbgModules(
         DC3Error_t *status,
         uint32_t* dbgModules
   );

   /**
    * @brief   Blocking cmd to enable debugging a given module on DC3.
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [in|out] dbgModuleSet: uint32_t pointer specifying which module to
    * enable (in) and the state of all dbg modules (out) upon return.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_enDbgModule(
         DC3Error_t* status,
         uint32_t* dbgModuleSet
   );

   /**
    * @brief   Blocking cmd to enable/disable debugging module(s) on DC3.
    *
    * This function allows to both enable and disable debugging for individual
    * modules.  This is controlled by the bEnable flag, where:
    *    @arg true:  enable
    *    @arg false: disable
    *
    * as well as controlling individual debug modules vs replacing the
    * entire bitfield set. This is controlled by bOverWrite flag, where:
    *    @arg false: leave the other debug modules as they are and only change
    *                the passed in field
    *    @arg true:  overwrite the entire debug bitfield with the passed in
    *                value composed of ORed DC3DbgModule_t fields.
    *
    * @note: the bEnable flag is ignored if bOverWrite is set to true since this
    * option just sets the bitfield to whatever is passed in.
    *
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [in|out] dbgModuleSet: uint32_t pointer specifying which module to
    * enable/disable (in) and the state of all dbg modules (out) upon return.
    *
    * @param [in] bEnable: bool flag that controls whether to enable or disable
    * the passed in bitfield.
    *    @arg true:  enable
    *    @arg false: disable
    *
    * @param [in] bOverWrite: bool flag that controls whether individual debug
    * module is enabled/disabled vs replacing the entire bitfield.
    *    @arg false: leave the other debug modules as they are and only change
    *                the passed in field
    *    @arg true:  overwrite the entire debug bitfield with the passed in
    *                value composed of ORed DC3DbgModule_t fields.
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_setDbgModules(
         DC3Error_t* status,
         uint32_t* dbgModuleSet,
         bool bEnable,
         bool bOverWrite
   );

   /**
    * @brief   Blocking cmd to enable/disable debugging output over serial/eth on DC3.
    *
    * This function allows to  enable and disable debugging over serial and ethernet
    * This is controlled by the bEnable flag, where:
    *    @arg true:  enable
    *    @arg false: disable
    *
    * @param [out] *status: DC3Error_t pointer to the returned status of from
    * the DC3 board.
    *    @arg  ERR_NONE: success.
    *    other error codes if failure.
    * @note: unless this variable is set to ERR_NONE at the completion, the
    * results of other returned data should not be trusted.
    *
    * @param [in] device: DC3MsgRoute_t specifying which device to enable/disable
    * debugging output on.
    *
    * @param [in] bEnable: bool flag that controls whether to enable or disable
    * the debugging output on the specified device
    *    @arg true:  enable
    *    @arg false: disable
    *
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t DC3_setDbgDevice(
         DC3Error_t* status,
         DC3MsgRoute_t device,
         bool bEnable
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
         const char* ipAddress,
         const char* pRemPort,
         const char* pLocPort
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
    * @param  [in]  pCallbackFunction: a DC3_ReqLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setReqCallBack( DC3_ReqMsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle Ack msgs.
    *
    * @param  [in]  pCallbackFunction: a DC3_AckLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setAckCallBack( DC3_AckMsgHandler_t pCallbackFunction );

   /**
    * This method sets a callback to handle Done msgs.
    *
    * @param  [in]  pCallbackFunction: a DC3_DoneLogHandler_t pointer to the
    * callback function that is implemented outside the library.
    *
    * @return APIError_t:
    *    @arg API_ERR_NONE: no errors were detected
    *    else some error code indicating what went wrong
    */
   APIError_t setDoneCallBack( DC3_DoneMsgHandler_t pCallbackFunction );

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
