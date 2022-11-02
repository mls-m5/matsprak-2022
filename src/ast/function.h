#pragma once

#include "token.h"
#include <string>
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

struct Function {
    FunctionSignature signature;
};
