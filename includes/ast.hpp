#ifndef AST_HPP
#define AST_HPP

#include "token.hpp"
#include <memory>
#include <variant>

#define EXPRESSION_VARIANT \
  std::variant<Node::GenericExpr, Node::BinaryExpr, Node::UnaryExpr, Node::NumericLiteral, Node::Identifier>

#define AST_VARIANT std::variant<Node::FunctionDecl, Node::VariableDecl, Node::ConditionalStatement, Node::GenericExpr>

namespace Node
{

struct VariableDecl;
struct Identifier;
struct NumericLiteral;
struct UnaryExpr;
struct BinaryExpr;
struct GenericExpr;
struct AST;

struct AST
{
  std::unique_ptr<AST_VARIANT> ast;
};

struct GenericExpr
{
  std::unique_ptr<EXPRESSION_VARIANT> expr;
};

struct BinaryExpr
{
  ConstituentToken op;
  std::unique_ptr<GenericExpr> lhs;
  std::unique_ptr<GenericExpr> rhs;
};

struct UnaryExpr
{
  ConstituentToken op;
  std::unique_ptr<GenericExpr> expr;
};

struct NumericLiteral
{
  Token token;
  ConstituentToken type;
};

struct Identifier
{
  Token token;
  ConstituentToken type;
};

struct VariableDecl
{
  Token token; // identifier + line + col
  ConstituentToken type;
  std::unique_ptr<GenericExpr> expr;
};

struct FunctionDecl
{
  Token token;                  // identifier + line + col
  ConstituentToken return_type; // return_type
  // I think the actual parameters will be stored in the symbol table
};

struct ConditionalStatement
{
  Token token; // type of expression
  std::unique_ptr<GenericExpr> expr;
  std::unique_ptr<ConditionalStatement> next; // if - else if - else
};

} // namespace Node

#endif // AST_HPP
