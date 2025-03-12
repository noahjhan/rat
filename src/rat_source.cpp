#include "rat_source.hpp"

RatSource::RatSource(const std::string &filename) : filename_(filename)
{
  fs_.open(filename_);
  if (!fs_.is_open())
  {
    throw std::invalid_argument("invalid file");
  }
  line_num = 1;
  col_num = 1;
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

void RatSource::seek_reset() { fs_.seekg(0, std::ios::beg); }

std::string RatSource::readLine()
{
  std::string line;
  if (std::getline(fs_, line))
  {
    line_num++;
    col_num = 1;
    return line;
  }
  return "";
}

std::string RatSource::readWord()
{
  std::string word;
  char ch;

  while (fs_.get(ch) && std::isspace(ch))
  {
    col_num++;
    if (ch == '\n')
    {
      col_num = 1;
      line_num++;
    }
  }

  if (!fs_.good())
  {
    return "";
  }

  do
  {
    word.push_back(ch);
    col_num++;
  } while (fs_.get(ch) && !std::isspace(ch));

  return word;
}

char RatSource::advanceChar()
{
  char ch;
  if (fs_.get(ch))
  {
    col_num++;
    if (ch == '\n')
    {
      col_num = 1;
      line_num++;
    }
    return ch;
  }
  return EOF;
}

char RatSource::peekChar() { return fs_.peek(); }

void RatSource::reverse()
{
  if (!fs_.good())
  {
    std::cerr << "error cannot reverse: EOF" << std::endl;
    return;
  }
  fs_.unget();
  if (fs_.peek() == '\n')
  {
    throw std::runtime_error("cannot reverse a newline character");
  }
  col_num--;
}

void RatSource::advanceWhitespace()
{
  char ch;
  while (fs_.get(ch))
  {
    col_num++;
    if (ch == EOF || ch == '\0')
    {
      std::cerr << "in advance whitespace: '" << int(ch)
                << "' invalid character" << std::endl;
    }
    if (ch == '\n')
    {
      line_num++;
      col_num = 1;
    }
    if (!std::isspace(ch))
    {
      fs_.unget();
      col_num--;
      return;
    }
  }
}

void RatSource::selectLine(const unsigned &ix)
{
  /// @todo
}

void RatSource::selectCol(const unsigned &i)
{
  /// @todo
}

unsigned int RatSource::getLineNum() { return this->line_num; }

unsigned int RatSource::getColNum() { return this->col_num; }

void RatSource::debugPrinter()
{
  seek_reset();
  if (!fs_.is_open())
  {
    throw std::invalid_argument("error opening file");
  }

  std::string line;

  while (std::getline(fs_, line))
  {
    std::cout << line << std::endl;
  }
  seek_reset();
}
