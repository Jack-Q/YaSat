/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * parser.h
 * Parser to read SAT problem from an ascii text 
 *   file in DIMACS format
 **********************************************/
#ifndef PARSER_HEADER
#define PARSER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class Parser {
public:
  Parser(ostream &message) : msg(message) { reset(); }

  void parse(istream &src, vector<unique_ptr<Clause>> &cls);
  void reset() { header_lits = header_lines = clause_lines = maxLiteral = 0; }

  inline int getMaxLiteral() const { return maxLiteral; }

private:
  int header_lines;
  int header_lits;
  int clause_lines;
  int maxLiteral;

  void parseClause(Clause &, string &s);
  ostream &msg;
};
}

#endif
