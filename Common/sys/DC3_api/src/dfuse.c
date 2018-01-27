// $Id$
/**
 * @file   dfuse.c
 * @brief  This file contains all the definitions for DFuSE API used by STM32
 * chips to flash their firmware using the builtin ROM bootloader.
 *
 * @date   03/14/2013
 * @author Harry Rostovtsev
 * @email  rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 */
// $Log$

#include <string.h>
#include "dfuse.h"
#include "stdio.h"
/******************************************************************************/
enum {
    ID_LOW_DENSITY                      = 0x0412,
    ID_MED_DENSITY                      = 0x0410,
    ID_HI_DENSITY                       = 0x0414,
    ID_CONNECTIVITY                     = 0x0418,
    ID_MED_DENSITY_VALUE                = 0x0420,
    ID_HI_DENSITY_VALUE                 = 0x0428,
    ID_XL_DENSITY                       = 0x0430,
    ID_MED_DENSITY_ULTRA_LOW_POWER      = 0x0436,
    ID_HI_DENSITY_ULTRA_LOW_POWER       = 0x0416,
    ID_STM32F2XX                        = 0x0411,
};

typedef struct {
    uint8_t     bootloaderVer;
    uint8_t     commands[DFUSE_MAX_CMDS];
    uint32_t    flashBeginAddr;
    uint32_t    flashEndAddr;
    uint16_t    flashPagesPerSector;
    size_t      flashPageSize;
    uint32_t    eraseDelay;
    uint32_t    writeDelay;
} DFUSE_DeviceParams;

static DFUSE_DeviceParams devParams = {
    0xFF,                       // bootloaderVer
    {                           // commands
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0
    },
    0x08000000,                 // flashBeginAddr
    0x08008000,                 // flashEndAddr
    4,                          // flashPagesPerSector
    1024,                       // flashPageSize
    40000,                      // eraseDelay
    80000                       // writeDelay
};

/******************************************************************************/
/**
 * Gets the max DFUSE command index
 *
 * @param   cmd: an DFUSE_Commands type that specifies the command
 * @return  int8_t: index into the command array that the command lives at.
 *  @arg    -1 if not found.
 */
int8_t DFUSE_getCmdIndex( DFUSE_Command cmd );
int8_t DFUSE_getCmdIndex( DFUSE_Command cmd )
{
    int8_t idx = -1;
    switch( cmd ) {
    case DFUSE_CMD_GET:                   idx++;/* Intentionally fall through */
    case DFUSE_CMD_GET_VER:               idx++;/* Intentionally fall through */
    case DFUSE_CMD_GET_ID:                idx++;/* Intentionally fall through */
    case DFUSE_CMD_READ_MEM:              idx++;/* Intentionally fall through */
    case DFUSE_CMD_GO:                    idx++;/* Intentionally fall through */
    case DFUSE_CMD_WRITE_MEM:             idx++;/* Intentionally fall through */
    case DFUSE_CMD_ERASE:                 idx++;/* Intentionally fall through */
    case DFUSE_CMD_EXT_ERASE:             idx++;/* Intentionally fall through */
    case DFUSE_CMD_WRITE_PROTECT:         idx++;/* Intentionally fall through */
    case DFUSE_CMD_WRITE_UNPROTECT:       idx++;/* Intentionally fall through */
    case DFUSE_CMD_READ_PROTECT:          idx++;/* Intentionally fall through */
    case DFUSE_CMD_READ_UNPROTECT:        idx++;/* Intentionally fall through */
    default:                  break;
    }
    assert( idx < DFUSE_MAX_CMDS );
    return idx;
}

/******************************************************************************/
uint8_t DFUSE_isCmdSupported( DFUSE_Command cmd )
{
    int8_t idx = DFUSE_getCmdIndex( cmd );
    assert( idx >= 0 );
    return devParams.commands[ idx ];
}

/******************************************************************************/
/**
 * Clears the internal device parameters array of which commands are supported
 *
 * @param   None
 * @return  None
 */
void DFUSE_clearSupportedCmds( void );
void DFUSE_clearSupportedCmds( void )
{
    for( uint8_t i = 0; i < DFUSE_MAX_CMDS; ++i ) {
        devParams.commands[i] = 0;
    }
}

