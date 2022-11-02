
#include "codegencpp.h"
#include "log.h"

namespace {

void codegen(std::ostream &out, const FunctionSignature &signature) {
    out << "void " << signature.name << "() \n";
}

} // namespace

void codegenCpp(std::ostream &out, const Module &module) {
    vout << "\n\n-----\ngenerating code" << std::endl;

    for (auto &f : module.functions) {
        codegen(out, f.signature);
        out << "{}\n\n";
    }
}
