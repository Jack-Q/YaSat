#include "yasat.h"

namespace yasat {

inline void printTitle(ostream &out);
inline void printHelp(ostream &out);

YaSat::YaSat() : state(0), clauseSource(nullptr) {}

int YaSat::parseArgument(char **arguments, int argumentCount) {
  int currentArgument = 1; // ignore application name
  while(currentArgument < argumentCount){
    string str = arguments[currentArgument];
    if(str == "--help" || str == "--usage")
      throw RoutineException(RoutineException::USAGE);
  }
  return 0;
}

void YaSat::printError() const {
  printTitle(std::cout);
  cout << "Error: *TODO* " << endl;
}

void YaSat::printUsage() const {
  printTitle(cout);
  printHelp(cout);
}

void YaSat::loadClause() {
  if (clauseSource == nullptr) {
  }
}

void YaSat::solve() {}

void YaSat::printResult() {}

// private small functions

inline void printTitle(ostream &out) {
  out << "########################################################" << endl
      << "##                        YaSat                       ##" << endl
      << "########################################################" << endl
      << "                              Jack Q (0540017, CS, NCTU)" << endl
      << endl;
}

inline void printHelp(ostream &out) {
  out << "Usage:                                                  " << endl
      << "  yasat INPUT OUTPUT                                    " << endl
      << "    >> read from data file and write to file            " << endl
      << "  yasat -stdin -stdout                                  " << endl
      << "    >> read from `stdin' and write to `stdout'          " << endl
      << "  yasat --help                                          " << endl
      << "    >> print usage information                          " << endl
      << endl;
}
}
