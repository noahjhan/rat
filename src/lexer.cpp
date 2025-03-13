#include "lexer.hpp"

/**
 *
 * @todo: implement line and col number
 *
 * @todo: allow for spaces between keywords and types
 *
 * @todo: add support for comments
 *
 * @todo: add support for escape sequence in string literals
 *
 * @todo: add support for lists i.e {1, 2, 3, 4, 5}
 *        perhaps an array type?
 *
 * @todo: add support for structs -> struct keyword
 *
 * @todo: test string literal edge cases
 *
 * @todo: add functionality for multi-line string literals i.e "hello""world" is one token (this is likely a parser
 *        issue)
 *
 * @todo: validate numeric literals such that they can only contain one of 'u' and one of type letter i.e  129.68df is
 *        invalid (what is a double float?)
 */

bool Lexer::isAcceptableIdentifier(const char &ch) { return std::isalnum(ch) || ch == '_'; }

/// @todo: stricter string literal evaluation
bool Lexer::isAcceptableStringLiteral(const char &ch) { return std::isprint(ch) || std::isspace(ch); }

/// @todo: add support for non-decimal base
bool Lexer::isAcceptableNumericLiteral(const char &ch)
{
  std::unordered_set<char> non_digits = {'u', 'i', 'f', 'd', 'l', 's', 'c', '.', '-'};
  return std::isdigit(ch) || (non_digits.find(ch) != non_digits.end());
}
/**
 * @todo: support for chars
 * this function may be redundant
 */
bool Lexer::isAcceptableCharLiteral(const char &ch) { return isprint(ch); }

Lexer::Lexer(const RatSource &source_file) : source_file_(source_file)
{
  punctuators_ = {':', '\'', '\"', '[', ']', '{', '}', '(', ')'};

  keywords_ = {"let", "op", "if", "else", "fn", "fn_", "fn?", "fn/", "null"};

  operators_ = {"=",  "+",  "-", "*", "/", "%", "==", "!=", "<",  ">",  "<=", ">=",
                "&&", "||", "!", "&", "|", "^", "~",  "<<", ">>", "->", "=>"};

  types_ = {"int",     "float", "double", "bool",   "char",  "long",  "short",
            "pointer", "uint",  "ulong",  "ushort", "uchar", "string"};

  str_representation = {{GenericToken::IDENTIFIER, "Identifier"},
                        {GenericToken::KEYWORD, "Keyword"},
                        {GenericToken::NUMERIC_LITERAL, "Numeric Literal"},
                        {GenericToken::STRING_LITERAL, "String Literal"},
                        {GenericToken::CHAR_LITERAL, "Char Literal"},
                        {GenericToken::PUNCTUATOR, "Punctuator"},
                        {GenericToken::OPERATOR, "Operator"},
                        {GenericToken::TYPE, "Type"}};
}

void Lexer::advanceStringLiteral()
{

  const unsigned int line_num = source_file_.getLineNum();
  const unsigned int col_num = source_file_.getColNum();

  char curr = source_file_.advanceChar();
  std::string partial;
  if (curr != '\"')
  {
    debugLineCol(line_num, col_num);
    std::cerr << "advanceStringLiteral: expected: \", recieved: " << curr << std::endl;
    debugPrintln(line_num);
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
      dequePush(GenericToken::STRING_LITERAL, partial, line_num, col_num);
      return;
    }
    if (curr == EOF || curr == '\n')
    {
      debugLineCol(line_num, col_num);
      debugPrintln(line_num);
      throw std::invalid_argument("error: string literals must be surrounded by double-quotes");
    }
  }
}

