#include "codegenil.h"
#include "ast/function.h"
#include "errors.h"
#include "log.h"
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

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

struct GenType {
    GenType(const Type &type)
        : name{convertType(type)} {}

    std::string name;
    bool isPtr = false;

    static GenType fromString(std::string str, bool isPtr) {
        return GenType{std::move(str), isPtr};
    }

private:
    GenType(std::string name, bool isPtr)
        : name{name}
        , isPtr{isPtr} {}
};

std::ostream &operator<<(std::ostream &out, const GenType &type) {
    if (type.isPtr) {
        out << "ptr ";
    }
    out << type.name;
    return out;
}

struct GenVar {
    std::string name;
    GenType type;
};

struct Codegen {
    std::stringstream header;
    std::stringstream out;
    int varNum = 1;
    int stringNum = 0;
    const Function *currentFunction = nullptr;

    //    std::unordered_map<std::string, GenVar> variables;
    std::vector<GenVar> variables;

    void resetVarNum(int value) {
        varNum = value;
        variables.clear();
    }

    void skipNumber() {
        ++varNum;
    }

    // Pass empty string as name if the variable is anonymous (only used for il)
    GenVar newName(std::string saveName, GenType type) {
        auto name = "%" + std::to_string(varNum);
        auto var = GenVar{name, type};
        if (!saveName.empty()) {
            variables.push_back(var);
        }
        ++varNum;
        return var;
    }

    // Create variable with alloca, type will always be a pointer
    GenVar newVar(std::string storeName, GenType type) {
        type.isPtr = true;
        auto var = newName(storeName, type);
        out << var.name << " = alloca " << type.name << "\n";

        return var;
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

    GenVar lastVar() {
        return variables.back();
        //        return "%" + std::to_string(varNum - 1);
    }

    // LoadPtr
    // Needed by for example return statements
    GenVar load(GenVar var) {
        if (!var.type.isPtr) {
            return var;
        }

        auto var2 = newName("", GenType::fromString(var.type.name, false));

        println(var2.name, "= load", var2.type, ", ptr", var.name);

        return var2;
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
std::string codegen(Codegen &gen, const StatementVariant &e);

std::string codegen(Codegen &gen, const StringLiteral &s) {
    return "ptr " + gen.newString(s.string.str());
}

std::string codegen(Codegen &gen, const NumericLiteral &s) {
    //    return "ptr " + gen.newString(s.string.str());
    auto type = GenType(s.type());
    auto var = gen.newVar("", s.type());
    gen << "store " << type.name << " " << s.value << ", ptr " << var.name
        << "\n";
    return var.name;
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

    auto var = gen.newName("", f.function->signature.type);
    gen.println(var.name, " = ", ss.str());
    return var.name;
}

void codegen(Codegen &gen, const FunctionSignature &f) {
    if (f.name.content() == "main") {
        gen << " " << convertType(f.type) << "  @main(i32 %0, ptr %1)";
        gen.resetVarNum(2);
    }
    else {
        gen << " " << convertType(f.type) << " @" << f.mangledName() << "(";
        bool isFirst = true;
        for (auto &arg : f.arguments) {
            if (!isFirst) {
                gen << ", ";
            }
            isFirst = false;
            gen << convertType(arg.type) << " "
                << gen.newName(arg.name.str(), arg.type).name;
        }
        gen << ") ";
    }
}

std::string codegen(Codegen &gen, const VariableAccessor &d) {
    throw NotImplemented{"implement this"};
    return {};
}

std::string codegen(Codegen &gen, const VariableDeclaration &d) {
    auto name = gen.newVar(d.name.str(), d.type);

    return name.name;
}

std::string codegen(Codegen &gen, const BinaryExpression &e) {
    throw NotImplemented{"implement this"};
    return {};
}

std::string codegen(Codegen &gen, const ReturnStatement &e) {
    auto var = gen.lastVar();
    auto loadedVar = gen.load(var);
    gen.println("ret", loadedVar.type, loadedVar.name);

    // https://stackoverflow.com/questions/36094685/instruction-expected-to-be-numbered
    gen.skipNumber();
    return {};
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
        auto var1 = gen.newVar("", GenType::fromString("i32", true));
        gen << "\n; just return zero in case of emergency\n";
        gen << "store i32 0, ptr " << var1.name << "\n";
        auto var2 = gen.newName("", GenType::fromString("i32", true));
        gen << var2.name << " = load i32, ptr " << var1.name << "\n";
        gen << "ret i32 " << var2.name << "\n";
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
            gen.resetVarNum(0);
            codegen(gen, f->signature);
            gen << ";\n";
        }
    }
}

std::string codegen(Codegen &gen, const Expression &e) {
    return std::visit([&gen](auto &e) { return codegen(gen, e); }, e);
}

std::string codegen(Codegen &gen, const StatementVariant &e) {
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
        //        gen.varNum = 0;
        codegen(gen, *f);
        gen << "\n\n";
    }

    out << gen.header.str() << "\n";
    out << gen.out.str() << "\n";
}
