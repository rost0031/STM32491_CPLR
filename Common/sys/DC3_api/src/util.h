// $Id$
/**
 * @file   util.h
 * @brief  This file contains various utility functions for dealing with tags
 *
 * @date   08/04/2013
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

/**
 * Reverse byte order of a uint16_t type.
 *
 * @param  : uint16_t value
 * @return : byte-reversed uint16_t value.
 */
uint16_t UTIL_rev_uint16_bytes(uint16_t value);

/**
 * Reverse byte order of a uint32_t type.
 *
 * @param  : uint32_t value
 * @return : byte-reversed uint32_t value.
 */
uint32_t UTIL_rev_uint32_bytes(uint32_t value);

/**
 * Reverse byte order of a uint64_t type.
 *
 * @param  : uint64_t value
 * @return : byte-reversed uint64_t value.
 */
uint64_t UTIL_rev_uint64_bytes(uint64_t value);

#endif                                                            /* _UTIL_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
