
#include "ast/workspace.h"
#include "codegen/cpp/codegencpp.h"
#include "codegen/il/codegenil.h"
#include "file.h"
#include "parse/parsemodule.h"
#include "state.h"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    auto state = State(openFile("demo/1-basic.msk"));

    auto workspace = std::make_unique<Workspace>();

    workspace->moduleLookup = {
        {"main", "demo/1-basic.msk"},
        {"apa", "demo/1-import.msk"},
    };

    auto root = parse(state, *workspace);

    //    codegenIl(std::cout, *root);
    codegenCpp(std::cout, *root);

    return 0;
}
