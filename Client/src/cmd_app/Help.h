/**
 * @file    Help.h
 * Contains all the extended help for all commands so they don't take up space
 * in main.cpp.
 *
 * @date    04/24/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HELP_H_
#define HELP_H_

/* Includes ------------------------------------------------------------------*/
#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <boost/shared_ptr.hpp>

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * This pretty prints command specific help and exits.
 *
 * @param [in] parsed_cmd: string containing the parsed command for which to
 * print help for.
 * @param [in] appName: string that contains the application name.
 * @return: None
 */
void HELP_printCmdSpecific(
      const std::string& parsed_cmd,
      const std::string& appName
);

/**
 * This function breaks apart a string given a set of delimiters
 *
 * @param [in] str: string& containing the text to break apart
 * @param [out] tokens: vector that will contain the tokens
 * @param [in] delims: a string of one or more delimiters.  Defaults to
 * whitespace.
 * @return: None
 */
void CMD_tokenize(
      const std::string& str,
      std::vector<string>& tokens,
      const std::string& delims
);

/* Exported classes ----------------------------------------------------------*/

/**
* @class ff_ostream_iterator
*
* @brief This class implements an interator that formats lines into a more
* readable form by inserting newlines at a user specified intervals in the text.
*
* Copied from:?
*     http://stackoverflow.com/questions/5280858/indenting-paragraph-with-cout
*/
class ff_ostream_iterator
    : public std::iterator<std::output_iterator_tag, char, void, void, void>
{
public:

    ff_ostream_iterator(): os_(),
    line_prefix_(0),
    max_line_length_(0),
    current_line_length_(),
    active_instance_(new ff_ostream_iterator*(this)) { }

    ff_ostream_iterator(std::ostream& os,
                        std::string line_prefix,
                        unsigned max_line_length)
        : os_(&os),
          line_prefix_(line_prefix),
          max_line_length_(max_line_length),
          current_line_length_(),
          active_instance_(new ff_ostream_iterator*(this))
    {
        *os_ << line_prefix;
    }

    ~ff_ostream_iterator() {
        if (*active_instance_ == this)
            insert_word();
    }

    ff_ostream_iterator& operator=(char c) {
        *active_instance_ = this;
        if (std::isspace(c)) {
            if (word_buffer_.size() > 0) {
                insert_word();
            }
        }
        else {
            word_buffer_.push_back(c);
        }
        return *this;
    }

    ff_ostream_iterator& operator*()     { return *this; }
    ff_ostream_iterator& operator++()    { return *this; }
    ff_ostream_iterator  operator++(int) { return *this; }

private:

    void insert_word() {
        if (word_buffer_.size() == 0)
            return;

        if (word_buffer_.size() + current_line_length_ <= max_line_length_) {
            write_word(word_buffer_);
        }
        else {
            *os_ << '\n' << line_prefix_;

            if (word_buffer_.size() <= max_line_length_) {
                current_line_length_ = 0;
                write_word(word_buffer_);
            }
            else {
                for (unsigned i(0);i<word_buffer_.size();i+=max_line_length_)
                {
                    current_line_length_ = 0;
                    write_word(word_buffer_.substr(i, max_line_length_));
                    if (current_line_length_ == max_line_length_) {
                        *os_ << '\n' << line_prefix_;
                    }
                }
            }
        }

        word_buffer_ = "";
    }

    void write_word(const std::string& word) {
        *os_ << word;
        current_line_length_ += word.size();
        if (current_line_length_ != max_line_length_) {
            *os_ << ' ';
            ++current_line_length_;
        }
    }

    std::ostream* os_;
    std::string word_buffer_;

    std::string line_prefix_;
    unsigned max_line_length_;
    unsigned current_line_length_;

    boost::shared_ptr<ff_ostream_iterator*> active_instance_;
};


#endif                                                             /* HELP_H_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
