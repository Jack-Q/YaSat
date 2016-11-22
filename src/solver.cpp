#include "solver.h"

namespace yasat {
/**
 * Sort out the original data read from the input
 */
void Solver::prep() {
  // int maxLit;

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
    clauseWatchingList.push_back(ClauseWatching(*clause));
    ClauseWatching &watching = clauseWatchingList.back();

    watching.firstWatching = 0;
    addClauseToLiteralList(watching, true);
    if (clause->getLiterialCount() > 1) {
      watching.secondWatching = 1;
      addClauseToLiteralList(watching, false);
    }

    // Handle single variable list
    if (clause->getLiterialCount() == 1) {
      pendingUniqueClauseWatching.push(&watching);
    }
  }

  for (size_t i = 0; i < literialMetaList.size(); i++) {
    literialMetaPtrOrderList[i] = &literialMetaList[i];
  }
  sort(literialMetaPtrOrderList.begin(), literialMetaPtrOrderList.end(),
       [](LiterialMeta *lit1, LiterialMeta *lit2) {
         return lit1->weight > lit2->weight;
       });

  printLiterialMetaList();
  printClauseWatchingList();
}

void Solver::solve() {
  while (true) {
    printLiterialMetaList();
    if (!rollback) {
      // handle initial BCP
      while (!pendingUniqueClauseWatching.empty()) {
        // Pick unhandled clause
        ClauseWatching *watching = pendingUniqueClauseWatching.front();
        pendingUniqueClauseWatching.pop();
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "handle unique clause " << *watching
            << endl;
#endif
        // place the unique literial at the first position
        Bool result_1 = watchingLiterialStatus(*watching, true);
        Bool result_2 = watchingLiterialStatus(*watching, false);
        if (result_1.isAssigned() && result_2.isAssigned()) {
          if (result_1.isTrue() || result_2.isTrue()) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
            msg << fmt::messageLabel
                << "current unique clause is resolved already" << endl;
#endif
            continue;
          } else {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
            msg << fmt::messageLabel << "confilct" << endl;
#endif
            rollbackAfterConflict();
            break;
          }
        }
        if (result_1.isAssigned())
          watching->swapWatchingIndex();

        Literial &lit = watching->clause[watching->firstWatching];
        LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
        literialAssignmentList.push_back(LiterialAssignment(litM));

        if (lit.isPositive()) {
// Perform possible assignment
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::warning << "implication"
              << fmt::reset << ": " << litM.listValue << " -> "
              << Bool::getTrueValue() << endl;
#endif
          if (updateWatchingLiterial(litM, Bool::getTrueValue()) < 0) {
            rollbackAfterConflict();
          }
        } else {
// Perform negative assignment
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::warning << "implication"
              << fmt::reset << ": " << litM.listValue << " -> "
              << Bool::getFalseValue() << endl;
#endif
          if (updateWatchingLiterial(litM, Bool::getFalseValue()) < 0) {
            rollbackAfterConflict();
          }
        }
      }

      if (rollback)
        continue;
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << "Make decision" << endl;
#endif
      bool assign = false;
      for (auto nxtPtr = literialMetaPtrOrderList.begin();
           nxtPtr < literialMetaPtrOrderList.end(); nxtPtr++) {
        auto nxt = *nxtPtr;
        if (nxt->assignmet.isAssigned())
          continue;
        literialAssignmentList.push_back(LiterialAssignment(
            *nxt, LiterialAssignment::LITERIAL_ASSIGN_DECISION));
        if (nxt->weightPositive > nxt->weightNegative) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::error << "decision"
              << fmt::reset << ": " << nxt->listValue << " -> "
              << Bool::getFalseValue() << endl;
#endif
          if (updateWatchingLiterial(*nxt, Bool::getFalseValue()) < 0) {
            rollbackAfterConflict();
            break;
          }
        } else {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::error << "decision"
              << fmt::reset << ": " << nxt->listValue << " -> "
              << Bool::getTrueValue() << endl;
#endif
          if (updateWatchingLiterial(*nxt, Bool::getTrueValue()) < 0) {
            rollbackAfterConflict();
            break;
          }
        }
        assign = true;
        break;
      }
      if (!rollback && !assign)
        return;
    } else {
      if (unsatisfiable) {
        return;
      }
      rollback = false;
      LiterialMeta &litM = literialAssignmentList.back().getLiterialMeta();

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << "Rollback, make another " << fmt::error
          << "decision" << fmt::reset << ": " << litM.listValue << " -> "
          << litM.assignmet << endl;
#endif
      if (updateWatchingLiterial(litM, litM.assignmet) < 0)
        rollbackAfterConflict();
    }
  }
}