void Lexer::advanceCharLiteral()
{
  const unsigned int line_num = source_file_.getLineNum();
  const unsigned int col_num = source_file_.getColNum();
  char curr = source_file_.advanceChar();
  std::string partial;
  if (curr != '\'')
  {

    debugLineCol(line_num, col_num);
    std::cerr << "advanceCharLiteral: expected: \', recieved: " << curr << std::endl;
    debugPrintln(line_num);
    throw std::invalid_argument("error: caller function did not align stream");
  }
  while (isAcceptableCharLiteral(curr))
  {
    std::unordered_set<char> escape_chars = {'n', 't', 'r', 'b', 'f', 'v', 'a', '\\', '?', '\'', '\"', '0'};
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
      if ((partial.size() != 3 && partial.size() != 4) || (partial.size() == 4 && partial[1] != '\\') ||
          (partial.size() == 3 && partial[1] == '\\') || (escape_chars.find(partial[2]) == escape_chars.end()))
      {
        debugLineCol(line_num, col_num);
        std::cerr << "recieved: '" << partial << '\'' << std::endl;
        debugPrintln(line_num);
        throw std::invalid_argument("syntax error: unrecognized char literal");
      }

      dequePush(GenericToken::CHAR_LITERAL, partial, line_num, col_num);
      return;
    }
    if (curr == EOF)
    {
      // end of file reached
      debugLineCol(line_num, col_num);
      debugPrintln(line_num);
      throw std::invalid_argument("error: character literals must be surrounded by single-quotes");
    }
  }
}

/**
 * @todo: access line and col num at the start of the token
 * to include more debug info
 */
bool Lexer::advanceToken()
{
  source_file_.advanceWhitespace();

  const unsigned int line_num = source_file_.getLineNum();
  const unsigned int col_num = source_file_.getColNum();

  char curr = source_file_.advanceChar();
  std::string partial;

  if (curr == EOF)
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

        debugLineCol(line_num, col_num);
        std::cerr << "expected empty partial, recieved: '" << partial << '\'' << std::endl;
        debugPrintln(line_num);
        throw std::invalid_argument("failed to correctly process token before punctuator");
      }
      partial.push_back(curr);
      dequePush(GenericToken::PUNCTUATOR, partial, line_num, col_num);
      return true;
    }
  }

  while (!std::isspace(curr) && curr != EOF)
  {
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
      dequePush(GenericToken::KEYWORD, partial, line_num, col_num);
      return true;
    }

    if (types_.find(partial) != types_.end())
    {
      dequePush(GenericToken::TYPE, partial, line_num, col_num);
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
      dequePush(GenericToken::OPERATOR, partial, line_num, col_num);
      return true;
    }
    curr = source_file_.advanceChar();
  }

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

  if ((is_numeric && is_identifier))
  {
    debugLineCol(line_num, col_num);
    std::cerr << "recieved: '" << partial << '\'' << std::endl;
    debugPrintln(line_num);
    throw std::invalid_argument("ambiguous token");
  }

  if (is_identifier)
  {
    dequePush(GenericToken::IDENTIFIER, partial, line_num, col_num);
    return true;
  }

  if (is_numeric)
  {
    dequePush(GenericToken::NUMERIC_LITERAL, partial, line_num, col_num);
    return true;
  }
  debugLineCol(line_num, col_num);
  std::cerr << "recieved: '" << partial << '\'' << std::endl;
  debugPrintln(line_num);
  throw std::invalid_argument("unrecognized token");
}
/// @todo: pass line and col num as param
void Lexer::dequePush(GenericToken type, const std::string &value, const unsigned int &line_num,
                      const unsigned int &col_num)
{
  Token t;
  t.type = type;
  t.value = value;
  t.line_num = line_num;
  t.col_num = col_num;
  tokens_.push_back(t);
}

std::deque<Token> &Lexer::getTokens() { return tokens_; }

void Lexer::debugPrintln(const unsigned int &line_num)
{
  source_file_.seekLine(line_num);
  std::cerr << line_num << " | " << source_file_.readLine() << std::endl;
}

void Lexer::debugLineCol(const unsigned int &line_num, const unsigned int &col_num)
{
  std::cerr << "at line: " << line_num << ", col: " << col_num << std::endl;
}

void Lexer::debugPrinter(bool verbose)
{
  if (verbose)
  {
    std::cout << "DEBUG" << std::endl;
    for (const auto &t : tokens_)
    {
      std::string str = str_representation.at(t.type);
      std::cout << "token: " << str << std::endl;
      std::cout << "value: " << t.value << std::endl;
      std::cout << "line: " << t.line_num << std::endl;
      std::cout << "col: " << t.col_num << std::endl;
      std::cout << std::endl;
    }
  }
  else
  {
    for (const auto &t : tokens_)
    {
      std::cout << t.value << std::endl;
    }
  }
}
