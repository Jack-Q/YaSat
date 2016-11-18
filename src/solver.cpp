#include "solver.h"

namespace yasat {
/**
 * Sort out the original data read from the input
 */
void Solver::prep() {
  // int maxLit;
  printLiterialMetaList();
  printClauseWatchingList();

  for (auto clause = clauses.begin(); clause != clauses.end(); clause++) {
    sort(clause->getList().begin(), clause->getList().end(),
         Literial::comparatorValue);

    // Update the literial weight
    int clauseWeight = 10 - clause->getLiterialCount();
    if (clauseWeight > 0) {
      for (auto lit = clause->getList().begin(); lit != clause->getList().end();
           lit++) {
        literialMetaList.at(lit->getVal() - 1).weight += clauseWeight;
        if (lit->isPositive()) {
          literialMetaList.at(lit->getVal() - 1).weightPositive += clauseWeight;
          // literialMetaList.at(lit->getVal() - 1)
          //     .positiveList.push_back(&*clause);
        } else {
          literialMetaList.at(lit->getVal() - 1).weightNegative += clauseWeight;
          // literialMetaList.at(lit->getVal() - 1)
          //     .negativeList.push_back(&*clause);
        }
      }
    }

    // Setup two literial watching
    ClauseWatching watching(*clause);
    watching.firstWatching = 0;
    addClauseToLiteralList(watching, 1);
    if(clause->getLiterialCount() > 1){
      watching.secondWatching = 1;
      addClauseToLiteralList(watching, 0);
    }
    clauseWatchingList.push_back(watching);

    // Handle single variable list
    if(clause->getLiterialCount() == 1){
      pendingUniqueClauseWatching.push(&clauseWatchingList.back());
    }
  }


}

void Solver::solve() {
  // handle initial BCP
  while(!pendingUniqueClauseWatching.empty()){
    // Pick unhandled clause
    ClauseWatching *watching = pendingUniqueClauseWatching.front();
    pendingUniqueClauseWatching.pop();

    // place the unique literial at the first position
    if(watchingLiterialStatus(*watching, 1).isAssigned())
      watching->swapWatchingIndex();

    // this unique clause may be assigned during former implcation
    if(watchingLiterialStatus(*watching, 1).isAssigned())
      continue;

    Literial &lit = watching->clause[watching->firstWatching];
    LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
    LiterialAssignment assignment(litM);

    if(lit.isPositive()){
      // Perform possible assignment
      litM.assignmet = Bool::getTrueValue();
    }else {
      // Perform negative assignment
      litM.assignmet = Bool::getFalseValue();
    }

  }

  printLiterialMetaList();
  printClauseWatchingList();


}

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
        << i->listValue << ":" << i->assignmet << fmt::reset << "("<<i->weight<<")"<< " ";
  }
  msg << endl;
}

void Solver::printClauseWatchingList() {
  msg << fmt::messageLabel << "Clause Watching: " << endl;
  for (auto i = clauseWatchingList.begin(); i != clauseWatchingList.end();
       i++) {
    msg << "      ";
    for (int j = 0; j < i->clause.getLiterialCount(); j++) {
      if (j == i->firstWatching || j == i->secondWatching) {
        msg << " [" << i->clause[j] << "] ";
      } else {
        msg << "  " << i->clause[j] << "  ";
      }
    }
    msg << endl;
  }
}
}
