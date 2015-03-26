/**
 * @file    CplusplusTemplate.cpp
 * Brief class description
 *
 * A more detailed class description and how it's used.  Also, why?
 *
 * @date    <mm>/<dd>/<year>
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) <YEAR> Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CPLUSPLUSTEMPLATE_H_
#define CPLUSPLUSTEMPLATE_H_


/* If need to include any C objects, includes, etc*/
#ifdef __cplusplus
extern "C" {
#endif

/* If need to make this section available externally in a DLL */
#ifdef _WIN32
   #ifdef BUILDING_CLIENT_DLL
   #define CLIENT_DLL __declspec(dllexport)
   #else
   #define CLIENT_DLL __declspec(dllimport)
   #endif
#else
   #define CLIENT_DLL
   #define __stdcall
#endif


#ifdef __cplusplus
}
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/


#endif                                                /* CPLUSPLUSTEMPLATE_H_ */
/******** Copyright (C) <YEAR> Datacard. All rights reserved *****END OF FILE****/
