#pragma once

#include "token.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct ArgumentType {
    Token name;
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
    std::string _mangledName = {};
    bool shouldExport = false;

    std::string mangledName() {
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
