#pragma once

#include "file.h"
#include <memory>
#include <string_view>

struct Token {
    std::shared_ptr<File> file;
    std::string_view content;
    int row = 0;
    int col = 0;

    enum Type {
        Word,
        Eof,
    };

    Type type = Eof;
};
