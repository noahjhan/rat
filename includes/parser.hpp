#ifndef PARSER_HPP
#define PARSER_HPP

#include <unordered_map>

#include "ast.hpp"
#include "dictionary.hpp"
#include "lexer.hpp"
#include "token.hpp"

class Parser
{
  public:
  Parser(std::deque<Token> &tokens);

  std::unique_ptr<Node::GenericExpr> tokenToExpr();

  ConstituentToken inferTypeNumericLiteral(const std::string &value);

  int numTokens() const;

  private:
  std::unordered_map<std::string, ConstituentToken> dictionary_ = DICT_INIT;
  std::deque<Token> tokens_;
};

#endif // PARSER_HPP
