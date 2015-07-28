/**
 * @file 	ApiShared.h
 * Contains all the Shared Events, Priorities, and any other needed declarations
 * for the DC3 Client Api. This is necessary to make sure there is no overlap in
 * enumerations between the shared code and the code pulling the shared code in.
 *
 * @date   	03/23/2015
 * @author 	Harry Rostovtsev
 * @email  	harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APISHARED_H_
#define APISHARED_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdlib>
#include <cstdio>
#include <cstring>                            /* for memcpy() and memset() */
#include <iostream>
#include <fcntl.h>
#include <fstream>

/* This is necessary since C++ code is pulling in a library compiled as C */
extern "C" {
#include "DC3CommApi.h"
}

#include "ApiErrorCodes.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif                                                        /* APISHARED_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/
