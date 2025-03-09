#include "../includes/rat_source.hpp"

RatSource::RatSource(const std::string &filename) : filename_(filename)
{
    fs_.open(filename_);
    if (!fs_.is_open())
    {
        throw std::invalid_argument("error opening file");
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