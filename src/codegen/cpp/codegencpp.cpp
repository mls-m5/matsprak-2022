
#include "codegencpp.h"
#include "ast/function.h"
#include "log.h"
#include <ostream>
#include <stdexcept>

namespace {

void codegen(std::ostream &out, const Expression &e);

void codegen(std::ostream &out, const FunctionSignature &signature) {
    out << "void " << signature.name << "()";
}

void codegen(std::ostream &out, const FunctionCall &f) {
    out << f.function->signature.name << "(";

    bool isFirst = true;

    for (auto &e : f.args->args) {
        if (!isFirst) {
            out << ", ";
        }
        isFirst = false;
        codegen(out, e);
    }

    out << ");\n";
}

void codegen(std::ostream &out, const StringLiteral &s) {
    out << s.string << " ";
}

void codegen(std::ostream &out, const Expression &e) {
    std::visit([&out](auto &e) { codegen(out, e); }, e);
}

void codegen(std::ostream &out, const FunctionBody &body) {
    out << "{\n";

    for (auto &command : body.commands) {
        codegen(out, command.e);
    }

    out << "}";
}

void codegenImport(std::ostream &out, const Module &module) {
    for (auto &f : module.functions) {
        if (f->signature.shouldExport) {
            codegen(out, f->signature);
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
        codegen(out, f->signature);
        out << ";\n";
    }
    out << "\n";

    for (auto &f : module.functions) {
        codegen(out, f->signature);
        codegen(out, f->body);
        out << "\n\n";
    }
}
