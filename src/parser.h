#ifndef PARSER_HEADER
#define PARSER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class Parser {
public:
  Parser() {reset();}

  void parse(istream &src, vector<Clause> &cls);
  void reset() { header_lits = header_lines = clause_lines = maxLiterial = 0; }

  inline int getMaxLiterial() const {return maxLiterial;}
private:
  int header_lines;
  int header_lits;
  int clause_lines;
  int maxLiterial;

  Clause parseClause(string& s);
};
}

#endif
