#ifndef SOLVER_HEADER
#define SOLVER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class LiterialMeta {
public:
  LiterialMeta(){}

  int listValue;
  Bool assignmet;

};

class Solver {
public:
  Solver(vector<Clause> &cls, int maxLit, ostream &message)
      : msg(message), maxLiterial(maxLit), clauses(cls),
        literialMetaList(maxLit) {
          int i = 1;
          for(auto litm = literialMetaList.begin(); litm!=literialMetaList.end(); litm++){
            litm->listValue = i++;
          }
        }

  void prep();

  void solve();

  void getSolution(vector<Literial> &sol) {
    sol.clear();
    for(auto litm = literialMetaList.begin(); litm!=literialMetaList.end(); litm++){
      sol.push_back(Literial(litm->listValue, litm->assignmet));
    }
  }

private:
  // informative
  ostream &msg;

  int maxLiterial;

  // Clause list
  vector<Clause> &clauses;

  // Literial list (for global information)
  vector<LiterialMeta> literialMetaList;

  // Stack for assignment history
};
}

#endif
