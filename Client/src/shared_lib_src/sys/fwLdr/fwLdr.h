// $Id$
/**
 * @file 	fwLdr.h
 * @brief   Contains declaration of the class used for FW image handling
 * 			for sending it to the Redwood L1 Laminator board.
 *
 * @date   	1/11/2013
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$
#ifndef FWLDR_H_
#define FWLDR_H_

#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdint.h>
using namespace std;

#define MAX_FW_IMAGE_SIZE 1000000

/**
* @class FWLdr
*
* @brief This class handles manipulation of FW Images
*
* FWLdr class handles reading, manipulation, parsing, and checking of the FW
* images that will be sent over to be loaded on the Redwood L1.
*
* @author Harry Rostovtsev
*
* $Header $
*/
class FWLdr {
private:
	char* 	m_buf_FWImage;/**<buffer to use to temporarily store the FW image*/
	unsigned int	m_size_FWImage;/**<size of the fw image that was read in */
	unsigned int	m_max_buf_size;/**<max size that we'll allow FW images to be */
	string	m_filename_FWImage;/**<relative path and filename of the FW image*/
	unsigned int	m_chunk_index;/**<used to keep track of reading chunks of FW image*/
	uint32_t m_FWImage_CRC;/**<CRC of the entire FW image */

public:
	/** Default constructor. Just initializes some variables.
	 */
	FWLdr();

	/** Constructor. Assigns the filename to look up and reads in the
	 * FW image file.
	 */
	FWLdr(const string& filename);

	/** Default destructor. Clears all the memory allocated for the FW image */
	~FWLdr(); //Destructor

	/**
	 * A getter method that returns the size of the FW image.
	 *
	 * @param	None
	 * @return 	m_size_FWImage: Size of FW image.
	 */
	unsigned int getFWImageSize(void) {return (m_size_FWImage);}

	/**
	 * A getter method that returns the FW image chunk index.
	 *
	 * @param	None
	 * @return 	m_chunk_index: FW image chunk index.
	 */
	unsigned int getFWChunkIndex(void) {return (m_chunk_index);}

	/**
	 * Loads the FW Image from a file, the name of which was passed in to the
	 * constructor.
	 *
	 * @param	filename: a ref to a string containing the relative filename and
	 * path to the FW image file.
	 * @return 	bytes_in_file: how many bytes were read in from the file
	 * 			containing the FW image.
	 */
	unsigned int load_FWImage_from_file(const string& filename);

	/**
	 * Prepares the loaded FW image to be read chunk by chunk. Specifically,
	 * this method resets all the counters associated with keeping track of
	 * where the reading of the image left off at last chunk.  This method is
	 * automatically called at the end of load_FWImage_from_file() method.
	 *
	 * @param	None
	 * @return 	None
	 */
	void prepare_FWImage_for_read(void);

	/**
	 * Gets the next chunk from the loaded FW image.  Uses the user specified
	 * size to update the internal offset that keeps track of where to get the
	 * next chunk of data.
	 *
	 * @param[in,out] buffer: a pointer to the memory location where to store
	 * the retrieved chunk of FWImage is.
	 * @param[in] 	size: how big of a chunk is being requested in bytes.
	 * @return 	number of bytes actually read.
	 */
	unsigned int get_FWImage_chunk(char* buffer, unsigned int size);

	/**
	 * Allows a peek at how much of the FW image is left to process when chunking
	 * it out.  This function is useful to know if the packet you just read is
	 * the last one.  This situation only happens when the file size just happens
	 * to be divisible by the size that the fw upgrade process is using.
	 *
	 * @param	None.
	 * @return 	Number of bytes left in the fw image.  If this number is zero,
	 * then the packet is the last one that should be sent to the fw upgrade.
	 */
	unsigned int peek_FWImage_size_remainder(void);

	/**
	 * Returns the total size of the read in FW Image
	 *
	 * @param	None.
	 * @return 	Number of bytes total in the fw image.
	 */
	unsigned int get_FWImage_size(void);

	/**
	 * Calculates a CRC given a buffer and buffer size
	 *
	 * @param[in] buffer: a pointer to the memory location where the data from
	 * which to calculate the CRC is locate.
	 * @param[in] 	size: how many bytes in the buffer.
	 * @return 	CRC checksum.
	 */
	uint32_t get_CRC32(char *buffer, unsigned int size);

	/**
	 * Gets the CRC of the entire FW image that was read in.
	 *
	 * @param	None.
	 * @return 	CRC checksum of the entire FW Image file that was read in.
	 */
	uint32_t get_FWImage_CRC32(void);

	/**
	 * Prints the FW Image out to the screen in 16 columns, just as seen by the
	 * ST-Link Flash Tool.
	 *
	 * @param	None
	 * @return 	bytes_printed: how many bytes were printed out.
	 */
	unsigned int pretty_print_FWImage(void);

};




#endif /* FWLDR_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
