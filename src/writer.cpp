#include "writer.h"

namespace yasat {

void Writer::write(ostream &out, vector<Literial> &solution) {
  if (solution.empty()) {
    out << "s UNSATISFIABLE" << endl;
    return;
  }
  out << "s SATISFIABLE" << endl;
  out << "v ";
  for(auto i = solution.begin(); i != solution.end(); i++)
  out << i->getInt() <<" ";
  out << "0" << endl;
}
}
