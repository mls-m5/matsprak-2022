
#include "ast/workspace.h"
#include "codegen/cpp/codegencpp.h"
#include "file.h"
#include "parse/parsemodule.h"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    auto workspace = std::make_unique<Workspace>();

    {
        workspace->moduleLookup = {
            {"main", "demo/1-basic.msk"},
            {"apa", "demo/1-import.msk"},
            {"std", "demo/1-std.msk"},
        };

        parseModule(openFile("demo/1-basic.msk"), *workspace);
    }

    if (workspace->hasParsingError) {
        std::cerr << "error while parsing... abort";
        return 1;
    }

    codegenCpp(std::cout, *workspace->root);

    return 0;
}
