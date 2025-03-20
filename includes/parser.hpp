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
#include "symbol_table.hpp"
#include "token.hpp"

class Parser
{
  public:
  Parser(std::deque<Token> &tokens, RatSource &source_file);

  std::unique_ptr<Node::AST> dispatch();

  std::shared_ptr<Node::VariableDecl> variableDeclaration();
  // std::shared_ptr<Node::FunctionDecl> functionDeclaration();
  std::shared_ptr<Node::FunctionDecl> voidfunctionDeclaration();
  // std::shared_ptr<Node::FunctionDecl> optionalfunctionDeclaration();
  // std::shared_ptr<Node::FunctionDecl> lambdafunctionDeclaration();
  std::vector<std::pair<std::string, ConstituentToken>> voidParameterlist();

  std::unique_ptr<Node::ConditionalStatement> conditionalStatement();

  std::unique_ptr<Node::GenericExpr> recurseNumeric();
  std::unique_ptr<Node::GenericExpr> recurseFactor();
  std::unique_ptr<Node::GenericExpr> recurseTerm();
  std::unique_ptr<Node::GenericExpr> recurseAdditive();
  std::unique_ptr<Node::GenericExpr> recurseShift();
  std::unique_ptr<Node::GenericExpr> recurseComparison();
  std::unique_ptr<Node::GenericExpr> recurseLogical();
  std::unique_ptr<Node::GenericExpr> recurseExpr();
  std::unique_ptr<Node::GenericExpr> tokenToExpr();

  inline Token pop();
  inline Token peek();

  std::unique_ptr<Node::ReturnStatement> returnStatment();

  ConstituentToken inferTypeNumericLiteral(const std::string &value);

  int numTokens() const;

  void debugPrintln(const unsigned int &line_num);
  void debugLineCol(const unsigned int &line_num, const unsigned int &col_num);
  void debugASTPrinter(Node::AST &node);
  void debugExprPrinterRecursive(Node::GenericExpr &node, int depth);
  void debugVariableDeclPrinter(Node::VariableDecl &node);
  void debugConditionalStatement(Node::ConditionalStatement &node);
  void debugFunctionDeclaration(Node::FunctionDecl &node);

  private:
  std::unordered_map<std::string, ConstituentToken> dictionary_ = DICT_INIT;
  std::unordered_map<ConstituentToken, std::string> reverse_dictionary_ = REVERSE_DICT;
  std::deque<Token> tokens_;
  RatSource source_file_;
  SymbolTable symbol_table_;
};

#endif // PARSER_HPP
