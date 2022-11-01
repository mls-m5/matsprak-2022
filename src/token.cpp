#include "token.h"
#include "errors.h"
#include <cctype>
#include <string>
#include <unordered_map>

namespace {

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
    SKEYWORD(;),
    SKEYWORD([),
    SKEYWORD(]),
    SKEYWORD({),
    SKEYWORD(}),
    {"(", Token::BeginParen},
    {")", Token::EndParen},
};

} // namespace

void Token::expect(Type t) const {
    if (t != type) {
        throw ParsingError{*this,
                           "Expected " + std::to_string(t) + " got " +
                               std::to_string(type)};
    }
}

Token::Type tokenType(std::string_view str) {
    if (auto f = keywords.find(std::string{str}); f != keywords.end()) {
        return f->second;
    }

    return Token::Word;
}
