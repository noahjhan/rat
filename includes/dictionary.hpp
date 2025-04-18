#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <string>
#include <unordered_map>

#include "token.hpp"

enum class Storage
{
    FLOAT,
    INT,
    UNSIGNED
};

const std::unordered_map<std::string, ConstituentToken> DICT = {
{"int", ConstituentToken::TYPE_INT},
{"float", ConstituentToken::TYPE_FLOAT},
{"double", ConstituentToken::TYPE_DOUBLE},
{"bool", ConstituentToken::TYPE_BOOL},
{"char", ConstituentToken::TYPE_CHAR},
{"long", ConstituentToken::TYPE_LONG},
{"short", ConstituentToken::TYPE_SHORT},
{"pointer", ConstituentToken::TYPE_POINTER},
{"uint", ConstituentToken::TYPE_UINT},
{"ulong", ConstituentToken::TYPE_ULONG},
{"ushort", ConstituentToken::TYPE_USHORT},
{"uchar", ConstituentToken::TYPE_UCHAR},
{"string", ConstituentToken::TYPE_STRING},
{"void", ConstituentToken::TYPE_VOID},
{"let", ConstituentToken::VARIABLE_DECLARATION_LET},
{"op", ConstituentToken::VARIABLE_DECLARATION_OP},
{"fn", ConstituentToken::FUNCTION_DECLARATION_F},
{"fn_", ConstituentToken::FUNCTION_DECLARATION_F_VOID},
{"fn?", ConstituentToken::FUNCTION_DECLARATION_F_OPTIONAL},
{"fn/", ConstituentToken::FUNCTION_DECLARATION_F_LAMBDA},
{"ret", ConstituentToken::FUNCTION_RETURN},
{"rev", ConstituentToken::FUNCTION_DECLARATION_F_VOID},
{"if", ConstituentToken::CONDITIONAL_IF},
{"else", ConstituentToken::CONDITIONAL_ELSE},
{"else if", ConstituentToken::CONDITIONAL_ELSE_IF},
{"match", ConstituentToken::PATTERN_MATCH},
{"null", ConstituentToken::NULL_LITERAL},
{":", ConstituentToken::COLON},
{"'", ConstituentToken::SINGLE_QUOTE},
{"\"", ConstituentToken::DOUBLE_QUOTE},
{",", ConstituentToken::COMMA},
{"[", ConstituentToken::BRACKET_OPEN},
{"]", ConstituentToken::BRACKET_CLOSE},
{"{", ConstituentToken::BRACE_OPEN},
{"}", ConstituentToken::BRACE_CLOSE},
{"(", ConstituentToken::PARENTHESES_OPEN},
{")", ConstituentToken::PARENTHESES_CLOSE},
{"//", ConstituentToken::COMMENT_LINE},
{"/*", ConstituentToken::COMMENT_BLOCK_OPEN},
{"*/", ConstituentToken::COMMENT_BLOCK_CLOSE},
{";", ConstituentToken::NEWLINE},
{"=", ConstituentToken::ASSIGNMENT},
{"+", ConstituentToken::ARITHMETIC_ADD},
{"-", ConstituentToken::ARITHMETIC_SUB},
{"*", ConstituentToken::ARITHMETIC_MUL},
{"/", ConstituentToken::ARITHMETIC_DIV},
{"%", ConstituentToken::ARITHMETIC_MOD},
{"==", ConstituentToken::COMPARISON_EQ},
{"!=", ConstituentToken::COMPARISON_NEQ},
{"<", ConstituentToken::COMPARISON_LT},
{">", ConstituentToken::COMPARISON_GT},
{"<=", ConstituentToken::COMPARISON_LTE},
{">=", ConstituentToken::COMPARISON_GTE},
{"&&", ConstituentToken::LOGICAL_AND},
{"||", ConstituentToken::LOGICAL_OR},
{"!", ConstituentToken::LOGICAL_NOT},
{"&", ConstituentToken::BITWISE_AND},
{"|", ConstituentToken::BITWISE_OR},
{"^", ConstituentToken::BITWISE_XOR},
{"~", ConstituentToken::BITWISE_NEG},
{"<<", ConstituentToken::BITWISE_SL},
{">>", ConstituentToken::BITWISE_SR},
{"->", ConstituentToken::ARROW}};

