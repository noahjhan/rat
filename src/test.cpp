#include "test.hpp"

#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
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
    lex.debugPrinter(true /* use true here for verbose printing */);
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
bool TEST_EXPR_TYPES()
{
  std::cout << PURPLE << "TEST CASE: Expr Types\n" << BAR << RESET << std::endl;
  try
  {
    std::string file_name = "data/expression_types.rat";
    auto rat = RatSource(file_name);
    auto lex = Lexer(rat);
    while (lex.advanceToken())
    {
    }
    lex.debugPrinter(true /* use true here for verbose printing */);
    std::deque<Token> dq = lex.getTokens();
    auto parse = Parser(dq, rat);
    auto root_expr = parse.recurseExpr();
    parse.debugASTPrinter(*root_expr);
    if (!root_expr)
    {
      throw std::runtime_error("Failed to parse the root expression");
    }

    auto &nodes = *root_expr;
    auto &root = *nodes.expr;

    assert(std::holds_alternative<Node::BinaryExpr>(root));
    auto &root_binary = std::get<Node::BinaryExpr>(root);
    assert(root_binary.op == ConstituentToken::ARITHMETIC_MUL);

    assert(std::holds_alternative<Node::BinaryExpr>(*root_binary.lhs->expr));
    auto &lhs_binary = std::get<Node::BinaryExpr>(*root_binary.lhs->expr);
    assert(lhs_binary.op == ConstituentToken::ARITHMETIC_ADD);

    assert(std::holds_alternative<Node::NumericLiteral>(*lhs_binary.lhs->expr));
    auto &lhs_numeric = std::get<Node::NumericLiteral>(*lhs_binary.lhs->expr);
    assert(lhs_numeric.type == ConstituentToken::TYPE_FLOAT);

    assert(std::holds_alternative<Node::NumericLiteral>(*lhs_binary.rhs->expr));
    auto &lhs_rhs_numeric =
    std::get<Node::NumericLiteral>(*lhs_binary.rhs->expr);
    assert(lhs_rhs_numeric.type == ConstituentToken::TYPE_INT);

    assert(
    std::holds_alternative<Node::NumericLiteral>(*root_binary.rhs->expr));
    auto &root_binary_rhs_numeric =
    std::get<Node::NumericLiteral>(*root_binary.rhs->expr);
    assert(root_binary_rhs_numeric.type == ConstituentToken::TYPE_DOUBLE);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << RED << BAR << "\nTEST CASE FAILED: Expr Types\n"
              << RESET << std::endl;
    return false;
  }
  std::cout << GREEN << BAR << "\nTEST CASE PASSED: Expr Types\n"
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
    lex.debugPrinter(true /* use true here for verbose printing */);
    std::deque<Token> dq = lex.getTokens();
    auto parse = Parser(dq, rat);
    auto root_expr = parse.recurseExpr();
    parse.debugASTPrinter(*root_expr);

    auto &nodes = *root_expr;
    auto &root = *nodes.expr;
    
    /// @todo write out full expression
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
  int totalTests = 3; // change per test
  int failedTests = 0;

  if (!TEST_LEXER())
  {
    failedTests++;
  }
  if (!TEST_EXPR_TYPES())
  {
    failedTests++;
  }
  if (!TEST_EXPR_AST())
  {
    failedTests++;
  }

  if (failedTests > 0)
  {
    std::cout << RED << "TOTAL TESTS: " << totalTests << std::endl;
    std::cout << BAR << "\n"
              << failedTests << " test(s) failed!" << "\n"
              << BAR << RESET << std::endl;
  }
  else
  {
    std::cout << GREEN << "TOTAL TESTS: " << totalTests << std::endl;
    std::cout << BAR << "\nAll tests passed!\n" << BAR << RESET << std::endl;
  }

  return (failedTests == 0);
}
