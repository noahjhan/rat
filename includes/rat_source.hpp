#ifndef RAT_SOURCE_HPP
#define RAT_SOURCE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <cctype>

class RatSource
{
public:
    RatSource(const std::string &filename);
    ~RatSource();
    RatSource(const RatSource &other);
    RatSource &operator=(const RatSource &other);
    void destructor();

    void debugPrinter();
    std::string readLine();
    std::string readWord();
    char advanceChar();
    char peekChar();
    void reverse();
    void advanceWhitespace();
    void selectLine(const unsigned &i);
    void selectCol(const unsigned &i);

private:
    std::string filename_;
    std::fstream fs_;
    char last_;
};
#endif // RAT_SOURCE_HPP