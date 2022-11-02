#pragma once

#include "file.h"
#include "token.h"
#include "tokenizer.h"
#include <memory>

class State {
public:
    State(std::shared_ptr<File> file)
        : _tokenizer{std::move(file)} {}

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

private:
    Tokenizer _tokenizer;
};
