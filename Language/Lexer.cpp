#include <iostream>
#include <cstdio>
#include <vector>
#include <string_view>
#include <string>
#include <regex>

#include "lexer.h"


Lexem get_lexem(std::string const& part, std::size_t& index)
{
    std::smatch match;
    if (std::regex_search(part.cbegin() + index, part.cend(), match, keywords, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Keyword, match[0]);
    }
    else if (std::regex_search(part.cbegin() + index, part.cend(), match, assign, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Assign, match[0]);
    }
    else if (std::regex_search(part.cbegin() + index, part.cend(), match, compare, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Compare, match[0]);
    }
    else if (std::regex_search(part.cbegin() + index, part.cend(), match, operators, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Operator, match[0]);
    }
    else if (std::regex_search(part.cbegin() + index, part.cend(), match, delims, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Delim, match[0]);
    }
    else if (std::regex_search(part.cbegin() + index, part.cend(), match, identifier, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Identifier, match[0]);
    }
    else if (std::regex_search(part.cbegin() + index, part.cend(), match, literal, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Literal, match[0]);
    }
    else if (std::regex_search(part.cbegin() + index, part.cend(), match, whitespace, std::regex_constants::match_continuous))
    {
        index += match[0].length();
        return Lexem(TokenType::Whitespace, " ");
    }

    std::string data = std::string({part[index++]});
    return Lexem(TokenType::Unidentified, data);
}

void Lexer::tokenize(std::string const program) {
    std::string word = "";
    std::size_t index = 0;
    while (index < program.size()) {
        Lexem l = get_lexem(program, index);
        if (l.Type == TokenType::Whitespace) continue;
        list_of_lexems.push_back(l);
    }
}

std::vector<Lexem> Lexer::get_list() {
    return list_of_lexems;
}

Lexem::Lexem(TokenType tokenType_, const std::string & tokenValue_) : contains(tokenValue_), Type(tokenType_) { /* empty */ }

void Lexem::debug_print() {
    std::cout << type_to_string(this->Type) << " : " << this->contains << std::endl;
}

