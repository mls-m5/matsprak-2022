#pragma once

#include "token.h"
#include <stdexcept>
#include <string>

class ParsingError : std::runtime_error {
public:
    ParsingError(Token token, std::string str)
        : runtime_error{str} {}
};
