#include "Hornsat.h"

Hornsat::ClassListPos::ClassListPos(
    Literal * lit_pointer, CurryNode::SideOfEquation eq_side) :
  lit_pointer(lit_pointer), eq_side(eq_side)
{
}

Hornsat::ClassListPos::~ClassListPos(){
#if DEBUG_DESTRUCTORS
  std::cout << "Done ~ClassListPos with " << lit_pointer << (eq_side == CurryNode::LHS ? " LHS" : " RHS") << std::endl;
#endif
}

std::ostream & operator << (std::ostream & os, Hornsat::ClassListPos const & clp){
  return (os 
      << *(clp.lit_pointer) 
      << (clp.eq_side == CurryNode::LHS ? " LHS" : " RHS"));
}


