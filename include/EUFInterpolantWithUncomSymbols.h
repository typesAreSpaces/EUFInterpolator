#ifndef _EUF_INTERPOLANT_US_
#define _EUF_INTERPOLANT_US_

#include "Rename.h"
#include "EUFInterpolant.h"

class EUFInterpolantWithUncomSymbols: public RenameWithUncomSymbols, public EUFInterpolant {

  public:
    EUFInterpolantWithUncomSymbols(z3::expr_vector const &, std::set<std::string> const &);
    ~EUFInterpolantWithUncomSymbols();
};

#endif
