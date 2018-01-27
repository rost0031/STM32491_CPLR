// $Id$
/**
 * @file   dfuse.h
 * @brief  This file contains all the definitions for DFuSE API used by STM32
 * chips to flash their firmware using the builtin ROM bootloader.
 *
 * @date   03/14/2013
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 */
// $Log$

#ifndef _DFUSE_H_
#define _DFUSE_H_

#include <stdint.h>
#include <sys/types.h>
#include <limits.h> //For CHAR_BIT
#include <assert.h>

#define             DFUSE_MAX_CMDS      12
#define             DFUSE_MAX_RETRIES   4

#define             DFUSE_MAX_SIZE      252

#define             DFUSE_ACK           0x79
#define             DFUSE_NACK          0x1F
#define             DFUSE_CONNECT       0x7F

#define             DFUSE_BAUD          115200

/******************************************************************************/
typedef enum {
    DFUSE_CMD_GET                       = 0x00,
    DFUSE_CMD_GET_VER                   = 0x01,
    DFUSE_CMD_GET_ID                    = 0x02,
    DFUSE_CMD_READ_MEM                  = 0x11,
    DFUSE_CMD_GO                        = 0x21,
    DFUSE_CMD_WRITE_MEM                 = 0x31,
    DFUSE_CMD_ERASE                     = 0x43,
    DFUSE_CMD_EXT_ERASE                 = 0x44,
    DFUSE_CMD_WRITE_PROTECT             = 0x63,
    DFUSE_CMD_WRITE_UNPROTECT           = 0x73,
    DFUSE_CMD_READ_PROTECT              = 0x82,
    DFUSE_CMD_READ_UNPROTECT            = 0x92
} DFUSE_Command;

/******************************************************************************/
typedef enum {
    DFUSE_NO_ERROR                      = 0,
    DFUSE_ERR_MISSING_LEADING_ACK       = 100,
    DFUSE_ERR_MISSING_TERMINATING_ACK   = 101,
    DFUSE_ERR_INVALID_LEN               = 102,
    DFUSE_ERR_INVALID_LEN_W_LEAD_ACK    = 103,
    DFUSE_ERR_INVALID_LEN_WOUT_LEAD_ACK = 104,
    DFUSE_ERR_INVALID_NUM_CMDS          = 105, //Wrong number of commands got sent back from Dfuse Bootloader
    DFUSE_ERR_INVALID_BOOTLOADER_VER    = 106, //Invalid Dfuse Bootloader Version
    DFUSE_ERR_INVALID_PROD_ID           = 107, //Invalid Product ID

    DFUSE_ERR_BYTE_ERROR                = 200,
    /* Range 200-356 is reserved for byte errors. Any function can return error
     * 200 + byte number to signify where an error occurred. */
    DFUSE_ERR_MAX_BYTE_ERROR            = 356

} DFUSE_Error;

/**
 * Checks if a given DFUSE API cmd is supported.  Not all versions of DFUSE API
 * support all commands.
 *
 * @param   cmd: an DFUSE_Commands type that specifies the command
 * @return  uint8_t:
 *  @arg    1: cmd is supported
 *  @arg    0: cmd is not supported
 */
uint8_t DFUSE_isCmdSupported( DFUSE_Command cmd );

/**
 * Returns the starting address of the Flash memory on the DFUSE device.  This
 * must be called after the DFUSE_CMD_GET and DFUSE_CMD_GET_ID have been sent
 * and parsed.
 *
 * @param   None
 * @return  uint32_t: address of the beginning of Flash memory.
 */
uint32_t DFUSE_getStartFlashAddr( void );

/******************************************************************************/
/*****   Functions for creating DFUSE commands ready to send over serial  *****/
/*****   and their corresponding Response Handlers ****************************/
/******************************************************************************/

