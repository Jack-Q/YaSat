#ifndef IO_HEADER
#define IO_HEADER

#include <fstream>
#include <iostream>
#include <ostream>

namespace yasat
{
// define reference to std streams for simplicity in usage and maintance
using ::std::istream;
using ::std::ostream;

using ::std::cin;
using ::std::cout;
using ::std::cerr;

using ::std::endl;
// inline ostream& endl(ostream &o){
//     o << "\n";
//     o.flush();
//     return o;
// }
}

#endif // !IO_HEADER