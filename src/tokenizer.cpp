#include "tokenizer.h"
#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace {

auto operatorChars = std::string_view{"+-*/-<>^(){}[];:,.\""};

auto multiCharOps = std::unordered_set<std::string_view>{
    "->",
};

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
        else if (isOpChar(ch())) {
            for (char c; c = ch(), isOpChar(c); stepChar()) {
            }

            auto len = _n - start;

            if (len > 1) {
                bool isFound = false;
                for (int i = len; i >= 2; --i) {
                    auto content = _content.substr(start, i);
                    if (auto f = multiCharOps.find(content);
                        f != multiCharOps.end()) {
                        _n = start + len;
                        isFound = true;
                        break;
                    }
                }

                if (!isFound) {
                    _n = start + 1;
                }
            }

            end = _n;
        }
        else {
            for (char c; c = ch(), !std::isspace(c); stepChar()) {
                end = _n + 1;
                if (ch() == '/' && peekCh() == '/') {
                    skipSpace();
                    break;
                }
                else if (isOpChar(c)) {
                    end = _n;
                    break;
                }
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
