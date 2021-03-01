#include "CongruenceClosure.h"

CongruenceClosure::CongruenceClosure(const Z3Subterms & subterms,
				     UnionFindExplain & ufe) :
  subterms(subterms), 
  ufe(ufe)
{
}

CongruenceClosure::~CongruenceClosure(){
#if DEBUG_DESTRUCTORS_CC
  std::cout << "Done ~CongruenceClosure" << std::endl;
#endif
}

std::ostream & operator << (std::ostream & os, const CongruenceClosure & cc){
  return os;
}
