#ifndef _EUF_INTERPOLANT_WE_
#define _EUF_INTERPOLANT_WE_

#include "EUFInterpolant.h"

class EUFInterpolantWithExpressions : public RenameWithExpressions, public EUFInterpolant {

  public:
  EUFInterpolantWithExpressions(z3::expr_vector const &, z3::expr_vector const &);
  ~EUFInterpolantWithExpressions();

  friend std::ostream & operator << (std::ostream &, EUFInterpolantWithExpressions const &);

};

#endif
