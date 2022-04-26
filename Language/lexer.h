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

class Lexer {
private:

    std::vector<Lexem> list_of_lexems;

public:

    void tokenize(std::string const program);
    std::vector<Lexem> get_list();
};