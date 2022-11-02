#include "ast/function.h"
#include "ast/module.h"
#include "ast/workspace.h"
#include "errors.h"
#include "log.h"
#include "parsefunction.h"
#include <iostream>
#include <memory>

namespace {

void parseModuleStatement(Module &m, State &s) {
    if (s.token() == Token::Module) {
        s.next();
        m.name = s.token();
        s.next().expect(Token::Semicolon);
        s.next();

        vout << "module " << m.name.content() << std::endl;
    }
}

void parseImport(Module &m, State &s) {
    s.token().expect(Token::Import);
    s.next().expect(Token::Word);

    auto filename = m.workspace->findModulePath(s.token());
    if (filename.empty()) {
        throw ParsingError(s.token(), "could not find module with name");
    }

    s.next().expect(Token::Semicolon);
    s.next();
}

void parseHeader(Module &m, State &s) {
    while (s.token() == Token::Import) {
        parseImport(m, s);
    }
}

void skipHeader(State &s) {
    while (s.token() == Token::Import) {
        for (; s.token().type() != Token::Semicolon; s.next()) {
        }
        s.next();
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

} // namespace

Module *parse(State &s, Workspace &workspace) {
    workspace.modules.push_back(std::make_unique<Module>());
    auto root = workspace.modules.back().get();
    root->workspace = &workspace;

    if (!workspace.root) {
        workspace.root = root;
    }

    try {
        parseModuleStatement(*root, s);
        parseHeader(*root, s);
        while (s) {
            parseRootDefinitions(*root, s);
        }

        s.reset();

        parseModuleStatement(*root, s);
        skipHeader(s);
        while (s) {
            parseRoot(*root, s);
        }
    }
    catch (ParsingError &e) {
        std::cerr << e.what() << std::endl;
    }

    return root;
}
