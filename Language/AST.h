//#pragma once
//
//#include <vector>
//#include <variant>
//#include <optional>
//
//
//// ***  LANGUAGE  ***
//
////    lang-> expr +
////    expr-> assign | while_do | do_while | if | print
////    assign = VAR ASSIGN_OP expr_value SEMICOLON
////    expr_value-> value(OP value)*
////    value -> (VAR | DIGIT) | infinity
////    infinity-> LB expr_value RB
////    condition-> LC condition_expr((OR | AND) condition_expr)* RB
////    condition_expr-> expr_value COMPARE expr_value
////    body-> LCB expr + RCB
////    while_do-> WHILE condition DO body
////    do_while-> DO body WHILE condition
////    if-> IF condition body(ELSE body) ?
////    print-> PRINT infinity SEMICOLON
//
//
//struct Context;
//
//struct AST {
//    virtual std::optional<AST> execute(Context* ctx) const;
//};
//
//struct AST_expr : public AST
//{
//    std::vector<std::variant<AST_assign, AST_while_do, AST_do_while, AST_if, AST_print>> body;
//};
//
//struct AST_body : public AST
//{
//    std::vector<AST_expr> body;
//};
//
//struct AST_var : public AST
//{
//    std::string name;
//};
//
//struct AST_val : public AST
//{
//    std::variant<double, std::string> value;
//};
//
//struct AST_expr_val : public AST
//{
//    std::vector<std::variant<AST_var, AST_val>> values;
//    std::vector<std::string> operators;
//};
//
//struct AST_assign : public AST
//{
//    AST_var target;
//    AST_expr_val expression;
//};
//
//struct AST_condition : public AST
//{
//    std::vector<std::variant<AST_var, AST_val>> values;
//    std::vector<std::string> operators;
//};
//
//struct AST_while_do : public AST
//{
//    AST_condition condition;
//    AST_body body;
//};
//
//struct AST_do_while : public AST
//{
//    AST_condition condition;
//    AST_body body;
//};
//
//struct AST_if : public AST {
//    AST_condition condition;
//    AST_body body;
//};
//
//struct AST_print : public AST {
//    AST_expr_val expression;
//};
//
//
//
