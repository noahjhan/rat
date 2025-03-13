#include "test.hpp"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define PURPLE "\033[1;35m"
#define BAR "=============================="
/*

try
{
  return true;
}
catch (const std::exception &e)
{
  return false;
}

*/

bool TEST_LEXER()
{
  std::cout << PURPLE << "TEST CASE: Lexer\n" << BAR << RESET << std::endl;
  try
  {

    std::string file_name = "data/testfile.rat";
    auto rat = RatSource(file_name);
    auto lex = Lexer(rat);
    while (lex.advanceToken())
    {
    }
    // lex.debugPrinter(false /* use true here for verbose printing */);
    std::deque<Token> dq = lex.getTokens();
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << RED << BAR << "\nTEST CASE FAILED: Lexer\n" << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << BAR << "\nTEST CASE PASSED: Lexer\n" << RESET << std::endl;
  return true;
}

bool TEST_PARSER()
{
  std::cout << PURPLE << "TEST CASE: Parser\n" << BAR << RESET << std::endl;
  try
  {

    std::string file_name = "data/expression.rat";
    auto rat = RatSource(file_name);
    auto lex = Lexer(rat);
    while (lex.advanceToken())
    {
    }
    // lex.debugPrinter(true /* use true here for verbose printing */);
    std::deque<Token> dq = lex.getTokens();
    auto parse = Parser(dq);
    while (parse.numTokens())
    {
      parse.tokenToExpr();
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << RED << BAR << "\nTEST CASE FAILED: Parser\n" << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << BAR << "\nTEST CASE PASSED: Parser\n" << RESET << std::endl;
  return true;
}

bool TEST_ALL() { return (TEST_LEXER() && TEST_PARSER()); }