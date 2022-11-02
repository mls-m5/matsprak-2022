#pragma once

#include "ast/module.h"
#include <ostream>

void codegenCpp(std::ostream &out, const Module &module);
