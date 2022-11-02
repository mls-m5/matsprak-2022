#include "tokenizer.h"
#include <algorithm>
#include <string_view>

namespace {

auto operatorChars = std::string_view{"+-*/-<>^"};

bool isOpChar(char c) {
    return std::find(operatorChars.begin(), operatorChars.end(), c) !=
           operatorChars.end();
}

} // namespace

const Token &Tokenizer::next() {
    _token = std::move(_peek);

    try {
        skipSpace();

        auto row = _row;
        auto col = _col;

        auto start = _n;
        auto end = _n;

        if (ch() == '"') {
            stepChar();

            for (; ch() != '"'; stepChar()) {
            }

            stepChar();
            end = _n;
        }
        if (isOpChar(ch())) {
            for (char c; c = ch(), isOpChar(c); stepChar()) {
            }

            end = _n;
        }
        else {
            for (char c; c = ch(), !std::isspace(c); stepChar()) {
                if (ch() == '/' && peekCh() == '/') {
                    skipSpace();
                    break;
                }
                ++end;
            }
        }

        auto content = _content.substr(start, end - start);

        _peek = Token{
            _file,
            content,
            tokenType(content),
            row,
            col,
        };
    }
    catch (std::out_of_range &e) {
        _peek = {nullptr};
    }

    return _token;
}
