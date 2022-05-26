#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <map>


#include "LinkedList.h"

// ***  LANGUAGE  ***

//    lang-> expr +
//    expr-> assign | while_do | do_while | if | print | linked
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

// linked -> LINKED VAR SEMICOLON

struct Context
{
    std::map<std::string, struct AST_val, struct AST_ll> variables;

public:
    struct AST_val get(std::string const& name) const;
};

struct AST {
    virtual std::optional<AST_val> execute(struct Context* ctx) const;
};


struct AST_var : public AST
{
    std::string name;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_ll : public AST
{
    std::string name;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_val : public AST
{
    std::variant<double, std::string> value;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};


struct AST_expr_val : public AST
{
    using value_type = std::variant<AST_var, AST_val, struct AST_infinity>;
    std::vector<value_type> values;
    std::vector<std::string> operators;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_infinity : public AST {
    AST_expr_val expression;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_assign : public AST
{
    AST_var target;
    AST_expr_val expression;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_condition_expr : public AST {
    std::vector<AST_expr_val> values;
    std::string compare;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_condition : public AST
{
    std::vector<AST_condition_expr> values;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};




struct AST_while_do;
struct AST_do_while;
struct AST_if;
struct AST_print;
struct AST_fail;

using AST_expr_t = std::variant<AST_assign, AST_while_do, AST_do_while, AST_if, AST_print, AST_ll, AST_fail>;

struct AST_expr : public AST
{
    std::vector<AST_expr_t> body;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_body : public AST
{
    std::vector<AST_expr> body;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_while_do : public AST
{
    AST_condition condition;
    AST_body body;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_do_while : public AST
{
    AST_condition condition;
    AST_body body;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_if : public AST {
    AST_condition condition;
    AST_body body_if;
    AST_body body_else;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_print : public AST {
    AST_infinity expression;
    std::optional<AST_val> execute(struct Context* ctx) const override;
};

struct AST_fail : public AST {
};

