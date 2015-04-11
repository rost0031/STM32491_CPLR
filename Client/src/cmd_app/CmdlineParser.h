/**
 * @file    CmdlineParser.h
 * Class that contains the command line parser.
 *
 * @date    03/26/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CMDLINEPARSER_H_
#define CMDLINEPARSER_H_

/* Includes ------------------------------------------------------------------*/
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

#include "Logging.h"
/* Namespaces ----------------------------------------------------------------*/
using namespace std;
namespace po = boost::program_options;
using namespace po;

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/
/**
* @class CmdlineParser
*
* @brief This class contains the command line parser.
*
* This class is responsible for gathering and parsing cmd line arguments.
*/
class CmdlineParser {

private:
    int     m_argc;/**< Standard C style number of args argument */
    char**  m_argv;/**< Standard C style pointer to a pointer of array of args */

    bool m_bInteractiveRunMode; /**< User can specify if the client will run
                                   interactively (menu/gui, so the AO doesn't
                                   stop after finishing one command) or 1 cmd at
                                   a time and quit.  The default is interactive */

    std::vector<string> m_command;  /**< Vector that will hold the command along with
                                  optional (ordered) arguments in the rest of
                                  the indexes. */
    string  m_parsed_cmd;             /**< String containing the parsed command */
    std::map<string, string> parsed_args;     /**< Map containing parsed arguments */


    /* Storage for various args collected from cmdline */
    CBMsgRoute m_conn_mode;                   /**< Connection mode to the DC3 */
    string  m_ip_address;                       /**< IP address to connect to */
    string  m_remote_port;                         /**< IP port to connect to */
    int     m_local_port;                        /**< IP port to connect from */
    string  m_serial_dev;                    /**< serial device to connect to */
    int     m_serial_baud;                 /**< serial device baudrate to use */


public:


    /**
     * @brief  Constructor that sets up logging.
     *
     * @param [in] *logger: Logging pointer to an instance of Logging class
     * @return None.
     */
    CmdlineParser();

    /**
     * @brief  Parse passed in command line arguments
     *
     * Parses cmd line arguments.
     *
     * @param [in] argc: int number of arguments on cmd line
     * @param [in] **argv: char** pointer to the cmd line arguments
     * @retval:
     *   0: if no problems were detected.
     *   1: if failed to parse arguments.
     */
    int parse( int argc, char** argv );

    /**
     * @brief  Default destructor.
     */
    ~CmdlineParser( void );

    /**
     * @brief  Parse the cmd line arguments that were stored by the constructor.
     * @param  None
     * @return None
     */
    void parseArgs( void );



};

#endif                                                    /* CMDLINEPARSER_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
