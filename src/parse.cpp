#include "parse.h"
#include <iostream>

Module parse(State &s) {
    auto root = Module{};

    if (s.token() == Token::Module) {
        s.next();
        root.name = s.token();
        s.next().expect(Token::Semicolon);

        vout << "module " << root.name.content << std::endl;
    }

    return Module{};
}
