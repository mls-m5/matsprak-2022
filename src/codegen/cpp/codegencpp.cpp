
#include "codegencpp.h"
#include "log.h"
#include <ostream>

namespace {

void codegen(std::ostream &out, const FunctionSignature &signature) {
    out << "void " << signature.name << "()";
}

void codegenImport(std::ostream &out, const Module &module) {
    for (auto &f : module.functions) {
        if (f.signature.shouldExport) {
            codegen(out, f.signature);
            out << ";\n";
        }
    }
}

} // namespace

void codegenCpp(std::ostream &out, const Module &module) {
    vout << "\n\n-----\ngenerating code" << std::endl;

    out << "\n//imports\n";
    for (auto &m : module.imports) {
        codegenImport(out, *m);
    }

    out << "\n//module " << module.name << "\n";

    for (auto &f : module.functions) {
        codegen(out, f.signature);
        out << ";\n";
    }
    out << "\n";
    for (auto &f : module.functions) {
        codegen(out, f.signature);
        out << "{}\n\n";
    }
}
