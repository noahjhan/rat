#include <string>

enum class TokenGeneric
{
    IDENTIFIER,
    KEYWORD,
    LITERAL,
    PUNCTUATOR,
    OPERATOR
};

enum class TokenType
{
    // identifier
    VARIABLE,
    FUNCTION,
    MODULE,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_LONG,
    TYPE_SHORT,
    TYPE_VOID,
    TYPE_ANY,
    TYPE_POINTER,
    TYPE_UINT,
    TYPE_ULONG,
    TYPE_USHORT,
    TYPE_UCHAR,

    // keyword
    VARIABLE_DECLARATION_LET,
    VARIABLE_DECLARATION_OPLET,    // optional let
    FUNCTION_DECLARATION_F,        // f
    FUNCTION_DECLARATION_F_VOID,   // f_
    FUNCTION_DECLARATION_F_PURE,   // f!
    FUNCTION_DECLARATION_F_LAMBDA, // f/
    FUNCTION_RETURN,
    CONDITIONAL_IF,
    CONDITIONAL_ELSE_IF,
    CONDITIONAL_ELSE,
    PATTERN_MATCH,

    // literals
    NUMERIC,
    STRING,
    BOOLEAN,
    NULL_LITERAL,

    // punctuators
    SEMICOLON,
    COLON,
    EXCLAMATION,
    QUOTE,
    DOUBLE_QUOTE_OPEN,
    DOUBLE_QUOTE_CLOSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    BRACE_OPEN,
    BRACE_CLOSE,
    PARENTHESES_OPEN,
    PARENTHESES_CLOSE,
    COMMENT_LINE,
    COMMENT_BLOCK_OPEN,
    COMMENT_BLOCK_CLOSE,

    // operators
    ARITHMETHIC_ADD,
    ARITHMETHIC_SUB,
    ARITHMETHIC_MUL,
    ARITHMETHIC_DIV,
    ARITHMETHIC_MOD,
    COMPARISON_EQ,
    COMPARISON_NEQ,
    COMPARISON_LT,
    COMPARISON_GT,
    COMPARISON_LTE,
    COMPARISON_GTE,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    ASSIGNMENT_EQUALS,
    ASSIGNMENT_INCREMENT,
    ASSIGNMENT_DECREMENT,
    ASSIGNMENT_MUL,
    ASSIGNMENT_DIV,
    ASSIGNMENT_MOD,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_NEG,
    BITWISE_SL,
    BITWISE_SR,
    ARROW,
    INCREMENT,
    DECREMENT
};

struct Token
{
    TokenGeneric generic;
    TokenType type;
    std::string value;
    unsigned int line_num;
    unsigned int col_num;

    Token(TokenGeneric g, TokenType t, std::string v, unsigned int line, unsigned int col)
        : generic(g), type(t), value(std::move(v)), line_num(line), col_num(col) {}
};
