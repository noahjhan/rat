#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
enum class GenericToken
{
    /// generic
    IDENTIFIER, // user-input [a-zA-Z0-9_]
    KEYWORD,
    NUMERIC_LITERAL, // expect a lot of conversions
    STRING_LITERAL,
    CHAR_LITERAL,
    PUNCTUATOR,
    OPERATOR,
    TYPE
};

enum class ConstituentToken
{
    IDENTIFIER,
    KEYWORD,
    LITERAL,
    PUNCTUATOR,
    OPERATOR,
    // identifier
    VARIABLE_ID,
    FUNCTION_ID,
    // MODULE,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_LONG,
    TYPE_SHORT,
    TYPE_POINTER,
    TYPE_UINT,
    TYPE_ULONG,
    TYPE_USHORT,
    TYPE_UCHAR,
    TYPE_STRING,

    TYPE_OP_INT,
    TYPE_OP_FLOAT,
    TYPE_OP_DOUBLE,
    TYPE_OP_BOOL,
    TYPE_OP_CHAR,
    TYPE_OP_LONG,
    TYPE_OP_SHORT,
    TYPE_OP_UINT,
    TYPE_OP_ULONG,
    TYPE_OP_USHORT,
    TYPE_OP_UCHAR,
    TYPE_OP_STRING,

    TYPE_VOID,
    TYPE_MAIN,

    NULL_LITERAL,

    // keyword
    VARIABLE_DECLARATION_LET,
    VARIABLE_DECLARATION_OP,         // optional let
    FUNCTION_DECLARATION_F,          // f
    FUNCTION_DECLARATION_F_VOID,     // f_
    FUNCTION_DECLARATION_F_OPTIONAL, // f?
    FUNCTION_DECLARATION_F_LAMBDA,   // f/
    FUNCTION_RETURN,
    FUNCTION_RETURN_VOID,
    CONDITIONAL_IF,
    CONDITIONAL_ELSE,
    CONDITIONAL_ELSE_IF,
    PATTERN_MATCH,

    /*
    // literals
    NUMERIC_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    CHAR_LITERAL
    */

    // punctuators
    COLON,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    COMMA,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    BRACE_OPEN,
    BRACE_CLOSE,
    PARENTHESES_OPEN,
    PARENTHESES_CLOSE,
    COMMENT_LINE,
    COMMENT_BLOCK_OPEN,
    COMMENT_BLOCK_CLOSE,
    NEWLINE,

    // operators
    ASSIGNMENT,
    ARITHMETIC_ADD,
    ARITHMETIC_SUB,
    ARITHMETIC_MUL,
    ARITHMETIC_DIV,
    ARITHMETIC_MOD,
    COMPARISON_EQ,
    COMPARISON_NEQ,
    COMPARISON_LT,
    COMPARISON_GT,
    COMPARISON_LTE,
    COMPARISON_GTE,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_NEG,
    BITWISE_SL,
    BITWISE_SR,
    ARROW,
    FAT_ARROW
};
struct Token
{
    // Token(GenericToken type, std::string value, unsigned int line_num, unsigned
    // int col_num) : type_(type), value_(std::move(value)), line_num_(line_num),
    // col_num_(col_num)
    // {
    // }

    GenericToken type;
    std::string value;
    unsigned int line_num;
    unsigned int col_num;
};
#endif // TOKEN_HPP
