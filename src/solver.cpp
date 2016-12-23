#include "solver.h"

namespace yasat {
/**
 * Sort out the original data read from the input
 */
void Solver::prep() {
  // int maxLit;

  for (auto clauseItr = clauses.begin(); clauseItr != clauses.end();
       clauseItr++) {
    Clause *clause = clauseItr->get();
    sort(clause->getList().begin(), clause->getList().end(),
         Literal::comparatorValue);

    // Update the literal weight
    updateLiteralWeightWithClause(clause);

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

  printLiteralMetaList();
  printClauseWatchingList();
}

void Solver::solve() {
  while (true) {
    literalWeightDecay();

    if(statis_decision % 3000 == 0)
      printStatisData();

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
            msg << fmt::messageLabel << "conflict" << endl;
#endif
            rollbackAfterConflict(&watching->clause);
            break;
          }
        }
        if (result_1.isAssigned())
          watching->swapWatchingIndex();

        // update statistical data
        statis_implication++;
        Literal &lit = watching->clause[watching->firstWatching];
        LiteralMeta &litM = literalMetaList[lit.getVal() - 1];
        newImplication(litM, watching->clause);

        if (lit.isPositive()) {
// Perform possible assignment
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::warning << "implication"
              << fmt::reset << ": " << litM.listValue << " -> "
              << Bool::getTrueValue() << endl;
#endif
          ClauseWatching *conflict =
              updateWatchingLiteral(litM, Bool::getTrueValue());
          if (conflict != nullptr) {
            rollbackAfterConflict(&conflict->clause);
          }
        } else {
// Perform negative assignment
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
          msg << fmt::messageLabel << "Make " << fmt::warning << "implication"
              << fmt::reset << ": " << litM.listValue << " -> "
              << Bool::getFalseValue() << endl;
#endif

          ClauseWatching *conflict =
              updateWatchingLiteral(litM, Bool::getFalseValue());
          if (conflict != nullptr) {
            rollbackAfterConflict(&conflict->clause);
          }
        }
      }

      if (rollback)
        continue;
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << "Make decision" << endl;
#endif

      if(!literalMetaPtrOrderListInHeapOrder)
        std::make_heap(literalMetaPtrOrderList.begin(), literalMetaPtrOrderList.end(),
               literalCompartor), literalMetaPtrOrderListInHeapOrder = true;
      while (!literalMetaPtrOrderList.empty() &&
             literalMetaPtrOrderList.front()->assignmentStatus) {

        // move the top literal to initial position
        std::pop_heap(literalMetaPtrOrderList.begin(),
                      literalMetaPtrOrderList.end(), literalCompartor);
        literalMetaPtrOrderList.pop_back();
      }

      if (literalMetaPtrOrderList.empty()) {
// No unassigned literal, found the result
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "Literal Meta List Empty" << endl;
#endif
        if (!rollback)
          return;
        else
          continue;
      }

      // move the top literal to initial position
      std::pop_heap(literalMetaPtrOrderList.begin(),
                    literalMetaPtrOrderList.end(), literalCompartor);
      auto nxt = literalMetaPtrOrderList.back();
      literalMetaPtrOrderList.pop_back();

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << fmt::error << "Pop" << fmt::reset
          << " literal " << nxt->listValue << " from list" << endl;
#endif

      // Update statistical data for decsion count
      statis_decision++;
      newDecision(*nxt);

      if (nxt->weightPositive > nxt->weightNegative) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "Make " << fmt::error << "decision"
            << fmt::reset << ": " << nxt->listValue << " -> "
            << Bool::getFalseValue() << endl;
#endif
        ClauseWatching *conflict =
            updateWatchingLiteral(*nxt, Bool::getFalseValue());
        if (conflict != nullptr) {
          rollbackAfterConflict(&conflict->clause);
        }
      } else {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "Make " << fmt::error << "decision"
            << fmt::reset << ": " << nxt->listValue << " -> "
            << Bool::getTrueValue() << endl;
#endif
        ClauseWatching *conflict =
            updateWatchingLiteral(*nxt, Bool::getTrueValue());
        if (conflict != nullptr) {
          rollbackAfterConflict(&conflict->clause);
        }
      }

      // if (!rollback)
      //   return;

    } else {
      if (unsatisfiable) {
        return;
      }
      rollback = false;
      LiteralMeta &litM = literalAssignmentList.back()->getLiteralMeta();

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << "Rollback, make another " << fmt::error
          << "decision" << fmt::reset << ": " << litM.listValue << " -> "
          << litM.assignment << endl;
#endif
      ClauseWatching *conflict = updateWatchingLiteral(litM, litM.assignment);
      if (conflict != nullptr)
        rollbackAfterConflict(&conflict->clause);
    }
  }
}

void Solver::getSolution(vector<Literal> &sol) {
  sol.clear();
  if (unsatisfiable)
    return;
  for (auto litm = literalMetaList.begin(); litm != literalMetaList.end();
       litm++) {
    sol.push_back(Literal(litm->listValue, litm->assignment.getValue(),
                          Bool::BOOL_UNASSIGN));
  }
}

