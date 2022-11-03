#pragma once

#include "file.h"
#include <cstddef>
#include <memory>
#include <ostream>
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
        Comma = ',',
        Colon = ':',
        Equal = '=',

        Eof = 0,

        Word = 256,
        Any, // For matching
        Keyword,
        Operator,
        StringLiteral,
        RightArrow,
        NumericLiteral,

        Module,
        Fn,
        Import,
        Export,
        Let,
        Return,
    };

    Token(std::shared_ptr<File> file,
          std::string_view content,
          Type type,
          int row,
          int col)
        : _file{std::move(file)}
        , _content{content}
        , _type{type}
        , _row{row}
        , _col{col} {}

    Token(std::nullptr_t) {}
    Token() = default;

    static Token fromString(std::string str, Token::Type type = Type::Word) {
        auto file = std::make_shared<File>("inline", str);
        auto token = Token(file, file->content(), Type::Word, 1, 1);
        return token;
    }

    bool operator==(const Type &t) const {
        return t == _type;
    }

    bool operator==(const Token &other) const {
        return other.content() == _content;
    }

    void expect(Type t) const;

    const std::shared_ptr<File> &file() const {
        return _file;
    }

    std::string_view content() const {
        return _content;
    }

    std::string str() const {
        return std::string{_content};
    }

    int row() const {
        return _row;
    }

    int col() const {
        return _col;
    }

    Type type() const {
        return _type;
    }

    friend std::ostream &operator<<(std::ostream &stream, const Token &token) {
        stream << token.content();
        return stream;
    }

    Token substr(size_t i, size_t n) {
        auto t = *this;
        t._content = _content.substr(i, n);
        return t;
    }

private:
    std::shared_ptr<File> _file;
    std::string_view _content;
    int _row = 0;
    int _col = 0;

    Type _type = {};
};
