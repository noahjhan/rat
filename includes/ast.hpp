#ifndef AST_HPP
#define AST_HPP

#include "token.hpp"
#include <memory>
#include <variant>

#define EXPRESSION_VARIANT                                           \
  std::variant<Node::GenericExpr, Node::BinaryExpr, Node::UnaryExpr, \
               Node::NumericLiteral, Node::Identifier>

namespace Node
{

struct Identifier;
struct NumericLiteral;
struct UnaryExpr;
struct BinaryExpr;
struct GenericExpr;

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

} // namespace Node

#endif // AST_HPP
