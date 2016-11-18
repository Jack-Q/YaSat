#include "solver.h"

namespace yasat {
/**
 * Sort out the original data read from the input
 */
void Solver::prep() {
  // int maxLit;
  printLiterialMetaList();
  for (auto clause = clauses.begin(); clause != clauses.end(); clause++) {
    sort(clause->getList().begin(), clause->getList().end(),
         Literial::comparatorValue);
    // TODO: Add positive and negative clauses to list
  }
}

void Solver::solve() { printLiterialMetaList(); }

void Solver::getSolution(vector<Literial> &sol) {
  sol.clear();
  for (auto litm = literialMetaList.begin(); litm != literialMetaList.end();
       litm++) {
    sol.push_back(Literial(litm->listValue, litm->assignmet.getValue(),
                           Bool::BOOL_UNASSIGN));
  }
}

void Solver::printLiterialMetaList() {

  msg << fmt::messageLabel << "LitMeta: ";
  for (auto i = literialMetaList.begin(); i != literialMetaList.end(); i++) {
    msg << (i->assignmet.isAssigned()
                ? i->assignmet.isTrue() ? fmt::message : fmt::negative
                : fmt::positive)
        << i->listValue << ":" << i->assignmet << fmt::reset << " ";
  }
  msg << endl;
}
}
