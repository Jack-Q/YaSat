#ifndef EXCEPTION_HEADER
#define EXCEPTION_HEADER

#include <string>
#include <stdexcept>

namespace yasat
{
class Exception : public std::exception
{
  public:
    explicit Exception(const char *msg, int lv = 1) : level(lv), message(msg) {}
    explicit Exception(const std::string &msg, int lv = 1) : level(lv), message(msg) {}

    virtual const char *what() const throw()
    {
        return message.c_str();
    }

    inline int getLevel() const throw()
    {
        return level;
    }

    virtual int action() const throw() {return 0;}

    virtual ~Exception() throw() {}

  protected:
    int level;
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
}
#endif // !EXCEPTION_HEADER