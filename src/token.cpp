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

#define KEYWORD(x)                                                             \
    { toLower(#x), Token::x }

std::unordered_map<std::string, Token::Type> keywords = {
    KEYWORD(Module),
    {";", Token::Semicolon},
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
