#include "../include/CustomException.h"
#include <iostream>
CustomException::CustomException(const std::string &msg) : message(msg) {}

// what() function definition
const char* CustomException::what() const noexcept{
    return message.c_str();
}