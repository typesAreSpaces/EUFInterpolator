#include "EUFInterpolantWithExpressions.h"

EUFInterpolantWithExpressions::EUFInterpolantWithExpressions(z3::expr_vector const & input_a, z3::expr_vector const & input_b) : 
  RenameWithExpressions(input_a, input_b), EUFInterpolant(renamed_input)
{
}

EUFInterpolantWithExpressions::~EUFInterpolantWithExpressions()
{
}

std::ostream & operator << (std::ostream & os, EUFInterpolantWithExpressions const & eufiexp){
  return os << eufiexp.removePrefix(eufiexp.getInterpolant());
}
