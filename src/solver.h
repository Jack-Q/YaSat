#ifndef SOLVER_HEADER
#define SOLVER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {

class ClauseWatching;
class LiterialMeta {
public:
  LiterialMeta() : weight(0), weightPositive(0), weightNegative(0) {}

  int listValue;
  Bool assignmet;

  // static weight, occrence time in clause less then 10 variable
  int weight;
  int weightPositive;
  int weightNegative;

  vector<ClauseWatching *> positiveList;
  vector<ClauseWatching *> negativeList;
  static inline bool weightPtrComparator(LiterialMeta *lit1,
                                         LiterialMeta *lit2) {
    return lit1->weight < lit2->weight;
  };
};

class ClauseWatching {
public:
  ClauseWatching(Clause &c)
      : firstWatching(-1), secondWatching(-1), clause(c) {}

  int firstWatching;
  int secondWatching;
  Clause &clause;

  inline void swapWatchingIndex() {
    int tmp = firstWatching;
    firstWatching = secondWatching;
    secondWatching = tmp;
  }

private:
  friend ostream &operator<<(ostream &o, ClauseWatching &w) {
    for (int j = 0; j < w.clause.getLiterialCount(); j++) {
      if (j == w.firstWatching || j == w.secondWatching) {
        o << " [" << w.clause[j] << "] ";
      } else {
        o << "  " << w.clause[j] << "  ";
      }
    }
    return o;
  }
};

// Assignment of literial, including decisoin and implcation
class LiterialAssignment {
public:
  static const int LITERIAL_ASSIGN_DECISION = 1;
  static const int LITERIAL_ASSIGN_IMPLICATION = 0;
  LiterialAssignment(LiterialMeta &litMeta,
                     int type = LITERIAL_ASSIGN_IMPLICATION)
      : type(type), firstAssign(true), litM(litMeta) {}

  inline bool isDecision() const { return type == LITERIAL_ASSIGN_DECISION; }
  inline bool isFirstAssign() const { return firstAssign; }
  void nextAssign() { firstAssign = false; }
  LiterialMeta &getLiterialMeta() { return litM; }

private:
  int type;
  // Each decision assignment ought to have two possible decisions to try
  bool firstAssign;
  LiterialMeta &litM;
};

class Solver {
public:
  Solver(vector<Clause> &cls, int maxLit, ostream &message)
      : msg(message), maxLiterial(maxLit), clauses(cls),
        literialMetaList(maxLit),
        unassignedLiterialQueue(LiterialMeta::weightPtrComparator) {
    clauseWatchingList.reserve(cls.size() * 2);
    int i = 1;
    for (auto litm = literialMetaList.begin(); litm != literialMetaList.end();
         litm++) {
      litm->listValue = i++;
    }
  }

  void prep();

  void solve();

  void getSolution(vector<Literial> &sol);

private:
  // informative
  ostream &msg;

  int maxLiterial;
  bool rollback = false;
  bool unsatisfiable = false;

  // Clause list
  vector<Clause> &clauses;
  vector<ClauseWatching> clauseWatchingList;

  void addClauseToLiteralList(ClauseWatching &watching, int isFirst);

  void removeClauseFromLiterialList(ClauseWatching &watching, int isFirst);

  // Literial list (for global information)
  vector<LiterialMeta> literialMetaList;
  priority_queue<LiterialMeta *, vector<LiterialMeta *>,
                 std::function<decltype(LiterialMeta::weightPtrComparator)>>
      unassignedLiterialQueue;

  // Queue for unique list item
  queue<ClauseWatching *> pendingUniqueClauseWatching;

  // Stack for assignment history
  vector<LiterialAssignment> literialAssignmentList;

  // utility functinos
  Bool clauseLiterialStatus(Clause &clause, int index) const;

  Bool watchingLiterialStatus(ClauseWatching &watching, bool isFirst) const;

  // Find next literial that can be watched
  // this will skip the status of current watched literials
  // return -1 if chrrent clause is already satisfied
  // return -2 if all other literials are assigned and failed
  int findNextWatchingLiterial(ClauseWatching &watching) const;

  void printLiterialMetaList();

  void printClauseWatchingList();

  int updateWatchingLiterial(LiterialMeta &litM, Bool assignValue);

  void rollbackAfterConflict();
};
}

#endif
