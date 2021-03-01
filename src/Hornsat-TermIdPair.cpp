#include "Hornsat.h"

Hornsat::TermIdPair::TermIdPair(TermId lhs, TermId rhs) : 
  lhs(lhs), rhs(rhs) 
{
}

std::ostream & operator << (std::ostream & os, Hornsat::TermIdPair const & tp){
  return (os << tp.lhs << " " << tp.rhs);
}