/******************************************************************************/
/**
 * Set DFUSE bootloader version
 *
 * @param   bootloaderVer: uint8_t variable that holds the new bootloader version
 * @return  None
 */
void DFUSE_setBootloaderVer( uint8_t bootloaderVer );
void DFUSE_setBootloaderVer( uint8_t bootloaderVer )
{
    devParams.bootloaderVer = bootloaderVer;
}

/******************************************************************************/
/**
 * Mark a DFUSE cmd as valid and supported by this version of DFUSE API
 *
 * @param   cmd: DFUSE_Command which to mark as supported/valid
 * @return  None
 */
void DFUSE_markCmdValid( DFUSE_Command cmd );
void DFUSE_markCmdValid( DFUSE_Command cmd )
{
    int8_t idx = DFUSE_getCmdIndex( cmd );
    if(idx >= 0) {
        devParams.commands[ idx ] = 1;
    }
}

/******************************************************************************/
/**
 * Sets the flash memory parameters for this device based on the id returned
 * from the DFUSE API supporting device.
 *
 * @param   id: response to DFUSE_CMD_GET_ID command sent to the DFUSE device.
 * @return  uint8_t:
 *  @arg    1: successfully matched the id to a known device.
 *  @arg    0: unknown device id.  Failed to set flash parameters.
 */
uint8_t DFUSE_setFlashMemParams( uint16_t id );
uint8_t DFUSE_setFlashMemParams( uint16_t id )
{
    switch(id) {
        case ID_LOW_DENSITY:
            devParams.flashEndAddr          = 0x08008000;
            break;
        case ID_MED_DENSITY:
            devParams.flashEndAddr          = 0x08020000;
            break;
        case ID_HI_DENSITY:
            devParams.flashEndAddr          = 0x08080000;
            devParams.flashPagesPerSector   = 2;
            devParams.flashPageSize         = 2048;
            break;
        case ID_CONNECTIVITY:
            devParams.flashEndAddr          = 0x08040000;
            devParams.flashPagesPerSector   = 2;
            devParams.flashPageSize         = 2048;
            break;
        case ID_MED_DENSITY_VALUE:
            devParams.flashEndAddr          = 0x08020000;
            break;
        case ID_HI_DENSITY_VALUE:
            devParams.flashEndAddr          = 0x08080000;
            devParams.flashPagesPerSector   = 2;
            devParams.flashPageSize         = 2048;
            break;
        case ID_XL_DENSITY:
            devParams.flashEndAddr          = 0x08100000;
            devParams.flashPagesPerSector   = 2;
            devParams.flashPageSize         = 2048;
            break;
        case ID_MED_DENSITY_ULTRA_LOW_POWER:
            devParams.flashEndAddr          = 0x08060000;
            devParams.flashPagesPerSector   = 16;
            devParams.flashPageSize         = 256;
            break;
        case ID_HI_DENSITY_ULTRA_LOW_POWER:
            devParams.flashEndAddr          = 0x08020000;
            devParams.flashPagesPerSector   = 16;
            devParams.flashPageSize         = 256;
            break;
        case ID_STM32F2XX:
            devParams.flashEndAddr          = 0x080FFFFF;
            break;
        default:
            return 1;
    }

    return DFUSE_NO_ERROR;
}

/******************************************************************************/
uint32_t DFUSE_getStartFlashAddr( void )
{
    return devParams.flashBeginAddr;
}

/******************************************************************************/
uint8_t DFUSE_createCmd( uint8_t *buffer, DFUSE_Command command )
{
    uint8_t cmd_len = 0;
    if ( DFUSE_CONNECT == command ) {
        buffer[0] = command;
        cmd_len = 1;
    } else {
        /* Make sure command exists */
        if ( 0 <= DFUSE_getCmdIndex( command ) ) {
            buffer[0] = command;
            buffer[1] = ~command;
            cmd_len = 2;
        } else {
            cmd_len = 0;
        }
    }
    return cmd_len;
}

