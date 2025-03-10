#include <unordered_map>

#include "lexer.hpp"
#include "rat_source.hpp"

int main()
{
  // std::string file_name = "data/testfile.rat";
  // std::string file_name = "data/var_ass_test.rat";
  std::string file_name = "data/multi_expect_test.rat";
  auto rat = RatSource(file_name);

  std::unordered_map<std::string, TokenType> map;
  std::deque<Token> d;
  auto lex = Lexer(rat);
  lex.expectVariableAssignment();
  lex.expectStringLiteral();
  lex.expectVariableAssignment();
  lex.expectStringLiteral();
  lex.debugPrinter();
  return 0;
}