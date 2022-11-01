#pragma once

#include "token.h"
#include <stdexcept>
#include <string>

class ParsingError : public std::runtime_error {
public:
    ParsingError(Token token, std::string str)
        : runtime_error{token.file->path().string() + ":" +
                        std::to_string(token.row) + ":" +
                        std::to_string(token.col) + ":\"" +
                        std::string{token.content} + "\" " + str} {}
};
