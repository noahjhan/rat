#include <unordered_map>

#include "lexer.hpp"
#include "parser.hpp"
#include "rat_source.hpp"

int main()
{
  std::string file_name = "data/testfile.rat";
  auto rat = RatSource(file_name);
  auto lex = Lexer(rat);
  while (lex.advanceToken())
  {
  }
  lex.debugPrinter(true); // use true as param if want verbose printing i.e. line & column numbers
  std::deque<Token> dq = lex.getTokens();
  auto parse = Parser(dq);
  return 0;
}