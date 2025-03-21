#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include <fstream>
#include <iostream>
#include <memory>
#include <variant>

#include "ast.hpp"

class Compiler
{
  public:
  Compiler(const std::unique_ptr<Node::AST> &ast, const std::string &filename);
  void initializeLocal();
  void dispatch(const std::shared_ptr<Node::AST> &tree);

  void functionDeclaration(const std::shared_ptr<Node::FunctionDecl> &decl);
  std::string declarationParameters(
  const std::vector<std::pair<std::string, ConstituentToken>> &paramters);
  void functionBody(const std::shared_ptr<Node::AST> &body);
  inline void open();
  inline void close();
  inline void append();

  private:
  std::shared_ptr<Node::AST> ast_;
  std::string filename_;
  std::fstream fs_;
};

#endif // ASSEMBLY_HPP
