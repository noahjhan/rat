#include "lexer.hpp"

/*

The lexer should really only convert words to genericToken enums

it should be able to (either in ratSource or in lexer) figure out line numbers

lexical errors will appear like invalid character for literal, unrecognized
keyword etc...

parser will handle syntax errors

@todo: implement line and col number

*/

bool Lexer::isAcceptableIdentifier(const char &ch)
{
  // if (punctuators_.find(ch) != punctuators_.end())
  // {
  //   return false;
  // }
  return std::isalnum(ch) || ch == '_';
}

bool Lexer::isAcceptableStringLiteral(const char &ch)
{
  // prob could have something more validating than this
  return std::isprint(ch) || std::isspace(ch);
}

bool Lexer::isAcceptableNumericLiteral(const char &ch)
{
  // no support for char conversion or non-decimal base
  std::unordered_set<char> non_digits = {'u', 'i', 'f', 'd', 'l',
                                         's', 'c', '.', '-'};
  return std::isdigit(ch) || (non_digits.find(ch) != non_digits.end());

  // @todo: support for float double short long etc ?
}

bool Lexer::isAcceptableCharLiteral(const char &ch)
{

  return true;
  // @todo: support for chars
}

Lexer::Lexer(const RatSource &source_file) : source_file_(source_file)
{
  punctuators_ = {':', '\'', '\"', '[', ']',
                  '{', '}',  '(',  ')'}; // subject to change

  keywords_ = {"let", "oplet", "if",  "else", "elif",
               "fn",  "fn_",   "fn?", "fn/",  "null"};

  operators_ = {"=", "+", "-",  "*",  "/",  "%",  "==", "!=",
                "<", ">", "<=", ">=", "&&", "||", "!",  "&",
                "|", "^", "~",  "<<", ">>", "->", "=>"};

  types_ = {"int",        "float",   "double",   "bool",      "char",
            "long",       "short",   "pointer",  "uint",      "ulong",
            "ushort",     "uchar",   "string",   "op_int",    "op_float",
            "op_double",  "op_bool", "op_char",  "op_long",   "op_short",
            "op_pointer", "op_uint", "op_ulong", "op_ushort", "op_uchar",
            "op_string"};
}

void Lexer::advanceStringLiteral()
{
  char curr = source_file_.advanceChar();
  std::string partial;
  if (curr != '\"')
  {
    std::cerr << "advanceStringLiteral: expected: \", recieved: " << curr
              << std::endl;
    throw std::invalid_argument("error: caller function did not align stream");
  }
  while (isAcceptableStringLiteral(curr))
  {
    partial.push_back(curr);
    curr = source_file_.advanceChar();
    if (curr == '\"')
    {
      // end of literal reached
      partial.push_back(curr);
      dequePush(GenericToken::STRING_LITERAL, partial);
      return;
    }
    if (curr == EOF || curr == '\0')
    {
      // end of file reached
      throw std::invalid_argument(
          "error: string literals must be surrounded by double-quotes");
    }
  }
}

void Lexer::advanceCharLiteral()
{
  char curr = source_file_.advanceChar();
  std::string partial;
  if (curr != '\'')
  {
    std::cerr << "advanceCharLiteral: expected: \', recieved: " << curr
              << std::endl;
    throw std::invalid_argument("error: caller function did not align stream");
  }
  while (isAcceptableCharLiteral(curr))
  {
    partial.push_back(curr);
    curr = source_file_.advanceChar();
    if (curr == '\'')
    {
      // end of literal reached
      partial.push_back(curr);
      if (source_file_.peekChar() == '\'')
      {
        partial.push_back(source_file_.advanceChar());
      }
      dequePush(GenericToken::CHAR_LITERAL, partial);
      return;
    }
    if (curr == EOF || curr == '\0')
    {
      // end of file reached
      throw std::invalid_argument(
          "error: character literals must be surrounded by single-quotes");
    }
  }
}

