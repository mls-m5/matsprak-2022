#pragma once

#include "ast/function.h"
#include "ast/module.h"
#include "errors.h"
#include "log.h"
#include "state.h"
#include "token.h"
#include "utils.h"

namespace {

FunctionSignature parseFunctionSignature(Module &m, State &s) {
    // TODO: make sure function bodies finds other function bodies
    auto f = FunctionSignature{};

    s.token().expect(Token::Fn);
    s.next().expect(Token::Word);

    f.name = s.token();
    s.next().expect(Token::BeginParen);

    // TODO: handle parens
    skipGroup(s, Token::BeginParen);

    f.shouldExport = s.decorations().shouldExport;

    vout << "function signature " << f.mangledName() << "()" << std::endl;

    return f;
}

FunctionCall parseCallStatement(Module &m, FunctionBody &body, State &s) {
    auto fc = FunctionCall{};

    fc.function = s.findFunction(s.token());
    if (!fc.function) {
        throw ParsingError{s.token(), "could not find function"};
    }

    s.next();

    skipGroup(s, Token::BeginParen);

    //    s.next().expect(Token::BeginParen);
    //    s.next().expect(Token::BeginParen);
    s.token().expect(Token::Semicolon);
    s.next();

    return fc;
}

bool parseBlockStatement(Module &m, Function &f, State &s) {
    switch (s.token().type()) {
    case Token::Word:
        f.body.commands.push_back({parseCallStatement(m, f.body, s)});
        break;

    case Token::EndBrace:
        s.next(); // End function
        return false;
    default:
        throw ParsingError(s.token(), "Unexpected symbol");
    }
    return true;
}

void parseFunctionBody(Module &m, Function &f, State &s) {
    s.token().expect(Token::BeginBrace);
    s.next();

    for (; s;) {
        if (!parseBlockStatement(m, f, s)) {
            break;
        }
    }
}

void parseFunction(Module &m, State &s) {
    auto signature = parseFunctionSignature(m, s);

    auto f = m.functionExact(signature.mangledName(), false);
    if (!f) {
        throw ParsingError{s.token(),
                           "internal error, could not find function " +
                               signature.mangledName()};
    }

    vout << "parsing function " << f->signature.mangledName() << std::endl;

    parseFunctionBody(m, *f, s);
}

} // namespace
