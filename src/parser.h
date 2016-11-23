#ifndef PARSER_HEADER
#define PARSER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class Parser {
public:
  Parser(ostream &message) : msg(message) { reset(); }

  void parse(istream &src, vector<Clause> &cls);
  void reset() { header_lits = header_lines = clause_lines = maxLiteral = 0; }

  inline int getMaxLiteral() const { return maxLiteral; }

private:
  int header_lines;
  int header_lits;
  int clause_lines;
  int maxLiteral;

  Clause parseClause(string &s);
  ostream &msg;
};
}

#endif
