#include "rat_source.hpp"

RatSource::RatSource(const std::string &filename) : filename_(filename)
{
    fs_.open(filename_);
    if (!fs_.is_open())
    {
        throw std::invalid_argument("invalid file");
    }
}

RatSource::~RatSource()
{
    fs_.close();
}

RatSource::RatSource(const RatSource &other)
{
    if (this != &other)
    {
        fs_.close();
        this->filename_ = other.filename_;
        this->fs_.open(this->filename_);
    }
}
RatSource &RatSource::operator=(const RatSource &other)
{
    if (this != &other)
    {
        fs_.close();
        this->filename_ = other.filename_;
        this->fs_.open(this->filename_);
    }
    return *this;
}
void RatSource::destructor()
{
    fs_.close();
}

void RatSource::seek_reset()
{
    fs_.seekg(0, std::ios::beg);
}

std::string RatSource::readLine()
{
    std::string line;
    last_ = std::getline(fs_, line) ? '\n' : '\0';
    return line;
}
std::string RatSource::readWord()
{
    std::string word;
    char ch;

    while (fs_.get(ch) && std::isspace(ch))
    {
        last_ = ch;
    }

    if (!fs_.good())
    {
        return "";
    }

    do
    {
        word.push_back(ch);
        last_ = ch;
    } while (fs_.get(ch) && !std::isspace(ch));

    return word;
}
char RatSource::advanceChar()
{
    char ch;
    if (fs_.get(ch))
    {
        last_ = ch;
        return ch;
    }
    last_ = '\0';
    return EOF;
}
char RatSource::peekChar()
{
    return fs_.peek();
}
void RatSource::reverse()
{
    if (last_ == '\0')
    {
        std::cerr << "error cannot reverse" << std::endl;
        return;
    }
    fs_.putback(last_);
    last_ = '\0';
}
void RatSource::advanceWhitespace()
{
    char ch;
    while (fs_.get(ch))
    {
        last_ = ch;
        if (!std::isspace(ch))
        {
            reverse();
            return;
        }
    }
}
void RatSource::selectLine(const unsigned &i)
{
    // @todo

    // seekg
}
void RatSource::selectCol(const unsigned &i)
{
    // @todo

    // seekg
}

void RatSource::debugPrinter()
{
    if (!fs_.is_open())
    {
        throw std::invalid_argument("error opening file");
    }

    std::string line;

    while (std::getline(fs_, line))
    {
        std::cout << line << std::endl;
    }
    fs_.close();
}
