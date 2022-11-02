#include "state.h"
#include "ast/workspace.h"

Function *State::findFunction(Token name) {
    for (auto &m : _workspace->modules) {
        if (auto f = m->functionExact(name.content(), true)) {
            return f;
        }
    }

    return _module->functionExact(name.content(), false);
}
