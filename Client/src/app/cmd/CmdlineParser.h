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
    int     argc;/**< Standard C style number of args argument */
    char**  argv;/**< Standard C style pointer to a pointer of array of args */

    bool bInteractiveRunMode; /**< User can specify if the client will run
                                   interactively (menu/gui, so the AO doesn't
                                   stop after finishing one command) or 1 cmd at
                                   a time and quit.  The default is interactive */

    vector<string> command;  /**< Vector that will hold the command along with
                                  optional (ordered) arguments in the rest of
                                  the indexes. */
    string  parsed_cmd;             /**< String containing the parsed command */
    map<string, string> parsed_args;     /**< Map containing parsed arguments */
public:


    /**
     * Default constructor
     */
    CmdlineParser( int argc, char** argv );

    /**
     * Default destructor.
     */
    ~CmdlineParser( void );

};

#endif                                                    /* CMDLINEPARSER_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
