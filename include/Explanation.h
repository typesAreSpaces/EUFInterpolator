#ifndef _EXPLAINATION_
#define _EXPLAINATION_

#define IsMember(set, elem) set.find(elem) != set.end()

#include <set>
#include "z3++.h"

struct Explanation 
{
  z3::expr_vector    result;
  std::set<unsigned> current_ids;

  Explanation(z3::context &);
  ~Explanation();
  void add(z3::expr_vector const &);
};

#endif
