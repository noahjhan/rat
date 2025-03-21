#ifndef TEST_HPP
#define TEST_HPP

#include <cassert>
#include <variant>

#include "assembly.hpp"
#include "lexer.hpp"
#include "parser.hpp"

bool TEST_LEXER();
bool TEST_EXPR_TYPES();
bool TEST_EXPR_AST();
bool TEST_VARIABLE_DECLARATION();
bool TEST_DISPATCH_SIMPLE();
bool TEST_FUNCTION_CALL();
bool TEST_COMPILE();

bool TEST_ALL();

#endif // TEST_HPP
