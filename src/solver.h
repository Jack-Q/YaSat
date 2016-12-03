#ifndef SOLVER_HEADER
#define SOLVER_HEADER
#include "clause.h"
#include "util.h"

namespace yasat {

class ClauseWatching;

class LiteralMeta {
public:
  LiteralMeta() : weight(0), weightPositive(0), weightNegative(0) {}

  int listValue;
  Bool assignmet;

  // static weight, occrence time in clause less then 10 variable
  int weight;
  int weightPositive;
  int weightNegative;

  vector<ClauseWatching *> positiveList;
  vector<ClauseWatching *> negativeList;
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
    for (int j = 0; j < w.clause.getLiteralCount(); j++) {
      if (j == w.firstWatching || j == w.secondWatching) {
        o << " [" << w.clause[j] << "] ";
      } else {
        o << "  " << w.clause[j] << "  ";
      }
    }
    return o;
  }
};

// Assignment of literal, including decisoin and implcation
class LiteralAssignment {
public:
  static const int LITERIAL_ASSIGN_DECISION = 1;
  static const int LITERIAL_ASSIGN_IMPLICATION = 0;

  LiteralAssignment(LiteralMeta &litMeta, Clause *ant)
      : LiteralAssignment(litMeta, LITERIAL_ASSIGN_IMPLICATION, ant) {}

  LiteralAssignment(LiteralMeta &litMeta,
                    int type = LITERIAL_ASSIGN_IMPLICATION,
                    Clause *antecedent = nullptr)
      : type(type), firstAssign(true), litM(litMeta), antecedent(antecedent) {}

  inline bool isDecision() const { return type == LITERIAL_ASSIGN_DECISION; }
  inline bool isFirstAssign() const { return firstAssign; }
  inline void nextAssign() { firstAssign = false; }
  inline LiteralMeta &getLiteralMeta() const{ return litM; }
  inline Clause *getAntecedent() const {return antecedent; }

private:
  int type;
  // Each decision assignment ought to have two possible decisions to try
  bool firstAssign;
  LiteralMeta &litM;
  Clause *antecedent;
};

class Solver {
public:
  Solver(vector<unique_ptr<Clause>> &cls, int maxLit, ostream &message)
      : msg(message), maxLiteral(maxLit), clauses(cls), literalMetaList(maxLit),
        literalMetaPtrOrderList(maxLit) {
    int i = 1;
    for (auto litm = literalMetaList.begin(); litm != literalMetaList.end();
         litm++) {
      litm->listValue = i++;
    }
  }

  void prep();

  void solve();

  void getSolution(vector<Literal> &sol);

private:
  // informative
  ostream &msg;

  int maxLiteral;
  bool rollback = false;
  bool unsatisfiable = false;

  // Clause list
  vector<unique_ptr<Clause>> &clauses;
  vector<unique_ptr<ClauseWatching>> clauseWatchingList;

  void addClauseToLiteralList(ClauseWatching &watching, int isFirst);

  void removeClauseFromLiteralList(ClauseWatching &watching, int isFirst);

  // Literal list (for global information)
  vector<LiteralMeta> literalMetaList;
  vector<LiteralMeta *> literalMetaPtrOrderList;

  // Queue for unique list item
  queue<ClauseWatching *> pendingUniqueClauseWatching;

  // Stack for assignment history
  vector<LiteralAssignment> literalAssignmentList;

  // utility functinos
  Bool clauseLiteralStatus(Clause &clause, int index) const;

  Bool watchingLiteralStatus(ClauseWatching &watching, bool isFirst) const;

  // Find next literal that can be watched
  // this will skip the status of current watched literals
  // return -1 if chrrent clause is already satisfied
  // return -2 if all other literals are assigned and failed
  int findNextWatchingLiteral(ClauseWatching &watching) const;

  void printLiteralMetaList();

  void printClauseWatchingList();

  int updateWatchingLiteral(LiteralMeta &litM, Bool assignValue);

  void rollbackAfterConflict();
};
}

#endif
