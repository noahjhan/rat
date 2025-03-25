#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <regex>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "dictionary.hpp"
#include "lexer.hpp"
#include "symbol_table.hpp"
#include "token.hpp"

// clang-format off
class Parser
{
  public:
  Parser(std::deque<Token> &tokens, RatSource &source_file);

  std::shared_ptr<Node::AST> dispatch();

  std::shared_ptr<Node::VariableDecl> variableDeclaration();
  std::vector<std::pair<std::string, ConstituentToken>> parameterlist();
  std::shared_ptr<Node::FunctionDecl> functionDeclaration();
  std::shared_ptr<Node::FunctionDecl> voidFunctionDeclaration();
  // std::shared_ptr<Node::FunctionDecl> optionalfunctionDeclaration();
  // std::shared_ptr<Node::FunctionDecl> lambdafunctionDeclaration();
  std::shared_ptr<Node::ReturnStatement> returnStatment();
  std::vector<std::shared_ptr<Node::GenericExpr>> callParameters();
  std::shared_ptr<Node::GenericExpr> functionCall();
  
  std::shared_ptr<Node::ConditionalStatement> conditionalStatement();
  
  std::shared_ptr<Node::GenericExpr> recurseNumeric();
  std::shared_ptr<Node::GenericExpr> recurseFactor();
  std::shared_ptr<Node::GenericExpr> recurseTerm();
  std::shared_ptr<Node::GenericExpr> recurseAdditive();
  std::shared_ptr<Node::GenericExpr> recurseShift();
  std::shared_ptr<Node::GenericExpr> recurseComparison();
  std::shared_ptr<Node::GenericExpr> recurseLogical();
  std::shared_ptr<Node::GenericExpr> recurseExpr();
  std::shared_ptr<Node::GenericExpr> tokenToExpr();
  ConstituentToken inferTypeNumericLiteral(const std::string &value);
  
  void debugASTPrinter(Node::AST &node);
  void debugExprPrinterRecursive(Node::GenericExpr &node, int depth);
  void debugVariableDeclPrinter(Node::VariableDecl &node);
  void debugConditionalStatement(Node::ConditionalStatement &node);
  void debugFunctionDeclaration(Node::FunctionDecl &node);
  void debugReturnStatement(Node::ReturnStatement &node);
  void debugPrintln(const unsigned int &line_num);
  void debugLineCol(const unsigned int &line_num, const unsigned int &col_num);
  
  inline Token pop();
  inline Token peek();
  inline void restore(const Token &token);
  int numTokens() const;
  
  private:
  std::unordered_map<std::string, ConstituentToken> dictionary_ = DICT;
  std::unordered_map<ConstituentToken, std::string> reverse_dictionary_ = REVERSE_DICT;
  std::unordered_map<std::string, ConstituentToken> function_dictionary_;
  std::deque<Token> tokens_;
  RatSource source_file_;
  SymbolTable symbol_table_;
  std::stack<ConstituentToken> local_return_type_;
};

#endif // PARSER_HPP
