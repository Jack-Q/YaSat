#ifndef SOLVER_HEADER
#define SOLVER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class LiterialMeta {
public:
  LiterialMeta() : weight(0), weightPositive(0), weightNegative(0) {}

  int listValue;
  Bool assignmet;

  // static weight, occrence time in clause less then 10 variable
  int weight;
  int weightPositive;
  int weightNegative;

  vector<Clause *> positiveList;
  vector<Clause *> negativeList;
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

private:
};

// Assignment of literial, including decisoin and implcation
class LiterialAssignment {
public:
  static const int LITERIAL_ASSIGN_DECISION = 1;
  static const int LITERIAL_ASSIGN_IMPLICATION = 0;
  LiterialAssignment(LiterialMeta &litMeta,
                     int type = LITERIAL_ASSIGN_IMPLICATION)
      : type(type), litM(litMeta) {}

  inline bool isDecision() const { return type == LITERIAL_ASSIGN_DECISION; }

private:
  int type;
  LiterialMeta &litM;
};

class Solver {
public:
  Solver(vector<Clause> &cls, int maxLit, ostream &message)
      : msg(message), maxLiterial(maxLit), clauses(cls),
        literialMetaList(maxLit),
        unassignedLiterialQueue(LiterialMeta::weightPtrComparator) {
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

  // Clause list
  vector<Clause> &clauses;
  vector<ClauseWatching> clauseWatchingList;
  void addClauseToLiteralList(ClauseWatching &watching, int isFirst) {
    Literial &lit =
        watching
            .clause[isFirst ? watching.firstWatching : watching.secondWatching];
    LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
    if (lit.isPositive())
      litM.positiveList.push_back(&watching.clause);
    else
      litM.negativeList.push_back(&watching.clause);
  }
  void removeClauseFromLiterialList(ClauseWatching &watching, int isFirst) {
    Literial &lit =
        watching
            .clause[isFirst ? watching.firstWatching : watching.secondWatching];
    LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
    if (lit.isPositive())
      litM.positiveList.erase(std::remove(litM.positiveList.begin(),
                                          litM.positiveList.end(),
                                          &watching.clause),
                              litM.positiveList.end());
    else
      litM.negativeList.erase(std::remove(litM.negativeList.begin(),
                                          litM.negativeList.end(),
                                          &watching.clause),
                              litM.negativeList.end());
  }

  // Literial list (for global information)
  vector<LiterialMeta> literialMetaList;
  priority_queue<LiterialMeta *, vector<LiterialMeta *>,
                 std::function<decltype(LiterialMeta::weightPtrComparator)>>
      unassignedLiterialQueue;

  queue<ClauseWatching *> pendingUniqueClauseWatching;
  // Stack for assignment history
  vector<LiterialAssignment> literialAssignmentList;

  // utility functinos
  inline Bool clauseLiterialStatus(Clause &clause, int index) const{
    Literial &lit = clause[index];
    const LiterialMeta &litM = literialMetaList[lit.getVal() - 1];
    if (litM.assignmet.isAssigned()) {
      return litM.assignmet.isTrue() ^ lit.isPositive()
                 ? Bool::getFalseValue()
                 : Bool::getTrueValue();
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
      return true;
    }
    return clauseLiterialStatus(watching.clause, watchingIndex);
  }

  // Find next literial that can be watched
  // this will skip the status of current watched literials
  // return -1 if all other literials are assigned
  inline int findNextWatchingLiterial(ClauseWatching &watching) const {
    int i = max(watching.firstWatching, watching.secondWatching);
    for(int j = i + 1; j != i; j = (j + 1) % watching.clause.getLiterialCount()){
      if(j == watching.firstWatching || watching.secondWatching) continue;
      if(!clauseLiterialStatus(watching.clause, j).isAssigned()) return j;
    }
    return -1;
  }


  void printLiterialMetaList();
  void printClauseWatchingList();
};
}

#endif
