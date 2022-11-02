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
    KEYWORD(Import),
    KEYWORD(Export),
    SKEYWORD(;),
    SKEYWORD([),
    SKEYWORD(]),
    SKEYWORD({),
    SKEYWORD(}),
    {"(", Token::BeginParen},
    {")", Token::EndParen},
};

auto createNames() {
    auto names = std::unordered_map<Token::Type, std::string>{};

    for (auto &pair : keywords) {
        names[pair.second] = pair.first;
    }

    names[Token::Word] = "Word";

    return names;
}

std::unordered_map<Token::Type, std::string> keywordNames = createNames();

} // namespace

void Token::expect(Type t) const {
    if (t != _type) {
        throw ParsingError{*this,
                           "Expected " + std::to_string(t) + " got " +
                               std::to_string(type())};
    }
}

std::string_view Token::typeName() const {
    return tokenName(_type);
}

Token::Type tokenType(std::string_view str) {
    if (auto f = keywords.find(std::string{str}); f != keywords.end()) {
        return f->second;
    }

    return Token::Word;
}

std::string_view tokenName(Token::Type type) {
    if (auto f = keywordNames.find(type); f != keywordNames.end()) {
        return f->second;
    }

    return {};
}
