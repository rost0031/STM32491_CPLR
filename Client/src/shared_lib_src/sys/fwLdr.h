/**
 * @file    fwLdr.h
 * Class and functions that handle the binary FW image
 *
 * @date    04/27/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FWLDR_H_
#define FWLDR_H_

/* Includes ------------------------------------------------------------------*/
#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdint.h>

#include "LogStub.h"
/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Exported defines ----------------------------------------------------------*/
#define MAX_FW_IMAGE_SIZE 1500000      /**<! Max size (in bytes) of FW images */

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/
/**
* @class FWLdr
*
* @brief This class handles manipulation of FW Images
*
* FWLdr class handles reading, manipulation, parsing, and checking of the FW
* images.
*/
class FWLdr {
private:
   LogStub *m_pLog;         /**< Pointer to LogStub instance used for logging */
	uint8_t *m_buffer;                  /**<buffer to use to store the FW image*/
	size_t   m_size;                 /**<size of the fw image that was read in */
	uint32_t m_chunk_index;                /**<keep track of chunks of FW image*/
	uint32_t m_CRCImage;                        /**<CRC of the entire FW image */
	string   m_buildTime;                           /**< parsed build datetime */
	uint16_t m_major;                              /**< Major version of image */
	uint16_t m_minor;                              /**< Minor version of image */

	/**
	 * Prepares the loaded FW image to be read chunk by chunk. Specifically,
	 * this method resets all the counters associated with keeping track of
	 * where the reading of the image left off at last chunk.  This method is
	 * automatically called at the end of loadFromFile() method.
	 *
	 * @param   None
	 * @return  None
	 */
	void prepare( void );

	/**
    * @brief   Parse version and build date/time from the filename.
    * @param   filename: const char pointer to buffer containing the relative
    * path and filename to the FW image file.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t parseFilename( const char *filename );

public:

	/**
    * A getter method that returns the size of the FW image.
    *
    * @param   None
    * @return  m_size_FWImage: Size of FW image.
    */
   size_t getSize(void) {return (m_size);}

   /**
    * A getter method that returns the FW image chunk index.
    *
    * @param   None
    * @return  m_chunk_index: FW image chunk index.
    */
   unsigned int getFWChunkIndex(void) {return (m_chunk_index);}

   /**
    * Allows a peek at how much of the FW image is left to process when chunking
    * it out.  This function is useful to know if the packet you just read is
    * the last one.  This situation only happens when the file size just happens
    * to be divisible by the size that the fw upgrade process is using.
    *
    * @param   None.
    * @return  Number of bytes left in the fw image.  If this number is zero,
    * then the packet is the last one that should be sent to the fw upgrade.
    */
   unsigned int remains(void) {return (m_size - m_chunk_index);}

   /**
    * @brief Returns the major version parsed from the filename of the FW image.
    *
    * @param   None.
    * @return  uint16_t:  Major version.
    */
   uint16_t getMajVer(void) {return (this->m_major);}

   /**
    * @brief Returns the minor version parsed from the filename of the FW image.
    *
    * @param   None.
    * @return  uint16_t:  Minor version.
    */
   uint16_t getMinVer(void) {return (this->m_minor);}

   /**
    * Returns the build datetime parsed from the filename of the FW image.
    *
    * @param   None.
    * @return  const char*:  Const char* pointer to the parsed build datetime.
    */
   const char* getDatetime(void) {return (this->m_buildTime.c_str());}

   /**
    * Returns the build datetime length.
    *
    * @param   None.
    * @return  size_t:  size of the datetime.
    */
   size_t getDatetimeLen(void) {return (this->m_buildTime.length());}

   /**
    * Loads the FW Image from a file, the name of which was passed in to the
    * constructor.
    *
    * @param   filename: const char pointer to buffer containing the relative
    * path and filename to the FW image file.
    * @return: APIError_t status of the client executing the command.
    *    @arg  API_ERR_NONE: success
    *    other error codes if failure.
    */
   APIError_t loadFromFile( const char *filename );

   /**
    * @brief   Calculates how many packets there will be in the FW image given
    * chunk size.
    *
    * @param [in] size: size_t indicating how big of packets to calc for.
    * @return  size_t: How many packets there will be.
    */
   size_t calcNumberOfPackets( size_t size );

   /**
    * Gets the next chunk from the loaded FW image.  Uses the user specified
    * size to update the internal offset that keeps track of where to get the
    * next chunk of data.
    * @param[in]  size: how big of a chunk is being requested in bytes.
    * @param[in,out] buffer: a pointer to the memory location where to store
    * the retrieved chunk of FWImage is.
    * @return  number of bytes actually read.
    */
   size_t getChunk(size_t size, uint8_t *buffer );

   /**
    * @brief Gets the next chunk from the loaded FW image and its CRC.
    * Uses the user specified size to update the internal offset that keeps
    * track of where to get the next chunk of data.
    *
    * @param[in,out] buffer: a pointer to the memory location where to store
    * the retrieved chunk of FWImage is.
    * @param[in]  size: size_t that specifies how big of a chunk is being
    * requested in bytes.
    * @return  number of bytes actually read.
    */
   size_t getChunkAndCRC( size_t size, uint8_t *buffer, uint32_t *crc );

	/**
	 * Calculates a CRC given a buffer and buffer size
	 *
	 * @param[in] buffer: a pointer to the memory location where the data from
	 * which to calculate the CRC is locate.
	 * @param[in] 	size: how many bytes in the buffer.
	 * @return 	CRC checksum.
	 */
	uint32_t calcCRC32(uint8_t *buffer, size_t size);

	/**
	 * Gets the CRC of the entire FW image that was read in.
	 *
	 * @param	None.
	 * @return 	CRC checksum of the entire FW Image file that was read in.
	 */
	uint32_t getImageCRC32(void);

	/**
	 * Prints the FW Image out to the screen in 16 columns, just as seen by the
	 * ST-Link Flash Tool.
	 *
	 * @param [in,out] *strBuffer: Buffer where to write the string version of
	 * the FW image.
	 * @param [in] strBufSize: size of the strBuffer.
	 * @return 	bytes_printed: how many bytes were printed out.
	 */
	unsigned int toString( char *strBuffer, size_t strBufSize  );

   /**
    * @brief   Sets a new LogStub pointer.
    * @param [in]  *log: LogStub pointer to a LogStub instance.
    * @return: None.
    */
   void setLogging( LogStub *log );

	/**
    * @brief Default constructor. Sets logging and reads file
    * @param [in] *log: LogStub pointer to the class that has the proper
    *                   callbacks set up for logging.
    * @param [in] *filename: const char pointer to the path and filename to open.
    */
   FWLdr( LogStub *log, const char* filename );

   /**
    * @brief Default constructor.  Sets logging.
    * @param [in] *log: LogStub pointer to the class that has the proper
    *                   callbacks set up for logging.
    */
   FWLdr( LogStub *log );

   /**
    * @brief Default destructor.
    * Clears all the memory allocated for the FW image
    * @param: None
    */
   ~FWLdr( void );
};


#endif                                                            /* FWLDR_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
