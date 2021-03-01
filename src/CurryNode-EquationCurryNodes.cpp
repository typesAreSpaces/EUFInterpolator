#include "CurryNode.h"

CurryNode::EquationCurryNodes::EquationCurryNodes(CurryNode & lhs, CurryNode & rhs) :
  lhs(lhs), rhs(rhs), kind_equation(lhs.isConstant() ? CONST_EQ : APPLY_EQ) 
{
}

CurryNode::EquationCurryNodes::EquationCurryNodes(CurryNode & lhs, CurryNode & rhs, KindEquation kind_equation) :
  lhs(lhs), rhs(rhs), kind_equation(kind_equation) 
{
}

std::ostream & operator << (std::ostream & os, CurryNode::EquationCurryNodes const & ecns){
  return os << ecns.lhs << " = " << ecns.rhs;
}

