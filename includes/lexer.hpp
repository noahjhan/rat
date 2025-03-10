#include <deque>

#include "rat_source.hpp"
#include "token.hpp"

// for this file the main idea is this
// be able to prcoess each word in a given file
// convert each word to a token and return a dequeue of tokens

class Lexer
{
public:
private:
    RatSource source_file_;
    std::deque<Token> tokens_;
};