#include <string>
#include <unordered_set>
#include <regex>

enum class TokenType
{
    Identifier,
    Keyword,
    Literal,
    Operator,
    Assign,
    Compare,
    Delim,
    Whitespace,
    Unidentified
};

static const std::regex keywords("(var|for|while|print|linked|func|if|else|or|and|do|linked)\\b");
static const std::regex operators("\\+|\\-|\\*|\\/|\\@");
static const std::regex compare("==|<>|>|<");
static const std::regex assign("=");
static const std::regex delims("\\(|\\)|\\{|\\}|;");
static const std::regex identifier("[a-zA-Z]+[a-zA-Z0_]*[0-9]*");
static const std::regex literal("[1-9]+[0-9]*|'(.*)'");
static const std::regex whitespace("\\s");


static std::string type_to_string(TokenType type) {
    switch (type)
    {
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::Keyword:
        return "Keyword";
    case TokenType::Literal:
        return "Literal";
    case TokenType::Operator:
        return "Operator";
    case TokenType::Delim:
        return "Delim";
    case TokenType::Unidentified:
        return "Unidentified";
    case TokenType::Whitespace:
        return "Whitespace";
    case TokenType::Assign:
        return "Assign";
    case TokenType::Compare:
        return "Compare";
    default:
        throw std::runtime_error("unreachable");
    }
}