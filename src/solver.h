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
  void addClauseToLiteralList(ClauseWatching &watching, int isFirst) {
    Literial &lit =
        watching
            .clause[isFirst ? watching.firstWatching : watching.secondWatching];
    LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
    if (lit.isPositive())
      litM.positiveList.push_back(&watching);
    else
      litM.negativeList.push_back(&watching);
  }
  void removeClauseFromLiterialList(ClauseWatching &watching, int isFirst) {
    Literial &lit =
        watching
            .clause[isFirst ? watching.firstWatching : watching.secondWatching];
    LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
    if (lit.isPositive())
      litM.positiveList.erase(std::remove(litM.positiveList.begin(),
                                          litM.positiveList.end(), &watching),
                              litM.positiveList.end());
    else
      litM.negativeList.erase(std::remove(litM.negativeList.begin(),
                                          litM.negativeList.end(), &watching),
                              litM.negativeList.end());
  }

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
  inline Bool clauseLiterialStatus(Clause &clause, int index) const {
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

  inline Bool watchingLiterialStatus(ClauseWatching &watching, bool isFirst) {
    int watchingIndex =
        isFirst ? watching.firstWatching : watching.secondWatching;
    if (watchingIndex == -1) {
      // Even though there is no watched variable, this one is satisfied, anyway
      return Bool::getTrueValue();
    }
    return clauseLiterialStatus(watching.clause, watchingIndex);
  }

  // Find next literial that can be watched
  // this will skip the status of current watched literials
  // return -1 if chrrent clause is already satisfied
  // return -2 if all other literials are assigned and failed
  inline int findNextWatchingLiterial(ClauseWatching &watching) const {
    int i = max(watching.firstWatching, watching.secondWatching);
    int c = watching.clause.getLiterialCount();
    msg << fmt::messageLabel << "Find next watching item in " << watching
        << endl;
    for (int j = (i + 1) % c; j != i; j = (j + 1) % c) {
      if ((j == watching.firstWatching) || (j == watching.secondWatching))
        continue;
      Bool litStatus = clauseLiterialStatus(watching.clause, j);
      if (litStatus.isAssigned()){
        if(litStatus.isTrue()){
          // since current clause is already satisfied, no further searching is required
          return -1;
        }
        continue;
      }else{
        // find an unsigned literial
        return j;
      }
    }
    // no result found
    return -2;
  }

  void printLiterialMetaList();
  void printClauseWatchingList();

  int updateWatchingLiterial(LiterialMeta &litM, Bool assignValue);

  void rollbackAfterConflict();
};
}

#endif
