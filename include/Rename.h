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

struct RenameWithExpressions : public Rename {
  std::set<std::string> a_local_names, common_names;

  void traversePartA(z3::expr const &);
  void traversePartB(z3::expr const &);
  z3::expr reformulate(z3::expr const &);

  RenameWithExpressions(z3::expr_vector const &, z3::expr_vector const &);
};

struct RenameWithUncomSymbols : public Rename {
  std::set<std::string> const & uncommon_names;

  z3::expr reformulate(z3::expr const &);

  RenameWithUncomSymbols(z3::expr_vector const &, std::set<std::string> const &);
};

#endif
