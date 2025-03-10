#include <unordered_map>

#include "lexer.hpp"
#include "rat_source.hpp"

int main()
{
  std::string file_name = "data/testfile.rat";
  auto rat = RatSource(file_name);

  std::unordered_map<std::string, TokenType> map;
  std::deque<Token> d;
  auto lex = Lexer(rat);
  while (lex.extractTokenLiteral())
  {
  }
  lex.debugPrinter();
  return 0;
}