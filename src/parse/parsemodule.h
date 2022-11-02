#pragma once

#include "ast/module.h"
#include "state.h"
#include <memory>

Module *parse(State &s, Workspace &);
