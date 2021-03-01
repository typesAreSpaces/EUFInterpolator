#include "CurryNode.h"

CurryNode::PairEquationCurryNodes::PairEquationCurryNodes(
    CurryNode::EquationCurryNodes const & first, 
    CurryNode::EquationCurryNodes const & second) :
  first(first), second(second) 
{
}

std::ostream & operator << (std::ostream & os, 
    CurryNode::PairEquationCurryNodes const & pecns){
  return os << "(" << pecns.first << ", " << pecns.second << ")";
}


