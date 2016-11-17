#ifndef SOLVER_HEADER
#define SOLVER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class Solver {
public:
  Solver(vector<Clause> &cls) : clauses(cls) {}

  void prep(){

  }

  void solve(){

  }

  void getSolution(vector<Literial>& sol){
    sol.clear();
    for(int i =0; i < 10; i++)
    sol.push_back(Literial(i));
  }

private:
  int interialCount;
  vector<Clause> &clauses;
};
}

#endif