/******************************************************************************/
uint16_t DFUSE_handleResp_Get( uint8_t *response, uint8_t len )
{
    /* Allow the buffer to start with ACK or not. */
    if ( len == 15 ) {
        if ( DFUSE_ACK == response[0] ) {
            /* Effectively drop the ACK at the beginning */
            response++;
            len--;
        } else {
            return ( DFUSE_ERR_INVALID_LEN_W_LEAD_ACK );
        }
    } else if ( len != 14 ) {
        /* Sanity check: len for this response should be 14 if no leading ACK */
        return ( DFUSE_ERR_INVALID_LEN_WOUT_LEAD_ACK );
    }

    if ( DFUSE_ACK != response[len-1] ) {
        /* Sanity check: last byte should be an ACK */
        return ( DFUSE_ERR_MISSING_TERMINATING_ACK );
    }

    int8_t cmd_idx;

    for ( uint8_t i = 0; i < len; i++ ) {
        switch ( i ) {
            case 0:
                /* First byte should be 11 or MAX_DFUSE_CMDS - 1.  It's how many
                 * commands are allowed: 2 of the commands are mutually exclusive
                 * so only 11 are actually allowed */
                if ( DFUSE_MAX_CMDS - 1 != response[i] ) {
                    return ( DFUSE_ERR_INVALID_NUM_CMDS );
                }
                break;

            case 1:
                /* Second byte is the bootloader version. Only 1-255 is allowed */
                if ( response[i] < 0 || response[i] > 255 ) {
                    return ( DFUSE_ERR_INVALID_BOOTLOADER_VER );
                } else {
                    devParams.bootloaderVer = response[i];
                }
                break;

            case 13:
                /* This should be the closing ACK. */
                if ( DFUSE_ACK != response[i] ) {
                    return ( DFUSE_ERR_MISSING_TERMINATING_ACK );
                }
                break;

            default:
                /* These are allowed commands */
                cmd_idx = DFUSE_getCmdIndex( (DFUSE_Command) response[i] );
                if ( cmd_idx < 0 ) {
                    /* Command not found in list of known commands */
                    return ( i + DFUSE_ERR_BYTE_ERROR );
                } else {
                    /* Command exists.  Mark it valid */
                    DFUSE_markCmdValid( (DFUSE_Command) response[i] );
                }

                break;
        }
    }

    /* If here, no errors were encountered. */
    return DFUSE_NO_ERROR;
}

/******************************************************************************/
uint16_t DFUSE_handleResp_GetVer( uint8_t *response, uint8_t len )
{
    /* Allow the buffer to start with ACK or not. */
    if ( len == 5 ) {
        if ( DFUSE_ACK == response[0] ) {
            /* Effectively drop the ACK at the beginning */
            response++;
            len--;
        } else {
            return ( DFUSE_ERR_INVALID_LEN_W_LEAD_ACK );
        }
    } else if ( len != 4 ) {
        /* Sanity check: len for this response should be 14 if no leading ACK */
        return ( DFUSE_ERR_INVALID_LEN_WOUT_LEAD_ACK );
    }

    if ( DFUSE_ACK != response[len-1] ) {
        /* Sanity check: last byte should be an ACK */
        return ( DFUSE_ERR_MISSING_TERMINATING_ACK );
    }

    for ( uint8_t i = 0; i < len; i++ ) {
        switch ( i ) {
            case 0:
                /* First byte is the bootloader version. Only 1-255 is allowed */
                if ( response[i] < 0 || response[i] > 255 ) {
                    return ( DFUSE_ERR_INVALID_BOOTLOADER_VER );
                } else {
                    devParams.bootloaderVer = response[i];
                }
                break;

            case 3:
                /* This should be the closing ACK. */
                if ( DFUSE_ACK != response[i] ) {
                    return ( DFUSE_ERR_MISSING_TERMINATING_ACK );
                }
                break;

            default:
                /* Do nothing.  These bytes should be zero for compatibility,
                 * according to AN3155 */
                break;
        }
    }

    /* If here, no errors were encountered. */
    return DFUSE_NO_ERROR;
}

