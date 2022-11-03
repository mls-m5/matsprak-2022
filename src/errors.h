#pragma once

#include "log.h"
#include "token.h"
#include <experimental/source_location>
#include <sstream>
#include <stdexcept>
#include <string>

namespace errors {
namespace detail {} // namespace detail
} // namespace errors

class ParsingError : public std::runtime_error {

    std::string createText(const Token &token,
                           const std::string &str,
                           std::experimental::source_location location) {
        auto ss = std::ostringstream{};

        if (token.file()) {
            ss << token.file()->lineAt(token.row()) << "\n";
            for (auto i = 2; i < token.col(); ++i) {
                ss << " ";
            }
            ss << "^------ here\n";
            ss << token.file()->path().string();
        }
        else {
            ss << "no file";
        }
        ss << ":" << token.row() << ":" << token.col();
        ss << ": error: \"" << token.content() << "\" " << str;

        if (logging::isVerbose) {
            ss << "\nin file: " << location.file_name() << ":"
               << location.line() << " in " << location.function_name()
               << "(...)";
        }
        return ss.str();
    }

public:
    ParsingError(const Token &token,
                 const std::string &str,
                 std::experimental::source_location location =
                     std::experimental::source_location::current())
        : runtime_error{createText(token, str, location)} {}
};

class NotImplemented : public std::runtime_error {
    std::string createText(const std::string &str,
                           std::experimental::source_location location) {
        auto ss = std::ostringstream{};

        ss << "not implemented: ";
        ss << "\nin file: " << location.file_name() << ":" << location.line()
           << " in " << location.function_name() << "(...)";
        return ss.str();
    }

public:
    NotImplemented(const std::string &str,
                   std::experimental::source_location location =
                       std::experimental::source_location::current())
        : runtime_error{createText(str, location)} {}
};

class GenError : public std::runtime_error {
    std::string createText(const std::string &str,
                           std::experimental::source_location location) {
        auto ss = std::ostringstream{};

        ss << "code generation error: ";
        ss << "\nin file: " << location.file_name() << ":" << location.line()
           << " in " << location.function_name() << "(...)";
        return ss.str();
    }

public:
    GenError(const std::string &str,
             std::experimental::source_location location =
                 std::experimental::source_location::current())
        : runtime_error{createText(str, location)} {}
};
