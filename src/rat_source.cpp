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