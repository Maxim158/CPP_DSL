#pragma once
#include <variant>

#include "lexer.h"
#include "AST.h"

class Parser {
private:

    std::vector<Lexem> list;
    std::vector<Lexem>::iterator CurrentToken;
    std::vector<Lexem>::iterator EndToken;

public:
   AST_body program;

public:

    void parse(std::vector<Lexem> list);

private:

    AST_expr expr();
    AST_expr_t assign();
    AST_expr_val expr_value();
    std::variant<AST_var, AST_val, AST_infinity> value();
    AST_infinity infinity();
    AST_condition condition();
    AST_condition_expr condition_expr();
    AST_body body();
    AST_expr_t while_do();
    AST_expr_t do_while();
    AST_expr_t if_();
    AST_expr_t print_();
    AST_expr_t linked();
    AST_expr_t call();

    void VAR_CREATE();
    void LINKED();
    void DOG();
    int INTEGER();
    std::string call_name();
    AST_var VAR();
    AST_val DIGIT();
    AST_ll LIST();
    void ASSIGN_OP();
    std::string OP();
    std::string COMPARE();
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

    AST_expr_t fail_parse();

};