#ifndef _RENAME_W_E_
#define _RENAME_W_E_

#include "Rename.h"

struct RenameWithExpressions : public Rename {
  std::set<std::string> a_local_names, common_names;

  void traversePartA(z3::expr const &);
  void traversePartB(z3::expr const &);
  z3::expr reformulate(z3::expr const &);

  RenameWithExpressions(z3::expr_vector const &, z3::expr_vector const &);
};

#endif
