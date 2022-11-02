#include "codegenil.h"
#include "ast/function.h"
#include "log.h"

namespace {

void codegen(std::ostream &out, const FunctionSignature &signature) {
    out << "void " << signature.name << "() \n";
}

} // namespace

void codegenIl(std::ostream &out, const Module &module) {
    vout << "generating code" << std::endl;

    for (auto &f : module.functions) {
        codegen(out, f.signature);
        out << "{}\n\n";
    }
}
