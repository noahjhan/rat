#ifndef AST_HPP
#define AST_HPP

#include "token.hpp"
#include <memory>
#include <variant>

namespace Node
{

struct BinaryExpr;
struct UnaryExpr;
struct NumericLiteral;
struct GenericExpr;

struct GenericExpr
{
  std::unique_ptr<
  std::variant<GenericExpr, BinaryExpr, UnaryExpr, NumericLiteral>>
  expr;
};

struct UnaryExpr
{
  ConstituentToken op;
  std::unique_ptr<GenericExpr> expr;
};

struct BinaryExpr
{
  ConstituentToken op;
  std::unique_ptr<GenericExpr> lhs;
  std::unique_ptr<GenericExpr> rhs;
};

struct NumericLiteral
{
  Token token;
  ConstituentToken type;
};

} // namespace Node

#endif // AST_HPP
