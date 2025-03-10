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
    if (std::getline(fs_, line))
    {
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
    }

    if (!fs_.good())
    {
        return "";
    }

    do
    {
        word.push_back(ch);
    } while (fs_.get(ch) && !std::isspace(ch));

    return word;
}

char RatSource::advanceChar()
{
    char ch;
    if (fs_.get(ch))
    {
        return ch;
    }
    return EOF;
}

char RatSource::peekChar()
{
    return fs_.peek();
}

void RatSource::reverse()
{
    if (!fs_.good())
    {
        std::cerr << "error cannot reverse: EOF" << std::endl;
        return;
    }
    fs_.unget();
}

void RatSource::advanceWhitespace()
{
    char ch;
    while (fs_.get(ch))
    {
        if (!std::isspace(ch))
        {
            fs_.unget();
            return;
        }
    }
}

void RatSource::selectLine(const unsigned &i)
{
    // @todo implement seeking to the specific line
    // use seekg with proper offset
}

void RatSource::selectCol(const unsigned &i)
{
    // @todo implement seeking to the specific col
    // modify the file pointer
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
