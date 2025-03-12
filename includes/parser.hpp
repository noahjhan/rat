#ifndef PARSER_HPP
#define PARSER_HPP

#include <unordered_map>

#include "dictionary.hpp"
#include "lexer.hpp"
#include "token.hpp"

class Parser
{
  public:
  Parser(std::deque<Token> &tokens);

  private:
  std::unordered_map<std::string, ConstituentToken> dictionary_ = DICT_INIT;
  // AST ast_;
  std::deque<Token> tokens_;
};

#endif // PARSER_HPP