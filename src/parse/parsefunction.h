#pragma once

#include "ast/function.h"
#include "ast/module.h"
#include "errors.h"
#include "log.h"
#include "state.h"
#include "token.h"
#include <optional>

namespace {

Expression parseExpression(Module &m, State &s);

Argument parseArgument(Module &m, State &s) {
    s.token().expect(Token::Word);

    auto arg = Argument{};
    arg.name = s.token();

    s.next().expect(Token::Colon);
    s.next().expect(Token::Word);

    arg.type.name = s.token();

    s.next();

    return arg;
}

Type parseType(Module &m, State &s) {
    auto type = Type{.name = s.token()};
    s.next();
    return type;
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

    f.shouldExport = s.decorations().shouldExport;

    if (s.token() == Token::RightArrow) {
        s.next();
        f.type = {parseType(m, s)};
    }

    vout << "function signature " << f.mangledName() << "()" << std::endl;

    return f;
}

Expression parseCallStatement(Module &m, State &s) {
    auto fc = FunctionCall{};

    fc.function = s.findFunction(s.token());
    if (!fc.function) {
        throw ParsingError{s.token(), "could not find function"};
    }

    s.next().expect(Token::BeginParen);
    s.next();
    for (; s.token().type() != Token::EndParen;) {
        fc.args->args.push_back(parseExpression(m, s));
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

// Expression parseParentheses(Module &m, State &s) {}

Expression parseBinary(Module &m, State &s) {}

Expression parseExpression(Module &m, State &s) {
    auto &f = s.function();
    auto &body = f.body;

    switch (s.token().type()) {
    case Token::Word:
        if (s.peek() == Token::BeginParen) {
            return parseCallStatement(m, s);
        }
        else if (s.peek() == Token::Operator) {
            throw "implement this";
        }
        break;
    case Token::BeginParen:
    case Token::StringLiteral: {
        auto sl = StringLiteral{s.token()};
        s.next();
        return {sl};
    }
    case Token::NumericLiteral: {
        auto nl = NumericLiteral{s.token()};
        s.next();
        return {nl};
    }

    default:
        break;
    }
    throw ParsingError(s.token(), "Unexpected symbol");
}

void parseVariableDeclaration(Module &m, State &s) {
    auto &body = s.function().body;

    s.next().expect(Token::Word);

    auto name = s.token();
    s.next();

    auto var = body.addVariable(name);

    if (!var) {
        throw ParsingError{
            s.token(), "variable " + s.token().str() + " is already declared"};
    }

    bool hasDefinedType = false;

    if (s.token() == Token::Colon) {
        s.next();
        var->type = {parseType(m, s)};
    }

    if (s.token() == Token::Equal) {
        s.next();
        auto exp = parseExpression(m, s);
        auto var = body.findVariable(name); // Could have been changed
        if (!hasDefinedType) {
            var->type = expressionType(exp);
        }
        body.commands.push_back({VariableDeclaration{name, var->type}});
        body.commands.push_back({exp});
    }

    s.expectNext(Token::Semicolon);
}

void parseReturnStatement(Module &m, State &s) {
    s.next();

    s.function().body.commands.push_back(
        {ReturnStatement{parseExpression(m, s)}});

    s.token().expect(Token::Semicolon);
    s.next();
}

bool parseBlockStatement(Module &m, State &s) {
    auto &f = s.function();

    switch (s.token().type()) {
    case Token::EndBrace:
        s.next(); // End function
        return false;
    case Token::Let:
        parseVariableDeclaration(m, s);
        break;
    case Token::Return:
        parseReturnStatement(m, s);
        break;
    default:
        f.body.commands.push_back({parseExpression(m, s)});
    }
    return true;
}

void parseFunctionBody(Module &m, State &s) {
    s.token().expect(Token::BeginBrace);
    s.next();

    for (; s;) {
        if (!parseBlockStatement(m, s)) {
            break;
        }
    }
}

void parseFunction(Module &m, State &s) {
    auto signature = parseFunctionSignature(m, s);

    auto f = m.functionExact(signature.mangledName(), false);

    s.function(f);

    if (!f) {
        throw ParsingError{s.token(),
                           "internal error, could not find function " +
                               signature.mangledName()};
    }

    vout << "parsing function " << f->signature.mangledName() << std::endl;

    parseFunctionBody(m, s);

    s.function(nullptr);
}

} // namespace
