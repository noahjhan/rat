#ifndef TEST_HPP
#define TEST_HPP

#include <cassert>
#include <variant>

#include "lexer.hpp"
#include "parser.hpp"

bool TEST_LEXER();

bool TEST_EXPR_SIMPLE();

bool TEST_EXPR_AST();

bool TEST_ALL();

#endif // TEST_HPP
