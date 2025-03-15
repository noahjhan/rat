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
    // lex.debugPrinter(true /* use true here for verbose printing */);
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

bool TEST_EXPR_SIMPLE()
{
  std::cout << PURPLE << "TEST CASE: Expr Simple\n" << BAR << RESET << std::endl;
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
    std::vector<Node::GenericExpr> nodes;
    while (parse.numTokens())
    {
      nodes.push_back(std::move(*(parse.tokenToExpr())));
    }
    assert(std::holds_alternative<Node::Punctuator>(*(nodes[0].expr)));

    assert(std::holds_alternative<Node::NumericLiteral>(*(nodes[1].expr)));
    assert(std::get<Node::NumericLiteral>(*(nodes[1].expr)).type == ConstituentToken::TYPE_FLOAT);

    assert(std::holds_alternative<Node::Operator>(*(nodes[2].expr)));

    assert(std::holds_alternative<Node::NumericLiteral>(*(nodes[3].expr)));
    assert(std::get<Node::NumericLiteral>(*(nodes[3].expr)).type == ConstituentToken::TYPE_INT);


    assert(std::holds_alternative<Node::Punctuator>(*(nodes[4].expr)));
    assert(std::holds_alternative<Node::Operator>(*(nodes[5].expr)));
    assert(std::holds_alternative<Node::NumericLiteral>(*(nodes[6].expr)));
    assert(std::get<Node::NumericLiteral>(*(nodes[6].expr)).type == ConstituentToken::TYPE_DOUBLE);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << RED << BAR << "\nTEST CASE FAILED: Expr Simple\n" << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << BAR << "\nTEST CASE PASSED: Expr Simple\n" << RESET << std::endl;
  return true;
}

bool TEST_ALL() { return (TEST_LEXER() && TEST_EXPR_SIMPLE()); }
