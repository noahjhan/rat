#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "dictionary.hpp"
#include "token.hpp"
// clang-format off
class Compiler
{
  public:
  Compiler(const std::unique_ptr<Node::AST> &ast, const std::string &filename);
  void initializeLocal();
  void dispatch(const std::shared_ptr<Node::AST> &tree);

  void functionDeclaration(const std::shared_ptr<Node::FunctionDecl> &decl);
  std::string declarationParameters(const std::vector<std::pair<std::string, ConstituentToken>> &paramters);
  void functionBody(const std::shared_ptr<Node::AST> &body);
  void functionCall(const std::unique_ptr<Node::FunctionCall> &call);
  void returnStatement(const ::std::unique_ptr<Node::ReturnStatement> &return_statement);

  void expression(const std::unique_ptr<Node::GenericExpr> &expr);

  void stringGlobal(const std::string &str);

  void variableDeclaration(const std::unique_ptr<Node::VariableDecl> &decl);

  inline void open();
  inline void close();
  inline void append();
  inline void readWrite();
  inline void read();
  inline void trunc();

  private:
  std::shared_ptr<Node::AST> ast_;
  std::string filename_;
  std::fstream fs_;
  std::unordered_map<std::string, std::pair<std::string, std::string>>
  scoped_registers_; // identifier -> "%reg_num, type"
  std::unordered_map<std::string, std::string>
  function_table_; // identifier -> "return_type @identifier"
  unsigned num_string_constants_ = 1;
  unsigned num_registers_ = 1;

  std::stringstream appendable_buffer_;
};

#endif // ASSEMBLY_HPP
