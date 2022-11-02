#pragma once

#include "errors.h"
#include "state.h"
#include "token.h"

// Use type Token::Any to match anything
void skipGroup(State &s, Token::Type type) {
    Token::Type begin = s.token().type();
    Token::Type end = Token::Eof;

    if (s.token().type() != Token::Any) {
        s.token().expect(type);
    }

    switch (begin) {
    case Token::BeginBrace:
        end = Token::EndBrace;
        break;
    case Token::BeginParen:
        end = Token::EndParen;
        break;
    case Token::BeginBracket:
        end = Token::EndBracket;
        break;
    default:
        throw ParsingError{s.token(),
                           "expected group but got " +
                               std::string{s.token().content()}};
    }

    int depth = 1;

    s.next();

    for (const Token *t = &s.token(); depth; t = &s.next()) {
        if (t->type() == begin) {
            ++depth;
        }
        else if (t->type() == end) {
            --depth;
        }
    }
}
