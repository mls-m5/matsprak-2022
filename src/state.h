#pragma once

#include "file.h"
#include "token.h"
#include "tokenizer.h"
#include <memory>

struct Decorations {
    bool shouldExport = false;
    bool isMut = false;
};

class State {
public:
    State(std::shared_ptr<File> file, struct Workspace &workspace)
        : _tokenizer{std::move(file)}
        , _workspace{&workspace} {}

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

private:
    Tokenizer _tokenizer;
    Decorations _decorations;
    Workspace *_workspace = nullptr;
};
