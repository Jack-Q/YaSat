#include "solver.h"

namespace yasat {
/**
 * Sort out the original data read from the input
 */
void Solver::prep() {
  // int maxLit;
  msg << fmt::messageLabel << "Sizeof Literial: " << sizeof(Literial) << endl;
  msg << fmt::messageLabel << "Sizeof Bool: " << sizeof(Bool) << endl;
  msg << fmt::messageLabel << "Sizeof Int: " << sizeof(int) << endl;

  for (auto clause = clauses.begin(); clause != clauses.end(); clause++) {
    sort(clause->getList().begin(), clause->getList().end(),
         Literial::comparatorValue);
  }
}

void Solver::solve() {}
}
