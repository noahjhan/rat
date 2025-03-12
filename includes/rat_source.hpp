// file stream abstraction for rat files

#ifndef RAT_SOURCE_HPP
#define RAT_SOURCE_HPP

#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class RatSource
{
public:
  RatSource(const std::string &filename);
  ~RatSource();
  RatSource(const RatSource &other);
  RatSource &operator=(const RatSource &other);
  void destructor();

  void seek_reset();
  std::string readLine();
  std::string readWord();

  char advanceChar();
  char peekChar();
  void reverse();
  void advanceWhitespace();
  void selectLine(const unsigned &i);
  void selectCol(const unsigned &i);

  unsigned int getLineNum();
  unsigned int getColNum();

  void debugPrinter();

private:
  std::string filename_;
  std::fstream fs_;
  unsigned int line_num;
  unsigned int col_num;
};
#endif // RAT_SOURCE_HPP