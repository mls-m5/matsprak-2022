#include "codegenil.h"
#include "ast/function.h"
#include "log.h"
#include <ostream>
#include <string>

namespace {

std::string convertType(const Type &type) {
    auto str = type.name.content();
    if (str == "int") {
        return "i32";
    }
    if (str == "float") {
        return "float";
    }

    return "void";
}

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

void codegen(Codegen &gen, const FunctionSignature &f) {
    if (f.name.content() == "main") {
        gen << " " << convertType(f.type) << "  @main(i32 %0, ptr %1)";
        gen.varNum = 2;
    }
    else {
        gen << " i32 @" << f.mangledName() << "(";
        bool isFirst = true;
        for (auto &arg : f.arguments) {
            if (!isFirst) {
                gen << ", ";
            }
            isFirst = false;
            gen << convertType(arg.type) << " " << gen.newName();
        }
        gen << ") ";
    }
}

void codegen(Codegen &gen, const Function &f) {
    gen << "define ";
    codegen(gen, f.signature);
    gen << "{\n";

    ++gen.varNum;

    // TODO: Replace with actual code
    auto var1 = gen.newVar("i32");
    gen << "store i32 0, ptr " << var1 << "\n";
    auto var2 = gen.newName();
    gen << var2 << " = load i32, ptr " << var1 << "\n";
    gen << "ret i32 " << var2 << "\n";

    gen << "}";
}

void codegenImport(Codegen &gen, const Module &module) {
    for (auto &f : module.functions) {
        if (f->signature.shouldExport) {
            gen << "declare";
            codegen(gen, f->signature);
            gen << ";\n";
        }
    }
}

} // namespace

void codegenIl(std::ostream &out, const Module &module) {
    vout << "\n\n----\ngenerating code" << std::endl;

    auto gen = Codegen{out};

    out << "\n; imports\n";
    for (auto &m : module.imports) {
        codegenImport(gen, *m);
    }

    out << "\n; module\n";
    for (auto &f : module.functions) {
        gen.varNum = 0;
        codegen(gen, *f);
        out << "\n\n";
    }
}
