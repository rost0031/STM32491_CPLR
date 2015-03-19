// $Id$
/**
 * @file   util.c
 * @brief  This file contains various utility functions for dealing with tags
 *
 * @date   08/04/2013
 * @author Harry Rostovtsev
 * @email  harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$
#include "util.h"

/******************************************************************************/
uint16_t UTIL_rev_uint16_bytes(uint16_t value)
{
   return ((uint16_t)((value & 0xFFU) << 8 | (value & 0xFF00U) >> 8));
}

/******************************************************************************/
uint32_t UTIL_rev_uint32_bytes(uint32_t value)
{
   return (
            (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
            (value & 0x00FF0000U) >> 8  | (value & 0xFF000000U) >> 24
          );
}

/******************************************************************************/
uint64_t UTIL_rev_uint64_bytes(uint64_t value)
{
  return(
           (value & 0x00000000000000FFUL) << 56 |
           (value & 0x000000000000FF00UL) << 40 |
           (value & 0x0000000000FF0000UL) << 24 |
           (value & 0x00000000FF000000UL) << 8  |
           (value & 0x000000FF00000000UL) >> 8  |
           (value & 0x0000FF0000000000UL) >> 24 |
           (value & 0x00FF000000000000UL) >> 40 |
           (value & 0xFF00000000000000UL) >> 56
        );
}

