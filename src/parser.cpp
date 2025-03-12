#include "parser.hpp"

Parser::Parser(std::deque<Token> &tokens) : tokens_(tokens)
{
  std::unordered_set<char> escape_chars = {'n', 't',  'r', 'b',  'f',  'v',
                                           'a', '\\', '?', '\'', '\"', '0'};
  for (const auto &token : tokens_)
  {
    if (token.value_.empty())
    {
      throw std::runtime_error("syntax error: empty token");
    }
    switch (token.type_)
    {
    case GenericToken::IDENTIFIER:
      break;
    case GenericToken::KEYWORD:
      if (dictionary_.find(token.value_) == dictionary_.end())
      {
        std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
        throw std::runtime_error("syntax error: unrecognized keyword");
      }
      break;
    case GenericToken::NUMERIC_LITERAL:
      break;
    case GenericToken::STRING_LITERAL:
      break;
    case GenericToken::CHAR_LITERAL:
      // @todo: this conditional could be more readable
      if ((token.value_.size() != 3 && token.value_.size() != 4) ||
          (token.value_.size() == 4 &&
           (token.value_[1] != '\\' ||
            (escape_chars.find(token.value_[2]) == escape_chars.end()))))
      {
        std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
        throw std::runtime_error("syntax error: unrecognized char literal");
      }
      break;
    case GenericToken::PUNCTUATOR:
      if (dictionary_.find(token.value_) == dictionary_.end())
      {
        std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
        throw std::runtime_error("syntax error: unrecognized punctuator");
      }
      break;
    case GenericToken::OPERATOR:
      if (dictionary_.find(token.value_) == dictionary_.end())
      {
        std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
        throw std::runtime_error("syntax error: unrecognized operator");
      }
      break;
    case GenericToken::TYPE:
      if (dictionary_.find(token.value_) == dictionary_.end())
      {
        std::cerr << "recieved: '" << token.value_ << '\'' << std::endl;
        throw std::runtime_error("syntax error: unrecognized type");
      }
      break;
    }
  }
}