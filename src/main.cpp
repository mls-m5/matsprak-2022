
#include "ast/workspace.h"
#include "codegen/cpp/codegencpp.h"
#include "codegen/il/codegenir.h"
#include "file.h"
#include "log.h"
#include "parse/parsemodule.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>

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

    //    codegenCpp(std::cout, *workspace->root);

    auto tmpPath = std::filesystem::path{"tmp.ll"};
    {
        auto file = std::ofstream{tmpPath};
        codegenIr(file, *workspace->root);
        std::cout.flush();
    }
    std::system(("cat " + tmpPath.string()).c_str());
    if (std::system(("llc-16 " + tmpPath.string()).c_str())) {
        std::cerr << "failed to compile ir code\n";
        return 1;
    }

    std::cout << "il compiled" << std::endl;

    if (std::system(("clang++-16 " + tmpPath.replace_extension(".s").string() +
                     " -o " + tmpPath.replace_extension("").string())
                        .c_str())) {
        std::cerr << "failed to compile assembly\n";
        return 1;
    }

    std::cout << "executable complete" << std::endl;

    int value =
        std::system(("./" + tmpPath.replace_extension("").string()).c_str());

    std::cout << "executable returned " << WEXITSTATUS(value) << std::endl;

    return 0;
}
