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

std::string toLower(std::string str) {
    for (auto &c : str) {
        c = std::tolower(c);
    }
    return str;
};

constexpr Token::Type firstOf(const char *ch) {
    return Token::Type{ch[0]};
}

#define KEYWORD(x)                                                             \
    { toLower(#x), Token::x }

// Stuff like ( )
#define SKEYWORD(x)                                                            \
    { #x, firstOf(#x) }

std::unordered_map<std::string, Token::Type> keywords = {
    KEYWORD(Module),
    KEYWORD(Fn),
    KEYWORD(Import),
    KEYWORD(Export),
    KEYWORD(Let),
    KEYWORD(Return),
    SKEYWORD(;),
    SKEYWORD([),
    SKEYWORD(]),
    SKEYWORD({),
    SKEYWORD(}),
    SKEYWORD(:),
    SKEYWORD(=),
    {"(", Token::BeginParen},
    {")", Token::EndParen},
    {",", Token::Comma},
    {"->", Token::RightArrow},
};

auto createNames() {
    auto names = std::unordered_map<Token::Type, std::string>{};

    for (auto &pair : keywords) {
        names[pair.second] = pair.first;
    }

    names[Token::Word] = "Word";
    names[Token::StringLiteral] = "StringLiteral";
    names[Token::NumericLiteral] = "NumericLiteral";

    return names;
}

std::unordered_map<Token::Type, std::string> keywordNames = createNames();

bool isOpChar(char c) {
    return std::find(operatorChars.begin(), operatorChars.end(), c) !=
           operatorChars.end();
}

Token::Type tokenType(std::string_view str) {
    if (str.empty()) {
        return Token::Eof;
    }
    if (str.front() == '"') {
        return Token::StringLiteral;
    }
    if (std::isdigit(str.front())) {
        return Token::NumericLiteral;
    }
    else if (std::find(
                 operatorChars.begin(), operatorChars.end(), str.front())) {
        return Token::Operator;
    }

    if (auto f = keywords.find(std::string{str}); f != keywords.end()) {
        return f->second;
    }

    return Token::Word;
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

std::string_view tokenName(Token::Type type) {
    if (auto f = keywordNames.find(type); f != keywordNames.end()) {
        return f->second;
    }

    return {};
}
