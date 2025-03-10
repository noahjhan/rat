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

    void seek_reset(); // points to 0th byte in file stream
    std::string readLine();
    std::string readWord(); // tokens are not always seperated by whitespace -> use for debugging
    char advanceChar();
    char peekChar();
    void reverse();
    void advanceWhitespace();
    void selectLine(const unsigned &i);
    void selectCol(const unsigned &i);

    void debugPrinter();

private:
    std::string filename_;
    std::fstream fs_;
};
#endif // RAT_SOURCE_HPP