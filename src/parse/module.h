#pragma once

#include "function.h"
#include "token.h"
#include <vector>

struct Module {
    Token name;

    std::vector<Function> functions;
};
