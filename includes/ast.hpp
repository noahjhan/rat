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
struct Punctuator;
struct Operator;
struct GenericExpr;

struct GenericExpr
{
  std::unique_ptr<std::variant<GenericExpr, BinaryExpr, UnaryExpr, NumericLiteral, Punctuator, Operator>> expr;
};

struct UnaryExpr
{
  std::unique_ptr<Operator> op;
  std::unique_ptr<GenericExpr> expr;
};

struct BinaryExpr
{
  std::unique_ptr<Operator> op;
  std::unique_ptr<GenericExpr> lhs;
  std::unique_ptr<GenericExpr> rhs;
};

struct NumericLiteral
{
  Token token;
  ConstituentToken type;
};

struct Punctuator
{
  Token token;
  ConstituentToken type;
};

struct Operator
{
  Token token;
  ConstituentToken type;
};

} // namespace Node

#endif // AST_HPP
