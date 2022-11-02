
#include "ast/workspace.h"
#include "codegen/cpp/codegencpp.h"
#include "codegen/il/codegenil.h"
#include "file.h"
#include "log.h"
#include "parse/parsemodule.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    auto workspace = std::make_unique<Workspace>();

    logging::isVerbose = true;

    {
        int version = 2;
        if (version == 1) {
            workspace->moduleLookup = {
                {"main", "demo/1-basic.msk"},
                {"apa", "demo/1-import.msk"},
                {"std", "demo/1-std.msk"},
            };
        }
        if (version == 2) {
            workspace->moduleLookup = {
                {"main", "demo/2-vars.msk"},
            };
        }

        auto f = workspace->findModulePath(Token::fromString("main"));

        parseModule(openFile(f), *workspace);
    }

    if (workspace->hasParsingError) {
        std::cerr << "error while parsing... abort";
        return 1;
    }

    codegenCpp(std::cout, *workspace->root);

    auto tmpPath = std::filesystem::path{"tmp.ll"};
    {
        auto file = std::ofstream{tmpPath};
        codegenIl(file, *workspace->root);
        std::cout.flush();
    }
    std::system(("cat " + tmpPath.string()).c_str());
    std::system(("llc-16 " + tmpPath.string()).c_str());

    return 0;
}
