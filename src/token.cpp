#include "token.h"
#include "errors.h"
#include "tokenizer.h"
#include <cctype>
#include <string>
#include <unordered_map>

namespace {} // namespace

void Token::expect(Type t) const {
    if (t != _type) {
        throw ParsingError{*this,
                           "Expected '" + std::string{tokenName(t)} + "' (" +
                               std::to_string(t) + ") got '" +
                               std::string{tokenName(type())} + "' (" +
                               std::to_string(type()) + ")"};
    }
}
