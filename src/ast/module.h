#pragma once

#include "function.h"
#include "token.h"
#include <string_view>
#include <vector>

struct Module {
    Module() = default;
    ~Module() = default;
    Module(const Module &) = delete;
    Module(Module &&) = delete;
    Module &operator=(const Module &) = delete;
    Module &operator=(Module &&) = delete;

    Token name;

    std::vector<Function> functions;
    std::vector<Module *> imports;

    struct Workspace *workspace;

    Function *functionExact(std::string_view mangledName) {
        for (auto &f : functions) {
            if (f.signature.mangledName() == mangledName) {
                return &f;
            }
        }

        return nullptr;
    }
};
