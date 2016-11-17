#ifndef CLAUSE_HEADER
#define CLAUSE_HEADER

#include "util.h"

namespace yasat {
class Literial;
class Bool {
public:
  Bool() : value(0u){};
  Bool(bool b) : Bool() { set(b); }
  Bool(int i) : Bool() { set(i); }
  inline void set(bool b) { value = b ? BOOL_TRUE : BOOL_FALSE; }
  inline void set(int i) {
    value = (value & (~0u << 2)) |
            (i & 1 ? (i & 2 ? BOOL_TRUE : BOOL_FALSE) : BOOL_UNASSIGN);
  }

private:
  // Value of variable
  // internal store form:
  // 0b00: unassigned value
  // 0b01: false value
  // 0b11: true value
  const static unsigned int BOOL_UNASSIGN = 0;
  const static unsigned int BOOL_TRUE = 3;
  const static unsigned int BOOL_FALSE = 1;
  unsigned int value;

public:
  inline friend ostream &operator<<(ostream &out, Bool &b) {
    return out << (b.value & 1 ? (b.value & 2 ? "TRUE" : "FALSE") : "UNDET");
  }
  friend Literial;
};

class Literial {
public:
  Literial() : value(0) {}
  Literial(int num) : Literial() {
    if (num < 0)
      value = Bool::BOOL_FALSE | (-num << 2);
    else if (num > 0)
      value = Bool::BOOL_TRUE | (num << 2);
  }

private:
  int value;
  inline friend ostream &operator<<(ostream& out, Literial lit) {
    return out <<( lit.value & 2 ? fmt::positive : fmt::negative )<<(lit.value >> 2) << fmt::reset;
  }
};

class Clause {
public:
  Clause() {}
  Clause(string s);
  ~Clause() {}

private:
  vector<Literial> lits;
};
}

#endif // !CLAUSE_HEADER