/**
 * Fills a passed in buffer with the correct data that corresponds to the
 * command to send to DFUSE API. This data can then be sent to the DFU device
 * directly over the serial port.  See AN3155 for the details of the DFUSE
 * serial protocol.
 *
 * @param [in|out]  buffer: pointer to a buffer that will contain the DFUSE command
 * @param [in]      command: DFUSE_Command type that specifies which command to
 * create a buffer for.
 *      @arg DFUSE_CONNECT: The Get command allows you to connect to the DFUSE
 *      API.  It's a single byte command {0x7F} and it doesn't get a full reply.
 *      Instead it just awaits for an ACK {0x79}.  Once this is received, other
 *      commands can be sent.
 *      A simple way to implement this is as follows:
 *      @code
 *      void ConnectToDFUSE( void )
 *      {
 *          char data[256];
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CONNECT );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read a single DFUSE_ACK from serial
 *
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_GET: The Get command allows you to get the version of the
 *      bootloader and the supported commands. When the bootloader receives the
 *      Get command, it transmits the bootloader version and the supported
 *      command codes to the host. The command sent is {0x00 0xFF} and the
 *      response can be parsed by DFUSE_handleResp_GetCmd() function.
 *      A simple way to implement this is as follows:
 *      @code
 *      void Send_CmdGet( void )
 *      {
 *          char data[256];
 *          char rec_data[256];
 *
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CMD_GET );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read a single DFUSE_ACK from serial
 *          SerialRead( usart, rec_data, DFUSE_ACK );  // Read serial until a DFUSE_ACK is seen (should be 14 bytes including the ACK)
 *          uint8_t error = DFUSE_handleResp_Get( rec_data, 14 );
 *
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_GET_VER: The Get Version & Read Protection Status command is
 *      used to get the bootloader version and the read protection status. When
 *      the bootloader receives the command, it transmits the version, read
 *      protection: number of times it was enabled and disabled to the host. The
 *      command sent is {0x01 0xFE} and the response can be parsed by
 *      DFUSE_handleResp_GetVer() function.
 *      A simple way to implement this is as follows:
 *      @code
 *      void Send_CmdGetVer( void )
 *      {
 *          char data[256];
 *          char rec_data[256];
 *
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CMD_GET_VER );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read a single DFUSE_ACK from serial
 *          SerialRead( usart, rec_data, DFUSE_ACK );  // Read serial until a DFUSE_ACK is seen (should be 4 bytes including the ACK)
 *          uint8_t error = DFUSE_handleResp_GetVer( rec_data, 4 );
 *
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_GET_ID: The Get ID command is used to get the version of the
 *      chip ID (identification). When the bootloader receives the command, it
 *      transmits the product ID to the host. The command sent is {0x02 0xFD} and
 *      the response can be parsed by DFUSE_handleResp_GetID() function.
 *      A simple way to implement this is as follows:
 *      @code
 *      void Send_CmdGetID( void )
 *      {
 *          char data[256];
 *          char rec_data[256];
 *
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CMD_GET_ID );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read a single DFUSE_ACK from serial
 *          SerialRead( usart, rec_data, DFUSE_ACK );  // Read serial until a DFUSE_ACK is seen (should be 4 bytes including the ACK)
 *          uint8_t error = DFUSE_handleResp_GetID( rec_data, 4 );
 *
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_READ_MEM:    Currently Unimplemented
 *      @arg DFUSE_CMD_GO:          Currently Unimplemented
 *      @arg DFUSE_CMD_WRITE_MEM: The WRITE command is used to write data to the
 *      flash memory. The command is sent as follows from host POV:
 *      Host->DFUSE: Byte 1: 0x31
 *      Host->DFUSE: Byte 2: 0xCE
 *      DFUSE->Host: Wait for ACK
 *
 *      To Send the address (for example 0x08003720 address),
 *      Host->DFUSE: Byte 3: 0x08
 *      Host->DFUSE: Byte 4: 0x00
 *      Host->DFUSE: Byte 5: 0x37
 *      Host->DFUSE: Byte 6: 0x20
 *      Host->DFUSE: Byte 7: 0x1F (XOR of previous 4 bytes or use the
 *      DFUSE_createWMAddrToSend() function to create the appropriate buffer).
 *      DFUSE->Host: Wait for ACK
 *
 *      Host->DFUSE: Byte 8: N - number of bytes to be written (1 byte), the data
 *      (N+1 bytes) and the checksum.  Use DFUSE_createWMDataToSend() function
 *      to create the appropriate buffer ready to send.
 *      DFUSE->Host: Wait for ACK
 *      End.
 *
 *      A simple way to implement this is as follows:
 *      @code
 *      void WriteMemoryOverDFUSE( void )
 *      {
 *          #define MAX_LEN 256
 *          #define MAX_DATA 252
 *
 *          usart = UART2; // This is specific to your hardware
 *          uint32_t addr = 0x08000000 //Some flash memory address
 *          char buffer[MAX_LEN]; //some buffer that's big enough to hold the largest dataset (252)
 *          char data[MAX_DATA]; // some data buffer that contains the data packets to write to flash
 *          uint8_t data_len = 112; // how much data is in the data array
 *
 *          uint8_t cmd_len = DFUSE_createCmd( buffer, DFUSE_CMD_WRITE_MEM ); //call to the DFUSE library
 *          SerialWrite( usart, buffer, cmd_len ); // your hardware specific serial write function
 *          WaitForAck();
 *
 *          uint8_t addr_to_send = DFUSE_createWMAddrToSend( buffer, MAX_LEN, addr ); // create a buffer with address in it
 *          SerialWrite( usart, buffer, addr_to_send ); // your hardware specific serial write function
 *          WaitForAck();
 *
 *          uint8_t data_to_send = DFUSE_createWMDataToSend( buffer, MAX_LEN, data, data_len );
 *          SerialWrite( usart, buffer, data_to_send );
 *          WaitForAck();
 *
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_ERASE: The ERASE command is used to wipe the flash memory.
 *      The command is sent as follows from host POV:
 *      Host->DFUSE: Byte 1: 0x43
 *      Host->DFUSE: Byte 2: 0xBC
 *      DFUSE->Host: Wait for ACK
 *      Host->DFUSE: Byte 3: 0xFF or number of pages to be erased . 1 (0 . N .
 *      maximum number of pages)
 *      Host->DFUSE: Byte 4: 0x00 (in case of global erase) or ((N + 1 bytes
 *      (page numbers) and then checksum XOR (N, N+1 bytes)).  There is no
 *      handler function needed for this command but the host is responsible for
 *      making sure that it sends the correct data to correctly implement the
 *      rest of the protocol (the data needed after the first ACK).
 *      A simple way to implement this is as follows:
 *      @code
 *      void Send_CmdErase( void )
 *      {
 *          char data[256];
 *
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CMD_ERASE );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read a single DFUSE_ACK from serial
 *
 *          uint8_t bytes_to_send = 2;
 *          data[0] = 0xFF;
 *          data[1] = 0x00;
 *          SerialWrite( usart, data, bytes_to_send ); // 0xFF,0x00 erases everything
 *          WaitForAck(); // Read a single DFUSE_ACK from serial
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_EXT_ERASE:   Currently Unimplemented
 *      @arg DFUSE_CMD_WRITE_PROTECT:  Currently Unimplemented
 *
 *      @arg DFUSE_CMD_WRITE_UNPROTECT: This command is used to disable write
 *      protection of the flash memory.
 *      The command is sent as follows from host POV:
 *      Host->DFUSE: Byte 1: 0x92
 *      Host->DFUSE: Byte 2: 0x6D
 *      DFUSE->Host: Wait for ACK
 *      DFUSE->Host: Wait for ACK followed by system reset of the DFUSE bootloader
 *      A simple way to implement this is as follows:
 *      @code
 *      void Send_CmdWriteUnprotect( void )
 *      {
 *          char data[256];
 *
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CMD_WRITE_UNPROTECT );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read first DFUSE_ACK from serial
 *          WaitForAck(); // Read second DFUSE_ACK from serial
 *
 *          ResetDFUSEDevice(); // Your function for resetting the dfuse device
 *          ConnectToDFUSE();  // Reconnect to the dfuse device
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_READ_PROTECT: This command is used to enable read protection
 *      of the flash memory. The command is sent as follows from host POV:
 *      Host->DFUSE: Byte 1: 0x82
 *      Host->DFUSE: Byte 2: 0x7D
 *      DFUSE->Host: Wait for ACK
 *      DFUSE->Host: Wait for ACK followed by system reset of the DFUSE bootloader
 *      It is recommended that you reset the reboot the DFUSE device and reconnect
 *      using the DFUSE_CONNECT command.
 *      A simple way to implement this is as follows:
 *      @code
 *      void Send_CmdReadProtect( void )
 *      {
 *          char data[256];
 *
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CMD_READ_PROTECT );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read first DFUSE_ACK from serial
 *          WaitForAck(); // Read second DFUSE_ACK from serial
 *
 *          ResetDFUSEDevice(); // Your function for resetting the dfuse device
 *          ConnectToDFUSE();  // Reconnect to the dfuse device
 *          return;
 *      }
 *      @endcode
 *
 *      @arg DFUSE_CMD_READ_UNPROTECT: This command is used to disable read
 *      protection of the flash memory, and in the process erasing the flash.
 *      The command is sent as follows from host POV:
 *      Host->DFUSE: Byte 1: 0x92
 *      Host->DFUSE: Byte 2: 0x6D
 *      DFUSE->Host: Wait for ACK
 *      DFUSE->Host: Wait for ACK followed by system reset of the DFUSE bootloader
 *      A simple way to implement this is as follows:
 *      @code
 *      void Send_CmdReadUnprotect( void )
 *      {
 *          char data[256];
 *
 *          uint8_t cmd_len = DFUSE_createCmd( data, DFUSE_CMD_READ_UNPROTECT );
 *          SerialWrite( usart, data, cmd_len );
 *          WaitForAck(); // Read first DFUSE_ACK from serial
 *          WaitForAck(); // Read second DFUSE_ACK from serial
 *
 *          ResetDFUSEDevice(); // Your function for resetting the dfuse device
 *          ConnectToDFUSE();  // Reconnect to the dfuse device
 *          return;
 *      }
 *      @endcode
 *
 * @return          uint8_t: number of bytes in the buffer upon completion.
 */
