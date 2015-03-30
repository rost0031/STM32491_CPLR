/**
 * @file    Job.h
 * Class that contains the job that the client needs to do.
 *
 * @date    03/25/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef JOB_H_
#define JOB_H_

#ifdef __cplusplus
extern "C" {
#endif

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
/**
* @class Job
*
* @brief This class is a data storage class.
*
* This class is responsible for gathering data about what a client should do
* from different sources (cmd line parsing, menu driven interface, gui, etc)
* and provide a standardized way to pass it to the client internals.
*/
class CLIENT_DLL Job {

private:

public:


    /**
     * Default constructor
     */
    Job( void );

    /**
     * Default destructor.
     */
    ~Job( void );

};

#endif                                                              /* JOB_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
