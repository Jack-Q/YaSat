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
      cout << "Header says: " << s << " file type with " << header_lines
           << " clauses, " << header_lits << " literials" << endl;
      src.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    default:
      std::getline(src >> std::ws, s);
      cls.push_back(Clause(s));
      clause_lines++;
    }
  }
  if (header_lines != clause_lines) {
    cout << fmt::warningLabel << "Clause count and file header mismatch! "
         << endl;
  }
}
}
