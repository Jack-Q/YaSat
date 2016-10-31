#ifndef IO_HEADER
#define IO_HEADER

#include <iostream>
#include <fstream>

namespace yasat
{
// define reference to std streams for simplicity in usage and maintance
std::istream &cin = std::cin;
std::ostream &cout = std::cout;
std::ostream &cerr = std::cerr;
}

#endif // !IO_HEADER