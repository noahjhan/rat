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
    std::cerr << RED << BAR << "\nTEST CASE FAILED: Lexer\n"
              << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << BAR << "\nTEST CASE PASSED: Lexer\n"
            << RESET << std::endl;
  return true;
}

bool TEST_EXPR_SIMPLE()
{
  std::cout << PURPLE << "TEST CASE: Expr Simple\n"
            << BAR << RESET << std::endl;
  try
  {
    std::string file_name = "data/simple_expression.rat";
    auto rat = RatSource(file_name);
    auto lex = Lexer(rat);
    while (lex.advanceToken())
    {
    }
    std::deque<Token> dq = lex.getTokens();
    auto parse = Parser(dq);
    std::vector<Node::GenericExpr> nodes;
    while (parse.numTokens())
    {
      auto node = parse.tokenToExpr();
      nodes.push_back(node ? std::move(*(node)) : Node::GenericExpr());
    }

    if (nodes.size() > 1 && nodes[1].expr)
    {
      assert(std::holds_alternative<Node::NumericLiteral>(*(nodes[1].expr)));
      assert(std::get<Node::NumericLiteral>(*(nodes[1].expr)).type ==
             ConstituentToken::TYPE_FLOAT);
    }

    if (nodes.size() > 3 && nodes[3].expr)
    {
      assert(std::holds_alternative<Node::NumericLiteral>(*(nodes[3].expr)));
      assert(std::get<Node::NumericLiteral>(*(nodes[3].expr)).type ==
             ConstituentToken::TYPE_INT);
    }

    if (nodes.size() > 6 && nodes[6].expr)
    {
      assert(std::holds_alternative<Node::NumericLiteral>(*(nodes[6].expr)));
      assert(std::get<Node::NumericLiteral>(*(nodes[6].expr)).type ==
             ConstituentToken::TYPE_DOUBLE);
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << RED << BAR << "\nTEST CASE FAILED: Expr Simple\n"
              << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << BAR << "\nTEST CASE PASSED: Expr Simple\n"
            << RESET << std::endl;
  return true;
}

bool TEST_EXPR_AST()
{
  std::cout << PURPLE << "TEST CASE: Expr AST\n" << BAR << RESET << std::endl;
  try
  {

    std::string file_name = "data/expression_ast.rat";
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
      auto expr = parse.recurseExpr(); // Get the unique_ptr
      nodes.push_back(expr ? std::move(*expr) : Node::GenericExpr());
    }
    parse.debugASTPrinter(nodes);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << RED << BAR << "\nTEST CASE FAILED: Expr AST\n"
              << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << BAR << "\nTEST CASE PASSED: Expr AST\n"
            << RESET << std::endl;
  return true;
}
bool TEST_ALL()
{
  return (TEST_LEXER() && TEST_EXPR_SIMPLE()) && TEST_EXPR_AST();
}
