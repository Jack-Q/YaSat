#include "clause.h"

namespace yasat{

Clause::Clause(string s) : Clause() {
  std::istringstream str(s);
  int number = -1;
  while (true) {
    str >> number;
    if (number == 0 || str.eof())
      break;
    lits.push_back(Literial(number));
  }
  if (number != 0)
    cout << fmt::warning << "Clause line not terminated with 0" << endl;
  if (lits.empty())
    cout << fmt::warning << "Empty clause line" << endl;
  for(auto i = lits.begin(); i < lits.end(); i++){
    cout << *i << " ";
  }
  cout << endl;
}
}