void Solver::getSolution(vector<Literial> &sol) {
  sol.clear();
  if (unsatisfiable)
    return;
  for (auto litm = literialMetaList.begin(); litm != literialMetaList.end();
       litm++) {
    sol.push_back(Literial(litm->listValue, litm->assignmet.getValue(),
                           Bool::BOOL_UNASSIGN));
  }
}

void Solver::addClauseToLiteralList(ClauseWatching &watching, int isFirst) {
  Literial &lit =
      watching
          .clause[isFirst ? watching.firstWatching : watching.secondWatching];
  LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
  if (lit.isPositive()) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
    msg << fmt::messageLabel << "add clause" << watching
        << "to positive list of " << lit << endl;
#endif
    litM.positiveList.push_back(&watching);
  } else {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
    msg << fmt::messageLabel << "add clause" << watching
        << "to negative list of " << lit << endl;
#endif
    litM.negativeList.push_back(&watching);
  }
}

void Solver::removeClauseFromLiterialList(ClauseWatching &watching,
                                          int isFirst) {
  Literial &lit =
      watching
          .clause[isFirst ? watching.firstWatching : watching.secondWatching];
  LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
  if (lit.isPositive()) {

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
    msg << fmt::messageLabel << "remove clause" << watching
        << "to positive list of " << lit << endl;
#endif
    litM.positiveList.erase(std::remove(litM.positiveList.begin(),
                                        litM.positiveList.end(), &watching),
                            litM.positiveList.end());
  } else {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
    msg << fmt::messageLabel << "remove clause" << watching
        << "from negative list of " << lit << endl;
#endif
    litM.negativeList.erase(std::remove(litM.negativeList.begin(),
                                        litM.negativeList.end(), &watching),
                            litM.negativeList.end());
  }
}

Bool Solver::clauseLiterialStatus(Clause &clause, int index) const {
  Literial &lit = clause[index];
  const LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
  if (litM.assignmet.isAssigned()) {
    return ((litM.assignmet.isTrue() && lit.isPositive()) ||
            (!litM.assignmet.isTrue() && !lit.isPositive()))
               ? Bool::getTrueValue()
               : Bool::getFalseValue();
  } else {
    // Not assigned status
    return Bool::getUnsignedValue();
  }
}

Bool Solver::watchingLiterialStatus(ClauseWatching &watching,
                                    bool isFirst) const {
  int watchingIndex =
      isFirst ? watching.firstWatching : watching.secondWatching;
  if (watchingIndex == -1) {
    // Even though there is no watched variable, this one is satisfied, anyway
    return Bool::getFalseValue();
  }
  return clauseLiterialStatus(watching.clause, watchingIndex);
}

int Solver::findNextWatchingLiterial(ClauseWatching &watching) const {
  int i = max(watching.firstWatching, watching.secondWatching);
  int c = watching.clause.getLiterialCount();
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
  msg << fmt::messageLabel << "Find next watching item in " << watching << endl;
#endif
  for (int j = (i + 1) % c; j != i; j = (j + 1) % c) {
    if ((j == watching.firstWatching) || (j == watching.secondWatching))
      continue;
    Bool litStatus = clauseLiterialStatus(watching.clause, j);
    if (litStatus.isAssigned()) {
      if (litStatus.isTrue()) {
        // since current clause is already satisfied, no further searching is
        // required
        return -1;
      }
      continue;
    } else {
      // find an unsigned literial
      return j;
    }
  }
  // no result found
  return -2;
}

