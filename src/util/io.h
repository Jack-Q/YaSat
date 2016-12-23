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
  inline ostream &message(ostream &);
  inline ostream &messageLabel(ostream &);
  inline ostream &positive(ostream &);
  inline ostream &negative(ostream &);
  inline ostream &reset(ostream &);
  inline ostream &statisLabel(ostream &);


#if defined(DEBUG_COLOR)
  // Colored output
  ostream &warning(ostream &o) { return o << "\e[1;33m"; }
  ostream &warningLabel(ostream &o) {
    return o << warning << "[WARNING] " << reset;
  }
  ostream &error(ostream &o) { return o << "\e[1;35m"; }
  ostream &errorLabel(ostream &o) { return o << error << "[ERROR] " << reset; }
  ostream &message(ostream &o) { return o << "\e[1;32m"; }
  ostream &messageLabel(ostream &o) {
    return o << message << "[MESSAGE] " << reset;
  }
  ostream &positive(ostream &o) { return o << "\e[1;37m"; }
  ostream &negative(ostream &o) { return o << "\e[1;30m\e[47m"; }
  ostream &reset(ostream &o) { return o << "\e[0m"; }
  ostream &statisLabel(ostream &o) { return o << "\e[1;32m" << "[STATIS]" << reset; }
#else
  // plain output
  ostream &warning(ostream &o) { return o; }
  ostream &warningLabel(ostream &o) { return o << "[WARNING] "; }
  ostream &error(ostream &o) { return o; }
  ostream &errorLabel(ostream &o) { return o << "[ERROR] "; }
  ostream &message(ostream &o) { return o; }
  ostream &messageLabel(ostream &o) { return o << "[MESSAGE] "; }
  ostream &positive(ostream &o) { return o; }
  ostream &negative(ostream &o) { return o; }
  ostream &reset(ostream &o) { return o; }
  ostream &statisLabel(ostream &o) { return o << "[STATIS]"; }
#endif
}
}

#endif // !IO_HEADER