const std::unordered_map<ConstituentToken, std::string> REVERSE_DICT = {
{ConstituentToken::TYPE_INT, "int"},
{ConstituentToken::TYPE_FLOAT, "float"},
{ConstituentToken::TYPE_DOUBLE, "double"},
{ConstituentToken::TYPE_BOOL, "bool"},
{ConstituentToken::TYPE_CHAR, "char"},
{ConstituentToken::TYPE_LONG, "long"},
{ConstituentToken::TYPE_SHORT, "short"},
{ConstituentToken::TYPE_POINTER, "pointer"},
{ConstituentToken::TYPE_UINT, "uint"},
{ConstituentToken::TYPE_ULONG, "ulong"},
{ConstituentToken::TYPE_USHORT, "ushort"},
{ConstituentToken::TYPE_UCHAR, "uchar"},
{ConstituentToken::TYPE_STRING, "string"},
{ConstituentToken::TYPE_VOID, "void"},
{ConstituentToken::VARIABLE_DECLARATION_LET, "let"},
{ConstituentToken::VARIABLE_DECLARATION_OP, "op"},
{ConstituentToken::FUNCTION_DECLARATION_F, "fn"},
{ConstituentToken::FUNCTION_DECLARATION_F_VOID, "fn_"},
{ConstituentToken::FUNCTION_DECLARATION_F_OPTIONAL, "fn?"},
{ConstituentToken::FUNCTION_DECLARATION_F_LAMBDA, "fn/"},
{ConstituentToken::FUNCTION_RETURN, "ret"},
{ConstituentToken::CONDITIONAL_IF, "if"},
{ConstituentToken::CONDITIONAL_ELSE, "else"},
{ConstituentToken::CONDITIONAL_ELSE_IF, "else if"},
{ConstituentToken::PATTERN_MATCH, "match"},
{ConstituentToken::NULL_LITERAL, "null"},
{ConstituentToken::COLON, ":"},
{ConstituentToken::SINGLE_QUOTE, "'"},
{ConstituentToken::DOUBLE_QUOTE, "\""},
{ConstituentToken::COMMA, ","},
{ConstituentToken::BRACKET_OPEN, "["},
{ConstituentToken::BRACKET_CLOSE, "]"},
{ConstituentToken::BRACE_OPEN, "{"},
{ConstituentToken::BRACE_CLOSE, "}"},
{ConstituentToken::PARENTHESES_OPEN, "("},
{ConstituentToken::PARENTHESES_CLOSE, ")"},
{ConstituentToken::COMMENT_LINE, "//"},
{ConstituentToken::COMMENT_BLOCK_OPEN, "/*"},
{ConstituentToken::COMMENT_BLOCK_CLOSE, "*/"},
{ConstituentToken::NEWLINE, ";"},
{ConstituentToken::ASSIGNMENT, "="},
{ConstituentToken::ARITHMETIC_ADD, "+"},
{ConstituentToken::ARITHMETIC_SUB, "-"},
{ConstituentToken::ARITHMETIC_MUL, "*"},
{ConstituentToken::ARITHMETIC_DIV, "/"},
{ConstituentToken::ARITHMETIC_MOD, "%"},
{ConstituentToken::COMPARISON_EQ, "=="},
{ConstituentToken::COMPARISON_NEQ, "!="},
{ConstituentToken::COMPARISON_LT, "<"},
{ConstituentToken::COMPARISON_GT, ">"},
{ConstituentToken::COMPARISON_LTE, "<="},
{ConstituentToken::COMPARISON_GTE, ">="},
{ConstituentToken::LOGICAL_AND, "&&"},
{ConstituentToken::LOGICAL_OR, "||"},
{ConstituentToken::LOGICAL_NOT, "!"},
{ConstituentToken::BITWISE_AND, "&"},
{ConstituentToken::BITWISE_OR, "|"},
{ConstituentToken::BITWISE_XOR, "^"},
{ConstituentToken::BITWISE_NEG, "~"},
{ConstituentToken::BITWISE_SL, "<<"},
{ConstituentToken::BITWISE_SR, ">>"},
{ConstituentToken::ARROW, "->"}};

