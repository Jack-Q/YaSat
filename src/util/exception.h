/**********************************************
 *  YaSat -- Yet Another SAT Solver
 *
 *  Jack Q (qiaobo@outlook.com)
 *  (0540017, CS, NCTU)
 **********************************************
 * util/exception.h
 * C++ exception definition for parser
 **********************************************/
#ifndef EXCEPTION_HEADER
#define EXCEPTION_HEADER

#include <stdexcept>
#include <string>

namespace yasat
{
class Exception : public std::exception
{
public:
  explicit Exception(const char *msg, int lv = 1, bool showUsage = false) : level(lv), isShowUsage(showUsage), message(msg){}
  explicit Exception(const std::string &msg, int lv = 1, bool showUsage = false) : level(lv), isShowUsage(showUsage), message(msg) {}

  virtual const char *what() const throw()
  {
    return message.c_str();
  }

  inline int getLevel() const throw()
  {
    return level;
  }

  inline bool showUsage() const throw()
  {
    return isShowUsage;
  }

  virtual int action() const throw() { return 0; }

  virtual ~Exception() throw() {}

protected:
  int level;
  bool isShowUsage;
  std::string message;
};

class RoutineException : public Exception
{
public:
  enum Type
  {
    USAGE,
    CANCELLATION
  };
  explicit RoutineException(Type t) : Exception("Routine Exception", 0), type(t) {}
  Type getType() const { return type; }
private:
  Type type;
};

class OptionException: public Exception {
public:
  explicit OptionException(const std::string &msg): Exception(msg, 1, true) {}
  explicit OptionException(const char *msg): Exception(msg, 1, true) {}
};
}
#endif // !EXCEPTION_HEADER
