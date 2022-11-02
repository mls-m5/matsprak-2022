#pragma once

#include "token.h"
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
};

struct Function {
    FunctionSignature signature;
};
