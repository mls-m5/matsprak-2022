#include "parse.h"
#include "errors.h"
#include "parse/function.h"
#include <iostream>

// Use type Token::Any to match anything
void skipGroup(State &s, Token::Type type) {
    Token::Type begin = s.token().type();
    Token::Type end = Token::Eof;

    if (s.token().type() != Token::Any) {
        s.token().expect(type);
    }

    switch (begin) {
    case Token::BeginBrace:
        end = Token::EndBrace;
        break;
    case Token::BeginParen:
        end = Token::EndParen;
        break;
    case Token::BeginBracket:
        end = Token::EndBracket;
        break;
    default:
        throw ParsingError{s.token(),
                           "expected group but got " +
                               std::string{s.token().content()}};
    }

    int depth = 1;

    s.next();

    for (const Token *t = &s.token(); depth; t = &s.next()) {
        if (t->type() == begin) {
            ++depth;
        }
        else if (t->type() == end) {
            --depth;
        }
    }
}

Function parseFunctionDefinition(Module &m, State &s) {
    // TODO: make sure function bodies finds other function bodies
    auto f = Function{};

    s.token().expect(Token::Fn);
    s.next().expect(Token::Word);

    f.name = s.token();
    s.next().expect(Token::BeginParen);

    // TODO: handle parens
    skipGroup(s, Token::BeginParen);

    vout << "function " << f.name.content() << "()" << std::endl;

    return f;
}

void parseRoot(Module &m, State &s) {
    auto &token = s.token();
    switch (token.type()) {
    case Token::Fn:
        m.functions.push_back(parseFunctionDefinition(m, s));
        skipGroup(s, Token::BeginBrace);
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
