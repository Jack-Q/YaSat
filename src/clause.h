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
  // 1 => 3, 3=> 1,0 => 0
  Bool(int i, bool assign) : value(i){
    if(assign) ;
  }
  inline void set(bool b) { value = b ? BOOL_TRUE : BOOL_FALSE; }
  inline void set(int i) {
    value = (value & (~0u << 2)) |
            (i & 1 ? (i & 2 ? BOOL_TRUE : BOOL_FALSE) : BOOL_UNASSIGN);
  }
  inline void setValue(int i){
    value = i & (~0u << 2);
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
  // Use 3 bit for last state
public:
  const static unsigned LITERIAL_POSITIVE = 1 << 3;
  const static unsigned LITERIAL_NEGATIVE = 0 << 3;
  const static unsigned LITERIAL_FORM_MASK = (~0u << 3 ) | 3;
  Literial() : value(0) {}
  Literial(int num) : Literial() {
    if (num < 0)
      value = Bool::BOOL_FALSE | (-num << 2);
    else if (num > 0)
      value = Bool::BOOL_TRUE | (num << 2);
  }

  inline int getInt() const{
    return (value & 2u >> 1) * (value >> 2);
  }

  inline int getVal() const {
    return value >> 2;
  }

  inline Bool getBool() const {
    return Bool(value, false);
  }

private:
  // the value is aligned with the form of Bool class
  unsigned int value;
  inline friend ostream &operator<<(ostream &out, Literial lit) {
    return out << (lit.value & 2u ? fmt::positive : fmt::negative)
               << (lit.value >> 2) << fmt::reset;
  }

};

class Clause {
public:
  Clause() {}
  ~Clause() {}

  inline vector<Literial> &getList(){return lits;}
private:
  vector<Literial> lits;
  inline friend ostream &operator<<(ostream &out, Clause cls) {
    for (auto i = cls.lits.begin(); i < cls.lits.end(); i++)
      out << *i << " ";
    return out;
  }
};
}

#endif // !CLAUSE_HEADER