uint8_t DFUSE_createCmd(
      uint8_t *buffer,
      DFUSE_Command command
);

/**
 * Response handler for the GET command.  Upon receiving the GET command, the
 * STM32 DFUSE interface sends the following:
 * Byte 1: ACK
 * Byte 2: N = 11 = the number of bytes to follow � 1 except current and ACKs.
 * Byte 3: Bootloader version (0 < Version < 255), example: 0x10 = Version 1.0
 * Byte 4: 0x00 � Get command
 * Byte 5: 0x01 � Get Version (and Read Protection Status)
 * Byte 6: 0x02 � Get ID
 * Byte 7: 0x11 � Read Memory command
 * Byte 8: 0x21 � Go command
 * Byte 9: 0x31 � Write Memory command
 * Byte 10: 0x43 or 0x44 � Erase command or Extended Erase command (mutually exclusive)
 * Byte 11: 0x63 � Write Protect command
 * Byte 12: 0x73 � Write Unprotect command
 * Byte 13: 0x82 � Readout Protect command
 * Byte 14: 0x92 � Readout Unprotect command
 * Byte 15: ACK
 *
 * The leading ACK can handled inside or outside of this handler function.
 *
 * @param   response: pointer to the buffer containing the response to the
 *          DFUSE_CMD_GET with or without the leading ACK
 * @param   len: number of bytes in the response buffer.
 * @return  uint16_t:
 *  @arg    DFUSE_NO_ERROR                      : successfully handled the response.
 *  or
 *  @arg    DFUSE_ERR_INVALID_NUM_CMDS          : Wrong number of commands got sent back from Dfuse Bootloader
 *  @arg    DFUSE_ERR_INVALID_BOOTLOADER_VER    : Invalid Dfuse Bootloader Version
 *  @arg    DFUSE_ERR_MISSING_TERMINATING_ACK   : Last byte in response was not an ACK
 *  @arg    DFUSE_ERR_INVALID_LEN_WOUT_LEAD_ACK : Invalid response length without leading ACK.  Expected 14
 *  @arg    DFUSE_ERR_INVALID_LEN_W_LEAD_ACK    : Invalid response length with leading ACK.  Expected 15
 *  @arg    DFUSE_ERR_BYTE_ERROR + n            : Invalid Dfuse cmd at byte n
 *
 */
