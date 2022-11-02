#pragma once

#include "ast/module.h"
#include <ostream>
class Module;

void codegenIl(std::ostream &out, const Module &module);
