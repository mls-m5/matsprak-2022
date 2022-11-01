#pragma once

#include "file.h"
#include "token.h"
#include <cctype>
#include <memory>
#include <stdexcept>
#include <string_view>

struct Tokenizer {
    Tokenizer(std::shared_ptr<File> file)
        : _file{file}
        , _content{_file->content()} {
        next();
        next();
    }

    const Token &token() const {
        return _token;
    }

    const Token &peek() const {
        return _peek;
    }

    void next() {
        _token = std::move(_peek);

        try {
            auto peek = Token{};

            for (char c; c = ch(), isGood() && std::isspace(c); stepChar()) {
            }

            peek.file = _file;
            peek.row = _row;
            peek.col = _col;

            auto start = _n;
            auto end = _n;
            for (char c; c = ch(), isGood() && !std::isspace(c); stepChar()) {
                ++end;
            }

            peek.content = _content.substr(start, end - start);
            peek.type = Token::Word;

            _peek = peek;
        }
        catch (std::out_of_range &e) {
            _peek = {};
            return;
        }
    }

    operator bool() {
        return _token.type != Token::Eof;
    }

private:
    void stepChar() {
        ++_n;
        if (!isGood()) {
            return;
        }
        auto c = ch();
        if (c == '\n') {
            ++_row;
            _col = 1;
        }
        else {
            ++_col;
        }
    }

    char ch() const {
        return _content.at(_n);
    }

    bool isGood() const {
        if (_n > _content.size()) {
            return false;
        }
        return true;
    }

    std::shared_ptr<File> _file;
    std::string_view _content;
    Token _token;
    Token _peek;

    int _n = 0;
    int _row = 0;
    int _col = 0;
};