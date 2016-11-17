#ifndef IO_HEADER
#define IO_HEADER

#include <fstream>
#include <iostream>
#include <ostream>

namespace yasat {
// define reference to std streams for simplicity in usage and maintance
using ::std::istream;
using ::std::ostream;

using ::std::cin;
using ::std::cout;
using ::std::cerr;

using ::std::endl;

namespace fmt {

inline ostream &warning(ostream &o) { return o << "\e[1;33m[WARNING] \e[m"; }

inline ostream &error(ostream &o) { return o << "\e[1;35m[ERROR] \e[m"; }

inline ostream &positive(ostream &o) { return o << "\e[4;37m"; }

inline ostream &negative(ostream &o) { return o << "\e[4;30m\e[47m"; }

inline ostream &reset(ostream &o) { return o << "\e[m"; }
}
}

#endif // !IO_HEADER
