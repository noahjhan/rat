#include <unordered_map>

#include "lexer.hpp"
#include "rat_source.hpp"

int main()
{
  // std::string file_name = "data/testfile.rat";
  // std::string file_name = "data/var_ass_test.rat";
  std::string file_name = "data/testfile.rat";
  auto rat = RatSource(file_name);

  auto lex = Lexer(rat);
  while (lex.advanceToken())
  {
  }
  lex.debugPrinter();
  return 0;
}