bool Lexer::advanceToken()
{
  source_file_.advanceWhitespace();
  char curr = source_file_.advanceChar();
  std::string partial;

  if (curr == EOF || curr == '\0')
  {
    return false;
  }

  // check if punctuator
  if (punctuators_.find(curr) != punctuators_.end())
  {
    // process punctuator
    if (curr == '\"')
    {
      source_file_.reverse();
      advanceStringLiteral();
      return true;
    }
    else if (curr == '\'')
    {
      source_file_.reverse();
      advanceCharLiteral();
      return true;
    }
    else
    {
      if (!partial.empty())
      {
        std::cerr << "expected empty partial, recieved: " << partial
                  << std::endl;
        throw std::runtime_error(
            "failed to correctly process token before punctuator");
      }
      partial.push_back(curr);
      dequePush(GenericToken::PUNCTUATOR, partial);
      return true;
    }
  }

  while (!std::isspace(curr) && curr != EOF && curr != '\0')
  {
    // std::cout << "DEBUG" << std::endl;
    // std::cout << "CURR: " << curr << std::endl;
    // std::cout << "PARTIAL: " << partial << std::endl;
    // usleep(100000);

    if (punctuators_.find(curr) != punctuators_.end())
    {
      source_file_.reverse();
      break;
    }
    partial.push_back(curr);

    // check if keyword
    if (keywords_.find(partial) != keywords_.end())
    {
      char peek = source_file_.peekChar();
      partial.push_back(peek);
      if (keywords_.find(partial) == keywords_.end())
      {
        partial.pop_back();
      }
      else
      {
        curr = source_file_.advanceChar();
      }
      dequePush(GenericToken::KEYWORD, partial);
      return true;
    }

    if (types_.find(partial) != types_.end())
    {
      dequePush(GenericToken::TYPE, partial);
      return true;
    }

    if (operators_.find(partial) != operators_.end())
    {
      char peek = source_file_.peekChar();
      partial.push_back(peek);
      if (operators_.find(partial) == operators_.end())
      {
        partial.pop_back();
      }
      else
      {
        curr = source_file_.advanceChar();
      }
      dequePush(GenericToken::OPERATOR, partial);
      return true;
    }
    curr = source_file_.advanceChar();
  }
  // generally identifiers cannot begin with numbers
  // numbers can only end with alphabetic symbols

  // handle numeric literals
  bool is_numeric = true;
  bool is_identifier = (!std::isdigit(partial.front()));
  for (const auto &ch : partial)
  {
    if (!isAcceptableNumericLiteral(ch))
    {
      is_numeric = false;
    }
    if (!isAcceptableIdentifier(ch))
    {
      is_identifier = false;
    }
  }

  if (is_numeric == is_identifier)
  {
    std::cerr << "recieved: '" << partial << '\'' << std::endl;
    throw std::invalid_argument("ambiguous token");
  }

  if (is_identifier)
  {
    dequePush(GenericToken::IDENTIFIER, partial);
    return true;
  }

  if (is_numeric)
  {
    dequePush(GenericToken::NUMERIC_LITERAL, partial);
    return true;
  }
  std::cerr << "recieved: '" << partial << '\'' << std::endl;
  throw std::runtime_error("unrecognized token");
}

void Lexer::dequePush(GenericToken type, const std::string &value)
{
  // will probably have to pass line and col number as param
  auto t =
      Token(type, value, source_file_.getLineNum(), source_file_.getColNum());
  tokens_.push_back(t);
}

std::deque<Token> &Lexer::getTokens() { return tokens_; }

void Lexer::debugPrinter(bool verbose)
{
  if (verbose)
  {
    std::cout << "DEBUG" << std::endl;
    for (const auto &t : tokens_)
    {
      std::cout << "value: " << t.value_ << std::endl;
      std::cout << "line: " << t.line_num_ << std::endl;
      std::cout << "col: " << t.col_num_ << std::endl;
      std::cout << std::endl;
    }
  }
  else
  {
    for (const auto &t : tokens_)
    {
      std::cout << t.value_ << std::endl;
    }
  }
}