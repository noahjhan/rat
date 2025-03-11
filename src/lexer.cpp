#include "lexer.hpp"

/*

The lexer should really only convert words to genericToken enums

it should be able to (either in ratSource or in lexer) figure out line numbers

lexical errors will appear like invalid character for literal, unrecognized
keyword etc...

parser will handle syntax errors

*/

bool isAcceptableIdentifier(const char &ch)
{
  return std::isalnum(ch) || ch == '_';
}

bool isAcceptableStringLiteral(const char &ch)
{
  // prob could have something more validating than this
  return std::isprint(ch) || std::isspace(ch);
}

bool isAcceptableNumericLiteral(const char &ch)
{
  // no support for char conversion or non-decimal base
  return std::isdigit(ch) || ch == '.' || ch == 'u';

  // @todo: support for float double short long etc ?
}

Lexer::Lexer(const RatSource &source_file) : source_file_(source_file)
{
  punctuators_ = {':', '\'', '\"', '[', ']',
                  '{', '}',  '(',  ')'}; // subject to change

  keywords_ = {"let", "if", "else", "elif", "fn", "fn_", "fn?", "fn/"};

  operators_ = {"+",  "-",  "*", "/", "%", "==", "!=", "<",  ">",  "<=", ">=",
                "&&", "||", "!", "&", "|", "^",  "~",  "<<", ">>", "->", "=>"};
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

void Lexer::advanceToken()
{
  source_file_.advanceWhitespace();
  char curr = source_file_.advanceChar();
  std::string partial;

  if (curr == EOF || curr == '\0')
  {
    std::cout << "end of file reached" << std::endl;
    return;
  }

  // check if punctuator
  if (punctuators_.find(curr) != punctuators_.end())
  {
    // process punctuator
    if (curr == '\"')
    {
      source_file_.reverse();
      advanceStringLiteral();
      return;
    }
    else if (curr == '\'')
    {
      // process char literal
    }
  }

  while (!std::isspace(curr) && curr != EOF && curr != '\0')
  {
    partial.push_back(curr);

    // check if keyword
    if (keywords_.find(partial) != keywords_.end())
    {
      char peek = source_file_.peekChar();
      partial.push_back(peek);
      if (keywords_.find(partial) != keywords_.end())
      {
        // process keyword with similar start
      }
      else
      {
        partial.pop_back();
        // process keyword with distinct begin
      }
    }

    if (operators_.find(partial) != operators_.end())
    {
      char peek = source_file_.peekChar();
      partial.push_back(peek);
      if (operators_.find(partial) != operators_.end())
      {
        // process multi character operator
      }
      else
      {
        partial.pop_back();
        // process single character operator
      }
    }
  }

  // check if operator

  // check if string literal

  // check if numeric literal
}

void Lexer::dequePush(GenericToken type, const std::string &value)
{
  // will probably have to pass line and col number as param
  auto t =
      Token(type, value, source_file_.getLineNum(), source_file_.getColNum());
  tokens_.push_back(t);
}