/******************************************************************************/
uint16_t DFUSE_handleResp_GetID( uint8_t* response, uint8_t len )
{
    /* Allow the buffer to start with ACK or not. */
    if ( len == 5 ) {
        if ( DFUSE_ACK == response[0] ) {
            /* Effectively drop the ACK at the beginning */
            response++;
            len--;
        } else {
            return ( DFUSE_ERR_INVALID_LEN_W_LEAD_ACK );
        }
    } else if ( len != 4 ) {
        /* Sanity check: len for this response should be 14 if no leading ACK */
        return ( DFUSE_ERR_INVALID_LEN_WOUT_LEAD_ACK );
    }

    if ( DFUSE_ACK != response[len-1] ) {
        /* Sanity check: last byte should be an ACK */
        return ( DFUSE_ERR_MISSING_TERMINATING_ACK );
    }

    /* First byte is number of bytes in PID which for STM32 is 1,
     * according to AN3155 from ST Micro.  Byte 1 and 2 are the major and minor
     * versions.  Major version should always be 0x04 for STM32 */
    uint16_t id = response[1] << CHAR_BIT | response[2];

    if ( DFUSE_NO_ERROR != DFUSE_setFlashMemParams( id ) ) {
        return DFUSE_ERR_INVALID_PROD_ID;
    }

    /* If here, no errors were encountered. */
    return DFUSE_NO_ERROR;
}

/******************************************************************************/
uint8_t DFUSE_createWMAddrToSend(
      uint8_t *buffer,
      uint8_t buffer_size,
      uint32_t addr
)
{
    /* Make sure that address is correctly aligned and buffer is big enough */
    assert(addr % 4 == 0);
    assert(buffer_size >= 5);

    /* Clear the buffer */
    memset( buffer, 0, 5 );

    for( uint8_t i = 0; i < 4; i++ ) {
        buffer[i] = (uint8_t)(addr >> ((3 - i) * CHAR_BIT));
        buffer[4] ^= buffer[i];
    }

    return 5;
}

/******************************************************************************/
uint16_t DFUSE_createWMDataToSend(
      uint8_t *buffer,
      uint8_t buffer_size,
      uint8_t *data,
      uint8_t data_size
)
{
    /* Make sure the data size and output buffers are properly sized */
    if ( data_size >= DFUSE_MAX_SIZE )
    {
       /* If user is trying to fill with more data than max allowed, disallow */
       return( 0 );
    }

    if ( buffer_size < ( data_size + 2 ) )
    {
       /* If user is trying to fill with more data than available buffer space,
        * disallow */
       return( 0 );
    }

    /* Clear the buffer */
    memset( buffer, 0, buffer_size );

    uint8_t padding = ( 4 - ( data_size % 4 ) ) % 4;
    uint8_t n = data_size + padding - 1;
    uint8_t checksum = n;

    /* N goes in the first byte of the buffer to be sent */
    buffer[0] = n;
    uint8_t buffer_ind = 1;

    /* Copy over the actual data (while calculating the checksum) in to the
     * send buffer */
    for( uint8_t i = 0; i < data_size; i++,buffer_ind++ ) {
        buffer[buffer_ind] = data[i];
        checksum ^= data[i];
    }

    /* Copy over the padding (while calculating the checksum) in to the
     * send buffer */
    for( size_t i = 0; i < padding; i++, buffer_ind++ ) {
        uint8_t data = 0xFF;
        checksum ^= data;
        buffer[buffer_ind] = data;
    }

    /* Copy over the checksum */
    buffer[buffer_ind++] = checksum;

    assert( buffer_ind == data_size + 2 );

    return buffer_ind;
}

/******************************************************************************/
uint8_t DFUSE_createRMAddrToSend(
      uint8_t *buffer,
      uint8_t buffer_size,
      uint32_t addr
)
{
   /* This function does exactly the same thing as the WMAddrToSend
    * so just call it */
   return ( DFUSE_createWMAddrToSend( buffer, buffer_size, addr ) );
}

/******************************************************************************/
uint8_t DFUSE_createRMDataToRead(
      uint8_t *cmd_buffer,
      uint8_t cmd_buffer_size,
      uint8_t bytes_to_read
)
{
    /* Make sure the read request is reasonable */
    assert( bytes_to_read <= DFUSE_MAX_SIZE );
    /* Just the number of bytes to read and its XOR */
    assert( cmd_buffer_size == 2 );

    /* Clear the buffer */
    memset( cmd_buffer, 0, cmd_buffer_size );

    /* N goes in the first byte of the buffer to be sent */
    cmd_buffer[0] = bytes_to_read;
    cmd_buffer[1] = ~bytes_to_read;

    /* Always return 2 since this is how many bytes the command buffer is */
    return( 2 );
}
