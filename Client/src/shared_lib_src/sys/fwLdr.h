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
	char* 	m_buf_FWImage;             /**<buffer to use to store the FW image*/
	size_t   m_size_FWImage;         /**<size of the fw image that was read in */
	unsigned int m_chunk_index;            /**<keep track of chunks of FW image*/
	uint32_t m_FWImage_CRC;                     /**<CRC of the entire FW image */
	string   m_buildTime;                           /**< parsed build datetime */

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
    * @return  None
    */
   ClientError_t parseFilename( const char *filename );

public:

	/**
    * A getter method that returns the size of the FW image.
    *
    * @param   None
    * @return  m_size_FWImage: Size of FW image.
    */
   unsigned int getSize(void) {return (m_size_FWImage);}

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
   unsigned int remains(void) {return (m_size_FWImage - m_chunk_index);}

   /**
    * Returns the total size of the read in FW Image
    *
    * @param   None.
    * @return  Number of bytes total in the fw image.
    */
   unsigned int getSize(size_t) {return (m_size_FWImage);}

   /**
    * Loads the FW Image from a file, the name of which was passed in to the
    * constructor.
    *
    * @param   filename: const char pointer to buffer containing the relative
    * path and filename to the FW image file.
    * @return  bytes_in_file: how many bytes were read in from the file
    *          containing the FW image.
    */
   size_t loadFromFile( const char *filename );

   /**
    * Gets the next chunk from the loaded FW image.  Uses the user specified
    * size to update the internal offset that keeps track of where to get the
    * next chunk of data.
    *
    * @param[in,out] buffer: a pointer to the memory location where to store
    * the retrieved chunk of FWImage is.
    * @param[in]  size: how big of a chunk is being requested in bytes.
    * @return  number of bytes actually read.
    */
   unsigned int getChunk(char* buffer, unsigned int size);

	/**
	 * Calculates a CRC given a buffer and buffer size
	 *
	 * @param[in] buffer: a pointer to the memory location where the data from
	 * which to calculate the CRC is locate.
	 * @param[in] 	size: how many bytes in the buffer.
	 * @return 	CRC checksum.
	 */
	uint32_t calcCRC32(char *buffer, unsigned int size);

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
    * @brief Default constructor. Initializes some variables.
    * @param [in] *log: LogStub pointer to the class that has the proper
    *                   callbacks set up for logging.
    * @param [in] *filename: const char pointer to the path and filename to open.
    */
   FWLdr( LogStub *log, const char* filename );

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
