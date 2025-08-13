#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

class cursor_error : public std::logic_error {
public:
    cursor_error() : std::logic_error("Cursor error!") {}
    explicit cursor_error(const std::string& what_arg) : std::logic_error(what_arg) {}
};

using namespace std;


#endif // __MAIN_H__
