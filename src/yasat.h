/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * yssat.h
 * main class manage the solver states
 **********************************************/

#ifndef YASAT_HEADER
#define YASAT_HEADER

#include "util.h"
#include "clause.h"

namespace yasat
{

class YaSat
{
public:
  YaSat();
  int parseArgument(char **arguments, int argumentCount);

  inline int getState() const;

  void printUsage() const;
  void printError() const;

  void loadClause();
  void solve();
  void printResult();

private:
  // State
  int state;
  int literialCount;
  vector<Clause> clauses;
  vector<Literial> solution;

  // IO destination
  istream *inputDataSource;
  ostream *outputResult;
  ostream *outputMessage;
  inline istream& dataSource();
  inline ostream& result();
  inline ostream& message();

};

inline int YaSat::getState() const
{
  return state;
}

inline istream& YaSat::dataSource(){
  return inputDataSource ? *inputDataSource : cin;
}
inline ostream& YaSat::result(){
  return outputResult ? *outputResult : cout;
}

inline ostream& YaSat::message(){
   return outputMessage ? *outputMessage : cout;
}

}
#endif // !YASAT_HEADER