void Solver::printLiterialMetaList() {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
  msg << fmt::messageLabel << "LitMeta: ";
  int limit = 100;
  for (auto i = literialMetaList.begin();
       i != literialMetaList.end() && limit--; i++) {
    msg << (i->assignmet.isAssigned()
                ? i->assignmet.isTrue() ? fmt::message : fmt::negative
                : fmt::positive)
        << i->listValue << ":" << i->assignmet << fmt::reset << "(" << i->weight
        << ")"
        << " ";
  }
  if (!++limit)
    msg << " ...";
  msg << endl;
#endif
}

void Solver::printClauseWatchingList() {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
  msg << fmt::messageLabel << "Clause Watching: " << endl;
  int limit = 100;
  for (auto i = clauseWatchingList.begin();
       i < clauseWatchingList.end() && limit--; i++) {
    msg << "      " << *i << endl;
  }
  if (!++limit)
    msg << " ..." << endl;
#endif
}

int Solver::updateWatchingLiterial(LiterialMeta &litM, Bool assignValue) {
  litM.assignmet = assignValue;
  auto &clauseUpdateList =
      assignValue.isTrue() ? litM.negativeList : litM.positiveList;
  for (auto i = clauseUpdateList.begin(); i < clauseUpdateList.end();
       /*Since iterator may invalidate during update, the increament
       depend on specific operation*/) {
    ClauseWatching *curWat = *i;
    Literial *curLit = nullptr;
    if (curWat->firstWatching >= 0) {
      curLit = &curWat->clause[curWat->firstWatching];
      if (curLit->getVal() != litM.listValue) {
        curWat->swapWatchingIndex();
      }
    } else {
      curWat->swapWatchingIndex();
    }

    if (!curLit) {
      curLit = &curWat->clause[curWat->firstWatching];
    }

    int nextIndex = findNextWatchingLiterial(*curWat);

    if (nextIndex == -2) {
      // curWat->firstWatching = curWat->secondWatching;
      // curWat->secondWatching = -1;
      Bool result = watchingLiterialStatus(*curWat, false);
      if (result.isAssigned() && !result.isTrue()) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << fmt::error << "find conflict" << fmt::reset
            << endl;
        msg << fmt::messageLabel << "conflict @ " << *curWat << endl;
#endif
        printLiterialMetaList();
        return -1;
      }
      if (!result.isAssigned()) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "add unique clause" << endl;
#endif
        pendingUniqueClauseWatching.push(curWat);
      }
      i++; // update iterator
    } else if (nextIndex == -1) {
// this clause is satisfied
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << " current clause \"" << *curWat
          << "\"is satisfied already, don't need to update watching literial "
          << endl;
#endif
      i++; // update iterator
    } else {
      // removeClauseFromLiterialList(*curWat, 1);
      // this will erease current element and return next valid iterator
      i = clauseUpdateList.erase(i);
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << "update watching item from "
          << curWat->firstWatching << " to " << nextIndex << endl;
#endif
      // update watching item
      curWat->firstWatching = nextIndex;
      addClauseToLiteralList(*curWat, 1);
    }
  }
  return 0;
}

void Solver::rollbackAfterConflict() {
  rollback = true;
  // Clear pending unique clause
  while (!pendingUniqueClauseWatching.empty())
    pendingUniqueClauseWatching.pop();
  while (!literialAssignmentList.empty()) {
    LiterialAssignment &lastAssignment = literialAssignmentList.back();
    if (lastAssignment.isDecision()) {
      // Decision
      if (lastAssignment.isFirstAssign()) {
        lastAssignment.nextAssign();
        if (lastAssignment.getLiterialMeta().assignmet.isTrue()) {
          lastAssignment.getLiterialMeta().assignmet = Bool::getFalseValue();
        } else {
          lastAssignment.getLiterialMeta().assignmet = Bool::getTrueValue();
        }
        return;
      } else {
        lastAssignment.getLiterialMeta().assignmet = Bool::getUnsignedValue();
        literialAssignmentList.pop_back();
      }
    } else {
      // Implication
      lastAssignment.getLiterialMeta().assignmet =
          Bool::Bool::getUnsignedValue();
      literialAssignmentList.pop_back();
    }
  }
  unsatisfiable = true;
  return;
}
}
