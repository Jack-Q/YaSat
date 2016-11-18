#ifndef SOLVER_HEADER
#define SOLVER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class Solver {
public:
  Solver(vector<Clause> &cls, ostream &message) : msg(message), clauses(cls) {}

  void prep();

  void solve();

  void getSolution(vector<Literial> &sol) {
    sol.clear();
    for (int i = 1; i < 10; i++)
      sol.push_back(Literial(i));
  }

private:
  // informative
  ostream &msg;

  int literialCount;

  // Clause list
  vector<Clause> &clauses;

  // Literial list (for global information)
};
}

#endif
