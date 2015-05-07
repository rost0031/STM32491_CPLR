/**
 * @file    fwLdr.cpp
 * Class and functions that handle the binary FW image
 *
 * @date    04/27/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "fwLdr.h"
#include <cstring>
#include <boost/crc.hpp>
#include "LogHelper.h"
#include "msg_utils.h"
#include "ClientErrorCodes.h"

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_FWL );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private class methods -----------------------------------------------------*/

/******************************************************************************/
ClientError_t FWLdr::loadFromFile( const char *filename )
{
   ClientError_t status = CLI_ERR_NONE;
   string fileName(filename);    /* Set the path and filename */

	ifstream fw_file(              /* open the file stream to read in the file */
	      filename,
	      ios::in | ios::binary | ios::ate
	);

   this->prepare();                             /* Clear out all current data */

	if (!fw_file) {
	   status = CLI_ERR_FW_UNABLE_TO_OPEN;
      ERR_printf(this->m_pLog,"Unable to open file %s", filename);
      return status;
	}

	/* if the file exists, read it */
	m_size = fw_file.tellg();                       /* Get the total file size */
	DBG_printf(this->m_pLog, "Size of %s is %d", filename, m_size);
	fw_file.seekg(0, ios::beg);                     /* Go to beginning of file */
	m_buffer = new uint8_t[m_size];                         /* Allocate buffer */
	fw_file.read((char *)m_buffer, m_size);           /* Read file into buffer */
	fw_file.close();                                             /* Close file */

	status = parseFilename(filename);
	if( CLI_ERR_NONE != status ) {
	   ERR_printf(this->m_pLog,
	         "Unable to parse filename %s for version and build datetime. Error: 0x%08x",
	         filename, status
	   );
      return status;
	}

	return status;
}

/******************************************************************************/
void FWLdr::prepare( void )
{
   m_size = 0;
   m_chunk_index = 0;
   m_CRCImage = 0;
	m_buildTime = "";
}

/******************************************************************************/
ClientError_t FWLdr::parseFilename( const char *filename )
{
   ClientError_t status = CLI_ERR_NONE;
   string filenameStr(filename);

   /* strip path from filename */
   size_t found = filenameStr.find_last_of("/\\");
   filenameStr = filenameStr.substr(found+1);

   /* Check for FW filename for all the expected data before attempting to parse
    * it */
   if( string::npos == filenameStr.find("DC3")) {
      status = CLI_ERR_FW_FILENAME_INVALID;
      ERR_printf(
            this->m_pLog,
            "FW filename Invalid: Missing 'DC3', error: 0x%08x",
            status
      );
      return status;
   }

   if( string::npos == filenameStr.find(".bin")) {
      status = CLI_ERR_FW_FILENAME_INVALID_EXT;
      ERR_printf(
            this->m_pLog,
            "FW filename extension invalid. Expecting '.bin', error: 0x%08x",
            status
      );
      return status;
   }

   if( string::npos == filenameStr.find("_v")) {
      status = CLI_ERR_FW_FILENAME_MISSING_VER;
      ERR_printf(
            this->m_pLog,
            "FW filename version missing: Missing '_vXY.WZ'), error: 0x%08x",
            status
      );
      return status;
   }

   if( string::npos == filenameStr.find("_v")) {
      status = CLI_ERR_FW_FILENAME_MISSING_DATETIME;
      ERR_printf(
            this->m_pLog,
            "FW filename build datetime missing: Missing '_20YYMMDDhhmmss'), error: 0x%08x",
            status
      );
      return status;
   }

   /* Example of filename passed in: DC3Boot_v00.01_20150428120611.bin */
   string::size_type lastPos = filenameStr.find_first_not_of("_", 0);
   // DC3Boot_v00.01_20150428120611.bin
   // |-lastpos

   /* Find first non-delimiter */
   string::size_type pos = filenameStr.find_first_of("_", lastPos);
   // DC3Boot_v00.01_20150428120611.bin
   //        |-pos

   string filenamePartDC3 = filenameStr.substr(lastPos, pos - lastPos);
   DBG_printf( this->m_pLog,"First part of filename is: %s", filenamePartDC3.c_str());

   lastPos = filenameStr.find_first_not_of("_", pos);       /* Skip delimiter */
   // DC3Boot_v00.01_20150428120611.bin
   //         |-lastpos


   pos = filenameStr.find_first_of("_", lastPos);  /* Find next non-delimiter */
   // DC3Boot_v00.01_20150428120611.bin
   //               |-pos

   string filenamePartVersion = filenameStr.substr(lastPos, pos - lastPos);
   DBG_printf( this->m_pLog,"Version part of filename is: %s", filenamePartVersion.c_str());

   /* Break apart version into major and minor */
   sscanf(
         filenamePartVersion.c_str(),
         "v%hu.%hu",
         &this->m_major, &this->m_minor
   );
   DBG_printf(this->m_pLog,"Major: %02d, Minor: %02d", this->m_major, this->m_minor);

   lastPos = filenameStr.find_first_not_of("_", pos);       /* Skip delimiter */
   // DC3Boot_v00.01_20150428120611.bin
   //                |-lastpos

   pos = filenameStr.find_first_of(".", lastPos);  /* Find next non-delimiter */
   string filenamePartDatetime = filenameStr.substr(lastPos, pos - lastPos);
   DBG_printf( this->m_pLog,"Datetime part of filename is: %s", filenamePartDatetime.c_str());
   this->m_buildTime = filenamePartDatetime;

   return status;
}

