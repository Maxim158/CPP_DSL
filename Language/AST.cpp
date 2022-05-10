#include <iostream>
#include <string>

#include "AST.h"


std::optional<AST_val> AST::execute(struct Context* ctx) const
{
    "Not implemented!";
    return std::nullopt;
}

std::optional<AST_val> AST_var::execute(Context * ctx) const
{
    return ctx->get(this->name);
}

AST_val Context::get(std::string const & name) const
{
    return this->variables.at(name);
}

std::optional<AST_val> AST_val::execute(Context* ctx) const
{
    return *this;
}

namespace
{
    static std::optional<AST_val> execute_variant(Context* ctx, AST_expr_val::value_type val)
    {
        if (std::holds_alternative<AST_var>(val)) return std::get<AST_var>(val).execute(ctx);
        if (std::holds_alternative<AST_val>(val)) return std::get<AST_val>(val).execute(ctx);
        if (std::holds_alternative<AST_infinity>(val)) return std::get<AST_infinity>(val).execute(ctx);
        throw "NotImplemented";
    }

    static std::optional<AST_val> execute_operator(std::string const& op, std::optional<AST_val> lval, std::optional<AST_val> rval)
    {
        if (!lval.has_value() || !rval.has_value()) return std::nullopt;
        if (lval.value().value.index() != rval.value().value.index()) throw "Types do not match";
        if (std::holds_alternative<double>(lval.value().value))
        {
            AST_val res;
            if (op == "+") res.value = std::get<double>(lval.value().value) + std::get<double>(rval.value().value);
            else if (op == "-") res.value = std::get<double>(lval.value().value) - std::get<double>(rval.value().value);
            else if (op == "*") res.value = std::get<double>(lval.value().value) * std::get<double>(rval.value().value);
            else if (op == "/") res.value = std::get<double>(lval.value().value) / std::get<double>(rval.value().value);
            else if (op == ">") res.value = std::get<double>(lval.value().value) > std::get<double>(rval.value().value);
            else if (op == "<") res.value = std::get<double>(lval.value().value) < std::get<double>(rval.value().value);
            else if (op == "==") res.value = std::get<double>(lval.value().value) == std::get<double>(rval.value().value);
            else if (op == "<>") res.value = std::get<double>(lval.value().value) != std::get<double>(rval.value().value);
            else return std::nullopt;
            return res;
        }

        return std::nullopt;
    }
}

std::optional<AST_val> AST_expr_val::execute(Context* ctx) const
{
    auto result = execute_variant(ctx, this->values.at(0));

    for (std::size_t i = 0; i < this->operators.size(); ++i)
    {
        auto rval = execute_variant(ctx, this->values.at(i + 1));
        result = execute_operator(this->operators[i], result, rval);
    }

    return result;
}

std::optional<AST_val> AST_infinity::execute(Context * ctx) const
{
    return this->expression.execute(ctx);
}

std::optional<AST_val> AST_assign::execute(Context * ctx) const
{
    ctx->variables[this->target.name] = this->expression.execute(ctx).value();
    return std::nullopt;
}

std::optional<AST_val> AST_condition_expr::execute(Context * ctx) const
{
    return execute_operator(this->compare, this->values.at(0).execute(ctx), this->values.at(1).execute(ctx));
}

std::optional<AST_val> AST_condition::execute(Context * ctx) const
{
    return this->values.at(0).execute(ctx);
}

namespace
{
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
}

std::optional<AST_val> AST_expr::execute(Context * ctx) const
{
    for (auto e : this->body)
        std::visit(overloaded {[&](auto &v) { v.execute(ctx); }}, e);
    return std::nullopt;
}

std::optional<AST_val> AST_body::execute(Context * ctx) const
{
    for (auto e : this->body)
        e.execute(ctx);
    return std::nullopt;
}

std::optional<AST_val> AST_while_do::execute(Context * ctx) const
{
    return std::optional<AST_val>();
}

std::optional<AST_val> AST_do_while::execute(Context * ctx) const
{
    return std::optional<AST_val>();
}

std::optional<AST_val> AST_if::execute(Context * ctx) const
{
    auto cond = this->condition.execute(ctx);
    if (cond.has_value() && std::holds_alternative<double>(cond.value().value) && std::get<double>(cond.value().value) != 0.0)
        return this->body_if.execute(ctx);
    else
        return this->body_else.execute(ctx);
}

std::optional<AST_val> AST_print::execute(Context * ctx) const
{
    auto val = this->expression.execute(ctx);
    if (!val.has_value()) std::cout << "nil";
    else if (std::holds_alternative<double>(val.value().value)) std::cout << std::get<double>(val.value().value);
    else if (std::holds_alternative<std::string>(val.value().value)) std::cout << std::get<std::string>(val.value().value);

    std::cout << std::endl;
    return std::nullopt;
}