uint16_t DFUSE_handleResp_Get(
      uint8_t *response,
      uint8_t len
);

/**
 * Response handler for the GET_VER command.  Upon receiving the GET command,
 * the STM32 DFUSE interface sends the following:
 * Byte 1: ACK
 * Byte 2: Bootloader version (0 < Version < 255), example: 0x10 = Version 1.0
 * Byte 3: Option byte 1: 0x00 to keep the compatibility with generic bootloader protocol
 * Byte 4: Option byte 2: 0x00 to keep the compatibility with generic bootloader protocol
 * Byte 5: ACK
 *
 * The leading ACK can handled inside or outside of this handler function.
 *
 * @param   response: pointer to the buffer containing the response to the
 *          GET_VER_AND_READ_PROTECT_STAT with or without the leading ACK.
 * @param   len: number of bytes in the response buffer.
 * @return  uint8_t:
 *  @arg    DFUSE_NO_ERROR                      : successfully handled the response.
 *  or
 *  @arg    DFUSE_ERR_INVALID_NUM_CMDS          : Wrong number of commands got sent back from Dfuse Bootloader
 *  @arg    DFUSE_ERR_INVALID_BOOTLOADER_VER    : Invalid Dfuse Bootloader Version
 *  @arg    DFUSE_ERR_MISSING_TERMINATING_ACK   : Last byte in response was not an ACK
 *  @arg    DFUSE_ERR_INVALID_LEN_WOUT_LEAD_ACK : Invalid response length without leading ACK.  Expected 4
 *  @arg    DFUSE_ERR_INVALID_LEN_W_LEAD_ACK    : Invalid response length with leading ACK.  Expected 5
 */
