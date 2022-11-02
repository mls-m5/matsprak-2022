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
        reset();
    }

    // Make ready for another pass
    void reset() {
        _row = 1;
        _col = 0;
        _n = 0;
        next();
        next();
    }

    const Token &token() const {
        return _token;
    }

    const Token &peek() const {
        return _peek;
    }

    const Token &next();

    operator bool() {
        return _token.type() != Token::Eof;
    }

private:
    void skipSpace() {
        for (bool restart = true; restart;) {
            restart = false;
            for (char c; c = ch(), isGood() && std::isspace(c); stepChar()) {
            }

            if (ch() == '/' && peekCh() == '/') {
                skipLine();
                restart = true;
            }
        }
    }

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

    char peekCh() const {
        if (_n < _content.size()) {
            return _content[_n];
        }
        return '\0';
    }

    void skipLine() {
        for (; ch() != '\n'; stepChar()) {
        }
    }

    bool isGood() const {
        if (_n > _content.size()) {
            return false;
        }
        return true;
    }

    std::shared_ptr<File> _file;
    std::string_view _content;
    Token _token{nullptr};
    Token _peek{nullptr};

    int _n = 0;
    int _row = 1;
    int _col = 0;
};
