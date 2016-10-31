#ifndef YASAT_HEADER
#define YASAT_HEADER

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
  std::istream *clauseSource;
};

inline int YaSat::getState() const
{
  return state;
}
}
#endif // !YASAT_HEADER