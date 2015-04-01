/**
 * @file    Logging.h
 * This class is responsible for logging and output.
 *
 * This class contains functions and methods which should be passed in as
 * callbacks to all the other classes in the dll and the program so that there's
 * consistent logging/debugging system throughout the entire program.
 *
 * @date    03/31/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGGING_H_
#define LOGGING_H_

/* Includes ------------------------------------------------------------------*/
#include "CBSharedDbgLevels.h"
#include "LogStub.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

#define LOG_printf(logObj, fmt, ...) \
      do { logObj->output(LOG, __func__, __LINE__, fmt, \
            ##__VA_ARGS__); \
      } while (0)

/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void CON_output(
      DBG_LEVEL_T dbgLvl,
      const char *pFuncName,
      int wLineNumber,
      char *fmt,
      ...
);
/* Exported classes ----------------------------------------------------------*/
/**
 * @class Logging
 *
 * @brief This class is a wrapper for the internal library log stubs.
 *
 * This class allows the caller of the Client library to implement any logging
 * by using local callbacks.  It also provides a direct interface to the various
 * control over internal logging the library uses.
 */
class Logging {

private:

public:

   LogStub *m_pLog;

   ClientError_t setMsgCallBack(
         CB_MsgHandler_t pCallbackFunction
   );

   ClientError_t setLibLogCallBack(
         CB_LibLogHandler_t pCallbackFunction
   );

   void output(
         DBG_LEVEL_T dbgLvl,
         const char *pFuncName,
         int wLineNumber,
         char *fmt,
         ...
   );

   /**
    * @brief   Default constructor.
    * This constructor doesn't set up any logging.  There will be complete
    * silence from the Client library unless "" is called to set up the callback
    * functions for logging.
    *
    * @param   None.
    * @return  None.
    */
   Logging( void );

   /**
    * @brief   Default destructor
    */
   ~Logging( void );

};

#endif                                                          /* LOGGING_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
