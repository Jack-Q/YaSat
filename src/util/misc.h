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
#include <memory>

namespace yasat {
using ::std::string;
using ::std::vector;
using ::std::sort;
using ::std::queue;
using ::std::priority_queue;

using ::std::move;
using ::std::unique_ptr;
using ::std::make_unique;

inline int max(int a, int b) { return a > b ? a : b; }
}
#endif
