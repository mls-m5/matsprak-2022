#pragma once

#include "ast/function.h"
#include "ast/module.h"
#include "errors.h"
#include "log.h"
#include "state.h"
#include "token.h"
#include "utils.h"

namespace {

Expression parseExpression(Module &m, FunctionBody &body, State &s);

Argument parseArgument(Module &m, State &s) {
    s.token().expect(Token::Word);

    auto arg = Argument{};
    arg.name = s.token();

    s.next().expect(Token::Colon);
    s.next().expect(Token::Word); // TODO: Handle this
    s.next();

    return arg;
}

FunctionSignature parseFunctionSignature(Module &m, State &s) {
    // TODO: make sure function bodies finds other function bodies
    auto f = FunctionSignature{};

    s.token().expect(Token::Fn);
    s.next().expect(Token::Word);

    f.name = s.token();
    s.next().expect(Token::BeginParen);
    s.next();

    for (; s.token().type() != Token::EndParen;) {
        f.arguments.push_back(parseArgument(m, s));
        if (s.token().type() == Token::Comma) {
            s.next();
            continue;
        }
        s.token().expect(Token::EndParen);
        break;
    }

    s.next();
    //    // TODO: handle parens
    //    skipGroup(s, Token::BeginParen);

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

    s.next().expect(Token::BeginParen);
    s.next();
    for (; s.token().type() != Token::EndParen;) {
        fc.args->args.push_back(parseExpression(m, body, s));
        if (s.token().type() == Token::Comma) {
            s.next();
            continue;
        }
        s.token().expect(Token::EndParen);
        break;
    }
    s.next();
    s.token().expect(Token::Semicolon);
    s.next();

    return fc;
}

Expression parseExpression(Module &m, FunctionBody &body, State &s) {
    switch (s.token().type()) {
    case Token::Word:
        if (s.peek() == Token::BeginParen) {
            return parseCallStatement(m, body, s);
        }
        break;
    case Token::StringLiteral:

    {
        auto sl = StringLiteral{s.token()};
        s.next();
        return sl;
    }

    default:
        break;
    }
    throw ParsingError(s.token(), "Unexpected symbol");
}

bool parseBlockStatement(Module &m, Function &f, State &s) {
    switch (s.token().type()) {

    case Token::EndBrace:
        s.next(); // End function
        return false;
    default:
        f.body.commands.push_back({parseExpression(m, f.body, s)});
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
