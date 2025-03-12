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
 */

int add(int a, int b) { return a + b; }

Parser::Parser(std::deque<Token> &tokens) : tokens_(tokens)
{

  for (const auto &token : tokens_)
  {
    if (token.value_.empty())
    {
      throw std::invalid_argument("syntax error: empty token");
    }
    switch (token.type_)
    {
      case GenericToken::IDENTIFIER:
        break;
      case GenericToken::KEYWORD:
        if (dictionary_.find(token.value_) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized keyword");
        }
        break;
      case GenericToken::NUMERIC_LITERAL:
        break;
      case GenericToken::STRING_LITERAL:
        break;
      case GenericToken::CHAR_LITERAL:
        break;
      case GenericToken::PUNCTUATOR:
        if (dictionary_.find(token.value_) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized punctuator");
        }
        break;
      case GenericToken::OPERATOR:
        if (dictionary_.find(token.value_) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized operator");
        }
        break;
      case GenericToken::TYPE:
        if (dictionary_.find(token.value_) == dictionary_.end())
        {
          std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized type");
        }
        break;
    }
  }
}