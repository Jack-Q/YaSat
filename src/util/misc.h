#ifndef MISC_HEADER
#define MISC_HEADER

#include <algorithm>
#include <cctype>
#include <cstring>
#include <functional>
#include <limits>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

namespace yasat {
using ::std::string;
using ::std::vector;
using ::std::sort;
using ::std::queue;
using ::std::priority_queue;

inline int max(int a, int b) { return a > b ? a : b; }
}
#endif
