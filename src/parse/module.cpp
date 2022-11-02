#include "module.h"
#include "ast/function.h"
#include "errors.h"
#include "function.h"
#include "log.h"
#include <iostream>

void parseModuleStatement(Module &m, State &s) {
    if (s.token() == Token::Module) {
        s.next();
        m.name = s.token();
        s.next().expect(Token::Semicolon);
        s.next();

        vout << "module " << m.name.content() << std::endl;
    }
}

void parseRootDefinitions(Module &m, State &s) {
    auto &token = s.token();
    switch (token.type()) {
    case Token::Fn:
        m.functions.push_back(
            Function{.signature = parseFunctionSignature(m, s)});
        skipGroup(s, Token::BeginBrace);
        break;
    default:
        throw ParsingError{token, "unexpected expression"};
    }
}

void parseRoot(Module &m, State &s) {
    auto &token = s.token();
    switch (token.type()) {
    case Token::Fn:
        parseFunction(m, s);
        break;
    default:
        throw ParsingError{token, "unexpected expression"};
    }
}

Module parse(State &s) {
    auto root = Module{};
    try {
        parseModuleStatement(root, s);
        while (s) {
            parseRootDefinitions(root, s);
        }

        s.reset();

        parseModuleStatement(root, s);
        while (s) {
            parseRoot(root, s);
        }
    }
    catch (ParsingError &e) {
        std::cerr << e.what() << std::endl;
    }

    return Module{};
}
