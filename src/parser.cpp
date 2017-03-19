/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * parser.cpp
 * implementation of the DIMACS parser
 **********************************************/
#include "parser.h"

namespace yasat {
void Parser::parse(istream &src, vector<unique_ptr<Clause>> &cls) {
  string s, temp;
  char c;
  while (true) {
    src >> std::ws;
    if (src.eof())
      break;
    c = src.peek();
    switch (c) {
    case 'c':
      std::getline(src >> c, s);
      cout << fmt::message << "[COMMENT]" << fmt::reset << s << endl;
      continue;
    case 'p':
      std::getline(src >> c, s);
      {
        std::istringstream iss(s);
        iss >> temp >> header_lits >> header_lines;
      }
      continue;
    default:
#ifdef PARSE_EXT_COMPAT
      if (c == '-' || c == '+' || (c >= '0' && c <= '9')) {
        // Data Section
        int number;
        Clause curClause;
        vector<Literal> &lits = curClause.getList();
        while (!src.eof()) {
          src >> number;
          if (number == 0) {
            if (lits.size() > 0) {
              cls.push_back(make_unique<Clause>(curClause));
              clause_lines++;
            }
            lits.clear();
            continue;
          }
          lits.push_back(Literal(number));
          if (abs(number) > maxLiteral)
            maxLiteral = number;
          src >> std::ws;
        }
        if (lits.size() > 0) {
          cls.push_back(make_unique<Clause>(curClause));
          clause_lines++;
        }
      }
#else
      std::getline(src >> std::ws, s);
      unique_ptr<Clause> clsPtr = make_unique<Clause>();
      parseClause(*clsPtr, s);
      if (clsPtr->getLiteralCount() > 0) {
        cls.push_back(move(clsPtr));
        clause_lines++;
      }
#endif
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

void Parser::parseClause(Clause &cls, string &s) {
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

}
}
