/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * clause.h
 * storage class representing a CNF clause in 
 *   a SAT problem
 **********************************************/
#ifndef CLAUSE_HEADER
#define CLAUSE_HEADER

#include "util.h"

namespace yasat {
class Literal;
class Bool {
public:
  Bool() : value(0u){};
  Bool(bool b) : Bool() { set(b); }
  Bool(unsigned int i) : Bool() { set(i); }
  // 1 => 3, 3=> 1,0 => 0
  Bool(int i, bool assign) : value(i) {
    if (!assign)
      invertValue();
  }
  inline void set(bool b) { value = b ? BOOL_TRUE : BOOL_FALSE; }
  inline void set(unsigned int i) {
    value = (value & (~0u << 2)) |
            (i & 1 ? (i & 2 ? BOOL_TRUE : BOOL_FALSE) : BOOL_UNASSIGN);
  }
  inline void setValue(int i) { value = i & (~0u << 2); }

  inline void invertValue() { value = value & 2 ? BOOL_FALSE : BOOL_TRUE; }

  inline operator bool() const { return (value & 2) != 0; }

  inline unsigned int getValue() const { return (value & 2) >> 1; }

  inline unsigned int isAssigned() const { return value & 1; }
  inline unsigned int isTrue() const { return (value & 2) >> 1; }

  // Value of variable
  // internal store form:
  // 0b00: unassigned value
  // 0b01: false value
  // 0b11: true value
  const static unsigned int BOOL_UNASSIGN = 0;
  const static unsigned int BOOL_TRUE = 3;
  const static unsigned int BOOL_FALSE = 1;
  inline static Bool getUnsignedValue(){
    const static Bool BOOL_UNASSIGNED_VALUE(BOOL_UNASSIGN);
    return BOOL_UNASSIGNED_VALUE;
  }
  inline static Bool getTrueValue(){
    const static Bool BOOL_TRUE_VALUE(BOOL_TRUE);
    return BOOL_TRUE_VALUE;
  }
  inline static Bool getFalseValue(){
    const static Bool BOOL_FALSE_VALUE(BOOL_FALSE);
    return BOOL_FALSE_VALUE;
  }

private:
  unsigned int value;

public:
  inline friend ostream &operator<<(ostream &out, Bool &b) {
    return out << (b.isAssigned() ? (b.isTrue() ? "TRUE" : "FALSE") : "UNDET");
  }
  friend Literal;
};

class Literal {
  // Use 3 bit for last state
public:
  const static unsigned LITERIAL_POSITIVE = 1 << 2;
  const static unsigned LITERIAL_NEGATIVE = 0 << 2;
  const static unsigned LITERIAL_FORM_MASK = ~4u;
  const static unsigned ASSIGNMENT_MASK = ~3u;
  Literal() : value(0) {}
  Literal(int num) : Literal() {
    if (num < 0)
      value = Bool::BOOL_UNASSIGN | LITERIAL_NEGATIVE | (-num << 3);
    else if (num > 0)
      value = Bool::BOOL_UNASSIGN | LITERIAL_POSITIVE | (num << 3);
  }
  Literal(int num, Bool assign) : Literal(num) {
    value &= ASSIGNMENT_MASK;
    value |= assign.value;
  }
  Literal(unsigned int num, unsigned int sign, unsigned int assign) {
    value = num << 3 | (sign & 1) << 2 | assign;
  }

  // Get integer representation form
  inline int getInt() const { return isPositive() ? getVal() : -getVal(); }

  // Get the literal
  inline int getVal() const { return value >> 3; }

  // whether the literal is positive (1) or negative (0)
  inline int isPositive() const { return (value & 4u) >> 2; }

  // get bool
  inline Bool getBool() const { return Bool(value, isPositive()); }

  typedef bool (*comparator)(Literal &, Literal &);
  inline static bool comparatorValue(Literal &a, Literal &b) {
    return a.value < b.value;
  }

private:
  // the value is aligned with the form of Bool class
  unsigned int value;
  inline friend ostream &operator<<(ostream &out, Literal lit) {
    return out << (lit.isPositive() ? fmt::positive : fmt::negative)
               << lit.getVal() << fmt::reset;
  }
};

class Clause {
public:
  Clause() {}
  ~Clause() {}

  inline vector<Literal> &getList() { return lits; }

  inline Literal &getLiteral(int at){
    return lits.at(at);
  }

  inline int getLiteralCount() const{
    return lits.size();
  }

  inline Literal& operator[](int at){
    return lits.at(at);
  }
private:
  vector<Literal> lits;
  inline friend ostream &operator<<(ostream &out, Clause cls) {
    for (auto i = cls.lits.begin(); i < cls.lits.end(); i++)
      out << *i << " ";
    return out;
  }
};
}

#endif // !CLAUSE_HEADER
