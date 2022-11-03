#pragma once

#include "token.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Type {
    Token name;
};

struct DecoratedType : public Type {
    bool isConst = true;
    bool isRef = false;

    static DecoratedType stringType() {
        // Handle this better in the future
        return {{Token::fromString("string")}, true};
    }

    static DecoratedType numericType() {
        // Handle this better in the future
        return {Token::fromString("int")};
    }
};

struct Argument {
    Token name;
    DecoratedType type;
};

struct Variable {
    Token name;
    DecoratedType type;
};

struct FunctionSignature {
    Token name;
    std::vector<Argument> arguments;
    mutable std::string _mangledName = {};
    bool shouldExport = false;
    DecoratedType type;

    std::string mangledName() const {
        if (_mangledName.empty()) {
            // TODO: implement this
            _mangledName = std::string{name.content()};
        }
        return _mangledName;
    }
};

// Expressions
// ------------------------------------
class Expression;

struct FunctionCall {
    ~FunctionCall();
    FunctionCall();
    struct Function *function = nullptr;
    std::shared_ptr<struct FunctionCallArgs> args;

    DecoratedType type() const;
};

struct StringLiteral {
    Token string;

    DecoratedType type() const {
        return DecoratedType::stringType();
    }
};

struct NumericLiteral {
    Token value;

    DecoratedType type() const {
        return DecoratedType::numericType();
    }
};

struct BinaryExpression {
    Token op;
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    DecoratedType type() const;
};

struct VariableAccessor {
    Token name;
    DecoratedType varType;

    DecoratedType type() const {
        return varType;
    }
};

#define EXPRESSION_LIST                                                        \
    FunctionCall, StringLiteral, NumericLiteral, BinaryExpression,             \
        VariableAccessor

// Done like this to be able to forward declare it
class Expression : public std::variant<EXPRESSION_LIST> {
public:
    using variant::variant;
};

// Statements
// -------------------------------------------

struct VariableDeclaration {
    Token name;
    DecoratedType type;
};

struct ReturnStatement {
    Expression e;
};

#define STATEMENT_LIST VariableDeclaration, ReturnStatement

inline const DecoratedType expressionType(const Expression &e) {
    return std::visit([](auto &e) { return e.type(); }, e);
}

struct FunctionCallArgs {
    std::vector<Expression> args;
};

using StatementVariant = std::variant<EXPRESSION_LIST, STATEMENT_LIST>;

struct Statement {
    Statement(Expression s)
        : e{std::visit(
              [](auto &&s) -> StatementVariant {
                  return StatementVariant{std::move(s)};
              },
              s)} {}
    Statement(StatementVariant s)
        : e{std::move(s)} {}

    StatementVariant e;
};

struct FunctionBody {
    std::vector<Variable> variables;
    std::vector<Statement> commands;

    Variable *addVariable(Token name) {
        if (findVariable(name)) {
            return nullptr;
        }

        variables.push_back({name});
        return &variables.back();
    }

    Variable *findVariable(Token name) {
        for (auto &var : variables) {
            if (var.name == name) {
                return &var;
            }
        }

        return nullptr;
    }
};

struct Function {
    Function() = default;
    Function(const Function &) = delete;
    Function(Function &&) = delete;
    Function &operator=(const Function &) = delete;
    Function &operator=(Function &&) = delete;

    FunctionSignature signature;

    FunctionBody body;
};

inline FunctionCall::~FunctionCall() = default;

inline FunctionCall::FunctionCall()
    : args{std::make_shared<FunctionCallArgs>()} {}

inline DecoratedType FunctionCall::type() const {
    return function->signature.type;
}

inline DecoratedType BinaryExpression::type() const {
    return expressionType(*left);
}
