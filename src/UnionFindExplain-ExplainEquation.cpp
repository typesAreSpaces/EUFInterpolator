#include "UnionFindExplain.h"
#include <ostream>

UnionFindExplain::ExplainEquation::ExplainEquation(EqClass source, EqClass target) :
  source(source), target(target) 
{
}

std::ostream & operator << (std::ostream & os, 
    UnionFindExplain::ExplainEquation const & eq){
  return os << eq.source << " := " << eq.target;
}
