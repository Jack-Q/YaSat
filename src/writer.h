#ifndef WRITER_HEADER
#define WRITER_HEADER

#include "util.h"
#include "clause.h"

namespace yasat{
class Writer{
public:
  Writer(ostream &message): msg(message){}

  void write(ostream &out, vector<Literial> &literials);
private:
  ostream &msg;
};
}

#endif
