#include "codegenil.h"
#include "ast/function.h"
#include "log.h"
#include <ostream>
#include <sstream>
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
    if (str == "str") {
        return "ptr";
    }

    return "void";
}

struct Codegen {
    std::stringstream header;
    std::stringstream out;
    int varNum = 1;
    int stringNum = 0;
    const Function *currentFunction = nullptr;

    std::string newName() {
        auto name = "%" + std::to_string(varNum);
        ++varNum;
        return name;
    }

    std::string newString(std::string value) {
        value.pop_back();
        value += "\\00\"";
        auto name = "@.str." + std::to_string(stringNum);
        header << name << " = private unnamed_addr constant ["
               << (value.size() - 2 - 2) << " x i8] c" << value << " align 1\n";
        ++stringNum;
        return name;
    }

    std::string lastVar() {
        return "%" + std::to_string(varNum - 1);
    }

    std::string newVar(std::string type) {
        auto name = newName();
        out << name << " = alloca " << type << "\n";

        return name;
    }

    template <typename T>
    friend std::ostream &operator<<(Codegen &gen, const T &t) {
        gen.out << t;
        return gen.out;
    }

    template <typename... Args>
    void println(Args... args) {
        ((out << " " << args), ...) << "\n";
    }
};

std::string codegen(Codegen &gen, const Expression &e);

std::string codegen(Codegen &gen, const StringLiteral &s) {
    return "ptr " + gen.newString(s.string.str());
}

std::string codegen(Codegen &gen, const FunctionCall &f) {
    auto ss = std::stringstream{};
    ss << "call " << convertType(f.function->signature.type) << " @"
       << f.function->signature.mangledName() << " (";

    bool isFirst = true;

    for (auto &e : f.args->args) {
        if (!isFirst) {
            ss << ", ";
        }
        isFirst = false;
        auto resName = codegen(gen, e);
        ss << resName;
    }

    ss << ")\n";

    if (f.function->signature.type.name.content() == "") {
        gen.println(ss.str());
        return "";
    }

    auto name = gen.newName();
    gen.println(name, " = ", ss.str());
    return name;
}

void codegen(Codegen &gen, const FunctionSignature &f) {
    if (f.name.content() == "main") {
        gen << " " << convertType(f.type) << "  @main(i32 %0, ptr %1)";
        gen.varNum = 2;
    }
    else {
        gen << " " << convertType(f.type) << " @" << f.mangledName() << "(";
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

void codegen(Codegen &gen, const FunctionBody &body) {
    for (auto &command : body.commands) {
        codegen(gen, command.e);
    }

    //    if (body.commands.empty()) {

    if (gen.currentFunction->signature.type.name.content().empty()) {
        gen << "ret void\n";
    }
    else {
        auto var1 = gen.newVar("i32");
        gen << "; just return zero\n";
        gen << "store i32 0, ptr " << var1 << "\n";
        auto var2 = gen.newName();
        gen << var2 << " = load i32, ptr " << var1 << "\n";
        gen << "ret i32 " << var2 << "\n";
        //    gen << "}\n";
        //    return;
    }

    gen << "}\n";
}

void codegen(Codegen &gen, const Function &f) {
    gen.currentFunction = &f;
    gen << "define ";
    codegen(gen, f.signature);
    gen << "{\n";

    ++gen.varNum;

    codegen(gen, f.body);
    gen.currentFunction = nullptr;
}

void codegenImport(Codegen &gen, const Module &module) {
    for (auto &f : module.functions) {
        if (f->signature.shouldExport) {
            gen << "declare";
            gen.varNum = 0;
            codegen(gen, f->signature);
            gen << ";\n";
        }
    }
}

std::string codegen(Codegen &gen, const Expression &e) {
    return std::visit([&gen](auto &e) { return codegen(gen, e); }, e);
}

} // namespace

void codegenIl(std::ostream &out, const Module &module) {
    vout << "\n\n----\ngenerating code" << std::endl;

    auto gen = Codegen{};

    gen << "\n; imports\n";
    for (auto &m : module.imports) {
        codegenImport(gen, *m);
    }

    gen << "\n; module\n";
    for (auto &f : module.functions) {
        gen.varNum = 0;
        codegen(gen, *f);
        gen << "\n\n";
    }

    out << gen.header.str() << "\n";
    out << gen.out.str() << "\n";
}
