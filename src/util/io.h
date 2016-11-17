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
inline ostream &warning(ostream &);
inline ostream &warningLabel(ostream &);
inline ostream &error(ostream &);
inline ostream &errorLabel(ostream &);
inline ostream &positive(ostream &);
inline ostream &negative(ostream &);
inline ostream &reset(ostream &);

ostream &warning(ostream &o) { return o << "\e[1;33m"; }
ostream &warningLabel(ostream &o) {
  return o << warning << "[WARNING] " << reset;
}
ostream &error(ostream &o) { return o << "\e[1;35m"; }
ostream &errorLabel(ostream &o) { return o << error << "[ERROR] " << reset; }
ostream &positive(ostream &o) { return o << "\e[1;37m"; }
ostream &negative(ostream &o) { return o << "\e[1;30m\e[47m"; }
ostream &reset(ostream &o) { return o << "\e[m"; }
}
}

#endif // !IO_HEADER
