
#include "codegencpp.h"
#include "ast/function.h"
#include "log.h"
#include <ostream>
#include <stdexcept>

namespace {

void codegen(std::ostream &out, const FunctionSignature &signature) {
    out << "void " << signature.name << "()";
}

void codegen(std::ostream &out, const FunctionCall &f) {
    out << f.function->signature.name << "();\n";
}

void codegen(std::ostream &out, const Expression &f) {
    throw std::runtime_error{"not implemented"};
}

void codegen(std::ostream &out, const FunctionBody &body) {
    out << "{\n";

    struct Visitor {
        std::ostream &out;

        void operator()(const FunctionCall &f) {
            codegen(out, f);
        }
        void operator()(const Expression &f) {
            out << "expression\n";
        }
    };

    for (auto &command : body.commands) {
        //        std::visit(Visitor{out}, command.e);
        std::visit([&out](auto &e) { codegen(out, e); }, command.e);
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