const std::unordered_map<ConstituentToken, std::string> TYPE_ASM = {
{ConstituentToken::TYPE_INT, "i32"},       {ConstituentToken::TYPE_FLOAT, "float"},
{ConstituentToken::TYPE_DOUBLE, "double"}, {ConstituentToken::TYPE_BOOL, "i1"},
{ConstituentToken::TYPE_CHAR, "i8"},       {ConstituentToken::TYPE_LONG, "i64"},
{ConstituentToken::TYPE_SHORT, "i16"},     {ConstituentToken::TYPE_POINTER, "ptr"},
{ConstituentToken::TYPE_UINT, "i32"},      {ConstituentToken::TYPE_ULONG, "i64"},
{ConstituentToken::TYPE_USHORT, "i16"},    {ConstituentToken::TYPE_UCHAR, "i8"},
{ConstituentToken::TYPE_VOID, "void"},     {ConstituentToken::TYPE_STRING, "ptr"},
{ConstituentToken::TYPE_MAIN, "i32"}};

const std::unordered_map<std::string, std::pair<ConstituentToken, ConstituentToken>>
REVERSE_TYPE_ASM = {
{"i32", {ConstituentToken::TYPE_INT, ConstituentToken::TYPE_UINT}},
{"i8", {ConstituentToken::TYPE_CHAR, ConstituentToken::TYPE_UCHAR}},
{"i16", {ConstituentToken::TYPE_SHORT, ConstituentToken::TYPE_USHORT}},
{"i64", {ConstituentToken::TYPE_LONG, ConstituentToken::TYPE_ULONG}},
{"i1", {ConstituentToken::TYPE_BOOL, ConstituentToken::TYPE_BOOL}},
{"ptr", {ConstituentToken::TYPE_STRING, ConstituentToken::TYPE_STRING}},
{"float", {ConstituentToken::TYPE_FLOAT, ConstituentToken::TYPE_FLOAT}},
{"double", {ConstituentToken::TYPE_DOUBLE, ConstituentToken::TYPE_DOUBLE}}};

// check aligns for void ?
const std::unordered_map<ConstituentToken, std::string> ALIGN_ASM = {
{ConstituentToken::TYPE_INT, "align 4"},    {ConstituentToken::TYPE_FLOAT, "align 4"},
{ConstituentToken::TYPE_DOUBLE, "align 8"}, {ConstituentToken::TYPE_BOOL, "align 1"},
{ConstituentToken::TYPE_CHAR, "align 1"},   {ConstituentToken::TYPE_LONG, "align 8"},
{ConstituentToken::TYPE_SHORT, "align 2"},  {ConstituentToken::TYPE_POINTER, "align 8"},
{ConstituentToken::TYPE_UINT, "align 4"},   {ConstituentToken::TYPE_ULONG, "align 8"},
{ConstituentToken::TYPE_USHORT, "align 2"}, {ConstituentToken::TYPE_UCHAR, "align 1"},
{ConstituentToken::TYPE_STRING, "align 8"}, {ConstituentToken::TYPE_MAIN, "align 4"}};

const std::unordered_map<std::string, std::string> STRING_TYPE_ALIGN = {
{"i32", "align 4"}, {"float", "align 4"}, {"double", "align 8"}, {"i8", "align 1"},
{"i64", "align 8"}, {"i16", "align 2"},   {"ptr", "align 8"}};

const std::unordered_map<std::string, std::pair<Storage, int>> SIZE_PRECEDENCE = {
{"i32", {Storage::INT, 3}},     {"i64", {Storage::INT, 4}},
{"i16", {Storage::INT, 2}},     {"i8", {Storage::INT, 1}},
{"float", {Storage::FLOAT, 1}}, {"double", {Storage::FLOAT, 2}},
};

#endif // DICTIONARY_HPP
