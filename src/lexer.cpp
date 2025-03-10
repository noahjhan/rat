
#include "lexer.hpp"

// helper function

bool isAcceptableIdentifier(const char &ch)
{
  if (std::isalnum(ch) || ch == '_')
  {
    return true;
  }
  else
  {
    return false;
  }
  // return (std::isalnum(ch) || ch == '_' || ch == '$');
}

bool isAcceptableStringLiteral(const char &ch)
{
  if (ch == EOF || ch == '\0')
  {
    return false;
  }

  if (std::isprint(ch) || std::isalnum(ch) || std::isspace(ch) ||
      std::ispunct(ch))
  {
    return true;
  }
  std::cerr << "in isAcceptableStringLiteral: '" << ch << "' " << std::endl;
  throw std::invalid_argument("error: unexpected character");
}

// lexer class

Lexer::Lexer(const RatSource &source_file) : source_file_(source_file)
{
  tokens_.clear();
  token_dict_ = {{"int", TokenType::TYPE_INT},
                 {"float", TokenType::TYPE_FLOAT},
                 {"double", TokenType::TYPE_DOUBLE},
                 {"bool", TokenType::TYPE_BOOL},
                 {"char", TokenType::TYPE_CHAR},
                 {"long", TokenType::TYPE_LONG},
                 {"short", TokenType::TYPE_SHORT},
                 {"pointer", TokenType::TYPE_POINTER},
                 {"uint", TokenType::TYPE_UINT},
                 {"ulong", TokenType::TYPE_ULONG},
                 {"ushort", TokenType::TYPE_USHORT},
                 {"uchar", TokenType::TYPE_UCHAR},
                 {"op_int", TokenType::TYPE_OP_INT},
                 {"op_float", TokenType::TYPE_OP_FLOAT},
                 {"op_double", TokenType::TYPE_OP_DOUBLE},
                 {"op_bool", TokenType::TYPE_OP_BOOL},
                 {"op_char", TokenType::TYPE_OP_CHAR},
                 {"op_long", TokenType::TYPE_OP_LONG},
                 {"op_short", TokenType::TYPE_OP_SHORT},
                 {"op_uint", TokenType::TYPE_OP_UINT},
                 {"op_ulong", TokenType::TYPE_OP_ULONG},
                 {"op_ushort", TokenType::TYPE_OP_USHORT},
                 {"op_uchar", TokenType::TYPE_OP_UCHAR},
                 {"let", TokenType::VARIABLE_DECLARATION_LET},
                 {"oplet", TokenType::VARIABLE_DECLARATION_OPLET},
                 {"f", TokenType::FUNCTION_DECLARATION_F},
                 {"f_", TokenType::FUNCTION_DECLARATION_F_VOID},
                 {"f?", TokenType::FUNCTION_DECLARATION_F_OPTIONAL},
                 {"f/", TokenType::FUNCTION_DECLARATION_F_LAMBDA},
                 {"ret", TokenType::FUNCTION_RETURN},
                 {"rev", TokenType::FUNCTION_DECLARATION_F_VOID},
                 {"if", TokenType::CONDITIONAL_IF},
                 {"else", TokenType::CONDITIONAL_ELSE},
                 {"else if", TokenType::CONDITIONAL_ELSE_IF},
                 {"match", TokenType::PATTERN_MATCH},
                 {"null", TokenType::NULL_LITERAL},
                 {";", TokenType::SEMICOLON},
                 {":", TokenType::COLON},
                 {"'", TokenType::SINGLE_QUOTE},
                 {"\"", TokenType::DOUBLE_QUOTE},
                 {",", TokenType::COMMA},
                 {"[", TokenType::BRACKET_OPEN},
                 {"]", TokenType::BRACKET_CLOSE},
                 {"{", TokenType::BRACE_OPEN},
                 {"}", TokenType::BRACE_CLOSE},
                 {"(", TokenType::PARENTHESES_OPEN},
                 {")", TokenType::PARENTHESES_CLOSE},
                 {"//", TokenType::COMMENT_LINE},
                 {"/*", TokenType::COMMENT_BLOCK_OPEN},
                 {"*/", TokenType::COMMENT_BLOCK_CLOSE},
                 {"+", TokenType::ARITHMETHIC_ADD},
                 {"-", TokenType::ARITHMETHIC_MUL},
                 {"*", TokenType::ARITHMETHIC_MUL},
                 {"/", TokenType::ARITHMETHIC_DIV},
                 {"%", TokenType::ARITHMETHIC_MOD},
                 {"==", TokenType::COMPARISON_EQ},
                 {"!=", TokenType::COMPARISON_NEQ},
                 {"<", TokenType::COMPARISON_LT},
                 {">", TokenType::COMPARISON_GT},
                 {"<=", TokenType::COMPARISON_LTE},
                 {">=", TokenType::COMPARISON_GTE},
                 {"&&", TokenType::LOGICAL_AND},
                 {"||", TokenType::LOGICAL_OR},
                 {"!", TokenType::LOGICAL_NOT},
                 {"&", TokenType::BITWISE_AND},
                 {"|", TokenType::BITWISE_OR},
                 {"^", TokenType::BITWISE_XOR},
                 {"~", TokenType::BITWISE_NEG},
                 {"<<", TokenType::BITWISE_SL},
                 {">>", TokenType::BITWISE_SR},
                 {"->", TokenType::ARROW}};
  punctuators_ = {';', '\'', '\"', '[', ']', '{', '}', '(', ')'};
}

