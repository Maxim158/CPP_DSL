#include <vector>
#include <variant>
#include <functional>
#include<iostream>

#include "Parser.h"


// ***  LANGUAGE  ***
//
//    VAR ->[A - Za - z_] + [0 - 9] *
//    DIGIT -> ([1 - 9] | 0)[0 - 9] *
//    ASSIGN_OP -> '='
//    OP ->[+| -| *| / | //]
//    COMPARE ->[> | < | >= | <= | ==]
//    LB -> '('
//    RB -> ')'
//    LCB -> '{'
//    RCB -> '}'
//    SEMICOLON -> ';'
//    OR -> 'or'
//    AND -> 'and'
//    WHILE -> 'while'
//    DO -> 'do'
//    IF -> 'if'
//
//    lang->expr +
//    expr->assign | while_do | do_while | if | print
//    assign = VAR ASSIGN_OP expr_value SEMICOLON
//    expr_value->value(OP value)*
//    value -> (VAR | DIGIT) | infinity
//    infinity->LB expr_value RB
//    condition->LC condition_expr((OR | AND) condition_expr)* RB
//    condition_expr->expr_value COMPARE expr_value
//    body->LCB expr + RCB
//    while_do->WHILE condition DO body
//    do_while->DO body WHILE condition
//    if->IF condition body(ELSE body) ?
//    print->PRINT infinity SEMICOLON


struct ParseException : public std::exception
{
    std::string why;

    ParseException(std::string why)
        : why(why)
    {}

    const char * what() const throw ()
    {
        return why.c_str();
    }
};


void Parser::parse(std::vector<Lexem> list_lex) {
    CurrentToken = list_lex.begin();
    EndToken = list_lex.end();

    std::cout << "List size: " << list_lex.size() << std::endl;

    try
    {
        while (CurrentToken != EndToken) {
            std::cout << "CYCLE!" << std::endl;
            expr();
        }
    }
    catch (ParseException&) {}
    std::cout << "DONE!" << std::endl;
    std::cout << (CurrentToken == EndToken ? "  SUCCESS!" : "FAILURE!") << std::endl;
}

void Parser::expr() {
    using parse_ft = std::pair<char const*, void(Parser::*)(void)>;
    std::vector<parse_ft> const parsers {
        {"assign", &Parser::assign},
        {"while_do", &Parser::while_do},
        {"do_while", &Parser::do_while},
        {"if", &Parser::if_},
        {"print", &Parser::print_},
        {"fail", &Parser::fail_parse}
    };

    std::cout << "trying expr()" << std::endl;

    for (parse_ft p : parsers)
    {
        std::cout << "trying " << p.first << '\n';
        try { (this->*p.second)(); std::cout << "matched " << p.first << '\n'; break; }
        catch (ParseException& e)
        {
            std::cerr << e.what() << std::endl;
            if (e.why == "failed to parse expression")
                throw;
        }
    }
}

void Parser::fail_parse() {
    throw ParseException("failed to parse expression");
}

void Parser::assign() {
    VAR_CREATE();
    VAR();
    ASSIGN_OP();
    expr_value();
    SEMICOLON();
}

void Parser::expr_value() {
     value();
     try { while (true) { OP(); value(); } }
     catch(ParseException&) {}
}

void Parser::value() {
    try { return VAR(); } catch (ParseException&) {}
    try { return DIGIT(); } catch (ParseException&) {}
    return infinity();
}

void Parser::infinity() {
    LB();
    expr_value();
    RB();
}

void Parser::condition() {                  
    LB();
    condition_expr();
    try {
        while (true) {
            try {
                OR();
                condition_expr();
            continue;
            }
            catch (ParseException&) {}
            AND();
            condition_expr();
        }
    }
    catch (ParseException&) {}
    RB();
}

void Parser::condition_expr() {
    expr_value();
    COMPARE();
    expr_value();
}

void Parser::body() {           
    LCB();
    expr();
    try {
        while (true) {
            expr();
        }
    }
    catch (ParseException&) {}
    RCB();
}

void Parser::while_do() {
    WHILE();
    condition();
    DO();
    body();
}

void Parser::do_while() {
    DO();
    body();
    WHILE();
    condition();
}

void Parser::if_() {
    IF();
    condition();
    body();
    try {
        ELSE();
        body();
    }
    catch (ParseException&) {}
}

void Parser::print_() {
    PRINT();
    infinity();
    SEMICOLON();
}


void Parser::VAR_CREATE() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "var") throw ParseException(std::string("Expected VAR but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::VAR() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Identifier) throw ParseException(std::string("Expected VAR but got ") + type_to_string(CurrentToken->Type));
    ++CurrentToken;
}

void Parser::DIGIT() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Literal) throw ParseException(std::string("Expected DIGIT but got ") + type_to_string(CurrentToken->Type));
    ++CurrentToken;
}

void Parser::ASSIGN_OP() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Assign) throw ParseException(std::string("Expected ASSIGN but got ") + type_to_string(CurrentToken->Type));
    ++CurrentToken;
}

void Parser::OP() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Operator) throw ParseException(std::string("Expected OPERATOR but got ") + type_to_string(CurrentToken->Type));
    ++CurrentToken;
}

void Parser::COMPARE() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Compare) throw ParseException(std::string("Expected COMPARE but got ") + type_to_string(CurrentToken->Type));
    ++CurrentToken;
}

void Parser::LB() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Delim) throw ParseException(std::string("Expected DELIM but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "(") throw ParseException(std::string("Expected LB but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::RB() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Delim) throw ParseException(std::string("Expected DELIM but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != ")") throw ParseException(std::string("Expected RB but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::LCB() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Delim) throw ParseException(std::string("Expected DELIM but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "{") throw ParseException(std::string("Expected LCB but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::RCB() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Delim) throw ParseException(std::string("Expected DELIM but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "}") throw ParseException(std::string("Expected RCB but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::SEMICOLON() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Delim) throw ParseException(std::string("Expected DELIM but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != ";") throw ParseException(std::string("Expected SEMICOLON but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::OR() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "or") throw ParseException(std::string("Expected OR but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::AND() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "and") throw ParseException(std::string("Expected AND but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::WHILE() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "while") throw ParseException(std::string("Expected WHILE but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::DO() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "do") throw ParseException(std::string("Expected DO but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::IF() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "if") throw ParseException(std::string("Expected IF but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::ELSE() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "else") throw ParseException(std::string("Expected ELSE but got ") + CurrentToken->contains);
    ++CurrentToken;
}

void Parser::PRINT() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "print") throw ParseException(std::string("Expected PRINT but got ") + CurrentToken->contains);
    ++CurrentToken;
}