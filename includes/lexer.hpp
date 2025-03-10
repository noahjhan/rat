#ifndef LEXER_HPP
#define LEXER_HPP

#include "rat_source.hpp"
#include "token.hpp"
#include <deque>
#include <unordered_map>
#include <unordered_set>

// for this file the main idea is this
// be able to prcoess each word in a given file
// convert each word to a token and return a dequeue of tokens

class Lexer
{
public:
  Lexer(const RatSource &source_file);

  bool extractTokenLiteral();
  std::string expectFunctionDeclaration();
  void expectVariableAssignment();
  void expectStringLiteral();
  TokenType stringToToken(const std::string &str);

  void tokenPush(TokenType t, std::string v);

  void debugPrinter();

private:
  RatSource source_file_;
  std::deque<Token> tokens_;
  std::unordered_map<std::string, TokenType> token_dict_;
  std::unordered_set<char> punctuators_;
};

// helper function

bool isAcceptableIdentifier(const char &ch);
bool isAcceptableStringLiteral(const char &ch);

#endif