#ifndef RAT_SOURCE_HPP
#define RAT_SOURCE_HPP

#include <string>
#include <iostream>
#include <fstream>
class RatSource
{
public:
    RatSource(const std::string &filename);
    ~RatSource();
    RatSource(const RatSource &other);
    RatSource &operator=(const RatSource &other);
    void destructor();
    void debugPrinter();

private:
    std::string filename_;
    std::fstream fs_;
};
#endif // RAT_SOURCE_HPP