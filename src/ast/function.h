#pragma once

#include "token.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Type {
    Token name;
};

struct ArgumentType : public Type {
    bool isConst = true;
    bool isRef = false;
};

struct Argument {
    Token name;
    ArgumentType type;
};

struct FunctionSignature {
    Token name;
    std::vector<Argument> arguments;
    mutable std::string _mangledName = {};
    bool shouldExport = false;
    Type type;

    std::string mangledName() const {
        if (_mangledName.empty()) {
            // TODO: implement this
            _mangledName = std::string{name.content()};
        }
        return _mangledName;
    }
};

struct FunctionCall {
    ~FunctionCall();
    FunctionCall();
    struct Function *function = nullptr;
    std::shared_ptr<struct FunctionCallArgs> args;
};

struct StringLiteral {
    Token string;
};

using Expression = std::variant<FunctionCall, StringLiteral>;

struct FunctionCallArgs {
    std::vector<Expression> args;
};

struct Statement {
    Expression e;
};

struct FunctionBody {
    std::vector<Statement> commands;
};

struct Function {
    FunctionSignature signature;

    FunctionBody body;
};

inline FunctionCall::~FunctionCall() = default;

inline FunctionCall::FunctionCall()
    : args{std::make_shared<FunctionCallArgs>()} {}
