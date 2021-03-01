#ifndef _EUF_INTERPOLANT_US_
#define _EUF_INTERPOLANT_US_

#include "EUFInterpolant.h"
#include "RenameWithUncomSymbols.h"

class EUFInterpolantWithUncomSymbols: public RenameWithUncomSymbols, public EUFInterpolant {

  public:
    EUFInterpolantWithUncomSymbols(z3::expr_vector const &, std::set<std::string> const &);
    ~EUFInterpolantWithUncomSymbols();
};

#endif
