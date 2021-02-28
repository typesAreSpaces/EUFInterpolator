#include "UnionFindExplain.h"
#include <ostream>

//ExplainEquation
//

UnionFindExplain::ExplainEquation::ExplainEquation(EqClass source, EqClass target) :
  source(source), target(target) 
{
}

//std::ostream & operator << (std::ostream & os, 
//ExplainEquation const & eq){
//return os << eq.source << " := " << eq.target;
//}
