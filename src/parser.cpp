#include "parser.h"

namespace yasat {
void Parser::parse(istream &src, vector<Clause> &cls) {
  string s;
  char c;
  while (true) {
    src >> std::ws;
    if (src.eof())
      break;
    c = src.peek();
    switch (c) {
    case 'c':
      src.ignore();
      std::getline(src >> std::ws, s);
      cout << s << endl;
      continue;
    case 'p':
      src.ignore();
      src >> s >> header_lits >> header_lines;
      src.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    default:
      std::getline(src >> std::ws, s);
      Clause curClause = parseClause(s);
      if (curClause.getLiteralCount() > 0){
        cls.push_back(curClause);
        clause_lines++;
      }

    }
  }

  // Check the header line
  if (header_lines != clause_lines) {
    cout << fmt::warningLabel << "Clause count and file header mismatch! "
         << endl
         << "     Expecting " << fmt::warning << header_lines << fmt::reset
         << " literals, read " << fmt::error << clause_lines << fmt::reset
         << " literals." << endl
         << "     The latter is used." << endl;
  }
  if (header_lits != maxLiteral) {
    cout << fmt::warningLabel
         << "Actual number of literals is not as the header says." << endl
         << "     Expecting " << fmt::warning << header_lits << fmt::reset
         << " literals, read " << fmt::error << maxLiteral << fmt::reset
         << " literals." << endl
         << "     The latter is used." << endl;
  }
}

Clause Parser::parseClause(string &s) {
  Clause cls = Clause();
  vector<Literal> &lits = cls.getList();
  std::istringstream str(s);

  int number = -1;
  while (true) {
    str >> number;
    if (number == 0)
      break;
    lits.push_back(Literal(number));
    if (abs(number) > maxLiteral)
      maxLiteral = number;
    str >> std::ws;
    if (str.eof())
      break;
  }

  if (number != 0)
    cout << fmt::warningLabel << "Clause line not terminated with 0" << endl;
  if (lits.empty())
    cout << fmt::warningLabel << "Empty clause line" << endl;
  if (!str.eof())
    cout << fmt::warningLabel << "Literals after 0 in a CNF line is ignored"
         << endl;

  return cls;
}
}
