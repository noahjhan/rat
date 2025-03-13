#include "rat_source.hpp"

/**
 *
 * @todo: bug in seek_reset: possibility for inf loop
 *
 */

RatSource::RatSource(const std::string &filename) : filename_(filename)
{
  fs_.open(filename_);
  if (!fs_.is_open())
  {
    throw std::invalid_argument("invalid file");
  }
  line_num = 1;
  col_num = 1;
  prev_line_num = 1;
  prev_col_num = 1;
}

RatSource::~RatSource() { fs_.close(); }

RatSource::RatSource(const RatSource &other)
{
  if (this != &other)
  {
    fs_.close();
    this->filename_ = other.filename_;
    this->fs_.open(this->filename_);
    this->line_num = other.line_num;
    this->col_num = other.col_num;
  }
}

RatSource &RatSource::operator=(const RatSource &other)
{
  if (this != &other)
  {
    fs_.close();
    this->filename_ = other.filename_;
    this->fs_.open(this->filename_);
    this->line_num = other.line_num;
    this->col_num = other.col_num;
  }
  return *this;
}

void RatSource::destructor() { fs_.close(); }

void RatSource::seekReset()
{
  fs_.close();
  fs_.open(filename_);
  if (!fs_.is_open())
  {
    throw std::invalid_argument("invalid file");
  }
  RESET_LINE;
  RESET_COL;
}

std::string RatSource::readLine()
{
  RESET_COL;
  std::string line;
  if (std::getline(fs_, line))
  {
    NEXT_LINE;
    return line;
  }
  return "";
}
/// @todo validate line resets
std::string RatSource::readWord()
{
  std::string word;
  char ch;

  while (fs_.get(ch) && std::isspace(ch))
  {
    NEXT_COL;
    if (ch == '\n')
    {
      RESET_COL;
      NEXT_LINE;
    }
  }

  if (!fs_.good())
  {
    return "";
  }

  do
  {
    word.push_back(ch);
    NEXT_COL;
  } while (fs_.get(ch) && !std::isspace(ch));

  return word;
}

char RatSource::advanceChar()
{
  char ch;
  if (fs_.get(ch))
  {
    NEXT_COL;
    if (ch == '\n')
    {
      RESET_COL;
      NEXT_LINE;
    }
    return ch;
  }
  return EOF;
}

char RatSource::peekChar() { return fs_.peek(); }

/**
 * @todo handle begin of file unget error for some reason the commented BOF error check breaks the lexing process
 * there is never an uncaught exception, so inside the if is never reached
 * calling tellg() causes issues
 */

void RatSource::reverse()
{
  /**
   *  @todo for some reason the commented BOF error check breaks the lexing process
   *  ...however there is no uncaught exception?
   *  merely calling tellg() causes issues
   */

  // if (fs_.tellg() == 0)
  // {
  //   throw std::invalid_argument("error: cannot reverse beginning of file");
  // }

  if (!fs_.good())
  {
    throw std::invalid_argument("error: cannot reverse end of file");
  }
  fs_.unget();
  if (fs_.peek() == '\n')
  {
    PREV_LINE;
  }
  PREV_COL;
}

void RatSource::advanceWhitespace()
{
  char ch;
  if (!std::isspace(fs_.peek()))
  {
    return;
  }
  while (fs_.get(ch))
  {
    NEXT_COL;
    if (ch == EOF)
    {
      return;
    }
    if (ch == '\n')
    {
      RESET_COL;
      NEXT_LINE;
    }
    if (!std::isspace(ch))
    {
      fs_.unget();
      if (fs_.peek() == '\n')
      {
        PREV_LINE;
      }
      PREV_COL;
      return;
    }
  }
}

void RatSource::seekLine(const unsigned &idx)
{
  char ch;
  seekReset();
  while (fs_.get(ch))
  {
    if (ch == EOF)
    {
      break;
    }
    if (ch == '\n')
    {
      NEXT_LINE;
    }
    if (line_num >= idx)
    {
      return;
    }
  }
  std::cerr << "file length: '" << line_num << "' requested: '" << idx
            << '\'' << std::endl;
  throw std::invalid_argument("error: index out of bounds");
}

void RatSource::seekCol(const unsigned &idx)
{
  /// @todo
}

unsigned int RatSource::getLineNum() { return this->line_num; }

unsigned int RatSource::getColNum() { return this->col_num; }

void RatSource::debugPrinter()
{
  seekReset();
  if (!fs_.is_open())
  {
    throw std::invalid_argument("error opening file");
  }

  std::string line;

  while (std::getline(fs_, line))
  {
    std::cout << line << std::endl;
  }
  seekReset();
}