const std::function<bool(LiteralMeta const *, LiteralMeta const *)>
    Solver::literalCompartor =
        [](LiteralMeta const *lit1, LiteralMeta const *lit2) -> bool {
  return lit1->weight > lit2->weight;
};

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
  if (litM.assignment.isAssigned()) {
    return ((litM.assignment.isTrue() && lit.isPositive()) ||
            (!litM.assignment.isTrue() && !lit.isPositive()))
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
  for (auto i = literalMetaList.begin(); i != literalMetaList.end() && limit--;
       i++) {
    msg << (i->assignment.isAssigned()
                ? i->assignment.isTrue() ? fmt::message : fmt::negative
                : fmt::positive)
        << i->listValue << ":" << i->assignment << fmt::reset << "("
        << i->weight << ")"
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

ClauseWatching *Solver::updateWatchingLiteral(LiteralMeta &litM,
                                              Bool assignValue) {
  litM.assignment = assignValue;
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
        printLiteralMetaList();
#endif
        return curWat;
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
  return nullptr;
}

void Solver::rollbackAfterConflict(Clause *antecedent) {
  // Update statistical data
  statis_conflict++;

  rollback = true;

  // Clear pending unique clause
  while (!pendingUniqueClauseWatching.empty())
    pendingUniqueClauseWatching.pop();

  Clause conflictClause;
  int implicantCount;
  vector<Literal> &conflictLiterals = conflictClause.getList();
  conflictLiterals = antecedent->getList();

  // process at current decision level
  while (!literalAssignmentList.empty()) {
    LiteralAssignment &lastAssignment = *literalAssignmentList.back();
    if (lastAssignment.isDecision())
      break;

    bool containsImplication = false;
    for (auto i = conflictLiterals.begin(); i < conflictLiterals.end(); i++) {
      if (i->getVal() == lastAssignment.getLiteralMeta().listValue) {
        containsImplication = true;
        break;
      }
    }

    // reset literal to unassigned state
    deleteLastAssignment();

    if (containsImplication) {
      // Implication
      antecedent = lastAssignment.getAntecedent();
      implicantCount = 0;
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::messageLabel << "Current learnt: " << conflictClause << endl;
      msg << fmt::messageLabel << "To be resolved: " << *antecedent << endl;
#endif

      // Resolve literal list
      for (auto i = antecedent->getList().begin(), j = conflictLiterals.begin();
           i < antecedent->getList().end(); i++) {
        while (j < conflictLiterals.end() && j->getVal() < i->getVal()) {
          j++;
        }
        if (j < conflictLiterals.end() && i->getVal() == j->getVal()) {
          if (i->isPositive() == j->isPositive()) {
            // Same literal, ignore
          } else {
            // Conflict literal pair, eliminate
            j = conflictLiterals.erase(j);
          }
        } else {
          // new literal
          j = conflictLiterals.insert(j, *i);
          j++;
        }
      }

      // count implcation literial count
      int i = 0;
      for (auto j = conflictLiterals.begin(); j < conflictLiterals.end(); j++)
        if (isAssignedAtCurrentLevel(*j))
          implicantCount++, i = j - conflictLiterals.begin();

      if (implicantCount == 1) {

        // Find the level that is approprate to be jumped to
        int backtrackingLevel = -1;
        for (auto j = conflictLiterals.begin(); j < conflictLiterals.end();
             j++) {
          auto &meta = literalMetaList[j->getVal() - 1];
          if (meta.assignment.isAssigned() &&
              meta.assignmentStatus->getAssignmentLevel() != assignmentLevel)
            backtrackingLevel = max(
                backtrackingLevel, meta.assignmentStatus->getAssignmentLevel());
        }

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "curent level: " << assignmentLevel
            << " the level to be backtracked: " << backtrackingLevel << endl;
#endif
        if (backtrackingLevel == -1) {
          break;
        }

        if(assignmentLevel - backtrackingLevel > statis_backtracklength)
          statis_backtracklength = assignmentLevel - backtrackingLevel;

// Find First UIP
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel
            << "Found 1UIP, conflict will to be added: " << conflictClause
            << endl;
#endif
        // Push new clause to learnt list
        leartClauses.push_back(make_unique<Clause>(conflictClause));
        // Create and add clause watching service
        unique_ptr<ClauseWatching> clauseWatching =
            make_unique<ClauseWatching>(*leartClauses.back());
        clauseWatching->firstWatching = i;
        addClauseToLiteralList(*clauseWatching, true);
        if (conflictLiterals.size() > 1) {
          clauseWatching->secondWatching =
              i == 0 ? 1 : 0; // TODO: add more heuristic selection
          addClauseToLiteralList(*clauseWatching, false);
        }
        clauseWatchingList.push_back(move(clauseWatching));
        // Update the weight of the literal
        #if defined(LITERAL_WEIGHT_UPDATE) 
        if(updateLiteralWeightWithClause(&*leartClauses.back()))
          literalMetaPtrOrderListInHeapOrder = false;
        #endif

        while (!literalAssignmentList.empty()) {
          LiteralAssignment &lastAssignment = *literalAssignmentList.back();
          if (lastAssignment.getAssignmentLevel() == backtrackingLevel) {
            // target level
            assignmentLevel = backtrackingLevel;
            pendingUniqueClauseWatching.push(&*clauseWatchingList.back());
            rollback = false; // the nonchronological backtracking will result
                              // in a state of unfinished BCP state
            return;
          } else {
            deleteLastAssignment();
          }
        }
        break;
      } else {

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
        msg << fmt::messageLabel << "implcant count: " << implicantCount
            << endl;
#endif
      }
    }
  }

  while (!literalAssignmentList.empty()) {
    LiteralAssignment &lastAssignment = *literalAssignmentList.back();
    if (lastAssignment.isDecision()) {
      // Decision
      if (lastAssignment.isFirstAssign()) {
        lastAssignment.nextAssign();
        if (lastAssignment.getLiteralMeta().assignment.isTrue()) {
          lastAssignment.getLiteralMeta().assignment = Bool::getFalseValue();
        } else {
          lastAssignment.getLiteralMeta().assignment = Bool::getTrueValue();
        }
        return;
      } else {
        deleteLastAssignment();
        assignmentLevel--;
      }
    } else {
      deleteLastAssignment();
    }
  }
  unsatisfiable = true;
  return;
}

}
