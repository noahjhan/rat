#ifndef LEXER_HPP
#define LEXER_HPP

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <regex>

#include "rat_source.hpp"
#include "token.hpp"

// for this file the main idea is this
// be able to prcoess each word in a given file
// convert each word to a token and return a dequeue of tokens

class Lexer
{
  public:
  Lexer(const RatSource &source_file);

  bool isAcceptableIdentifier(const char &ch);
  bool isAcceptableStringLiteral(const char &ch);
  bool isAcceptableNumericLiteral(const char &ch);
  bool isAcceptableCharLiteral(const char &ch);

  bool isAcceptableNumericSequence(const std::string &value);

  bool advanceToken();

  void advanceStringLiteral();
  void advanceCharLiteral();

  void dequePush(GenericToken type, const std::string &value,
                 const unsigned int &line_num, const unsigned int &col_num);

  void debugPrintln(const unsigned int &line_num);
  void debugLineCol(const unsigned int &line_num, const unsigned int &col_num);

  void debugPrinter(bool verbose = false);

  std::deque<Token> &getTokens();

  private:
  RatSource source_file_;
  std::deque<Token> tokens_;
  std::unordered_set<char> punctuators_;
  std::unordered_set<std::string> keywords_;
  std::unordered_set<std::string> operators_;
  std::unordered_set<std::string> types_;
  std::unordered_map<GenericToken, std::string> str_representation;
};

#endif // LEXER_HPP
