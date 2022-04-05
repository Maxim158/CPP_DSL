#pragma once

#include "Tokens.h"


struct Lexem
{
    TokenType Type;
    std::string contains;

    Lexem(TokenType tokenType_, const std::string& tokenValue_);

    Lexem(const Lexem& token) = default;
    void debug_print();
};


std::vector<Lexem> tokenizer(std::string const program);
