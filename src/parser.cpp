#include "parser.hpp"

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