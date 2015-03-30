// $Id$
/**
 * @file 	fwLdr.cpp
 * @brief   Contains implementation of the class used for FW image handling
 * 			for sending it to the Redwood L1 Laminator board.
 *
 * @date   	1/11/2013
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$


#include "fwLdr.h"
#include <cstring>
#include <boost/crc.hpp>

using namespace std;

FWLdr::FWLdr(void) {
	m_max_buf_size = MAX_FW_IMAGE_SIZE;
	m_filename_FWImage = "";
	m_buf_FWImage = new char[m_max_buf_size];
	m_size_FWImage = 0;
	m_chunk_index = 0;
	m_FWImage_CRC = 0;
}

FWLdr::FWLdr(const string& filename) {
	m_max_buf_size = MAX_FW_IMAGE_SIZE;
	m_buf_FWImage = new char[m_max_buf_size];
	m_filename_FWImage = filename;
	m_size_FWImage = load_FWImage_from_file(m_filename_FWImage);
	m_chunk_index = 0;
}

FWLdr::~FWLdr() {
	delete m_buf_FWImage;
}

unsigned int FWLdr::load_FWImage_from_file(const string& filename) {

	/* Set the internal filename so the class is aware of it from now on */
	m_filename_FWImage = filename;

	/* open the file stream to read in the file */
	ifstream fw_file (m_filename_FWImage.c_str(), ios::in | ios::binary | ios::ate);

	/* if the file exists, read it */
	if (fw_file) {

		/* Get the total file size */
		m_size_FWImage = fw_file.tellg();
//		cout << "Successfully read " << m_size_FWImage << " bytes from " << m_filename_FWImage << endl;

		/* Go to beginning of file */
		fw_file.seekg(0, ios::beg);
		/* Read the file into buffer */
		fw_file.read(m_buf_FWImage, m_size_FWImage);
		/* Close file */
		fw_file.close();
	} else {
		cout << "Unable to open file " << m_filename_FWImage << endl;
	}

	/* Since we just read in a new image, go ahead and do a reset on the read
	 * indexes */
	prepare_FWImage_for_read();

	/* Return number of bytes that were read in */
	return(m_size_FWImage);
}

void FWLdr::prepare_FWImage_for_read() {
	m_chunk_index = 0;
	m_FWImage_CRC = 0;
}

unsigned int FWLdr::get_FWImage_chunk(char* buffer, unsigned int size) {
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

unsigned int FWLdr::peek_FWImage_size_remainder(void) {
	return (m_size_FWImage - m_chunk_index);
}

unsigned int FWLdr::get_FWImage_size() {
	return (m_size_FWImage);
}

uint32_t FWLdr::get_CRC32(char *buffer, unsigned int size) {
	boost::crc_32_type crc_result;
	crc_result.reset();
	crc_result.process_bytes(buffer, size);
//	cout << std::hex << "CRC1 = " << crc_result.checksum() << endl;

	return (crc_result.checksum());
}

uint32_t FWLdr::get_FWImage_CRC32() {
	m_FWImage_CRC = get_CRC32(m_buf_FWImage, m_size_FWImage);
	return (m_FWImage_CRC);
}

unsigned int FWLdr::pretty_print_FWImage() {
	unsigned int bytes_printed = 0;
	if (m_size_FWImage > 0) {
		for (unsigned int i=0; i< m_size_FWImage; i++, bytes_printed++) {

			if (0 == i%16) {
				printf("\n");
			}
			printf("%2x ", (uint8_t)m_buf_FWImage[i]);
		}
		cout << endl;
	}
	return(bytes_printed);
}

/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
