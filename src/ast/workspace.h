#pragma once

#include "module.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

struct Workspace {
    std::unordered_map<std::string, std::filesystem::path> moduleLookup;

    std::filesystem::path findModulePath(const Token &token) {
        auto str = std::string{token.content()};
        if (auto f = moduleLookup.find(str); f != moduleLookup.end()) {
            return f->second;
        }

        return {};
    }

    std::vector<std::unique_ptr<Module>> modules;

    Module *root = nullptr;
};
