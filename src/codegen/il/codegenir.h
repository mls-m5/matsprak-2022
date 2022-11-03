#pragma once

#include "ast/module.h"
#include <ostream>
class Module;

void codegenIr(std::ostream &out, const Module &module);
