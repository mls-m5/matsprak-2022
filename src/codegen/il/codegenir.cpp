#include "codegenir.h"
#include "ast/function.h"
#include "errors.h"
#include "log.h"
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
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

int getTypeAlignment(std::string_view name) {
    static auto map = std::unordered_map<std::string_view, int>{
        {"i32", 4},
        {"i16", 4},
        {"i8", 1},
        {"ptr", 8},
        {"void", 0},
    };

    return map.at(name);
}

struct GenType {
    GenType(const Type &type)
        : name{convertType(type)}
        , align{getTypeAlignment(name)} {}

    std::string name;
    bool isPtr = false;
    int align = 0;

    static GenType fromString(std::string str, bool isPtr) {
        return GenType{std::move(str), isPtr};
    }

private:
    GenType(std::string name, bool isPtr)
        : name{name}
        , isPtr{isPtr}
        , align{getTypeAlignment(name)} {}
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

GenVar voidVar() {
    return {"", GenType::fromString("void", false)};
}

struct Codegen {
    std::stringstream header;
    std::stringstream out;
    int varNum = 1;
    int stringNum = 0;
    const Function *currentFunction = nullptr;

    //    std::unordered_map<std::string, GenVar> variables;
    std::vector<std::pair<std::string, GenVar>> variables;

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
            variables.push_back({saveName, var});
        }
        ++varNum;
        return var;
    }

    // Create variable with alloca, type will always be a pointer
    GenVar newVar(std::string storeName, GenType type) {
        type.isPtr = true;
        auto var = newName(storeName, type);
        println(var.name, "= alloca", type.name, ", align ", type.align);

        return var;
    }

    GenVar findVariable(std::string name) {
        for (auto &pair : variables) {
            if (pair.first == name) {
                return pair.second;
            }
        }
        throw std::runtime_error{
            "could not find variable when generating il: " + name};
    }

    GenVar newString(std::string value) {
        value.pop_back();
        value += "\\00\"";
        auto name = "@.str." + std::to_string(stringNum);
        header << name << " = private unnamed_addr constant ["
               << (value.size() - 2 - 2) << " x i8] c" << value << " align 1\n";
        ++stringNum;

        auto var = newName("", GenType::fromString("i8", true));

        println(var.name, "ptr", name);

        return var;
    }

    GenVar lastVar() {
        return variables.back().second;
        //        return "%" + std::to_string(varNum - 1);
    }

    // LoadPtr
    // Needed by for example return statements
    GenVar load(GenVar var) {
        if (!var.type.isPtr) {
            return var;
        }

        auto var2 = newName("", GenType::fromString(var.type.name, false));

        println(var2.name,
                "= load",
                var2.type,
                ", ptr",
                var.name,
                ", align",
                var.type.align);

        return var2;
    }

    void store(GenVar from, GenVar to) {
        if (from.type.isPtr) {
            throw GenError{"cannot store of ptr type"};
        }
        println("store",
                from.type.name,
                from.name,
                ", ptr",
                to.name,
                ", align",
                to.type.align);
    }

    template <typename T>
    friend std::ostream &operator<<(Codegen &gen, const T &t) {
        gen.out << t;
        return gen.out;
    }

    template <typename... Args>
    void println(Args... args) {
        out << "  ";
        ((out << " " << args), ...) << "\n";
    }
};

GenVar codegen(Codegen &gen, const Expression &e);
GenVar codegen(Codegen &gen, const StatementVariant &e);

GenVar codegen(Codegen &gen, const StringLiteral &s) {

    return gen.newString(s.string.str());
}

GenVar codegen(Codegen &gen, const NumericLiteral &s) {
    //    return "ptr " + gen.newString(s.string.str());
    auto type = GenType(s.type());
    auto var = gen.newVar("", s.type());
    gen.println("store ",
                type.name,
                s.value,
                ", ptr",
                var.name,
                ", align",
                var.type.align);
    return var;
}

GenVar codegen(Codegen &gen, const FunctionCall &f) {
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
        ss << resName.name;
    }

    ss << ")\n";

    if (f.function->signature.type.name.content() == "") {
        gen.println(ss.str());
        return voidVar();
    }

    auto var = gen.newName("", f.function->signature.type);
    gen.println(var.name, " = ", ss.str());
    return var;
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

GenVar codegen(Codegen &gen, const VariableAccessor &d) {
    return gen.findVariable(d.name.str());
    throw NotImplemented{"implement this"};
}

GenVar codegen(Codegen &gen, const VariableDeclaration &d) {
    auto name = gen.newVar(d.name.str(), d.type);

    if (d.e) {
        auto reference = codegen(gen, *d.e);
        auto value = gen.load(reference);

        gen.store(value, name);
    }

    return name;
}

GenVar codegen(Codegen &gen, const BinaryExpression &e) {
    auto left = codegen(gen, *e.left);
    auto right = codegen(gen, *e.right);

    auto a = gen.load(left);
    auto b = gen.load(right);

    auto res = gen.newName("", e.type());

    // TODO: Handle different types of operations
    gen.println(res.name, "= add ", a.type, a.name, ",", b.name);
    return res;
}

GenVar codegen(Codegen &gen, const ReturnStatement &e) {
    auto var = codegen(gen, e.e);
    auto loadedVar = gen.load(var);
    gen.println("ret", loadedVar.type, loadedVar.name);

    // https://stackoverflow.com/questions/36094685/instruction-expected-to-be-numbered
    gen.skipNumber();
    return voidVar();
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
        //        auto var1 = gen.newVar("", GenType::fromString("i32", true));
        //        gen << "\n; just return zero in case of emergency\n";
        //        gen << "store i32 0, ptr " << var1.name << "\n";
        //        auto var2 = gen.newName("", GenType::fromString("i32", true));
        //        gen << var2.name << " = load i32, ptr " << var1.name << "\n";
        //        gen << "ret i32 " << var2.name << "\n";
        //    gen << "}\n";
        //    return;
        gen.println("; return 0 just in case");
        gen.println("ret i32 0");
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

GenVar codegen(Codegen &gen, const Expression &e) {
    return std::visit([&gen](auto &e) { return codegen(gen, e); }, e);
}

GenVar codegen(Codegen &gen, const StatementVariant &e) {
    return std::visit([&gen](auto &e) { return codegen(gen, e); }, e);
}

} // namespace

void codegenIr(std::ostream &out, const Module &module) {
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
