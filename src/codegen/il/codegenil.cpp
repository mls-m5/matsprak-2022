#include "codegenil.h"
#include "ast/function.h"
#include "log.h"
#include <ostream>
#include <string>

namespace {

struct Codegen {
    std::ostream &out;

    std::string newName() {
        auto name = "%" + std::to_string(varNum);
        ++varNum;
        return name;
    }

    std::string newVar(std::string type) {
        auto name = newName();
        out << name << " = alloca " << type << "\n";

        return name;
    }

    int varNum = 1;
};

void codegen(std::ostream &out, const Function &f) {
    auto gen = Codegen{out};
    if (f.signature.name.content() == "main") {
        out << "define i32 @main(i32 %0, ptr %1) {\n";
        gen.varNum = 3;
    }
    else {
        out << "define i32 @" << f.signature._mangledName << "() {\n";
    }

    // TODO: Replace with actual code
    auto var1 = gen.newVar("i32");
    auto var2 = gen.newName();
    out << var2 << " = load i32, ptr " << var1 << "\n";
    out << "ret i32 " << var2 << "\n";

    out << "}";
}

} // namespace

void codegenIl(std::ostream &out, const Module &module) {
    vout << "\n\n----\ngenerating code" << std::endl;

    for (auto &f : module.functions) {
        codegen(out, *f);
        out << "\n\n";
    }
}
