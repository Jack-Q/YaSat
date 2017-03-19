/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * solver.h
 * declaraiton of core state classes of YaSat
 **********************************************/
#ifndef SOLVER_HEADER
#define SOLVER_HEADER
#include "clause.h"
#include "util.h"

namespace yasat {

class ClauseWatching;
class LiteralAssignment;

class LiteralMeta {
public:
  LiteralMeta() : weight(0), weightPositive(0), weightNegative(0) {}

  int listValue;
  Bool assignment;
  LiteralAssignment *assignmentStatus = nullptr;

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

  LiteralAssignment(LiteralMeta &litMeta, int level, Clause *ant)
      : LiteralAssignment(litMeta, level, LITERIAL_ASSIGN_IMPLICATION, ant) {}

  LiteralAssignment(LiteralMeta &litMeta, int level,
                    int type = LITERIAL_ASSIGN_DECISION,
                    Clause *antecedent = nullptr)
      : type(type), firstAssign(true), litM(litMeta), antecedent(antecedent),
        assignmentLevel(level) {}

  inline bool isDecision() const { return type == LITERIAL_ASSIGN_DECISION; }
  inline bool isFirstAssign() const { return firstAssign; }
  inline void nextAssign() { firstAssign = false; }
  inline LiteralMeta &getLiteralMeta() const { return litM; }
  inline Clause *getAntecedent() const { return antecedent; }
  inline int getAssignmentLevel() const { return assignmentLevel; }

private:
  const int type;
  // Each decision assignment ought to have two possible decisions to try
  bool firstAssign;
  LiteralMeta &litM;
  Clause *antecedent;
  const int assignmentLevel;
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

  int statis_conflict = 0;
  int statis_decision = 0;
  int statis_implication = 0;
  int statis_backtracklength = 0;

  int assignmentLevel = 0;
  bool rollback = false;
  bool unsatisfiable = false;

  // Compartor for heuristical ordering
  static const std::function<bool(LiteralMeta const *, LiteralMeta const *)>
      literalCompartor;

  // Clause list
  vector<unique_ptr<Clause>> &clauses;
  vector<unique_ptr<Clause>> leartClauses;
  vector<unique_ptr<ClauseWatching>> clauseWatchingList;

  void addClauseToLiteralList(ClauseWatching &watching, int isFirst);

  void removeClauseFromLiteralList(ClauseWatching &watching, int isFirst);

  // Literal list (for global information)
  vector<LiteralMeta> literalMetaList;
  vector<LiteralMeta *> literalMetaPtrOrderList;
  bool literalMetaPtrOrderListInHeapOrder = false;

  // Queue for unique list item
  queue<ClauseWatching *> pendingUniqueClauseWatching;

  // Stack for assignment history
  vector<unique_ptr<LiteralAssignment>> literalAssignmentList;
  inline LiteralAssignment &newImplication(LiteralMeta &litM, Clause &clause) {
    literalAssignmentList.push_back(
        make_unique<LiteralAssignment>(litM, assignmentLevel, &clause));
    litM.assignmentStatus = literalAssignmentList.back().get();
    return *literalAssignmentList.back();
  }
  inline LiteralAssignment &newDecision(LiteralMeta &litM) {
    literalAssignmentList.push_back(
        make_unique<LiteralAssignment>(litM, ++assignmentLevel));
    litM.assignmentStatus = literalAssignmentList.back().get();
    return *literalAssignmentList.back();
  }

  // Delete an assignment from the list and reset to unassigned state
  // as well as push back to the list
  inline void deleteLastAssignment() {
    auto &meta = literalAssignmentList.back()->getLiteralMeta();
    meta.assignment = Bool::Bool::getUnsignedValue();
    meta.assignmentStatus = nullptr;
    literalAssignmentList.pop_back();

#if defined(DEBUG) && defined(DEBUG_VERBOSE)
    msg << fmt::messageLabel << fmt::error << "Push" << fmt::reset
        << " unassigned literal " << meta.listValue << " into list" << endl;
#endif
    literalMetaPtrOrderList.push_back(&meta);
    std::push_heap(literalMetaPtrOrderList.begin(),
                   literalMetaPtrOrderList.end(), literalCompartor);
  }

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

  ClauseWatching *updateWatchingLiteral(LiteralMeta &litM, Bool assignValue);

  inline bool isAssignedAtCurrentLevel(Literal &lit) const {
    auto &meta = literalMetaList.at(lit.getVal() - 1);
    return meta.assignment.isAssigned() &&
           (meta.assignmentStatus->getAssignmentLevel() == assignmentLevel);
  }

  void rollbackAfterConflict(Clause *antecedent);
  inline void literalWeightDecay() {
#if defined(LITERAL_WEIGHT_DECAY)
    static int last_decay = 0;
    if (statis_conflict % 200 == 0 && statis_conflict != last_decay) {
#if defined(DEBUG) && defined(DEBUG_VERBOSE)
      msg << fmt::error << "Decay Weight @ " << statis_conflict << fmt::reset
          << endl;
#endif
      last_decay = statis_conflict;
      for (auto i = literalMetaList.begin(); i < literalMetaList.end(); i++)
        // TODO:  decay rate ought to be set by outer setting
        i->weight = i->weight * 4 / 3;
    }
#endif
  }
  // Update literal weight when a new clause is added
  // return false if all literal is untouched
  inline bool updateLiteralWeightWithClause(Clause *clause) {
    int clauseWeight = 15 - clause->getLiteralCount();
    if (clauseWeight <= 0)
      return false;
    for (auto lit = clause->getList().begin(); lit != clause->getList().end();
         lit++) {
      literalMetaList.at(lit->getVal() - 1).weight += clauseWeight;
      // if (lit->isPositive()) {
      //   literalMetaList.at(lit->getVal() - 1).weightPositive += clauseWeight;
      //   // literalMetaList.at(lit->getVal() - 1)
      //   //     .positiveList.push_back(&*clause);
      // } else {
      //   literalMetaList.at(lit->getVal() - 1).weightNegative += clauseWeight;
      //   // literalMetaList.at(lit->getVal() - 1)
      //   //     .negativeList.push_back(&*clause);
      // }
    }
    return true;
  }

  inline void printStatisData() {
#if defined(PRINT_STATIS)
    msg << fmt::statisLabel << "IMP: " << fmt::message << statis_implication
        << fmt::reset << "\tDEC: " << fmt::message << statis_decision
        << fmt::reset << "\tCON: " << fmt::message << statis_conflict
        << fmt::reset << "\tLER: " << fmt::message << leartClauses.size()
        << fmt::reset << "\tMAX_BACK: " << fmt::message
        << statis_backtracklength << fmt::reset << endl;
#endif
  }
};
}

#endif
