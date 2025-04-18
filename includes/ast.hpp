#ifndef AST_HPP
#define AST_HPP

#include "token.hpp"
#include <memory>
#include <variant>
#include <vector>

#define EXPRESSION_VARIANT                                                    \
    std::variant<Node::GenericExpr, Node::BinaryExpr, Node::UnaryExpr,        \
                 Node::NumericLiteral, Node::StringLiteral, Node::Identifier, \
                 Node::FunctionCall>

#define AST_VARIANT                                                                  \
    std::variant<Node::FunctionDecl, Node::VariableDecl, Node::ConditionalStatement, \
                 Node::ReturnStatement, Node::GenericExpr>

// clang-format off
namespace Node
{

struct FunctionCall;
struct ReturnStatement;
struct ConditionalStatement;
struct FunctionDecl;
struct VariableDecl;
struct Identifier;
struct NumericLiteral;
struct StringLiteral;
struct UnaryExpr;
struct BinaryExpr;
struct GenericExpr;
struct AST;

struct AST
{
  std::shared_ptr<AST_VARIANT> curr;
  std::shared_ptr<AST> next;
};

struct GenericExpr
{
  std::shared_ptr<EXPRESSION_VARIANT> expr;
};

struct BinaryExpr
{
  ConstituentToken op;
  std::shared_ptr<GenericExpr> lhs;
  std::shared_ptr<GenericExpr> rhs;
};

struct UnaryExpr
{
  ConstituentToken op;
  std::shared_ptr<GenericExpr> expr;
};

struct NumericLiteral
{
  Token token; // value + line + col
  ConstituentToken type;
};

struct StringLiteral
{
  Token token; // value + line + col
  ConstituentToken type;
};

struct Identifier
{
  Token token; // identifier + line + col
  ConstituentToken type;
};

struct VariableDecl
{
  Token token; // identifier + line + col
  ConstituentToken type;
  std::shared_ptr<GenericExpr> expr;
};

struct FunctionDecl
{
  ConstituentToken type;
  Token token; // identifier + line + col
  std::vector<std::pair<std::string, ConstituentToken>> parameters;
  ConstituentToken return_type; // return_type
  std::shared_ptr<AST> body;
  // parameters will be stored in the symbol table
};

struct ConditionalStatement
{
  Token token; // type of expression
  std::shared_ptr<GenericExpr> expr;
  std::shared_ptr<ConditionalStatement> next; // if - else if - else
  std::shared_ptr<AST> body;
};

struct ReturnStatement
{
  Token token;
  ConstituentToken type;
  std::shared_ptr<GenericExpr> expr;
};

struct FunctionCall
{
  Token token; // identifier + line + col
  ConstituentToken type;
  std::vector<std::shared_ptr<GenericExpr>> parameters;
  // std::shared_ptr<FunctionDecl> function; // cannot store due to recursion 
};

} // namespace Node

#endif // AST_HPP
