#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <cassert>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

#define SYMBOL_VARIANT std::variant<Node::VariableDecl, Node::FunctionDecl>

class SymbolTable
{
  public:
  SymbolTable();
  void enterScope();
  void exitScope();

  std::shared_ptr<Node::FunctionDecl> lookupFunction(const std::string &identifier);
  std::shared_ptr<Node::VariableDecl> lookupVariable(const std::string &identifier);

  bool findVariable(const std::string &identifier);
  bool findFunction(const std::string &identifier);

  void addFunction(const std::string &identifier, const std::shared_ptr<Node::FunctionDecl> &declaration);
  void addVariable(const std::string &identifier, const std::shared_ptr<Node::VariableDecl> &declaration);

  void debugSize();

  private:
  std::vector<std::shared_ptr<SYMBOL_VARIANT>> stack_;
  std::unordered_map<std::string, std::shared_ptr<Node::FunctionDecl>> function_table_;
  std::unordered_map<std::string, std::shared_ptr<Node::VariableDecl>> variable_table_;
};

#endif // SYMBOL_TABLE_HPP
