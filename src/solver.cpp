#include "solver.h"

namespace yasat {
/**
 * Sort out the original data read from the input
 */
void Solver::prep() {
  // int maxLit;

  for (auto clauseItr = clauses.begin(); clauseItr != clauses.end(); clauseItr++) {
    Clause *clause = clauseItr->get();
    sort(clause->getList().begin(), clause->getList().end(),
         Literal::comparatorValue);

    // Update the literal weight
    int clauseWeight = 10 - clause->getLiteralCount();
    if (clauseWeight > 0) {
      for (auto lit = clause->getList().begin(); lit != clause->getList().end();
           lit++) {
        literalMetaList.at(lit->getVal() - 1).weight += clauseWeight;
        if (lit->isPositive()) {
          literalMetaList.at(lit->getVal() - 1).weightPositive += clauseWeight;
          // literalMetaList.at(lit->getVal() - 1)
          //     .positiveList.push_back(&*clause);
        } else {
          literalMetaList.at(lit->getVal() - 1).weightNegative += clauseWeight;
          // literalMetaList.at(lit->getVal() - 1)
          //     .negativeList.push_back(&*clause);
        }
      }
    }

    // Setup two literal watching
    clauseWatchingList.push_back(make_unique<ClauseWatching>(*clause));
    ClauseWatching &watching = *clauseWatchingList.back();

    watching.firstWatching = 0;
    addClauseToLiteralList(watching, true);
    if (clause->getLiteralCount() > 1) {
      watching.secondWatching = 1;
      addClauseToLiteralList(watching, false);
    }

    // Handle single variable list
    if (clause->getLiteralCount() == 1) {
      pendingUniqueClauseWatching.push(&watching);
    }
  }

  for (size_t i = 0; i < literalMetaList.size(); i++) {
    literalMetaPtrOrderList[i] = &literalMetaList[i];
  }
  sort(literalMetaPtrOrderList.begin(), literalMetaPtrOrderList.end(),
       [](LiteralMeta *lit1, LiteralMeta *lit2) {
         return lit1->weight > lit2->weight;
       });

  printLiteralMetaList();
  printClauseWatchingList();
}

void Solver::solve() {
  while (true) {
    printLiteralMetaList();
    if (!rollback) {
      // handle initial BCP
      while (!pendingUniqueClauseWatching.empty()) {
        // Pick unhandled' clause
        ClauseWatching *watching = pendingUniqueClauseWatching.front();
        pendingUniqueClauseWatching.pop();
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "handle unique clause " << *watching
            << endl;
#endif
        // place the unique literal at the first position
        Bool result_1 = watchingLiteralStatus(*watching, true);
        Bool result_2 = watchingLiteralStatus(*watching, false);
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

        Literal &lit = watching->clause[watching->firstWatching];
        LiteralMeta &litM = literalMetaList[lit.getVal() - 1];
        literalAssignmentList.push_back(LiteralAssignment(litM, &watching->clause));

        if (lit.isPositive()) {
// Perform possible assignment
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::warning << "implication"
              << fmt::reset << ": " << litM.listValue << " -> "
              << Bool::getTrueValue() << endl;
#endif
          if (updateWatchingLiteral(litM, Bool::getTrueValue()) < 0) {
            rollbackAfterConflict();
          }
        } else {
// Perform negative assignment
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::warning << "implication"
              << fmt::reset << ": " << litM.listValue << " -> "
              << Bool::getFalseValue() << endl;
#endif
          if (updateWatchingLiteral(litM, Bool::getFalseValue()) < 0) {
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
      for (auto nxtPtr = literalMetaPtrOrderList.begin();
           nxtPtr < literalMetaPtrOrderList.end(); nxtPtr++) {
        auto nxt = *nxtPtr;
        if (nxt->assignmet.isAssigned())
          continue;
        literalAssignmentList.push_back(LiteralAssignment(
            *nxt, LiteralAssignment::LITERIAL_ASSIGN_DECISION));
        if (nxt->weightPositive > nxt->weightNegative) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::error << "decision"
              << fmt::reset << ": " << nxt->listValue << " -> "
              << Bool::getFalseValue() << endl;
#endif
          if (updateWatchingLiteral(*nxt, Bool::getFalseValue()) < 0) {
            rollbackAfterConflict();
            break;
          }
        } else {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::error << "decision"
              << fmt::reset << ": " << nxt->listValue << " -> "
              << Bool::getTrueValue() << endl;
#endif
          if (updateWatchingLiteral(*nxt, Bool::getTrueValue()) < 0) {
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
      LiteralMeta &litM = literalAssignmentList.back().getLiteralMeta();

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << "Rollback, make another " << fmt::error
          << "decision" << fmt::reset << ": " << litM.listValue << " -> "
          << litM.assignmet << endl;
#endif
      if (updateWatchingLiteral(litM, litM.assignmet) < 0)
        rollbackAfterConflict();
    }
  }
}

void Solver::getSolution(vector<Literal> &sol) {
  sol.clear();
  if (unsatisfiable)
    return;
  for (auto litm = literalMetaList.begin(); litm != literalMetaList.end();
       litm++) {
    sol.push_back(Literal(litm->listValue, litm->assignmet.getValue(),
                           Bool::BOOL_UNASSIGN));
  }
}

void Solver::addClauseToLiteralList(ClauseWatching &watching, int isFirst) {
  Literal &lit =
      watching
          .clause[isFirst ? watching.firstWatching : watching.secondWatching];
  LiteralMeta &litM = literalMetaList[lit.getVal() - 1];
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

void Solver::removeClauseFromLiteralList(ClauseWatching &watching,
                                          int isFirst) {
  Literal &lit =
      watching
          .clause[isFirst ? watching.firstWatching : watching.secondWatching];
  LiteralMeta &litM = literalMetaList[lit.getVal() - 1];
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

Bool Solver::clauseLiteralStatus(Clause &clause, int index) const {
  Literal &lit = clause[index];
  const LiteralMeta &litM = literalMetaList[lit.getVal() - 1];
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

Bool Solver::watchingLiteralStatus(ClauseWatching &watching,
                                    bool isFirst) const {
  int watchingIndex =
      isFirst ? watching.firstWatching : watching.secondWatching;
  if (watchingIndex == -1) {
    // Even though there is no watched variable, this one is satisfied, anyway
    return Bool::getFalseValue();
  }
  return clauseLiteralStatus(watching.clause, watchingIndex);
}

int Solver::findNextWatchingLiteral(ClauseWatching &watching) const {
  int i = max(watching.firstWatching, watching.secondWatching);
  int c = watching.clause.getLiteralCount();
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
  msg << fmt::messageLabel << "Find next watching item in " << watching << endl;
#endif
  for (int j = (i + 1) % c; j != i; j = (j + 1) % c) {
    if ((j == watching.firstWatching) || (j == watching.secondWatching))
      continue;
    Bool litStatus = clauseLiteralStatus(watching.clause, j);
    if (litStatus.isAssigned()) {
      if (litStatus.isTrue()) {
        // since current clause is already satisfied, no further searching is
        // required
        return -1;
      }
      continue;
    } else {
      // find an unsigned literal
      return j;
    }
  }
  // no result found
  return -2;
}

void Solver::printLiteralMetaList() {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
  msg << fmt::messageLabel << "LitMeta: ";
  int limit = 100;
  for (auto i = literalMetaList.begin();
       i != literalMetaList.end() && limit--; i++) {
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
    msg << "      " << **i << endl;
  }
  if (!++limit)
    msg << " ..." << endl;
#endif
}

int Solver::updateWatchingLiteral(LiteralMeta &litM, Bool assignValue) {
  litM.assignmet = assignValue;
  auto &clauseUpdateList =
      assignValue.isTrue() ? litM.negativeList : litM.positiveList;
  for (auto i = clauseUpdateList.begin(); i < clauseUpdateList.end();
       /*Since iterator may invalidate during update, the increament
       depend on specific operation*/) {
    ClauseWatching *curWat = *i;
    Literal *curLit = nullptr;
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

    int nextIndex = findNextWatchingLiteral(*curWat);

    if (nextIndex == -2) {
      // curWat->firstWatching = curWat->secondWatching;
      // curWat->secondWatching = -1;
      Bool result = watchingLiteralStatus(*curWat, false);
      if (result.isAssigned() && !result.isTrue()) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << fmt::error << "find conflict" << fmt::reset
            << endl;
        msg << fmt::messageLabel << "conflict @ " << *curWat << endl;
#endif
        printLiteralMetaList();
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
          << "\"is satisfied already, don't need to update watching literal "
          << endl;
#endif
      i++; // update iterator
    } else {
      // removeClauseFromLiteralList(*curWat, 1);
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
  while (!literalAssignmentList.empty()) {
    LiteralAssignment &lastAssignment = literalAssignmentList.back();
    if (lastAssignment.isDecision()) {
      // Decision
      if (lastAssignment.isFirstAssign()) {
        lastAssignment.nextAssign();
        if (lastAssignment.getLiteralMeta().assignmet.isTrue()) {
          lastAssignment.getLiteralMeta().assignmet = Bool::getFalseValue();
        } else {
          lastAssignment.getLiteralMeta().assignmet = Bool::getTrueValue();
        }
        return;
      } else {
        lastAssignment.getLiteralMeta().assignmet = Bool::getUnsignedValue();
        literalAssignmentList.pop_back();
      }
    } else {
      // Implication
      lastAssignment.getLiteralMeta().assignmet =
          Bool::Bool::getUnsignedValue();
      literalAssignmentList.pop_back();
    }
  }
  unsatisfiable = true;
  return;
}
}
