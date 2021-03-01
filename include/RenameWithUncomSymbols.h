#ifndef _RENAME_W_U_
#define _RENAME_W_U_

#include "Rename.h"

struct RenameWithUncomSymbols : public Rename {
  std::set<std::string> const & uncommon_names;

  z3::expr reformulate(z3::expr const &);

  RenameWithUncomSymbols(z3::expr_vector const &, std::set<std::string> const &);
};

#endif
