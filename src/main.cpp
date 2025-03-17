#include <unordered_map>

#include "lexer.hpp"
#include "parser.hpp"
#include "rat_source.hpp"
#include "test.hpp"
// clang-format off
int main() { 
  return (!TEST_EXPR_AST()); 
}