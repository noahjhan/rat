#include "test.hpp"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
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
  std::cout << BLUE << BAR << "\nTEST CASE: Lexer\n"
            << RESET << std::endl;
  try
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
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << RED << "\nTEST CASE FAILED: Lexer\n"
              << BAR
              << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << "\nTEST CASE PASSED: Lexer\n"
            << BAR << RESET << std::endl;
  return true;
}

bool TEST_ALL()
{
  return TEST_LEXER();
}