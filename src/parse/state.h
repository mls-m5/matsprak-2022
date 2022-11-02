#pragma once

#include "ast/function.h"
#include "ast/module.h"
#include "file.h"
#include "token.h"
#include "tokenizer.h"
#include <memory>

struct Decorations {
    bool shouldExport = false;
    bool isMut = false;
};

// Temporary state of tokenizer and parsing. Disicarded after parsing is done
class State {
public:
    State(std::shared_ptr<File> file,
          Module &module,
          struct Workspace &workspace)
        : _tokenizer{std::move(file)}
        , _workspace{&workspace}
        , _module{&module} {}

    const Token &token() {
        return _tokenizer.token();
    }

    const Token &peek() {
        return _tokenizer.peek();
    }

    const Token &next() {
        return _tokenizer.next();
    }

    operator bool() {
        return _tokenizer;
    }

    void reset() {
        _tokenizer.reset();
    }

    Decorations &decorations() {
        return _decorations;
    }

    void resetDecorations() {
        _decorations = {};
    }

    Workspace &workspace() {
        return *_workspace;
    }

    Module &module() {
        return *_module;
    };

    // Current function
    Function *function() {
        return _function;
    }

    void function(Function *f) {
        _function = f;
    }

    Function *findFunction(Token name);

private:
    Tokenizer _tokenizer;
    Decorations _decorations;
    Workspace *_workspace = nullptr;
    Function *_function = nullptr;
    Module *_module = nullptr;
};
