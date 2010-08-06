#ifndef BASE_BASE_H__
#define BASE_BASE_H__

#define BOOST_SPIRIT_THREADSAFE 1

// C standard library headers
#include <stdlib.h>

// C++ standard library headers
#include <string>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <vector>

#ifdef _WINDOWS
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include "onyx/base/macros.h"
#include "onyx/base/scoped_ptr.h"
#include "onyx/base/shared_ptr.h"

namespace boost {}

using base::scoped_ptr;

using std::list;
using std::map;
using std::pair;
using std::set;
using std::stack;
using std::vector;
using std::tr1::unordered_map;

namespace base{

typedef std::string string;

}

#endif // BASE_BASE_H__
