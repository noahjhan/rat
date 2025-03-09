#include "rat_source.hpp"

int main()
{
    RatSource rat_file = RatSource("data/testfile.rat");
    rat_file.debugPrinter();
    return 0;
}