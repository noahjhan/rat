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
    // lex.debugPrinter(true /* use true here for verbose printing */);
    std::deque<Token> dq = lex.getTokens();
    auto parse = Parser(dq, rat);
    auto root_expr = parse.recurseExpr();
    // parse.debugASTPrinter(*root_expr);
    if (!root_expr)
    {
      throw std::runtime_error("Failed to parse the root expression");
    }

    auto &nodes = *root_expr;
    auto &root = *nodes.expr;
    assert(std::holds_alternative<Node::BinaryExpr>(root));
    auto &root_binary = std::get<Node::BinaryExpr>(root);
    assert(root_binary.op == ConstituentToken::ARITHMETIC_SUB);

    // Validate the left-hand side of the root node
    assert(std::holds_alternative<Node::BinaryExpr>(*root_binary.lhs->expr));
    auto &lhs_binary = std::get<Node::BinaryExpr>(*root_binary.lhs->expr);
    assert(lhs_binary.op == ConstituentToken::BITWISE_OR);

    // Validate the left operand of the OR operation
    assert(std::holds_alternative<Node::BinaryExpr>(*lhs_binary.lhs->expr));
    auto &lhs_binary_lhs = std::get<Node::BinaryExpr>(*lhs_binary.lhs->expr);
    assert(lhs_binary_lhs.op == ConstituentToken::ARITHMETIC_SUB);

    // Validate the left operand of the subtraction
    assert(std::holds_alternative<Node::BinaryExpr>(*lhs_binary_lhs.lhs->expr));
    auto &lhs_binary_lhs_lhs =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs.lhs->expr);
    assert(lhs_binary_lhs_lhs.op == ConstituentToken::BITWISE_XOR);

    // Validate the left operand of the XOR operation
    assert(
    std::holds_alternative<Node::UnaryExpr>(*lhs_binary_lhs_lhs.lhs->expr));
    auto &lhs_binary_lhs_lhs_lhs =
    std::get<Node::UnaryExpr>(*lhs_binary_lhs_lhs.lhs->expr);
    assert(lhs_binary_lhs_lhs_lhs.op == ConstituentToken::LOGICAL_NOT);

    // Validate the inner part of the logical NOT
    assert(std::holds_alternative<Node::BinaryExpr>(
    *lhs_binary_lhs_lhs_lhs.expr->expr));
    auto &lhs_binary_lhs_lhs_lhs_expr =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs_lhs_lhs.expr->expr);
    assert(lhs_binary_lhs_lhs_lhs_expr.op == ConstituentToken::BITWISE_OR);

    // Validate the left operand of the OR operation
    assert(std::holds_alternative<Node::BinaryExpr>(
    *lhs_binary_lhs_lhs_lhs_expr.lhs->expr));
    auto &lhs_binary_lhs_lhs_lhs_expr_lhs =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs_lhs_lhs_expr.lhs->expr);
    assert(lhs_binary_lhs_lhs_lhs_expr_lhs.op == ConstituentToken::BITWISE_AND);

    // Validate the left operand of the AND operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_lhs_expr_lhs.lhs->expr));
    auto &lhs_binary_lhs_lhs_lhs_expr_lhs_lhs =
    std::get<Node::NumericLiteral>(*lhs_binary_lhs_lhs_lhs_expr_lhs.lhs->expr);
    assert(lhs_binary_lhs_lhs_lhs_expr_lhs_lhs.token.value == "1");

    // Validate the right operand of the AND operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_lhs_expr_lhs.rhs->expr));
    auto &lhs_binary_lhs_lhs_lhs_expr_lhs_rhs =
    std::get<Node::NumericLiteral>(*lhs_binary_lhs_lhs_lhs_expr_lhs.rhs->expr);
    assert(lhs_binary_lhs_lhs_lhs_expr_lhs_rhs.token.value == "2");

    // Validate the right operand of the OR operation
    assert(std::holds_alternative<Node::UnaryExpr>(
    *lhs_binary_lhs_lhs_lhs_expr.rhs->expr));
    auto &lhs_binary_lhs_lhs_lhs_expr_rhs =
    std::get<Node::UnaryExpr>(*lhs_binary_lhs_lhs_lhs_expr.rhs->expr);
    assert(lhs_binary_lhs_lhs_lhs_expr_rhs.op == ConstituentToken::LOGICAL_NOT);

    // Validate the inner part of the logical NOT
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_lhs_expr_rhs.expr->expr));
    auto &lhs_binary_lhs_lhs_lhs_expr_rhs_expr =
    std::get<Node::NumericLiteral>(*lhs_binary_lhs_lhs_lhs_expr_rhs.expr->expr);
    assert(lhs_binary_lhs_lhs_lhs_expr_rhs_expr.token.value == "3");

    // Validate the right operand of the XOR operation
    assert(
    std::holds_alternative<Node::BinaryExpr>(*lhs_binary_lhs_lhs.rhs->expr));
    auto &lhs_binary_lhs_lhs_rhs =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs_lhs.rhs->expr);
    assert(lhs_binary_lhs_lhs_rhs.op == ConstituentToken::ARITHMETIC_ADD);

    // Validate the left operand of the addition
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_rhs.lhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_lhs =
    std::get<Node::NumericLiteral>(*lhs_binary_lhs_lhs_rhs.lhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_lhs.token.value == "4");

    // Validate the right operand of the addition
    assert(std::holds_alternative<Node::BinaryExpr>(
    *lhs_binary_lhs_lhs_rhs.rhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_rhs =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs_lhs_rhs.rhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_rhs.op == ConstituentToken::ARITHMETIC_DIV);

    // Validate the left operand of the division
    assert(std::holds_alternative<Node::BinaryExpr>(
    *lhs_binary_lhs_lhs_rhs_rhs.lhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_rhs_lhs =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs_lhs_rhs_rhs.lhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_rhs_lhs.op ==
           ConstituentToken::ARITHMETIC_MUL);

    // Validate the left operand of the multiplication
    assert(std::holds_alternative<Node::BinaryExpr>(
    *lhs_binary_lhs_lhs_rhs_rhs_lhs.lhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs_lhs_rhs_rhs_lhs.lhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs.op ==
           ConstituentToken::ARITHMETIC_SUB);

    // Validate the left operand of the subtraction
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs.lhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs_lhs =
    std::get<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs.lhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs_lhs.token.value == "5");

    // Validate the right operand of the subtraction
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs.rhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs_rhs =
    std::get<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs.rhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_rhs_lhs_lhs_rhs.token.value == "6");

    // Validate the right operand of the multiplication
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_rhs_rhs_lhs.rhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_rhs_lhs_rhs =
    std::get<Node::NumericLiteral>(*lhs_binary_lhs_lhs_rhs_rhs_lhs.rhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_rhs_lhs_rhs.token.value == "7");

    // Validate the right operand of the division
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_lhs_lhs_rhs_rhs.rhs->expr));
    auto &lhs_binary_lhs_lhs_rhs_rhs_rhs =
    std::get<Node::NumericLiteral>(*lhs_binary_lhs_lhs_rhs_rhs.rhs->expr);
    assert(lhs_binary_lhs_lhs_rhs_rhs_rhs.token.value == "8");

    // Validate the right operand of the subtraction
    assert(std::holds_alternative<Node::BinaryExpr>(*lhs_binary_lhs.rhs->expr));
    auto &lhs_binary_rhs =
    std::get<Node::BinaryExpr>(*lhs_binary_lhs.rhs->expr);
    assert(lhs_binary_rhs.op == ConstituentToken::BITWISE_AND);

    // Validate the left operand of the AND operation
    assert(
    std::holds_alternative<Node::NumericLiteral>(*lhs_binary_rhs.lhs->expr));
    auto &lhs_binary_rhs_lhs =
    std::get<Node::NumericLiteral>(*lhs_binary_rhs.lhs->expr);
    assert(lhs_binary_rhs_lhs.token.value == "9");

    // Validate the right operand of the AND operation
    assert(std::holds_alternative<Node::UnaryExpr>(*lhs_binary_rhs.rhs->expr));
    auto &lhs_binary_rhs_rhs =
    std::get<Node::UnaryExpr>(*lhs_binary_rhs.rhs->expr);
    assert(lhs_binary_rhs_rhs.op == ConstituentToken::BITWISE_NEG);

    // Validate the inner part of the bitwise negation
    assert(
    std::holds_alternative<Node::BinaryExpr>(*lhs_binary_rhs_rhs.expr->expr));
    auto &lhs_binary_rhs_rhs_expr =
    std::get<Node::BinaryExpr>(*lhs_binary_rhs_rhs.expr->expr);
    assert(lhs_binary_rhs_rhs_expr.op == ConstituentToken::COMPARISON_LT);

    // Validate the left operand of the less than comparison
    assert(std::holds_alternative<Node::BinaryExpr>(
    *lhs_binary_rhs_rhs_expr.lhs->expr));
    auto &lhs_binary_rhs_rhs_expr_lhs =
    std::get<Node::BinaryExpr>(*lhs_binary_rhs_rhs_expr.lhs->expr);
    assert(lhs_binary_rhs_rhs_expr_lhs.op == ConstituentToken::BITWISE_OR);

    // Validate the left operand of the OR operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_rhs_rhs_expr_lhs.lhs->expr));
    auto &lhs_binary_rhs_rhs_expr_lhs_lhs =
    std::get<Node::NumericLiteral>(*lhs_binary_rhs_rhs_expr_lhs.lhs->expr);
    assert(lhs_binary_rhs_rhs_expr_lhs_lhs.token.value == "10");

    // Validate the right operand of the OR operation
    assert(std::holds_alternative<Node::BinaryExpr>(
    *lhs_binary_rhs_rhs_expr_lhs.rhs->expr));
    auto &lhs_binary_rhs_rhs_expr_lhs_rhs =
    std::get<Node::BinaryExpr>(*lhs_binary_rhs_rhs_expr_lhs.rhs->expr);
    assert(lhs_binary_rhs_rhs_expr_lhs_rhs.op == ConstituentToken::BITWISE_XOR);

    // Validate the left operand of the XOR operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_rhs_rhs_expr_lhs_rhs.lhs->expr));
    auto &lhs_binary_rhs_rhs_expr_lhs_rhs_lhs =
    std::get<Node::NumericLiteral>(*lhs_binary_rhs_rhs_expr_lhs_rhs.lhs->expr);
    assert(lhs_binary_rhs_rhs_expr_lhs_rhs_lhs.token.value == "11");

    // Validate the right operand of the XOR operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_rhs_rhs_expr_lhs_rhs.rhs->expr));
    auto &lhs_binary_rhs_rhs_expr_lhs_rhs_rhs =
    std::get<Node::NumericLiteral>(*lhs_binary_rhs_rhs_expr_lhs_rhs.rhs->expr);
    assert(lhs_binary_rhs_rhs_expr_lhs_rhs_rhs.token.value == "12");

    // Validate the right operand of the less than comparison
    assert(std::holds_alternative<Node::NumericLiteral>(
    *lhs_binary_rhs_rhs_expr.rhs->expr));
    auto &lhs_binary_rhs_rhs_expr_rhs =
    std::get<Node::NumericLiteral>(*lhs_binary_rhs_rhs_expr.rhs->expr);
    assert(lhs_binary_rhs_rhs_expr_rhs.token.value == "13");

    // Validate the right-hand side of the root node
    assert(std::holds_alternative<Node::BinaryExpr>(*root_binary.rhs->expr));
    auto &rhs_binary = std::get<Node::BinaryExpr>(*root_binary.rhs->expr);
    assert(rhs_binary.op == ConstituentToken::BITWISE_OR);

    // Validate the left operand of the OR operation
    assert(std::holds_alternative<Node::BinaryExpr>(*rhs_binary.lhs->expr));
    auto &rhs_binary_lhs = std::get<Node::BinaryExpr>(*rhs_binary.lhs->expr);
    assert(rhs_binary_lhs.op == ConstituentToken::ARITHMETIC_MUL);

    // Validate the left operand of the multiplication
    assert(
    std::holds_alternative<Node::NumericLiteral>(*rhs_binary_lhs.lhs->expr));
    auto &rhs_binary_lhs_lhs =
    std::get<Node::NumericLiteral>(*rhs_binary_lhs.lhs->expr);
    assert(rhs_binary_lhs_lhs.token.value == "14");

    // Validate the right operand of the multiplication
    assert(std::holds_alternative<Node::BinaryExpr>(*rhs_binary_lhs.rhs->expr));
    auto &rhs_binary_lhs_rhs =
    std::get<Node::BinaryExpr>(*rhs_binary_lhs.rhs->expr);
    assert(rhs_binary_lhs_rhs.op == ConstituentToken::ARITHMETIC_DIV);

    // Validate the left operand of the division
    assert(std::holds_alternative<Node::NumericLiteral>(
    *rhs_binary_lhs_rhs.lhs->expr));
    auto &rhs_binary_lhs_rhs_lhs =
    std::get<Node::NumericLiteral>(*rhs_binary_lhs_rhs.lhs->expr);
    assert(rhs_binary_lhs_rhs_lhs.token.value == "15");

    // Validate the right operand of the division
    assert(
    std::holds_alternative<Node::BinaryExpr>(*rhs_binary_lhs_rhs.rhs->expr));
    auto &rhs_binary_lhs_rhs_rhs =
    std::get<Node::BinaryExpr>(*rhs_binary_lhs_rhs.rhs->expr);
    assert(rhs_binary_lhs_rhs_rhs.op == ConstituentToken::ARITHMETIC_MOD);

    // Validate the left operand of the modulo operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *rhs_binary_lhs_rhs_rhs.lhs->expr));
    auto &rhs_binary_lhs_rhs_rhs_lhs =
    std::get<Node::NumericLiteral>(*rhs_binary_lhs_rhs_rhs.lhs->expr);
    assert(rhs_binary_lhs_rhs_rhs_lhs.token.value == "16");

    // Validate the right operand of the modulo operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *rhs_binary_lhs_rhs_rhs.rhs->expr));
    auto &rhs_binary_lhs_rhs_rhs_rhs =
    std::get<Node::NumericLiteral>(*rhs_binary_lhs_rhs_rhs.rhs->expr);
    assert(rhs_binary_lhs_rhs_rhs_rhs.token.value == "17");

    // Validate the right operand of the OR operation
    assert(std::holds_alternative<Node::BinaryExpr>(*rhs_binary.rhs->expr));
    auto &rhs_binary_rhs = std::get<Node::BinaryExpr>(*rhs_binary.rhs->expr);
    assert(rhs_binary_rhs.op == ConstituentToken::ARITHMETIC_SUB);

    // Validate the left operand of the subtraction
    assert(
    std::holds_alternative<Node::NumericLiteral>(*rhs_binary_rhs.lhs->expr));
    auto &rhs_binary_rhs_lhs =
    std::get<Node::NumericLiteral>(*rhs_binary_rhs.lhs->expr);
    assert(rhs_binary_rhs_lhs.token.value == "18");

    // Validate the right operand of the subtraction
    assert(std::holds_alternative<Node::BinaryExpr>(*rhs_binary_rhs.rhs->expr));
    auto &rhs_binary_rhs_rhs =
    std::get<Node::BinaryExpr>(*rhs_binary_rhs.rhs->expr);
    assert(rhs_binary_rhs_rhs.op == ConstituentToken::BITWISE_XOR);

    // Validate the left operand of the XOR operation
    assert(std::holds_alternative<Node::NumericLiteral>(
    *rhs_binary_rhs_rhs.lhs->expr));
    auto &rhs_binary_rhs_rhs_lhs =
    std::get<Node::NumericLiteral>(*rhs_binary_rhs_rhs.lhs->expr);
    assert(rhs_binary_rhs_rhs_lhs.token.value == "19");

    // Validate the right operand of the XOR operation
    assert(
    std::holds_alternative<Node::BinaryExpr>(*rhs_binary_rhs_rhs.rhs->expr));
    auto &rhs_binary_rhs_rhs_rhs =
    std::get<Node::BinaryExpr>(*rhs_binary_rhs_rhs.rhs->expr);
    assert(rhs_binary_rhs_rhs_rhs.op == ConstituentToken::COMPARISON_GTE);

    // Validate the left operand of the comparison
    assert(std::holds_alternative<Node::NumericLiteral>(
    *rhs_binary_rhs_rhs_rhs.lhs->expr));
    auto &rhs_binary_rhs_rhs_rhs_lhs =
    std::get<Node::NumericLiteral>(*rhs_binary_rhs_rhs_rhs.lhs->expr);
    assert(rhs_binary_rhs_rhs_rhs_lhs.token.value == "20");

    // Validate the right operand of the comparison
    assert(std::holds_alternative<Node::NumericLiteral>(
    *rhs_binary_rhs_rhs_rhs.rhs->expr));
    auto &rhs_binary_rhs_rhs_rhs_rhs =
    std::get<Node::NumericLiteral>(*rhs_binary_rhs_rhs_rhs.rhs->expr);
    assert(rhs_binary_rhs_rhs_rhs_rhs.token.value == "21");
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