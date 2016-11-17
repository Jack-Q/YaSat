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
  int state;
  istream *clauseSource;
};

inline int YaSat::getState() const
{
  return state;
}

}
#endif // !YASAT_HEADER
