#ifndef PARSER_HEADER
#define PARSER_HEADER

#include "clause.h"
#include "util.h"

namespace yasat {
class Parser {
public:
  Parser() : header_lines(0), header_lits(0), clause_lines(0) {}

  void parse(istream &src, vector<Clause> &cls);
  void reset() { header_lits = header_lines = clause_lines = 0; }

private:
  int header_lines;
  int header_lits;
  int clause_lines;
};
}

#endif
