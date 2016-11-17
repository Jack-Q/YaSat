#include "clause.h"

namespace yasat{


// parsing literials in a clause in CNF file (one line)
// and construct the clause object
Clause::Clause(string s) : Clause() {
  std::istringstream str(s);
  int number = -1;
  while (true) {
    str >> number;
    if (number == 0)
      break;
    lits.push_back(Literial(number));
    str >> std::ws;
    if(str.eof())
      break;
  }
  if (number != 0)
    cout << fmt::warningLabel << "Clause line not terminated with 0" << endl;
  if (lits.empty())
    cout << fmt::warningLabel << "Empty clause line" << endl;
  if (!str.eof())
    cout << fmt::warningLabel << "Literials after 0 in a CNF line is ignored" << endl;
  for(auto i = lits.begin(); i < lits.end(); i++){
    cout << *i << " ";
  }
  cout << endl;
}
}
