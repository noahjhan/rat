#ifndef TEST_HPP
#define TEST_HPP

#include <cassert>
#include <variant>

#include "../includes/assembly.hpp"
#include "../includes/lexer.hpp"
#include "../includes/parser.hpp"
#include "../includes/semantic.hpp"

bool TEST_LEXER();
bool TEST_EXPR_TYPES();
bool TEST_EXPR_AST();
bool TEST_VARIABLE_DECLARATION();
bool TEST_DISPATCH_SIMPLE();
bool TEST_FUNCTION_CALL();
bool TEST_COMPILE();
bool TEST_SEMANTIC();

bool TEST_ALL();

#endif // TEST_HPP
