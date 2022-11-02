#include "parse.h"
#include "errors.h"
#include "parse/function.h"
#include <iostream>

void skipGroup(State &s) {

    //    switch (s.token().type()) {}

    s.token().expect(Token::BeginBrace);

    int depth = 1;

    for (const Token *t = &s.token(); depth; t = &s.next()) {
        switch (t->type()) {
        case Token::BeginBrace:
            ++depth;
            break;
        case Token::EndBrace:
            ++depth;
            break;
        default:
            break;
        }
    }

    s.next();
}

Function parseFunctionDefinition(Module &m, State &s) {
    // TODO: make sure function bodies finds other function bodies
    auto f = Function{};

    s.token().expect(Token::Fn);
    s.next().expect(Token::Word);

    f.name = s.token();
    s.next().expect(Token::BeginParen);

    vout << "function " << f.name.content() << "()" << std::endl;

    // TODO: handle parens
    s.next().expect(Token::EndParen);
    s.next();

    skipGroup(s);

    return f;
}

void parseRoot(Module &m, State &s) {
    auto &token = s.token();
    switch (token.type()) {
    case Token::Fn:
        m.functions.push_back(parseFunctionDefinition(m, s));
        skipGroup(s);
        break;
    default:
        throw ParsingError{token, "unexpected expression"};
    }
}

Module parse(State &s) {
    auto root = Module{};

    try {

        if (s.token() == Token::Module) {
            s.next();
            root.name = s.token();
            s.next().expect(Token::Semicolon);
            s.next();

            vout << "module " << root.name.content() << std::endl;
        }

        while (s) {
            parseRoot(root, s);
        }
    }
    catch (ParsingError &e) {
        std::cerr << e.what() << std::endl;
    }

    return Module{};
}
