#ifndef SOLVER_HEADER
#define SOLVER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class LiterialMeta{
public:
  int listValue;

  // vector<Clause&>
};

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

  inline void setMaxLiterial(int maxLit){
    maxLiterial = maxLit;
  }

private:
  // informative
  ostream &msg;

  int maxLiterial;

  // Clause list
  vector<Clause> &clauses;

  // Literial list (for global information)

};
}

#endif
