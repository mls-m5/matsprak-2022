#pragma once

#include "ast/function.h"
#include "ast/module.h"
#include "log.h"
#include "state.h"
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

void parseFunction(Module &m, State &s) {
    auto signature = parseFunctionSignature(m, s);

    auto f = m.functionExact(signature.mangledName());

    vout << "parning function " << f->signature.mangledName() << std::endl;

    skipGroup(s, Token::BeginBrace);
}

} // namespace
