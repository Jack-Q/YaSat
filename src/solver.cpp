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
    clauseWatchingList.push_back(ClauseWatching(*clause));
    ClauseWatching &watching = clauseWatchingList.back();

    watching.firstWatching = 0;
    addClauseToLiteralList(watching, 1);
    if (clause->getLiterialCount() > 1) {
      watching.secondWatching = 1;
      addClauseToLiteralList(watching, 0);
    }

    // Handle single variable list
    if (clause->getLiterialCount() == 1) {
      pendingUniqueClauseWatching.push(&watching);
    }
  }
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
        msg << fmt::messageLabel << "handle unique clause " << *watching
            << endl;
        // place the unique literial at the first position
        if (watchingLiterialStatus(*watching, 1).isAssigned())
          watching->swapWatchingIndex();

        // this unique clause may be assigned during former implcation
        if (watchingLiterialStatus(*watching, 1).isAssigned())
          continue;

        Literial &lit = watching->clause[watching->firstWatching];
        LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
        literialAssignmentList.push_back(LiterialAssignment(litM));

        if (lit.isPositive()) {
          // TODO: Mark current as satisfied
          // Perform possible assignment
          if (updateWatchingLiterial(litM, Bool::getTrueValue()))
            rollbackAfterConflict();
        } else {
          // Perform negative assignment
          if (updateWatchingLiterial(litM, Bool::getFalseValue()))
            rollbackAfterConflict();
        }
      }

      msg << fmt::messageLabel << "Make decision" << endl;
      bool assign = false;
      for (auto nxt = literialMetaList.begin(); nxt < literialMetaList.end();
           nxt++) {
        if (nxt->assignmet.isAssigned())
          continue;
        literialAssignmentList.push_back(LiterialAssignment(
            *nxt, LiterialAssignment::LITERIAL_ASSIGN_DECISION));
        if (nxt->weightPositive > nxt->weightNegative) {
          if (updateWatchingLiterial(*nxt, Bool::getFalseValue()) < 0)
            rollbackAfterConflict();
        } else {
          if (updateWatchingLiterial(*nxt, Bool::getTrueValue()) < 0)
            rollbackAfterConflict();
        }
        assign = true;
        break;
      }
      if(!assign) return;
    } else {
      if (unsatisfiable) {
        return;
      }
      rollback = false;
      msg << fmt::messageLabel << "Rollback, make another decision" << endl;
      LiterialMeta &litM = literialAssignmentList.back().getLiterialMeta();
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

void Solver::printLiterialMetaList() {
  msg << fmt::messageLabel << "LitMeta: ";
  for (auto i = literialMetaList.begin(); i != literialMetaList.end(); i++) {
    msg << (i->assignmet.isAssigned()
                ? i->assignmet.isTrue() ? fmt::message : fmt::negative
                : fmt::positive)
        << i->listValue << ":" << i->assignmet << fmt::reset << "(" << i->weight
        << ")"
        << " ";
  }
  msg << endl;
}

void Solver::printClauseWatchingList() {
  msg << fmt::messageLabel << "Clause Watching: " << endl;
  for (auto i = clauseWatchingList.begin(); i < clauseWatchingList.end(); i++) {
    msg << "      " << *i << endl;
  }
}

int Solver::updateWatchingLiterial(LiterialMeta &litM, Bool assignValue) {
  litM.assignmet = assignValue;
  auto &clauseUpdateList =
      assignValue.isTrue() ? litM.negativeList : litM.positiveList;
  for (auto i = clauseUpdateList.begin(); i < clauseUpdateList.end(); i++) {
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

    removeClauseFromLiterialList(*curWat, 1);
    int nextIndex = findNextWatchingLiterial(*curWat);
    if (nextIndex < 0) {
      // curWat->firstWatching = curWat->secondWatching;
      // curWat->secondWatching = -1;
      if (watchingLiterialStatus(*curWat, false)) {
        msg << fmt::messageLabel << fmt::error << "find conflict" << fmt::reset
            << endl;
        return -1;

      } else {
        msg << fmt::messageLabel << "add unique clause" << endl;
        pendingUniqueClauseWatching.push(curWat);
      }
    } else {
      msg << fmt::messageLabel << "update watching item from "
          << curWat->firstWatching << " to " << nextIndex << endl;
      // update watching item
      curWat->firstWatching = nextIndex;
      addClauseToLiteralList(*curWat, 1);
    }
  }
  return 0;
}

void Solver::rollbackAfterConflict() {
  rollback = true;
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
