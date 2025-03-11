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

  bool advanceToken();

  void advanceStringLiteral();
  void advanceCharLiteral();
  void advanceNumericLiteral();
  void advanceIdentifier();

  void dequePush(GenericToken type, const std::string &value);
  void debugPrinter();

private:
  RatSource source_file_;
  std::deque<Token> tokens_;
  std::unordered_set<char> punctuators_;
  std::unordered_set<std::string> keywords_;
  std::unordered_set<std::string> operators_;
};

// helper functions
bool isAcceptableIdentifier(const char &ch);
bool isAcceptableStringLiteral(const char &ch);
bool isAcceptableNumericLiteral(const char &ch);

#endif `