bool Lexer::extractTokenLiteral()
{
  source_file_
      .advanceWhitespace(); // for now ignore whitespace at start of file
  std::string partial;

  char curr = source_file_.peekChar();

  if (curr == EOF)
  {
    return false;
  }

  while (!std::isspace(curr))
  {
    curr = source_file_.advanceChar();
    if (curr == EOF || curr == '\0')
    {
      std::cerr << "in advance whitespace: '" << int(curr)
                << "' invalid character" << std::endl;
      return false;
    }
    else if (punctuators_.find(curr) != punctuators_.end() && partial.empty())
    {
      partial.push_back(curr);
      break;
    }
    else if (punctuators_.find(curr) != punctuators_.end())
    {
      source_file_.reverse();
      break;
    }
    partial.push_back(curr);
  }
  TokenType token_type = stringToToken(partial);
  auto t = Token(token_type, partial, source_file_.getLineNum(),
                 source_file_.getColNum());
  tokens_.push_back(t);
  return true;
}

void Lexer::expectVariableAssignment()
{
  std::vector<std::string> token_values; // ?? maybe
  source_file_.advanceWhitespace();
  std::string partial;
  // first step extract let as string

  partial = source_file_.readWord();

  if (partial != "let")
  {
    std::cerr << "in expectVariableAssignment: found keyword '" << partial
              << "' expected 'let'" << std::endl;
    throw std::invalid_argument("error: unexpected keyword");
  }

  tokenPush(stringToToken(partial), partial, source_file_.getLineNum(),
            source_file_.getColNum());

  partial.clear();

  source_file_.advanceWhitespace();
  char curr = source_file_.peekChar();

  if (std::isdigit(curr))
  {
    std::cerr << "in expectVariableAssignment: found character '" << curr
              << '\'' << std::endl;
    throw std::invalid_argument("identifier cannot start with a number");
  }

  while (isAcceptableIdentifier(curr))
  {
    curr = source_file_.advanceChar();
    partial.push_back(curr);
    curr = source_file_.peekChar(); // can def be optimized
  }

  tokenPush(TokenType::VARIABLE_ID, partial, source_file_.getLineNum(),
            source_file_.getColNum());
  partial.clear();
  source_file_.advanceWhitespace();
  curr = source_file_.advanceChar();
  partial.push_back(curr);

  if (curr != ':')
  {
    std::cerr << "in expectVariableAssignment: found character '" << curr
              << '\'' << std::endl;
    throw std::invalid_argument("error: expected ':'");
  }

  tokenPush(stringToToken(partial), partial, source_file_.getLineNum(),
            source_file_.getColNum());
  partial.clear();
  source_file_.advanceWhitespace();
  partial = source_file_.readWord();

  tokenPush(stringToToken(partial), partial, source_file_.getLineNum(),
            source_file_.getColNum());
  partial.clear();
  source_file_.advanceWhitespace();
  curr = source_file_.advanceChar();
  partial.push_back(curr);

  if (curr != '=')
  {
    std::cerr << "in expectVariableAssignment: found character '" << curr
              << '\'' << std::endl;
    throw std::invalid_argument("error: expected '='");
  }

  tokenPush(stringToToken(partial), partial, source_file_.getLineNum(),
            source_file_.getColNum());
  partial.clear();
  source_file_.advanceWhitespace();
  partial = source_file_.readWord();

  tokenPush(TokenType::LITERAL, partial, source_file_.getLineNum(),
            source_file_.getColNum());
  return;
}

void Lexer::expectStringLiteral()
{ // expect current char to be "

  bool exit_flag = false;
  char curr = source_file_.advanceChar();
  std::string partial;

  if (curr != '"')
  {
    std::cerr << "in expectStringLiteral: expected '\"', recieved '" << curr
              << '\'' << std::endl;
    throw std::invalid_argument("error unexpected char");
  }

  while (isAcceptableStringLiteral(curr))
  {
    partial.push_back(curr);

    if (curr == '\"' && exit_flag)
    {
      tokenPush(TokenType::LITERAL, partial, source_file_.getLineNum(),
                source_file_.getColNum());
      return;
    }
    else if (curr == '\"')
    {
      exit_flag = !exit_flag;
    }

    curr = source_file_.advanceChar();
  }
  if (curr == '\0' || curr == EOF)
  {
    std::cerr << "in expectStringLiteral: expected closing quotation"
              << std::endl;
              throw std::invalid_argument("invalid syntax");
  }
}

TokenType Lexer::stringToToken(const std::string &str)
{
  auto it = token_dict_.find(str);
  if (it != token_dict_.end())
  {
    return it->second;
  }
  return TokenType::LITERAL;

  std::cerr << "Unrecognized token: '" << str
            << "' at line: " << std::to_string(source_file_.getLineNum())
            << " at col: " << std::to_string(source_file_.getColNum())
            << std::endl;
}

void Lexer::tokenPush(TokenType t, std::string v, unsigned int line,
                      unsigned int col)
{
  auto token = Token(t, v, line, col);
  tokens_.push_back(token);
}

void Lexer::debugPrinter()
{
  for (const auto &elem : tokens_)
  {
    std::cout << elem.value << std::endl;
  }
}