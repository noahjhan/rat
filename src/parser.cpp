#include "parser.hpp"

/**
 * @brief
 * constructor for parser should validate tokens generated by lexer.
 * Although, lexer should handle most of the errors associated with unrecognized
 * tokens. Realistically, parse errors should occur elsewhere when the provided
 * deque does not match language grammar.
 *
 * skip literals becuase they are
 * validated in lexer in advanceLiteral()
 *
 * skip identifiers because they are
 * validated in lexer in advanceToken()
 *
 * tl;dr
 * this constructor may be redundant / for debug purposes
 *
 * @todo copy over the debug functions from lexer, maybe make a generic debug.cpp file
 */

int add(int a, int b) { return a + b; }

Parser::Parser(std::deque<Token> &tokens) : tokens_(tokens)
{

  for (const auto &token : tokens_)
  {
    if (token.value.empty())
    {
      throw std::invalid_argument("syntax error: empty token");
    }
    switch (token.type)
    {
      case GenericToken::IDENTIFIER: break;
      case GenericToken::KEYWORD:
        if (dictionary_.find(token.value) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized keyword");
        }
        break;
      case GenericToken::NUMERIC_LITERAL: break;
      case GenericToken::STRING_LITERAL: break;
      case GenericToken::CHAR_LITERAL: break;
      case GenericToken::PUNCTUATOR:
        if (dictionary_.find(token.value) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized punctuator");
        }
        break;
      case GenericToken::OPERATOR:
        if (dictionary_.find(token.value) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized operator");
        }
        break;
      case GenericToken::TYPE:
        if (dictionary_.find(token.value) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized type");
        }
        break;
    }
  }
}
/// @todo
///
/// the point of this dispatch will be to pop a token
/// peek the next token and use only that information
/// to determine how to interpret them into AST
void Parser::dispatch()
{
  if (tokens_.empty())
  {
    throw std::invalid_argument("empty deque");
  }
  Token curr = tokens_.front();
  tokens_.pop_front();
  Token next = tokens_.front();
  switch (curr.type)
  {

    {
      if (dictionary_.find(curr.value) == dictionary_.end())
      {
        std::cerr << "recieved '" << curr.value << '\'' << std::endl;
        throw std::invalid_argument("unrecognized keyword");
      }
      ConstituentToken keyword = dictionary_.at(curr.value);
    }
  }
}

/// @todo recursive descent
/// @return
//
std::unique_ptr<Node::GenericExpr> Parser::tokenToExpr()
{
  // lets assume for now the deque only contains valid expression tokens
  // lets assume for now the deque only contains puctuators, literals, operators
  // I guess currently this takes in a deque and pops a token, and processes it as if it is part of an expression
  //
  //
  Token token = tokens_.front();
  if (tokens_.empty())
  {
    throw std::runtime_error("error: empty token deque");
  }
  if (0)
  {
    throw std::invalid_argument("error: non expression token");
  }

  std::unique_ptr<Node::GenericExpr> gen_expr_ptr;
  tokens_.pop_front(); // maybe here maybe at the end

  switch (token.type)
  {
    case GenericToken::IDENTIFIER: throw std::runtime_error("identifier : @todo");
    case GenericToken::KEYWORD: throw std::invalid_argument("error: keyword found in expression");
    case GenericToken::NUMERIC_LITERAL:
    {
      Node::NumericLiteral node;
      node.token = token;
      node.type = inferTypeNumericLiteral(token.value);
      auto ptr = std::make_unique<std::variant<Node::GenericExpr, Node::BinaryExpr, Node::UnaryExpr,
                                               Node::NumericLiteral, Node::Punctuator, Node::Operator>>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    }
    break;
    case GenericToken::STRING_LITERAL:
      std::cerr << "recieved: '" << token.value << '\'' << std::endl;
      throw std::runtime_error("string literal : @todo");
    case GenericToken::CHAR_LITERAL:
    {
      Node::NumericLiteral node;
      node.token = token;
      node.type = ConstituentToken::TYPE_CHAR;
      auto ptr = std::make_unique<std::variant<Node::GenericExpr, Node::BinaryExpr, Node::UnaryExpr,
                                               Node::NumericLiteral, Node::Punctuator, Node::Operator>>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    }
    break;
    case GenericToken::PUNCTUATOR:
    {
      if (dictionary_.find(token.value) == dictionary_.end())
      {
        std::cerr << "recieved: '" << token.value << '\'' << std::endl;
        throw std::invalid_argument("error: dictionary does not contain punctuator");
      }
      Node::Punctuator node;
      node.token = token;
      node.type = dictionary_.at(token.value);
      auto ptr = std::make_unique<std::variant<Node::GenericExpr, Node::BinaryExpr, Node::UnaryExpr,
                                               Node::NumericLiteral, Node::Punctuator, Node::Operator>>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    }
    break;
    case GenericToken::OPERATOR:
    {
      if (dictionary_.find(token.value) == dictionary_.end())
      {
        throw std::invalid_argument("error: dictionary does not contain operator");
      }
      Node::Operator node;
      node.token = token;
      node.type = dictionary_.at(token.value);
      auto ptr = std::make_unique<std::variant<Node::GenericExpr, Node::BinaryExpr, Node::UnaryExpr,
                                               Node::NumericLiteral, Node::Punctuator, Node::Operator>>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    }
    break;
    case GenericToken::TYPE: throw std::invalid_argument("error: keyword found in expression");
  }
  if (gen_expr_ptr)
  {
    return gen_expr_ptr;
  }
  return std::unique_ptr<Node::GenericExpr>(); // throw exception here
}

/// @todo suppport for optional
/// @todo move some of this to lexer
ConstituentToken Parser::inferTypeNumericLiteral(const std::string &value)
{
  static const std::regex pattern(R"(\d+(\.\d+[fd]?)?|u[ilsc]?)");
  std::smatch match;
  if (!std::regex_match(value, match, pattern))
  {
    throw std::invalid_argument("error: invalid numeric literal");
  }

  bool is_u_type = value.find('u') != std::string::npos;
  bool is_f_type = value.find('f') != std::string::npos || value.find('d') != std::string::npos;

  if (is_u_type && is_f_type)
  {
    throw std::invalid_argument("ambiguous numeric literal");
  }

  char suffix = match[2].str().empty() ? '\0' : match[2].str()[0];

  switch (suffix)
  {
    case 'u': return ConstituentToken::TYPE_UINT;
    case 'i': return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    case 'l': return is_u_type ? ConstituentToken::TYPE_ULONG : ConstituentToken::TYPE_LONG;
    case 's': return is_u_type ? ConstituentToken::TYPE_USHORT : ConstituentToken::TYPE_SHORT;
    case 'c': return is_u_type ? ConstituentToken::TYPE_UCHAR : ConstituentToken::TYPE_CHAR;
    case 'f': return ConstituentToken::TYPE_FLOAT;
    case 'd': return ConstituentToken::TYPE_DOUBLE;
    case '\0': return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    default: std::cerr << value << std::endl; throw std::invalid_argument("ambiguous numeric literal token");
  }
}


int Parser::numTokens() const { return tokens_.size(); }
