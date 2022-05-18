#include <vector>
#include <variant>
#include <functional>
#include<iostream>

#include "Parser.h"
#include "AST.h"
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
//    lang-> expr +
//    expr-> assign | while_do | do_while | if | print
//    assign = VAR ASSIGN_OP expr_value SEMICOLON
//    expr_value-> value(OP value)*
//    value -> (VAR | DIGIT) | infinity
//    infinity-> LB expr_value RB
//    condition-> LC condition_expr((OR | AND) condition_expr)* RB
//    condition_expr-> expr_value COMPARE expr_value
//    body-> LCB expr + RCB
//    while_do-> WHILE condition DO body
//    do_while-> DO body WHILE condition
//    if-> IF condition body(ELSE body) ?
//    print-> PRINT infinity SEMICOLON


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
    
    //std::cout << "List size: " << list_lex.size() << std::endl;

    try
    {
        while (CurrentToken != EndToken) {
            //std::cout << "CYCLE!" << std::endl;
            program.body.push_back(expr());
        }
    }
    catch (ParseException&) {}
    //std::cout << "DONE!" << std::endl;
    //std::cout << (CurrentToken == EndToken ? "  SUCCESS!" : "FAILURE!") << std::endl;
}

AST_expr Parser::expr() {
    using parse_ft = std::pair<char const*, AST_expr_t(Parser::*)(void)>;
    std::vector<parse_ft> const parsers {
        {"assign", &Parser::assign},
        {"while_do", &Parser::while_do},
        {"do_while", &Parser::do_while},
        {"if", &Parser::if_},
        {"print", &Parser::print_},
        {"fail", &Parser::fail_parse}
    };

    //std::cout << "trying expr()" << std::endl;
    AST_expr ex;

    for (parse_ft p : parsers)
    {
        //std::cout << "trying " << p.first << '\n';
        try {  ex.body.push_back((this->*p.second)()); /*std::cout << "matched " << p.first << '\n';*/ break; }
        catch (ParseException& e)
        {
            //std::cerr << e.what() << std::endl;
            if (e.why == "failed to parse expression")
                throw;
        }
    }

    return ex;
}

AST_expr_t Parser::fail_parse() {
    AST_fail fail;
    throw ParseException("failed to parse expression");
    return fail;
}

AST_expr_t Parser::assign() {
    AST_assign as{};
    VAR_CREATE();
    as.target = VAR();
    ASSIGN_OP();
    as.expression = expr_value();
    SEMICOLON();
    return as;
}

AST_expr_val Parser::expr_value() {
    AST_expr_val ex_val;
    ex_val.values.push_back(value());
    try { while (true) { ex_val.operators.push_back(OP()); ex_val.values.push_back(value()); } }
    catch(ParseException&) {}
    return ex_val;
}

std::variant<AST_var, AST_val, AST_infinity, AST_call> Parser::value() {
    try { return VAR(); } catch (ParseException&) {}
    try { return DIGIT(); } catch (ParseException&) {}
    try { return LIST(); } catch (ParseException&) {}
    try { return CALL(); } catch (ParseException&) {}
    return infinity();
}

AST_infinity Parser::infinity() {
    AST_infinity inf;
    LB();
    inf.expression = expr_value();
    RB();
    return inf;
}

AST_condition Parser::condition() {
    AST_condition cond;
    LB();
    cond.values.push_back(condition_expr());
    try {
        while (true) {
            try {
                OR();
                cond.values.push_back(condition_expr());
            continue;
            }
            catch (ParseException&) {}
            AND();
            cond.values.push_back(condition_expr());
        }
    }
    catch (ParseException&) {}
    RB();
    return cond;
}

AST_condition_expr Parser::condition_expr() {
    AST_condition_expr cond_exp;
    cond_exp.values.push_back(expr_value());
    cond_exp.compare = COMPARE();
    cond_exp.values.push_back(expr_value());
    return cond_exp;
}

AST_body Parser::body() {
    AST_body bd;
    LCB();
    bd.body.push_back(expr());
    try {
        while (true) {
            bd.body.push_back(expr());
        }
    }
    catch (ParseException&) {}
    RCB();
    return bd;
}

AST_expr_t Parser::while_do() {
    AST_while_do wd;
    WHILE();
    wd.condition = condition();
    DO();
    wd.body = body();
    return wd;
}

AST_expr_t Parser::do_while() {
    AST_do_while dw;
    DO();
    dw.body = body();
    WHILE();
    dw.condition = condition();
    return dw;
}

AST_expr_t Parser::if_() {
    AST_if iff;
    IF();
    iff.condition = condition();
    iff.body_if = body();
    try {
        ELSE();
        iff.body_else = body();
    }
    catch (ParseException&) {}
    return iff;
}

AST_expr_t Parser::print_() {
    AST_print pr;
    PRINT();
    pr.expression = infinity();
    SEMICOLON();
    return pr;
}


void Parser::VAR_CREATE() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Keyword) throw ParseException(std::string("Expected KEYWORD but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains != "var") throw ParseException(std::string("Expected VAR but got ") + CurrentToken->contains);
    ++CurrentToken;
}

AST_var Parser::VAR() {
    AST_var vr;
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Identifier) throw ParseException(std::string("Expected VAR but got ") + type_to_string(CurrentToken->Type));
    vr.name = CurrentToken->contains;
    ++CurrentToken;
    return vr;
}

AST_val Parser::DIGIT() {
    AST_val val;
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Literal) throw ParseException(std::string("Expected DIGIT but got ") + type_to_string(CurrentToken->Type));
    if (CurrentToken->contains[0] != '\'')
        val.value = strtod(CurrentToken->contains.c_str(), nullptr);
    else val.value = CurrentToken->contains.substr(1, CurrentToken->contains.size() - 2);
    ++CurrentToken;
    return val;
}

AST_call Parser::CALL() {
    AST_call cl;
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Delim || CurrentToken->contains != "@") throw ParseException(std::string("Expected @ but got ") + type_to_string(CurrentToken->Type));
    return cl;
}

AST_val Parser::LIST() {
    AST_val lst;
    lst.value = LinkedList();
    if (CurrentToken->Type != TokenType::Delim || CurrentToken->contains != "[") throw ParseException("expected list opening bracket");
    ++CurrentToken;

    while (CurrentToken->Type != TokenType::Delim || CurrentToken->contains != "]")
    {
        auto& l = std::get<std::shared_ptr<LinkedList const>>(lst.value);
        l->append(l, expr_value());
        if (CurrentToken->Type == TokenType::Delim && CurrentToken->contains == ",") ++CurrentToken;
    }

    if (CurrentToken->Type != TokenType::Delim || CurrentToken->contains != "]") throw ParseException("expected list opening bracket");
    ++CurrentToken;
    return lst;
}

void Parser::ASSIGN_OP() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Assign) throw ParseException(std::string("Expected ASSIGN but got ") + type_to_string(CurrentToken->Type));
    ++CurrentToken;
}

std::string Parser::OP() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Operator) throw ParseException(std::string("Expected OPERATOR but got ") + type_to_string(CurrentToken->Type));
    return CurrentToken++->contains;
}

std::string Parser::COMPARE() {
    if (CurrentToken == EndToken) throw ParseException("Unexpected end of file");
    if (CurrentToken->Type != TokenType::Compare) throw ParseException(std::string("Expected COMPARE but got ") + type_to_string(CurrentToken->Type));
    return CurrentToken++->contains;
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