/******************************************************************************/
size_t FWLdr::calcNumberOfPackets( size_t size )
{
   uint16_t remainder = m_size % size;
   size_t nPackets = ( m_size / size ) + (remainder > 0 ? 1 : 0);
   return nPackets;
}

/******************************************************************************/
size_t FWLdr::getChunk( size_t size, uint8_t *buffer )
{
	if (m_chunk_index + size <= m_size) {
		memcpy( buffer, &m_buffer[m_chunk_index], size);
		m_chunk_index += size;
		return (size);
	} else if (	m_chunk_index < m_size &&
				m_size - m_chunk_index < size) {
		int left_over_size = m_size - m_chunk_index;
		memcpy( buffer, &m_buffer[m_chunk_index], left_over_size);
		m_chunk_index += left_over_size;

		/* A little sanity check */
		assert(m_chunk_index == m_size);

		return (left_over_size);
	} else {
		assert (m_chunk_index >! m_size);
		return (0);
	}
}

/******************************************************************************/
size_t FWLdr::getChunkAndCRC( size_t size, uint8_t *buffer, uint32_t *crc )
{
   size_t retSize = getChunk( size, buffer );
   *crc = calcCRC32( buffer, retSize );
   return retSize;
}

/******************************************************************************/
uint32_t FWLdr::calcCRC32(uint8_t *buffer, size_t size)
{
	boost::crc_32_type crc_result;
	crc_result.reset();
	crc_result.process_bytes(buffer, size);
//	cout << std::hex << "CRC1 = " << crc_result.checksum() << endl;

	return (crc_result.checksum());
}

/******************************************************************************/
uint32_t FWLdr::getImageCRC32( void )
{
	m_CRCImage = calcCRC32(m_buffer, m_size);
	return (m_CRCImage);
}

/******************************************************************************/
unsigned int FWLdr::toString( char *strBuffer, size_t strBufSize )
{
   unsigned int bytes_printed = 0;

   ClientError_t convertStatus = MSG_hexToStr(
         (uint8_t*)m_buffer,
         m_size,
         strBuffer,
         strBufSize,
         (uint16_t*)&bytes_printed,
         8,
         ' ',
         true
   );

   if ( CLI_ERR_NONE != convertStatus ) {
      return 0;
   }


//	if (m_size > 0) {
//		for (unsigned int i=0; i< m_size; i++, bytes_printed++) {
//
//			if (0 == i%16) {
//				printf("\n");
//			}
//			sprintf(strBuffer, "%02x ", (uint8_t)m_buffer[i]);
//		}
//		cout << endl;
//	}
	return(bytes_printed);
}

/******************************************************************************/
void FWLdr::setLogging( LogStub *log )
{
   this->m_pLog = log;
   DBG_printf(this->m_pLog,"Logging setup successful.");
}

/******************************************************************************/
FWLdr::FWLdr( LogStub *log, const char* filename ) :
                  m_pLog(NULL),
                  m_buffer(NULL),
                  m_size(0),
                  m_chunk_index(0),
                  m_CRCImage(0),
                  m_buildTime(""),
                  m_major(0),
                  m_minor(0)
{
   this->setLogging(log);

   m_size = this->loadFromFile(filename);
}

/******************************************************************************/
FWLdr::FWLdr( LogStub *log ) :
                  m_pLog(NULL),
                  m_buffer(NULL),
                  m_size(0),
                  m_chunk_index(0),
                  m_CRCImage(0),
                  m_buildTime(""),
                  m_major(0),
                  m_minor(0)
{
   this->setLogging(log);
}

/******************************************************************************/
FWLdr::~FWLdr()
{
   delete[] m_buffer;
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
