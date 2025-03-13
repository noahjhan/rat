#ifndef RAT_SOURCE_HPP
#define RAT_SOURCE_HPP

#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <unordered_map>

#define RESET_LINE            \
  do                          \
  {                           \
    prev_line_num = line_num; \
    line_num = 1;             \
  } while (0)
#define RESET_COL           \
  do                        \
  {                         \
    prev_col_num = col_num; \
    col_num = 1;            \
  } while (0)
#define NEXT_LINE             \
  do                          \
  {                           \
    prev_line_num = line_num; \
    ++line_num;               \
  } while (0)
#define NEXT_COL            \
  do                        \
  {                         \
    prev_col_num = col_num; \
    ++col_num;              \
  } while (0)
#define PREV_LINE (line_num = prev_line_num)
#define PREV_COL (col_num = prev_col_num)

class RatSource
{
  public:
  RatSource(const std::string &filename);
  ~RatSource();
  RatSource(const RatSource &other);
  RatSource &operator=(const RatSource &other);
  void destructor();

  void seekReset();
  std::string readLine();
  std::string readWord();

  char advanceChar();
  char peekChar();
  void reverse();
  void advanceWhitespace();
  void seekLine(const unsigned &i);
  void seekCol(const unsigned &i);

  unsigned int getLineNum();
  unsigned int getColNum();

  void debugPrinter();

  private:
  std::string filename_;
  std::fstream fs_;
  unsigned int line_num;
  unsigned int col_num;
  unsigned int prev_line_num;
  unsigned int prev_col_num;
};
#endif // RAT_SOURCE_HPP