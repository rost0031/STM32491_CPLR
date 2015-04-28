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
size_t FWLdr::loadFromFile( const char *filename )
{
   string fileName(filename);    /* Set the path and filename */

	ifstream fw_file(              /* open the file stream to read in the file */
	      filename,
	      ios::in | ios::binary | ios::ate
	);

	if (fw_file) {                              /* if the file exists, read it */
		m_size_FWImage = fw_file.tellg();            /* Get the total file size */
//		cout << "Successfully read " << m_size_FWImage << " bytes from " << m_filename_FWImage << endl;
		fw_file.seekg(0, ios::beg);                  /* Go to beginning of file */
		this->prepare();                          /* Clear out all current data */
		m_buf_FWImage = new char[m_size_FWImage];            /* Allocate buffer */
		fw_file.read(m_buf_FWImage, m_size_FWImage);   /* Read file into buffer */
		parseFilename(filename);
		fw_file.close();                                          /* Close file */
	} else {
		cout << "Unable to open file " << filename << endl;
	}
	/* Return number of bytes that were read in */
	return(m_size_FWImage);
}

/******************************************************************************/
void FWLdr::prepare( void )
{
   m_size_FWImage = 0;
   m_chunk_index = 0;
   m_FWImage_CRC = 0;
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

   if( string::npos == filenameStr.find("DC3")) {
      status = CLI_ERR_INVALID_FW_FILENAME;
      ERR_printf(
            this->m_pLog,
            "Invalid FW filename (missing 'DC3'), error: 0x%08x",
            status
      );
      return status;
   }

   if( string::npos == filenameStr.find(".bin")) {
      status = CLI_ERR_INVALID_FW_FILENAME_EXT;
      ERR_printf(
            this->m_pLog,
            "Invalid FW filename extension (expecting '.bin'), error: 0x%08x",
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

   lastPos = filenameStr.find_first_not_of("_", pos);       /* Skip delimiter */
   // DC3Boot_v00.01_20150428120611.bin
   //                |-lastpos

   pos = filenameStr.find_first_of(".", lastPos);  /* Find next non-delimiter */
   string filenamePartDatetime = filenameStr.substr(lastPos, pos - lastPos);
   DBG_printf( this->m_pLog,"Datetime part of filename is: %s", filenamePartDatetime.c_str());
}

/******************************************************************************/
unsigned int FWLdr::getChunk( char* buffer, unsigned int size )
{
	if (m_chunk_index + size <= m_size_FWImage) {
		memcpy( buffer, &m_buf_FWImage[m_chunk_index], size);
		m_chunk_index += size;
		return (size);
	} else if (	m_chunk_index < m_size_FWImage &&
				m_size_FWImage - m_chunk_index < size) {
		int left_over_size = m_size_FWImage - m_chunk_index;
		memcpy( buffer, &m_buf_FWImage[m_chunk_index], left_over_size);
		m_chunk_index += left_over_size;

		/* A little sanity check */
		assert(m_chunk_index == m_size_FWImage);

		return (left_over_size);
	} else {
		assert (m_chunk_index >! m_size_FWImage);
		return (0);
	}
}

/******************************************************************************/
uint32_t FWLdr::calcCRC32(char *buffer, unsigned int size)
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
	m_FWImage_CRC = calcCRC32(m_buf_FWImage, m_size_FWImage);
	return (m_FWImage_CRC);
}

/******************************************************************************/
unsigned int FWLdr::toString( char *strBuffer, size_t strBufSize )
{
   unsigned int bytes_printed = 0;

   ClientError_t convertStatus = MSG_hexToStr(
         (uint8_t*)m_buf_FWImage,
         m_size_FWImage,
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


//	if (m_size_FWImage > 0) {
//		for (unsigned int i=0; i< m_size_FWImage; i++, bytes_printed++) {
//
//			if (0 == i%16) {
//				printf("\n");
//			}
//			sprintf(strBuffer, "%02x ", (uint8_t)m_buf_FWImage[i]);
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
FWLdr::FWLdr( LogStub *log, const char* filename )
{
   this->prepare();
   this->setLogging(log);

   m_size_FWImage = this->loadFromFile(filename);
}

/******************************************************************************/
FWLdr::FWLdr( LogStub *log )
{
   this->m_pLog = log;
//   this->prepare();
//   this->setLogging(log);
}

/******************************************************************************/
FWLdr::~FWLdr()
{
   delete m_buf_FWImage;
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
