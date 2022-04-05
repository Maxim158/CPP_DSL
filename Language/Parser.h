#pragma once
#include "lexer.h"

class Parser {
private:
    std::vector<Lexem> list;
    std::vector<Lexem>::iterator CurrentToken;
    std::vector<Lexem>::iterator EndToken;

public:

    void parse(std::vector<Lexem> list);

private:
    void expr();
    void assign();
    void expr_value();
    void value();
    void infinity();
    void condition();
    void condition_expr();
    void body();
    void while_do();
    void do_while();
    void if_();
    void print_();

    void VAR_CREATE();
    void VAR();
    void DIGIT();
    void ASSIGN_OP();
    void OP();
    void COMPARE();
    void LB();
    void RB();
    void LCB();
    void RCB();
    void SEMICOLON();
    void OR();
    void AND();
    void WHILE();
    void DO();
    void IF();
    void ELSE();
    void PRINT();

    void fail_parse();

};