uint16_t DFUSE_handleResp_GetVer(
      uint8_t *response,
      uint8_t len
);

/**
 * Response handler for the GET_ID. Upon receiving the GET_ID command, the STM32
 * DFUSE interface sends the following:
 * Byte 1: ACK
 * Byte 2: N = the number of bytes � 1 (N = 1 for STM32), except for current byte and ACKs.
 * Byte 3: 0x04;   Most Sig Byte of Prod ID Code
 * Byte 4: 0x1X;   Least Sig Byte of Prod ID Code
 * Byte 5: ACK
 *
 * The leading ACK can handled inside or outside of this handler function.
 *
 * @param   response: pointer to the buffer containing the response to the
 *          GET_ID not including the first ACK
 * @param   len: number of bytes in the response buffer.
 * @return  uint8_t:
 *  @arg    DFUSE_NO_ERROR                      : successfully handled the response.
 *  or
 *  @arg    DFUSE_ERR_INVALID_NUM_CMDS          : Wrong number of commands got sent back from Dfuse Bootloader
 *  @arg    DFUSE_ERR_MISSING_TERMINATING_ACK   : Last byte in response was not an ACK
 *  @arg    DFUSE_ERR_INVALID_LEN_WOUT_LEAD_ACK : Invalid response length without leading ACK.  Expected 4
 *  @arg    DFUSE_ERR_INVALID_LEN_W_LEAD_ACK    : Invalid response length with leading ACK.  Expected 5
 *  @arg    DFUSE_ERR_INVALID_PROD_ID           : Invalid product ID from DFUSE
 */
uint16_t DFUSE_handleResp_GetID(
      uint8_t *response,
      uint8_t len
);

/**
 * Creates an address in a buffer with the appropriate checksum to send to the
 * DFUSE device as part of the WRITE_MEMORY cmd.
 *
 * @param [in|out]  buffer: pointer to a buffer that will contain the DFUSE command
 * @param [in]      buffer_size: size of the buffer being passed in.
 * @param [in]      addr: a uint32_t that is the address at which to start the MEM_WRITE cmd.

 * @return          uint8_t: number of bytes in the buffer upon completion.
 */
uint8_t DFUSE_createWMAddrToSend(
      uint8_t *buffer,
      uint8_t buffer_size,
      uint32_t addr
);

/**
 * Creates a correctly formed data buffer with the appropriate checksum to send
 * the actual data to write to flash to the DFUSE device as part of the
 * WRITE_MEMORY cmd.
 *
 * @param [in|out]  buffer: pointer to a buffer that will contain the DFUSE command.
 * @param [in]      buffer_size:  how many bytes the buffer can hold.
 * @param [in]      data:  pointer to the buffer that contains the fw data.
 * @param [in]      data_size:  how many bytes of data there are.
 * @return          uint16_t: number of bytes in the final p_buffer upon completion.
 */
uint16_t DFUSE_createWMDataToSend(
      uint8_t *buffer,
      uint8_t buffer_size,
      uint8_t *data,
      uint8_t data_size
);

/**
 * Creates an address in a buffer with the appropriate checksum to send to the
 * DFUSE device as part of the READ_MEMORY cmd.  This function just calls the
 * DFUSE_createWMAddrToSend() since they both perform exactly the same action.
 *
 * @param [in|out]  buffer: pointer to a buffer that will contain the DFUSE command
 * @param [in]      buffer_size: size of the buffer being passed in.
 * @param [in]      addr: a uint32_t that is the address at which to start the MEM_WRITE cmd.

 * @return          uint8_t: number of bytes in the buffer upon completion.
 */
uint8_t DFUSE_createRMAddrToSend(
      uint8_t *buffer,
      uint8_t buffer_size,
      uint32_t addr
);

/**
 * Creates a correctly formed data buffer with the appropriate checksum to send
 * the number of bytes to read to the DFUSE device as part of the READ_MEMORY cmd.
 *
 * @param [in|out]  buffer: pointer to a buffer that will contain the DFUSE command.
 * @param [in]      buffer_size:  how many bytes the buffer can hold.
 * @param [in]      data:  pointer to the buffer that contains the fw data.
 * @param [in]      data_size:  how many bytes of data there are.
 * @return          uint8_t: number of bytes in the final p_buffer upon completion.
 */
uint8_t DFUSE_createRMDataToRead(
      uint8_t *cmd_buffer,
      uint8_t cmd_buffer_size,
      uint8_t bytes_to_read
);

#endif                                                           /* _DFUSE_H_ */
/******** Copyright (C) 2013 Harry Rostovtsev. All rights reserved *****END OF FILE****/
