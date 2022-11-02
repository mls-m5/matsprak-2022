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

    template <typename T>
    friend std::ostream &operator<<(Codegen &gen, const T &t) {
        gen.out << t;
        return gen.out;
    }
};

void codegen(Codegen &gen, const Function &f) {
    if (f.signature.name.content() == "main") {
        gen << "define i32 @main(i32 %0, ptr %1) {\n";
        gen.varNum = 2;
    }
    else {
        gen << "define i32 @" << f.signature._mangledName << "(";
        bool isFirst = true;
        for (auto &arg : f.signature.arguments) {
            if (!isFirst) {
                gen << ", ";
            }
            isFirst = false;
            gen << "i32 " << gen.newName();
        }
        gen << ") {\n";
    }

    ++gen.varNum;

    // TODO: Replace with actual code
    auto var1 = gen.newVar("i32");
    gen << "store i32 0, ptr " << var1 << "\n";
    auto var2 = gen.newName();
    gen << var2 << " = load i32, ptr " << var1 << "\n";
    gen << "ret i32 " << var2 << "\n";

    gen << "}";
}

} // namespace

void codegenIl(std::ostream &out, const Module &module) {
    vout << "\n\n----\ngenerating code" << std::endl;

    auto gen = Codegen{out};

    for (auto &f : module.functions) {
        gen.varNum = 0;
        codegen(gen, *f);
        out << "\n\n";
    }
}
