#include "semantic.hpp"

Analyzer::Analyzer(const std::shared_ptr<Node::AST> &ast) : ast_(ast)
{
}

void Analyzer::exprTypeSetter(const std::unique_ptr<Node::GenericExpr> &expr)
{
  std::shared_ptr<ConstituentToken> ptr;
  exprTypeChecker(expr, ptr);
}

void Analyzer::exprTypeChecker(const std::unique_ptr<Node::GenericExpr> &expr,
                               std::shared_ptr<ConstituentToken> &ptr)
{

  if (!expr || !expr->expr) {
    return;
  }

  if (std::holds_alternative<Node::GenericExpr>(*expr->expr)) {
    throw std::invalid_argument("expressions cannot hold expression type");
    // auto &variant = std::get<Node::GenericExpr>(*expr->expr);
  }
  else if (std::holds_alternative<Node::BinaryExpr>(*expr->expr)) {
    auto &variant = std::get<Node::BinaryExpr>(*expr->expr);
    exprTypeChecker(variant.lhs, ptr);
    exprTypeChecker(variant.rhs, ptr);
    if (*ptr == ConstituentToken::TYPE_STRING) {
      if (variant.op != ConstituentToken::ARITHMETIC_ADD &&
          variant.op != ConstituentToken::COMPARISON_EQ &&
          variant.op != ConstituentToken::COMPARISON_NEQ) {
        throw std::invalid_argument("invalid operation for a string literal");
      }
    }
    else if (*ptr == ConstituentToken::TYPE_FLOAT ||
             *ptr == ConstituentToken::TYPE_DOUBLE) {
      if (variant.op == ConstituentToken::ARITHMETIC_MOD ||
          variant.op == ConstituentToken::BITWISE_AND ||
          variant.op == ConstituentToken::BITWISE_NEG ||
          variant.op == ConstituentToken::BITWISE_OR ||
          variant.op == ConstituentToken::BITWISE_XOR ||
          variant.op == ConstituentToken::BITWISE_NEG ||
          variant.op == ConstituentToken::BITWISE_SL ||
          variant.op == ConstituentToken::BITWISE_SR) {
        throw std::invalid_argument("invalid operation for floating type value");
      }
    }
    return;
  }
  else if (std::holds_alternative<Node::UnaryExpr>(*expr->expr)) {
    auto &variant = std::get<Node::UnaryExpr>(*expr->expr);
    exprTypeChecker(variant.expr, ptr);
    if (*ptr == ConstituentToken::TYPE_STRING) {
      throw std::invalid_argument("invalid operation for string literal @todo []");
    }
    else if (*ptr == ConstituentToken::TYPE_FLOAT ||
             *ptr == ConstituentToken::TYPE_DOUBLE) {
      if (variant.op != ConstituentToken::ARITHMETIC_ADD &&
          variant.op != ConstituentToken::ARITHMETIC_SUB &&
          variant.op != ConstituentToken::LOGICAL_NOT) {
        throw std::invalid_argument("invalid operation on a floating type value");
      }
    }
    return;
  }
  else if (std::holds_alternative<Node::NumericLiteral>(*expr->expr)) {
    auto &variant = std::get<Node::NumericLiteral>(*expr->expr);
    setTypeOrThrow(ptr, variant.type);
    return;
  }
  else if (std::holds_alternative<Node::StringLiteral>(*expr->expr)) {
    auto &variant = std::get<Node::StringLiteral>(*expr->expr);
    auto next = ConstituentToken::TYPE_STRING;
    setTypeOrThrow(ptr, next);
    return;
  }
  else if (std::holds_alternative<Node::Identifier>(*expr->expr)) {
    auto &variant = std::get<Node::Identifier>(*expr->expr);
    setTypeOrThrow(ptr, variant.type);
    return;
  }
  else if (std::holds_alternative<Node::FunctionCall>(*expr->expr)) {
    auto &variant = std::get<Node::FunctionCall>(*expr->expr);
    if (!variant.function) {
      throw std::invalid_argument("null function");
    }
    setTypeOrThrow(ptr, variant.function->return_type);
    return;
  }
  else {
    throw std::invalid_argument("error: expression holds invalid type");
  }
}

ConstituentToken Analyzer::inferTypeNumericLiteral(const std::string &value)
{
  bool is_u_type = value.find('u') != std::string::npos;
  bool is_f_type = value.find('f') != std::string::npos ||
                   value.find('d') != std::string::npos ||
                   value.find('.') != std::string::npos;

  // this is already requireed in lexer.cpp
  if (is_u_type && is_f_type) {
    throw std::invalid_argument("ambiguous numeric literal: '" + value + "'");
  }

  // type inference default int
  if (std ::isdigit(value.back())) {
    return is_f_type ? ConstituentToken::TYPE_DOUBLE : ConstituentToken::TYPE_INT;
  }

  switch (value.back()) {
    case 'u':
      return ConstituentToken::TYPE_UINT;
    case 'i':
      return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    case 'l':
      return is_u_type ? ConstituentToken::TYPE_ULONG : ConstituentToken::TYPE_LONG;
    case 's':
      return is_u_type ? ConstituentToken::TYPE_USHORT : ConstituentToken::TYPE_SHORT;
    case 'c':
      return is_u_type ? ConstituentToken::TYPE_UCHAR : ConstituentToken::TYPE_CHAR;
    case 'f':
      return ConstituentToken::TYPE_FLOAT;
    case 'd':
      return ConstituentToken::TYPE_DOUBLE;
    case '\0':
      return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    default:
      std::cerr << value << std::endl;
      throw std::invalid_argument("ambiguous numeric literal token");
  }
}

void setTypeOrThrow(std::shared_ptr<ConstituentToken> &ptr, const ConstituentToken &next)
{
  if (!ptr) {
    *ptr = next;
  }
  else if (*ptr != next) {
    throw std::invalid_argument("error: expression cannot contain multiple types");
  }
}