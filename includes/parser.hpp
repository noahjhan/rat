#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <regex>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "dictionary.hpp"
#include "lexer.hpp"
#include "token.hpp"

class Parser
{
  public:
  Parser(std::deque<Token> &tokens);

  void dispatch();

  std::unique_ptr<Node::GenericExpr> recurseNumeric();
  std::unique_ptr<Node::GenericExpr> recurseFactor();
  std::unique_ptr<Node::GenericExpr> recurseTerm();
  std::unique_ptr<Node::GenericExpr> recurseExpr();


  std::unique_ptr<Node::GenericExpr> tokenToExpr();

  ConstituentToken inferTypeNumericLiteral(const std::string &value);

  int numTokens() const;

  void debugASTPrinter(std::vector<Node::GenericExpr> &vect);
  void debugASTPrinterRecursive(const Node::GenericExpr &node, int depth);

  private:
  std::unordered_map<std::string, ConstituentToken> dictionary_ = DICT_INIT;
  std::deque<Token> tokens_;
};

#endif // PARSER_HPP
