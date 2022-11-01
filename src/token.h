#pragma once

#include "file.h"
#include <memory>
#include <string_view>

struct Token {
    enum Type : int {
        BeginParen = '(',
        EndParen = ')',
        BeginBrace = '{',
        EndBrace = '}',
        BeginBracket = '[',
        EndBracket = ']',
        Semicolon = ';',

        Eof = 0,

        Word = 256,
        Keyword,
        Operator,

        Module,
        Fn,
    };

    std::shared_ptr<File> file;
    std::string_view content;
    int row = 0;
    int col = 0;

    Type type = {};

    bool operator==(Type t) const {
        return t == type;
    }

    void expect(Type t) const;
};

Token::Type tokenType(std::string_view str);
