#include "writer.h"

namespace yasat {

void Writer::write(ostream &out, vector<Literial> &solution) {
  if (solution.empty()) {
    out << "s UNSATISFIABLE" << endl;
    msg << fmt::messageLabel << "Result: " << fmt::error << "UNSATISFIABLE"
        << fmt::reset << endl;
    return;
  }
  out << "s SATISFIABLE" << endl;
  out << "v ";
  for (auto i = solution.begin(); i != solution.end(); i++)
    out << i->getInt() << " ";
  out << "0" << endl;

  size_t limit = 50;
  msg << fmt::messageLabel << "Result: " << fmt::message << "SATISFIABLE"
      << fmt::reset << endl;
  msg << fmt::messageLabel << "Assignment: ";
  for (auto i = solution.begin(); i != solution.end() && --limit; i++)
    msg << *i << " ";
  msg << endl;
  if(solution.size() > 50){
    msg << fmt::message << "..." << fmt::message << endl;
  }
}
}
