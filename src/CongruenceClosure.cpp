#include "CongruenceClosure.h"

CongruenceClosure::CongruenceClosure(const Z3Subterms & subterms,
				     UnionFindExplain & ufe) :
  subterms(subterms), ufe(ufe), sig_table(ufe)
{
}

CongruenceClosure::~CongruenceClosure(){
#if DEBUG_DESTRUCTORS_CC
  std::cout << "Done ~CongruenceClosure" << std::endl;
#endif
}

UnionFindExplain & CongruenceClosure::getUnionFindExplain() const {
  return ufe;
}

std::ostream & operator << (std::ostream & os, const CongruenceClosure & cc){
  return os;
}
