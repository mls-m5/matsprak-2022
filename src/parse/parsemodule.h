#pragma once

#include "ast/module.h"
#include <memory>

Module *parseModule(std::shared_ptr<File> file, Workspace &workspace);
