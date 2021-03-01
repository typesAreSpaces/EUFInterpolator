#ifndef _RENAME_
#define _RENAME_
#define DEBUG_RENAME 0

#include <string>
#include <set>
#include <vector>
#include <utility>
#include "z3++.h"

struct Rename {
  std::vector<bool> visited;
  z3::expr_vector   renamed_input;

  z3::expr        removePrefix(z3::expr const &) const;
  z3::expr_vector removePrefix(z3::expr_vector const &) const;

  Rename(z3::context &);
};


#endif
